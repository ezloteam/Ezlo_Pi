

/**
 * @file    ezlopi_service_system_temperature_sensor.c
 * @brief
 * @author
 * @version
 * @date
 */
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

#include "sdkconfig.h"
#if defined(CONFIG_EZPI_ENABLE_SYSTEM_TEMPERATURE)

#include <math.h>

#include "driver/temp_sensor.h"
#include "esp_err.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_core_setting_commands.h"
#include "ezlopi_core_actions.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_cloud_constants.h"

#include "ezlopi_service_system_temperature_sensor.h"

/**
 * @brief Function to prepare temperature sensor related cloud parameters and items
 *
 * @return int
 */
static int ezpi_prepare();
/**
 * @brief Function to initialize and start temperature data
 *
 * @param item Pointer to item linked to temperature sensor
 * @return int
 */
static int ezpi_init(l_ezlopi_item_t *item);
/**
 * @brief Function to populate JSON with temperature data and scale to send to cloud
 *
 * @param item Pointer to item linked to temperature sensor
 * @param arg User argument, contains JSON to be populated
 * @return int
 */
static int ezpi_get_cjson_value(l_ezlopi_item_t *item, void *arg);
/**
 * @brief Function to notify the cloud about the data
 *
 * @param item Pointer to item linked to temperature sensor
 * @return int
 */
static int ezpi_notify(l_ezlopi_item_t *item);
/**
 * @brief Function to prepare cloud properties for provided device
 *
 * @param device Pointer to device
 */
static void ezpi_prepare_device_cloud_properties(l_ezlopi_device_t *device);
/**
 * @brief Function to prepare item properties for provided item
 *
 * @param item pointer to item
 */
static void ezpi_prepare_item_properties(l_ezlopi_item_t *item);

static temp_sensor_config_t device_tempeature_sensor_configuration = TSENS_CONFIG_DEFAULT();
static bool system_temperature_initialized = false;
static float system_temperature = 0.0f;

int EZPI_system_temperature_device(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = ezpi_prepare();
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = ezpi_init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ezpi_get_cjson_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        ezpi_notify(item);
        break;
    }
    default:
    {
        break;
    }
    }
    return ret;
}

static void ezpi_prepare_device_cloud_properties(l_ezlopi_device_t *device)
{
    device->cloud_properties.category = category_temperature;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
    snprintf(device->cloud_properties.device_name, sizeof(device->cloud_properties.device_name), "System temperature");
}

static void ezpi_prepare_item_properties(l_ezlopi_item_t *item)
{
    item->interface_type = EZLOPI_DEVICE_INTERFACE_ANALOG_INPUT;
    item->cloud_properties.show = true;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_temp;
    item->cloud_properties.value_type = value_type_temperature;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    e_enum_temperature_scale_t scale_to_use = ezlopi_core_setting_get_temperature_scale();
    item->cloud_properties.scale = (TEMPERATURE_SCALE_FAHRENHEIT == scale_to_use) ? scales_fahrenheit : scales_celsius;

    item->interface.adc.gpio_num = 0;
    item->interface.adc.resln_bit = 0;
}

static int ezpi_prepare()
{
    int ret = -1;
    l_ezlopi_device_t *device = ezlopi_device_add_device(NULL, NULL);
    if (device)
    {
        ezpi_prepare_device_cloud_properties(device);
        l_ezlopi_item_t *temp_item = ezlopi_device_add_item_to_device(device, EZPI_system_temperature_device);
        if (temp_item)
        {
            ezpi_prepare_item_properties(temp_item);
            ret = 0;
        }
        else
        {
            ezlopi_device_free_device(device);
        }
    }
    return ret;
}

static int ezpi_init(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (!system_temperature_initialized)
    {
        ESP_ERROR_CHECK(temp_sensor_set_config(device_tempeature_sensor_configuration));
        ESP_ERROR_CHECK(temp_sensor_start());
        system_temperature_initialized = true;
    }
    return ret;
}

static int ezpi_get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    int ret = -1;
    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        ezlopi_valueformatter_float_to_cjson(cj_result, system_temperature, item->cloud_properties.scale);
        ret = 0;
    }
    return ret;
}

static int ezpi_notify(l_ezlopi_item_t *item)
{
    int ret = 0;
    float system_temperature_current_value = 0;
    float _eplison = 1.0;
    esp_err_t error = temp_sensor_read_celsius(&system_temperature_current_value);
    if (ESP_OK == error)
    {
        e_enum_temperature_scale_t scale_to_use = ezlopi_core_setting_get_temperature_scale();
        item->cloud_properties.scale = (TEMPERATURE_SCALE_FAHRENHEIT == scale_to_use) ? scales_fahrenheit : scales_celsius;

        if (TEMPERATURE_SCALE_FAHRENHEIT == scale_to_use)
        {
            system_temperature_current_value = (system_temperature_current_value * (9.0f / 5.0f)) + 32.0f;
            _eplison = 2.5;
        }

        if (fabs(system_temperature - system_temperature_current_value) > _eplison)
        {
            system_temperature = system_temperature_current_value;
            ezlopi_device_value_updated_from_device_broadcast(item);
        }
    }
    return ret;
}

#endif // CONFIG_EZPI_ENABLE_SYSTEM_TEMPERATURE

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
