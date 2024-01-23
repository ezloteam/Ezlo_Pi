#include <math.h>
#include "trace.h"
// #include "cJSON.h"

#include "ezlopi_core_timer.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_hal_i2c_master.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "bme680_bsec.h"
#include "sensor_0010_I2C_BME680.h"

static int __prepare(void *arg);
static int __init(l_ezlopi_item_t *item);
static int __notify(l_ezlopi_item_t *item);
static int __get_cjson_value(l_ezlopi_item_t *item, void *arg);

static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg);

int sensor_0010_I2C_BME680(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
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

static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    char *device_name = NULL;
    CJSON_GET_VALUE_STRING(cj_device, "dev_name", device_name);

    ASSIGN_DEVICE_NAME_V2(device, device_name);
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg)
{
    CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type);
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.show = true;
    item->user_arg = user_arg;

    if (ezlopi_item_name_temp == item->cloud_properties.item_name)
    {
        item->interface.i2c_master.enable = true;
        item->interface.i2c_master.clock_speed = 100000;
        CJSON_GET_VALUE_INT(cj_device, "gpio_scl", item->interface.i2c_master.scl);
        CJSON_GET_VALUE_INT(cj_device, "gpio_sda", item->interface.i2c_master.sda);
        CJSON_GET_VALUE_INT(cj_device, "slave_addr", item->interface.i2c_master.address);
    }
    else
    {
        item->interface.i2c_master.enable = false;
    }
}

static int __prepare(void *arg)
{
    int ret = 0;

    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (prep_arg && prep_arg->cjson_device)
    {
        cJSON *cj_device = prep_arg->cjson_device;
        bme680_data_t *user_data = (bme680_data_t *)malloc(sizeof(bme680_data_t));
        if (user_data)
        {
            memset(user_data, 0, sizeof(bme680_data_t));
            l_ezlopi_device_t *temp_humid_device = ezlopi_device_add_device(cj_device);
            if (temp_humid_device)
            {
                temp_humid_device->cloud_properties.category = category_temperature;
                __prepare_device_cloud_properties(temp_humid_device, cj_device);
                l_ezlopi_item_t *temperature_item = ezlopi_device_add_item_to_device(temp_humid_device, sensor_0010_I2C_BME680);
                if (temperature_item)
                {
                    temperature_item->cloud_properties.item_name = ezlopi_item_name_temp;
                    temperature_item->cloud_properties.value_type = value_type_temperature;
                    temperature_item->cloud_properties.scale = scales_celsius;
                    __prepare_cloud_properties(temperature_item, cj_device, user_data);
                }
                l_ezlopi_item_t *humidity_item = ezlopi_device_add_item_to_device(temp_humid_device, sensor_0010_I2C_BME680);
                if (humidity_item)
                {
                    humidity_item->cloud_properties.item_name = ezlopi_item_name_humidity;
                    humidity_item->cloud_properties.value_type = value_type_humidity;
                    humidity_item->cloud_properties.scale = scales_percent;
                    __prepare_cloud_properties(humidity_item, cj_device, user_data);
                }
                if ((NULL == temperature_item) && (NULL == humidity_item))
                {
                    ezlopi_device_free_device(temp_humid_device);
                }
            }
            else
            {
                ezlopi_device_free_device(temp_humid_device);
            }
            l_ezlopi_device_t *pressure_device = ezlopi_device_add_device(cj_device);
            if (pressure_device)
            {
                pressure_device->cloud_properties.category = category_level_sensor;
                __prepare_device_cloud_properties(pressure_device, cj_device);
                l_ezlopi_item_t *pressure_item = ezlopi_device_add_item_to_device(pressure_device, sensor_0010_I2C_BME680);
                if (pressure_item)
                {
                    pressure_item->cloud_properties.item_name = ezlopi_item_name_atmospheric_pressure;
                    pressure_item->cloud_properties.value_type = value_type_pressure;
                    pressure_item->cloud_properties.scale = scales_kilo_pascal;
                    __prepare_cloud_properties(pressure_item, cj_device, user_data);
                }
                else
                {
                    ezlopi_device_free_device(pressure_device);
                }
            }
            else
            {
                ezlopi_device_free_device(pressure_device);
            }
            l_ezlopi_device_t *aqi_device = ezlopi_device_add_device(cj_device);
            if (aqi_device)
            {
                aqi_device->cloud_properties.category = category_level_sensor;
                __prepare_device_cloud_properties(aqi_device, cj_device);
                l_ezlopi_item_t *aqi_item = ezlopi_device_add_item_to_device(aqi_device, sensor_0010_I2C_BME680);
                if (aqi_item)
                {
                    aqi_item->cloud_properties.item_name = ezlopi_item_name_volatile_organic_compound_level;
                    aqi_item->cloud_properties.value_type = value_type_substance_amount;
                    aqi_item->cloud_properties.scale = scales_parts_per_million;
                    __prepare_cloud_properties(aqi_item, cj_device, user_data);
                }
                else
                {
                    ezlopi_device_free_device(aqi_device);
                }
            }
            else
            {
                ezlopi_device_free_device(aqi_device);
            }
            l_ezlopi_device_t *altitude_device = ezlopi_device_add_device(cj_device);
            if (altitude_device)
            {
                altitude_device->cloud_properties.category = category_level_sensor;
                __prepare_device_cloud_properties(altitude_device, cj_device);
                l_ezlopi_item_t *altitude_item = ezlopi_device_add_item_to_device(altitude_device, sensor_0010_I2C_BME680);
                if (altitude_item)
                {
                    altitude_item->cloud_properties.item_name = ezlopi_item_name_distance;
                    altitude_item->cloud_properties.value_type = value_type_length;
                    altitude_item->cloud_properties.scale = scales_meter;
                    __prepare_cloud_properties(altitude_item, cj_device, user_data);
                }
                else
                {
                    ezlopi_device_free_device(altitude_device);
                }
            }
            else
            {
                ezlopi_device_free_device(altitude_device);
            }
            l_ezlopi_device_t *co2_device = ezlopi_device_add_device(cj_device);
            if (co2_device)
            {
                co2_device->cloud_properties.category = category_level_sensor;
                __prepare_device_cloud_properties(co2_device, cj_device);
                l_ezlopi_item_t *co2_item = ezlopi_device_add_item_to_device(co2_device, sensor_0010_I2C_BME680);
                if (co2_item)
                {
                    co2_item->cloud_properties.item_name = ezlopi_item_name_co2_level;
                    co2_item->cloud_properties.value_type = value_type_substance_amount;
                    co2_item->cloud_properties.scale = scales_parts_per_million;
                    __prepare_cloud_properties(co2_item, cj_device, user_data);
                }
                else
                {
                    ezlopi_device_free_device(co2_device);
                }
            }
            else
            {
                ezlopi_device_free_device(co2_device);
            }

            if (NULL == pressure_device)
            {
                ezlopi_device_free_device(pressure_device);
            }
        }
    }

    return ret;
}

static int __init(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (item)
    {
        if (true == item->interface.i2c_master.enable)
        {
            ezlopi_i2c_master_init(&item->interface.i2c_master);
            bme680_setup(item->interface.i2c_master.sda, item->interface.i2c_master.scl, true);
        }
        ret = 1;
    }
    return ret;
}

static int __get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;
    cJSON *cj_device = (cJSON *)arg;
    if (cj_device && item)
    {
        bme680_data_t *user_data = (bme680_data_t *)item->user_arg;
        if (ezlopi_item_name_temp == item->cloud_properties.item_name)
        {
            char *valueFormatted = ezlopi_valueformatter_float(user_data->temperature);
            cJSON_AddStringToObject(cj_device, "valueFormatted", valueFormatted);
            free(valueFormatted);
            cJSON_AddNumberToObject(cj_device, "value", user_data->temperature);
        }
        if (ezlopi_item_name_humidity == item->cloud_properties.item_name)
        {
            char *valueFormatted = ezlopi_valueformatter_float(user_data->humidity);
            cJSON_AddStringToObject(cj_device, "valueFormatted", valueFormatted);
            free(valueFormatted);
            cJSON_AddNumberToObject(cj_device, "value", user_data->humidity);
        }
        if (ezlopi_item_name_atmospheric_pressure == item->cloud_properties.item_name)
        {
            char *valueFormatted = ezlopi_valueformatter_float(user_data->pressure / 1000.0);
            cJSON_AddStringToObject(cj_device, "valueFormatted", valueFormatted);
            free(valueFormatted);
            cJSON_AddNumberToObject(cj_device, "value", (user_data->pressure / 1000.0));
        }
        if (ezlopi_item_name_volatile_organic_compound_level == item->cloud_properties.item_name)
        {
            char *valueFormatted = ezlopi_valueformatter_float(user_data->iaq);
            cJSON_AddStringToObject(cj_device, "valueFormatted", valueFormatted);
            free(valueFormatted);
            cJSON_AddNumberToObject(cj_device, "value", (user_data->iaq));
        }
        if (ezlopi_item_name_distance == item->cloud_properties.item_name)
        {
            char *valueFormatted = ezlopi_valueformatter_float(user_data->altitude);
            cJSON_AddStringToObject(cj_device, "valueFormatted", valueFormatted);
            free(valueFormatted);
            cJSON_AddNumberToObject(cj_device, "value", (user_data->altitude));
        }
        if (ezlopi_item_name_co2_level == item->cloud_properties.item_name)
        {
            char *valueFormatted = ezlopi_valueformatter_float(user_data->co2_equivalent);
            cJSON_AddStringToObject(cj_device, "valueFormatted", valueFormatted);
            free(valueFormatted);
            cJSON_AddNumberToObject(cj_device, "value", (user_data->co2_equivalent));
        }
    }

    return ret;
}

static int __notify(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (item)
    {
        bme680_data_t *user_data = (bme680_data_t *)item->user_arg;
        float temperature = user_data->temperature;
        float humidity = user_data->humidity;
        float pressure = user_data->pressure;
        float iaq = user_data->iaq;
        float altitude = user_data->altitude;
        float co2_eqv = user_data->co2_equivalent;

        bme680_get_data(user_data);

        if (ezlopi_item_name_temp == item->cloud_properties.item_name)
        {
            if (fabs(user_data->temperature - temperature) > 0.05)
            {
                user_data->temperature = temperature;
                ezlopi_device_value_updated_from_device_v3(item);
            }
        }
        if (ezlopi_item_name_humidity == item->cloud_properties.item_name)
        {
            if (fabs(user_data->humidity - humidity) > 0.05)
            {
                user_data->humidity = humidity;
                ezlopi_device_value_updated_from_device_v3(item);
            }
        }
        if (ezlopi_item_name_atmospheric_pressure == item->cloud_properties.item_name)
        {
            if (fabs((user_data->pressure / 1000.0f) - (pressure / 1000.0f)) > 0.05)
            {
                user_data->pressure = pressure;
                ezlopi_device_value_updated_from_device_v3(item);
            }
        }
        if (ezlopi_item_name_volatile_organic_compound_level == item->cloud_properties.item_name)
        {
            if (fabs(user_data->iaq - iaq) > 0.05)
            {
                user_data->iaq = iaq;
                ezlopi_device_value_updated_from_device_v3(item);
            }
        }
        if (ezlopi_item_name_distance == item->cloud_properties.item_name)
        {
            if (fabs(user_data->altitude - altitude) > 0.05)
            {
                user_data->altitude = altitude;
                ezlopi_device_value_updated_from_device_v3(item);
            }
        }
        if (ezlopi_item_name_co2_level == item->cloud_properties.item_name)
        {
            if (fabs(user_data->co2_equivalent - co2_eqv) > 0.05)
            {
                user_data->co2_equivalent = co2_eqv;
                ezlopi_device_value_updated_from_device_v3(item);
            }
        }
    }

    return ret;
}