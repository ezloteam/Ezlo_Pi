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
* @file    sensor_0012_I2C_BME280.c
* @brief   perform some function on sensor_0012
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

#include "ezlopi_hal_i2c_master.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "sensor_0012_I2C_BME280.h"
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
typedef struct s_ezlopi_bmp280
{
    float pressure;
    float humidity;
    float temperature;
    bmp280_t bmp280_dev;
    bmp280_params_t bmp280_params;

} s_ezlopi_bmp280_t;

/*******************************************************************************
*                          Static Function Prototypes
*******************************************************************************/
static void __prepare_item_humidity_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg);
static void __prepare_item_pressure_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg);
static void __prepare_item_temperature_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg);
static void __prepare_device_cloud_properties_child_pressure(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_device_cloud_properties_parent_temp_humid(l_ezlopi_device_t *device, cJSON *cj_device);

static ezlopi_error_t __init(l_ezlopi_item_t *item);
static ezlopi_error_t __notify(l_ezlopi_item_t *item);
static ezlopi_error_t __prepare(void *arg);
static ezlopi_error_t __get_cjson_value(l_ezlopi_item_t *item, void *arg);

/*******************************************************************************
*                          Static Data Definitions
*******************************************************************************/

/*******************************************************************************
*                          Extern Data Definitions
*******************************************************************************/

/*******************************************************************************
*                          Extern Function Definitions
*******************************************************************************/

ezlopi_error_t SENSOR_0012_i2c_bme280(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
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
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = __get_cjson_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        ret = __notify(item);
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

static bool check_double_val_equal(double first, double second)
{
    bool ret = true;

    // TRACE_I("==> %f", fabs((first - second)));
    if (fabs((first - second)) > 0.001)
    {
        ret = false;
    }
    else
    {
        ret = true;
    }
    return ret;
}

static ezlopi_error_t __notify(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item)
    {
        s_ezlopi_bmp280_t *sensor_params = (s_ezlopi_bmp280_t *)item->user_arg;
        if (sensor_params)
        {
            float temperature, pressure, humidity;
            bool update_cloud = false;
            if (ESP_OK == bmp280_read_float(&item->interface.i2c_master, &sensor_params->bmp280_dev, &temperature, &pressure, &humidity))
            {
                if (ezlopi_item_name_temp == item->cloud_properties.item_name)
                {
                    item->cloud_properties.scale = EZPI_core_setting_get_temperature_scale_str();

                    e_enum_temperature_scale_t scale_to_use = EZPI_core_setting_get_temperature_scale();
                    if (TEMPERATURE_SCALE_FAHRENHEIT == scale_to_use)
                    {
                        temperature = (temperature * (9.0f / 5.0f)) + 32.0f;
                    }
                    update_cloud = (!check_double_val_equal(sensor_params->temperature, temperature) ? true : false);
                }
                if (ezlopi_item_name_humidity == item->cloud_properties.item_name)
                {
                    update_cloud = (!check_double_val_equal(sensor_params->humidity, humidity) ? true : false);
                }
                if (ezlopi_item_name_atmospheric_pressure == item->cloud_properties.item_name)
                {
                    update_cloud = (!check_double_val_equal(sensor_params->pressure, pressure) ? true : false);
                }
            }
            if (update_cloud)
            {
                sensor_params->temperature = temperature;
                sensor_params->humidity = humidity;
                sensor_params->pressure = pressure;
                EZPI_core_device_value_updated_from_device_broadcast(item);
                update_cloud = false;
                ret = EZPI_SUCCESS;
            }
        }
    }

    return ret;
}

static ezlopi_error_t __get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item && arg)
    {
        cJSON *cj_device = (cJSON *)arg;
        s_ezlopi_bmp280_t *bmp280_sensor_params = (s_ezlopi_bmp280_t *)item->user_arg;
        if (cj_device && bmp280_sensor_params)
        {
            if (ezlopi_item_name_temp == item->cloud_properties.item_name)
            {
                EZPI_core_valueformatter_float_to_cjson(cj_device, bmp280_sensor_params->temperature, scales_celsius);
                ret = EZPI_SUCCESS;
            }
            else if (ezlopi_item_name_humidity == item->cloud_properties.item_name)
            {
                EZPI_core_valueformatter_float_to_cjson(cj_device, bmp280_sensor_params->humidity, scales_percent);
                ret = EZPI_SUCCESS;
            }
            else if (ezlopi_item_name_atmospheric_pressure == item->cloud_properties.item_name)
            {
                EZPI_core_valueformatter_float_to_cjson(cj_device, (bmp280_sensor_params->pressure / 1000.0), scales_kilo_pascal);
                ret = EZPI_SUCCESS;
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
        s_ezlopi_bmp280_t *bmp280_sensor_params = (s_ezlopi_bmp280_t *)item->user_arg;
        if (bmp280_sensor_params)
        {
            if (item->interface.i2c_master.enable)
            {
                EZPI_hal_i2c_master_init(&item->interface.i2c_master);
                bmp280_init_default_params(&bmp280_sensor_params->bmp280_params);
                bmp280_init(&bmp280_sensor_params->bmp280_dev, &bmp280_sensor_params->bmp280_params, &item->interface.i2c_master);
                bmp280_read_float(&item->interface.i2c_master, &bmp280_sensor_params->bmp280_dev, &bmp280_sensor_params->temperature, &bmp280_sensor_params->pressure, &bmp280_sensor_params->humidity);
                ret = EZPI_SUCCESS;
            }
        }
    }

    return ret;
}

static ezlopi_error_t __prepare(void *arg)
{
    ezlopi_error_t ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;

    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (prep_arg && prep_arg->cjson_device)
    {
        s_ezlopi_bmp280_t *bme280_sensor_params = (s_ezlopi_bmp280_t *)ezlopi_malloc(__FUNCTION__, sizeof(s_ezlopi_bmp280_t));
        if (bme280_sensor_params)
        {
            memset(bme280_sensor_params, 0, sizeof(s_ezlopi_bmp280_t));
            l_ezlopi_device_t *parent_temp_humid_device = EZPI_core_device_add_device(prep_arg->cjson_device, "temp_hum");
            if (parent_temp_humid_device)
            {
                TRACE_I("Parent_temp_humid_device-[0x%x] ", parent_temp_humid_device->cloud_properties.device_id);
                __prepare_device_cloud_properties_parent_temp_humid(parent_temp_humid_device, prep_arg->cjson_device);

                //------------------------------------------------------------------------
                l_ezlopi_item_t *temperature_item = EZPI_core_device_add_item_to_device(parent_temp_humid_device, SENSOR_0012_i2c_bme280);
                if (temperature_item)
                {
                    __prepare_item_temperature_properties(temperature_item, prep_arg->cjson_device, (void *)bme280_sensor_params);
                }

                l_ezlopi_item_t *humidity_item = EZPI_core_device_add_item_to_device(parent_temp_humid_device, SENSOR_0012_i2c_bme280);
                if (humidity_item)
                {
                    __prepare_item_humidity_properties(humidity_item, prep_arg->cjson_device, (void *)bme280_sensor_params);
                }
                //------------------------------------------------------------------------

                l_ezlopi_device_t *child_pressure_device = EZPI_core_device_add_device(prep_arg->cjson_device, "pressure");
                if (child_pressure_device)
                {
                    TRACE_I("Child_pressure_device-[0x%x] ", child_pressure_device->cloud_properties.device_id);
                    __prepare_device_cloud_properties_child_pressure(child_pressure_device, prep_arg->cjson_device);

                    l_ezlopi_item_t *pressure_item = EZPI_core_device_add_item_to_device(child_pressure_device, SENSOR_0012_i2c_bme280);
                    if (pressure_item)
                    {
                        __prepare_item_pressure_properties(pressure_item, prep_arg->cjson_device, (void *)bme280_sensor_params);
                    }
                    else
                    {
                        EZPI_core_device_free_device(child_pressure_device);
                    }
                }

                if ((NULL == temperature_item) &&
                    (NULL == humidity_item) &&
                    (NULL == child_pressure_device))
                {
                    ezlopi_free(__FUNCTION__, bme280_sensor_params);
                    EZPI_core_device_free_device(parent_temp_humid_device);
                }
                else
                {
                    ret = EZPI_SUCCESS;
                }
            }
            else
            {
                ezlopi_free(__FUNCTION__, bme280_sensor_params);
            }
        }
    }

    return ret;
}

static void __prepare_device_cloud_properties_parent_temp_humid(l_ezlopi_device_t *device, cJSON *cj_device)
{
    device->cloud_properties.category = category_temperature;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_device_cloud_properties_child_pressure(l_ezlopi_device_t *device, cJSON *cj_device)
{
    device->cloud_properties.category = category_level_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_item_temperature_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg)
{
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_dev_type_str, item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;

    item->cloud_properties.scale = EZPI_core_setting_get_temperature_scale_str();

    item->cloud_properties.item_name = ezlopi_item_name_temp;
    item->cloud_properties.value_type = value_type_temperature;
    item->cloud_properties.item_id = EZPI_core_cloud_generate_item_id();

    item->interface.i2c_master.enable = true;
    item->interface.i2c_master.channel = I2C_NUM_0;
    item->interface.i2c_master.clock_speed = 100000;
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_scl_str, item->interface.i2c_master.scl);
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_sda_str, item->interface.i2c_master.sda);
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_slave_addr_str, item->interface.i2c_master.address);

    item->is_user_arg_unique = true;
    item->user_arg = user_arg;
}

static void __prepare_item_humidity_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg)
{
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_dev_type_str, item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.scale = scales_percent;
    item->cloud_properties.item_name = ezlopi_item_name_humidity;
    item->cloud_properties.value_type = value_type_humidity;
    item->cloud_properties.item_id = EZPI_core_cloud_generate_item_id();

    item->interface.i2c_master.enable = false;
    item->interface.i2c_master.clock_speed = 100000;
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_scl_str, item->interface.i2c_master.scl);
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_sda_str, item->interface.i2c_master.sda);
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_slave_addr_str, item->interface.i2c_master.address);

    item->is_user_arg_unique = true;
    item->user_arg = user_arg;
}

static void __prepare_item_pressure_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg)
{
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_dev_type_str, item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.scale = scales_kilo_pascal;
    item->cloud_properties.item_name = ezlopi_item_name_atmospheric_pressure;
    item->cloud_properties.value_type = value_type_pressure;
    item->cloud_properties.item_id = EZPI_core_cloud_generate_item_id();

    item->interface.i2c_master.enable = false;
    item->interface.i2c_master.clock_speed = 100000;
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_scl_str, item->interface.i2c_master.scl);
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_sda_str, item->interface.i2c_master.sda);
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_slave_addr_str, item->interface.i2c_master.address);

    item->is_user_arg_unique = false;
    item->user_arg = user_arg;
}

/*******************************************************************************
*                          End of File
*******************************************************************************/