#include <math.h>
#include "ezlopi_util_trace.h"

#include "ezlopi_core_timer.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "ds18b20_onewire.h"
#include "sensor_0030_oneWire_DS18B20.h"

static int __prepare(void* arg);
static int __init(l_ezlopi_item_t* item);
static int __notify(l_ezlopi_item_t* item);
static int __get_cjson_value(l_ezlopi_item_t* item, void* arg);

static esp_err_t ds18b20_write_data(uint8_t* data, uint32_t gpio_pin);
static esp_err_t ds18b20_read_data(uint8_t* data, uint32_t gpio_pin);
static bool ds18b20_reset_line(uint32_t gpio_pin);
static esp_err_t ds18b20_write_to_scratchpad(uint8_t th_val, uint8_t tl_val, uint8_t resolution, uint8_t gpio_pin);
static bool ds18b20_recognize_device(uint32_t gpio_pin);
static esp_err_t ds18b20_get_temperature_data(double* temperature_data, uint32_t gpio_pin);
static uint8_t ds18b20_calculate_crc(const uint8_t* data, uint8_t len);

int sensor_0030_oneWire_DS18B20(e_ezlopi_actions_t action, l_ezlopi_item_t* item, void* arg, void* user_arg)
{
    int ret = 0;
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

static int __notify(l_ezlopi_item_t* item)
{
    int ret = 0;
    double* temperature_prev_value = (double*)item->user_arg;
    double temperature_current_value = 0.00;
    esp_err_t error = ds18b20_get_temperature_data(&temperature_current_value, item->interface.onewire_master.onewire_pin);
    if (ESP_OK == error)
    {
        // TRACE_I("Current %f, prev %f", temperature_current_value, *temperature_prev_value);
        // TRACE_I("Diff is %f", fabs(*temperature_prev_value - temperature_current_value));
        if (fabs(*temperature_prev_value - temperature_current_value) > 0.2)
        {
            *temperature_prev_value = temperature_current_value;
            ezlopi_device_value_updated_from_device_v3(item);
        }
    }
    return ret;
}

static int __get_cjson_value(l_ezlopi_item_t* item, void* arg)
{
    int ret = 0;

    if (item && arg)
    {
        cJSON* cj_result = (cJSON*)arg;
        double* temperatue_value = (double*)item->user_arg;
        cJSON_AddNumberToObject(cj_result, ezlopi_value_str, *temperatue_value);
        char* valueFormatted = ezlopi_valueformatter_double(*temperatue_value);
        cJSON_AddStringToObject(cj_result, ezlopi_valueFormatted_str, valueFormatted);
        free(valueFormatted);
        cJSON_AddStringToObject(cj_result, ezlopi_scale_str, scales_celsius);
    }
    return ret;
}

static int __init(l_ezlopi_item_t* item)
{
    int ret = -1;
    if ((item) && (item->interface.onewire_master.enable))
    {
        if (GPIO_IS_VALID_GPIO(item->interface.onewire_master.onewire_pin) &&
            ds18b20_reset_line(item->interface.onewire_master.onewire_pin))
        {
            if (ds18b20_recognize_device(item->interface.onewire_master.onewire_pin))
            {
                double* temperature_prev_value = (double*)item->user_arg;
                TRACE_D("Providing initial settings to DS18B20");
                ds18b20_write_to_scratchpad(DS18B20_TH_HIGHER_THRESHOLD, DS18B20_TL_LOWER_THRESHOLD, 12, item->interface.onewire_master.onewire_pin);
                ds18b20_get_temperature_data(temperature_prev_value, item->interface.onewire_master.onewire_pin);
                ret = 1;
            }
            else
            {
                ret = -1;
            }
        }
        else
        {
            ret = -1;
        }
    }
    return ret;
}

static void __prepare_device_cloud_properties(l_ezlopi_device_t* device, cJSON* cj_device)
{
    // char *device_name = NULL;
    // CJSON_GET_VALUE_STRING(cj_device, ezlopi_dev_name_str, device_name);
    // ASSIGN_DEVICE_NAME_V2(device, device_name);
    // device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();

    device->cloud_properties.category = category_temperature;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_item_properties(l_ezlopi_item_t* item, cJSON* cj_device)
{
    CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_type_str, item->interface_type);
    item->cloud_properties.show = true;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_temp;
    item->cloud_properties.value_type = value_type_temperature;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.scale = scales_celsius;
    item->interface_type = EZLOPI_DEVICE_INTERFACE_ONEWIRE_MASTER;

    item->interface.onewire_master.enable = true;
    CJSON_GET_VALUE_INT(cj_device, ezlopi_gpio_str, item->interface.onewire_master.onewire_pin);
}

static int __prepare(void* arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t* prep_arg = (s_ezlopi_prep_arg_t*)arg;

    if (prep_arg && prep_arg->cjson_device)
    {
        l_ezlopi_device_t* device = ezlopi_device_add_device(prep_arg->cjson_device);
        if (device)
        {
            __prepare_device_cloud_properties(device, prep_arg->cjson_device);
            l_ezlopi_item_t* item_temperature = ezlopi_device_add_item_to_device(device, sensor_0030_oneWire_DS18B20);
            if (item_temperature)
            {
                __prepare_item_properties(item_temperature, prep_arg->cjson_device);

                double* temperature_value = (double*)malloc(sizeof(double));
                if (temperature_value)
                {
                    memset(temperature_value, 0, sizeof(double));
                    *temperature_value = 65536.0f;
                    item_temperature->user_arg = (void*)temperature_value;
                }
                ret = 1;
            }
            else
            {
                ezlopi_device_free_device(device);
                ret = -1;
            }
        }
        else
        {
            ret = -1;
        }
    }

    return ret;
}

static esp_err_t ds18b20_write_data(uint8_t* data, uint32_t gpio_pin)
{
    esp_err_t error = ESP_OK;
    error = one_wire_write_byte_to_line(data, gpio_pin);
    return error;
}

static esp_err_t ds18b20_read_data(uint8_t* data, uint32_t gpio_pin)
{
    esp_err_t error = ESP_OK;
    error = one_wire_read_byte_from_line(data, gpio_pin);
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
    present = one_wire_reset_line(gpio_pin);
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

static esp_err_t ds18b20_get_temperature_data(double* temperature_data, uint32_t gpio_pin)
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

static uint8_t ds18b20_calculate_crc(const uint8_t* data, uint8_t len)
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
