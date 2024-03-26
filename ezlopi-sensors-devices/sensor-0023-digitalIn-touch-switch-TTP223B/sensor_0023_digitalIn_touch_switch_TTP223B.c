#include "driver/gpio.h"
#include "ezlopi_util_trace.h"

#include "ezlopi_core_timer.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "ezlopi_service_gpioisr.h"

#include "sensor_0023_digitalIn_touch_switch_TTP223B.h"

static int __prepare(void* arg);
static void __prepare_touch_switch_device_cloud_properties(l_ezlopi_device_t* device, cJSON* cj_device);
static void __prepare_touch_switch_properties(l_ezlopi_item_t* item, cJSON* cj_device);
static int __init(l_ezlopi_item_t* item);
static void touch_switch_callback(void* arg);
static int __get_cjson_value(l_ezlopi_item_t* item, void* arg);

/**
 * @brief Public function to interface bme280. This is used to handles all the action on the bme280 sensor and is the entry point to interface the sensor.
 *
 * @param action e_ezlopi_actions_t
 * @param arg Other arguments if needed
 * @return int
 */
int sensor_0023_digitalIn_touch_switch_TTP223B(e_ezlopi_actions_t action, l_ezlopi_item_t* item, void* arg, void* user_arg)
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
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = __get_cjson_value(item, arg);
        break;
    }
    default:
    {
        break;
    }
    }
    return ret;
}

static int __get_cjson_value(l_ezlopi_item_t* item, void* arg)
{
    int ret = 0;
    if (item)
    {
        cJSON* param = (cJSON*)arg;
        if (param)
        {
            cJSON_AddBoolToObject(param, ezlopi_value_str, item->interface.gpio.gpio_in.value);
            const char* valueFormatted = ezlopi_valueformatter_bool(item->interface.gpio.gpio_in.value ? true : false);
            cJSON_AddStringToObject(param, ezlopi_valueFormatted_str, valueFormatted);
        }
    }

    return ret;
}

static int __init(l_ezlopi_item_t* item)
{
    int ret = 0;
    if (item)
    {

        if (GPIO_IS_VALID_GPIO(item->interface.gpio.gpio_in.gpio_num))
        {
            const gpio_config_t touch_switch_config = {
                .pin_bit_mask = (1ULL << item->interface.gpio.gpio_in.gpio_num),
                .mode = GPIO_MODE_INPUT,
                .pull_up_en = GPIO_PULLUP_DISABLE,
                .pull_down_en = (item->interface.gpio.gpio_in.pull == GPIO_PULLDOWN_ONLY) ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE,
                .intr_type = GPIO_INTR_POSEDGE,
            };

            if (0 == gpio_config(&touch_switch_config)) // ESP_OK
            {
                gpio_isr_service_register_v3(item, touch_switch_callback, 200);
                ret = 1;
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

static void touch_switch_callback(void* arg)
{
    l_ezlopi_item_t* item = (l_ezlopi_item_t*)arg;
    item->interface.gpio.gpio_in.value = !item->interface.gpio.gpio_in.value;
    ezlopi_device_value_updated_from_device_v3(item);
}

static int __prepare(void* arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t* prep_arg = (s_ezlopi_prep_arg_t*)arg;

    if (prep_arg && prep_arg->cjson_device)
    {
        l_ezlopi_device_t* touch_device = ezlopi_device_add_device(prep_arg->cjson_device);
        if (touch_device)
        {
            __prepare_touch_switch_device_cloud_properties(touch_device, prep_arg->cjson_device);
            l_ezlopi_item_t* touch_switch_item = ezlopi_device_add_item_to_device(touch_device, sensor_0023_digitalIn_touch_switch_TTP223B);
            if (touch_switch_item)
            {
                __prepare_touch_switch_properties(touch_switch_item, prep_arg->cjson_device);
            }
            else
            {
                ezlopi_device_free_device(touch_device);
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

static void __prepare_touch_switch_device_cloud_properties(l_ezlopi_device_t* device, cJSON* cj_device)
{
    device->cloud_properties.category = category_switch;
    device->cloud_properties.subcategory = subcategory_in_wall;
    device->cloud_properties.device_type = dev_type_switch_outlet;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_touch_switch_properties(l_ezlopi_item_t* item, cJSON* cj_device)
{
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_dev_type_str, item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_switch;
    item->cloud_properties.value_type = value_type_bool;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.scale = NULL;

    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_dev_name_str, item->interface.gpio.gpio_in.gpio_num);
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_ip_inv_str, item->interface.gpio.gpio_in.invert);
    int val_ip = 0;
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_val_ip_str, val_ip);

    item->interface.gpio.gpio_in.value = (true == item->interface.gpio.gpio_in.invert) ? !val_ip : val_ip;
    item->interface.gpio.gpio_in.enable = true;
    item->interface.gpio.gpio_in.interrupt = GPIO_INTR_POSEDGE;
    item->interface.gpio.gpio_in.pull = GPIO_PULLDOWN_ONLY;
}
