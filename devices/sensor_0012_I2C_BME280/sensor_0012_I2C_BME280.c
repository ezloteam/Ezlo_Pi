#include <string.h>
#include "cJSON.h"

#include "trace.h"
#include "ezlopi_cloud.h"
#include "ezlopi_timer.h"
#include "ezlopi_actions.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_valueformatter.h"

#include "stdlib.h"

#include "sensor_0012_I2C_BME280.h"

static bool sensor_bme280_initialized = false;

static int __prepare(void *arg);
static void __prepare_temp_humid_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_pressure_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_temperature_properties(l_ezlopi_item_t *item, cJSON *cj_device);
static void __prepare_humidity_properties(l_ezlopi_item_t *item, cJSON *cj_device);
static void __prepare_pressure_properties(l_ezlopi_item_t *item, cJSON *cj_device);
static int __init(l_ezlopi_item_t *item);
static int __notify(l_ezlopi_item_t *item);
static int __get_cjson_value(l_ezlopi_item_t *item, void *arg);

static int8_t sensor_bme280_read_value_from_sensor(l_ezlopi_item_t *item, sensor_bme280_data_t *data_ptr);
static void user_delay_us(uint32_t period, void *intf_ptr);
static int8_t user_i2c_read(uint8_t reg_addr, uint8_t *sensor_data, uint32_t len, void *intf_ptr);
static int8_t user_i2c_write(uint8_t reg_addr, const uint8_t *sensor_data, uint32_t len, void *intf_ptr);

/**
 * @brief Public function to interface bme280. This is used to handles all the action on the bme280 sensor and is the entry point to interface the sensor.
 *
 * @param action e_ezlopi_actions_t
 * @param arg Other arguments if needed
 * @return int
 */
int sensor_0012_I2C_BME280(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        __prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        __init(item);
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
    return 0;
}

static int __notify(l_ezlopi_item_t *item)
{
    int ret = 0;

    static int update_count = 0;
    if (10 == ++update_count)
    {
        ezlopi_device_value_updated_from_device_v3(item);
        update_count = 0;
    }

    return ret;
}

static int __get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;

    char valueFormatted[20];
    sensor_bme280_data_t sensor_data;
    memset(&sensor_data, 0, sizeof(sensor_bme280_data_t));
    sensor_bme280_read_value_from_sensor(item, &sensor_data);
    cJSON *cj_device = (cJSON *)arg;
    if (cj_device)
    {
        if (ezlopi_item_name_temp == item->cloud_properties.item_name)
        {
            TRACE_E("Temperature is: %f", sensor_data.temperature);
            snprintf(valueFormatted, 20, "%.3f", sensor_data.temperature);
            cJSON_AddStringToObject(cj_device, "valueFormatted", valueFormatted);
            cJSON_AddNumberToObject(cj_device, "value", sensor_data.temperature);
            cJSON_AddStringToObject(cj_device, "scale", "celsius");
        }
        if (ezlopi_item_name_humidity == item->cloud_properties.item_name)
        {
            TRACE_E("Humidity is: %f", sensor_data.humidity);
            snprintf(valueFormatted, 20, "%.3f", sensor_data.humidity);
            cJSON_AddStringToObject(cj_device, "valueFormatted", valueFormatted);
            cJSON_AddNumberToObject(cj_device, "value", sensor_data.humidity);
            cJSON_AddStringToObject(cj_device, "scale", "percent");
        }
        if (ezlopi_item_name_atmospheric_pressure == item->cloud_properties.item_name)
        {
            double pressure = 0.01 * sensor_data.pressure;
            TRACE_E("Pressure is: %f", pressure);
            snprintf(valueFormatted, 20, "%.3f", pressure);
            cJSON_AddStringToObject(cj_device, "valueFormatted", valueFormatted);
            cJSON_AddNumberToObject(cj_device, "value", pressure);
            cJSON_AddStringToObject(cj_device, "scale", "kilo_pascal");
        }
    }

    return ret;
}

static int __init(l_ezlopi_item_t *item)
{
    int ret = 0;

    sensor_bme280_dev_t *sensor_config = (sensor_bme280_dev_t *)item->user_arg;
    if (sensor_config && !sensor_bme280_initialized)
    {
        ezlopi_i2c_master_init(&item->interface.i2c_master);
        uint8_t sampling_settting = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL;
        ret = bme280_init(sensor_config);
        if (ret != BME280_OK)
        {
            TRACE_E("Failed to initialize bme280 (code %+d).", ret);
        }
        else
        {
            TRACE_I("Sensor bme280 was successfully initialized.");
        }

        ret = bme280_set_sensor_settings(sampling_settting, sensor_config);
        if (ret != BME280_OK)
        {
            TRACE_E("Failed to set sensor settings (code %+d).", ret);
        }
        else
        {
            TRACE_I("Sensor setting was successfully set.");
        }
        sensor_bme280_initialized = true;
    }

    return ret;
}

static void __prepare_sensor_config(sensor_bme280_dev_t *sensor_config, cJSON *cjson_device, bme280_identifier_t *bme280_identifier)
{
    sensor_config->read = user_i2c_read;
    sensor_config->write = user_i2c_write;
    sensor_config->delay_us = user_delay_us;
    sensor_config->intf = BME280_I2C_INTF;
    sensor_config->settings.osr_h = BME280_OVERSAMPLING_1X;
    sensor_config->settings.osr_p = BME280_OVERSAMPLING_1X;
    sensor_config->settings.osr_t = BME280_OVERSAMPLING_1X;
    sensor_config->settings.filter = BME280_FILTER_COEFF_16;
    sensor_config->intf_ptr = bme280_identifier;
    CJSON_GET_VALUE_INT(cjson_device, "slave_addr", sensor_config->chip_id);
}

static int __prepare(void *arg)
{
    int ret = 0;

    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (prep_arg && prep_arg->cjson_device)
    {
        l_ezlopi_device_t *temp_humid_device = ezlopi_device_add_device();
        l_ezlopi_device_t *pressure_deivce = ezlopi_device_add_device();
        if (temp_humid_device && pressure_deivce)
        {
            __prepare_temp_humid_device_cloud_properties(temp_humid_device, prep_arg->cjson_device);
            l_ezlopi_item_t *temperature_item = ezlopi_device_add_item_to_device(temp_humid_device, sensor_0012_I2C_BME280);
            if (temperature_item)
            {
                __prepare_temperature_properties(temperature_item, prep_arg->cjson_device);
            }
            l_ezlopi_item_t *humidity_item = ezlopi_device_add_item_to_device(temp_humid_device, sensor_0012_I2C_BME280);
            if (humidity_item)
            {
                __prepare_humidity_properties(humidity_item, prep_arg->cjson_device);
            }
            __prepare_pressure_device_cloud_properties(pressure_deivce, prep_arg->cjson_device);
            l_ezlopi_item_t *pressure_item = ezlopi_device_add_item_to_device(pressure_deivce, sensor_0012_I2C_BME280);
            if (pressure_item)
            {
                __prepare_pressure_properties(pressure_item, prep_arg->cjson_device);
            }
            if ((NULL == temperature_item) && (NULL == humidity_item))
            {
                ezlopi_device_free_device(temp_humid_device);
            }
            else if (NULL == pressure_deivce)
            {
                ezlopi_device_free_device(pressure_deivce);
            }
            else
            {
                sensor_bme280_dev_t *sensor_config = (sensor_bme280_dev_t *)malloc(sizeof(sensor_bme280_dev_t));
                if (sensor_config)
                {
                    bme280_identifier_t *bme280_identifier = (bme280_identifier_t *)malloc(sizeof(bme280_identifier_t));
                    if (bme280_identifier)
                    {
                        bme280_identifier->dev_addr = temperature_item->interface.i2c_master.address;
                        __prepare_sensor_config(sensor_config, prep_arg->cjson_device, bme280_identifier);
                    }
                    temperature_item->user_arg = sensor_config;
                    humidity_item->user_arg = sensor_config;
                    pressure_item->user_arg = sensor_config;
                }
            }
        }
    }

    return ret;
}

static void __prepare_temp_humid_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    char *device_name = NULL;
    CJSON_GET_VALUE_STRING(cj_device, "dev_name", device_name);

    ASSIGN_DEVICE_NAME_V2(device, device_name);
    device->cloud_properties.category = category_level_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
}

static void __prepare_pressure_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    char *device_name = NULL;
    CJSON_GET_VALUE_STRING(cj_device, "dev_name", device_name);

    ASSIGN_DEVICE_NAME_V2(device, device_name);
    device->cloud_properties.category = category_generic_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
}

static void __prepare_temperature_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_temp;
    item->cloud_properties.value_type = value_type_temperature;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    item->interface.i2c_master.enable = true;
    item->interface.i2c_master.clock_speed = 100000;
    CJSON_GET_VALUE_INT(cj_device, "gpio_scl", item->interface.i2c_master.scl);
    CJSON_GET_VALUE_INT(cj_device, "gpio_sda", item->interface.i2c_master.sda);
}

static void __prepare_humidity_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_humidity;
    item->cloud_properties.value_type = value_type_humidity;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    item->interface.i2c_master.enable = true;
    item->interface.i2c_master.clock_speed = 100000;
    CJSON_GET_VALUE_INT(cj_device, "gpio_scl", item->interface.i2c_master.scl);
    CJSON_GET_VALUE_INT(cj_device, "gpio_sda", item->interface.i2c_master.sda);
}

static void __prepare_pressure_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_atmospheric_pressure;
    item->cloud_properties.value_type = value_type_pressure;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    item->interface.i2c_master.enable = true;
    item->interface.i2c_master.clock_speed = 100000;
    CJSON_GET_VALUE_INT(cj_device, "gpio_scl", item->interface.i2c_master.scl);
    CJSON_GET_VALUE_INT(cj_device, "gpio_sda", item->interface.i2c_master.sda);
}

/**
 * @brief static function to read data from the sensor.
 *
 * @return return `0` if everything is successfuly done.
 */
static int8_t sensor_bme280_read_value_from_sensor(l_ezlopi_item_t *item, sensor_bme280_data_t *data_ptr)
{
    int ret = 0;
    sensor_bme280_dev_t *sensor_config = (sensor_bme280_dev_t *)item->user_arg;
    if (sensor_config)
    {
        int8_t ret = bme280_set_sensor_mode(BME280_FORCED_MODE, sensor_config);
        if (ret != BME280_OK)
        {
            TRACE_E("Failed to set sensor mode (code %+d).", ret);
        }
        else
        {
            TRACE_I("Sensor mode set successfully!!");
        }

        ret = bme280_get_sensor_data(BME280_ALL, data_ptr, sensor_config);
        if (ret != BME280_OK)
        {
            TRACE_E("Failed to get sensor data (code %+d).", ret);
        }
        else
        {
            TRACE_I("Sensor mode obtained successfully!!");
        }
    }
    else
    {
        ret = 1;
    }

    return ret;
}

static int8_t user_i2c_read(uint8_t register_addr, uint8_t *data, uint32_t len, void *intf_ptr)
{
    i2c_master_write_read_device(I2C_NUM_0, CHIP_ID, &register_addr, 1, data, len, 1000 / portTICK_RATE_MS);
    return 0;
}

static void user_delay_us(uint32_t period, void *intf_ptr)
{
    vTaskDelay(period / portTICK_PERIOD_MS);
}

static int8_t user_i2c_write(uint8_t register_addr, const uint8_t *data, uint32_t len, void *intf_ptr)
{
    i2c_master_write_read_device(I2C_NUM_0, CHIP_ID, &register_addr, 1, data, len, 1000 / portTICK_RATE_MS);
    return 0;
}
