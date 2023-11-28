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
 * @file	bsec2.h
 * @date	17 January 2023
 * @version	2.0.6
 *
 */

#ifndef BSEC2_H_
#define BSEC2_H_

/* dependent library header */
#include "bme68xLibrary.h"
#include "bsec_datatypes.h"
#include "bsec_interface_multi.h"

#ifndef ARRAY_LEN
#define ARRAY_LEN(array)				(sizeof(array)/sizeof(array[0]))
#endif

#define BSEC_CHECK_INPUT(x, shift)		    (x & (1 << (shift-1)))
#define BSEC_TOTAL_HEAT_DUR                 UINT16_C(140)
#define BSEC_INSTANCE_SIZE                  3272
#define BSEC_E_INSUFFICIENT_INSTANCE_SIZE   (bsec_library_return_t)-105

#define millis() (esp_timer_get_time()/1000)

typedef bsec_output_t bsec_data;
typedef bsec_virtual_sensor_t bsec_sensor;

typedef struct
{
    bsec_data output[BSEC_NUMBER_OUTPUTS];
    uint8_t n_outputs;
} bsec_outputs;

typedef void (*bsec_callback)(const bme68x_data data, const bsec_outputs outputs);

#ifdef __cplusplus
extern "C" {
#endif

void bsec2_setup(s_ezlopi_i2c_master_t* bme68x_i2c_master_conf);

/**
 * @brief Function to initialize the sensor based on custom callbacks
 * @param intf     : BME68X_SPI_INTF or BME68X_I2C_INTF interface
 * @param read     : Read callback
 * @param write    : Write callback
 * @param idle_task : Delay or Idle function
 * @param intf_ptr : Pointer to the interface descriptor
 * @return True if everything initialized correctly
 */
bool bsec2_begin_custom(bme68x_intf intf, bme68x_read_fptr_t read, bme68x_write_fptr_t write,
        bme68x_delay_us_fptr_t idle_task, void *intf_ptr);

/**
 * @brief Function to initialize the sensor
 * @return True if everything initialized correctly
 */
bool bsec2_begin(void);

/**
 * @brief Function that sets the desired sensors and the sample rates
 * @param sensor_list	: The list of output sensors
 * @param n_sensors		: Number of outputs requested
 * @param sample_rate	: The sample rate of requested sensors
 * @return	true for success, false otherwise
 */
bool bsec2_update_subscription(bsec_sensor sensor_list[], uint8_t n_sensors, float sample_rate); // float sample_rate = BSEC_SAMPLE_RATE_ULP

bool bsec2_run(void);

/**
 * @brief Callback from the user to read data from the BME68x using parallel/forced mode, process and store outputs
 * @return	true for success, false otherwise
 */
void bsec2_attach_callback(bsec_callback callback);

/**
 * @brief Function to get the BSEC outputs
 * @return	pointer to BSEC outputs if available else nullptr
 */
const bsec_outputs* bsec2_get_outputs(void);

/**
 * @brief Function to get the BSEC output by sensor id
 * @return	pointer to BSEC output, nullptr otherwise
 */
bsec_data bsec2_get_data(bsec_sensor id);

/**
 * @brief Function to set the temperature offset
 * @param temp_offset	: Temperature offset in degree Celsius
 */
void bsec2_set_temperature_offset(float temp_offset);

/**
 * @brief Function to get the state of the algorithm to save to non-volatile memory
 * @param state			: Pointer to a memory location, to hold the state
 * @return	true for success, false otherwise
 */
bool bsec2_get_state(uint8_t *state);

/**
 * @brief Function to set the state of the algorithm from non-volatile memory
 * @param state			: Pointer to a memory location that contains the state
 * @return	true for success, false otherwise
 */
bool bsec2_set_state(uint8_t *state);

/** 
 * @brief Function to retrieve the current library configuration
 * @param config    : Pointer to a memory location, to hold the serialized config blob
 * @return	true for success, false otherwise
 */
bool bsec2_get_config(uint8_t *config);

/**
 * @brief Function to set the configuration of the algorithm from memory
 * @param state			: Pointer to a memory location that contains the configuration
 * @return	true for success, false otherwise
 */
bool bsec2_set_config(const uint8_t *config);

/**
 * @brief Function to calculate an int64_t timestamp in milliseconds
 */
int64_t bsec2_get_time_ms(void);

/**
 * @brief Function to assign the memory block to the bsec instance
 * 
 * @param[in] mem_block : reference to the memory block
 */
void bsec2_allocate_memory(uint8_t *mem_block); // &mem_block)[BSEC_INSTANCE_SIZE]

/**
 * @brief Function to de-allocate the dynamically allocated memory
 */
void bsec2_clear_memory(void);

int8_t bsec2_get_sensor_status(void);

bool bsec2_begin_common();
bool bsec2_process_data(int64_t currTimeNs, const bme68x_data* data);
void bsec2_set_bme68x_config_parallel(void);
void bsec2_set_bme68x_config_forced(void);

bsec_library_return_t bsec2_get_status(void);
bsec_version_t bsec2_get_version(void);

#ifdef __cplusplus
}
#endif

#endif /* BSEC2_CLASS_H */
