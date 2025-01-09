/* ===========================================================================
** Copyright (C) 2024 Ezlo Innovation Inc
**
** Under EZLO AVAILABLE SOURCE LICENSE (EASL) AGREEMENT
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/
/**
 * @file    sensor_0063_other_MQ9_LPG_flameable_detector.c
 * @brief   perform some function on sensor_0063
 * @author  xx
 * @version 0.1
 * @date    xx
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <math.h>

#include "ezlopi_core_actions.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"
#include "ezlopi_core_processes.h"

#include "ezlopi_hal_adc.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "ezlopi_service_loop.h"

#include "sensor_0063_other_MQ9_LPG_flameable_detector.h"
#include "EZLOPI_USER_CONFIG.h"

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/
typedef struct s_mq9_value
{
    uint8_t status_flag : 3; // BIT2 = avg_volt_flag  ; BIT1 = loop_stop_flag  ; BIT0 = Calibration_complete_LPG_flameable
    uint8_t heating_count;
    uint8_t avg_vol_count; // counter for calculating avg_voltage.
    float calib_avg_volt;
    float _LPG_flameable_ppm;
    float MQ9_R0_constant;
} s_mq9_value_t;
/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/

static ezlopi_error_t __0063_prepare(void *arg);
static ezlopi_error_t __0063_init(l_ezlopi_item_t *item);
static ezlopi_error_t __0063_get_item(l_ezlopi_item_t *item, void *arg);
static ezlopi_error_t __0063_get_cjson_value(l_ezlopi_item_t *item, void *arg);
static ezlopi_error_t __0063_notify(l_ezlopi_item_t *item);

static float __extract_MQ9_sensor_ppm(l_ezlopi_item_t *item);
static void __calibrate_MQ9_R0_resistance(void *params);
static void __prepare_device_digi_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_item_digi_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device);
static void __prepare_device_adc_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_item_adc_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_data);

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
ezlopi_error_t SENSOR_0063_other_mq9_lpg_flameable_detector(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    ezlopi_error_t ret = EZPI_SUCCESS;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = __0063_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = __0063_init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    {
        ret = __0063_get_item(item, arg);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = __0063_get_cjson_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        ret = __0063_notify(item);
        break;
    }
    default:
    {
        break;
    }
    }
    return ret;
}

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/

static ezlopi_error_t __0063_prepare(void *arg)
{
    ezlopi_error_t ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (device_prep_arg && (NULL != device_prep_arg->cjson_device))
    {
        //---------------------------  DIGI - DEVICE 1 --------------------------------------------
        l_ezlopi_device_t *MQ9_device_parent_digi = EZPI_core_device_add_device(device_prep_arg->cjson_device, "digi");
        if (MQ9_device_parent_digi)
        {
            TRACE_I("Parent_MQ9_device_digi-[0x%x] ", MQ9_device_parent_digi->cloud_properties.device_id);
            __prepare_device_digi_cloud_properties(MQ9_device_parent_digi, device_prep_arg->cjson_device);

            l_ezlopi_item_t *MQ9_item_digi = EZPI_core_device_add_item_to_device(MQ9_device_parent_digi, SENSOR_0063_other_mq9_lpg_flameable_detector);
            if (MQ9_item_digi)
            {
                __prepare_item_digi_cloud_properties(MQ9_item_digi, device_prep_arg->cjson_device);
                ret = EZPI_SUCCESS;
            }

            //---------------------------- ADC - DEVICE 2 -------------------------------------------
            s_mq9_value_t *MQ9_value = (s_mq9_value_t *)ezlopi_malloc(__FUNCTION__, sizeof(s_mq9_value_t));
            if (NULL != MQ9_value)
            {
                memset(MQ9_value, 0, sizeof(s_mq9_value_t));
                l_ezlopi_device_t *MQ9_device_child_adc = EZPI_core_device_add_device(device_prep_arg->cjson_device, "adc");
                if (MQ9_device_child_adc)
                {
                    TRACE_I("Child_MQ9_device_child_adc-[0x%x] ", MQ9_device_child_adc->cloud_properties.device_id);
                    __prepare_device_adc_cloud_properties(MQ9_device_child_adc, device_prep_arg->cjson_device);

                    l_ezlopi_item_t *MQ9_item_adc = EZPI_core_device_add_item_to_device(MQ9_device_child_adc, SENSOR_0063_other_mq9_lpg_flameable_detector);
                    if (MQ9_item_adc)
                    {
                        __prepare_item_adc_cloud_properties(MQ9_item_adc, device_prep_arg->cjson_device, MQ9_value);
                        ret = EZPI_SUCCESS;
                    }
                    else
                    {
                        EZPI_core_device_free_device(MQ9_device_child_adc);
                        ezlopi_free(__FUNCTION__, MQ9_value);
                    }
                }
                else
                {
                    ezlopi_free(__FUNCTION__, MQ9_value);
                }
            }
        }
    }
    return ret;
}

static ezlopi_error_t __0063_init(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_ERR_INIT_DEVICE_FAILED;
    if (NULL != item)
    {
        if (ezlopi_item_name_gas_alarm == item->cloud_properties.item_name)
        {
            if (GPIO_IS_VALID_GPIO(item->interface.gpio.gpio_in.gpio_num))
            {
                // intialize digital_pin
                gpio_config_t input_conf = {};
                input_conf.pin_bit_mask = (1ULL << (item->interface.gpio.gpio_in.gpio_num));
                input_conf.intr_type = GPIO_INTR_DISABLE;
                input_conf.mode = GPIO_MODE_INPUT;
                input_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
                input_conf.pull_up_en = GPIO_PULLUP_ENABLE;
                ret = (0 == gpio_config(&input_conf)) ? EZPI_SUCCESS : EZPI_ERR_INIT_DEVICE_FAILED;
            }
        }
        else if (ezlopi_item_name_smoke_density == item->cloud_properties.item_name)
        {
            s_mq9_value_t *MQ9_value = (s_mq9_value_t *)item->user_arg;
            if (MQ9_value)
            {
                if (GPIO_IS_VALID_GPIO(item->interface.adc.gpio_num))
                { // initialize analog_pin
                    if (EZPI_SUCCESS == EZPI_hal_adc_init(item->interface.adc.gpio_num, item->interface.adc.resln_bit))
                    {                                             // calibrate if not done
                        if (0 == (BIT0 & MQ9_value->status_flag)) // Calibration_complete_LPG_flameable == 0
                        {
                            MQ9_value->heating_count = 20;
                            MQ9_value->avg_vol_count = MQ9_AVG_CAL_COUNT;
                            EZPI_service_loop_add("mq9_loop", __calibrate_MQ9_R0_resistance, 1000, (void *)item);
                            // #if defined(CONFIG_FREERTOS_USE_TRACE_FACILITY)
                            //                             EZPI_core_process_set_process_info(ENUM_EZLOPI_SENSOR_MQ9_TASK, &ezlopi_sensor_mq9_task_handle, EZLOPI_SENSOR_MQ9_TASK_DEPTH);
                            // #endif
                            ret = EZPI_SUCCESS;
                        }
                    }
                }
            }
        }
    }
    return ret;
}

static void __prepare_device_digi_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    device->cloud_properties.category = category_security_sensor;
    device->cloud_properties.subcategory = subcategory_gas;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_item_digi_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_gas_alarm;
    item->cloud_properties.value_type = value_type_token;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.item_id = EZPI_core_cloud_generate_item_id();

    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_dev_type_str, item->interface_type); // _max = 10
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio1_str, item->interface.gpio.gpio_in.gpio_num);
    TRACE_S("MQ9-> DIGITAL_PIN: %d ", item->interface.gpio.gpio_in.gpio_num);
}

static void __prepare_device_adc_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    device->cloud_properties.category = category_level_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_item_adc_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_data)
{
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_smoke_density;
    item->cloud_properties.value_type = value_type_substance_amount;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = scales_parts_per_million;
    item->cloud_properties.item_id = EZPI_core_cloud_generate_item_id();

    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_dev_type_str, item->interface_type); // _max = 10
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio2_str, item->interface.adc.gpio_num);
    TRACE_S("MQ9-> ADC_PIN: %d ", item->interface.adc.gpio_num);
    item->interface.adc.resln_bit = 3; // ADC 12_bit

    // passing the custom data_structure
    item->is_user_arg_unique = true;
    item->user_arg = user_data; // since 'item->user_arg' exist in only one-child [item_adc]
}

static ezlopi_error_t __0063_get_item(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        if (cj_result)
        {
            if (ezlopi_item_name_gas_alarm == item->cloud_properties.item_name)
            {
                //-------------------  POSSIBLE JSON ENUM LPG_flameableNTENTS ----------------------------------
                cJSON *json_array_enum = cJSON_CreateArray(__FUNCTION__);
                if (NULL != json_array_enum)
                {
                    char *mq9_sensor_gas_alarm_token[] = {
                        "no_gas",
                        "combustible_gas_detected",
                        "toxic_gas_detected",
                        "unknown",
                    };
                    for (uint8_t i = 0; i < MQ9_GAS_ALARM_MAX; i++)
                    {
                        cJSON *json_value = cJSON_CreateString(__FUNCTION__, mq9_sensor_gas_alarm_token[i]);
                        if (NULL != json_value)
                        {
                            cJSON_AddItemToArray(json_array_enum, json_value);
                        }
                    }
                    cJSON_AddItemToObject(__FUNCTION__, cj_result, ezlopi_enum_str, json_array_enum);
                }
                //--------------------------------------------------------------------------------------
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_valueFormatted_str, (char *)item->user_arg ? item->user_arg : "no_gas");
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_value_str, (char *)item->user_arg ? item->user_arg : "no_gas");
            }
            else if (ezlopi_item_name_smoke_density == item->cloud_properties.item_name)
            {
                s_mq9_value_t *MQ9_value = ((s_mq9_value_t *)item->user_arg);
                if (MQ9_value)
                {
                    EZPI_core_valueformatter_float_to_cjson(cj_result, MQ9_value->_LPG_flameable_ppm, item->cloud_properties.scale);
                }
            }
            ret = EZPI_SUCCESS;
        }
    }
    return ret;
}

static ezlopi_error_t __0063_get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        if (cj_result)
        {
            if (ezlopi_item_name_gas_alarm == item->cloud_properties.item_name)
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_valueFormatted_str, (char *)item->user_arg ? item->user_arg : "no_gas");
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_value_str, (char *)item->user_arg ? item->user_arg : "no_gas");
            }
            else if (ezlopi_item_name_smoke_density == item->cloud_properties.item_name)
            {
                s_mq9_value_t *MQ9_value = ((s_mq9_value_t *)item->user_arg);
                if (MQ9_value)
                {
                    EZPI_core_valueformatter_float_to_cjson(cj_result, MQ9_value->_LPG_flameable_ppm, item->cloud_properties.scale);
                }
            }
            ret = EZPI_SUCCESS;
        }
    }
    return ret;
}

static ezlopi_error_t __0063_notify(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item)
    {
        if (ezlopi_item_name_gas_alarm == item->cloud_properties.item_name)
        {
            const char *curret_value = NULL;
            if (0 == gpio_get_level(item->interface.gpio.gpio_in.gpio_num)) // when D0 -> 0V,
            {
                curret_value = "combustible_gas_detected";
            }
            else
            {
                curret_value = "no_gas";
            }
            if (curret_value != (char *)item->user_arg) // calls update only if there is change in state
            {
                item->user_arg = (void *)curret_value;
                EZPI_core_device_value_updated_from_device_broadcast(item);
            }
        }
        else if (ezlopi_item_name_smoke_density == item->cloud_properties.item_name)
        {
            // extract the sensor_output_values
            s_mq9_value_t *MQ9_value = (s_mq9_value_t *)item->user_arg;
            if ((MQ9_value) && (BIT0 == (BIT0 & MQ9_value->status_flag))) // calibration_complete == 1
            {
                if (BIT1 == (BIT1 & MQ9_value->status_flag)) // loop_stop_flag == 1
                {
                    MQ9_value->status_flag ^= BIT1; // toggle BIT1 // loop_stop_flag => 0
                    // TRACE_D(" MQ9_value->status_flag : %03x", MQ9_value->status_flag);
                    EZPI_service_loop_remove(__calibrate_MQ9_R0_resistance);
                    TRACE_S("......Removed :- MQ9_calib_loop");
                }
                else
                {
                    double new_value = (double)__extract_MQ9_sensor_ppm(item);
                    if (fabs((double)(MQ9_value->_LPG_flameable_ppm) - new_value) > 0.0001)
                    {
                        MQ9_value->_LPG_flameable_ppm = (float)new_value;
                        EZPI_core_device_value_updated_from_device_broadcast(item);
                    }
                }
            }
        }
        ret = EZPI_SUCCESS;
    }
    return ret;
}

static float __extract_MQ9_sensor_ppm(l_ezlopi_item_t *item)
{
    s_mq9_value_t *MQ9_value = (s_mq9_value_t *)item->user_arg;
    if (MQ9_value)
    { // calculation process
        //-------------------------------------------------
        int mq9_adc_pin = item->interface.adc.gpio_num;
        s_ezlopi_analog_data_t ezlopi_analog_data = {.value = 0, .voltage = 0};
        // extract the mean_sensor_analog_output_voltage
        float analog_sensor_volt = 0;
        for (uint8_t x = 10; x > 0; x--)
        {
            EZPI_hal_adc_get_adc_data(mq9_adc_pin, &ezlopi_analog_data);
#ifdef VOLTAGE_DIVIDER_ADDED
            analog_sensor_volt += ((float)(ezlopi_analog_data.voltage) * 2.0f);
#else
            analog_sensor_volt += (float)(ezlopi_analog_data.voltage);
#endif
            vTaskDelay(10 / portTICK_PERIOD_MS); // 10ms
        }
        analog_sensor_volt = analog_sensor_volt / 10.0f;

        //-----------------------------------------------------------------------------------
        // Stage_2 : [from 'sensor_0063_ADC_MQ9_methane_gas_detector.h']

        // 1. Calculate 'Rs_gas' for the gas detected
        float Rs_gas = (((MQ9_VOLT_RESOLUTION_Vc * mq9_eqv_RL) / (analog_sensor_volt / 1000.0f)) - mq9_eqv_RL);

        // 1.1 Calculate @ 'ratio' during LPG_flameable presence
        double _ratio = (Rs_gas / ((MQ9_value->MQ9_R0_constant <= 0) ? (1.0f) : (MQ9_value->MQ9_R0_constant))); // avoid dividing by zero??
        if (_ratio <= 0)
        {
            _ratio = 0;
        }
        //-------------------------------------------------

        // 1.2 Calculate _LPG_flameable_ppm
        float _LPG_flameable_ppm = (float)pow(10, (((float)log10(_ratio)) - b_coeff_mq9) / m_slope_mq9); // ---> _LPG_flameable_ppm = 10 ^ [ ( log(ratio) - b ) / m ]
        if (_LPG_flameable_ppm < 0)
        {
            _LPG_flameable_ppm = 0; // No negative values accepted or upper datasheet recomendation.
        }

        TRACE_E("_LPG_flameable_ppm [LPG_flameable] : %.2f -> ratio[RS/R0] : %.2f -> Volts : %0.2fmv", _LPG_flameable_ppm, (float)_ratio, analog_sensor_volt);
        return _LPG_flameable_ppm;
        //-------------------------------------------------
    }
    return 0;
}

static void __calibrate_MQ9_R0_resistance(void *params)
{
    l_ezlopi_item_t *item = (l_ezlopi_item_t *)params;
    if (NULL != item)
    {
        s_mq9_value_t *MQ9_value = (s_mq9_value_t *)item->user_arg;
        if (MQ9_value && (0 == (BIT1 & MQ9_value->status_flag))) // loop_stop_flag == 0
        {
            int mq9_adc_pin = item->interface.adc.gpio_num;
            //-------------------------------------------------
            // let the sensor to heat for 20seconds
            if (MQ9_value->heating_count > 0)
            {
                // if (0 == MQ9_value->heating_count % 20)
                // {
                //     TRACE_E("Heating sensor.........time left: %d sec", MQ9_value->heating_count / 10);
                // }
                MQ9_value->heating_count--;
            }
            else // after heating the sensor for 20 sec
            {
                //-------------------------------------------------
                // extract the mean_sensor_analog_output_voltage
                if (MQ9_value->avg_vol_count != 0)
                {
                    s_ezlopi_analog_data_t ezlopi_analog_data = {.value = 0, .voltage = 0};
                    // extract ADC values
                    EZPI_hal_adc_get_adc_data(mq9_adc_pin, &ezlopi_analog_data);
#ifdef VOLTAGE_DIVIDER_ADDED
                    MQ9_value->calib_avg_volt += (float)((ezlopi_analog_data.voltage) * 2.0f); // [0-2.4V] X2
#else
                    MQ9_value->calib_avg_volt += (float)(ezlopi_analog_data.voltage);
#endif
                    // TRACE_D(" _count : %d", MQ9_value->avg_vol_count);
                    MQ9_value->avg_vol_count--;

                    if (0 == MQ9_value->avg_vol_count)
                    {
                        MQ9_value->status_flag |= BIT2;
                    }
                }

                if (BIT2 == (MQ9_value->status_flag & BIT2))
                {
                    MQ9_value->status_flag ^= BIT2; // avg_volt_flag => 0
                    MQ9_value->calib_avg_volt /= MQ9_AVG_CAL_COUNT;
                    //-------------------------------------------------
                    // Calculate the 'Rs' of heater during clean air [calibration phase]
                    // Range -> [2Kohm - 20Kohm]
                    float RS_calib = 0;                                                                                      // Define variable for sensor resistance
                    RS_calib = ((MQ9_VOLT_RESOLUTION_Vc * mq9_eqv_RL) / (MQ9_value->calib_avg_volt / 1000.0f)) - mq9_eqv_RL; // Calculate RS in fresh air
                    TRACE_E("CALIB_TASK -> 'RS_calib' = %.2f", RS_calib);
                    if (RS_calib < 0)
                    {
                        RS_calib = 0; // No negative values accepted.
                    }
                    // Calculate the R0_air which is constant through-out
                    MQ9_value->MQ9_R0_constant = (RS_calib / RatioMQ9CleanAir); // Calculate MQ9_R0_constant
                    TRACE_E("CALIB_TASK -> 'MQ9_R0_constant' = %.2f", (MQ9_value->MQ9_R0_constant));
                    if ((MQ9_value->MQ9_R0_constant) < 0)
                    {
                        (MQ9_value->MQ9_R0_constant) = 0; // No negative values accepted.
                    }
                    // loop_stop_flag => 1 // Calibration_complete_LPG_flameable => 1;
                    MQ9_value->status_flag |= (BIT0 | BIT1);
                }
            }
        }
    }
    // #if defined(CONFIG_FREERTOS_USE_TRACE_FACILITY)
    //     EZPI_core_process_set_is_deleted(ENUM_EZLOPI_SENSOR_MQ9_TASK);
    // #endif
}

/*******************************************************************************
 *                          End of File
 *******************************************************************************/