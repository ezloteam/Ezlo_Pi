/* ===========================================================================
** Copyright (C) 2025 Ezlo Innovation Inc
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
 * @file    sensor_0048_other_MQ4_CH4_detector.c
 * @brief   perform some function on sensor_0048
 * @author
 * @version 0.1
 * @date    xx
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <math.h>

#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"
#include "ezlopi_core_processes.h"

#include "ezlopi_hal_adc.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "ezlopi_service_loop.h"

#include "sensor_0048_other_MQ4_CH4_detector.h"
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
typedef struct s_mq4_value
{
    uint8_t status_flag : 3; // BIT2 = avg_volt_flag  ; BIT1 = loop_stop_flag  ; BIT0 = Calibration_complete_CH4
    uint8_t heating_count;
    uint8_t avg_vol_count; // counter for calculating avg_voltage.
    float calib_avg_volt;
    float _CH4_ppm;
    float MQ4_R0_constant;
} s_mq4_value_t;

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
static ezlopi_error_t __0048_prepare(void *arg);
static ezlopi_error_t __0048_init(l_ezlopi_item_t *item);
static ezlopi_error_t __0048_get_item(l_ezlopi_item_t *item, void *arg);
static ezlopi_error_t __0048_get_cjson_value(l_ezlopi_item_t *item, void *arg);
static ezlopi_error_t __0048_notify(l_ezlopi_item_t *item);

static void __calibrate_MQ4_R0_resistance(void *params);
static float __extract_MQ4_sensor_ppm(l_ezlopi_item_t *item);
static void __prepare_item_digi_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device);
static void __prepare_device_adc_cloud_properties_child_adc(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_device_digi_cloud_properties_parent_digi(l_ezlopi_device_t *device, cJSON *cj_device);
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
ezlopi_error_t
SENSOR_0048_other_mq4_ch4_detector(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    ezlopi_error_t ret = EZPI_SUCCESS;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = __0048_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = __0048_init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    {
        ret = __0048_get_item(item, arg);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = __0048_get_cjson_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        ret = __0048_notify(item);
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
static ezlopi_error_t __0048_prepare(void *arg)
{
    ezlopi_error_t ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (device_prep_arg && (NULL != device_prep_arg->cjson_device))
    {
        //---------------------------  DIGI - DEVICE 1 --------------------------------------------
        l_ezlopi_device_t *MQ4_device_parent_digi = EZPI_core_device_add_device(device_prep_arg->cjson_device, "digi");
        if (MQ4_device_parent_digi)
        {
            TRACE_I("Parent_MQ4_device_digi-[0x%x] ", MQ4_device_parent_digi->cloud_properties.device_id);
            __prepare_device_digi_cloud_properties_parent_digi(MQ4_device_parent_digi, device_prep_arg->cjson_device);
            l_ezlopi_item_t *MQ4_item_digi = EZPI_core_device_add_item_to_device(MQ4_device_parent_digi, SENSOR_0048_other_mq4_ch4_detector);
            if (MQ4_item_digi)
            {
                __prepare_item_digi_cloud_properties(MQ4_item_digi, device_prep_arg->cjson_device);
                ret = EZPI_SUCCESS;
            }

            //---------------------------- ADC - DEVICE 2 -------------------------------------------
            s_mq4_value_t *MQ4_value = (s_mq4_value_t *)ezlopi_malloc(__FUNCTION__, sizeof(s_mq4_value_t));
            if (NULL != MQ4_value)
            {
                memset(MQ4_value, 0, sizeof(s_mq4_value_t));
                l_ezlopi_device_t *MQ4_device_child_adc = EZPI_core_device_add_device(device_prep_arg->cjson_device, "adc");
                if (MQ4_device_child_adc)
                {
                    TRACE_I("Child_MQ4_device_adc-[0x%x] ", MQ4_device_child_adc->cloud_properties.device_id);
                    __prepare_device_adc_cloud_properties_child_adc(MQ4_device_child_adc, device_prep_arg->cjson_device);

                    l_ezlopi_item_t *MQ4_item_adc = EZPI_core_device_add_item_to_device(MQ4_device_child_adc, SENSOR_0048_other_mq4_ch4_detector);
                    if (MQ4_item_adc)
                    {
                        __prepare_item_adc_cloud_properties(MQ4_item_adc, device_prep_arg->cjson_device, MQ4_value);
                        ret = EZPI_SUCCESS;
                    }
                    else
                    {
                        EZPI_core_device_free_device(MQ4_device_child_adc);
                        ezlopi_free(__FUNCTION__, MQ4_value);
                    }
                }
                else
                {
                    ezlopi_free(__FUNCTION__, MQ4_value);
                }
            }
        }
    }
    return ret;
}
static ezlopi_error_t __0048_init(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_ERR_INIT_DEVICE_FAILED;
    if (NULL != item)
    {
        if ((ezlopi_item_name_gas_alarm == item->cloud_properties.item_name))
        {
            if (GPIO_IS_VALID_GPIO(item->interface.gpio.gpio_in.gpio_num))
            { // intialize digital_pin
                gpio_config_t input_conf = {};
                input_conf.pin_bit_mask = (1ULL << (item->interface.gpio.gpio_in.gpio_num));
                input_conf.intr_type = GPIO_INTR_DISABLE;
                input_conf.mode = GPIO_MODE_INPUT;
                input_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
                input_conf.pull_up_en = GPIO_PULLUP_ENABLE;
                ret = (0 == gpio_config(&input_conf)) ? EZPI_SUCCESS : EZPI_ERR_INIT_DEVICE_FAILED;
            }
        }
        else if ((ezlopi_item_name_smoke_density == item->cloud_properties.item_name))
        {
            s_mq4_value_t *MQ4_value = (s_mq4_value_t *)item->user_arg;
            if (MQ4_value)
            {
                if (GPIO_IS_VALID_GPIO(item->interface.adc.gpio_num))
                { // initialize analog_pin
                    if (EZPI_SUCCESS == EZPI_hal_adc_init(item->interface.adc.gpio_num, item->interface.adc.resln_bit))
                    {                                             // calibrate if not done
                        if (0 == (BIT0 & MQ4_value->status_flag)) // Calibration_complete_CH4 == 0
                        {
                            MQ4_value->heating_count = 20;
                            MQ4_value->avg_vol_count = MQ4_AVG_CAL_COUNT;
                            EZPI_service_loop_add("mq4_loop", __calibrate_MQ4_R0_resistance, 1000, (void *)item);
                            // #if defined(CONFIG_FREERTOS_USE_TRACE_FACILITY)
                            //                             EZPI_core_process_set_process_info(ENUM_EZLOPI_SENSOR_MQ4_TASK, &ezlopi_sensor_mq4_task_handle, EZLOPI_SENSOR_MQ4_TASK_DEPTH);
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
//------------------------------------------------------------------------------------------------------
static void __prepare_device_digi_cloud_properties_parent_digi(l_ezlopi_device_t *device, cJSON *cj_device)
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

    CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_type_str, item->interface_type); // _max = 10
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio1_str, item->interface.gpio.gpio_in.gpio_num);
    TRACE_S("MQ4-> DIGITAL_PIN: %d ", item->interface.gpio.gpio_in.gpio_num);
}

static void __prepare_device_adc_cloud_properties_child_adc(l_ezlopi_device_t *device, cJSON *cj_device)
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

    CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_type_str, item->interface_type); // _max = 10
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio2_str, item->interface.adc.gpio_num);
    TRACE_S("MQ4-> ADC_PIN: %d ", item->interface.adc.gpio_num);
    item->interface.adc.resln_bit = 3; // ADC 12_bit

    // passing the custom data_structure
    item->user_arg = user_data;
    item->is_user_arg_unique = true; // since 'item->user_arg' exist in only one-child [item_adc]
}

static ezlopi_error_t __0048_get_item(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        if (cj_result)
        {
            if (ezlopi_item_name_gas_alarm == item->cloud_properties.item_name)
            {
                //-------------------  POSSIBLE JSON ENUM CONTENTS ----------------------------------
                cJSON *json_array_enum = cJSON_CreateArray(__FUNCTION__);
                if (NULL != json_array_enum)
                {
                    char *mq4_sensor_gas_alarm_token[] = {
                        "no_gas",
                        "combustible_gas_detected",
                        "toxic_gas_detected",
                        "unknown",
                    };
                    for (uint8_t i = 0; i < MQ4_GAS_ALARM_MAX; i++)
                    {
                        cJSON *json_value = cJSON_CreateString(__FUNCTION__, mq4_sensor_gas_alarm_token[i]);
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
                s_mq4_value_t *MQ4_value = ((s_mq4_value_t *)item->user_arg);
                if (MQ4_value)
                {
                    EZPI_core_valueformatter_float_to_cjson(cj_result, MQ4_value->_CH4_ppm, scales_parts_per_million);
                }
            }
            ret = EZPI_SUCCESS;
        }
    }
    return ret;
}
static ezlopi_error_t __0048_get_cjson_value(l_ezlopi_item_t *item, void *arg)
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
            if (ezlopi_item_name_smoke_density == item->cloud_properties.item_name)
            {
                s_mq4_value_t *MQ4_value = ((s_mq4_value_t *)item->user_arg);
                if (MQ4_value)
                {
                    EZPI_core_valueformatter_float_to_cjson(cj_result, MQ4_value->_CH4_ppm, item->cloud_properties.scale);
                }
            }
            ret = EZPI_SUCCESS;
        }
    }
    return ret;
}
static ezlopi_error_t __0048_notify(l_ezlopi_item_t *item)
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
            s_mq4_value_t *MQ4_value = (s_mq4_value_t *)item->user_arg;
            if ((MQ4_value) && (BIT0 == (BIT0 & MQ4_value->status_flag))) // calibration_complete == 1
            {
                if (BIT1 == (BIT1 & MQ4_value->status_flag)) // loop_stop_flag == 1
                {
                    MQ4_value->status_flag ^= BIT1; // toggle BIT1 // loop_stop_flag => 0
                    // TRACE_D(" MQ4_value->status_flag : %03x", MQ4_value->status_flag);
                    EZPI_service_loop_remove(__calibrate_MQ4_R0_resistance);
                    TRACE_S("......Removed :- MQ4_calib_loop");
                }
                else
                {
                    double new_value = (double)__extract_MQ4_sensor_ppm(item);
                    if (fabs((double)(MQ4_value->_CH4_ppm) - new_value) > 0.0001)
                    {
                        MQ4_value->_CH4_ppm = (float)new_value;
                        EZPI_core_device_value_updated_from_device_broadcast(item);
                    }
                }
            }
        }
        ret = EZPI_SUCCESS;
    }
    return ret;
}

static float __extract_MQ4_sensor_ppm(l_ezlopi_item_t *item)
{
    s_mq4_value_t *MQ4_value = (s_mq4_value_t *)item->user_arg;
    if (MQ4_value)
    { // calculation process
        //-------------------------------------------------
        int mq4_adc_pin = item->interface.adc.gpio_num;
        s_ezlopi_analog_data_t ezlopi_analog_data = {.value = 0, .voltage = 0};
        // extract the mean_sensor_analog_output_voltage
        MQ4_value->calib_avg_volt = 0;
        for (uint8_t x = 10; x > 0; x--)
        {
            EZPI_hal_adc_get_adc_data(mq4_adc_pin, &ezlopi_analog_data);
#ifdef VOLTAGE_DIVIDER_ADDED
            MQ4_value->calib_avg_volt += ((float)(ezlopi_analog_data.voltage) * 2.0f);
#else
            MQ4_value->calib_avg_volt += (float)(ezlopi_analog_data.voltage);
#endif
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        MQ4_value->calib_avg_volt = MQ4_value->calib_avg_volt / 10.0f;

        //-----------------------------------------------------------------------------------
        // Stage_2 : [from 'sensor_0048_ADC_MQ4_methane_gas_detector.h']

        // 1. Calculate 'Rs_gas' for the gas detected
        float Rs_gas = (((MQ4_VOLT_RESOLUTION_Vc * mq4_eqv_RL) / (MQ4_value->calib_avg_volt / 1000.0f)) - mq4_eqv_RL);

        // 1.1 Calculate @ 'ratio' during CH4 presence
        double _ratio = (Rs_gas / (((MQ4_value->MQ4_R0_constant) <= 0) ? (1.0f) : (MQ4_value->MQ4_R0_constant))); // avoid dividing by zero??

        //-------------------------------------------------

        // 1.2 Calculate _CH4_ppm
        float _CH4_ppm = (float)pow(10, (((float)log10(_ratio)) - b_coeff_mq4) / m_slope_mq4); // ---> _CH4_ppm = 10 ^ [ ( log(ratio) - b ) / m ]
        if (_CH4_ppm < 0)
        {
            _CH4_ppm = 0; // No negative values accepted or upper datasheet recomendation.
        }

        TRACE_E("_CH4_ppm [CH4] : %.2f -> ratio[RS/R0] : %.2f -> Volts : %0.2fmv", _CH4_ppm, (float)_ratio, MQ4_value->calib_avg_volt);
        return _CH4_ppm;
        //-------------------------------------------------
    }
    return 0;
}
static void __calibrate_MQ4_R0_resistance(void *params)
{
    l_ezlopi_item_t *item = (l_ezlopi_item_t *)params;
    if (NULL != item)
    {
        s_mq4_value_t *MQ4_value = (s_mq4_value_t *)item->user_arg;
        if (MQ4_value && (0 == (BIT1 & MQ4_value->status_flag))) // loop_stop_flag == 0
        {
            int mq4_adc_pin = item->interface.adc.gpio_num;
            //-------------------------------------------------
            // let the sensor to heat for 20seconds
            if (MQ4_value->heating_count > 0)
            {
                // if (0 == MQ4_value->heating_count % 20)
                // {
                //     TRACE_E("Heating sensor.........time left: %d sec", MQ4_value->heating_count / 10);
                // }
                MQ4_value->heating_count--;
            }
            else // after heating the sensor for 20 sec
            {
                //-------------------------------------------------
                // extract the mean_sensor_analog_output_voltage
                if (MQ4_value->avg_vol_count != 0)
                {
                    s_ezlopi_analog_data_t ezlopi_analog_data = {.value = 0, .voltage = 0};
                    // extract ADC values
                    EZPI_hal_adc_get_adc_data(mq4_adc_pin, &ezlopi_analog_data);
#ifdef VOLTAGE_DIVIDER_ADDED
                    MQ4_value->calib_avg_volt += (float)((ezlopi_analog_data.voltage) * 2.0f); // [0-2.4V] X2
#else
                    MQ4_value->calib_avg_volt += (float)(ezlopi_analog_data.voltage);
#endif
                    // TRACE_D(" _count : %d", MQ4_value->avg_vol_count);
                    MQ4_value->avg_vol_count--;

                    if (0 == MQ4_value->avg_vol_count)
                    {
                        MQ4_value->status_flag |= BIT2;
                    }
                }

                if (BIT2 == (MQ4_value->status_flag & BIT2))
                {
                    MQ4_value->status_flag ^= BIT2; // avg_volt_flag => 0
                    MQ4_value->calib_avg_volt /= MQ4_AVG_CAL_COUNT;
                    //-------------------------------------------------
                    // Calculate the 'Rs' of heater during clean air [calibration phase]
                    // Range -> [2Kohm - 20Kohm]
                    float RS_calib = 0;                                                                                      // Define variable for sensor resistance
                    RS_calib = ((MQ4_VOLT_RESOLUTION_Vc * mq4_eqv_RL) / (MQ4_value->calib_avg_volt / 1000.0f)) - mq4_eqv_RL; // Calculate RS in fresh air
                    TRACE_E("CALIB_TASK -> 'RS_calib' = %.2f", RS_calib);
                    if (RS_calib < 0)
                    {
                        RS_calib = 0; // No negative values accepted.
                    }
                    // Calculate the R0_air which is constant through-out
                    MQ4_value->MQ4_R0_constant = (RS_calib / RatioMQ4CleanAir); // Calculate MQ4_R0_constant
                    TRACE_E("CALIB_TASK -> 'MQ4_R0_constant' = %.2f", (MQ4_value->MQ4_R0_constant));
                    if ((MQ4_value->MQ4_R0_constant) < 0)
                    {
                        (MQ4_value->MQ4_R0_constant) = 0; // No negative values accepted.
                    }
                    // loop_stop_flag => 1 // Calibration_complete_CH4 => 1;
                    MQ4_value->status_flag |= (BIT0 | BIT1);
                }
            }
        }
    }
    // #if defined(CONFIG_FREERTOS_USE_TRACE_FACILITY)
    //     EZPI_core_process_set_is_deleted(ENUM_EZLOPI_SENSOR_MQ4_TASK);
    // #endif
}

/*******************************************************************************
 *                          End of File
 *******************************************************************************/