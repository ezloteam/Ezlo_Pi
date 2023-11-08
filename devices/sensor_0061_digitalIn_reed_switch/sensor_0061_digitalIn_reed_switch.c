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

#include "sensor_0061_digitalIn_reed_switch.h"
//-----------------------------------------------------------------------
const char *reed_door_window_states[] =
    {
        "dw_is_opened",
        "dw_is_closed",
        "unknown"};

static int __0061_prepare(void *arg);
static int __0061_init(l_ezlopi_item_t *item);
static int __0061_get_item(l_ezlopi_item_t *item, void *arg);
static int __0061_get_cjson_value(l_ezlopi_item_t *item, void *arg);
static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device);
static void _0061_update_from_device(l_ezlopi_item_t *item);
//-----------------------------------------------------------------------

int sensor_0061_digitalIn_reed_switch(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        __0061_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        __0061_init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    {
        __0061_get_item(item, arg);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        __0061_get_cjson_value(item, arg);
        break;
    }
    default:
    {
        break;
    }
    }
    return ret;
}

//----------------------------------------------------------------------------------------
static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    char *device_name = NULL;
    CJSON_GET_VALUE_STRING(cj_device, "dev_name", device_name);
    ASSIGN_DEVICE_NAME_V2(device, device_name);
    device->cloud_properties.category = category_security_sensor;
    device->cloud_properties.subcategory = subcategory_door;
    device->cloud_properties.device_type = dev_type_doorlock;
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
}
static void __prepare_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    item->cloud_properties.show = true;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_dw_state;
    item->cloud_properties.value_type = value_type_token;
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
static int __0061_prepare(void *arg)
{
    int ret = 0;
    if (arg)
    {
        s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)arg;
        if (device_prep_arg && (NULL != device_prep_arg->cjson_device))
        {
            l_ezlopi_device_t *reed_device = ezlopi_device_add_device();
            if (reed_device)
            {
                __prepare_device_cloud_properties(reed_device, device_prep_arg->cjson_device);
                l_ezlopi_item_t *reed_item = ezlopi_device_add_item_to_device(reed_device, sensor_0061_digitalIn_reed_switch);
                if (reed_item)
                {
                    reed_item->cloud_properties.device_id = reed_device->cloud_properties.device_id;
                    __prepare_item_cloud_properties(reed_item, device_prep_arg->cjson_device);
                    // if you want to add a custom data_structure , add here
                }
                else
                {
                    ezlopi_device_free_device(reed_device);
                }
            }
            else
            {
                ezlopi_device_free_device(reed_device);
            }
        }
        ret = 1;
    }
    return ret;
}

static int __0061_init(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (NULL != item)
    {
        if (GPIO_IS_VALID_GPIO(item->interface.gpio.gpio_in.gpio_num))
        {
            // intialize digital_pin
            gpio_config_t input_conf = {
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
            ret = gpio_config(&input_conf);
            if (ret)
            {
                TRACE_E("Error initializing Reed switch");
            }
            else
            {
                item->interface.gpio.gpio_in.value = gpio_get_level(item->interface.gpio.gpio_in.gpio_num);
                gpio_isr_service_register_v3(item, _0061_update_from_device, 200);
            }
        }
    }
    return ret;
}

static int __0061_get_item(l_ezlopi_item_t *item, void *arg)
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
                for (uint8_t i = 0; i < REED_DOOR_WINDOW_MAX; i++)
                {
                    cJSON *json_value = cJSON_CreateString(reed_door_window_states[i]);
                    if (NULL != json_value)
                    {
                        cJSON_AddItemToArray(json_array_enum, json_value);
                    }
                }
                cJSON_AddItemToObject(cj_result, "enum", json_array_enum);
            }
            //--------------------------------------------------------------------------------------

            cJSON_AddStringToObject(cj_result, "valueFormatted", (char *)item->user_arg ? item->user_arg : "dw_is_closed");
            cJSON_AddStringToObject(cj_result, "value", (char *)item->user_arg ? item->user_arg : "dw_is_closed");
            ret = 1;
        }
    }
    return ret;
}
static int __0061_get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;
    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        if (cj_result)
        {
            cJSON_AddStringToObject(cj_result, "valueFormatted", (char *)item->user_arg ? item->user_arg : "dw_is_closed");
            cJSON_AddStringToObject(cj_result, "value", (char *)item->user_arg ? item->user_arg : "dw_is_closed");
            ret = 1;
        }
    }
    return ret;
}

//------------------------------------------------------------------------------------------------------------
static void _0061_update_from_device(l_ezlopi_item_t *item)
{
    if (item)
    {
        char *curret_value = NULL;
        item->interface.gpio.gpio_in.value = gpio_get_level(item->interface.gpio.gpio_in.gpio_num);
        item->interface.gpio.gpio_in.value = (false == item->interface.gpio.gpio_in.invert) ? (item->interface.gpio.gpio_in.value) : (!item->interface.gpio.gpio_in.value);
        if (0 == (item->interface.gpio.gpio_in.value)) // when D0 -> 0V,
        {
            curret_value = reed_door_window_states[0]; //"dw_is_opened";
        }
        else
        {
            curret_value = reed_door_window_states[1]; //"dw_is_closed";
        }
        if (curret_value != (char *)item->user_arg) // calls update only if there is change in state
        {
            item->user_arg = (void *)curret_value;
            ezlopi_device_value_updated_from_device_v3(item);
        }
    }
}