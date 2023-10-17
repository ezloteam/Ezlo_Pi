#include "trace.h"
#include "items.h"
#include "cJSON.h"
#include "gpio_isr_service.h"

#include "ezlopi_actions.h"
#include "ezlopi_timer.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "ezlopi_cloud_scales_str.h"
#include "ezlopi_gpio.h"

#include "0060_sensor_digitalIn_vibration_detector.h"
//---------------------------------------------------------------------------------------------------------
static int __0060_prepare(void *arg);
static int __0060_init(l_ezlopi_item_t *item);
static int __0060_get_item(l_ezlopi_item_t *item, void *arg);
static int __0060_get_cjson_value(l_ezlopi_item_t *item, void *arg);
static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device);
static void _0060_update_from_device(l_ezlopi_item_t *item);
//---------------------------------------------------------------------------------------------------------
int sensor_vibration_detector_v3(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        __0060_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        __0060_init(item);
        break;
    }
    // case EZLOPI_ACTION_HUB_GET_ITEM:
    // {
    //     __0060_get_item(item, arg);
    //     break;
    // }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        __0060_get_cjson_value(item, arg);
        break;
    }
    default:
    {
        break;
    }
    }
    return ret;
}
//---------------------------------------------------------------------------------------------------------

static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    char *dev_name = NULL;
    CJSON_GET_VALUE_STRING(cj_device, "dev_name", dev_name);
    ASSIGN_DEVICE_NAME_V2(device, dev_name);
    device->cloud_properties.category = category_security_sensor;
    device->cloud_properties.subcategory = subcategory_motion;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
}
static void __prepare_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    item->cloud_properties.show = true;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_motion;
    item->cloud_properties.value_type = value_type_bool;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type); // _max = 10
    CJSON_GET_VALUE_INT(cj_device, "gpio", item->interface.gpio.gpio_in.gpio_num);
    CJSON_GET_VALUE_INT(cj_device, "logic_inv", item->interface.gpio.gpio_in.invert);

    item->interface.gpio.gpio_in.enable = true;
    item->interface.gpio.gpio_in.mode = GPIO_MODE_INPUT;
    item->interface.gpio.gpio_in.value = 0;
    item->interface.gpio.gpio_in.pull = GPIO_PULLDOWN_ONLY;
    item->interface.gpio.gpio_in.interrupt = GPIO_INTR_ANYEDGE;
}
static int __0060_prepare(void *arg)
{
    int ret = 0;
    if (arg)
    {
        s_ezlopi_prep_arg_t *dev_prep_arg = (s_ezlopi_prep_arg_t *)arg;
        if (dev_prep_arg && (NULL != dev_prep_arg->cjson_device))
        {
            l_ezlopi_device_t *vibration_device = ezlopi_device_add_device();
            if (vibration_device)
            {
                __prepare_device_cloud_properties(vibration_device, dev_prep_arg->cjson_device);
                l_ezlopi_item_t *vibration_item = ezlopi_device_add_item_to_device(vibration_device, sensor_vibration_detector_v3);
                if (vibration_item)
                {
                    __prepare_item_cloud_properties(vibration_item, dev_prep_arg->cjson_device);
                }
                else
                {
                    ezlopi_device_free_device(vibration_device);
                }
            }
            else
            {
                ezlopi_device_free_device(vibration_device);
            }
            ret = 1;
        }
    }
    return ret;
}

static int __0060_init(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (NULL != item)
    {
        if (GPIO_IS_VALID_GPIO(item->interface.gpio.gpio_in.gpio_num))
        {
            const gpio_config_t io_config = {
                .pin_bit_mask = (1ULL << (item->interface.gpio.gpio_in.gpio_num)),
                .mode = item->interface.gpio.gpio_in.mode,
                .pull_down_en = ((item->interface.gpio.gpio_in.pull == GPIO_PULLDOWN_ONLY) ||
                                 (item->interface.gpio.gpio_in.pull == GPIO_PULLUP_PULLDOWN))
                                    ? GPIO_PULLDOWN_ENABLE
                                    : GPIO_PULLDOWN_DISABLE,
                .pull_up_en = ((item->interface.gpio.gpio_in.pull == GPIO_PULLUP_ONLY) ||
                               (item->interface.gpio.gpio_in.pull == GPIO_PULLUP_PULLDOWN))
                                  ? GPIO_PULLUP_ENABLE
                                  : GPIO_PULLUP_DISABLE,
                .intr_type = item->interface.gpio.gpio_in.interrupt,
            };
            ret = gpio_config(&io_config);
            if (ret)
            {
                TRACE_E("Error initializing Vibration sensor");
            }
            else
            {
                item->interface.gpio.gpio_in.value = gpio_get_level(item->interface.gpio.gpio_in.gpio_num);
                gpio_isr_service_register_v3(item, _0060_update_from_device, 200);
            }
        }
    }
    return ret;
}

static int __0060_get_item(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;
    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        if (cj_result)
        {
            cJSON_AddStringToObject(cj_result, "valueFormatted", (true == (item->interface.gpio.gpio_in.value) ? "true" : "false"));
            cJSON_AddBoolToObject(cj_result, "value", item->interface.gpio.gpio_in.value);
            ret = 1;
        }
    }
    return ret;
}

static int __0060_get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;
    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        if (cj_result)
        {
            cJSON_AddStringToObject(cj_result, "valueFormatted", (true == (item->interface.gpio.gpio_in.value) ? "true" : "false"));
            cJSON_AddBoolToObject(cj_result, "value", item->interface.gpio.gpio_in.value);
            ret = 1;
        }
    }
    return ret;
}
//------------------------------------------------------------------------------------------------------------
static void _0060_update_from_device(l_ezlopi_item_t *item)
{
    if (item)
    {
        int gpio_level = gpio_get_level(item->interface.gpio.gpio_in.gpio_num);
        item->interface.gpio.gpio_in.value = (0 == item->interface.gpio.gpio_in.invert) ? gpio_level : !gpio_level;
        ezlopi_device_value_updated_from_device_v3(item);
    }
}