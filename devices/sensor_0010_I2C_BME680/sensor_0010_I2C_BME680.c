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

#include "sensor_0010_I2C_BME680.h"
#include "bme680_bsec.h"

#define ADD_PROPERTIES_DEVICE_LIST(_properties, device_id, category, sub_category, item_name, value_type, cjson_device, sensor_0010_I2C_BME680_data)                 \
    {                                                                                                                                          \
        _properties = sensor_0010_I2C_BME680_sensor_prepare_properties(device_id, category, sub_category,                        \
                                                                                     item_name, value_type, cjson_device, sensor_0010_I2C_BME680_data); \
        if (NULL != _properties)                                                                                                               \
        {                                                                                                                                      \
            add_device_to_list(prep_arg, _properties, NULL);                                                                                   \
        }                                                                                                                                      \
    }

static s_ezlopi_device_properties_t *AQI_properties = NULL;
static s_ezlopi_device_properties_t *temperature_properties = NULL;
static s_ezlopi_device_properties_t *pressure_properties = NULL;
static s_ezlopi_device_properties_t *humidity_properties = NULL;
static s_ezlopi_device_properties_t *CO2_properties = NULL;
static s_ezlopi_device_properties_t *altitude_properties = NULL;

static bool sensor_0010_I2C_BME680_i2c_initialized = false;
static int count = 5;

static int sensor_0010_I2C_BME680_sensor_prepare(void *arg);
static s_ezlopi_device_properties_t *sensor_0010_I2C_BME680_sensor_prepare_properties(uint32_t device_id, const char *category,
                                                                      const char *sub_category, const char *item_name,
                                                                      const char *value_type, cJSON *cjson_device, bme680_data_t *sensor_0010_I2C_BME680_data);
static int add_device_to_list(s_ezlopi_prep_arg_t *prep_arg, s_ezlopi_device_properties_t *properties, void *user_args);
static int sensor_0010_I2C_BME680_init(s_ezlopi_device_properties_t *properties);
static int sensor_0010_I2C_BME680_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args);
static int sensor_0010_I2C_BME680_update_values(s_ezlopi_device_properties_t *properties);

int sensor_0010_I2C_BME680(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg)
{
    int ret = 0;
    // TRACE_B("Action is %s", ezlopi_actions_to_string(action));
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        sensor_0010_I2C_BME680_sensor_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        sensor_0010_I2C_BME680_init(properties);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        sensor_0010_I2C_BME680_get_value_cjson(properties, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        sensor_0010_I2C_BME680_update_values(properties);
        break;
    }
    default:
    {
        break;
    }
    }
    return ret;
}

static int sensor_0010_I2C_BME680_sensor_prepare(void *arg)
{
    int ret = 0;

    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    bme680_data_t *sensor_0010_I2C_BME680_data = (bme680_data_t *)malloc(sizeof(bme680_data_t));

    if ((NULL != prep_arg) && (NULL != prep_arg->cjson_device) && (NULL != sensor_0010_I2C_BME680_data))
    {
        memset(sensor_0010_I2C_BME680_data, 0, sizeof(bme680_data_t));
        uint32_t device_id = ezlopi_cloud_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(temperature_properties, device_id, category_temperature, subcategory_not_defined, ezlopi_item_name_temp, value_type_temperature, prep_arg->cjson_device, sensor_0010_I2C_BME680_data);
        // device_id = ezlopi_cloud_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(humidity_properties, device_id, category_humidity, subcategory_not_defined, ezlopi_item_name_humidity, value_type_humidity, prep_arg->cjson_device, sensor_0010_I2C_BME680_data);
        device_id = ezlopi_cloud_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(pressure_properties, device_id, category_generic_sensor, subcategory_not_defined, ezlopi_item_name_atmospheric_pressure, value_type_int, prep_arg->cjson_device, sensor_0010_I2C_BME680_data);
        device_id = ezlopi_cloud_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(AQI_properties, device_id, category_generic_sensor, subcategory_not_defined, ezlopi_item_name_volatile_organic_compound_level, value_type_substance_amount, prep_arg->cjson_device, sensor_0010_I2C_BME680_data);
        device_id = ezlopi_cloud_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(altitude_properties, device_id, category_generic_sensor, subcategory_not_defined, ezlopi_item_name_distance, value_type_length, prep_arg->cjson_device, sensor_0010_I2C_BME680_data);
        device_id = ezlopi_cloud_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(CO2_properties, device_id, category_generic_sensor, subcategory_not_defined, ezlopi_item_name_co2_level, value_type_substance_amount, prep_arg->cjson_device, sensor_0010_I2C_BME680_data);        
    }
    return ret;
}

static s_ezlopi_device_properties_t *sensor_0010_I2C_BME680_sensor_prepare_properties(uint32_t device_id, const char *category,
                                                                      const char *sub_category, const char *item_name,
                                                                      const char *value_type, cJSON *cjson_device, bme680_data_t *sensor_0010_I2C_BME680_data)
{
    s_ezlopi_device_properties_t *bme680_properties = NULL;

    if (NULL != cjson_device)
    {
        bme680_properties = (s_ezlopi_device_properties_t *)malloc(sizeof(s_ezlopi_device_properties_t));

        if (bme680_properties)
        {
            memset(bme680_properties, 0, sizeof(s_ezlopi_device_properties_t));

            bme680_properties->interface_type = EZLOPI_DEVICE_INTERFACE_I2C_MASTER;

            char *device_name = NULL;

            if (ezlopi_item_name_temp == item_name)
            {
                device_name = "Tmp / Hum ";
            }
            if (ezlopi_item_name_humidity == item_name)
            {
                device_name = "Tmp / Hum ";
            }
            if (ezlopi_item_name_atmospheric_pressure == item_name)
            {
                device_name = "ATM";
            }
            if (ezlopi_item_name_volatile_organic_compound_level == item_name)
            {
                device_name = "AQI";
            }
            if (ezlopi_item_name_distance == item_name)
            {
                device_name = "Altitude";
            }
            if (ezlopi_item_name_co2_level == item_name)
            {
                device_name = "Equivalent CO2";
            }            
            ASSIGN_DEVICE_NAME(bme680_properties, device_name);
            bme680_properties->ezlopi_cloud.category = category;
            bme680_properties->ezlopi_cloud.subcategory = sub_category;
            bme680_properties->ezlopi_cloud.item_name = item_name;
            bme680_properties->ezlopi_cloud.value_type = value_type;
            bme680_properties->ezlopi_cloud.device_type = dev_type_sensor;
            bme680_properties->ezlopi_cloud.reachable = true;
            bme680_properties->ezlopi_cloud.battery_powered = false;
            bme680_properties->ezlopi_cloud.show = true;
            bme680_properties->ezlopi_cloud.has_getter = true;
            bme680_properties->ezlopi_cloud.has_setter = false;
            bme680_properties->ezlopi_cloud.room_name[0] = '\0';
            bme680_properties->ezlopi_cloud.device_id = device_id;
            bme680_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
            bme680_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();

            CJSON_GET_VALUE_INT(cjson_device, "gpio_scl", bme680_properties->interface.i2c_master.scl);
            CJSON_GET_VALUE_INT(cjson_device, "gpio_sda", bme680_properties->interface.i2c_master.sda);
            
            bme680_properties->interface.i2c_master.clock_speed = 100000;
            bme680_properties->interface.i2c_master.address = 0x77;
            bme680_properties->interface.i2c_master.enable = true;
            bme680_properties->interface.i2c_master.channel = I2C_NUM_0;
            bme680_properties->user_arg = sensor_0010_I2C_BME680_data;
        }
    }
    return bme680_properties;
}

static int add_device_to_list(s_ezlopi_prep_arg_t *prep_arg, s_ezlopi_device_properties_t *properties, void *user_args)
{
    int ret = 0;

    if (properties)
    {
        if (0 == ezlopi_devices_list_add(prep_arg->device, properties, user_args))
        {
            free(properties);
        }
        else
        {
            ret = 1;
        }
    }
    return ret;
}

static int sensor_0010_I2C_BME680_init(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;

    if ((!sensor_0010_I2C_BME680_i2c_initialized))
    {
        bme680_setup(properties->interface.i2c_master.sda, properties->interface.i2c_master.scl, true);
        sensor_0010_I2C_BME680_i2c_initialized = true;
    }
    return ret;
}

static int sensor_0010_I2C_BME680_update_values(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;
    bme680_data_t *sensor_0010_I2C_BME680_data = (bme680_data_t *)properties->user_arg;
    if (NULL != sensor_0010_I2C_BME680_data)
    {
        bool val = bme680_get_data(sensor_0010_I2C_BME680_data);
        if(val)
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


            ezlopi_device_value_updated_from_device(temperature_properties);
            ezlopi_device_value_updated_from_device(humidity_properties);
            ezlopi_device_value_updated_from_device(pressure_properties);
            ezlopi_device_value_updated_from_device(AQI_properties);
            ezlopi_device_value_updated_from_device(CO2_properties);
            ezlopi_device_value_updated_from_device(altitude_properties);
        }
        
    }
    return ret;
}

static int sensor_0010_I2C_BME680_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args)
{
    int ret = 0;
 
    cJSON *cjson_properties = (cJSON *)args;
    char formatted_value[20];
    bme680_data_t *sensor_0010_I2C_BME680_data = (bme680_data_t *)properties->user_arg;

    if (cjson_properties && sensor_0010_I2C_BME680_data)
    {
        if (ezlopi_item_name_temp == properties->ezlopi_cloud.item_name)
        {
            snprintf(formatted_value, 20, "%.2f", sensor_0010_I2C_BME680_data->temperature);
            cJSON_AddStringToObject(cjson_properties, "valueFormatted", formatted_value);
            cJSON_AddNumberToObject(cjson_properties, "value", sensor_0010_I2C_BME680_data->temperature);
            cJSON_AddStringToObject(cjson_properties, "scale", "celsius");
        }
        if (ezlopi_item_name_humidity == properties->ezlopi_cloud.item_name)
        {
            snprintf(formatted_value, 20, "%.2f", sensor_0010_I2C_BME680_data->humidity);
            cJSON_AddStringToObject(cjson_properties, "valueFormatted", formatted_value);
            cJSON_AddNumberToObject(cjson_properties, "value", sensor_0010_I2C_BME680_data->humidity);
            cJSON_AddStringToObject(cjson_properties, "scale", "percent");
        }
        if (ezlopi_item_name_atmospheric_pressure == properties->ezlopi_cloud.item_name)
        {
            sensor_0010_I2C_BME680_data->pressure = sensor_0010_I2C_BME680_data->pressure / 1000.0;
            snprintf(formatted_value, 20, "%.2f", sensor_0010_I2C_BME680_data->pressure);
            cJSON_AddStringToObject(cjson_properties, "valueFormatted", formatted_value);
            cJSON_AddNumberToObject(cjson_properties, "value", (int)(sensor_0010_I2C_BME680_data->pressure));
            cJSON_AddStringToObject(cjson_properties, "scale", "kilo_pascal");
        }
        if (ezlopi_item_name_volatile_organic_compound_level == properties->ezlopi_cloud.item_name)
        {
            snprintf(formatted_value, 20, "%.2f", sensor_0010_I2C_BME680_data->iaq);
            TRACE_I("ezlopi_item_name_volatile_organic_compound_level: %s", formatted_value);
            cJSON_AddStringToObject(cjson_properties, "valueFormatted", formatted_value);
            // cJSON_AddStringToObject(cjson_properties, "scale", "parts_per_million");        
            cJSON_AddNumberToObject(cjson_properties, "value", (uint16_t)sensor_0010_I2C_BME680_data->iaq); 
        }     
         if (ezlopi_item_name_distance == properties->ezlopi_cloud.item_name)
        {
            snprintf(formatted_value, 20, "%.2f", sensor_0010_I2C_BME680_data->altitude);
            cJSON_AddStringToObject(cjson_properties, "valueFormatted", formatted_value);
            cJSON_AddNumberToObject(cjson_properties, "value", (int)sensor_0010_I2C_BME680_data->altitude);
            cJSON_AddStringToObject(cjson_properties, "scale", "meter");     
        } 

        if (ezlopi_item_name_co2_level == properties->ezlopi_cloud.item_name)
        {
            snprintf(formatted_value, 20, "%.2f", sensor_0010_I2C_BME680_data->co2_equivalent);
            cJSON_AddStringToObject(cjson_properties, "valueFormatted", formatted_value);
            cJSON_AddNumberToObject(cjson_properties, "value", (uint16_t)sensor_0010_I2C_BME680_data->co2_equivalent);
            // cJSON_AddStringToObject(cjson_properties, "scale", "");    
        }             
    }
    return ret;
}