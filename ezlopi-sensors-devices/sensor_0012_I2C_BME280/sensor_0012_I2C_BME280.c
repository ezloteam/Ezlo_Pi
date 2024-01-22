#include <math.h>
#include "stdlib.h"
#include <string.h>
#include "cJSON.h"

#include "trace.h"

#include "ezlopi_core_cloud.h"
#include "ezlopi_core_timer.h"
#include "ezlopi_core_actions.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_core_device_value_updated.h"

#include "sensor_0012_I2C_BME280.h"

typedef struct s_ezlopi_bmp280
{
    float pressure;
    float humidity;
    float temperature;
    bmp280_t bmp280_dev;
    bmp280_params_t bmp280_params;

} s_ezlopi_bmp280_t;

static void __prepare_humidity_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg);
static void __prepare_pressure_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg);
static void __prepare_temperature_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg);
static void __prepare_pressure_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_temp_humid_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);

static int __init(l_ezlopi_item_t *item);
static int __notify(l_ezlopi_item_t *item);
static int __prepare(void *arg);
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
            cJSON_AddStringToObject(cj_device, ezlopi_valueFormatted_str, valueFormatted);
            cJSON_AddNumberToObject(cj_device, ezlopi_value_str, bmp280_sensor_params->temperature);
            cJSON_AddStringToObject(cj_device, ezlopi_scale_str, scales_celsius);
        }

        if (ezlopi_item_name_humidity == item->cloud_properties.item_name)
        {
            snprintf(valueFormatted, 20, "%0.3f", bmp280_sensor_params->humidity);
            cJSON_AddStringToObject(cj_device, ezlopi_valueFormatted_str, valueFormatted);
            cJSON_AddNumberToObject(cj_device, ezlopi_value_str, bmp280_sensor_params->humidity);
            cJSON_AddStringToObject(cj_device, ezlopi_scale_str, scales_percent);
        }

        if (ezlopi_item_name_atmospheric_pressure == item->cloud_properties.item_name)
        {
            snprintf(valueFormatted, 20, "%0.3f", (bmp280_sensor_params->pressure / 1000.0));
            cJSON_AddStringToObject(cj_device, ezlopi_valueFormatted_str, valueFormatted);
            cJSON_AddNumberToObject(cj_device, ezlopi_value_str, (bmp280_sensor_params->pressure / 1000.0));
            cJSON_AddStringToObject(cj_device, ezlopi_scale_str, scales_kilo_pascal);
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
            ret = 1;
        }
    }

    if (0 == ret)
    {
        ret = -1;
        if (item->user_arg)
        {
            free(item->user_arg);
            item->user_arg = NULL;
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
        l_ezlopi_device_t *pressure_deivce = ezlopi_device_add_device(prep_arg->cjson_device);
        l_ezlopi_device_t *temp_humid_device = ezlopi_device_add_device(prep_arg->cjson_device);

        s_ezlopi_bmp280_t *bme280_sensor_params = (s_ezlopi_bmp280_t *)malloc(sizeof(s_ezlopi_bmp280_t));

        if (temp_humid_device && pressure_deivce && bme280_sensor_params)
        {
            memset(bme280_sensor_params, 0, sizeof(s_ezlopi_bmp280_t));

            __prepare_temp_humid_device_cloud_properties(temp_humid_device, prep_arg->cjson_device);

            l_ezlopi_item_t *temperature_item = ezlopi_device_add_item_to_device(temp_humid_device, sensor_0012_I2C_BME280);
            if (temperature_item)
            {
                temperature_item->cloud_properties.device_id = temp_humid_device->cloud_properties.device_id;
                __prepare_temperature_properties(temperature_item, prep_arg->cjson_device, (void *)bme280_sensor_params);
            }
            else
            {
                ret = -1;
            }

            l_ezlopi_item_t *humidity_item = ezlopi_device_add_item_to_device(temp_humid_device, sensor_0012_I2C_BME280);
            if (humidity_item)
            {
                humidity_item->cloud_properties.device_id = temp_humid_device->cloud_properties.device_id;
                __prepare_humidity_properties(humidity_item, prep_arg->cjson_device, (void *)bme280_sensor_params);
            }
            else
            {
                ret = -1;
            }

            __prepare_pressure_device_cloud_properties(pressure_deivce, prep_arg->cjson_device);

            l_ezlopi_item_t *pressure_item = ezlopi_device_add_item_to_device(pressure_deivce, sensor_0012_I2C_BME280);
            if (pressure_item)
            {
                pressure_item->cloud_properties.device_id = pressure_deivce->cloud_properties.device_id;
                __prepare_pressure_properties(pressure_item, prep_arg->cjson_device, (void *)bme280_sensor_params);
            }
            else
            {
                ret = -1;
            }
        }

        if (-1 == ret)
        {
            if (temp_humid_device)
            {
                ezlopi_device_free_device(temp_humid_device);
            }
            if (pressure_deivce)
            {
                ezlopi_device_free_device(pressure_deivce);
            }
            if (bme280_sensor_params)
            {
                free(bme280_sensor_params);
            }
        }
        else
        {
            ret = 1;
        }
    }

    return ret;
}

static void __prepare_temp_humid_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    // char *device_name = NULL;
    // CJSON_GET_VALUE_STRING(cj_device, ezlopi_dev_name_str, device_name);
    // ASSIGN_DEVICE_NAME_V2(device, device_name);
    // device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();

    device->cloud_properties.category = category_level_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_pressure_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    // char *device_name = NULL;
    // CJSON_GET_VALUE_STRING(cj_device, ezlopi_dev_name_str, device_name);
    // ASSIGN_DEVICE_NAME_V2(device, device_name);
    // device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();

    device->cloud_properties.category = category_generic_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_temperature_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg)
{
    CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_type_str, item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.scale = scales_celsius;
    item->cloud_properties.item_name = ezlopi_item_name_temp;
    item->cloud_properties.value_type = value_type_temperature;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    item->interface.i2c_master.enable = true;
    item->interface.i2c_master.clock_speed = 100000;
    CJSON_GET_VALUE_INT(cj_device, ezlopi_gpio_scl_str, item->interface.i2c_master.scl);
    CJSON_GET_VALUE_INT(cj_device, ezlopi_gpio_sda_str, item->interface.i2c_master.sda);
    CJSON_GET_VALUE_INT(cj_device, "slave_addr", item->interface.i2c_master.address);

    item->user_arg = user_arg;
}

static void __prepare_humidity_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg)
{
    CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_type_str, item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.scale = scales_percent;
    item->cloud_properties.item_name = ezlopi_item_name_humidity;
    item->cloud_properties.value_type = value_type_humidity;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    item->interface.i2c_master.enable = false;
    item->interface.i2c_master.clock_speed = 100000;
    CJSON_GET_VALUE_INT(cj_device, ezlopi_gpio_scl_str, item->interface.i2c_master.scl);
    CJSON_GET_VALUE_INT(cj_device, ezlopi_gpio_sda_str, item->interface.i2c_master.sda);
    CJSON_GET_VALUE_INT(cj_device, "slave_addr", item->interface.i2c_master.address);

    item->user_arg = user_arg;
}

static void __prepare_pressure_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg)
{
    CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_type_str, item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.scale = scales_kilo_pascal;
    item->cloud_properties.item_name = ezlopi_item_name_atmospheric_pressure;
    item->cloud_properties.value_type = value_type_pressure;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    item->interface.i2c_master.enable = false;
    item->interface.i2c_master.clock_speed = 100000;
    CJSON_GET_VALUE_INT(cj_device, ezlopi_gpio_scl_str, item->interface.i2c_master.scl);
    CJSON_GET_VALUE_INT(cj_device, ezlopi_gpio_sda_str, item->interface.i2c_master.sda);
    CJSON_GET_VALUE_INT(cj_device, "slave_addr", item->interface.i2c_master.address);

    item->user_arg = user_arg;
}
