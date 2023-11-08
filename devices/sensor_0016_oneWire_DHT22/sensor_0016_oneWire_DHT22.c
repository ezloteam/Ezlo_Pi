
#include <math.h>
#include "cJSON.h"
#include "ezlopi_cloud.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "ezlopi_cloud_scales_str.h"
#include "ezlopi_valueformatter.h"

#include "trace.h"
#include "ezlopi_adc.h"
#include "dht22.h"

#include "sensor_0016_oneWire_DHT22.h"

static int dht22_sensor_prepare_v3(void *arg);
static int dht22_sensor_init_v3(l_ezlopi_item_t *item);
static int dht22_sensor_get_sensor_value_v3(l_ezlopi_item_t *item, void *args);
static int dht11_sensor_setup_item_properties_temperature(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg);
static int dht11_sensor_setup_item_properties_humidity(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg);

static int dht22_sensor_setup_device_cloud_properties_temperature(l_ezlopi_device_t *device, cJSON *cj_device);
static int dht22_sensor_setup_device_cloud_properties_humidity(l_ezlopi_device_t *device, cJSON *cj_device);

static int __notify(l_ezlopi_item_t *item);

int sensor_0016_oneWire_DHT22(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    TRACE_B("Action: %s", ezlopi_actions_to_string(action));
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        dht22_sensor_prepare_v3(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        dht22_sensor_init_v3(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        dht22_sensor_get_sensor_value_v3(item, arg);
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

static int dht22_sensor_init_v3(l_ezlopi_item_t *item)
{
    int ret = 0;

    setDHTgpio(item->interface.onewire_master.onewire_pin);

    return ret;
}

static int __notify(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (item)
    {
        s_ezlopi_dht22_data_t *dht22_data = (s_ezlopi_dht22_data_t *)item->user_arg;
        if (dht22_data)
        {
            readDHT();
            if (ezlopi_item_name_temp == item->cloud_properties.item_name)
            {

                float temperature = getTemperature();
                // TRACE_E("Temperature: %.2f", temperature);
                if (fabs(dht22_data->temperature - temperature) > 0.5)
                {
                    dht22_data->temperature = temperature;
                    ezlopi_device_value_updated_from_device_v3(item);
                }
            }
            else if (ezlopi_item_name_humidity == item->cloud_properties.item_name)
            {
                float humidity = getHumidity();
                // TRACE_E("Humidity: %.2f", humidity);
                if (fabs(dht22_data->humidity - humidity) > 0.5)
                {
                    dht22_data->humidity = humidity;
                    ezlopi_device_value_updated_from_device_v3(item);
                }
            }
            else
            {
            }
        }
    }
    return ret;
}
static int dht22_sensor_get_sensor_value_v3(l_ezlopi_item_t *item, void *args)
{
    int ret = 0;
    cJSON *cj_properties = (cJSON *)args;
    if (item && cj_properties)
    {

        s_ezlopi_dht22_data_t *dht22_data = (s_ezlopi_dht22_data_t *)item->user_arg;

        if (ezlopi_item_name_temp == item->cloud_properties.item_name)
        {

            cJSON_AddNumberToObject(cj_properties, "value", dht22_data->temperature);
            char *valueFormatted = ezlopi_valueformatter_float(dht22_data->temperature);
            cJSON_AddStringToObject(cj_properties, "valueFormatted", valueFormatted);
            free(valueFormatted);
            cJSON_AddStringToObject(cj_properties, "scale", "celsius");
        }

        if (ezlopi_item_name_humidity == item->cloud_properties.item_name)
        {
            cJSON_AddNumberToObject(cj_properties, "value", dht22_data->humidity);
            char *valueFormatted = ezlopi_valueformatter_float(dht22_data->humidity);
            cJSON_AddStringToObject(cj_properties, "valueFormatted", valueFormatted);
            free(valueFormatted);
            cJSON_AddStringToObject(cj_properties, "scale", "percent");
        }
    }
    return ret;
}

static int dht22_sensor_prepare_v3(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (prep_arg)
    {
        cJSON *cjson_device = prep_arg->cjson_device;
        if (cjson_device)
        {
            l_ezlopi_device_t *device_temperature = ezlopi_device_add_device();
            l_ezlopi_device_t *device_humidity = ezlopi_device_add_device();
            if (device_temperature)
            {
                s_ezlopi_dht22_data_t *dht22_sensor_data_temp = (s_ezlopi_dht22_data_t *)malloc(sizeof(s_ezlopi_dht22_data_t));

                if (dht22_sensor_data_temp)
                {

                    dht22_sensor_setup_device_cloud_properties_temperature(device_temperature, cjson_device);

                    l_ezlopi_item_t *item_temperature = ezlopi_device_add_item_to_device(device_temperature, sensor_0016_oneWire_DHT22);
                    if (item_temperature)
                    {
                        item_temperature->cloud_properties.device_id = device_temperature->cloud_properties.device_id;
                        dht11_sensor_setup_item_properties_temperature(item_temperature, cjson_device, dht22_sensor_data_temp);
                    }
                }
            }

            if (device_humidity)
            {
                s_ezlopi_dht22_data_t *dht22_sensor_data_hum = (s_ezlopi_dht22_data_t *)malloc(sizeof(s_ezlopi_dht22_data_t));

                if (dht22_sensor_data_hum)
                {

                    dht22_sensor_setup_device_cloud_properties_humidity(device_humidity, cjson_device);

                    l_ezlopi_item_t *item_humidity = ezlopi_device_add_item_to_device(device_humidity, sensor_0016_oneWire_DHT22);
                    if (item_humidity)
                    {
                        item_humidity->cloud_properties.device_id = device_humidity->cloud_properties.device_id;
                        dht11_sensor_setup_item_properties_humidity(item_humidity, cjson_device, dht22_sensor_data_hum);
                    }
                }
            }
        }
    }

    return ret;
}

static int dht22_sensor_setup_device_cloud_properties_temperature(l_ezlopi_device_t *device, cJSON *cj_device)
{
    int ret = 0;
    if (device && cj_device)
    {
        char *device_name = NULL;
        CJSON_GET_VALUE_STRING(cj_device, "dev_name", device_name);

        ASSIGN_DEVICE_NAME_V2(device, device_name);
        device->cloud_properties.category = category_temperature;
        device->cloud_properties.subcategory = subcategory_not_defined;
        device->cloud_properties.device_type = dev_type_sensor;
        device->cloud_properties.info = NULL;
        device->cloud_properties.device_type_id = NULL;
        device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
    }
    return ret;
}

static int dht22_sensor_setup_device_cloud_properties_humidity(l_ezlopi_device_t *device, cJSON *cj_device)
{
    int ret = 0;
    if (device && cj_device)
    {
        char *device_name = NULL;
        CJSON_GET_VALUE_STRING(cj_device, "dev_name", device_name);

        ASSIGN_DEVICE_NAME_V2(device, device_name);
        device->cloud_properties.category = category_humidity;
        device->cloud_properties.subcategory = subcategory_not_defined;
        device->cloud_properties.device_type = dev_type_sensor;
        device->cloud_properties.info = NULL;
        device->cloud_properties.device_type_id = NULL;
        device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
    }
    return ret;
}

static int dht11_sensor_setup_item_properties_temperature(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg)
{
    int ret = 0;

    if (item && cj_device)
    {
        item->cloud_properties.show = true;
        item->cloud_properties.has_getter = true;
        item->cloud_properties.has_setter = false;
        item->cloud_properties.item_name = ezlopi_item_name_temp;
        item->cloud_properties.value_type = value_type_temperature;
        item->cloud_properties.scale = scales_celsius;
        item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
        item->user_arg = user_arg;

        CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type);

        item->interface.onewire_master.enable = true;
        CJSON_GET_VALUE_INT(cj_device, "gpio", item->interface.onewire_master.onewire_pin);
    }

    return ret;
}

static int dht11_sensor_setup_item_properties_humidity(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg)
{
    int ret = 0;

    if (item && cj_device)
    {
        item->cloud_properties.show = true;
        item->cloud_properties.has_getter = true;
        item->cloud_properties.has_setter = false;
        item->cloud_properties.item_name = ezlopi_item_name_humidity;
        item->cloud_properties.value_type = value_type_humidity;
        item->cloud_properties.scale = scales_percent;
        item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
        item->user_arg = user_arg;

        CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type);

        item->interface.onewire_master.enable = true;
        CJSON_GET_VALUE_INT(cj_device, "gpio", item->interface.onewire_master.onewire_pin);
    }

    return ret;
}
