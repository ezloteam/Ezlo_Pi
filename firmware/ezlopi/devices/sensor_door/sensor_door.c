

#include "trace.h"
#include "sensor_door.h"
#include "esp_err.h"
#include "ezlopi_actions.h"
#include "ezlopi_timer.h"
#include "items.h"
#include "cJSON.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "ezlopi_device_value_updated.h"
#include "gpio_isr_service.h"

static int sensor_door_init(s_ezlopi_device_properties_t *properties);
static int get_door_sensor_value(s_ezlopi_device_properties_t *properties, void *args);
static s_ezlopi_device_properties_t *sensor_door_prepare_properties(void *args);
static int sensor_door_prepare(void *args);

int setup_door_sensor(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *args)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        TRACE_I("%s", ezlopi_actions_to_string(action));
        sensor_door_prepare(args);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        TRACE_I("%s", ezlopi_actions_to_string(action));
        sensor_door_init(properties);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        TRACE_I("EZLOPI_ACTION_GET_EZLOPI_VALUE event.");
        get_door_sensor_value(properties, args);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_200_MS:
    {
        TRACE_I("%s", ezlopi_actions_to_string(action));
        ezlopi_device_value_updated_from_device(properties);
        break;
    }
    default:
    {
        TRACE_E("Default action encountered.(action: %s)", ezlopi_actions_to_string(action));
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
            if (0 == ezlopi_devices_list_add(prep_arg->device, sensor_door_device_properties))
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
            sensor_door_properties->ezlopi_cloud.device_id = ezlopi_device_generate_device_id();
            sensor_door_properties->ezlopi_cloud.room_id = ezlopi_device_generate_room_id();
            sensor_door_properties->ezlopi_cloud.item_id = ezlopi_device_generate_item_id();
            sensor_door_properties->interface.gpio.gpio_in.gpio_num = GPIO_NUM_36;
        }
    }
    return sensor_door_properties;
}

static int get_door_sensor_value(s_ezlopi_device_properties_t *properties, void *args)
{
    int ret = 0;
#ifdef CONFIG_IDF_TARGET_ESP32
    int sensor_data = hall_sensor_read();
#else
    int sensor_data = 0;
#endif
    TRACE_E("Reading value from the door sensor.");
    char *door_is = ((sensor_data >= 60) || (sensor_data <= 20)) ? "dw_is_closed" : "dw_is_opened";
    // TRACE_I("The door is %s", door_is);
    cJSON *cjson_propertise = (cJSON *)args;
    if (cjson_propertise)
    {
        if (cJSON_AddStringToObject(cjson_propertise, "value", door_is))
        {
            ret = 1;
        }
    }

    return ret;
}

static int sensor_door_init(s_ezlopi_device_properties_t *properties)
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
    return error;
}
