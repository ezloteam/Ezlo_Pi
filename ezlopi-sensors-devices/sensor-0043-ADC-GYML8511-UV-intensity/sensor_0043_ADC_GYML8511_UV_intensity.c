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
 * @file    sensor_0043_ADC_GYML8511_UV_intensity.c
 * @brief   perform some function on sensor_0043
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

#include "ezlopi_hal_adc.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "sensor_0043_ADC_GYML8511_UV_intensity.h"
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
typedef struct s_gyml8511_data
{
    float uv_data;
} s_gyml8511_data_t;

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/

static ezlopi_error_t __0043_prepare(void *arg);
static ezlopi_error_t __0043_init(l_ezlopi_item_t *item);
static ezlopi_error_t __0043_get_cjson_value(l_ezlopi_item_t *item, void *arg);
static ezlopi_error_t __0043_notify(l_ezlopi_item_t *item);
static float mapfloat(float x, float in_min, float in_max, float out_min, float out_max);
static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_item_cloud_properties(l_ezlopi_item_t *item, void *user_data);
static void __prepare_item_interface_properties(l_ezlopi_item_t *item, cJSON *cj_device);

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
ezlopi_error_t SENSOR_0043_adc_gyml8511_uv_intensity(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    ezlopi_error_t ret = EZPI_SUCCESS;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = __0043_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = __0043_init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = __0043_get_cjson_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        ret = __0043_notify(item);
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

static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    device->cloud_properties.category = category_level_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type_id = NULL;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type = dev_type_sensor;
}

static void __prepare_item_cloud_properties(l_ezlopi_item_t *item, void *user_data)
{
    item->cloud_properties.item_id = EZPI_core_cloud_generate_item_id();
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_solar_radiation;
    item->cloud_properties.show = true;
    item->cloud_properties.value_type = value_type_ultraviolet;
    item->cloud_properties.scale = scales_watt_per_square_meter;
    //----- CUSTOM DATA STRUCTURE -----------------------------------------
    item->is_user_arg_unique = true;
    item->user_arg = user_data;
}

static void __prepare_item_interface_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    if (item && cj_device)
    {
        item->interface_type = EZLOPI_DEVICE_INTERFACE_MAX; // other
        CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_str, item->interface.adc.gpio_num);
        item->interface.adc.resln_bit = 3;
    }
}
static ezlopi_error_t __0043_prepare(void *arg)
{
    ezlopi_error_t ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (device_prep_arg && (NULL != device_prep_arg->cjson_device))
    {
        cJSON *cj_device = device_prep_arg->cjson_device;

        s_gyml8511_data_t *gyml8511_value = (s_gyml8511_data_t *)ezlopi_malloc(__FUNCTION__, sizeof(s_gyml8511_data_t));
        if (NULL != gyml8511_value)
        {
            memset(gyml8511_value, 0, sizeof(s_gyml8511_data_t));
            l_ezlopi_device_t *gyml8511_device = EZPI_core_device_add_device(cj_device, NULL);
            if (gyml8511_device)
            {
                __prepare_device_cloud_properties(gyml8511_device, cj_device);

                l_ezlopi_item_t *gyml8511_item = EZPI_core_device_add_item_to_device(gyml8511_device, SENSOR_0043_adc_gyml8511_uv_intensity);
                if (gyml8511_item)
                {
                    __prepare_item_cloud_properties(gyml8511_item, gyml8511_value);
                    __prepare_item_interface_properties(gyml8511_item, cj_device);
                    ret = EZPI_SUCCESS;
                }
                else
                {
                    EZPI_core_device_free_device(gyml8511_device);
                    ezlopi_free(__FUNCTION__, gyml8511_value);
                }
            }
            else
            {
                ezlopi_free(__FUNCTION__, gyml8511_value);
            }
        }
    }
    return ret;
}
static ezlopi_error_t __0043_init(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_ERR_INIT_DEVICE_FAILED;
    if (NULL != item)
    {
        s_gyml8511_data_t *user_data = (s_gyml8511_data_t *)item->user_arg;
        if (user_data)
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
static ezlopi_error_t __0043_get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        if (cj_result)
        {
            s_gyml8511_data_t *user_data = (s_gyml8511_data_t *)item->user_arg;
            if (user_data)
            {
                EZPI_core_valueformatter_float_to_cjson(cj_result, (user_data->uv_data) / 10, NULL);
                ret = EZPI_SUCCESS;
            }
        }
    }
    return ret;
}
static ezlopi_error_t __0043_notify(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item)
    {
        s_gyml8511_data_t *user_data = (s_gyml8511_data_t *)item->user_arg;
        if (user_data)
        {
            s_ezlopi_analog_data_t adc_data = {.value = 0, .voltage = 0};
            EZPI_hal_adc_get_adc_data(item->interface.adc.gpio_num, &adc_data);
            float new_uvIntensity = mapfloat(((float)(adc_data.voltage) / 1000), 0.97, 2.7, 0.0, 15.0);
            TRACE_S("%dmv -> intensity: %.2f", adc_data.voltage, new_uvIntensity);
            if (fabs((user_data->uv_data) - new_uvIntensity) > 0.01)
            {
                user_data->uv_data = new_uvIntensity;
                EZPI_core_device_value_updated_from_device_broadcast(item);
                ret = EZPI_SUCCESS;
            }
        }
    }
    return ret;
}
static float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
    float res = ((x - in_min) * ((out_max - out_min) / (in_max - in_min)) + out_min);
    if (res < 0)
    {
        res = 0;
    }
    return res;
}

/*******************************************************************************
 *                          End of File
 *******************************************************************************/