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
 * @file    sensor_0015_oneWire_DHT11.c
 * @brief   perform some function on sensor_0015
 * @author  xx
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
#include "ezlopi_core_setting_commands.h"
#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "dht11.h"
#include "sensor_0015_oneWire_DHT11.h"
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
typedef struct s_ezlopi_dht11_data
{
    float temperature;
    float humidity;
} s_ezlopi_dht11_data_t;

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
static ezlopi_error_t __0015_prepare(void *arg);
static ezlopi_error_t __0015_init(l_ezlopi_item_t *item);
static ezlopi_error_t __0015_get_value(l_ezlopi_item_t *item, void *args);
static ezlopi_error_t __0015_notify(l_ezlopi_item_t *item);

static ezlopi_error_t __dht11_setup_item_properties_temperature(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg);
static ezlopi_error_t __dht11_setup_item_properties_humidity(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg);
static ezlopi_error_t __dht11_setup_device_cloud_properties_temperature(l_ezlopi_device_t *device, cJSON *cj_device);
static ezlopi_error_t __dht11_setup_device_cloud_properties_humidity(l_ezlopi_device_t *device, cJSON *cj_device);
/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
ezlopi_error_t SENSOR_0015_oneWire_dht11(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    ezlopi_error_t ret = EZPI_SUCCESS;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = __0015_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = __0015_init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = __0015_get_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        ret = __0015_notify(item);
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
 *                          Static Function Definitions
 *******************************************************************************/
static ezlopi_error_t __0015_prepare(void *arg)
{
    ezlopi_error_t ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (prep_arg)
    {
        cJSON *cjson_device = prep_arg->cjson_device;
        if (cjson_device)
        {
            s_ezlopi_dht11_data_t *dht11_sensor_data = (s_ezlopi_dht11_data_t *)ezlopi_malloc(__FUNCTION__, sizeof(s_ezlopi_dht11_data_t));
            if (dht11_sensor_data)
            {
                memset(dht11_sensor_data, 0, sizeof(s_ezlopi_dht11_data_t));
                l_ezlopi_device_t *parent_device_temperature = EZPI_core_device_add_device(cjson_device, "temp");
                if (parent_device_temperature)
                {
                    TRACE_I("Parent_dht11_temp_device-[0x%x] ", parent_device_temperature->cloud_properties.device_id);
                    __dht11_setup_device_cloud_properties_temperature(parent_device_temperature, cjson_device);
                    l_ezlopi_item_t *item_temperature = EZPI_core_device_add_item_to_device(parent_device_temperature, SENSOR_0015_oneWire_dht11);
                    if (item_temperature)
                    {
                        __dht11_setup_item_properties_temperature(item_temperature, cjson_device, dht11_sensor_data);
                    }

                    l_ezlopi_device_t *child_device_humidity = EZPI_core_device_add_device(cjson_device, "humi");
                    if (child_device_humidity)
                    {
                        TRACE_I("Child_dht11_humi_device-[0x%x] ", child_device_humidity->cloud_properties.device_id);
                        __dht11_setup_device_cloud_properties_humidity(child_device_humidity, cjson_device);

                        l_ezlopi_item_t *item_humidity = EZPI_core_device_add_item_to_device(child_device_humidity, SENSOR_0015_oneWire_dht11);
                        if (item_humidity)
                        {
                            __dht11_setup_item_properties_humidity(item_humidity, cjson_device, dht11_sensor_data);
                        }
                        else
                        {
                            EZPI_core_device_free_device(child_device_humidity);
                        }
                    }

                    if ((NULL == item_temperature) &&
                        (NULL == child_device_humidity))
                    {
                        EZPI_core_device_free_device(parent_device_temperature);
                        ezlopi_free(__FUNCTION__, dht11_sensor_data);
                    }
                    else
                    {
                        ret = EZPI_SUCCESS;
                    }
                }
                else
                {
                    ezlopi_free(__FUNCTION__, dht11_sensor_data);
                }
            }
        }
    }

    return ret;
}

static ezlopi_error_t __dht11_setup_device_cloud_properties_temperature(l_ezlopi_device_t *device, cJSON *cj_device)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (device && cj_device)
    {
        device->cloud_properties.category = category_temperature;
        device->cloud_properties.subcategory = subcategory_not_defined;
        device->cloud_properties.device_type_id = NULL;
        device->cloud_properties.info = NULL;
        device->cloud_properties.device_type = dev_type_sensor;
        ret = EZPI_SUCCESS;
    }
    return ret;
}

static ezlopi_error_t __dht11_setup_device_cloud_properties_humidity(l_ezlopi_device_t *device, cJSON *cj_device)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (device && cj_device)
    {
        device->cloud_properties.category = category_humidity;
        device->cloud_properties.subcategory = subcategory_not_defined;
        device->cloud_properties.device_type_id = NULL;
        device->cloud_properties.info = NULL;
        device->cloud_properties.device_type = dev_type_sensor;
        ret = EZPI_SUCCESS;
    }
    return ret;
}

static ezlopi_error_t __dht11_setup_item_properties_temperature(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item && cj_device)
    {
        item->cloud_properties.item_id = EZPI_core_cloud_generate_item_id();
        item->cloud_properties.has_getter = true;
        item->cloud_properties.has_setter = false;
        item->cloud_properties.item_name = ezlopi_item_name_temp;
        item->cloud_properties.show = true;
        item->cloud_properties.value_type = value_type_temperature;

        item->cloud_properties.scale = EZPI_core_setting_get_temperature_scale_str();

        item->is_user_arg_unique = true;
        item->user_arg = user_arg;

        CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_dev_type_str, item->interface_type);

        item->interface.onewire_master.enable = true;
        CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_str, item->interface.onewire_master.onewire_pin);
        ret = EZPI_SUCCESS;
    }

    return ret;
}

static ezlopi_error_t __dht11_setup_item_properties_humidity(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item && cj_device)
    {
        item->cloud_properties.item_id = EZPI_core_cloud_generate_item_id();
        item->cloud_properties.has_getter = true;
        item->cloud_properties.has_setter = false;
        item->cloud_properties.item_name = ezlopi_item_name_humidity;
        item->cloud_properties.show = true;
        item->cloud_properties.value_type = value_type_humidity;
        item->cloud_properties.scale = scales_percent;

        item->is_user_arg_unique = false;
        item->user_arg = user_arg;

        CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_dev_type_str, item->interface_type);

        item->interface.onewire_master.enable = false;
        CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_str, item->interface.onewire_master.onewire_pin);
        ret = EZPI_SUCCESS;
    }
    return ret;
}

static ezlopi_error_t __0015_init(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_ERR_INIT_DEVICE_FAILED;
    if (item && item->interface.onewire_master.enable)
    {
        s_ezlopi_dht11_data_t *dht11_data = (s_ezlopi_dht11_data_t *)item->user_arg;
        if (dht11_data)
        {
            if (GPIO_IS_VALID_GPIO((gpio_num_t)item->interface.onewire_master.onewire_pin))
            {
                // setDHT11gpio(item->interface.onewire_master.onewire_pin);
                ret = EZPI_SUCCESS;
            }
        }
    }
    return ret;
}

static ezlopi_error_t __0015_get_value(l_ezlopi_item_t *item, void *args)
{
    ezlopi_error_t ret = EZPI_SUCCESS;
    cJSON *cj_properties = (cJSON *)args;
    if (item && cj_properties)
    {
        s_ezlopi_dht11_data_t *dht11_data = (s_ezlopi_dht11_data_t *)item->user_arg;
        if (ezlopi_item_name_temp == item->cloud_properties.item_name)
        {
            EZPI_core_valueformatter_float_to_cjson(cj_properties, dht11_data->temperature, scales_celsius);
            ret = EZPI_SUCCESS;
        }
        else if (ezlopi_item_name_humidity == item->cloud_properties.item_name)
        {
            EZPI_core_valueformatter_float_to_cjson(cj_properties, dht11_data->humidity, scales_percent);
            ret = EZPI_SUCCESS;
        }
    }
    return ret;
}

static ezlopi_error_t __0015_notify(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item)
    {
        float temperature, humidity = 0;
        s_ezlopi_dht11_data_t *dht11_data = (s_ezlopi_dht11_data_t *)item->user_arg;
        if (dht11_data && (DHT11_OK == readDHT11(&temperature, &humidity, item->interface.onewire_master.onewire_pin)))
        {
            if (ezlopi_item_name_temp == item->cloud_properties.item_name)
            {
                if (temperature > 15)
                {
                    item->cloud_properties.scale = EZPI_core_setting_get_temperature_scale_str();

                    e_enum_temperature_scale_t scale_to_use = EZPI_core_setting_get_temperature_scale();
                    if (TEMPERATURE_SCALE_FAHRENHEIT == scale_to_use)
                    {
                        temperature = (temperature * (9.0f / 5.0f)) + 32.0f;
                    }

                    if (fabs(dht11_data->temperature - temperature) > 1)
                    {
                        dht11_data->temperature = temperature;
                        EZPI_core_device_value_updated_from_device_broadcast(item);
                    }
                }
            }
            else if (ezlopi_item_name_humidity == item->cloud_properties.item_name)
            {
                if (humidity > 20)
                {
                    if (fabs(dht11_data->humidity - humidity) > 1)
                    {
                        dht11_data->humidity = humidity;
                        EZPI_core_device_value_updated_from_device_broadcast(item);
                    }
                }
            }
        }
    }
    return ret;
}

/*******************************************************************************
 *                          End of File
 *******************************************************************************/