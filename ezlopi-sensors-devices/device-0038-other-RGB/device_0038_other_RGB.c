#include "ezlopi_util_trace.h"

#include "ezlopi_core_timer.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_hal_pwm.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "device_0038_other_RGB.h"
#include "EZLOPI_USER_CONFIG.h"

typedef struct s_rgb_args
{
    bool RGB_LED_initialized;
    l_ezlopi_item_t* RGB_LED_item;
    l_ezlopi_item_t* RGB_LED_dimmer_item;
    l_ezlopi_item_t* RGB_LED_onoff_switch_item;

    float brightness;
    s_ezlopi_pwm_t red_struct;
    s_ezlopi_pwm_t green_struct;
    s_ezlopi_pwm_t blue_struct;
    float previous_dim_factor;
} s_rgb_args_t;

static int __prepare(void* arg);
static int __init(l_ezlopi_item_t* item);
static int __set_cjson_value(l_ezlopi_item_t* item, void* arg);
static int __get_cjson_value(l_ezlopi_item_t* item, void* arg);

int device_0038_other_RGB(e_ezlopi_actions_t action, l_ezlopi_item_t* item, void* arg, void* user_arg)
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

static int RGB_LED_change_color_value(s_rgb_args_t* rgb_args)
{
    int ret = 0;
    if (rgb_args)
    {
        TRACE_D("Brightness value is %d, %d, %d", (uint8_t)(rgb_args->red_struct.value * rgb_args->brightness), (uint8_t)(rgb_args->green_struct.value * rgb_args->brightness),
            (uint8_t)(rgb_args->blue_struct.value * rgb_args->brightness));

        ezlopi_pwm_change_duty(rgb_args->red_struct.channel, rgb_args->red_struct.speed_mode, (uint8_t)(rgb_args->red_struct.value * rgb_args->brightness));
        ezlopi_pwm_change_duty(rgb_args->green_struct.channel, rgb_args->green_struct.speed_mode, (uint8_t)(rgb_args->green_struct.value * rgb_args->brightness));
        ezlopi_pwm_change_duty(rgb_args->blue_struct.channel, rgb_args->blue_struct.speed_mode, (uint8_t)(rgb_args->blue_struct.value * rgb_args->brightness));
    }
    return ret;
}

static int __get_cjson_value(l_ezlopi_item_t* item, void* arg)
{
    int ret = 0;
    if (item && arg)
    {
        cJSON* cj_properties = (cJSON*)arg;
        s_rgb_args_t* rgb_args = (s_rgb_args_t*)item->user_arg;

        if ((NULL != cj_properties) && (NULL != rgb_args))
        {
            if (ezlopi_item_name_rgbcolor == item->cloud_properties.item_name)
            {
                cJSON* color_values = cJSON_AddObjectToObject(__FUNCTION__, cj_properties, ezlopi_value_str);
                if (color_values)
                {
                    cJSON_AddNumberToObject(__FUNCTION__, color_values, ezlopi_red_str, rgb_args->red_struct.value);
                    cJSON_AddNumberToObject(__FUNCTION__, color_values, ezlopi_green_str, rgb_args->green_struct.value);
                    cJSON_AddNumberToObject(__FUNCTION__, color_values, ezlopi_blue_str, rgb_args->blue_struct.value);

                    char formatted_rgb_value[32];
                    snprintf(formatted_rgb_value, sizeof(formatted_rgb_value), "#%02x%02x%02x", rgb_args->red_struct.value, rgb_args->green_struct.value, rgb_args->blue_struct.value);
                    cJSON_AddStringToObject(__FUNCTION__, cj_properties, ezlopi_valueFormatted_str, formatted_rgb_value);
                }
            }
            else if (ezlopi_item_name_switch == item->cloud_properties.item_name)
            {
                ezlopi_valueformatter_bool_to_cjson(item, cj_properties, rgb_args->brightness);
            }
            else if (ezlopi_item_name_dimmer == item->cloud_properties.item_name)
            {
                int dim_percentage = (int)(rgb_args->brightness * 100);
                ezlopi_valueformatter_int32_to_cjson(item, cj_properties, dim_percentage);
            }
        }
    }
    return ret;
}

static int __set_cjson_value(l_ezlopi_item_t* item, void* arg)
{
    int ret = 0;
    if (item && arg)
    {
        cJSON* cjson_params = (cJSON*)arg;
        s_rgb_args_t* rgb_args = (s_rgb_args_t*)item->user_arg;
        if ((NULL != cjson_params) && (NULL != rgb_args))
        {
            if (ezlopi_item_name_rgbcolor == item->cloud_properties.item_name)
            {
                cJSON* cjson_params_rgb_values = cJSON_GetObjectItem(__FUNCTION__, cjson_params, ezlopi_value_str);

                CJSON_TRACE("cjson_params_rgb_values", cjson_params_rgb_values);

                CJSON_GET_VALUE_DOUBLE(cjson_params_rgb_values, ezlopi_red_str, rgb_args->red_struct.value);
                CJSON_GET_VALUE_DOUBLE(cjson_params_rgb_values, ezlopi_green_str, rgb_args->green_struct.value);
                CJSON_GET_VALUE_DOUBLE(cjson_params_rgb_values, ezlopi_blue_str, rgb_args->blue_struct.value);

                RGB_LED_change_color_value(rgb_args);
            }
            if (ezlopi_item_name_switch == item->cloud_properties.item_name)
            {
                int led_state = 0;
                CJSON_GET_VALUE_DOUBLE(cjson_params, ezlopi_value_str, led_state);
                rgb_args->previous_dim_factor = ((0 == led_state) ? rgb_args->brightness : rgb_args->previous_dim_factor);
                rgb_args->brightness = ((0 == led_state) ? 0.0 : ((0 == rgb_args->previous_dim_factor) ? 1.0 : rgb_args->previous_dim_factor));
                TRACE_D("Brightness value is %d, %d, %d", (uint8_t)(rgb_args->red_struct.value * rgb_args->brightness), (uint8_t)(rgb_args->green_struct.value * rgb_args->brightness),
                    (uint8_t)(rgb_args->blue_struct.value * rgb_args->brightness));
                RGB_LED_change_color_value(rgb_args);
                ezlopi_device_value_updated_from_device_broadcast(rgb_args->RGB_LED_dimmer_item);
            }
            if (ezlopi_item_name_dimmer == item->cloud_properties.item_name)
            {
                int dim_percent = 0;
                CJSON_GET_VALUE_DOUBLE(cjson_params, ezlopi_value_str, dim_percent);
                float dim_brightness_factor = dim_percent / 100.0;
                TRACE_D("dim_percent %d, dim_brightness_factor is %f", dim_percent, dim_brightness_factor);
                rgb_args->brightness = dim_brightness_factor;
                RGB_LED_change_color_value(rgb_args);
                ezlopi_device_value_updated_from_device_broadcast(rgb_args->RGB_LED_onoff_switch_item);
            }
        }
    }
    return ret;
}

static int __init(l_ezlopi_item_t* item)
{
    int ret = 0;

    if (item)
    {
        s_rgb_args_t* rgb_args = (s_rgb_args_t*)item->user_arg;
        if (rgb_args)
        {
            if (GPIO_IS_VALID_GPIO(rgb_args->red_struct.gpio_num) &&
                GPIO_IS_VALID_GPIO(rgb_args->green_struct.gpio_num) &&
                GPIO_IS_VALID_GPIO(rgb_args->blue_struct.gpio_num))
            {
                if (false == rgb_args->RGB_LED_initialized)
                {
                    s_ezlopi_channel_speed_t* RGB_LED_red_channel_speed = ezlopi_pwm_init(rgb_args->red_struct.gpio_num, rgb_args->red_struct.pwm_resln, rgb_args->red_struct.freq_hz, rgb_args->red_struct.duty_cycle);
                    if (RGB_LED_red_channel_speed)
                    {
                        rgb_args->red_struct.channel = RGB_LED_red_channel_speed->channel;
                        rgb_args->red_struct.speed_mode = RGB_LED_red_channel_speed->speed_mode;
                        TRACE_D("red channel is %d", rgb_args->red_struct.channel);
                        ezlopi_free(__FUNCTION__, RGB_LED_red_channel_speed);
                    }

                    s_ezlopi_channel_speed_t* RGB_LED_green_channel_speed = ezlopi_pwm_init(rgb_args->green_struct.gpio_num, rgb_args->green_struct.pwm_resln, rgb_args->green_struct.freq_hz, rgb_args->green_struct.duty_cycle);
                    if (RGB_LED_green_channel_speed)
                    {
                        rgb_args->green_struct.channel = RGB_LED_green_channel_speed->channel;
                        rgb_args->green_struct.speed_mode = RGB_LED_green_channel_speed->speed_mode;
                        TRACE_D("green channel is %d", rgb_args->green_struct.channel);
                        ezlopi_free(__FUNCTION__, RGB_LED_green_channel_speed);
                    }

                    s_ezlopi_channel_speed_t* RGB_LED_blue_channel_speed = ezlopi_pwm_init(rgb_args->blue_struct.gpio_num, rgb_args->blue_struct.pwm_resln, rgb_args->blue_struct.freq_hz, rgb_args->blue_struct.duty_cycle);
                    if (RGB_LED_blue_channel_speed)
                    {
                        rgb_args->blue_struct.channel = RGB_LED_blue_channel_speed->channel;
                        rgb_args->blue_struct.speed_mode = RGB_LED_blue_channel_speed->speed_mode;
                        TRACE_D("blue channel is %d", rgb_args->blue_struct.channel);
                        ezlopi_free(__FUNCTION__, RGB_LED_blue_channel_speed);
                    }

                    RGB_LED_change_color_value(rgb_args);

                    rgb_args->RGB_LED_initialized = true;
                    ret = 1;
                }
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

static void __prepare_device_cloud_properties(l_ezlopi_device_t* device, cJSON* cj_device)
{
    device->cloud_properties.category = category_dimmable_light;
    device->cloud_properties.subcategory = subcategory_dimmable_colored;
    device->cloud_properties.device_type = dev_type_dimmer_outlet;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_RGB_LED_user_args(s_rgb_args_t* rgb_args, cJSON* cj_device)
{
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_gpio1_str, rgb_args->red_struct.gpio_num);
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_gpio2_str, rgb_args->green_struct.gpio_num);
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_gpio3_str, rgb_args->blue_struct.gpio_num);

    rgb_args->red_struct.duty_cycle = 0;
    rgb_args->green_struct.duty_cycle = 0;
    rgb_args->blue_struct.duty_cycle = 0;

    rgb_args->red_struct.freq_hz = 5000;
    rgb_args->green_struct.freq_hz = 5000;
    rgb_args->blue_struct.freq_hz = 5000;

    rgb_args->red_struct.pwm_resln = 8;
    rgb_args->green_struct.pwm_resln = 8;
    rgb_args->blue_struct.pwm_resln = 8;

    rgb_args->red_struct.value = 255;
    rgb_args->green_struct.value = 255;
    rgb_args->blue_struct.value = 255;
    rgb_args->brightness = 0.0;
}

static void __prepare_RGB_LED_item(l_ezlopi_item_t* item, cJSON* cj_device, void* user_arg)
{
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.item_name = ezlopi_item_name_rgbcolor,
        item->cloud_properties.show = true;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.value_type = value_type_rgb;
    item->interface_type = EZLOPI_DEVICE_INTERFACE_PWM;
    item->interface.pwm.gpio_num = 0;
    item->interface.pwm.channel = 0;
    item->interface.pwm.value = 0;
    item->interface.pwm.duty_cycle = 0;
    item->interface.pwm.freq_hz = 5000;
    item->interface.pwm.pwm_resln = 8;

    item->is_user_arg_unique = true;
    item->user_arg = user_arg;
}

static void __prepare_RGB_LED_onoff_switch_item(l_ezlopi_item_t* item, cJSON* cj_device, void* user_arg)
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

    item->is_user_arg_unique = true;
    item->user_arg = user_arg;
}

static void __prepare_RGB_LED_dimmer_item(l_ezlopi_item_t* item, cJSON* cj_device, void* user_arg)
{
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.item_name = ezlopi_item_name_dimmer,
        item->cloud_properties.show = true;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.value_type = value_type_int;
    item->interface_type = EZLOPI_DEVICE_INTERFACE_PWM;
    item->interface.pwm.gpio_num = 0;
    item->interface.pwm.channel = 0;
    item->interface.pwm.value = 0;
    item->interface.pwm.duty_cycle = 0;
    item->interface.pwm.freq_hz = 5000;
    item->interface.pwm.pwm_resln = 8;

    item->is_user_arg_unique = true;
    item->user_arg = user_arg;
}

static int __prepare(void* arg)
{
    int ret = 0;

    s_ezlopi_prep_arg_t* prep_arg = (s_ezlopi_prep_arg_t*)arg;
    if (prep_arg && prep_arg->cjson_device)
    {
        l_ezlopi_device_t* RGB_device = ezlopi_device_add_device(prep_arg->cjson_device, NULL);
        if (RGB_device)
        {
            ret = 1;
            s_rgb_args_t* rgb_args = ezlopi_malloc(__FUNCTION__, sizeof(s_rgb_args_t));
            if (rgb_args)
            {
                memset(rgb_args, 0, sizeof(s_rgb_args_t));
                rgb_args->previous_dim_factor = 1.0;

                __prepare_device_cloud_properties(RGB_device, prep_arg->cjson_device);
                __prepare_RGB_LED_user_args(rgb_args, prep_arg->cjson_device);

                rgb_args->RGB_LED_item = ezlopi_device_add_item_to_device(RGB_device, device_0038_other_RGB);
                if (rgb_args->RGB_LED_item)
                {
                    __prepare_RGB_LED_item(rgb_args->RGB_LED_item, prep_arg->cjson_device, rgb_args);
                }

                rgb_args->RGB_LED_onoff_switch_item = ezlopi_device_add_item_to_device(RGB_device, device_0038_other_RGB);
                if (rgb_args->RGB_LED_onoff_switch_item)
                {
                    __prepare_RGB_LED_onoff_switch_item(rgb_args->RGB_LED_onoff_switch_item, prep_arg->cjson_device, rgb_args);
                }

                rgb_args->RGB_LED_dimmer_item = ezlopi_device_add_item_to_device(RGB_device, device_0038_other_RGB);
                if (rgb_args->RGB_LED_dimmer_item)
                {
                    __prepare_RGB_LED_dimmer_item(rgb_args->RGB_LED_dimmer_item, prep_arg->cjson_device, rgb_args);
                }

                if (!rgb_args->RGB_LED_item && !rgb_args->RGB_LED_onoff_switch_item && !rgb_args->RGB_LED_dimmer_item)
                {
                    ezlopi_free(__FUNCTION__, rgb_args);
                    ezlopi_device_free_device(RGB_device);
                    ret = -1;
                }
            }
            else
            {
                ezlopi_device_free_device(RGB_device);
                ret = -1;
            }
        }
    }

    return ret;
}