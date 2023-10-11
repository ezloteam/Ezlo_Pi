#include "trace.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "driver/gpio.h"
#include "gpio_isr_service.h"
#include "cJSON.h"

#include "sensor_0060_digitalIn_vibration_detector.h"

//------------------------------------------------------------------------------
//          STATIC DECLARATION
//------------------------------------------------------------------------------
static s_ezlopi_device_properties_t *sensor_0060_prepare(cJSON *cjson_device);
static int sensor_0060_digitalIn_prepare_add(void *args);
static int sensor_0060_digitalIn_init(s_ezlopi_device_properties_t *properties);
static void sensor_0060_digitalIn_get_item(s_ezlopi_device_properties_t *properties, void *args);
static int sensor_0060_digitalIn_get_value_json(s_ezlopi_device_properties_t *properties, void *args);
static void sensor_0060_update_from_device(s_ezlopi_device_properties_t *properties);
//------------------------------------------------------------------------------
int sensor_0060_digitalIn_vibration_detector(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *args, void *user_args)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = sensor_0060_digitalIn_prepare_add(args);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = sensor_0060_digitalIn_init(properties);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    // {
    //     sensor_0060_digitalIn_get_item(properties, args);
    //     break;
    // }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = sensor_0060_digitalIn_get_value_json(properties, args);
        break;
    }
    default:
        break;
    }
    return ret;
}
//------------------------------------------------------------------------------

static int sensor_0060_digitalIn_prepare_add(void *args)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)args;
    if ((NULL != device_prep_arg) && (NULL != device_prep_arg->cjson_device))
    {
        s_ezlopi_device_properties_t *sensor_0060_properties = sensor_0060_prepare(device_prep_arg->cjson_device);
        if (sensor_0060_properties)
        {
            if (0 == ezlopi_devices_list_add(device_prep_arg->device, sensor_0060_properties, NULL))
            {
                free(sensor_0060_properties);
            }
            else
            {
                ret = 1;
            }
        }
    }
    return ret;
}
static s_ezlopi_device_properties_t *sensor_0060_prepare(cJSON *cjson_device)
{
    s_ezlopi_device_properties_t *sensor_0060_properties = malloc(sizeof(s_ezlopi_device_properties_t));
    if (sensor_0060_properties)
    {
        memset(sensor_0060_properties, 0, sizeof(s_ezlopi_device_properties_t));
        sensor_0060_properties->interface_type = EZLOPI_DEVICE_INTERFACE_DIGITAL_INPUT;

        char *device_name = NULL;
        CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
        ASSIGN_DEVICE_NAME(sensor_0060_properties, device_name);
        sensor_0060_properties->ezlopi_cloud.category = category_security_sensor;
        sensor_0060_properties->ezlopi_cloud.subcategory = subcategory_motion;
        sensor_0060_properties->ezlopi_cloud.item_name = ezlopi_item_name_motion;
        sensor_0060_properties->ezlopi_cloud.device_type = dev_type_sensor_motion;
        sensor_0060_properties->ezlopi_cloud.value_type = value_type_bool;
        sensor_0060_properties->ezlopi_cloud.has_getter = true;
        sensor_0060_properties->ezlopi_cloud.has_setter = false;
        sensor_0060_properties->ezlopi_cloud.reachable = true;
        sensor_0060_properties->ezlopi_cloud.battery_powered = false;
        sensor_0060_properties->ezlopi_cloud.show = true;
        sensor_0060_properties->ezlopi_cloud.room_name[0] = '\0';
        sensor_0060_properties->ezlopi_cloud.device_id = ezlopi_cloud_generate_device_id();
        sensor_0060_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
        sensor_0060_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();

        CJSON_GET_VALUE_INT(cjson_device, "gpio", sensor_0060_properties->interface.gpio.gpio_in.gpio_num);
        CJSON_GET_VALUE_INT(cjson_device, "logic_inv", sensor_0060_properties->interface.gpio.gpio_in.invert);

        sensor_0060_properties->interface.gpio.gpio_in.enable = true;
        sensor_0060_properties->interface.gpio.gpio_in.interrupt = GPIO_INTR_ANYEDGE;
    }
    return sensor_0060_properties;
}
static int sensor_0060_digitalIn_init(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;
    if (GPIO_IS_VALID_GPIO(properties->interface.gpio.gpio_in.gpio_num))
    {
        const gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL << properties->interface.gpio.gpio_in.gpio_num),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_ENABLE,
            .intr_type = properties->interface.gpio.gpio_in.interrupt,
        };

        if (gpio_config(&io_conf))
        {
            properties->interface.gpio.gpio_in.value = gpio_get_level(properties->interface.gpio.gpio_in.gpio_num);
        }

        gpio_isr_service_register(properties, sensor_0060_update_from_device, 200);
    }
    return ret;
}
static void sensor_0060_update_from_device(s_ezlopi_device_properties_t *properties)
{
    int gpio_level = gpio_get_level(properties->interface.gpio.gpio_in.gpio_num);
    properties->interface.gpio.gpio_in.value = (0 == properties->interface.gpio.gpio_in.invert) ? gpio_level : !gpio_level; // (if you want to activate after detecting vibration once and not stop) write --> 1 : 0;
    ezlopi_device_value_updated_from_device(properties);
}
#if 0
static void sensor_0060_digitalIn_get_item(s_ezlopi_device_properties_t *properties, void *args)
{
    cJSON *cjson_propertise = (cJSON *)args;
    if (cjson_propertise)
    {
        int gpio_level = gpio_get_level(properties->interface.gpio.gpio_in.gpio_num);
        properties->interface.gpio.gpio_in.value = (0 == properties->interface.gpio.gpio_in.invert) ? gpio_level : !gpio_level; // (if you want to activate after detecting vibration once and not stop) write --> 1 : 0;
        cJSON_AddBoolToObject(cjson_propertise, "value", properties->interface.gpio.gpio_in.value);
    }
}
#endif
static int sensor_0060_digitalIn_get_value_json(s_ezlopi_device_properties_t *properties, void *args)
{
    int ret = 0;
    char valueFormatted[20];
    cJSON *cjson_propertise = (cJSON *)args;
    if (cjson_propertise)
    {
        snprintf(valueFormatted, 20, "%s", ((0 == properties->interface.gpio.gpio_in.value) ? "false" : "true"));
        cJSON_AddStringToObject(cjson_propertise, "valueFormatted", valueFormatted);
        cJSON_AddBoolToObject(cjson_propertise, "value", properties->interface.gpio.gpio_in.value);
        ret = 1;
    }
    return ret;
}