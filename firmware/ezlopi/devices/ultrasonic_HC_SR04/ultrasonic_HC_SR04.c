#include "stdlib.h"
#include "driver/gpio.h"

#include "cJSON.h"
#include "trace.h"
#include "frozen.h"
#include "items.h"

#include "ezlopi_timer.h"
#include "ezlopi_cloud.h"
#include "ezlopi_actions.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_gpio.h"

#include "gpio_isr_service.h"
#include "ultrasonic_HC_SR04.h"

#include "driver/mcpwm.h"
#include "soc/rtc.h"

typedef struct s_value_capture
{
    int64_t start_of_sample;
    int64_t end_of_sample;

} s_value_capture_t;

static int __prepare(void *arg);
static int __init(l_ezlopi_item_t *item);
static int __notify(l_ezlopi_item_t *item);
static int __get_value_cjson(l_ezlopi_item_t *item, void *arg);

int ultrasonic_hcsr04_v3(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
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
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = __get_value_cjson(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        ret = __notify(item);
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
    if (item && arg && item->user_arg)
    {
        int64_t time_us = 0;
        cJSON *cj_result = (cJSON *)arg;
        s_value_capture_t *cap_value = (s_value_capture_t *)item->user_arg;
        if (cap_value->end_of_sample && cap_value->start_of_sample)
        {
            if (cap_value->end_of_sample < cap_value->start_of_sample)
            {
                time_us = INT64_MAX - cap_value->start_of_sample;
                time_us += cap_value->end_of_sample;
            }
            else
            {
                time_us = cap_value->end_of_sample - cap_value->start_of_sample;
            }
        }

        double distance_cm = (double)time_us ? ((double)time_us / 58.0) : 0.0;

        cJSON_AddNumberToObject(cj_result, "value", distance_cm);
    }
    return ret;
}

static void __interrupt_upcall_os(void *arg)
{
    l_ezlopi_item_t *item = (l_ezlopi_item_t *)arg;
    if (item && item->user_arg)
    {
        s_value_capture_t *value_cap = (s_value_capture_t *)item->user_arg;
        if (EZLOPI_GPIO_HIGH == gpio_get_level(item->interface.gpio.gpio_in.gpio_num))
        {
            value_cap->start_of_sample = esp_timer_get_time();
        }
        else
        {
            value_cap->end_of_sample = esp_timer_get_time();
            ezlopi_device_value_updated_from_device_v3(item);
        }
    }
}

static int __init(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (GPIO_IS_VALID_OUTPUT_GPIO(item->interface.gpio.gpio_out.gpio_num))
    {
        const gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL << item->interface.gpio.gpio_out.gpio_num),
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = ((item->interface.gpio.gpio_out.pull == GPIO_PULLUP_ONLY) ||
                           (item->interface.gpio.gpio_out.pull == GPIO_PULLUP_PULLDOWN))
                              ? GPIO_PULLUP_ENABLE
                              : GPIO_PULLUP_DISABLE,
            .pull_down_en = ((item->interface.gpio.gpio_out.pull == GPIO_PULLDOWN_ONLY) ||
                             (item->interface.gpio.gpio_out.pull == GPIO_PULLUP_PULLDOWN))
                                ? GPIO_PULLDOWN_ENABLE
                                : GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
        };

        gpio_config(&io_conf);
    }

    if (GPIO_IS_VALID_GPIO(item->interface.gpio.gpio_in.gpio_num))
    {
        const gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL << item->interface.gpio.gpio_in.gpio_num),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = ((item->interface.gpio.gpio_in.pull == GPIO_PULLUP_ONLY) ||
                           (item->interface.gpio.gpio_in.pull == GPIO_PULLUP_PULLDOWN))
                              ? GPIO_PULLUP_ENABLE
                              : GPIO_PULLUP_DISABLE,
            .pull_down_en = ((item->interface.gpio.gpio_in.pull == GPIO_PULLDOWN_ONLY) ||
                             (item->interface.gpio.gpio_in.pull == GPIO_PULLUP_PULLDOWN))
                                ? GPIO_PULLDOWN_ENABLE
                                : GPIO_PULLDOWN_DISABLE,
            .intr_type = (GPIO_PULLUP_ONLY == item->interface.gpio.gpio_in.pull)
                             ? GPIO_INTR_POSEDGE
                             : GPIO_INTR_NEGEDGE,
        };

        gpio_config(&io_conf);
        gpio_isr_service_register_v3(item, __interrupt_upcall_os, 10);
    }
    return ret;
}

static void __setup_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    char *device_name = NULL;
    CJSON_GET_VALUE_STRING(cj_device, "dev_name", device_name);

    ASSIGN_DEVICE_NAME_V2(device, device_name);
    device->cloud_properties.category = category_security_sensor;
    device->cloud_properties.subcategory = subcategory_motion;
    device->cloud_properties.device_type = dev_type_sensor_motion;
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
}

static void __setup_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    item->cloud_properties.show = true;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.item_name = ezlopi_item_name_motion;
    item->cloud_properties.value_type = value_type_bool;
    item->cloud_properties.scale = scales_centi_meter;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    // for value capture
    item->user_arg = malloc(sizeof(s_value_capture_t));
}

static void __setup_item_interface_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    item->interface_type = EZLOPI_DEVICE_INTERFACE_DIGITAL_OUTPUT;

    // item->interface.gpio.gpio_out.gpio_num = 9;
    item->interface.gpio.gpio_out.enable = true;
    item->interface.gpio.gpio_out.interrupt = GPIO_INTR_DISABLE;
    item->interface.gpio.gpio_out.invert = EZLOPI_GPIO_LOGIC_NONINVERTED;
    item->interface.gpio.gpio_out.mode = GPIO_MODE_OUTPUT;
    item->interface.gpio.gpio_out.pull = GPIO_PULLDOWN_ONLY;
    item->interface.gpio.gpio_out.value = 0;
    CJSON_GET_VALUE_INT(cj_device, "gpio_out", item->interface.gpio.gpio_out.gpio_num);

    // item->interface.gpio.gpio_in.gpio_num = 10;
    item->interface.gpio.gpio_in.enable = true;
    item->interface.gpio.gpio_in.interrupt = GPIO_INTR_ANYEDGE;
    item->interface.gpio.gpio_in.invert = EZLOPI_GPIO_LOGIC_NONINVERTED;
    item->interface.gpio.gpio_in.mode = GPIO_MODE_INPUT;
    item->interface.gpio.gpio_in.pull = GPIO_PULLDOWN_ONLY;
    item->interface.gpio.gpio_in.value = 0;
    CJSON_GET_VALUE_INT(cj_device, "gpio_in", item->interface.gpio.gpio_in.gpio_num);
}

static int __prepare(void *arg)
{
    int ret = 0;
    if (arg)
    {
        s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
        cJSON *cj_device = prep_arg->cjson_device;
        if (cj_device)
        {
            l_ezlopi_device_t *device = ezlopi_device_add_device();
            if (device)
            {
                __setup_device_cloud_properties(device, cj_device);
                l_ezlopi_item_t *item = ezlopi_device_add_item_to_device(device, ultrasonic_hcsr04_v3);
                if (item)
                {
                    __setup_item_cloud_properties(item, cj_device);
                    __setup_item_interface_properties(item, cj_device);
                }
            }
        }
    }
    return ret;
}

static int __notify(l_ezlopi_item_t *item)
{
    ESP_ERROR_CHECK(gpio_set_level(item->interface.gpio.gpio_out.gpio_num, 1));
    vTaskDelay(1 / portTICK_PERIOD_MS);
    ESP_ERROR_CHECK(gpio_set_level(item->interface.gpio.gpio_out.gpio_num, 0));
}
