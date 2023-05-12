
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
#include "ezlopi_adc.h"
#include "dht22.h"
#include "016_sens_dht22_sensor.h"


static int dht22_sensor_prepare(void* args);
static int dht22_sensor_add_to_list(s_ezlopi_prep_arg_t *prep_arg, s_ezlopi_device_properties_t *dht22_sensor_properties, void *user_arg);
static s_ezlopi_device_properties_t *dht22_sensor_prepare_properties(uint32_t device_id, const char *category, const char *sub_category, const char *item_name, const char *value_type, cJSON *cjson_device);
static int dht22_sensor_init(s_ezlopi_device_properties_t* properties);
static int dht22_sensor_get_sensor_value(s_ezlopi_device_properties_t* properties, void* args);


#define ADD_PROPERTIES_DEVICE_LIST(device_id, category, sub_category, item_name, value_type, cjson_device)                                      \
    {                                                                                                                                           \
        s_ezlopi_device_properties_t *_properties = dht22_sensor_prepare_properties(device_id, category, sub_category,                          \
                                                                                       item_name, value_type, cjson_device);                    \
        if (NULL != _properties)                                                                                                                \
        {                                                                                                                                       \
            dht22_sensor_add_to_list(prep_arg, _properties, NULL);                                                                              \
        }                                                                                                                                       \
    }


int dht22_sensor(e_ezlopi_actions_t action, s_ezlopi_device_properties_t* ezlo_device, void* arg, void* user_args)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        dht22_sensor_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        dht22_sensor_init(ezlo_device);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        dht22_sensor_get_sensor_value(ezlo_device, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        ezlopi_device_value_updated_from_device(ezlo_device);
        break;
    }
    default:
    {
        break;
    }
    }
    return ret;
}


static int dht22_sensor_prepare(void* arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;

    if ((NULL != prep_arg) && (NULL != prep_arg->cjson_device))
    {
        uint32_t device_id = ezlopi_cloud_generate_device_id();

        // ****************************** I2C Interface ******************************
        // temperature
        ADD_PROPERTIES_DEVICE_LIST(device_id, category_temperature, subcategory_not_defined, ezlopi_item_name_temp, value_type_temperature, prep_arg->cjson_device);
        device_id = ezlopi_cloud_generate_device_id();
        // humidity
        ADD_PROPERTIES_DEVICE_LIST(device_id, category_humidity, subcategory_not_defined, ezlopi_item_name_humidity, value_type_humidity, prep_arg->cjson_device);
    }
    return ret;
}


static s_ezlopi_device_properties_t *dht22_sensor_prepare_properties(uint32_t device_id, const char *category, const char *sub_category, const char *item_name, const char *value_type, cJSON *cjson_device)
{
    s_ezlopi_device_properties_t* dht22_sensor_properties = NULL;

    if(NULL != cjson_device)
    {
        dht22_sensor_properties = (s_ezlopi_device_properties_t*)malloc(sizeof(s_ezlopi_device_properties_t));
        if(dht22_sensor_properties)
        {
            memset(dht22_sensor_properties, 0, sizeof(s_ezlopi_device_properties_t));
            dht22_sensor_properties->interface_type = EZLOPI_DEVICE_INTERFACE_ONEWIRE_MASTER;

            char *device_name = NULL;
            CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
            ASSIGN_DEVICE_NAME(dht22_sensor_properties, device_name);
            dht22_sensor_properties->ezlopi_cloud.category = category;
            dht22_sensor_properties->ezlopi_cloud.subcategory = sub_category;
            dht22_sensor_properties->ezlopi_cloud.item_name = item_name;
            dht22_sensor_properties->ezlopi_cloud.device_type = dev_type_sensor;
            dht22_sensor_properties->ezlopi_cloud.value_type = value_type;
            dht22_sensor_properties->ezlopi_cloud.reachable = true;
            dht22_sensor_properties->ezlopi_cloud.battery_powered = false;
            dht22_sensor_properties->ezlopi_cloud.show = true;
            dht22_sensor_properties->ezlopi_cloud.room_name[0] = '\0';
            dht22_sensor_properties->ezlopi_cloud.device_id = device_id;
            dht22_sensor_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
            dht22_sensor_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();
            CJSON_GET_VALUE_INT(cjson_device, "gpio", dht22_sensor_properties->interface.onewire_master.onewire_pin);
            // dht22_sensor_properties->interface.onewire_master.onewire_pin = 2;
            dht22_sensor_properties->interface.onewire_master.enable = true;
        }
    }

    return dht22_sensor_properties;
}

static int dht22_sensor_add_to_list(s_ezlopi_prep_arg_t *prep_arg, s_ezlopi_device_properties_t *dht22_sensor_properties, void *user_arg)
{
    int ret = 0;
    if (dht22_sensor_properties)
    {
        if (0 == ezlopi_devices_list_add(prep_arg->device, dht22_sensor_properties, user_arg))
        {
            free(dht22_sensor_properties);
        }
        else
        {
            ret = 1;
        }
    }
    return ret;
}


static int dht22_sensor_init(s_ezlopi_device_properties_t* properties)
{
    int ret = 0;

    setDHTgpio(properties->interface.onewire_master.onewire_pin);

    return ret;
}

static int dht22_sensor_get_sensor_value(s_ezlopi_device_properties_t* properties, void* args)
{
    int ret = 0;

    cJSON* cjson_properties = (cJSON*)args;
    if(cjson_properties)
    {
        if (ezlopi_item_name_temp == properties->ezlopi_cloud.item_name)
        {
            readDHT();
            float temperature = getTemperature();
            cJSON_AddNumberToObject(cjson_properties, "value", temperature);
            cJSON_AddStringToObject(cjson_properties, "scale", "celsius");
        }
        if(ezlopi_item_name_humidity == properties->ezlopi_cloud.item_name)
        {
            readDHT();
            float humidity = getHumidity();
            cJSON_AddNumberToObject(cjson_properties, "value", humidity);
            cJSON_AddStringToObject(cjson_properties, "scale", "percent");
        }
    }

    return ret;
}