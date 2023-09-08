
#include <string.h>
#include "sdkconfig.h"

#include "cJSON.h"
#include "trace.h"
#include "ezlopi_timer.h"
#include "items.h"

#include "ezlopi_cloud.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_devices.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_i2c_master.h"
#include "ezlopi_spi_master.h"

#include "0010_I2C_BME680.h"
#include "bme680_bsec.h"

static bool sensor_0010_I2C_BME680_i2c_initialized = false;

static int __prepare(void *arg);
static int __init(l_ezlopi_item_t *item);
static int __notify(l_ezlopi_item_t *item);
static int __get_cjson_value(l_ezlopi_item_t *item, void *arg);

int sensor_0010_I2C_BME680_v3(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    int ret = 0;
    // TRACE_B("Action is %s", ezlopi_actions_to_string(action));
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

static int __get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;

    cJSON *cjson_properties = (cJSON *)arg;
    char formatted_value[20];
    bme680_data_t *sensor_0010_I2C_BME680_data = (bme680_data_t *)item->user_arg;

    if (cjson_properties && sensor_0010_I2C_BME680_data)
    {
        if (ezlopi_item_name_temp == item->cloud_properties.item_name)
        {
            snprintf(formatted_value, 20, "%.2f", sensor_0010_I2C_BME680_data->temperature);
            cJSON_AddStringToObject(cjson_properties, "valueFormatted", formatted_value);
            cJSON_AddNumberToObject(cjson_properties, "value", sensor_0010_I2C_BME680_data->temperature);
        }
        if (ezlopi_item_name_humidity == item->cloud_properties.item_name)
        {
            snprintf(formatted_value, 20, "%.2f", sensor_0010_I2C_BME680_data->humidity);
            cJSON_AddStringToObject(cjson_properties, "valueFormatted", formatted_value);
            cJSON_AddNumberToObject(cjson_properties, "value", sensor_0010_I2C_BME680_data->humidity);
        }
        if (ezlopi_item_name_atmospheric_pressure == item->cloud_properties.item_name)
        {
            sensor_0010_I2C_BME680_data->pressure = sensor_0010_I2C_BME680_data->pressure / 1000.0;
            snprintf(formatted_value, 20, "%.2f", sensor_0010_I2C_BME680_data->pressure);
            cJSON_AddStringToObject(cjson_properties, "valueFormatted", formatted_value);
            cJSON_AddNumberToObject(cjson_properties, "value", (int)(sensor_0010_I2C_BME680_data->pressure));
        }
        if (ezlopi_item_name_volatile_organic_compound_level == item->cloud_properties.item_name)
        {
            snprintf(formatted_value, 20, "%.2f", sensor_0010_I2C_BME680_data->iaq);
            TRACE_I("ezlopi_item_name_volatile_organic_compound_level: %s", formatted_value);
            cJSON_AddStringToObject(cjson_properties, "valueFormatted", formatted_value);
            cJSON_AddNumberToObject(cjson_properties, "value", (uint16_t)sensor_0010_I2C_BME680_data->iaq);
        }
        if (ezlopi_item_name_distance == item->cloud_properties.item_name)
        {
            snprintf(formatted_value, 20, "%.2f", sensor_0010_I2C_BME680_data->altitude);
            cJSON_AddStringToObject(cjson_properties, "valueFormatted", formatted_value);
            cJSON_AddNumberToObject(cjson_properties, "value", (int)sensor_0010_I2C_BME680_data->altitude);
        }

        if (ezlopi_item_name_co2_level == item->cloud_properties.item_name)
        {
            snprintf(formatted_value, 20, "%.2f", sensor_0010_I2C_BME680_data->co2_equivalent);
            cJSON_AddStringToObject(cjson_properties, "valueFormatted", formatted_value);
            cJSON_AddNumberToObject(cjson_properties, "value", (uint16_t)sensor_0010_I2C_BME680_data->co2_equivalent);
        }
    }

    return ret;
}

static void sensor_0010_I2C_BME680_update_values(l_ezlopi_item_t *item)
{
    bme680_data_t *sensor_0010_I2C_BME680_data = (bme680_data_t *)item->user_arg;
    if (NULL != sensor_0010_I2C_BME680_data)
    {
        bool val = bme680_get_data(sensor_0010_I2C_BME680_data);
        if (val)
        {
            // TRACE_B("Value is %d", val);
            TRACE_B("---------------------------------------");
            TRACE_B("Air Quality Index : %f", sensor_0010_I2C_BME680_data->iaq);
            TRACE_B("AIQ accuracy : %s", sensor_0010_I2C_BME680_data->iaq_accuracy ? "TRUE" : "FALSE");
            TRACE_B("co2_equivalent : %f", sensor_0010_I2C_BME680_data->co2_equivalent);
            TRACE_B("voc_equivalent : %f", sensor_0010_I2C_BME680_data->voc_equivalent);
            TRACE_B("Temperature : %f", sensor_0010_I2C_BME680_data->temperature);
            TRACE_B("Pressure : %f", sensor_0010_I2C_BME680_data->pressure);
            TRACE_B("Humidity : %f", sensor_0010_I2C_BME680_data->humidity);
            TRACE_B("Gas resistance : %f", sensor_0010_I2C_BME680_data->gas_resistance);
            TRACE_B("Altitude : %f", sensor_0010_I2C_BME680_data->altitude);
            TRACE_B("Stabilization_status : %s", sensor_0010_I2C_BME680_data->stabilization_status ? "TRUE" : "FALSE");
            TRACE_B("run in status : %s", sensor_0010_I2C_BME680_data->run_in_status ? "TRUE" : "FALSE");
            TRACE_B("---------------------------------------");
        }
    }
}

static int __notify(l_ezlopi_item_t *item)
{
    int ret = 0;
    static int count = 0;
    if (5 == ++count)
    {
        sensor_0010_I2C_BME680_update_values(item);
        ezlopi_device_value_updated_from_device_v3(item);
        count = 0;
    }
    return ret;
}

static int __init(l_ezlopi_item_t *item)
{
    int ret = 0;

    if ((!sensor_0010_I2C_BME680_i2c_initialized))
    {
        bme680_setup(item->interface.i2c_master.sda, item->interface.i2c_master.scl, true);
        sensor_0010_I2C_BME680_i2c_initialized = true;
    }
    return ret;
}

static void __prepare_temperature_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg)
{
    CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type);
    item->cloud_properties.item_name = ezlopi_item_name_temp;
    item->cloud_properties.value_type = value_type_temperature;
    item->cloud_properties.scale = scales_celsius;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.show = true;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    CJSON_GET_VALUE_INT(cj_device, "gpio_sda", item->interface.i2c_master.sda);
    CJSON_GET_VALUE_INT(cj_device, "gpio_scl", item->interface.i2c_master.scl);
    item->interface.i2c_master.clock_speed = 100000;
    item->interface.i2c_master.address = 0x77;
    item->interface.i2c_master.enable = true;
    item->interface.i2c_master.channel = I2C_NUM_0;
    item->user_arg = user_arg;
}

static void __prepare_humidity_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg)
{
    CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type);
    item->cloud_properties.item_name = ezlopi_item_name_humidity;
    item->cloud_properties.value_type = value_type_humidity;
    item->cloud_properties.scale = scales_percent;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.show = true;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    CJSON_GET_VALUE_INT(cj_device, "gpio_sda", item->interface.i2c_master.sda);
    CJSON_GET_VALUE_INT(cj_device, "gpio_scl", item->interface.i2c_master.scl);
    item->interface.i2c_master.clock_speed = 100000;
    item->interface.i2c_master.address = 0x77;
    item->interface.i2c_master.enable = true;
    item->interface.i2c_master.channel = I2C_NUM_0;
    item->user_arg = user_arg;
}

static void __prepare_pressure_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg)
{
    CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type);
    item->cloud_properties.item_name = ezlopi_item_name_atmospheric_pressure;
    item->cloud_properties.value_type = value_type_int;
    item->cloud_properties.scale = scales_kilo_pascal;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.show = true;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    CJSON_GET_VALUE_INT(cj_device, "gpio_sda", item->interface.i2c_master.sda);
    CJSON_GET_VALUE_INT(cj_device, "gpio_scl", item->interface.i2c_master.scl);
    item->interface.i2c_master.clock_speed = 100000;
    item->interface.i2c_master.address = 0x77;
    item->interface.i2c_master.enable = true;
    item->interface.i2c_master.channel = I2C_NUM_0;
    item->user_arg = user_arg;
}

static void __prepare_AIQ_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg)
{
    CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type);
    item->cloud_properties.item_name = ezlopi_item_name_volatile_organic_compound_level;
    item->cloud_properties.value_type = value_type_substance_amount;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.show = true;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    CJSON_GET_VALUE_INT(cj_device, "gpio_sda", item->interface.i2c_master.sda);
    CJSON_GET_VALUE_INT(cj_device, "gpio_scl", item->interface.i2c_master.scl);
    item->interface.i2c_master.clock_speed = 100000;
    item->interface.i2c_master.address = 0x77;
    item->interface.i2c_master.enable = true;
    item->interface.i2c_master.channel = I2C_NUM_0;
    item->user_arg = user_arg;
}

static void __prepare_altitude_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg)
{
    CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type);
    item->cloud_properties.item_name = ezlopi_item_name_distance;
    item->cloud_properties.value_type = value_type_length;
    item->cloud_properties.scale = scales_meter;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.show = true;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    CJSON_GET_VALUE_INT(cj_device, "gpio_sda", item->interface.i2c_master.sda);
    CJSON_GET_VALUE_INT(cj_device, "gpio_scl", item->interface.i2c_master.scl);
    item->interface.i2c_master.clock_speed = 100000;
    item->interface.i2c_master.address = 0x77;
    item->interface.i2c_master.enable = true;
    item->interface.i2c_master.channel = I2C_NUM_0;
    item->user_arg = user_arg;
}

static void __prepare_CO2_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg)
{
    CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type);
    item->cloud_properties.item_name = ezlopi_item_name_co2_level;
    item->cloud_properties.value_type = value_type_substance_amount;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.show = true;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    CJSON_GET_VALUE_INT(cj_device, "gpio_sda", item->interface.i2c_master.sda);
    CJSON_GET_VALUE_INT(cj_device, "gpio_scl", item->interface.i2c_master.scl);
    item->interface.i2c_master.clock_speed = 100000;
    item->interface.i2c_master.address = 0x77;
    item->interface.i2c_master.enable = true;
    item->interface.i2c_master.channel = I2C_NUM_0;
    item->user_arg = user_arg;
}

static void __prepare_temp_humid_device(cJSON *cj_device, void *user_arg)
{
    l_ezlopi_device_t *temp_humid_device = ezlopi_device_add_device();
    if (temp_humid_device)
    {
        char *device_name = "Temp/Humid";
        // CJSON_GET_VALUE_STRING(cj_device, "dev_name", device_name);
        ASSIGN_DEVICE_NAME_V2(temp_humid_device, device_name);
        temp_humid_device->cloud_properties.category = category_level_sensor;
        temp_humid_device->cloud_properties.subcategory = subcategory_not_defined;
        temp_humid_device->cloud_properties.device_type = dev_type_sensor;
        temp_humid_device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();

        l_ezlopi_item_t *temp_item = ezlopi_device_add_item_to_device(temp_humid_device, sensor_0010_I2C_BME680_v3);
        if (temp_item)
        {
            __prepare_temperature_item_cloud_properties(temp_item, cj_device, user_arg);
        }
        l_ezlopi_item_t *humidity_item = ezlopi_device_add_item_to_device(temp_humid_device, sensor_0010_I2C_BME680_v3);
        if (humidity_item)
        {
            __prepare_humidity_item_cloud_properties(humidity_item, cj_device, user_arg);
        }
        if ((NULL == temp_item) && (NULL == humidity_item))
        {
            ezlopi_device_free_device(temp_humid_device);
        }
    }
}

static void __prepare_pressure_device(cJSON *cj_device, void *user_arg)
{
    l_ezlopi_device_t *pressure_device = ezlopi_device_add_device();
    if (pressure_device)
    {
        char *device_name = "Pressure";
        // CJSON_GET_VALUE_STRING(cj_device, "dev_name", device_name);
        ASSIGN_DEVICE_NAME_V2(pressure_device, device_name);
        pressure_device->cloud_properties.category = category_generic_sensor;
        pressure_device->cloud_properties.subcategory = subcategory_not_defined;
        pressure_device->cloud_properties.device_type = dev_type_sensor;
        pressure_device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();

        l_ezlopi_item_t *pressure_item = ezlopi_device_add_item_to_device(pressure_device, sensor_0010_I2C_BME680_v3);
        if (pressure_item)
        {
            __prepare_pressure_item_cloud_properties(pressure_item, cj_device, user_arg);
        }
        if (NULL == pressure_item)
        {
            ezlopi_device_free_device(pressure_device);
        }
    }
}

static void __prepare_AIQ_device(cJSON *cj_device, void *user_arg)
{
    l_ezlopi_device_t *AIQ_device = ezlopi_device_add_device();
    if (AIQ_device)
    {
        char *device_name = "AIQ";
        // CJSON_GET_VALUE_STRING(cj_device, "dev_name", device_name);
        ASSIGN_DEVICE_NAME_V2(AIQ_device, device_name);
        AIQ_device->cloud_properties.category = category_generic_sensor;
        AIQ_device->cloud_properties.subcategory = subcategory_not_defined;
        AIQ_device->cloud_properties.device_type = dev_type_sensor;
        AIQ_device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();

        l_ezlopi_item_t *AIQ_item = ezlopi_device_add_item_to_device(AIQ_device, sensor_0010_I2C_BME680_v3);
        if (AIQ_item)
        {
            __prepare_AIQ_item_cloud_properties(AIQ_item, cj_device, user_arg);
        }
        if (NULL == AIQ_item)
        {
            ezlopi_device_free_device(AIQ_device);
        }
    }
}

static void __prepare_altitude_device(cJSON *cj_device, void *user_arg)
{
    l_ezlopi_device_t *altitude_device = ezlopi_device_add_device();
    if (altitude_device)
    {
        char *device_name = "Altitude";
        // CJSON_GET_VALUE_STRING(cj_device, "dev_name", device_name);
        ASSIGN_DEVICE_NAME_V2(altitude_device, device_name);
        altitude_device->cloud_properties.category = category_generic_sensor;
        altitude_device->cloud_properties.subcategory = subcategory_not_defined;
        altitude_device->cloud_properties.device_type = dev_type_sensor;
        altitude_device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();

        l_ezlopi_item_t *altitude_item = ezlopi_device_add_item_to_device(altitude_device, sensor_0010_I2C_BME680_v3);
        if (altitude_item)
        {
            __prepare_altitude_item_cloud_properties(altitude_item, cj_device, user_arg);
        }
        if (NULL == altitude_item)
        {
            ezlopi_device_free_device(altitude_device);
        }
    }
}

static void __prepare_CO2_device(cJSON *cj_device, void *user_arg)
{
    l_ezlopi_device_t *CO2_device = ezlopi_device_add_device();
    if (CO2_device)
    {
        char *device_name = "CO2";
        // CJSON_GET_VALUE_STRING(cj_device, "dev_name", device_name);
        ASSIGN_DEVICE_NAME_V2(CO2_device, device_name);
        CO2_device->cloud_properties.category = category_generic_sensor;
        CO2_device->cloud_properties.subcategory = subcategory_not_defined;
        CO2_device->cloud_properties.device_type = dev_type_sensor;
        CO2_device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();

        l_ezlopi_item_t *CO2_item = ezlopi_device_add_item_to_device(CO2_device, sensor_0010_I2C_BME680_v3);
        if (CO2_item)
        {
            __prepare_CO2_item_cloud_properties(CO2_item, cj_device, user_arg);
        }
        if (NULL == CO2_item)
        {
            ezlopi_device_free_device(CO2_device);
        }
    }
}

static int __prepare(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    bme680_data_t *sensor_bme680_data = (bme680_data_t *)malloc(sizeof(bme680_data_t));
    if (prep_arg && prep_arg->cjson_device && sensor_bme680_data)
    {
        memset(sensor_bme680_data, 0, sizeof(bme680_data_t));
        __prepare_temp_humid_device(prep_arg->cjson_device, (void *)sensor_bme680_data);
        __prepare_pressure_device(prep_arg->cjson_device, (void *)sensor_bme680_data);
        __prepare_AIQ_device(prep_arg->cjson_device, (void *)sensor_bme680_data);
        __prepare_altitude_device(prep_arg->cjson_device, (void *)sensor_bme680_data);
        __prepare_CO2_device(prep_arg->cjson_device, (void *)sensor_bme680_data);
    }
    return ret;
}

