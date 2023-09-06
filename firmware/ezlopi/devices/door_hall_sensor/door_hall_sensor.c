#include "sdkconfig.h"

#ifdef CONFIG_IDF_TARGET_ESP32
#include "trace.h"
#include "door_hall_sensor.h"
#include "esp_err.h"
#include "ezlopi_actions.h"
#include "ezlopi_timer.h"
#include "items.h"
#include "cJSON.h"

#include "ezlopi_cloud.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_device_value_updated.h"
#include "gpio_isr_service.h"

static int __prepare(void *arg);
static int __init(l_ezlopi_item_t *item);
static int __get_value_cjson(l_ezlopi_item_t *item, void *arg);
static int __notify(l_ezlopi_item_t *item);

int door_hall_sensor_v3(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    int ret = 0;

    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = __prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = __init(item);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = __get_value_cjson(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        ret = __notify(item);
    }

    default:
    {
        break;
    }
    }

    return ret;
}

static int __get_value_cjson(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;
    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        int sensor_data = hall_sensor_read();
        char *door_is = ((sensor_data >= 60) || (sensor_data <= 20)) ? "dw_is_closed" : "dw_is_opened";

        cJSON_AddStringToObject(cj_result, "value", door_is);
        ret = 1;
    }
    return ret;
}

static int __notify(l_ezlopi_item_t *item)
{
    return ezlopi_device_value_updated_from_device_v3(item);
}

static int __init(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (item)
    {
#ifdef CONFIG_IDF_TARGET_ESP32
        esp_err_t error = adc1_config_width(ADC_WIDTH_BIT_12);
#else
        esp_err_t error = ESP_ERR_NOT_FOUND;
#endif
        if (error)
        {
            TRACE_E("Error 'sensor_door_init'. rror: %s)", esp_err_to_name(error));
        }
        else
        {
            TRACE_I("Width configuration was successfully done!");
        }
        ret = 1;
    }
    return ret;
}

static void __setup_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    char *device_name = NULL;
    ASSIGN_DEVICE_NAME_V2(device, device_name);

    device->cloud_properties.category = category_security_sensor;
    device->cloud_properties.subcategory = subcategory_door;
    device->cloud_properties.device_type = dev_type_doorlock;
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
}

static void __setup_item_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    item->cloud_properties.show = true;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.item_name = ezlopi_item_name_dw_state;
    item->cloud_properties.value_type = value_type_token;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    item->interface.gpio.gpio_in.gpio_num = GPIO_NUM_36;
    item->interface_type = EZLOPI_DEVICE_INTERFACE_DIGITAL_INPUT;
}

static int __prepare(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (arg)
    {
        cJSON *cj_device = prep_arg->cjson_device;
        if (cj_device)
        {
            l_ezlopi_device_t *device = ezlopi_device_add_device();
            if (device)
            {
                __setup_device_cloud_properties(device, cj_device);
                l_ezlopi_item_t *item = ezlopi_device_add_item_to_device(device, door_hall_sensor_v3);
                if (item)
                {
                    __setup_item_properties(item, cj_device);
                    ret = 1;
                }
                else
                {
                    ezlopi_device_free_device(device);
                }
            }
        }
    }

    return ret;
}

#if 1
static int sensor_door_init(s_ezlopi_device_properties_t *properties);
static int get_door_sensor_value(s_ezlopi_device_properties_t *properties, void *args);
static s_ezlopi_device_properties_t *sensor_door_prepare_properties(void *args);
static int sensor_door_prepare(void *args);

int door_hall_sensor(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *args, void *user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        sensor_door_prepare(args);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        sensor_door_init(properties);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        get_door_sensor_value(properties, args);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_200_MS:
    {
        ezlopi_device_value_updated_from_device(properties);
        break;
    }
    default:
    {
        break;
    }
    }

    return ret;
}

static int sensor_door_prepare(void *args)
{
    int ret = 0;

    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)args;
    cJSON *cjson_device = prep_arg->cjson_device;

    s_ezlopi_device_properties_t *sensor_door_device_properties = NULL;

    if ((NULL == sensor_door_device_properties) && (NULL != cjson_device))
    {
        sensor_door_device_properties = sensor_door_prepare_properties(cjson_device);
        if (sensor_door_device_properties)
        {
            if (0 == ezlopi_devices_list_add(prep_arg->device, sensor_door_device_properties, NULL))
            {
                free(sensor_door_device_properties);
            }
            else
            {
                ret = 1;
            }
        }
    }
    return ret;
}

static s_ezlopi_device_properties_t *sensor_door_prepare_properties(void *args)
{
    cJSON *cjson_device = (cJSON *)args;
    s_ezlopi_device_properties_t *sensor_door_properties = NULL;
    if ((NULL == sensor_door_properties) && (NULL != cjson_device))
    {
        sensor_door_properties = (s_ezlopi_device_properties_t *)malloc(sizeof(s_ezlopi_device_properties_t));
        if (sensor_door_properties)
        {
            memset(sensor_door_properties, 0, sizeof(s_ezlopi_device_properties_t));
            sensor_door_properties->interface_type = EZLOPI_DEVICE_INTERFACE_ANALOG_INPUT;

            char *device_name = NULL;
            CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
            ASSIGN_DEVICE_NAME(sensor_door_properties, device_name);
            sensor_door_properties->ezlopi_cloud.category = category_security_sensor;
            sensor_door_properties->ezlopi_cloud.subcategory = subcategory_door;
            sensor_door_properties->ezlopi_cloud.item_name = ezlopi_item_name_dw_state;
            sensor_door_properties->ezlopi_cloud.device_type = dev_type_doorlock;
            sensor_door_properties->ezlopi_cloud.value_type = value_type_token;
            sensor_door_properties->ezlopi_cloud.has_getter = true;
            sensor_door_properties->ezlopi_cloud.has_setter = false;
            sensor_door_properties->ezlopi_cloud.reachable = true;
            sensor_door_properties->ezlopi_cloud.battery_powered = false;
            sensor_door_properties->ezlopi_cloud.show = true;
            sensor_door_properties->ezlopi_cloud.room_name[0] = '\0';
            sensor_door_properties->ezlopi_cloud.device_id = ezlopi_cloud_generate_device_id();
            sensor_door_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
            sensor_door_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();
            sensor_door_properties->interface.gpio.gpio_in.gpio_num = GPIO_NUM_36;
        }
    }
    return sensor_door_properties;
}

#endif // CONFIG_IDF_TARGET_ESP32
#endif