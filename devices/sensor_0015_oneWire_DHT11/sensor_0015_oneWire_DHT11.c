
#include "cJSON.h"
#include "ezlopi_cloud.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "trace.h"
#include "dht11.h"
#include "sensor_0015_oneWire_DHT11.h"

static float humidity_dht11 = 0;
static float temperature_dht11 = 0;
static bool dht11_initialized = false;
static int counter = 5;

static int dht11_sensor_prepare(void *args);
static int dht11_sensor_add_to_list(s_ezlopi_prep_arg_t *prep_arg, s_ezlopi_device_properties_t *dht11_sensor_properties, void *user_arg);
static s_ezlopi_device_properties_t *dht11_sensor_prepare_properties(uint32_t device_id, const char *category, const char *sub_category, const char *item_name, const char *value_type, cJSON *cjson_device);
static int dht11_sensor_init(s_ezlopi_device_properties_t *properties);
static int dht11_sensor_get_sensor_value(s_ezlopi_device_properties_t *properties, void *args);

#define ADD_PROPERTIES_DEVICE_LIST(device_id, category, sub_category, item_name, value_type, cjson_device)                \
    {                                                                                                                     \
        s_ezlopi_device_properties_t *_properties = dht11_sensor_prepare_properties(device_id, category, sub_category,    \
                                                                                    item_name, value_type, cjson_device); \
        if (NULL != _properties)                                                                                          \
        {                                                                                                                 \
            dht11_sensor_add_to_list(prep_arg, _properties, NULL);                                                        \
        }                                                                                                                 \
    }

int sensor_0015_oneWire_DHT11(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *ezlo_device, void *arg, void *user_args)
{

    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        dht11_sensor_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        dht11_sensor_init(ezlo_device);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        dht11_sensor_get_sensor_value(ezlo_device, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        if (5 == counter)
        {
            ezlopi_device_value_updated_from_device(ezlo_device);
            counter = 0;
        }
        counter++;
        break;
    }
    default:
    {
        break;
    }
    }
    return ret;
}

static int dht11_sensor_prepare(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;

    if ((NULL != prep_arg) && (NULL != prep_arg->cjson_device))
    {
        uint32_t device_id = ezlopi_cloud_generate_device_id();
        // temperature
        ADD_PROPERTIES_DEVICE_LIST(device_id, category_temperature, subcategory_not_defined, ezlopi_item_name_temp, value_type_temperature, prep_arg->cjson_device);
        // humidity
        ADD_PROPERTIES_DEVICE_LIST(device_id, category_humidity, subcategory_not_defined, ezlopi_item_name_humidity, value_type_humidity, prep_arg->cjson_device);
    }
    return ret;
}

static s_ezlopi_device_properties_t *dht11_sensor_prepare_properties(uint32_t device_id, const char *category, const char *sub_category, const char *item_name, const char *value_type, cJSON *cjson_device)
{
    s_ezlopi_device_properties_t *dht11_sensor_properties = NULL;

    if (NULL != cjson_device)
    {
        dht11_sensor_properties = (s_ezlopi_device_properties_t *)malloc(sizeof(s_ezlopi_device_properties_t));
        if (dht11_sensor_properties)
        {
            memset(dht11_sensor_properties, 0, sizeof(s_ezlopi_device_properties_t));
            dht11_sensor_properties->interface_type = EZLOPI_DEVICE_INTERFACE_ONEWIRE_MASTER;

            char *device_name = NULL;

            CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
            ASSIGN_DEVICE_NAME(dht11_sensor_properties, device_name);
            dht11_sensor_properties->ezlopi_cloud.category = category;
            dht11_sensor_properties->ezlopi_cloud.subcategory = sub_category;
            dht11_sensor_properties->ezlopi_cloud.item_name = item_name;
            dht11_sensor_properties->ezlopi_cloud.device_type = dev_type_sensor;
            dht11_sensor_properties->ezlopi_cloud.value_type = value_type;
            dht11_sensor_properties->ezlopi_cloud.reachable = true;
            dht11_sensor_properties->ezlopi_cloud.battery_powered = false;
            dht11_sensor_properties->ezlopi_cloud.show = true;
            dht11_sensor_properties->ezlopi_cloud.room_name[0] = '\0';
            dht11_sensor_properties->ezlopi_cloud.device_id = device_id;
            dht11_sensor_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
            dht11_sensor_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();
            CJSON_GET_VALUE_INT(cjson_device, "gpio", dht11_sensor_properties->interface.onewire_master.onewire_pin);
            dht11_sensor_properties->interface.onewire_master.enable = true;
        }
    }
    return dht11_sensor_properties;
}

static int dht11_sensor_add_to_list(s_ezlopi_prep_arg_t *prep_arg, s_ezlopi_device_properties_t *dht11_sensor_properties, void *user_arg)
{
    int ret = 0;
    if (dht11_sensor_properties)
    {
        if (0 == ezlopi_devices_list_add(prep_arg->device, dht11_sensor_properties, user_arg))
        {
            free(dht11_sensor_properties);
        }
        else
        {
            ret = 1;
        }
    }
    return ret;
}

static int dht11_sensor_init(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;

    if (!dht11_initialized)
    {
        setDHT11gpio(properties->interface.onewire_master.onewire_pin);
        dht11_initialized = true;
    }

    return ret;
}

static int dht11_sensor_get_sensor_value(s_ezlopi_device_properties_t *properties, void *args)
{
    int ret = 0;
    cJSON *cjson_properties = (cJSON *)args;
    char formatted_value[20];
    if (cjson_properties)
    {
        dht11_read_data(&humidity_dht11,&temperature_dht11);
        if (ezlopi_item_name_temp == properties->ezlopi_cloud.item_name)
        {
            TRACE_I("Temperature : %.2f *c", temperature_dht11);
            snprintf(formatted_value, 20, "%0.2f", temperature_dht11);
            cJSON_AddStringToObject(cjson_properties, "valueFormatted", formatted_value);
            cJSON_AddNumberToObject(cjson_properties, "value", temperature_dht11);
            cJSON_AddStringToObject(cjson_properties, "scale", "celsius");
        }
        if (ezlopi_item_name_humidity == properties->ezlopi_cloud.item_name)
        {
            TRACE_I("Humidity : %.2f percent", humidity_dht11);
            snprintf(formatted_value, 20, "%0.2f", humidity_dht11);
            cJSON_AddStringToObject(cjson_properties, "valueFormatted", formatted_value);
            cJSON_AddNumberToObject(cjson_properties, "value", humidity_dht11);
            cJSON_AddStringToObject(cjson_properties, "scale", "percent");
        }
    }

    return ret;
}
