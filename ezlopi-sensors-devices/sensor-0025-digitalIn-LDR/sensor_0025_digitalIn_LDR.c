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

static int __prepare(void *arg);
static int __init(l_ezlopi_item_t *item);
static int __get_value_cjson(l_ezlopi_item_t *item, void *arg);
static void __setup_item_properties(l_ezlopi_item_t *item, cJSON *cj_device);
static void __setup_device_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __value_updated_from_interrupt(void *arg);

int sensor_0025_digitalIn_LDR(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
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
        ret = __get_value_cjson(item, arg); // updater function missing
        break;
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
        cJSON *cj_value_obj = (cJSON *)arg;
        int gpio_level = gpio_get_level(item->interface.gpio.gpio_in.gpio_num);
        item->interface.gpio.gpio_in.value = (0 == item->interface.gpio.gpio_in.invert) ? gpio_level : !gpio_level;
        cJSON_AddBoolToObject(cj_value_obj, ezlopi_value_str, item->interface.gpio.gpio_in.value);
        const char *valueFormatted = ezlopi_valueformatter_bool(item->interface.gpio.gpio_in.value ? true : false);
        cJSON_AddStringToObject(cj_value_obj, ezlopi_valueFormatted_str, valueFormatted);
        ret = 1;
    }
    return ret;
}

static int __init(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (item)
    {
        if (GPIO_IS_VALID_GPIO(item->interface.gpio.gpio_in.gpio_num))
        {
            const gpio_config_t io_conf = {
                .pin_bit_mask = (1ULL << item->interface.gpio.gpio_in.gpio_num),
                .mode = GPIO_MODE_INPUT,
                .pull_up_en = GPIO_PULLUP_DISABLE,
                .pull_down_en = (item->interface.gpio.gpio_in.pull == GPIO_PULLDOWN_ONLY) ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE,
                .intr_type = GPIO_INTR_ANYEDGE,
            };

            if (ESP_OK == gpio_config(&io_conf))
            {
                item->interface.gpio.gpio_in.value = gpio_get_level(item->interface.gpio.gpio_in.gpio_num);
            }

            gpio_isr_service_register_v3(item, __value_updated_from_interrupt, 200);
            ret = 1;
        }
        if (0 == ret)
        {
            ret = -1;
            if (item->user_arg)
            {
                free(item->user_arg);
                item->user_arg = NULL;
            }
        }
    }
    return ret;
}

static void __value_updated_from_interrupt(void *arg)
{
    if (arg)
    {
        ezlopi_device_value_updated_from_device_v3((l_ezlopi_item_t *)arg);
    }
}

static int __prepare(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (prep_arg)
    {
        cJSON *cj_device = prep_arg->cjson_device;
        if (cj_device)
        {
            l_ezlopi_device_t *device = ezlopi_device_add_device(prep_arg->cjson_device);
            if (device)
            {
                __setup_device_properties(device, cj_device);
                l_ezlopi_item_t *item = ezlopi_device_add_item_to_device(device, NULL);
                if (item)
                {
                    item->func = sensor_0025_digitalIn_LDR;
                    __setup_item_properties(item, cj_device);
                    ret = 1;
                }
                else
                {
                    ezlopi_device_free_device(device);
                    ret = -1;
                }
            }
        }
    }

    return ret;
}

static void __setup_item_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    item->cloud_properties.show = true;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_switch;
    item->cloud_properties.value_type = value_type_bool;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_type_str, item->interface_type);
    CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_name_str, item->interface.gpio.gpio_in.gpio_num);
    CJSON_GET_VALUE_INT(cj_device, "logic_inv", item->interface.gpio.gpio_in.invert);
    CJSON_GET_VALUE_INT(cj_device, ezlopi_val_ip_str, item->interface.gpio.gpio_in.value);
    item->interface.gpio.gpio_in.enable = true;
    item->interface.gpio.gpio_in.interrupt = GPIO_INTR_ANYEDGE;
    item->interface.gpio.gpio_in.pull = GPIO_PULLDOWN_ONLY;
}

static void __setup_device_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    // char *device_name = NULL;
    // CJSON_GET_VALUE_STRING(cj_device, ezlopi_dev_name_str, device_name);
    // ASSIGN_DEVICE_NAME_V2(device, device_name);
    // device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();

    device->cloud_properties.category = category_switch;
    device->cloud_properties.subcategory = subcategory_in_wall;
    device->cloud_properties.device_type = dev_type_switch_outlet;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}
