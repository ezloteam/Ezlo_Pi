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
* @file    sensor_0026_ADC_LDR.c
* @brief   perform some function on sensor_0026
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

#include "sensor_0026_ADC_LDR.h"
/*******************************************************************************
*                          Extern Data Declarations
*******************************************************************************/
static const char *light_alarm_states[] =
{
    "no_light",
    "light_detected",
    "unknown",
};


/*******************************************************************************
*                          Extern Function Declarations
*******************************************************************************/

/*******************************************************************************
*                          Type & Macro Definitions
*******************************************************************************/

/*******************************************************************************
*                          Static Function Prototypes
*******************************************************************************/
static ezlopi_error_t __prepare(void *arg);
static ezlopi_error_t __init(l_ezlopi_item_t *item);
static ezlopi_error_t __notify(l_ezlopi_item_t *item);
static ezlopi_error_t __get_value_cjson(l_ezlopi_item_t *item, void *arg);
static void __setup_device_cloud_params(l_ezlopi_device_t *device, cJSON *cj_device);
static void __setup_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device);
static ezlopi_error_t _get_item_list(l_ezlopi_item_t *item, void *arg);

/*******************************************************************************
*                          Static Data Definitions
*******************************************************************************/

/*******************************************************************************
*                          Extern Data Definitions
*******************************************************************************/

/*******************************************************************************
*                          Extern Function Definitions
*******************************************************************************/

ezlopi_error_t SENSOR_0026_adc_ldr(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    ezlopi_error_t ret = EZPI_SUCCESS;

    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = __prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = __init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    {
        _get_item_list(item, arg);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        __get_value_cjson(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        __notify(item);
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

static void __setup_device_cloud_params(l_ezlopi_device_t *device, cJSON *cj_device)
{
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
    device->cloud_properties.device_type = dev_type_device;
    device->cloud_properties.category = category_light_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
}

static void __setup_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    if (item)
    {
        item->cloud_properties.show = true;
        item->cloud_properties.has_getter = true;
        item->cloud_properties.has_setter = false;
        item->cloud_properties.item_name = ezlopi_item_name_light_alarm;
        item->cloud_properties.value_type = value_type_token;
        item->cloud_properties.scale = NULL;
        item->cloud_properties.item_id = EZPI_core_cloud_generate_item_id();

        item->interface_type = EZLOPI_DEVICE_INTERFACE_ANALOG_INPUT;
        item->interface.adc.resln_bit = 3;
        CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_str, item->interface.adc.gpio_num);
    }
}

static ezlopi_error_t __prepare(void *arg)
{
    ezlopi_error_t ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (prep_arg && prep_arg->cjson_device)
    {
        cJSON *cj_device = prep_arg->cjson_device;
        if (cj_device)
        {
            l_ezlopi_device_t *device = EZPI_core_device_add_device(cj_device, NULL);
            if (device)
            {
                __setup_device_cloud_params(device, cj_device);
                l_ezlopi_item_t *item = EZPI_core_device_add_item_to_device(device, SENSOR_0026_adc_ldr);
                if (item)
                {
                    __setup_item_cloud_properties(item, cj_device);
                    ret = EZPI_SUCCESS;
                }
                else
                {
                    EZPI_core_device_free_device(device);
                }
            }
        }
    }
    return ret;
}

static ezlopi_error_t __init(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_ERR_INIT_DEVICE_FAILED;
    if (item)
    {
        if (GPIO_IS_VALID_GPIO(item->interface.adc.gpio_num))
        {
            if (EZPI_SUCCESS == EZPI_hal_adc_init(item->interface.adc.gpio_num, item->interface.adc.resln_bit))
            {
                ret = EZPI_SUCCESS;
            }
        }
    }
    return ret;
}

static ezlopi_error_t _get_item_list(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_FAILED;
    cJSON *cj_result = (cJSON *)arg;
    if (cj_result && item)
    {
        cJSON *json_array_enum = cJSON_CreateArray(__FUNCTION__);
        if (NULL != json_array_enum)
        {
            for (uint8_t i = 0; i < LIGHT_ALARM_MAX; i++)
            {
                cJSON *json_value = cJSON_CreateString(__FUNCTION__, light_alarm_states[i]);
                if (NULL != json_value)
                {
                    cJSON_AddItemToArray(json_array_enum, json_value);
                }
            }
            cJSON_AddItemToObject(__FUNCTION__, cj_result, ezlopi_enum_str, json_array_enum);
        }
        cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_value_str, ((char *)item->user_arg) ? item->user_arg : light_alarm_states[0]);
        cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_valueFormatted_str, ((char *)item->user_arg) ? item->user_arg : light_alarm_states[0]);
        ret = EZPI_SUCCESS;
    }

    return ret;
}

static ezlopi_error_t __get_value_cjson(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_FAILED;
    cJSON *cj_result = (cJSON *)arg;
    if (cj_result && item)
    {
        cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_value_str, ((char *)item->user_arg) ? item->user_arg : light_alarm_states[0]);
        cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_valueFormatted_str, ((char *)item->user_arg) ? item->user_arg : light_alarm_states[0]);
        ret = EZPI_SUCCESS;
    }
    return ret;
}

static ezlopi_error_t __notify(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item)
    {
        s_ezlopi_analog_data_t ezlopi_analog_data = { .value = 0, .voltage = 0 };
        EZPI_hal_adc_get_adc_data(item->interface.adc.gpio_num, &ezlopi_analog_data);
        TRACE_I("Value is: %d, voltage is: %d", ezlopi_analog_data.value, ezlopi_analog_data.voltage);
        const char *curr_ldr_state = NULL;
        if (((float)(4096 - ezlopi_analog_data.value) / 4096.0f) > 0.5f) // pot_val : [100% - 30%]
        {
            curr_ldr_state = light_alarm_states[0]; // no light
        }
        else
        {
            curr_ldr_state = light_alarm_states[1]; // light detected
        }
        if (curr_ldr_state != (char *)item->user_arg)
        {
            item->user_arg = (void *)curr_ldr_state;
            EZPI_core_device_value_updated_from_device_broadcast(item);
            ret = EZPI_SUCCESS;
        }
    }
    return ret;
}

/*******************************************************************************
*                          End of File
*******************************************************************************/