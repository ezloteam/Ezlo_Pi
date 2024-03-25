#include "ezlopi_util_trace.h"

#include "ezlopi_core_timer.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_hal_gpio.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "ezlopi_service_gpioisr.h"

#include "sensor_0065_digitalIn_float_switch.h"
//-----------------------------------------------------------------------
const char* water_level_alarm_token[] = {
    "water_level_ok",
    "water_level_below_low_threshold",
    "water_level_above_high_threshold",
    "unknown",
};
//-----------------------------------------------------------------------
static int __0065_prepare(void* arg);
static int __0065_init(l_ezlopi_item_t* item);
static int __0065_get_item(l_ezlopi_item_t* item, void* arg);
static int __0065_get_cjson_value(l_ezlopi_item_t* item, void* arg);

static void __0065_update_from_device(void* arg);
static void __prepare_item_cloud_properties(l_ezlopi_item_t* item, cJSON* cj_device);
static void __prepare_device_cloud_properties(l_ezlopi_device_t* device, cJSON* cj_device);
//-----------------------------------------------------------------------

int sensor_0065_digitalIn_float_switch(e_ezlopi_actions_t action, l_ezlopi_item_t* item, void* arg, void* user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = __0065_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = __0065_init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    {
        ret = __0065_get_item(item, arg);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = __0065_get_cjson_value(item, arg);
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
static void __prepare_device_cloud_properties(l_ezlopi_device_t* device, cJSON* cj_device)
{
    device->cloud_properties.category = category_level_sensor;
    device->cloud_properties.subcategory = subcategory_water;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}
static void __prepare_item_cloud_properties(l_ezlopi_item_t* item, cJSON* cj_device)
{
    item->cloud_properties.show = true;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_water_level_alarm;
    item->cloud_properties.value_type = value_type_token;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_type_str, item->interface_type); // _max = 10
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_str, item->interface.gpio.gpio_in.gpio_num);
    CJSON_GET_VALUE_INT(cj_device, "logic_inv", item->interface.gpio.gpio_in.invert);

    item->interface.gpio.gpio_in.enable = true;
    item->interface.gpio.gpio_in.mode = GPIO_MODE_INPUT;
    item->interface.gpio.gpio_in.value = 0;
    item->interface.gpio.gpio_in.pull = GPIO_PULLDOWN_ONLY;
    item->interface.gpio.gpio_in.interrupt = GPIO_INTR_ANYEDGE;
}
static int __0065_prepare(void* arg)
{
    int ret = 0;
    if (arg)
    {
        s_ezlopi_prep_arg_t* device_prep_arg = (s_ezlopi_prep_arg_t*)arg;
        if (device_prep_arg && (NULL != device_prep_arg->cjson_device))
        {
            l_ezlopi_device_t* float_device = ezlopi_device_add_device(device_prep_arg->cjson_device);
            if (float_device)
            {
                __prepare_device_cloud_properties(float_device, device_prep_arg->cjson_device);
                l_ezlopi_item_t* float_item = ezlopi_device_add_item_to_device(float_device, sensor_0065_digitalIn_float_switch);
                if (float_item)
                {
                    __prepare_item_cloud_properties(float_item, device_prep_arg->cjson_device);
                    ret = 1;
                }
                else
                {
                    ret = -1;
                    ezlopi_device_free_device(float_device);
                }
            }
        }
        ret = 1;
    }
    return ret;
}

static int __0065_init(l_ezlopi_item_t* item)
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

            if (0 == gpio_config(&input_conf))
            {
                ret = 1;
                item->interface.gpio.gpio_in.value = gpio_get_level(item->interface.gpio.gpio_in.gpio_num);
                gpio_isr_service_register_v3(item, __0065_update_from_device, 200);
            }
            else
            {
                ret = -1;
            }
        }
        else
        {
            ret = -1;
        }
    }
    return ret;
}
static int __0065_get_item(l_ezlopi_item_t* item, void* arg)
{
    int ret = 0;
    if (item && arg)
    {
        cJSON* cj_result = (cJSON*)arg;
        if (cj_result)
        {
            //-------------------  POSSIBLE JSON ENUM LPGNTENTS ----------------------------------
            cJSON* json_array_enum = cJSON_CreateArray();
            if (NULL != json_array_enum)
            {
                for (uint8_t i = 0; i < WATER_LEVEL_ALARM_MAX; i++)
                {
                    cJSON* json_value = cJSON_CreateString(water_level_alarm_token[i]);
                    if (NULL != json_value)
                    {
                        cJSON_AddItemToArray(json_array_enum, json_value);
                    }
                }
                cJSON_AddItemToObject(cj_result, ezlopi_enum_str, json_array_enum);
            }
            //--------------------------------------------------------------------------------------

            cJSON_AddStringToObject(cj_result, ezlopi_valueFormatted_str, (char*)item->user_arg ? item->user_arg : "water_level_ok");
            cJSON_AddStringToObject(cj_result, ezlopi_value_str, (char*)item->user_arg ? item->user_arg : "water_level_ok");
            ret = 1;
        }
    }
    return ret;
}

static int __0065_get_cjson_value(l_ezlopi_item_t* item, void* arg)
{
    int ret = 0;
    if (item && arg)
    {
        cJSON* cj_result = (cJSON*)arg;
        if (cj_result)
        {
            cJSON_AddStringToObject(cj_result, ezlopi_valueFormatted_str, (char*)item->user_arg ? item->user_arg : "water_level_ok");
            cJSON_AddStringToObject(cj_result, ezlopi_value_str, (char*)item->user_arg ? item->user_arg : "water_level_ok");
            ret = 1;
        }
    }
    return ret;
}

//------------------------------------------------------------------------------------------------------------
static void __0065_update_from_device(void* arg)
{
    l_ezlopi_item_t* item = (l_ezlopi_item_t*)arg;
    if (item)
    {
        const char* curret_value = NULL;
        item->interface.gpio.gpio_in.value = gpio_get_level(item->interface.gpio.gpio_in.gpio_num);
        item->interface.gpio.gpio_in.value = (false == item->interface.gpio.gpio_in.invert) ? (item->interface.gpio.gpio_in.value) : (!item->interface.gpio.gpio_in.value);
        if (0 == (item->interface.gpio.gpio_in.value)) // when D0 -> 0V,
        {
            curret_value = "water_level_ok";
        }
        else
        {
            curret_value = "water_level_above_high_threshold";
        }
        if (curret_value != (char*)item->user_arg) // calls update only if there is change in state
        {
            item->user_arg = (void*)curret_value;
            ezlopi_device_value_updated_from_device_v3(item);
        }
    }
}