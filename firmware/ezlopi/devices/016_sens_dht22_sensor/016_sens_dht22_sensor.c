
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
#include "trace.h"
#include "ezlopi_adc.h"
#include "dht22.h"
#include "016_sens_dht22_sensor.h"

static int dht22_sensor_prepare_v3(void *arg);
static int dht22_sensor_init_v3(l_ezlopi_item_t *item);
static int dht22_sensor_get_sensor_value_v3(l_ezlopi_item_t *item, void *args);
static int dht11_sensor_setup_item_properties_temperature(l_ezlopi_item_t *item, cJSON *cj_device);
static int dht11_sensor_setup_item_properties_humidity(l_ezlopi_item_t *item, cJSON *cj_device);
static int dht22_sensor_setup_device_cloud_properties_humidity(l_ezlopi_device_t *device, cJSON *cj_device);
static int dht22_sensor_setup_device_cloud_properties_temperature(l_ezlopi_device_t *device, cJSON *cj_device);

int dht22_sensor_v3(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
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
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        dht22_sensor_get_sensor_value_v3(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        TRACE_B("Here");
        ezlopi_device_value_updated_from_device_v3(item);
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

static int dht22_sensor_get_sensor_value_v3(l_ezlopi_item_t *item, void *args)
{
    int ret = 0;
    cJSON *cj_properties = (cJSON *)args;
    if (item && cj_properties)
    {
        if (ezlopi_item_name_temp == item->cloud_properties.item_name)
        {
            readDHT();
            float temperature = getTemperature();
            cJSON_AddNumberToObject(cj_properties, "value", temperature);
            cJSON_AddStringToObject(cj_properties, "scale", "celsius");
        }

        if (ezlopi_item_name_humidity == item->cloud_properties.item_name)
        {
            readDHT();
            float humidity = getHumidity();
            cJSON_AddNumberToObject(cj_properties, "value", humidity);
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
            l_ezlopi_device_t *parent_device = ezlopi_device_add_device();
            if (parent_device)
            {
                dht22_sensor_setup_device_cloud_properties_temperature(parent_device, cjson_device);

                l_ezlopi_item_t *item_temperature = ezlopi_device_add_item_to_device(parent_device, NULL);
                if (item_temperature)
                {
                    item_temperature->func = dht22_sensor_v3;
                    dht11_sensor_setup_item_properties_temperature(item_temperature, cjson_device);
                }

                // dht22_sensor_setup_device_cloud_properties_humidity(parent_device, cjson_device);
                l_ezlopi_item_t *item_humidity = ezlopi_device_add_item_to_device(parent_device, NULL);
                if (item_humidity)
                {
                    item_humidity->func = dht22_sensor_v3;
                    dht11_sensor_setup_item_properties_humidity(item_humidity, cjson_device);
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
        device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
    }
    return ret;
}

static int dht11_sensor_setup_item_properties_temperature(l_ezlopi_item_t *item, cJSON *cj_device)
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

        CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type);

        item->interface.onewire_master.enable = true;
        CJSON_GET_VALUE_INT(cj_device, "gpio", item->interface.onewire_master.onewire_pin);
    }

    return ret;
}

static int dht11_sensor_setup_item_properties_humidity(l_ezlopi_item_t *item, cJSON *cj_device)
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

        CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type);

        item->interface.onewire_master.enable = true;
        CJSON_GET_VALUE_INT(cj_device, "gpio", item->interface.onewire_master.onewire_pin);
    }

    return ret;
}
