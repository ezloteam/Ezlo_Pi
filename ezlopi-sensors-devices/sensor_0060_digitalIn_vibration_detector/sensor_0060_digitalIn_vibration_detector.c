#include "ezlopi_util_trace.h"
// #include "cJSON.h"

#include "ezlopi_core_timer.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_hal_gpio.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "ezlopi_service_gpioisr.h"
#include "sensor_0060_digitalIn_vibration_detector.h"

//---------------------------------------------------------------------------------------------------------
const char *Sw420_vibration_activity_state_token[] = {
    "no_activity",
    "shake",
    "tilt",
    "drop",
};

static int __0060_prepare(void *arg);
static int __0060_init(l_ezlopi_item_t *item);
static int __0060_get_item(l_ezlopi_item_t *item, void *arg);
static int __0060_get_cjson_value(l_ezlopi_item_t *item, void *arg);
static int __0060_notify(l_ezlopi_item_t *item);

static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device);
//---------------------------------------------------------------------------------------------------------
int sensor_0060_digitalIn_vibration_detector(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
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
    case EZLOPI_ACTION_HUB_GET_ITEM:
    {
        __0060_get_item(item, arg);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        __0060_get_cjson_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        __0060_notify(item);
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
    // char *dev_name = NULL;
    // CJSON_GET_VALUE_STRING(cj_device, ezlopi_dev_name_str, dev_name);
    // ASSIGN_DEVICE_NAME_V2(device, dev_name);
    // device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();

    device->cloud_properties.category = category_security_sensor;
    device->cloud_properties.subcategory = subcategory_motion;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}
static void __prepare_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    item->cloud_properties.show = true;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_activity;
    item->cloud_properties.value_type = value_type_token;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_type_str, item->interface_type); // _max = 10
    CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_name_str, item->interface.gpio.gpio_in.gpio_num);
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
            l_ezlopi_device_t *vibration_device = ezlopi_device_add_device(dev_prep_arg->cjson_device);
            if (vibration_device)
            {
                __prepare_device_cloud_properties(vibration_device, dev_prep_arg->cjson_device);
                l_ezlopi_item_t *vibration_item = ezlopi_device_add_item_to_device(vibration_device, sensor_0060_digitalIn_vibration_detector);
                if (vibration_item)
                {
                    vibration_item->cloud_properties.device_id = vibration_device->cloud_properties.device_id;
                    __prepare_item_cloud_properties(vibration_item, dev_prep_arg->cjson_device);
                }
                else
                {
                    ezlopi_device_free_device(vibration_device);
                }
            }
        }
        ret = 1;
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
            //-------------------  POSSIBLE JSON ENUM LPGNTENTS ----------------------------------
            cJSON *json_array_enum = cJSON_CreateArray();
            if (NULL != json_array_enum)
            {
                for (uint8_t i = 0; i < SW420_VIBRATION_ACTIVITY_MAX; i++)
                {
                    cJSON *json_value = cJSON_CreateString(Sw420_vibration_activity_state_token[i]);
                    if (NULL != json_value)
                    {
                        cJSON_AddItemToArray(json_array_enum, json_value);
                    }
                }
                cJSON_AddItemToObject(cj_result, ezlopi_enum_str, json_array_enum);
            }
            //--------------------------------------------------------------------------------------

            cJSON_AddStringToObject(cj_result, ezlopi_valueFormatted_str, (char *)item->user_arg ? item->user_arg : "no_activity");
            cJSON_AddStringToObject(cj_result, ezlopi_value_str, (char *)item->user_arg ? item->user_arg : "no_activity");
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
            cJSON_AddStringToObject(cj_result, ezlopi_valueFormatted_str, (char *)item->user_arg ? item->user_arg : "no_activity");
            cJSON_AddStringToObject(cj_result, ezlopi_value_str, (char *)item->user_arg ? item->user_arg : "no_activity");
            ret = 1;
        }
    }
    return ret;
}
//------------------------------------------------------------------------------------------------------------
static int __0060_notify(l_ezlopi_item_t *item)
{
    int ret = 0;
    const char *curret_value = NULL;
    item->interface.gpio.gpio_in.value = gpio_get_level(item->interface.gpio.gpio_in.gpio_num);
    item->interface.gpio.gpio_in.value = (false == item->interface.gpio.gpio_in.invert) ? (item->interface.gpio.gpio_in.value) : (!item->interface.gpio.gpio_in.value);

    if (0 == (item->interface.gpio.gpio_in.value)) // when D0 -> 0V,
    {
        curret_value = Sw420_vibration_activity_state_token[0]; //"no_activity";
    }
    else
    {
        curret_value = Sw420_vibration_activity_state_token[1]; //"shake";
    }

    if (curret_value != (char *)item->user_arg) // calls update only if there is change in state
    {
        item->user_arg = (void *)curret_value;
        ezlopi_device_value_updated_from_device_v3(item);
        ret = 1;
    }

    return ret;
}