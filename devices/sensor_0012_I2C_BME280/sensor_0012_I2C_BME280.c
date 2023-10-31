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
#include "ezlopi_cloud_scales_str.h"
#include "math.h"
#include "stdlib.h"

#include "sensor_0012_I2C_BME280.h"

static int __prepare(void *arg);
static void __prepare_temp_humid_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_pressure_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_temperature_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg);
static void __prepare_humidity_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg);
static void __prepare_pressure_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg);
static int __init(l_ezlopi_item_t *item);
static int __notify(l_ezlopi_item_t *item);
static int __get_cjson_value(l_ezlopi_item_t *item, void *arg);

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

static bool check_double_val_equal(double first, double second)
{
    bool ret = true;

    // TRACE_B("==> %f", fabs((first - second)));
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
static int __notify(l_ezlopi_item_t *item)
{
    int ret = 0;

    s_ezlopi_bmp280_t *sensor_params = (s_ezlopi_bmp280_t *)item->user_arg;
    float temperature, pressure, humidity;
    bool update_cloud = false;
    bmp280_read_float(&item->interface.i2c_master, &sensor_params->bmp280_dev, &temperature, &pressure, &humidity);
    // TRACE_B("Temp: %f, Humidity: %f, pressure: %f", temperature, humidity, pressure);

    if (ezlopi_item_name_temp == item->cloud_properties.item_name)
    {
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
    if (update_cloud)
    {
        sensor_params->temperature = temperature;
        sensor_params->humidity = humidity;
        sensor_params->pressure = pressure;
        ezlopi_device_value_updated_from_device_v3(item);
        update_cloud = false;
    }

    // static int update_count = 0;
    // if (5 == ++update_count)
    // {
    //     update_count = 0;
    //     s_ezlopi_bmp280_t *bmp280_sensor_params = (s_ezlopi_bmp280_t *)item->user_arg;
    //     bmp280_read_float(&item->interface.i2c_master, &bmp280_sensor_params->bmp280_dev, &bmp280_sensor_params->temperature, &bmp280_sensor_params->pressure, &bmp280_sensor_params->humidity);
    //     ezlopi_device_value_updated_from_device_v3(item);
    // }

    return ret;
}

static int __get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;

    char valueFormatted[20];
    cJSON *cj_device = (cJSON *)arg;
    s_ezlopi_bmp280_t *bmp280_sensor_params = (s_ezlopi_bmp280_t *)item->user_arg;
    if (cj_device && bmp280_sensor_params)
    {
        if (ezlopi_item_name_temp == item->cloud_properties.item_name)
        {
            snprintf(valueFormatted, 20, "%0.3f", bmp280_sensor_params->temperature);
            cJSON_AddStringToObject(cj_device, "valueFormatted", valueFormatted);
            cJSON_AddNumberToObject(cj_device, "value", bmp280_sensor_params->temperature);
        }
        if (ezlopi_item_name_humidity == item->cloud_properties.item_name)
        {
            snprintf(valueFormatted, 20, "%0.3f", bmp280_sensor_params->humidity);
            cJSON_AddStringToObject(cj_device, "valueFormatted", valueFormatted);
            cJSON_AddNumberToObject(cj_device, "value", bmp280_sensor_params->humidity);
        }
        if (ezlopi_item_name_atmospheric_pressure == item->cloud_properties.item_name)
        {
            snprintf(valueFormatted, 20, "%0.3f", (bmp280_sensor_params->pressure / 1000.0));
            cJSON_AddStringToObject(cj_device, "valueFormatted", valueFormatted);
            cJSON_AddNumberToObject(cj_device, "value", (bmp280_sensor_params->pressure / 1000.0));
        }
    }

    return ret;
}

static int __init(l_ezlopi_item_t *item)
{
    int ret = 0;

    if (item->interface.i2c_master.enable)
    {
        s_ezlopi_bmp280_t *bmp280_sensor_params = (s_ezlopi_bmp280_t *)item->user_arg;
        if (bmp280_sensor_params)
        {
            ezlopi_i2c_master_init(&item->interface.i2c_master);
            bmp280_init_default_params(&bmp280_sensor_params->bmp280_params);
            bmp280_init(&bmp280_sensor_params->bmp280_dev, &bmp280_sensor_params->bmp280_params, &item->interface.i2c_master);
            bmp280_read_float(&item->interface.i2c_master, &bmp280_sensor_params->bmp280_dev, &bmp280_sensor_params->temperature, &bmp280_sensor_params->pressure, &bmp280_sensor_params->humidity);
        }
    }

    return ret;
}

static int __prepare(void *arg)
{
    int ret = 0;

    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (prep_arg && prep_arg->cjson_device)
    {
        l_ezlopi_device_t *temp_humid_device = ezlopi_device_add_device();
        l_ezlopi_device_t *pressure_deivce = ezlopi_device_add_device();
        s_ezlopi_bmp280_t *bme280_sensor_params = (s_ezlopi_bmp280_t *)malloc(sizeof(s_ezlopi_bmp280_t));
        if (temp_humid_device && pressure_deivce && bme280_sensor_params)
        {
            memset(bme280_sensor_params, 0, sizeof(s_ezlopi_bmp280_t));
            __prepare_temp_humid_device_cloud_properties(temp_humid_device, prep_arg->cjson_device);
            l_ezlopi_item_t *temperature_item = ezlopi_device_add_item_to_device(temp_humid_device, sensor_0012_I2C_BME280);
            if (temperature_item)
            {
                __prepare_temperature_properties(temperature_item, prep_arg->cjson_device, (void *)bme280_sensor_params);
            }
            l_ezlopi_item_t *humidity_item = ezlopi_device_add_item_to_device(temp_humid_device, sensor_0012_I2C_BME280);
            if (humidity_item)
            {
                __prepare_humidity_properties(humidity_item, prep_arg->cjson_device, (void *)bme280_sensor_params);
            }
            __prepare_pressure_device_cloud_properties(pressure_deivce, prep_arg->cjson_device);
            l_ezlopi_item_t *pressure_item = ezlopi_device_add_item_to_device(pressure_deivce, sensor_0012_I2C_BME280);
            if (pressure_item)
            {
                __prepare_pressure_properties(pressure_item, prep_arg->cjson_device, (void *)bme280_sensor_params);
            }
            if ((NULL == temperature_item) && (NULL == humidity_item))
            {
                ezlopi_device_free_device(temp_humid_device);
            }
            if (NULL == pressure_deivce)
            {
                ezlopi_device_free_device(pressure_deivce);
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

static void __prepare_temperature_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg)
{
    CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.scale = scales_celsius;
    item->cloud_properties.item_name = ezlopi_item_name_temp;
    item->cloud_properties.value_type = value_type_temperature;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    item->interface.i2c_master.enable = true;
    item->interface.i2c_master.clock_speed = 100000;
    CJSON_GET_VALUE_INT(cj_device, "gpio_scl", item->interface.i2c_master.scl);
    CJSON_GET_VALUE_INT(cj_device, "gpio_sda", item->interface.i2c_master.sda);
    CJSON_GET_VALUE_INT(cj_device, "slave_addr", item->interface.i2c_master.address);

    item->user_arg = user_arg;
}

static void __prepare_humidity_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg)
{
    CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.scale = scales_percent;
    item->cloud_properties.item_name = ezlopi_item_name_humidity;
    item->cloud_properties.value_type = value_type_humidity;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    item->interface.i2c_master.enable = false;
    item->interface.i2c_master.clock_speed = 100000;
    CJSON_GET_VALUE_INT(cj_device, "gpio_scl", item->interface.i2c_master.scl);
    CJSON_GET_VALUE_INT(cj_device, "gpio_sda", item->interface.i2c_master.sda);
    CJSON_GET_VALUE_INT(cj_device, "slave_addr", item->interface.i2c_master.address);

    item->user_arg = user_arg;
}

static void __prepare_pressure_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg)
{
    CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.scale = scales_kilo_pascal;
    item->cloud_properties.item_name = ezlopi_item_name_atmospheric_pressure;
    item->cloud_properties.value_type = value_type_pressure;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    item->interface.i2c_master.enable = false;
    item->interface.i2c_master.clock_speed = 100000;
    CJSON_GET_VALUE_INT(cj_device, "gpio_scl", item->interface.i2c_master.scl);
    CJSON_GET_VALUE_INT(cj_device, "gpio_sda", item->interface.i2c_master.sda);
    CJSON_GET_VALUE_INT(cj_device, "slave_addr", item->interface.i2c_master.address);

    item->user_arg = user_arg;
}
