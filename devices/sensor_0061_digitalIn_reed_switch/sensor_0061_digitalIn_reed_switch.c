#include "trace.h"
#include "ezlopi_cloud.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_device_value_updated.h"
#include "gpio_isr_service.h"

#include "sensor_0061_digitalIn_reed_switch.h"

//----------------------------------------------------------------------

static s_ezlopi_device_properties_t *sensor_0061_prepare_properties(cJSON *cjson_device);
static int sensor_0061_prepare_and_add(void *arg);
static int sensor_0061_init(s_ezlopi_device_properties_t *properties);
static void sensor_0061_get_item(s_ezlopi_device_properties_t *properties, void *arg);
static int sensor_0061_get_value_cjson(s_ezlopi_device_properties_t *properties, void *arg);
static void sensor_0061_update_from_device(s_ezlopi_device_properties_t *properties);
//----------------------------------------------------------------------

int sensor_0061_digitalIn_reed_switch(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg)
{
    int ret = 0;
    // TRACE_E("Action : %s", ezlopi_actions_to_string(action));
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
        ret = sensor_0061_prepare_and_add(arg);
        /* code */
        break;
    case EZLOPI_ACTION_INITIALIZE:
        ret = sensor_0061_init(properties);
        /* code */
        break;
    case EZLOPI_ACTION_HUB_GET_ITEM:
    {
        sensor_0061_get_item(properties, arg);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
        ret = sensor_0061_get_value_cjson(properties, arg);
        /* code */
        break;

    default:
        break;
    }
    return ret;
}
//----------------------------------------------------------------------

static s_ezlopi_device_properties_t *sensor_0061_prepare_properties(cJSON *cjson_device)
{
    s_ezlopi_device_properties_t *sensor_0061_device_properties = malloc(sizeof(s_ezlopi_device_properties_t));

    if (sensor_0061_device_properties)
    {
        memset(sensor_0061_device_properties, 0, sizeof(s_ezlopi_device_properties_t));
        sensor_0061_device_properties->interface_type = EZLOPI_DEVICE_INTERFACE_DIGITAL_INPUT;

        char *device_name = NULL;
        CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
        ASSIGN_DEVICE_NAME(sensor_0061_device_properties, device_name);
        sensor_0061_device_properties->ezlopi_cloud.category = category_switch;
        sensor_0061_device_properties->ezlopi_cloud.subcategory = subcategory_relay;
        sensor_0061_device_properties->ezlopi_cloud.item_name = ezlopi_item_name_switch;
        sensor_0061_device_properties->ezlopi_cloud.device_type = dev_type_sensor;
        sensor_0061_device_properties->ezlopi_cloud.value_type = value_type_bool;
        sensor_0061_device_properties->ezlopi_cloud.has_getter = true;
        sensor_0061_device_properties->ezlopi_cloud.has_setter = false;
        sensor_0061_device_properties->ezlopi_cloud.reachable = true;
        sensor_0061_device_properties->ezlopi_cloud.battery_powered = false;
        sensor_0061_device_properties->ezlopi_cloud.show = true;
        sensor_0061_device_properties->ezlopi_cloud.room_name[0] = '\0';
        sensor_0061_device_properties->ezlopi_cloud.device_id = ezlopi_cloud_generate_device_id();
        sensor_0061_device_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
        sensor_0061_device_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();

        CJSON_GET_VALUE_INT(cjson_device, "gpio", sensor_0061_device_properties->interface.gpio.gpio_in.gpio_num);
        CJSON_GET_VALUE_INT(cjson_device, "logic_inv", sensor_0061_device_properties->interface.gpio.gpio_in.invert);

        sensor_0061_device_properties->interface.gpio.gpio_in.enable = true;
        sensor_0061_device_properties->interface.gpio.gpio_in.interrupt = GPIO_INTR_ANYEDGE;
        sensor_0061_device_properties->interface.gpio.gpio_in.pull = GPIO_PULLDOWN_ONLY;
    }
    return sensor_0061_device_properties;
}
//----------------------------------------------------------------------

static int sensor_0061_prepare_and_add(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if ((NULL != device_prep_arg) && (NULL != device_prep_arg->cjson_device))
    {
        s_ezlopi_device_properties_t *sensor_0061_properties = sensor_0061_prepare_properties(device_prep_arg->cjson_device);
        if (sensor_0061_properties)
        {
            if (0 == ezlopi_devices_list_add(device_prep_arg->device, sensor_0061_properties, NULL))
            {
                free(sensor_0061_properties);
            }
            else
            {
                ret = 1;
            }
        }
    }
    return ret;
}
//----------------------------------------------------------------------

static int sensor_0061_init(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;
    static bool guard = false;
    if (!guard)
    {
        if (GPIO_IS_VALID_GPIO(properties->interface.gpio.gpio_in.gpio_num))
        {
            gpio_config_t io_conf = {
                .pin_bit_mask = (1ULL << properties->interface.gpio.gpio_in.gpio_num),
                .mode = GPIO_MODE_INPUT,
                .pull_up_en = GPIO_PULLUP_DISABLE,
                .pull_down_en = (properties->interface.gpio.gpio_in.pull == GPIO_PULLDOWN_ONLY) ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE,
                .intr_type = properties->interface.gpio.gpio_in.pull,
            };

            if (ESP_OK == gpio_config(&io_conf))
            {
                properties->interface.gpio.gpio_in.value = gpio_get_level(properties->interface.gpio.gpio_in.gpio_num);
            }

            gpio_isr_service_register(properties, sensor_0061_update_from_device, 200);
        }
        guard = true;
    }

    return ret;
}
//----------------------------------------------------------------------

static void sensor_0061_get_item(s_ezlopi_device_properties_t *properties, void *arg)
{
    cJSON *cjson_propertise = (cJSON *)arg;
    if (cjson_propertise)
    {
        // int gpio_level = gpio_get_level(properties->interface.gpio.gpio_in.gpio_num);
        // properties->interface.gpio.gpio_in.value = (0 == properties->interface.gpio.gpio_in.invert) ? gpio_level : !gpio_level;
        cJSON_AddBoolToObject(cjson_propertise, "value", properties->interface.gpio.gpio_in.value);
    }
}

static int sensor_0061_get_value_cjson(s_ezlopi_device_properties_t *properties, void *arg)
{
    int ret = 0;
    cJSON *cjson_propertise = (cJSON *)arg;
    if (cjson_propertise)
    {
        cJSON_AddBoolToObject(cjson_propertise, "value", properties->interface.gpio.gpio_in.value);
        ret = 1;
    }
    return ret;
}
//----------------------------------------------------------------------

static void sensor_0061_update_from_device(s_ezlopi_device_properties_t *properties)
{
    int gpio_level = gpio_get_level(properties->interface.gpio.gpio_in.gpio_num);
    properties->interface.gpio.gpio_in.value = (0 == properties->interface.gpio.gpio_in.invert) ? gpio_level : !gpio_level;
    ezlopi_device_value_updated_from_device(properties);
}