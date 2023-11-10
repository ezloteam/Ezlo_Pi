#include "math.h"
#include "cJSON.h"
#include <string.h>
#include "sdkconfig.h"
#include "driver/gpio.h"

#include "trace.h"
#include "items.h"

#include "ezlopi_timer.h"
#include "ezlopi_cloud.h"
#include "ezlopi_devices.h"
#include "ezlopi_i2c_master.h"
#include "ezlopi_spi_master.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_valueformatter.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_device_value_updated.h"

#include "led_strip.h"
#include "color_codes.h"
#include "device_0009_other_RMT_SK6812.h"

static bool sk6812_led_strip_initialized = false;

static l_ezlopi_item_t *switch_item = NULL;
static l_ezlopi_item_t *dimmer_item = NULL;
static l_ezlopi_item_t *dimmer_up_item = NULL;
static l_ezlopi_item_t *dimmer_down_item = NULL;
static l_ezlopi_item_t *dimmer_stop_item = NULL;
static l_ezlopi_item_t *rgb_color_item = NULL;

static int __prepare(void *arg);
static int __init(l_ezlopi_item_t *item);
static int __set_cjson_value(l_ezlopi_item_t *item, void *arg);
static int __get_cjson_value(l_ezlopi_item_t *item, void *arg);

int device_0009_other_RMT_SK6812(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
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
    case EZLOPI_ACTION_SET_VALUE:
    {
        ret = __set_cjson_value(item, arg);
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

static int __get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;

    led_strip_t *sk6812_strip = (led_strip_t *)item->user_arg;
    cJSON *cjson_properties = (cJSON *)arg;
    if ((NULL != cjson_properties) && (NULL != sk6812_strip))
    {
        if (ezlopi_item_name_rgbcolor == item->cloud_properties.item_name)
        {
            cJSON *color_json = cJSON_AddObjectToObject(cjson_properties, "value");
            if (color_json)
            {
                int green = sk6812_strip->buf[0];
                int red = sk6812_strip->buf[1];
                int blue = sk6812_strip->buf[2];
                cJSON_AddNumberToObject(color_json, "red", red);
                cJSON_AddNumberToObject(color_json, "green", green);
                cJSON_AddNumberToObject(color_json, "blue", blue);
                cJSON_AddNumberToObject(color_json, "cwhite", ((red << 16) | (green << 8) | (blue)));
                char *formatted_val = ezlopi_valueformatter_rgb(red, green, blue);
                cJSON_AddStringToObject(cjson_properties, "valueFormatted", formatted_val);
                free(formatted_val);
            }
        }
        else if (ezlopi_item_name_dimmer == item->cloud_properties.item_name)
        {
            item->interface.pwm.duty_cycle = (int)ceil(((sk6812_strip->brightness * 100.0) / 255.0));
            cJSON_AddNumberToObject(cjson_properties, "value", item->interface.pwm.duty_cycle);
            char *formatted_val = ezlopi_valueformatter_int32(item->interface.pwm.duty_cycle);
            cJSON_AddStringToObject(cjson_properties, "valueFormatted", formatted_val);
            free(formatted_val);
        }
        else if (ezlopi_item_name_switch == item->cloud_properties.item_name)
        {
            item->interface.gpio.gpio_in.value = (0 == sk6812_strip->brightness) ? 0 : 1;
            cJSON_AddBoolToObject(cjson_properties, "value", item->interface.gpio.gpio_in.value);
            char *formatted_val = ezlopi_valueformatter_bool(item->interface.gpio.gpio_in.value ? true : false);
            cJSON_AddStringToObject(cjson_properties, "valueFormatted", formatted_val);
        }
    }
    return ret;
}

static int __set_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;
    static uint8_t previous_brightness = 0;

    led_strip_t *sk6812_strip = (led_strip_t *)item->user_arg;
    cJSON *cjson_params = (cJSON *)arg;

    if ((NULL != cjson_params) && (NULL != sk6812_strip))
    {
        if (ezlopi_item_name_rgbcolor == item->cloud_properties.item_name)
        {
            cJSON *cjson_params_color_values = cJSON_GetObjectItem(cjson_params, "value");

            rgb_t color = {
                .red = 0,
                .green = 0,
                .blue = 0,
            };

            CJSON_GET_VALUE_INT(cjson_params_color_values, "red", color.red);
            CJSON_GET_VALUE_INT(cjson_params_color_values, "green", color.green);
            CJSON_GET_VALUE_INT(cjson_params_color_values, "blue", color.blue);

            led_strip_fill(sk6812_strip, 0, sk6812_strip->length, color);
            led_strip_flush(sk6812_strip);
            ezlopi_device_value_updated_from_device_v3(dimmer_item);
        }
        else if (ezlopi_item_name_dimmer == item->cloud_properties.item_name)
        {
            int dimmable_value_percentage = 0;
            CJSON_GET_VALUE_INT(cjson_params, "value", dimmable_value_percentage);
            int dimmable_value = (int)((dimmable_value_percentage * 255) / 100);
            led_strip_set_brightness(sk6812_strip, dimmable_value);
            led_strip_flush(sk6812_strip);
            ezlopi_device_value_updated_from_device_v3(switch_item);
        }
        else if (ezlopi_item_name_switch == item->cloud_properties.item_name)
        {
            int led_state = 0;
            CJSON_GET_VALUE_INT(cjson_params, "value", led_state);
            previous_brightness = (false == led_state) ? sk6812_strip->brightness : previous_brightness;
            int brightness_value = (false == led_state) ? 0 : ((0 == previous_brightness) ? 255 : previous_brightness);
            led_strip_set_brightness(sk6812_strip, brightness_value);
            led_strip_flush(sk6812_strip);
            ezlopi_device_value_updated_from_device_v3(dimmer_item);
        }
        else
        {
            TRACE_B("item->cloud_properties.item_name => %s", item->cloud_properties.item_name);
        }
    }
    return ret;
}

static int __init(l_ezlopi_item_t *item)
{
    int ret = 0;
    led_strip_t *sk6812_strip = (led_strip_t *)item->user_arg;
    if ((!sk6812_led_strip_initialized) && (NULL != sk6812_strip))
    {
        sk6812_strip->type = LED_STRIP_SK6812;
        sk6812_strip->length = 1;
        sk6812_strip->gpio = item->interface.pwm.gpio_num;
        sk6812_strip->buf = NULL;
        sk6812_strip->brightness = 255;
        sk6812_strip->channel = RMT_CHANNEL_0;

        led_strip_install();
        ESP_ERROR_CHECK(led_strip_init(sk6812_strip));
        rgb_t color = {
            .red = 255,
            .green = 255,
            .blue = 255,
        };
        led_strip_fill(sk6812_strip, 0, sk6812_strip->length, color);
        led_strip_set_brightness(sk6812_strip, 255);
        ESP_ERROR_CHECK(led_strip_flush(sk6812_strip));
        sk6812_led_strip_initialized = true;
    }
    return ret;
}

static void __prepare_device_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    char *device_name = NULL;
    CJSON_GET_VALUE_STRING(cj_device, "dev_name", device_name);
    ASSIGN_DEVICE_NAME_V2(device, device_name);
    device->cloud_properties.category = category_dimmable_light;
    device->cloud_properties.subcategory = subcategory_dimmable_colored;
    device->cloud_properties.device_type = dev_type_dimmer_outlet;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
}

static void __prepare_SK6812_RGB_color_item(l_ezlopi_item_t *item, cJSON *cj_device)
{
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.item_name = ezlopi_item_name_rgbcolor,
    item->cloud_properties.show = true;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.value_type = value_type_rgb;
    item->interface_type = EZLOPI_DEVICE_INTERFACE_PWM;
    item->interface.pwm.channel = 0;
    item->interface.pwm.value = 0;
    CJSON_GET_VALUE_INT(cj_device, "gpio", item->interface.pwm.gpio_num);
    CJSON_GET_VALUE_INT(cj_device, "duty_cycle", item->interface.pwm.duty_cycle);
    CJSON_GET_VALUE_INT(cj_device, "freq_hz", item->interface.pwm.freq_hz);
    item->interface.pwm.pwm_resln = 12;
}

static void __prepare_SK6812_RGB_dimmer_item(l_ezlopi_item_t *item, cJSON *cj_device)
{
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.item_name = ezlopi_item_name_dimmer,
    item->cloud_properties.show = true;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.value_type = value_type_int;
    item->interface_type = EZLOPI_DEVICE_INTERFACE_PWM;
    item->interface.pwm.channel = 0;
    item->interface.pwm.value = 0;
    CJSON_GET_VALUE_INT(cj_device, "gpio", item->interface.pwm.gpio_num);
    CJSON_GET_VALUE_INT(cj_device, "duty_cycle", item->interface.pwm.duty_cycle);
    CJSON_GET_VALUE_INT(cj_device, "freq_hz", item->interface.pwm.freq_hz);
    item->interface.pwm.pwm_resln = 12;
}

static void __prepare_SK6812_RGB_dimmer_up_item(l_ezlopi_item_t *item, cJSON *cj_device)
{
    CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.item_name = ezlopi_item_name_dimmer_up;
    item->cloud_properties.value_type = value_type_int;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    CJSON_GET_VALUE_INT(cj_device, "gpio", item->interface.pwm.gpio_num);
    CJSON_GET_VALUE_INT(cj_device, "duty_cycle", item->interface.pwm.duty_cycle);
    CJSON_GET_VALUE_INT(cj_device, "freq_hz", item->interface.pwm.freq_hz);
    item->interface.pwm.pwm_resln = 12;
}

static void __prepare_SK6812_RGB_dimmer_down_item(l_ezlopi_item_t *item, cJSON *cj_device)
{
    CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.item_name = ezlopi_item_name_dimmer_down;
    item->cloud_properties.value_type = value_type_int;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    CJSON_GET_VALUE_INT(cj_device, "gpio", item->interface.pwm.gpio_num);
    CJSON_GET_VALUE_INT(cj_device, "duty_cycle", item->interface.pwm.duty_cycle);
    CJSON_GET_VALUE_INT(cj_device, "freq_hz", item->interface.pwm.freq_hz);
    item->interface.pwm.pwm_resln = 12;
}

static void __prepare_SK6812_RGB_dimmer_stop_item(l_ezlopi_item_t *item, cJSON *cj_device)
{
    CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.item_name = ezlopi_item_name_dimmer_stop;
    item->cloud_properties.value_type = value_type_int;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    CJSON_GET_VALUE_INT(cj_device, "gpio", item->interface.pwm.gpio_num);
    CJSON_GET_VALUE_INT(cj_device, "duty_cycle", item->interface.pwm.duty_cycle);
    CJSON_GET_VALUE_INT(cj_device, "freq_hz", item->interface.pwm.freq_hz);
    item->interface.pwm.pwm_resln = 12;
}

static void __prepare_SK6812_LED_onoff_switch_item(l_ezlopi_item_t *item, cJSON *cj_device)
{
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.item_name = ezlopi_item_name_switch,
    item->cloud_properties.show = true;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.value_type = value_type_bool;
    item->interface_type = EZLOPI_DEVICE_INTERFACE_DIGITAL_OUTPUT;
    item->interface.gpio.gpio_out.enable = true;
    item->interface.gpio.gpio_out.gpio_num = 0;
    item->interface.gpio.gpio_out.interrupt = false;
    item->interface.gpio.gpio_out.invert = false;
    item->interface.gpio.gpio_out.mode = GPIO_MODE_INPUT;
    item->interface.gpio.gpio_out.pull = false;
    item->interface.gpio.gpio_out.value = false;
    item->interface.gpio.gpio_in.enable = false;
}

static int __prepare(void *arg)
{
    int ret = 0;

    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (prep_arg && prep_arg->cjson_device)
    {
        l_ezlopi_device_t *device = ezlopi_device_add_device();
        if (device)
        {
            __prepare_device_properties(device, prep_arg->cjson_device);
            rgb_color_item = ezlopi_device_add_item_to_device(device, device_0009_other_RMT_SK6812);
            dimmer_item = ezlopi_device_add_item_to_device(device, device_0009_other_RMT_SK6812);
            dimmer_up_item = ezlopi_device_add_item_to_device(device, device_0009_other_RMT_SK6812);
            dimmer_down_item = ezlopi_device_add_item_to_device(device, device_0009_other_RMT_SK6812);
            dimmer_stop_item = ezlopi_device_add_item_to_device(device, device_0009_other_RMT_SK6812);
            switch_item = ezlopi_device_add_item_to_device(device, device_0009_other_RMT_SK6812);
            if (switch_item && dimmer_item && dimmer_up_item && dimmer_down_item && dimmer_stop_item && rgb_color_item)
            {
                rgb_color_item->cloud_properties.device_id = device->cloud_properties.device_id;
                dimmer_item->cloud_properties.device_id = device->cloud_properties.device_id;
                dimmer_up_item->cloud_properties.device_id = device->cloud_properties.device_id;
                dimmer_down_item->cloud_properties.device_id = device->cloud_properties.device_id;
                dimmer_stop_item->cloud_properties.device_id = device->cloud_properties.device_id;
                switch_item->cloud_properties.device_id = device->cloud_properties.device_id;
                __prepare_SK6812_RGB_color_item(rgb_color_item, prep_arg->cjson_device);
                __prepare_SK6812_RGB_dimmer_item(dimmer_item, prep_arg->cjson_device);
                __prepare_SK6812_RGB_dimmer_up_item(dimmer_up_item, prep_arg->cjson_device);
                __prepare_SK6812_RGB_dimmer_down_item(dimmer_down_item, prep_arg->cjson_device);
                __prepare_SK6812_RGB_dimmer_stop_item(dimmer_stop_item, prep_arg->cjson_device);
                __prepare_SK6812_LED_onoff_switch_item(switch_item, prep_arg->cjson_device);
                led_strip_t *sk6812_strip = (led_strip_t *)malloc(sizeof(led_strip_t));
                if (sk6812_strip)
                {
                    memset(sk6812_strip, 0, sizeof(led_strip_t));
                    switch_item->user_arg = sk6812_strip;
                    dimmer_item->user_arg = sk6812_strip;
                    dimmer_up_item->user_arg = sk6812_strip;
                    dimmer_down_item->user_arg = sk6812_strip;
                    dimmer_stop_item->user_arg = sk6812_strip;
                    rgb_color_item->user_arg = sk6812_strip;
                }
            }
            else
            {
                ezlopi_device_free_device(device);
            }
        }
        else
        {
            ezlopi_device_free_device(device);
        }
    }

    return ret;
}
