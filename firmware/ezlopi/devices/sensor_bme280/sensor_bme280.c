#include <string.h>

#include "sensor_bme280.h"
#include "ezlopi_actions.h"
// #include "ezlopi_sensors.h"
#include "ezlopi_timer.h"
#include "items.h"
#include "frozen.h"
#include "trace.h"
#include "cJSON.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "ezlopi_device_value_updated.h"
#include <math.h>

static void user_delay_us(uint32_t period, void *intf_ptr);
static int8_t user_i2c_read(uint8_t reg_addr, uint8_t *data, uint32_t len, void *intf_ptr);
static int8_t user_i2c_write(uint8_t reg_addr, const uint8_t *data, uint32_t len, void *intf_ptr);

bme280_identifier_t identifier = {
    .dev_addr = 0x76,
};

static sensor_bme280_dev_t device = {
    .intf = BME280_I2C_INTF,
    .read = user_i2c_read,
    .write = user_i2c_write,
    .delay_us = user_delay_us,
    .settings = {
        .osr_h = BME280_OVERSAMPLING_1X,
        .osr_p = BME280_OVERSAMPLING_1X,
        .osr_t = BME280_OVERSAMPLING_1X,
        .filter = BME280_FILTER_COEFF_16,
    },
    .intf_ptr = &identifier,
};

enum device_feature
{
    TEMPERATURE_FEATURE = 0,
    HUMIDITY_FEATURE = 1,
    PRESSURE_FEATURE = 2
};

typedef enum device_feature device_feature_t;

static int add_multiple_device_features(device_feature_t feature, void *arg);
static int prepare_sensor(void *arg);
static int sensor_bme280_prepare(const char *category, const char *sub_category, const char *item_name, const char *value_type, void *arg);
static int8_t sensor_bme280_get_value(sensor_bme280_data_t *data);
static int sensor_bme280_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args);
static int sensor_bme280_init();
static int add_device_to_list(s_ezlopi_prep_arg_t *prep_arg, s_ezlopi_device_properties_t *sensor_bme_device_properties);

/**
 * @brief Public function to interface bme280. This is used to handles all the action on the bme280 sensor and is the entry point to interface the sensor.
 *
 * @param action e_ezlopi_actions_t
 * @param arg Other arguments if needed
 * @return int
 */
int sensor_bme280(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg)
{
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        prepare_sensor(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        sensor_bme280_init(properties);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        sensor_bme280_get_value_cjson(properties, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        ezlopi_device_value_updated_from_device(properties);
        break;
    }
    default:
    {
        break;
    }
    }
    return 0;
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

static int add_device_to_list(s_ezlopi_prep_arg_t *prep_arg, s_ezlopi_device_properties_t *sensor_bme_device_properties)
{
    int ret = 0;
    if (sensor_bme_device_properties)
    {
        if (0 == ezlopi_devices_list_add(prep_arg->device, sensor_bme_device_properties))
        {
            free(sensor_bme_device_properties);
        }
        else
        {
            ret = 1;
        }
    }
    return ret;
}

static int add_multiple_device_features(device_feature_t feature, void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    cJSON *cjson_device = prep_arg->cjson_device;

    s_ezlopi_device_properties_t *sensor_bme_device_properties = NULL;

    if ((NULL == sensor_bme_device_properties) && (NULL != cjson_device))
    {
        switch (feature)
        {
        case TEMPERATURE_FEATURE:
        {
            TRACE_I("Adding temperature feature.");
            sensor_bme_device_properties = sensor_bme280_prepare(category_temperature, subcategory_not_defined, ezlopi_item_name_temp, value_type_temperature, cjson_device);
            add_device_to_list(prep_arg, sensor_bme_device_properties);
            break;
        }
        case HUMIDITY_FEATURE:
        {
            TRACE_I("Adding humidity feature.");
            sensor_bme_device_properties = sensor_bme280_prepare(category_humidity, subcategory_not_defined, ezlopi_item_name_humidity, value_type_humidity, cjson_device);
            add_device_to_list(prep_arg, sensor_bme_device_properties);
            break;
        }
        case PRESSURE_FEATURE:
        {
            TRACE_I("Adding pressure feature.");
            sensor_bme_device_properties = sensor_bme280_prepare(category_weather, subcategory_not_defined, ezlopi_item_name_atmospheric_pressure, value_type_pressure, cjson_device);
            add_device_to_list(prep_arg, sensor_bme_device_properties);
            break;
        }
        default:
        {
            TRACE_B("Unknown BME280 feature.");
            break;
        }
        }
    }

    return ret;
}

static int prepare_sensor(void *arg)
{
    int ret = 0;
    add_multiple_device_features(TEMPERATURE_FEATURE, arg);
    add_multiple_device_features(HUMIDITY_FEATURE, arg);
    add_multiple_device_features(PRESSURE_FEATURE, arg);
    return ret;
}

static int sensor_bme280_prepare(const char *category, const char *sub_category, const char *item_name, const char *value_type, void *arg)
{
    cJSON *cjson_device = (cJSON *)arg;
    s_ezlopi_device_properties_t *sensor_ble280_properties = NULL;
    if ((NULL == sensor_ble280_properties) && (NULL != cjson_device))
    {
        sensor_ble280_properties = malloc(sizeof(s_ezlopi_device_properties_t));
        if (sensor_ble280_properties)
        {
            int tmp_var = 0;
            memset(sensor_ble280_properties, 0, sizeof(s_ezlopi_device_properties_t));
            sensor_ble280_properties->interface_type = EZLOPI_DEVICE_INTERFACE_I2C_MASTER;

            char *device_name = NULL;
            CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
            ASSIGN_DEVICE_NAME(sensor_ble280_properties, device_name);
            sensor_ble280_properties->ezlopi_cloud.category = category;
            sensor_ble280_properties->ezlopi_cloud.subcategory = sub_category;
            sensor_ble280_properties->ezlopi_cloud.item_name = item_name;
            sensor_ble280_properties->ezlopi_cloud.device_type = dev_type_sensor;
            sensor_ble280_properties->ezlopi_cloud.value_type = value_type;
            sensor_ble280_properties->ezlopi_cloud.has_getter = true;
            sensor_ble280_properties->ezlopi_cloud.has_setter = false;
            sensor_ble280_properties->ezlopi_cloud.reachable = true;
            sensor_ble280_properties->ezlopi_cloud.battery_powered = false;
            sensor_ble280_properties->ezlopi_cloud.show = true;
            sensor_ble280_properties->ezlopi_cloud.room_name[0] = '\0';
            sensor_ble280_properties->ezlopi_cloud.device_id = ezlopi_device_generate_device_id();
            sensor_ble280_properties->ezlopi_cloud.room_id = ezlopi_device_generate_room_id();
            sensor_ble280_properties->ezlopi_cloud.item_id = ezlopi_device_generate_item_id();

            CJSON_GET_VALUE_INT(cjson_device, "gpio_scl", sensor_ble280_properties->interface.i2c_master.scl);
            CJSON_GET_VALUE_INT(cjson_device, "gpio_sda", sensor_ble280_properties->interface.i2c_master.sda);

            sensor_ble280_properties->interface.i2c_master.enable = true;
            sensor_ble280_properties->interface.i2c_master.clock_speed = 100000;
            sensor_ble280_properties->interface.i2c_master.channel = EZLOPI_I2C_1;
        }
    }

    return ((int)sensor_ble280_properties);
}

/**
 * @brief static function to read data from the sensor.
 *
 * @return return `0` if everything is successfuly done.
 */
static int8_t sensor_bme280_get_value(sensor_bme280_data_t *data)
{
    int ret = bme280_set_sensor_mode(BME280_FORCED_MODE, &device);
    int data_len = 100;
    if (ret != BME280_OK)
    {
        TRACE_E("Failed to set sensor mode (code %+d).", ret);
    }
    else
    {
        TRACE_I("Sensor mode set successfully!!");
    }

    ret = bme280_get_sensor_data(BME280_ALL, data, &device);
    if (ret != BME280_OK)
    {
        TRACE_E("Failed to get sensor data (code %+d).", ret);
    }
    else
    {
        TRACE_I("Sensor mode obtained successfully!!");
        data->pressure = 0.01 * data->pressure;
    }
    return data;
}

/**
 * @brief Static function to initialize the bme280 sensor.
 *
 * @return returns 0 for successful initialization.
 */
static int sensor_bme280_init(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;
    ezlopi_i2c_master_init(&properties->interface.i2c_master);
    TRACE_I("I2C master init successfully.");
    uint8_t sampling_settting = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL;

    ret = bme280_init(&device);
    if (ret != BME280_OK)
    {
        TRACE_E("Failed to initialize bme280 (code %+d).", ret);
    }
    else
    {
        TRACE_I("Sensor bme280 was successfully initialized.");
    }

    ret = bme280_set_sensor_settings(sampling_settting, &device);
    if (ret != BME280_OK)
    {
        TRACE_E("Failed to set sensor settings (code %+d).", ret);
    }
    else
    {
        TRACE_I("Sensor setting was successfully set.");
    }
    return ret;
}

static int sensor_bme280_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args)
{
    int ret = 0;
    sensor_bme280_data_t *data = (sensor_bme280_data_t *)malloc(sizeof(sensor_bme280_data_t));
    memset(data, 0, sizeof(sensor_bme280_data_t));
    sensor_bme280_get_value(data);
    cJSON *cjson_propertise = (cJSON *)args;
    if (cjson_propertise)
    {
        if (category_temperature == properties->ezlopi_cloud.category)
        {
            TRACE_E("Temperature is: %f", data->temperature);
            cJSON_AddNumberToObject(cjson_propertise, "value", data->temperature);
            cJSON_AddStringToObject(cjson_propertise, "scale", "celsius");
        }
        if (category_humidity == properties->ezlopi_cloud.category)
        {
            TRACE_E("Humidity is: %f", data->humidity);
            cJSON_AddNumberToObject(cjson_propertise, "value", data->humidity);
            cJSON_AddStringToObject(cjson_propertise, "scale", "percent");
        }
        if (category_not_defined == properties->ezlopi_cloud.category)
        {
            TRACE_E("Pressure is: %f", data->pressure);
            cJSON_AddNumberToObject(cjson_propertise, "value", data->pressure);
            cJSON_AddStringToObject(cjson_propertise, "scale", "kilo_pascal");
        }

        ret = 1;
    }

    free(data);
    return ret;
}
