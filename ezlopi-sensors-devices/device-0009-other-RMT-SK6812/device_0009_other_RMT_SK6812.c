#include <math.h>
#include "../../build/config/sdkconfig.h"
#include "driver/gpio.h"
#include "ezlopi_util_trace.h"

// #include "ezlopi_core_timer.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"
#include "ezlopi_core_errors.h"

// #include "ezlopi_hal_i2c_master.h"
// #include "ezlopi_hal_spi_master.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "led_strip.h"
#include "color_codes.h"

#include "device_0009_other_RMT_SK6812.h"
#include "EZLOPI_USER_CONFIG.h"


typedef struct s_dimmer_args
{
    led_strip_t sk6812_strip;
    l_ezlopi_item_t *switch_item;
    l_ezlopi_item_t *dimmer_item;
    l_ezlopi_item_t *dimmer_up_item;
    l_ezlopi_item_t *dimmer_down_item;
    l_ezlopi_item_t *dimmer_stop_item;
    l_ezlopi_item_t *rgb_color_item;
    uint8_t previous_brightness;
    bool sk6812_led_strip_initialized;
} s_dimmer_args_t;

static ezlopi_error_t __prepare(void *arg);
static ezlopi_error_t __init(l_ezlopi_item_t *item);
static ezlopi_error_t __set_cjson_value(l_ezlopi_item_t *item, void *arg);
static ezlopi_error_t __get_cjson_value(l_ezlopi_item_t *item, void *arg);

ezlopi_error_t device_0009_other_RMT_SK6812(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    ezlopi_error_t ret = EZPI_SUCCESS;

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

static ezlopi_error_t __get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item && arg)
    {
        led_strip_t *sk6812_strip = (led_strip_t *)item->user_arg;
        cJSON *cj_properties = (cJSON *)arg;
        if ((NULL != cj_properties) && (NULL != sk6812_strip))
        {
            if (ezlopi_item_name_rgbcolor == item->cloud_properties.item_name)
            {
                cJSON *color_json = cJSON_AddObjectToObject(__FUNCTION__, cj_properties, ezlopi_value_str);
                if (color_json)
                {
                    int green = sk6812_strip->buf[0];
                    int red = sk6812_strip->buf[1];
                    int blue = sk6812_strip->buf[2];

                    cJSON_AddNumberToObject(__FUNCTION__, color_json, ezlopi_red_str, red);
                    cJSON_AddNumberToObject(__FUNCTION__, color_json, ezlopi_green_str, green);
                    cJSON_AddNumberToObject(__FUNCTION__, color_json, ezlopi_blue_str, blue);
                    cJSON_AddNumberToObject(__FUNCTION__, color_json, ezlopi_cwhite_str, ((red << 16) | (green << 8) | (blue)));

                    char formatted_rgb_value[32];
                    snprintf(formatted_rgb_value, sizeof(formatted_rgb_value), "#%02x%02x%02x", red, green, blue);
                    cJSON_AddStringToObject(__FUNCTION__, cj_properties, ezlopi_valueFormatted_str, formatted_rgb_value);
                }
            }
            else if (ezlopi_item_name_dimmer == item->cloud_properties.item_name)
            {
                item->interface.pwm.duty_cycle = (int)ceil(((sk6812_strip->brightness * 100.0) / 255.0));
                ezlopi_valueformatter_uint32_to_cjson(cj_properties, item->interface.pwm.duty_cycle, item->cloud_properties.scale);
            }
            else if (ezlopi_item_name_switch == item->cloud_properties.item_name)
            {
                item->interface.gpio.gpio_in.value = (0 == sk6812_strip->brightness) ? 0 : 1;
                ezlopi_valueformatter_bool_to_cjson(cj_properties, item->interface.gpio.gpio_out.value, item->cloud_properties.scale);
            }
            ret = EZPI_SUCCESS;
        }
    }
    return ret;
}

static ezlopi_error_t __set_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_FAILED;

    if (item && arg)
    {
        cJSON *cjson_params = (cJSON *)arg;
        s_dimmer_args_t *dimmer_args = (s_dimmer_args_t *)item->user_arg;
        if ((dimmer_args) && (cjson_params))
        {
            if (ezlopi_item_name_rgbcolor == item->cloud_properties.item_name)
            {
                cJSON *cjson_params_color_values = cJSON_GetObjectItem(__FUNCTION__, cjson_params, ezlopi_value_str);

                rgb_t color = {
                    .red = 0,
                    .green = 0,
                    .blue = 0,
                };

                CJSON_GET_VALUE_DOUBLE(cjson_params_color_values, ezlopi_red_str, color.red);
                CJSON_GET_VALUE_DOUBLE(cjson_params_color_values, ezlopi_green_str, color.green);
                CJSON_GET_VALUE_DOUBLE(cjson_params_color_values, ezlopi_blue_str, color.blue);

                led_strip_fill(&dimmer_args->sk6812_strip, 0, dimmer_args->sk6812_strip.length, color);
                led_strip_flush(&dimmer_args->sk6812_strip);

                ezlopi_device_value_updated_from_device_broadcast(dimmer_args->dimmer_item);
                ret = EZPI_SUCCESS;
            }
            else if (ezlopi_item_name_dimmer == item->cloud_properties.item_name)
            {
                int dimmable_value_percentage = 0;
                CJSON_GET_VALUE_DOUBLE(cjson_params, ezlopi_value_str, dimmable_value_percentage);
                int dimmable_value = (int)((dimmable_value_percentage * 255) / 100);
                led_strip_set_brightness(&dimmer_args->sk6812_strip, dimmable_value);
                led_strip_flush(&dimmer_args->sk6812_strip);

                ezlopi_device_value_updated_from_device_broadcast(dimmer_args->switch_item);
                ret = EZPI_SUCCESS;
            }
            else if (ezlopi_item_name_switch == item->cloud_properties.item_name)
            {
                int led_state = 0;
                CJSON_GET_VALUE_DOUBLE(cjson_params, ezlopi_value_str, led_state);
                dimmer_args->previous_brightness = (false == led_state) ? dimmer_args->sk6812_strip.brightness : dimmer_args->previous_brightness;
                int brightness_value = (false == led_state) ? 0 : ((0 == dimmer_args->previous_brightness) ? 255 : dimmer_args->previous_brightness);
                led_strip_set_brightness(&dimmer_args->sk6812_strip, brightness_value);
                led_strip_flush(&dimmer_args->sk6812_strip);

                ezlopi_device_value_updated_from_device_broadcast(dimmer_args->dimmer_item);
                ret = EZPI_SUCCESS;
            }
            else
            {
                TRACE_D("item->cloud_properties.item_name => %s", item->cloud_properties.item_name);
            }
        }
    }

    return ret;
}

static ezlopi_error_t __init(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_SUCCESS;
    if (item)
    {
        if (GPIO_IS_VALID_GPIO(item->interface.pwm.gpio_num))
        {
            s_dimmer_args_t *dimmer_args = (s_dimmer_args_t *)item->user_arg;
            if (dimmer_args)
            {
                if (0 == dimmer_args->sk6812_led_strip_initialized)
                {
                    dimmer_args->sk6812_strip.type = LED_STRIP_SK6812;
                    dimmer_args->sk6812_strip.length = 1;
                    dimmer_args->sk6812_strip.gpio = item->interface.pwm.gpio_num;
                    dimmer_args->sk6812_strip.buf = NULL;
                    dimmer_args->sk6812_strip.brightness = 255;
                    dimmer_args->sk6812_strip.channel = RMT_CHANNEL_0;

                    led_strip_install();
                    esp_err_t err = led_strip_init(&dimmer_args->sk6812_strip);
                    if (ESP_OK == err)
                    {
                        rgb_t color = {
                            .red = 255,
                            .green = 255,
                            .blue = 255,
                        };

                        err |= led_strip_fill(&dimmer_args->sk6812_strip, 0, dimmer_args->sk6812_strip.length, color);
                        if (ESP_OK == (err = led_strip_set_brightness(&dimmer_args->sk6812_strip, 255)))
                        {
                            if (ESP_OK == (err = led_strip_flush(&dimmer_args->sk6812_strip)))
                            {
                                dimmer_args->sk6812_led_strip_initialized = true;
                            }
                        }
                    }

                    if (ESP_OK != err)
                    {
                        TRACE_E("Couldn't initiate device!, error: %d", err);
                        ret = EZPI_ERR_INIT_DEVICE_FAILED;
                    }
                }
            }
            else
            {
                ret = EZPI_ERR_INIT_DEVICE_FAILED;
            }
        }
        else
        {
            ret = EZPI_ERR_INIT_DEVICE_FAILED;
        }
    }
    return ret;
}

static void __prepare_device_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    device->cloud_properties.category = category_dimmable_light;
    device->cloud_properties.subcategory = subcategory_dimmable_colored;
    device->cloud_properties.device_type = dev_type_dimmer_outlet;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_SK6812_RGB_color_item(l_ezlopi_item_t *item, cJSON *cj_device)
{
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.item_name = ezlopi_item_name_rgbcolor;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.value_type = value_type_rgb;
    item->interface_type = EZLOPI_DEVICE_INTERFACE_PWM;
    item->interface.pwm.channel = 0;
    item->interface.pwm.value = 0;
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio1_str, item->interface.pwm.gpio_num);
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_duty_cycle_str, item->interface.pwm.duty_cycle);
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_freq_hz_str, item->interface.pwm.freq_hz);
    item->interface.pwm.pwm_resln = 12;
}

static void __prepare_SK6812_RGB_dimmer_item(l_ezlopi_item_t *item, cJSON *cj_device)
{
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.item_name = ezlopi_item_name_dimmer;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.value_type = value_type_int;
    item->interface_type = EZLOPI_DEVICE_INTERFACE_PWM;
    item->interface.pwm.channel = 0;
    item->interface.pwm.value = 0;
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio1_str, item->interface.pwm.gpio_num);
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_duty_cycle_str, item->interface.pwm.duty_cycle);
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_freq_hz_str, item->interface.pwm.freq_hz);
    item->interface.pwm.pwm_resln = 12;
}

static void __prepare_SK6812_RGB_dimmer_up_item(l_ezlopi_item_t *item, cJSON *cj_device)
{
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_dev_type_str, item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.item_name = ezlopi_item_name_dimmer_up;
    item->cloud_properties.value_type = value_type_int;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio1_str, item->interface.pwm.gpio_num);
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_duty_cycle_str, item->interface.pwm.duty_cycle);
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_freq_hz_str, item->interface.pwm.freq_hz);
    item->interface.pwm.pwm_resln = 12;
}

static void __prepare_SK6812_RGB_dimmer_down_item(l_ezlopi_item_t *item, cJSON *cj_device)
{
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_dev_type_str, item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.item_name = ezlopi_item_name_dimmer_down;
    item->cloud_properties.value_type = value_type_int;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio1_str, item->interface.pwm.gpio_num);
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_duty_cycle_str, item->interface.pwm.duty_cycle);
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_freq_hz_str, item->interface.pwm.freq_hz);
    item->interface.pwm.pwm_resln = 12;
}

static void __prepare_SK6812_RGB_dimmer_stop_item(l_ezlopi_item_t *item, cJSON *cj_device)
{
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_dev_type_str, item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.item_name = ezlopi_item_name_dimmer_stop;
    item->cloud_properties.value_type = value_type_int;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio1_str, item->interface.pwm.gpio_num);
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_duty_cycle_str, item->interface.pwm.duty_cycle);
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_freq_hz_str, item->interface.pwm.freq_hz);
    item->interface.pwm.pwm_resln = 12;
}

static void __prepare_SK6812_LED_onoff_switch_item(l_ezlopi_item_t *item, cJSON *cj_device)
{
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.item_name = ezlopi_item_name_switch;
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

static ezlopi_error_t __prepare(void *arg)
{
    ezlopi_error_t ret = EZPI_SUCCESS;

    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (prep_arg && prep_arg->cjson_device)
    {
        l_ezlopi_device_t *device = ezlopi_device_add_device(prep_arg->cjson_device, NULL);
        if (device)
        {
            __prepare_device_properties(device, prep_arg->cjson_device);

            s_dimmer_args_t *dimmer_args = ezlopi_malloc(__FUNCTION__, sizeof(s_dimmer_args_t));
            if (dimmer_args)
            {
                memset(dimmer_args, 0, sizeof(s_dimmer_args_t));

                dimmer_args->rgb_color_item = ezlopi_device_add_item_to_device(device, device_0009_other_RMT_SK6812);
                dimmer_args->dimmer_item = ezlopi_device_add_item_to_device(device, device_0009_other_RMT_SK6812);
                dimmer_args->dimmer_up_item = ezlopi_device_add_item_to_device(device, device_0009_other_RMT_SK6812);
                dimmer_args->dimmer_down_item = ezlopi_device_add_item_to_device(device, device_0009_other_RMT_SK6812);
                dimmer_args->dimmer_stop_item = ezlopi_device_add_item_to_device(device, device_0009_other_RMT_SK6812);
                dimmer_args->switch_item = ezlopi_device_add_item_to_device(device, device_0009_other_RMT_SK6812);

                if (dimmer_args->switch_item && dimmer_args->dimmer_item && dimmer_args->dimmer_up_item && dimmer_args->dimmer_down_item && dimmer_args->dimmer_stop_item && dimmer_args->rgb_color_item)
                {
                    dimmer_args->rgb_color_item->is_user_arg_unique = true; /*linked to only one device so 'TRUE'*/
                    dimmer_args->dimmer_item->is_user_arg_unique = true;
                    dimmer_args->dimmer_up_item->is_user_arg_unique = true;
                    dimmer_args->dimmer_down_item->is_user_arg_unique = true;
                    dimmer_args->dimmer_stop_item->is_user_arg_unique = true;
                    dimmer_args->switch_item->is_user_arg_unique = true;

                    dimmer_args->rgb_color_item->user_arg = dimmer_args;
                    dimmer_args->dimmer_item->user_arg = dimmer_args;
                    dimmer_args->dimmer_up_item->user_arg = dimmer_args;
                    dimmer_args->dimmer_down_item->user_arg = dimmer_args;
                    dimmer_args->dimmer_stop_item->user_arg = dimmer_args;
                    dimmer_args->switch_item->user_arg = dimmer_args;

                    __prepare_SK6812_RGB_color_item(dimmer_args->rgb_color_item, prep_arg->cjson_device);
                    __prepare_SK6812_RGB_dimmer_item(dimmer_args->dimmer_item, prep_arg->cjson_device);
                    __prepare_SK6812_RGB_dimmer_up_item(dimmer_args->dimmer_up_item, prep_arg->cjson_device);
                    __prepare_SK6812_RGB_dimmer_down_item(dimmer_args->dimmer_down_item, prep_arg->cjson_device);
                    __prepare_SK6812_RGB_dimmer_stop_item(dimmer_args->dimmer_stop_item, prep_arg->cjson_device);
                    __prepare_SK6812_LED_onoff_switch_item(dimmer_args->switch_item, prep_arg->cjson_device);
                }
                else
                {
                    ezlopi_free(__FUNCTION__, dimmer_args);
                    ezlopi_device_free_device(device);
                    ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
                }
            }
            else
            {
                ezlopi_device_free_device(device);
                ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
            }
        }
    }

    return ret;
}
