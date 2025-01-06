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
 * @file    sensor_0030_oneWire_DS18B20.c
 * @brief   perform some function on sensor_0030
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

#include "ds18b20_onewire.h"
#include "sensor_0030_oneWire_DS18B20.h"
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
static ezlopi_error_t __prepare(void *arg);
static ezlopi_error_t __init(l_ezlopi_item_t *item);
static ezlopi_error_t __notify(l_ezlopi_item_t *item);
static ezlopi_error_t __get_cjson_value(l_ezlopi_item_t *item, void *arg);

static esp_err_t ds18b20_write_data(uint8_t *data, uint32_t gpio_pin);
static esp_err_t ds18b20_read_data(uint8_t *data, uint32_t gpio_pin);
static bool ds18b20_reset_line(uint32_t gpio_pin);
static esp_err_t ds18b20_write_to_scratchpad(uint8_t th_val, uint8_t tl_val, uint8_t resolution, uint8_t gpio_pin);
static bool ds18b20_recognize_device(uint32_t gpio_pin);
static esp_err_t ds18b20_get_temperature_data(double *temperature_data, uint32_t gpio_pin);
static uint8_t ds18b20_calculate_crc(const uint8_t *data, uint8_t len);

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
ezlopi_error_t SENSOR_0030_oneWire_DS18B20(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
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
        __get_cjson_value(item, arg);
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
static ezlopi_error_t __notify(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_FAILED;
    double *temperature_prev_value = (double *)item->user_arg;
    double temperature_current_value = 0.00;
    esp_err_t error = ds18b20_get_temperature_data(&temperature_current_value, item->interface.onewire_master.onewire_pin);
    if (ESP_OK == error)
    {
        // TRACE_I("Current %f, prev %f", temperature_current_value, *temperature_prev_value);
        // TRACE_I("Diff is %f", fabs(*temperature_prev_value - temperature_current_value));
        item->cloud_properties.scale = EZPI_core_setting_get_temperature_scale_str();

        e_enum_temperature_scale_t scale_to_use = EZPI_core_setting_get_temperature_scale();
        if (TEMPERATURE_SCALE_FAHRENHEIT == scale_to_use)
        {
            temperature_current_value = (temperature_current_value * (9.0f / 5.0f)) + 32.0f;
        }

        if (fabs(*temperature_prev_value - temperature_current_value) > 0.2)
        {
            *temperature_prev_value = temperature_current_value;
            EZPI_core_device_value_updated_from_device_broadcast(item);
            ret = EZPI_SUCCESS;
        }
    }
    return ret;
}

static ezlopi_error_t __get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_FAILED;

    if (item && arg && item->user_arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        double *temperatue_value = (double *)item->user_arg;
        EZPI_core_valueformatter_double_to_cjson(cj_result, *temperatue_value, scales_celsius);
        ret = EZPI_SUCCESS;
    }
    return ret;
}

static ezlopi_error_t __init(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_ERR_INIT_DEVICE_FAILED;
    if ((item) && (item->interface.onewire_master.enable))
    {
        if (GPIO_IS_VALID_GPIO(item->interface.onewire_master.onewire_pin) &&
            ds18b20_reset_line(item->interface.onewire_master.onewire_pin))
        {
            if (ds18b20_recognize_device(item->interface.onewire_master.onewire_pin))
            {
                double *temperature_prev_value = (double *)item->user_arg;
                TRACE_D("Providing initial settings to DS18B20");
                ds18b20_write_to_scratchpad(DS18B20_TH_HIGHER_THRESHOLD, DS18B20_TL_LOWER_THRESHOLD, 12, item->interface.onewire_master.onewire_pin);
                ds18b20_get_temperature_data(temperature_prev_value, item->interface.onewire_master.onewire_pin);
                ret = EZPI_SUCCESS;
            }
        }
    }
    return ret;
}

static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    device->cloud_properties.category = category_temperature;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_item_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_dev_type_str, item->interface_type);
    item->cloud_properties.show = true;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_temp;
    item->cloud_properties.value_type = value_type_temperature;
    item->cloud_properties.item_id = EZPI_core_cloud_generate_item_id();

    item->cloud_properties.scale = EZPI_core_setting_get_temperature_scale_str();

    item->interface_type = EZLOPI_DEVICE_INTERFACE_ONEWIRE_MASTER;

    item->interface.onewire_master.enable = true;
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_str, item->interface.onewire_master.onewire_pin);
}

static ezlopi_error_t __prepare(void *arg)
{
    ezlopi_error_t ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;

    if (prep_arg && prep_arg->cjson_device)
    {
        l_ezlopi_device_t *device = EZPI_core_device_add_device(prep_arg->cjson_device, NULL);
        if (device)
        {
            __prepare_device_cloud_properties(device, prep_arg->cjson_device);
            l_ezlopi_item_t *item_temperature = EZPI_core_device_add_item_to_device(device, SENSOR_0030_oneWire_DS18B20);
            if (item_temperature)
            {
                __prepare_item_properties(item_temperature, prep_arg->cjson_device);

                double *temperature_value = (double *)ezlopi_malloc(__FUNCTION__, sizeof(double));
                if (temperature_value)
                {
                    memset(temperature_value, 0, sizeof(double));
                    *temperature_value = 65536.0f;
                    item_temperature->is_user_arg_unique = true;
                    item_temperature->user_arg = (void *)temperature_value;
                    ret = EZPI_SUCCESS;
                }
            }
            else
            {
                EZPI_core_device_free_device(device);
            }
        }
    }

    return ret;
}

static esp_err_t ds18b20_write_data(uint8_t *data, uint32_t gpio_pin)
{
    esp_err_t error = ESP_OK;
    error = DS18B20_write_byte_to_line(data, gpio_pin);
    return error;
}

static esp_err_t ds18b20_read_data(uint8_t *data, uint32_t gpio_pin)
{
    esp_err_t error = ESP_OK;
    error = DS18B20_read_byte_from_line(data, gpio_pin);
    return error;
}

static esp_err_t ds18b20_write_to_scratchpad(uint8_t th_val, uint8_t tl_val, uint8_t resolution, uint8_t gpio_pin)
{
    esp_err_t error = ESP_OK;
    uint8_t ds18b20_skip_rom = DS18B20_ROM_COMMAND_SKIP_ROM;
    uint8_t ds18b20_write_scratchpad = DS18B20_FUNCTION_COMMAND_WRITE_SCRATCHPAD;
    uint8_t ds18b20_th_val = th_val;
    uint8_t ds18b20_tl_vla = tl_val;
    uint8_t ds18b20_resolution = 0;

    switch (resolution)
    {
    case 9:
    {
        ds18b20_resolution = DS18B20_TEMPERATURE_9_BIT_RESOLUTION;
        break;
    }
    case 10:
    {
        ds18b20_resolution = DS18B20_TEMPERATURE_10_BIT_RESOLUTION;
        break;
    }
    case 11:
    {
        ds18b20_resolution = DS18B20_TEMPERATURE_11_BIT_RESOLUTION;
        break;
    }
    case 12:
    default:
    {
        ds18b20_resolution = DS18B20_TEMPERATURE_12_BIT_RESOLUTION;
        break;
    }
    }

    if (ds18b20_reset_line(gpio_pin))
    {
        ds18b20_write_data(&ds18b20_skip_rom, gpio_pin);
        // write to the scratch pad. (TH, TL, Configuration) = (42, -10, 12-bit resolution.)
        ds18b20_write_data(&ds18b20_write_scratchpad, gpio_pin);
        ds18b20_write_data(&ds18b20_th_val, gpio_pin);
        ds18b20_write_data(&ds18b20_tl_vla, gpio_pin);
        ds18b20_write_data(&ds18b20_resolution, gpio_pin);
    }
    else
    {
        error = ESP_FAIL;
    }
    return error;
}

static bool ds18b20_reset_line(uint32_t gpio_pin)
{
    bool present = false;
    present = DS18B20_reset_line(gpio_pin);
    return present;
}

static bool ds18b20_recognize_device(uint32_t gpio_pin)
{
    uint8_t read_rom_command = 0x33;
    ds18b20_write_data(&read_rom_command, gpio_pin);
    uint8_t data_from_ds18b20 = 0;
    ds18b20_read_data(&data_from_ds18b20, gpio_pin);
    return (data_from_ds18b20 == DS18B20_FAMILY_CODE) ? true : false;
}

static esp_err_t ds18b20_get_temperature_data(double *temperature_data, uint32_t gpio_pin)
{
    esp_err_t error = ESP_OK;
    uint8_t ds18b20_skip_rom = DS18B20_ROM_COMMAND_SKIP_ROM;
    uint8_t ds18b20_convert_temperature = DS18B20_FUNCTION_COMMAND_CONVERT_TEMP;
    uint8_t ds18b20_read_scratchpad = DS18B20_FUNCTION_COMMAND_READ_SCRATCHPAD;

    uint8_t ds18b20_temperature_lsb = 0;
    uint8_t ds18b20_temperature_msb = 0;
    uint8_t ds18b20_th_value = 0;
    uint8_t ds18b20_tl_data = 0;
    uint8_t ds18b20_config_data = 0;
    uint8_t ds18b20_reserved_reg_1 = 0;
    uint8_t ds18b20_reserved_reg_2 = 0;
    uint8_t ds18b20_reserved_reg_3 = 0;
    uint8_t ds18b20_crc = 0;

    if (ds18b20_reset_line(gpio_pin))
    {
        ds18b20_write_data(&ds18b20_skip_rom, gpio_pin);
        ds18b20_write_data(&ds18b20_convert_temperature, gpio_pin);
        vTaskDelay(750 / portTICK_RATE_MS);
        ds18b20_reset_line(gpio_pin);
        ds18b20_write_data(&ds18b20_skip_rom, gpio_pin);
        ds18b20_write_data(&ds18b20_read_scratchpad, gpio_pin);

        // Now read the data;
        ds18b20_read_data(&ds18b20_temperature_lsb, gpio_pin);
        ds18b20_read_data(&ds18b20_temperature_msb, gpio_pin);
        ds18b20_read_data(&ds18b20_th_value, gpio_pin);
        ds18b20_read_data(&ds18b20_tl_data, gpio_pin);
        ds18b20_read_data(&ds18b20_config_data, gpio_pin);
        ds18b20_read_data(&ds18b20_reserved_reg_1, gpio_pin);
        ds18b20_read_data(&ds18b20_reserved_reg_2, gpio_pin);
        ds18b20_read_data(&ds18b20_reserved_reg_3, gpio_pin);
        ds18b20_read_data(&ds18b20_crc, gpio_pin);

        uint8_t ds18b20_scratchpad_data_array[8] = {
            ds18b20_temperature_lsb,
            ds18b20_temperature_msb,
            ds18b20_th_value,
            ds18b20_tl_data,
            ds18b20_config_data,
            ds18b20_reserved_reg_1,
            ds18b20_reserved_reg_2,
            ds18b20_reserved_reg_3,
        };

        uint8_t calculated_crc = ds18b20_calculate_crc(ds18b20_scratchpad_data_array, 8);
        if (calculated_crc != ds18b20_crc)
        {
            TRACE_E("CRC check failed!!");
            error = ESP_FAIL;
        }
        else
        {
            double temp = 0;
            temp = (double)(ds18b20_temperature_lsb + (ds18b20_temperature_msb * 256.0)) / 16.0;
            *temperature_data = temp;
        }
    }
    else
    {
        error = ESP_FAIL;
    }
    return error;
}

static uint8_t ds18b20_calculate_crc(const uint8_t *data, uint8_t len)
{
    uint8_t crc = 0;
    uint8_t length = len;
    while (length--)
    {
        crc = *data++ ^ crc;
        crc = pgm_read_byte(dscrc2x16_table + (crc & 0x0f)) ^ pgm_read_byte(dscrc2x16_table + 16 + ((crc >> 4) & 0x0f));
    }
    return crc;
}

/*******************************************************************************
 *                          End of File
 *******************************************************************************/