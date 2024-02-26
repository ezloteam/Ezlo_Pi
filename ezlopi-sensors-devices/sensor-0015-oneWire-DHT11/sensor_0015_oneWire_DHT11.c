#include <math.h>

#include "ezlopi_util_trace.h"

#include "ezlopi_core_timer.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "dht11.h"
#include "sensor_0015_oneWire_DHT11.h"

typedef struct s_ezlopi_dht11_data
{
    float temperature;
    float humidity;
} s_ezlopi_dht11_data_t;

static int __0015_prepare(void* arg);
static int __0015_init(l_ezlopi_item_t* item);
static int __0015_get_value(l_ezlopi_item_t* item, void* args);
static int __0015_notify(l_ezlopi_item_t* item);

static int __dht11_setup_item_properties_temperature(l_ezlopi_item_t* item, cJSON* cj_device, void* user_arg);
static int __dht11_setup_item_properties_humidity(l_ezlopi_item_t* item, cJSON* cj_device, void* user_arg);
static int __dht11_setup_device_cloud_properties_temperature(l_ezlopi_device_t* device, cJSON* cj_device);
static int __dht11_setup_device_cloud_properties_humidity(l_ezlopi_device_t* device, cJSON* cj_device);

//-----------------------------------------------------------------------------------------------
int sensor_0015_oneWire_DHT11(e_ezlopi_actions_t action, l_ezlopi_item_t* item, void* arg, void* user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        __0015_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        __0015_init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        __0015_get_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        __0015_notify(item);
        break;
    }
    default:
    {
        break;
    }
    }
    return ret;
}
//-----------------------------------------------------------------------------------------------

static int __0015_prepare(void* arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t* prep_arg = (s_ezlopi_prep_arg_t*)arg;
    if (prep_arg)
    {
        cJSON* cjson_device = prep_arg->cjson_device;
        if (cjson_device)
        {
            s_ezlopi_dht11_data_t* dht11_sensor_data = (s_ezlopi_dht11_data_t*)malloc(sizeof(s_ezlopi_dht11_data_t));
            if (dht11_sensor_data)
            {
                l_ezlopi_device_t* device_temperature = ezlopi_device_add_device(cjson_device);
                if (device_temperature)
                {
                    __dht11_setup_device_cloud_properties_temperature(device_temperature, cjson_device);
                    l_ezlopi_item_t* item_temperature = ezlopi_device_add_item_to_device(device_temperature, sensor_0015_oneWire_DHT11);
                    if (item_temperature)
                    {
                        item_temperature->cloud_properties.device_id = device_temperature->cloud_properties.device_id;
                        __dht11_setup_item_properties_temperature(item_temperature, cjson_device, dht11_sensor_data);
                    }
                    else
                    {
                        ezlopi_device_free_device(device_temperature);
                        ret = -1;
                    }
                }
                l_ezlopi_device_t* device_humidity = ezlopi_device_add_device(cjson_device);
                if (device_humidity)
                {
                    __dht11_setup_device_cloud_properties_humidity(device_humidity, cjson_device);
                    l_ezlopi_item_t* item_humidity = ezlopi_device_add_item_to_device(device_humidity, sensor_0015_oneWire_DHT11);
                    if (item_humidity)
                    {
                        item_humidity->cloud_properties.device_id = device_humidity->cloud_properties.device_id;
                        __dht11_setup_item_properties_humidity(item_humidity, cjson_device, dht11_sensor_data);
                    }
                    else
                    {
                        ezlopi_device_free_device(device_humidity);
                        ret = -1;
                    }
                }
                if ((NULL == device_temperature) && (NULL == device_humidity))
                {
                    free(dht11_sensor_data);
                }
            }
        }
    }

    return ret;
}

static int __dht11_setup_device_cloud_properties_temperature(l_ezlopi_device_t* device, cJSON* cj_device)
{
    int ret = 0;
    if (device && cj_device)
    {
        char* device_name = NULL;
        CJSON_GET_VALUE_STRING(cj_device, ezlopi_dev_name_str, device_name);

        ASSIGN_DEVICE_NAME_V2(device, device_name);
        device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
        device->cloud_properties.category = category_temperature;
        device->cloud_properties.subcategory = subcategory_not_defined;
        device->cloud_properties.device_type_id = NULL;
        device->cloud_properties.info = NULL;
        device->cloud_properties.device_type = dev_type_sensor;
    }
    return ret;
}

static int __dht11_setup_device_cloud_properties_humidity(l_ezlopi_device_t* device, cJSON* cj_device)
{
    int ret = 0;
    if (device && cj_device)
    {
        char* device_name = NULL;
        CJSON_GET_VALUE_STRING(cj_device, ezlopi_dev_name_str, device_name);

        ASSIGN_DEVICE_NAME_V2(device, device_name);
        device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
        device->cloud_properties.category = category_humidity;
        device->cloud_properties.subcategory = subcategory_not_defined;
        device->cloud_properties.device_type_id = NULL;
        device->cloud_properties.info = NULL;
        device->cloud_properties.device_type = dev_type_sensor;
    }
    return ret;
}

static int __dht11_setup_item_properties_temperature(l_ezlopi_item_t* item, cJSON* cj_device, void* user_arg)
{
    int ret = 0;
    if (item && cj_device)
    {
        item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
        item->cloud_properties.has_getter = true;
        item->cloud_properties.has_setter = false;
        item->cloud_properties.item_name = ezlopi_item_name_temp;
        item->cloud_properties.show = true;
        item->cloud_properties.value_type = value_type_temperature;
        item->cloud_properties.scale = scales_celsius;
        item->user_arg = user_arg;

        CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_type_str, item->interface_type);

        item->interface.onewire_master.enable = true;
        CJSON_GET_VALUE_INT(cj_device, ezlopi_gpio_str, item->interface.onewire_master.onewire_pin);
    }

    return ret;
}

static int __dht11_setup_item_properties_humidity(l_ezlopi_item_t* item, cJSON* cj_device, void* user_arg)
{
    int ret = 0;
    if (item && cj_device)
    {
        item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
        item->cloud_properties.has_getter = true;
        item->cloud_properties.has_setter = false;
        item->cloud_properties.item_name = ezlopi_item_name_humidity;
        item->cloud_properties.show = true;
        item->cloud_properties.value_type = value_type_humidity;
        item->cloud_properties.scale = scales_percent;
        item->user_arg = user_arg;

        CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_type_str, item->interface_type);

        item->interface.onewire_master.enable = true;
        CJSON_GET_VALUE_INT(cj_device, ezlopi_gpio_str, item->interface.onewire_master.onewire_pin);
    }
    return ret;
}

static int __0015_init(l_ezlopi_item_t* item)
{
    int ret = 0;
    if (item)
    {
        s_ezlopi_dht11_data_t *dht11_data = (s_ezlopi_dht11_data_t *)item->user_arg;
        if (dht11_data)
        {
            if (GPIO_IS_VALID_GPIO((gpio_num_t)item->interface.onewire_master.onewire_pin))
            {
                setDHT11gpio(item->interface.onewire_master.onewire_pin);
                ret = 1;
            }
            else
            {
                ret = -1;
                free(item->user_arg); // this will free ; memory address linked to all items
                item->user_arg = NULL;
                ezlopi_device_free_device_by_item(item);
            }
        }
        else
        {
            ret = -1;
            ezlopi_device_free_device_by_item(item);
        }
    }
    return ret;
}

static int __0015_get_value(l_ezlopi_item_t* item, void* args)
{
    int ret = 0;
    cJSON* cj_properties = (cJSON*)args;
    if (item && cj_properties)
    {
        s_ezlopi_dht11_data_t* dht11_data = (s_ezlopi_dht11_data_t*)item->user_arg;
        if (ezlopi_item_name_temp == item->cloud_properties.item_name)
        {
            cJSON_AddNumberToObject(cj_properties, ezlopi_value_str, dht11_data->temperature);
            char *valueFormatted = ezlopi_valueformatter_float(dht11_data->temperature);
            if (valueFormatted)
            {
                cJSON_AddStringToObject(cj_properties, ezlopi_valueFormatted_str, valueFormatted);
                free(valueFormatted);
            }
            cJSON_AddStringToObject(cj_properties, ezlopi_scale_str, item->cloud_properties.scale);
        }
        else if (ezlopi_item_name_humidity == item->cloud_properties.item_name)
        {
            cJSON_AddNumberToObject(cj_properties, ezlopi_value_str, dht11_data->humidity);
            char *valueFormatted = ezlopi_valueformatter_float(dht11_data->humidity);
            if (valueFormatted)
            {
                cJSON_AddStringToObject(cj_properties, ezlopi_valueFormatted_str, valueFormatted);
                free(valueFormatted);
            }
            cJSON_AddStringToObject(cj_properties, ezlopi_scale_str, item->cloud_properties.scale);
        }
    }
    return ret;
}

static int __0015_notify(l_ezlopi_item_t* item)
{
    int ret = 0;
    if (item)
    {
        s_ezlopi_dht11_data_t* dht11_data = (s_ezlopi_dht11_data_t*)item->user_arg;
        if (dht11_data)
        {
            readDHT11();
            if (ezlopi_item_name_temp == item->cloud_properties.item_name)
            {
                float temperature = getTemperature_dht11();
                if (temperature > 15)
                { // TRACE_S("Temperature: %.2f", temperature);
                    if (fabs(dht11_data->temperature - temperature) > 1)
                    {
                        dht11_data->temperature = temperature;
                        ezlopi_device_value_updated_from_device_v3(item);
                    }
                }
            }
            else if (ezlopi_item_name_humidity == item->cloud_properties.item_name)
            {
                float humidity = getHumidity_dht11();
                if (humidity > 20)
                { // TRACE_S("Humidity: %.2f", humidity);
                    if (fabs(dht11_data->humidity - humidity) > 1)
                    {
                        dht11_data->humidity = humidity;
                        ezlopi_device_value_updated_from_device_v3(item);
                    }
                }
            }
        }
    }
    return ret;
}