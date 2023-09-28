#include "0035_sensor_touch_sensor_tpp_223b.h"
#include "gpio_isr_service.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"

#include "ezlopi_cloud.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "items.h"
#include "trace.h"
#include "cJSON.h"

static bool sensor_bme280_initialized = false;

static int __prepare(void *arg);
static void __prepare_touch_sensor_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_touch_sensor_properties(l_ezlopi_item_t *item, cJSON *cj_device);
static int __init(l_ezlopi_item_t *item);
static void touch_switch_callback(void *arg);
static int __get_cjson_value(l_ezlopi_item_t *item, void *arg);

/**
 * @brief Public function to interface bme280. This is used to handles all the action on the bme280 sensor and is the entry point to interface the sensor.
 *
 * @param action e_ezlopi_actions_t
 * @param arg Other arguments if needed
 * @return int
 */
int sensor_touch_sensor_ttp_223b_v3(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        __prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        __init(item);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        __get_cjson_value(item, arg);
        break;
    }
    default:
    {
        break;
    }
    }
    return 0;
}

static int __get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;

    cJSON *param = (cJSON *)arg;
    if (param)
    {
        cJSON_AddStringToObject(param, "valueFormatted", (true == (item->interface.gpio.gpio_in.value) ? "true" : "false"));
        cJSON_AddBoolToObject(param, "value", item->interface.gpio.gpio_in.value);
    }

    return ret;
}

static int __init(l_ezlopi_item_t *item)
{
    int ret = 0;

    const gpio_config_t touch_switch_config = {
        .pin_bit_mask = (1ULL << item->interface.gpio.gpio_in.gpio_num),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = (item->interface.gpio.gpio_in.pull == GPIO_PULLDOWN_ONLY) ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE,
        .intr_type = item->interface.gpio.gpio_in.interrupt,
    };

    ESP_ERROR_CHECK(gpio_config(&touch_switch_config));

    int gpio_level = gpio_get_level(item->interface.gpio.gpio_in.gpio_num);
    item->interface.gpio.gpio_in.value = (false == item->interface.gpio.gpio_in.invert) ? gpio_level : !gpio_level;
    gpio_isr_service_register_v3(item, touch_switch_callback, 200);

    return ret;
}

static void touch_switch_callback(void *arg)
{
    l_ezlopi_item_t *item = (l_ezlopi_item_t *)arg;
    int gpio_level = gpio_get_level(item->interface.gpio.gpio_in.gpio_num);
    item->interface.gpio.gpio_in.value = (false == item->interface.gpio.gpio_in.invert) ? gpio_level : !gpio_level;
    ezlopi_device_value_updated_from_device_v3(item);
}

static int __prepare(void *arg)
{
    int ret = 0;

    s_ezlopi_prep_arg_t *prep_arg = (cJSON *)arg;
    if (prep_arg && prep_arg->cjson_device)
    {
        l_ezlopi_device_t *touch_device = ezlopi_device_add_device();
        if (touch_device)
        {
            __prepare_touch_sensor_device_cloud_properties(touch_device, prep_arg->cjson_device);
            l_ezlopi_item_t *touch_switch_item = ezlopi_device_add_item_to_device(touch_device, sensor_touch_sensor_ttp_223b_v3);
            if (touch_switch_item)
            {
                __prepare_touch_sensor_properties(touch_switch_item, prep_arg->cjson_device);
            }
            else
            {
                ezlopi_device_free_device(touch_device);
            }
        }
        else
        {
            ezlopi_device_free_device(touch_device);
        }
    }

    return ret;
}

static void __prepare_touch_sensor_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    char *device_name = NULL;
    CJSON_GET_VALUE_STRING(cj_device, "dev_name", device_name);
    ASSIGN_DEVICE_NAME_V2(device, device_name);

    device->cloud_properties.category = category_switch;
    device->cloud_properties.subcategory = subcategory_in_wall;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
}

static void __prepare_touch_sensor_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.scale = true;
    item->cloud_properties.item_name = ezlopi_item_name_switch;
    item->cloud_properties.value_type = value_type_bool;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.scale = NULL;

    CJSON_GET_VALUE_INT(cj_device, "gpio", item->interface.gpio.gpio_in.gpio_num);
    CJSON_GET_VALUE_INT(cj_device, "ip_inv", item->interface.gpio.gpio_in.invert);
    CJSON_GET_VALUE_INT(cj_device, "val_ip", item->interface.gpio.gpio_in.value);

    item->interface.gpio.gpio_in.enable = true;
    item->interface.gpio.gpio_in.interrupt = GPIO_INTR_ANYEDGE;
    item->interface.gpio.gpio_in.pull = GPIO_PULLDOWN_ONLY;
}