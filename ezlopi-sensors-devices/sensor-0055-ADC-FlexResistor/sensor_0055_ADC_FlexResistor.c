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
 * @file    sensor_0055_ADC_FlexResistor.c
 * @brief   perform some function on sensor_0055
 * @author  xx
 * @version 0.1
 * @date    xx
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_hal_adc.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "sensor_0055_ADC_FlexResistor.h"
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
static ezlopi_error_t __0055_prepare(void *arg);
static ezlopi_error_t __0055_init(l_ezlopi_item_t *item);
static ezlopi_error_t __0055_get_cjson_value(l_ezlopi_item_t *item, void *arg);
static ezlopi_error_t __0055_notify(l_ezlopi_item_t *item);
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
ezlopi_error_t SENSOR_0055_adc_flexresistor(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    ezlopi_error_t ret = EZPI_SUCCESS;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = __0055_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = __0055_init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = __0055_get_cjson_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        ret = __0055_notify(item);
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
    item->cloud_properties.item_name = ezlopi_item_name_electrical_resistivity;
    item->cloud_properties.value_type = value_type_electrical_resistance;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = scales_ohm_meter;
    item->cloud_properties.item_id = EZPI_core_cloud_generate_item_id();

    CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_type_str, item->interface_type); // _max = 10
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_str, item->interface.adc.gpio_num);
    item->interface.adc.resln_bit = 3; // ADC 12_bit

    // passing the custom data_structure
    item->is_user_arg_unique = true;
    item->user_arg = user_data;
}
//------------------------------------------------------------------------------------------------------
static ezlopi_error_t __0055_prepare(void *arg)
{
    ezlopi_error_t ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (device_prep_arg && (NULL != device_prep_arg->cjson_device))
    {
        flex_t *flex_res_value = (flex_t *)ezlopi_malloc(__FUNCTION__, sizeof(flex_t));
        if (flex_res_value)
        {
            memset(flex_res_value, 0, sizeof(flex_t));
            l_ezlopi_device_t *device_adc = EZPI_core_device_add_device(device_prep_arg->cjson_device, NULL);
            if (device_adc)
            {
                __prepare_device_adc_cloud_properties(device_adc, device_prep_arg->cjson_device);
                l_ezlopi_item_t *item_adc = EZPI_core_device_add_item_to_device(device_adc, SENSOR_0055_adc_flexresistor);
                if (item_adc)
                {
                    __prepare_item_adc_cloud_properties(item_adc, device_prep_arg->cjson_device, flex_res_value);
                    ret = EZPI_SUCCESS;
                }
                else
                {
                    EZPI_core_device_free_device(device_adc);
                    ezlopi_free(__FUNCTION__, flex_res_value);
                }
            }
            else
            {
                ezlopi_free(__FUNCTION__, flex_res_value);
            }
        }
    }
    return ret;
}
static ezlopi_error_t __0055_init(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (NULL != item)
    {
        flex_t *flex_res_value = (flex_t *)ezlopi_malloc(__FUNCTION__, sizeof(flex_t));
        if (flex_res_value)
        {
            if (GPIO_IS_VALID_GPIO(item->interface.adc.gpio_num))
            {
                if (EZPI_SUCCESS == EZPI_hal_adc_init(item->interface.adc.gpio_num, item->interface.adc.resln_bit))
                {
                    ret = EZPI_SUCCESS;
                }
            }
        }
    }
    return ret;
}
static ezlopi_error_t __0055_get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_SUCCESS;
    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        if (cj_result)
        {
            flex_t *flex_res_value = (flex_t *)item->user_arg;
            if (flex_res_value)
            {
                EZPI_core_valueformatter_int32_to_cjson(cj_result, flex_res_value->rs_0055, NULL);
                ret = EZPI_SUCCESS;
            }
        }
    }
    return ret;
}
static ezlopi_error_t __0055_notify(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_SUCCESS;
    if (item)
    {
        flex_t *flex_res_value = (flex_t *)item->user_arg;
        if (flex_res_value)
        {
            s_ezlopi_analog_data_t ezlopi_analog_data = {.value = 0,
                                                         .voltage = 0};
            // extract the sensor_output_values
            EZPI_hal_adc_get_adc_data(item->interface.adc.gpio_num, &ezlopi_analog_data);
            float Vout = (ezlopi_analog_data.voltage) / 1000.0f; // millivolt -> voltage

            // calculate the 'rs_0055' resistance value using [voltage divider rule]
            int new_rs_0055 = (int)(((flex_Vin / Vout) - 1) * flex_Rout);
            if (new_rs_0055 != flex_res_value->rs_0055)
            {
                EZPI_core_device_value_updated_from_device_broadcast(item);
                flex_res_value->rs_0055 = new_rs_0055;
            }
            ret = EZPI_SUCCESS;
        }
    }
    return ret;
}

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
