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
* @file    sensor_0057_other_KY026_FlameDetector.c
* @brief   perform some function on sensor_0057
* @author  xx
* @version 0.1
* @date    xx
*/

/*******************************************************************************
*                          Include Files
*******************************************************************************/
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_hal_adc.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "sensor_0057_other_KY026_FlameDetector.h"
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

/*******************************************************************************
*                          Static Function Prototypes
*******************************************************************************/
static ezlopi_error_t __0057_prepare(void *arg);
static ezlopi_error_t __0057_init(l_ezlopi_item_t *item);
static ezlopi_error_t __0057_get_item(l_ezlopi_item_t *item, void *arg);
static ezlopi_error_t __0057_get_cjson_value(l_ezlopi_item_t *item, void *arg);
static ezlopi_error_t __0057_notify(l_ezlopi_item_t *item);

static void __prepare_device_digi_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_item_digi_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device);
static void __prepare_device_adc_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_item_adc_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_data);
static void __extract_KY026_sensor_value(uint32_t flame_adc_pin, float *analog_sensor_volt, float *max_reading);

/*******************************************************************************
*                          Static Data Definitions
*******************************************************************************/
static const char *ky206_sensor_heat_alarm_token[] = {
    "heat_ok",
    "overheat_detected",
    "under_heat_detected",
    "unknown",
};
/*******************************************************************************
*                          Extern Data Definitions
*******************************************************************************/

/*******************************************************************************
*                          Extern Function Definitions
*******************************************************************************/
ezlopi_error_t SENSOR_0057_other_ky026_flamedetector(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    ezlopi_error_t ret = EZPI_SUCCESS;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = __0057_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = __0057_init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    {
        ret = __0057_get_item(item, arg);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = __0057_get_cjson_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        ret = __0057_notify(item);
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

static ezlopi_error_t __0057_prepare(void *arg)
{
    ezlopi_error_t ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (device_prep_arg && (NULL != device_prep_arg->cjson_device))
    {
        //---------------------------  DIGI - DEVICE 1 --------------------------------------------
        l_ezlopi_device_t *flame_device_parent_digi = EZPI_core_device_add_device(device_prep_arg->cjson_device, "digi");
        if (flame_device_parent_digi)
        {
            TRACE_I("Parent_flame_device_digi-[0x%x] ", flame_device_parent_digi->cloud_properties.device_id);
            __prepare_device_digi_cloud_properties(flame_device_parent_digi, device_prep_arg->cjson_device);
            l_ezlopi_item_t *flame_item_digi = EZPI_core_device_add_item_to_device(flame_device_parent_digi, SENSOR_0057_other_ky026_flamedetector);
            if (flame_item_digi)
            {
                flame_item_digi->cloud_properties.device_id = flame_device_parent_digi->cloud_properties.device_id;
                __prepare_item_digi_cloud_properties(flame_item_digi, device_prep_arg->cjson_device);
                ret = EZPI_SUCCESS;
            }

            //---------------------------- ADC - DEVICE 2 -------------------------------------------
            flame_t *flame_struct = (flame_t *)ezlopi_malloc(__FUNCTION__, sizeof(flame_t));
            if (NULL != flame_struct)
            {
                memset(flame_struct, 0, sizeof(flame_t));
                l_ezlopi_device_t *flame_device_child_adc = EZPI_core_device_add_device(device_prep_arg->cjson_device, "adc");
                if (flame_device_child_adc)
                {
                    TRACE_I("Child_flame_device_adc-[0x%x] ", flame_device_child_adc->cloud_properties.device_id);
                    __prepare_device_adc_cloud_properties(flame_device_child_adc, device_prep_arg->cjson_device);

                    l_ezlopi_item_t *flame_item_adc = EZPI_core_device_add_item_to_device(flame_device_child_adc, SENSOR_0057_other_ky026_flamedetector);
                    if (flame_item_adc)
                    {
                        flame_item_adc->cloud_properties.device_id = flame_device_child_adc->cloud_properties.device_id;
                        __prepare_item_adc_cloud_properties(flame_item_adc, device_prep_arg->cjson_device, flame_struct);
                        ret = EZPI_SUCCESS;
                    }
                    else
                    {
                        EZPI_core_device_free_device(flame_device_child_adc);
                        ezlopi_free(__FUNCTION__, flame_struct);
                    }
                }
                else
                {
                    ezlopi_free(__FUNCTION__, flame_struct);
                }
            }
        }
    }
    return ret;
}

static ezlopi_error_t __0057_init(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_ERR_INIT_DEVICE_FAILED;
    if (NULL != item)
    {
        if (ezlopi_item_name_heat_alarm == item->cloud_properties.item_name)
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
        else if (ezlopi_item_name_temperature_changes == item->cloud_properties.item_name)
        {
            flame_t *flame_struct = (flame_t *)item->user_arg;
            if (flame_struct)
            {
                if (GPIO_IS_VALID_GPIO(item->interface.adc.gpio_num))
                { // initialize analog_pin
                    if (EZPI_SUCCESS == EZPI_hal_adc_init(item->interface.adc.gpio_num, item->interface.adc.resln_bit))
                    {
                        ret = EZPI_SUCCESS;
                    }
                }
            }
        }
    }
    return ret;
}

//------------------------------------------------------------------------------------------------------
static void __prepare_device_digi_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    device->cloud_properties.category = category_security_sensor;
    device->cloud_properties.subcategory = subcategory_heat;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_item_digi_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_heat_alarm;
    item->cloud_properties.value_type = value_type_token;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.item_id = EZPI_core_cloud_generate_item_id();

    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_dev_type_str, item->interface_type); // _max = 10
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio1_str, item->interface.gpio.gpio_in.gpio_num);
    TRACE_S("flame_> DIGITAL_PIN: %d ", item->interface.gpio.gpio_in.gpio_num);
}
//------------------------------------------------------------------------------------------------------
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
    item->cloud_properties.item_name = ezlopi_item_name_temperature_changes;
    item->cloud_properties.value_type = value_type_general_purpose;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = scales_percent;
    item->cloud_properties.item_id = EZPI_core_cloud_generate_item_id();

    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_dev_type_str, item->interface_type); // _max = 10
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio2_str, item->interface.adc.gpio_num);
    TRACE_S("flame_> ADC_PIN: %d ", item->interface.adc.gpio_num);
    item->interface.adc.resln_bit = 3; // ADC 12_bit

    // passing the custom data_structure
    item->is_user_arg_unique = true;
    item->user_arg = user_data;
}

//------------------------------------------------------------------------------------------------------
static ezlopi_error_t __0057_get_item(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        if (cj_result)
        {
            if (ezlopi_item_name_heat_alarm == item->cloud_properties.item_name)
            {
                //-------------------  POSSIBLE JSON ENUM CONTENTS ----------------------------------
                cJSON *json_array_enum = cJSON_CreateArray(__FUNCTION__);
                if (NULL != json_array_enum)
                {
                    for (uint8_t i = 0; i < KY206_HEAT_ALARM_MAX; i++)
                    {
                        cJSON *json_value = cJSON_CreateString(__FUNCTION__, ky206_sensor_heat_alarm_token[i]);
                        if (NULL != json_value)
                        {
                            cJSON_AddItemToArray(json_array_enum, json_value);
                        }
                    }
                    cJSON_AddItemToObject(__FUNCTION__, cj_result, ezlopi_enum_str, json_array_enum);
                }
                //--------------------------------------------------------------------------------------
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_valueFormatted_str, (char *)item->user_arg ? item->user_arg : ky206_sensor_heat_alarm_token[0]);
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_value_str, (char *)item->user_arg ? item->user_arg : ky206_sensor_heat_alarm_token[0]);
            }
            else if (ezlopi_item_name_temperature_changes == item->cloud_properties.item_name)
            {
                flame_t *flame_struct = (flame_t *)item->user_arg;
                if (flame_struct)
                {
                    EZPI_core_valueformatter_float_to_cjson(cj_result, flame_struct->absorbed_percent, item->cloud_properties.scale);
                }
                ret = EZPI_SUCCESS;
            }
        }
    }
    return ret;
}

static ezlopi_error_t __0057_get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        if (cj_result)
        {
            if (ezlopi_item_name_heat_alarm == item->cloud_properties.item_name)
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_valueFormatted_str, (char *)item->user_arg ? item->user_arg : ky206_sensor_heat_alarm_token[0]);
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_value_str, (char *)item->user_arg ? item->user_arg : ky206_sensor_heat_alarm_token[0]);
            }
            else if (ezlopi_item_name_temperature_changes == item->cloud_properties.item_name)
            {
                flame_t *flame_struct = (flame_t *)item->user_arg;
                if (flame_struct)
                {
                    EZPI_core_valueformatter_float_to_cjson(cj_result, flame_struct->absorbed_percent, item->cloud_properties.scale);
                }
            }
            ret = EZPI_SUCCESS;
        }
    }
    return ret;
}

static ezlopi_error_t __0057_notify(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item)
    {
        if (ezlopi_item_name_heat_alarm == item->cloud_properties.item_name)
        {
            const char *curret_value = NULL;
            if (0 == gpio_get_level(item->interface.gpio.gpio_in.gpio_num)) // when D0 -> 0V,
            {
                curret_value = ky206_sensor_heat_alarm_token[0];
            }
            else
            {
                curret_value = "overheat_detected";
            }
            if (curret_value != (char *)item->user_arg) // calls update only if there is change in state
            {
                item->user_arg = (void *)curret_value;
                EZPI_core_device_value_updated_from_device_broadcast(item);
            }
        }
        else if (ezlopi_item_name_temperature_changes == item->cloud_properties.item_name)
        {
            flame_t *flame_struct = (flame_t *)item->user_arg;
            if (flame_struct)
            {
                float analog_sensor_volt = 0, max_volt_reading = 0;
                // extract the sensor_output_values
                __extract_KY026_sensor_value(item->interface.adc.gpio_num, &analog_sensor_volt, &max_volt_reading);
                float new_percent = ((1 - (analog_sensor_volt / max_volt_reading)) * 100.0f);
                // TRACE_E("Heat-detected: %.2f percent", absorbed_percent);
                if (new_percent != flame_struct->absorbed_percent)
                {
                    EZPI_core_device_value_updated_from_device_broadcast(item);
                    flame_struct->absorbed_percent = new_percent;
                }
            }
        }
        ret = EZPI_SUCCESS;
    }
    return ret;
}
//--------------------------------------------------------------------------------------------------------------------------------------
static void __extract_KY026_sensor_value(uint32_t flame_adc_pin, float *analog_sensor_volt, float *max_reading)
{
    static float max = 0;
    // calculation process
    s_ezlopi_analog_data_t ezlopi_analog_data = { .value = 0, .voltage = 0 };
    //-------------------------------------------------
    // extract the mean_sensor_analog_output_voltage
    for (uint8_t x = 10; x > 0; x--)
    {
        EZPI_hal_adc_get_adc_data(flame_adc_pin, &ezlopi_analog_data);
#ifdef VOLTAGE_DIVIDER_ADDED
        *analog_sensor_volt += ((float)((ezlopi_analog_data.voltage) / 1000.0f) * 2.0f); // V
#else
        *analog_sensor_volt += (float)(ezlopi_analog_data.voltage);
#endif
    }
    // find mean
    *analog_sensor_volt = *analog_sensor_volt / 10.0f;

    if (max < *analog_sensor_volt)
    {
        max = *analog_sensor_volt;
    }
    // Set max_value of the readings done
    *max_reading = max;
}

/*******************************************************************************
*                          End of File
*******************************************************************************/