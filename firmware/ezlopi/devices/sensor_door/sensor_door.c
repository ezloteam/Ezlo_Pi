#include "sdkconfig.h"

#ifdef CONFIG_IDF_TARGET_ESP32
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

static const char* dw_status_dw_is_closed = "dw_is_closed";
static const char* dw_status_dw_is_opened = "dw_is_opened";

static int sensor_door_init(s_ezlopi_device_properties_t *properties);
static int get_door_sensor_value(s_ezlopi_device_properties_t *properties, void *args);
static s_ezlopi_device_properties_t *sensor_door_prepare_properties(void *args);
static int sensor_door_prepare(void *args);
static int ezlopi_get_ezlopi_door_value(s_ezlopi_device_properties_t* properties, void* args);

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
        ezlopi_get_ezlopi_door_value(properties, args);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_200_MS:
    {
        get_door_sensor_value(properties, args);
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
            sensor_door_properties->ezlopi_cloud.device_id = ezlopi_device_generate_device_id();
            sensor_door_properties->ezlopi_cloud.room_id = ezlopi_device_generate_room_id();
            sensor_door_properties->ezlopi_cloud.item_id = ezlopi_device_generate_item_id();
            sensor_door_properties->interface.gpio.gpio_in.gpio_num = GPIO_NUM_36;
            sensor_door_properties->user_arg = (void*)dw_status_dw_is_closed;
        }
    }
    return sensor_door_properties;
}

static int get_door_sensor_value(s_ezlopi_device_properties_t *properties, void *args)
{
    int ret = 0;
    char* dw_status = (char*)properties->user_arg;
#ifdef CONFIG_IDF_TARGET_ESP32
    int sensor_data = hall_sensor_read();
#else
    int sensor_data = 0;
#endif
    
    properties->user_arg = (void*)(((sensor_data >= 60) || (sensor_data <= 20)) ? dw_status_dw_is_closed : dw_status_dw_is_opened);
    // TRACE_I("The current door status is %s", (char*)properties->user_arg);
    // TRACE_E("The previous door status is %s", dw_status);
    if(dw_status != (char*)properties->user_arg)
    {
        ezlopi_device_value_updated_from_device(properties);
    }
    return ret;
}

static int ezlopi_get_ezlopi_door_value(s_ezlopi_device_properties_t* properties, void* args)
{
    int ret = 0;
    cJSON *cjson_propertise = (cJSON *)args;
    if (cjson_propertise)
    {
        if (cJSON_AddStringToObject(cjson_propertise, "value", (char*)properties->user_arg))
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
        TRACE_E("Error 'sensor_door_init'. error: %s)", esp_err_to_name(error));
    }
    else
    {
        TRACE_I("Width configuration was successfully done!");
    }
    return error;
}

#endif // CONFIG_IDF_TARGET_ESP32
