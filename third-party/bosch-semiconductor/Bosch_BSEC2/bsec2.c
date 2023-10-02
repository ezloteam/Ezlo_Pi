/**
 * Copyright (c) 2021 Bosch Sensortec GmbH. All rights reserved.
 *
 * BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @file	bsec2.cpp
 * @date	17 January 2023
 * @version	2.0.6
 *
 */

#include "bsec2.h"

/* Stores the version of the BSEC algorithm */
static bsec_version_t version;
static bsec_library_return_t status;

bsec_bme_settings_t bme_conf;

bsec_callback new_data_callback;

bsec_outputs outputs;
/* operating mode of sensor */
uint8_t op_mode;

float ext_temp_offset;
/** Global variables to help create a millisecond timestamp that doesn't overflow every 51 days.
 * If it overflows, it will have a negative value. Something that should never happen.
 */
uint32_t ovf_counter;

uint32_t last_millis;
/* Pointer to hold the address of the instance */
uint8_t *bsec_instance;

static uint8_t work_buffer[BSEC_MAX_WORKBUFFER_SIZE];

/**
 * @brief Constructor of Bsec2 class
 */
void bsec2_setup(s_ezlopi_i2c_master_t* bme68x_i2c_master_conf)
{
    bme68xlib_setup(bme68x_i2c_master_conf);

    ovf_counter = 0;
    last_millis = 0;
    status = BSEC_OK;
    ext_temp_offset = 0.0f;
    op_mode = BME68X_SLEEP_MODE;
    new_data_callback = NULL;
    bsec_instance = NULL;

    memset(&version, 0, sizeof(version));
    memset(&bme_conf, 0, sizeof(bme_conf));
    memset(&outputs, 0, sizeof(outputs));
}

/**
 * @brief Function to initialize the sensor based on custom callbacks
 */
bool bsec2_begin_custom(bme68x_intf intf, bme68x_read_fptr_t read, bme68x_write_fptr_t write,
        bme68x_delay_us_fptr_t idle_task, void *intf_ptr)
{
    bme68xlib_begin_custom(intf, read, write, idle_task, intf_ptr);

    if (bme68xlib_check_status() == BME68X_ERROR)
        return false;

    return bsec2_begin_common();
}

/**
 * @brief Function to initialize the sensor based on the Wire library
 */ 
bool bsec2_begin(void)
{
    bme68xlib_begin();

    if (bme68xlib_check_status() == BME68X_ERROR)
        return false;

    return bsec2_begin_common();
}

/**
 * @brief Function to request/subscribe for desired virtual outputs with the supported sample rates
 */
bool bsec2_update_subscription(bsec_sensor sensor_list[], uint8_t n_sensors, float sample_rate)
{
    bsec_sensor_configuration_t virtualSensors[BSEC_NUMBER_OUTPUTS], sensor_settings[BSEC_MAX_PHYSICAL_SENSOR];
    uint8_t n_sensor_settings = BSEC_MAX_PHYSICAL_SENSOR;

    for (uint8_t i = 0; i < n_sensors; i++)
    {
        virtualSensors[i].sensor_id = sensor_list[i];
        virtualSensors[i].sample_rate = sample_rate;
    }

    /* Subscribe to library virtual sensors outputs */
    status = bsec_update_subscription_m(bsec_instance, virtualSensors, n_sensors, sensor_settings, &n_sensor_settings);
    if (status != BSEC_OK)
        return false;

    return true;
}

/**
 * @brief Callback from the user to read data from the BME68X using parallel mode/forced mode, process and store outputs
 */
bool bsec2_run(void)
{
    uint8_t n_fields_left = 0;
    bme68x_data data;
    int64_t curr_time_ns = bsec2_get_time_ms() * INT64_C(1000000);
    op_mode = bme_conf.op_mode;

    if (curr_time_ns >= bme_conf.next_call)
    {
        /* Provides the information about the current sensor configuration that is
           necessary to fulfill the input requirements, eg: operation mode, timestamp
           at which the sensor data shall be fetched etc */
        // printf("curr_time_ns %lld\n", curr_time_ns);
        status = bsec_sensor_control_m(bsec_instance ,curr_time_ns, &bme_conf);
        // printf("Status is %d\n", status);
        if (status != BSEC_OK)
            return false;

        switch (bme_conf.op_mode)
        {
        case BME68X_FORCED_MODE:
            bsec2_set_bme68x_config_forced();
            break;
        case BME68X_PARALLEL_MODE:
            if (op_mode != bme_conf.op_mode)
            {
                bsec2_set_bme68x_config_parallel();
            }
            break;

        case BME68X_SLEEP_MODE:
            if (op_mode != bme_conf.op_mode)
            {
                bme68xlib_set_op_mode(BME68X_SLEEP_MODE);
                op_mode = BME68X_SLEEP_MODE;
            }
            break;
        }

        if (bme68xlib_check_status() == BME68X_ERROR)
            return false;

        if (bme_conf.trigger_measurement && bme_conf.op_mode != BME68X_SLEEP_MODE)
        {
            if (bme68xlib_fetch_data())
            {
                do
                {
                    n_fields_left = bme68xlib_get_data(&data);
                    /* check for valid gas data */
                    if (data.status & BME68X_GASM_VALID_MSK)
                    {
                        if (!bsec2_process_data(curr_time_ns, &data))
                        {
                            return false;
                        }
                    }
                } while (n_fields_left);
            }

        }

    }
    return true;
}

/**
 * @brief Function to get the state of the algorithm to save to non-volatile memory
 */
bool bsec2_get_state(uint8_t *state)
{
    uint32_t n_serialized_state = BSEC_MAX_STATE_BLOB_SIZE;

    status = bsec_get_state_m(bsec_instance, 0, state, BSEC_MAX_STATE_BLOB_SIZE, work_buffer, BSEC_MAX_WORKBUFFER_SIZE,
            &n_serialized_state);
    if (status != BSEC_OK)
        return false;
    return true;
}

/**
 * @brief Function to set the state of the algorithm from non-volatile memory
 */
bool bsec2_set_state(uint8_t *state)
{
    status = bsec_set_state_m(bsec_instance, state, BSEC_MAX_STATE_BLOB_SIZE, work_buffer, BSEC_MAX_WORKBUFFER_SIZE);
    if (status != BSEC_OK)
        return false;

    memset(&bme_conf, 0, sizeof(bme_conf));

    return true;
}

/**
 * @brief Function to retrieve the current library configuration
 */
bool bsec2_get_config(uint8_t *config)
{
    uint32_t n_serialized_settings = 0;
    
    status = bsec_get_configuration_m(bsec_instance, 0, config, BSEC_MAX_PROPERTY_BLOB_SIZE, work_buffer, BSEC_MAX_WORKBUFFER_SIZE, &n_serialized_settings);
    if (status != BSEC_OK)
        return false;

    return true;
}

/**
 * @brief Function to set the configuration of the algorithm from memory
 */
bool bsec2_set_config(const uint8_t *config)
{
    status = bsec_set_configuration_m(bsec_instance, config, BSEC_MAX_PROPERTY_BLOB_SIZE, work_buffer, BSEC_MAX_WORKBUFFER_SIZE);
    if (status != BSEC_OK)
        return false;

    memset(&bme_conf, 0, sizeof(bme_conf));

    return true;
}

/**
 * @brief Function to calculate an int64_t timestamp in milliseconds
 */
int64_t bsec2_get_time_ms(void)
{
    int64_t timeMs = millis();
    // printf("last_millis is %d and timeMS is %lld\n", last_millis, timeMs);

    if (last_millis > timeMs) /* An overflow occurred */
    { 
        ovf_counter++;
    }

    last_millis = timeMs;

    return timeMs + (ovf_counter * INT64_C(0xFFFFFFFF));
}

/**
 * @brief Function to assign the memory block to the bsec instance
 */
void bsec2_allocate_memory(uint8_t *mem_block)
{
    /* allocating memory for the bsec instance */
    bsec_instance = mem_block;
}

/**
 * @brief Function to de-allocate the dynamically allocated memory
 */
void bsec2_clear_memory(void)
{
    free(bsec_instance);
    bsec_instance = NULL;
}

/* Private functions */

/**
 * @brief Reads data from the BME68X sensor and process it
 */
bool bsec2_process_data(int64_t curr_time_ns, const bme68x_data* data)
{
    bsec_input_t inputs[BSEC_MAX_PHYSICAL_SENSOR]; /* Temp, Pres, Hum & Gas */
    uint8_t n_inputs = 0;
    /* Checks all the required sensor inputs, required for the BSEC library for the requested outputs */
    if (BSEC_CHECK_INPUT(bme_conf.process_data, BSEC_INPUT_TEMPERATURE))
    {
        inputs[n_inputs].sensor_id = BSEC_INPUT_HEATSOURCE;
        inputs[n_inputs].signal = ext_temp_offset;
        inputs[n_inputs].time_stamp = curr_time_ns;
        n_inputs++;
#ifdef BME68X_USE_FPU
        inputs[n_inputs].signal = data->temperature;
#else
        inputs[n_inputs].signal = data->temperature / 100.0f;
#endif
        inputs[n_inputs].sensor_id = BSEC_INPUT_TEMPERATURE;
        inputs[n_inputs].time_stamp = curr_time_ns;
        n_inputs++;
    }
    if (BSEC_CHECK_INPUT(bme_conf.process_data, BSEC_INPUT_HUMIDITY))
    {
#ifdef BME68X_USE_FPU
        inputs[n_inputs].signal = data->humidity;
#else
        inputs[n_inputs].signal = data->humidity / 1000.0f;
#endif
        inputs[n_inputs].sensor_id = BSEC_INPUT_HUMIDITY;
        inputs[n_inputs].time_stamp = curr_time_ns;
        n_inputs++;
    }
    if (BSEC_CHECK_INPUT(bme_conf.process_data, BSEC_INPUT_PRESSURE))
    {
        inputs[n_inputs].sensor_id = BSEC_INPUT_PRESSURE;
        inputs[n_inputs].signal = data->pressure;
        inputs[n_inputs].time_stamp = curr_time_ns;
        n_inputs++;
    }
    if (BSEC_CHECK_INPUT(bme_conf.process_data, BSEC_INPUT_GASRESISTOR) &&
            (data->status & BME68X_GASM_VALID_MSK))
    {
        inputs[n_inputs].sensor_id = BSEC_INPUT_GASRESISTOR;
        inputs[n_inputs].signal = data->gas_resistance;
        inputs[n_inputs].time_stamp = curr_time_ns;
        n_inputs++;
    }
    if (BSEC_CHECK_INPUT(bme_conf.process_data, BSEC_INPUT_PROFILE_PART) &&
            (data->status & BME68X_GASM_VALID_MSK))
    {
        inputs[n_inputs].sensor_id = BSEC_INPUT_PROFILE_PART;
        inputs[n_inputs].signal = (op_mode == BME68X_FORCED_MODE) ? 0 : data->gas_index;
        inputs[n_inputs].time_stamp = curr_time_ns;
        n_inputs++;
    }

    if (n_inputs > 0)
    {

        outputs.n_outputs = BSEC_NUMBER_OUTPUTS;
        memset(outputs.output, 0, sizeof(outputs.output));

        /* Processing of the input signals and returning of output samples is performed by bsec_do_steps() */
        status = bsec_do_steps_m(bsec_instance, inputs, n_inputs, outputs.output, &outputs.n_outputs);

        if (status != BSEC_OK)
            return false;

        if(new_data_callback)
            new_data_callback(*data, outputs);
    }
    return true;
}

/**
 * @brief Common code for the begin function
 */
bool bsec2_begin_common()
{
    if (!bsec_instance)
    {
        /* allocate memory for the instance if not allocated */
        bsec_instance = (uint8_t*) malloc( sizeof(uint8_t) * bsec_get_instance_size_m() );
    }

    if (BSEC_INSTANCE_SIZE < bsec_get_instance_size_m())
    {
        status = BSEC_E_INSUFFICIENT_INSTANCE_SIZE;
        return false;
    }
    status = bsec_init_m(bsec_instance);
    if (status != BSEC_OK)
        return false;

    status = bsec_get_version_m(bsec_instance, &version);
    if (status != BSEC_OK)
        return false;

    memset(&bme_conf, 0, sizeof(bme_conf));
    memset(&outputs, 0, sizeof(outputs));

    return true;
}

/**
 * @brief Set the BME68X sensor configuration to forced mode
 */
void bsec2_set_bme68x_config_forced(void)
{
    /* Set the filter, odr, temperature, pressure and humidity settings */
    bme68xlib_set_tph(bme_conf.temperature_oversampling, bme_conf.pressure_oversampling, bme_conf.humidity_oversampling);

    if (bme68xlib_check_status() == BME68X_ERROR)
        return;

    bme68xlib_set_heater_prof_p2(bme_conf.heater_temperature, bme_conf.heater_duration);

    if (bme68xlib_check_status() == BME68X_ERROR)
        return;

    bme68xlib_set_op_mode(BME68X_FORCED_MODE);
    if (bme68xlib_check_status() == BME68X_ERROR)
        return;

    op_mode = BME68X_FORCED_MODE;
}

/**
 * @brief Set the BME68X sensor configuration to parallel mode
 */
void bsec2_set_bme68x_config_parallel(void)
{
    uint16_t sharedHeaterDur = 0;

    /* Set the filter, odr, temperature, pressure and humidity settings */
    bme68xlib_set_tph(bme_conf.temperature_oversampling, bme_conf.pressure_oversampling, bme_conf.humidity_oversampling);

    if (bme68xlib_check_status() == BME68X_ERROR)
        return;

    sharedHeaterDur = BSEC_TOTAL_HEAT_DUR - (bme68xlib_get_meas_dur(BME68X_PARALLEL_MODE) / INT64_C(1000));

    bme68xlib_set_heater_prof_p4(bme_conf.heater_temperature_profile, bme_conf.heater_duration_profile, sharedHeaterDur,
            bme_conf.heater_profile_len);

    if (bme68xlib_check_status() == BME68X_ERROR)
        return;

    bme68xlib_set_op_mode(BME68X_PARALLEL_MODE);

    if (bme68xlib_check_status() == BME68X_ERROR)
        return;

    op_mode = BME68X_PARALLEL_MODE;
}

int8_t bsec2_get_sensor_status(void)
{
    return bme68xlib_get_status();
}

bsec_library_return_t bsec2_get_status(void)
{
    return status;
}

bsec_version_t bsec2_get_version(void)
{
    return version;
}

/**
 * @brief Callback from the user to read data from the BME68x using parallel/forced mode, process and store outputs
 * @return	true for success, false otherwise
 */
void bsec2_attach_callback(bsec_callback callback)
{
    new_data_callback = callback;
}

/**
 * @brief Function to get the BSEC outputs
 * @return	pointer to BSEC outputs if available else nullptr
 */
const bsec_outputs* bsec2_get_outputs(void)
{
    if (outputs.n_outputs)
        return &outputs;
    return NULL;
}

/**
 * @brief Function to get the BSEC output by sensor id
 * @return	pointer to BSEC output, nullptr otherwise
 */
bsec_data bsec2_get_data(bsec_sensor id)
{
    bsec_data emp;
    memset(&emp, 0, sizeof(emp));
    for (uint8_t i = 0; i < outputs.n_outputs; i++)
        if (id == outputs.output[i].sensor_id)
            return outputs.output[i];
    return emp;
}

/**
 * @brief Function to set the temperature offset
 * @param temp_offset	: Temperature offset in degree Celsius
 */
void bsec2_set_temperature_offset(float temp_offset)
{
    ext_temp_offset = temp_offset;
}
