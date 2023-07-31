
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

static int dht22_sensor_prepare_v3(void *arg);
static int dht22_sensor_init_v3(l_ezlopi_item_t *item);
static int dht22_sensor_get_sensor_value_v3(l_ezlopi_item_t *item, void *args);
static int dht11_sensor_setup_item_properties_temperature(l_ezlopi_item_t *item, cJSON *cj_device);
static int dht11_sensor_setup_item_properties_humidity(l_ezlopi_item_t *item, cJSON *cj_device);
static int dht22_sensor_setup_device_cloud_properties_humidity(l_ezlopi_device_t *device, cJSON *cj_device);
static int dht22_sensor_setup_device_cloud_properties_temperature(l_ezlopi_device_t *device, cJSON *cj_device);

int dht22_sensor_v3(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
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
            l_ezlopi_device_t *device_temperature = ezlopi_device_add_device();
            if (device_temperature)
            {
                dht22_sensor_setup_device_cloud_properties_temperature(device_temperature, cjson_device);

                l_ezlopi_item_t *item_temperature = ezlopi_device_add_item_to_device(device_temperature, NULL);
                if (item_temperature)
                {
                    dht11_sensor_setup_item_properties_temperature(item_temperature, cjson_device);
                }
            }

            l_ezlopi_device_t *device_humidity = ezlopi_device_add_device();
            if (device_humidity)
            {
                dht22_sensor_setup_device_cloud_properties_humidity(device_humidity, cjson_device);
                l_ezlopi_item_t *item_humidity = ezlopi_device_add_item_to_device(device_humidity, NULL);
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
        item->cloud_properties.item_name = ezlopi_item_name_humidity;
        item->cloud_properties.value_type = value_type_humidity;
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
        item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

        CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type);

        item->interface.onewire_master.enable = true;
        CJSON_GET_VALUE_INT(cj_device, "gpio", item->interface.onewire_master.onewire_pin);
    }

    return ret;
}

#if 0
static int dht22_sensor_prepare(void *args);
static int dht22_sensor_add_to_list(s_ezlopi_prep_arg_t *prep_arg, s_ezlopi_device_properties_t *dht22_sensor_properties, void *user_arg);
static s_ezlopi_device_properties_t *dht22_sensor_prepare_properties(uint32_t device_id, const char *category, const char *sub_category, const char *item_name, const char *value_type, cJSON *cjson_device);
static int dht22_sensor_init(s_ezlopi_device_properties_t *properties);
static int dht22_sensor_get_sensor_value(s_ezlopi_device_properties_t *properties, void *args);

#define ADD_PROPERTIES_DEVICE_LIST(device_id, category, sub_category, item_name, value_type, cjson_device)                \
    {                                                                                                                     \
        s_ezlopi_device_properties_t *_properties = dht22_sensor_prepare_properties(device_id, category, sub_category,    \
                                                                                    item_name, value_type, cjson_device); \
        if (NULL != _properties)                                                                                          \
        {                                                                                                                 \
            dht22_sensor_add_to_list(prep_arg, _properties, NULL);                                                        \
        }                                                                                                                 \
    }

int dht22_sensor(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *ezlo_device, void *arg, void *user_args)
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

static int dht22_sensor_prepare(void *arg)
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
    s_ezlopi_device_properties_t *dht22_sensor_properties = NULL;

    if (NULL != cjson_device)
    {
        dht22_sensor_properties = (s_ezlopi_device_properties_t *)malloc(sizeof(s_ezlopi_device_properties_t));
        if (dht22_sensor_properties)
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

static int dht22_sensor_init(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;

    setDHTgpio(properties->interface.onewire_master.onewire_pin);

    return ret;
}



static int dht22_sensor_get_sensor_value(s_ezlopi_device_properties_t *properties, void *args)
{
    int ret = 0;

    cJSON *cjson_properties = (cJSON *)args;
    if (cjson_properties)
    {
        if (ezlopi_item_name_temp == properties->ezlopi_cloud.item_name)
        {
            readDHT();
            float temperature = getTemperature();
            cJSON_AddNumberToObject(cjson_properties, "value", temperature);
            cJSON_AddStringToObject(cjson_properties, "scale", "celsius");
        }
        if (ezlopi_item_name_humidity == properties->ezlopi_cloud.item_name)
        {
            readDHT();
            float humidity = getHumidity();
            cJSON_AddNumberToObject(cjson_properties, "value", humidity);
            cJSON_AddStringToObject(cjson_properties, "scale", "percent");
        }
    }

    return ret;
}
#endif
