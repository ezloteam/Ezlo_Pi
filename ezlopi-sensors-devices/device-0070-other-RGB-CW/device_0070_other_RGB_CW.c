#include <math.h>
#include "ezlopi_util_trace.h"

#include "ezlopi_core_timer.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_hal_pwm.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "device_0070_other_RGB_CW.h"

typedef struct s_rgb_args
{
    bool RGB_LED_initialized;
    l_ezlopi_item_t* RGB_LED_item;
    l_ezlopi_item_t* RGB_LED_dimmer_item;
    l_ezlopi_item_t* RGB_LED_onoff_switch_item;
    l_ezlopi_item_t* RGB_LED_item_dimmer_up;
    l_ezlopi_item_t* RGB_LED_item_dimmer_down;
    l_ezlopi_item_t* RGB_LED_item_dimmer_stop;
    // l_ezlopi_item_t* RGB_LED_item_dimmer_switch;

    float brightness;
    s_ezlopi_pwm_t wwhite_struct;
    s_ezlopi_pwm_t cwhite_struct;
    s_ezlopi_pwm_t red_struct;
    s_ezlopi_pwm_t green_struct;
    s_ezlopi_pwm_t blue_struct;
    s_ezlopi_pwm_t amber_struct;
    s_ezlopi_pwm_t cyan_struct;
    s_ezlopi_pwm_t purple_struct;
    s_ezlopi_pwm_t indexed_struct;
    float previous_dim_factor;

    uint32_t previous_brightness_value;
    uint32_t current_brightness_value;

} s_rgb_args_t;

static int __prepare(void* arg);
static int __init(l_ezlopi_item_t* item);
static int __set_cjson_value(l_ezlopi_item_t* item, void* arg);
static int __get_cjson_value(l_ezlopi_item_t* item, void* arg);



int device_0070_other_RGB_CW(e_ezlopi_actions_t action, l_ezlopi_item_t* item, void* arg, void* user_arg)
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
        TRACE_S("-> %s", ezlopi_actions_to_string(action));
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
        TRACE_D("Brightness value is %d, %d, %d, %d, %d, %d, %d, %d, %d",
            (uint8_t)(rgb_args->wwhite_struct.value * rgb_args->brightness),
            (uint8_t)(rgb_args->cwhite_struct.value * rgb_args->brightness),
            (uint8_t)(rgb_args->red_struct.value * rgb_args->brightness),
            (uint8_t)(rgb_args->green_struct.value * rgb_args->brightness),
            (uint8_t)(rgb_args->blue_struct.value * rgb_args->brightness),
            (uint8_t)(rgb_args->amber_struct.value * rgb_args->brightness),
            (uint8_t)(rgb_args->cyan_struct.value * rgb_args->brightness),
            (uint8_t)(rgb_args->purple_struct.value * rgb_args->brightness),
            (uint8_t)(rgb_args->indexed_struct.value * rgb_args->brightness));

        if ((rgb_args->wwhite_struct.value > 0) || (rgb_args->cwhite_struct.value > 0))
        {
            ezlopi_pwm_change_duty(rgb_args->green_struct.channel, rgb_args->green_struct.speed_mode, (uint8_t)(rgb_args->wwhite_struct.value * rgb_args->brightness));
            ezlopi_pwm_change_duty(rgb_args->blue_struct.channel, rgb_args->blue_struct.speed_mode, (uint8_t)(rgb_args->cwhite_struct.value * rgb_args->brightness));

            // uint32_t white_balance = fabs(rgb_args->wwhite_struct.value - rgb_args->wwhite_struct.value);
            // if (rgb_args->wwhite_struct.value >= rgb_args->cwhite_struct.value)
            // {
            //     ezlopi_pwm_change_duty(rgb_args->red_struct.channel, rgb_args->red_struct.speed_mode, (uint8_t)((255) * rgb_args->brightness));
            //     ezlopi_pwm_change_duty(rgb_args->green_struct.channel, rgb_args->green_struct.speed_mode, (uint8_t)((200 * (white_balance / 255)) * rgb_args->brightness));
            //     ezlopi_pwm_change_duty(rgb_args->blue_struct.channel, rgb_args->blue_struct.speed_mode, (uint8_t)((100 * (white_balance / 255)) * rgb_args->brightness));
            // }
            // else
            // {   //cold white
            //     ezlopi_pwm_change_duty(rgb_args->red_struct.channel, rgb_args->red_struct.speed_mode, (uint8_t)((255) * rgb_args->brightness));
            //     ezlopi_pwm_change_duty(rgb_args->green_struct.channel, rgb_args->green_struct.speed_mode, (uint8_t)((200 * (255 - white_balance / 255)) * rgb_args->brightness));
            //     ezlopi_pwm_change_duty(rgb_args->blue_struct.channel, rgb_args->blue_struct.speed_mode, (uint8_t)((120 * (255 - white_balance / 255)) * rgb_args->brightness));
            // }

            ezlopi_pwm_change_duty(rgb_args->red_struct.channel, rgb_args->red_struct.speed_mode, (uint8_t)(0 * rgb_args->brightness));
            ezlopi_pwm_change_duty(rgb_args->green_struct.channel, rgb_args->green_struct.speed_mode, (uint8_t)(0 * rgb_args->brightness));
            ezlopi_pwm_change_duty(rgb_args->blue_struct.channel, rgb_args->blue_struct.speed_mode, (uint8_t)(0 * rgb_args->brightness));
        }
        else
        {
            ezlopi_pwm_change_duty(rgb_args->green_struct.channel, rgb_args->green_struct.speed_mode, (uint8_t)(0 * rgb_args->brightness));
            ezlopi_pwm_change_duty(rgb_args->blue_struct.channel, rgb_args->blue_struct.speed_mode, (uint8_t)(0 * rgb_args->brightness));
            ezlopi_pwm_change_duty(rgb_args->red_struct.channel, rgb_args->red_struct.speed_mode, (uint8_t)(rgb_args->red_struct.value * rgb_args->brightness));
            ezlopi_pwm_change_duty(rgb_args->green_struct.channel, rgb_args->green_struct.speed_mode, (uint8_t)(rgb_args->green_struct.value * rgb_args->brightness));
            ezlopi_pwm_change_duty(rgb_args->blue_struct.channel, rgb_args->blue_struct.speed_mode, (uint8_t)(rgb_args->blue_struct.value * rgb_args->brightness));
        }
    }
    return ret;
}

static int __get_cjson_value(l_ezlopi_item_t* item, void* arg)
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
                cJSON* color_values = cJSON_AddObjectToObject(cjson_params, ezlopi_value_str);
                if (color_values)
                {
                    cJSON_AddNumberToObject(color_values, "wwhite", rgb_args->wwhite_struct.value);
                    cJSON_AddNumberToObject(color_values, "cwhite", rgb_args->cwhite_struct.value);
                    cJSON_AddNumberToObject(color_values, "red", rgb_args->red_struct.value);
                    cJSON_AddNumberToObject(color_values, "green", rgb_args->green_struct.value);
                    cJSON_AddNumberToObject(color_values, "blue", rgb_args->blue_struct.value);
                    cJSON_AddNumberToObject(color_values, "amber", rgb_args->amber_struct.value);
                    cJSON_AddNumberToObject(color_values, "cyan", rgb_args->cyan_struct.value);
                    cJSON_AddNumberToObject(color_values, "purple", rgb_args->purple_struct.value);
                    cJSON_AddNumberToObject(color_values, "indexed", rgb_args->indexed_struct.value);

                    char* formatted_val = ezlopi_valueformatter_rgb(rgb_args->red_struct.value, rgb_args->green_struct.value, rgb_args->blue_struct.value);
                    if (formatted_val)
                    {
                        cJSON_AddStringToObject(cjson_params, ezlopi_valueFormatted_str, formatted_val);
                        free(formatted_val);
                    }
                }
            }
            else if (ezlopi_item_name_switch == item->cloud_properties.item_name)
            {
                int state = ((0 == rgb_args->brightness) ? 0 : 1);
                cJSON_AddBoolToObject(cjson_params, ezlopi_value_str, state);
                cJSON_AddStringToObject(cjson_params, ezlopi_valueFormatted_str, ezlopi_valueformatter_bool(state ? true : false));
            }
            else if (ezlopi_item_name_dimmer == item->cloud_properties.item_name)
            {
                int dim_percentage = (int)(rgb_args->brightness * 100);
                cJSON_AddNumberToObject(cjson_params, ezlopi_value_str, dim_percentage);
                char* formatted_val = ezlopi_valueformatter_int(dim_percentage);
                if (formatted_val)
                {
                    cJSON_AddStringToObject(cjson_params, ezlopi_valueFormatted_str, formatted_val);
                    free(formatted_val);
                }
            }
            else if (ezlopi_item_name_dimmer_up == item->cloud_properties.item_name)
            {
                cJSON_AddNumberToObject(cjson_params, ezlopi_value_str, 0);
                char* formatted_val = ezlopi_valueformatter_int(0);
                if (formatted_val)
                {
                    cJSON_AddStringToObject(cjson_params, ezlopi_valueFormatted_str, formatted_val);
                    free(formatted_val);
                }
            }
            else if (ezlopi_item_name_dimmer_down == item->cloud_properties.item_name)
            {
                cJSON_AddNumberToObject(cjson_params, ezlopi_value_str, 0);
                char* formatted_val = ezlopi_valueformatter_int(0);
                if (formatted_val)
                {
                    cJSON_AddStringToObject(cjson_params, ezlopi_valueFormatted_str, formatted_val);
                    free(formatted_val);
                }
            }
            else if (ezlopi_item_name_dimmer_stop == item->cloud_properties.item_name)
            {
                cJSON_AddNumberToObject(cjson_params, ezlopi_value_str, 0);
                char* formatted_val = ezlopi_valueformatter_int(0);
                if (formatted_val)
                {
                    cJSON_AddStringToObject(cjson_params, ezlopi_valueFormatted_str, formatted_val);
                    free(formatted_val);
                }
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
                cJSON* cjson_params_rgb_values = cJSON_GetObjectItem(cjson_params, ezlopi_value_str);

                CJSON_TRACE("cjson_params_rgb_values", cjson_params_rgb_values);
                CJSON_GET_VALUE_INT(cjson_params_rgb_values, "wwhite", rgb_args->wwhite_struct.value);
                CJSON_GET_VALUE_INT(cjson_params_rgb_values, "cwhite", rgb_args->cwhite_struct.value);
                CJSON_GET_VALUE_INT(cjson_params_rgb_values, "red", rgb_args->red_struct.value);
                CJSON_GET_VALUE_INT(cjson_params_rgb_values, "green", rgb_args->green_struct.value);
                CJSON_GET_VALUE_INT(cjson_params_rgb_values, "blue", rgb_args->blue_struct.value);
                CJSON_GET_VALUE_INT(cjson_params_rgb_values, "amber", rgb_args->amber_struct.value);
                CJSON_GET_VALUE_INT(cjson_params_rgb_values, "cyan", rgb_args->cyan_struct.value);
                CJSON_GET_VALUE_INT(cjson_params_rgb_values, "purple", rgb_args->purple_struct.value);
                CJSON_GET_VALUE_INT(cjson_params_rgb_values, "indexed", rgb_args->indexed_struct.value);

                RGB_LED_change_color_value(rgb_args);
            }
            else if (ezlopi_item_name_switch == item->cloud_properties.item_name)
            {
                int led_state = 0;
                CJSON_GET_VALUE_INT(cjson_params, ezlopi_value_str, led_state);
                rgb_args->previous_dim_factor = ((0 == led_state) ? rgb_args->brightness : rgb_args->previous_dim_factor);
                rgb_args->brightness = ((0 == led_state) ? 0.0 : ((0 == rgb_args->previous_dim_factor) ? 1.0 : rgb_args->previous_dim_factor));
                RGB_LED_change_color_value(rgb_args);
                ezlopi_device_value_updated_from_device_v3(rgb_args->RGB_LED_onoff_switch_item);
            }
            else if (ezlopi_item_name_dimmer == item->cloud_properties.item_name)
            {
                int dim_percent = 0;
                CJSON_GET_VALUE_INT(cjson_params, ezlopi_value_str, dim_percent);
                float dim_brightness_factor = dim_percent / 100.0;
                TRACE_W("dim_percent %d, dim_brightness_factor is %f", dim_percent, dim_brightness_factor);
                rgb_args->brightness = dim_brightness_factor;
                RGB_LED_change_color_value(rgb_args);
                ezlopi_device_value_updated_from_device_v3(rgb_args->RGB_LED_dimmer_item);
            }
            // else if (ezlopi_item_name_dimmer_up == item->cloud_properties.item_name)
            // {
            //     int dim_percent = 0;
            //      cJSON* cjson_params_dimer_values = cJSON_GetObjectItem(cjson_params, ezlopi_value_str);
            //     CJSON_GET_VALUE_INT(cjson_params_dimer_values, "wwhite", rgb_args->wwhite_struct.value)
            //     CJSON_GET_VALUE_INT(cjson_params, ezlopi_value_str, dim_percent);
            //     float dim_brightness_factor = dim_percent / 100.0;
            //     TRACE_W("dim_percent %d, dim_brightness_factor is %f", dim_percent, dim_brightness_factor);
            //     rgb_args->brightness = dim_brightness_factor;
            //     RGB_LED_change_color_value(rgb_args);
            //     ezlopi_device_value_updated_from_device_v3(rgb_args->RGB_LED_item_dimmer_up);
            // }
            // else if (ezlopi_item_name_dimmer_down == item->cloud_properties.item_name)
            // {
            //     int dim_percent = 0;
            //     CJSON_GET_VALUE_INT(cjson_params, ezlopi_value_str, dim_percent);
            //     float dim_brightness_factor = dim_percent / 100.0;
            //     TRACE_W("dim_percent %d, dim_brightness_factor is %f", dim_percent, dim_brightness_factor);
            //     rgb_args->brightness = dim_brightness_factor;
            //     RGB_LED_change_color_value(rgb_args);
            //     ezlopi_device_value_updated_from_device_v3(rgb_args->RGB_LED_item_dimmer_down);
            // }
            // else if (ezlopi_item_name_dimmer_stop == item->cloud_properties.item_name)
            // {
            //     int dim_percent = 0;
            //     CJSON_GET_VALUE_INT(cjson_params, ezlopi_value_str, dim_percent);
            //     float dim_brightness_factor = dim_percent / 100.0;
            //     TRACE_W("dim_percent %d, dim_brightness_factor is %f", dim_percent, dim_brightness_factor);
            //     rgb_args->brightness = dim_brightness_factor;
            //     RGB_LED_change_color_value(rgb_args);
            //     ezlopi_device_value_updated_from_device_v3(rgb_args->RGB_LED_item_dimmer_stop);
            // }
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
            if (GPIO_IS_VALID_GPIO(rgb_args->wwhite_struct.gpio_num) &&
                GPIO_IS_VALID_GPIO(rgb_args->cwhite_struct.gpio_num) &&
                GPIO_IS_VALID_GPIO(rgb_args->red_struct.gpio_num) &&
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
                        free(RGB_LED_red_channel_speed);
                    }

                    s_ezlopi_channel_speed_t* RGB_LED_green_channel_speed = ezlopi_pwm_init(rgb_args->green_struct.gpio_num, rgb_args->green_struct.pwm_resln, rgb_args->green_struct.freq_hz, rgb_args->green_struct.duty_cycle);
                    if (RGB_LED_green_channel_speed)
                    {
                        rgb_args->green_struct.channel = RGB_LED_green_channel_speed->channel;
                        rgb_args->green_struct.speed_mode = RGB_LED_green_channel_speed->speed_mode;
                        TRACE_D("green channel is %d", rgb_args->green_struct.channel);
                        free(RGB_LED_green_channel_speed);
                    }

                    s_ezlopi_channel_speed_t* RGB_LED_blue_channel_speed = ezlopi_pwm_init(rgb_args->blue_struct.gpio_num, rgb_args->blue_struct.pwm_resln, rgb_args->blue_struct.freq_hz, rgb_args->blue_struct.duty_cycle);
                    if (RGB_LED_blue_channel_speed)
                    {
                        rgb_args->blue_struct.channel = RGB_LED_blue_channel_speed->channel;
                        rgb_args->blue_struct.speed_mode = RGB_LED_blue_channel_speed->speed_mode;
                        TRACE_D("blue channel is %d", rgb_args->blue_struct.channel);
                        free(RGB_LED_blue_channel_speed);
                    }

                    s_ezlopi_channel_speed_t* RGB_LED_wwhite_channel_speed = ezlopi_pwm_init(rgb_args->wwhite_struct.gpio_num, rgb_args->wwhite_struct.pwm_resln, rgb_args->wwhite_struct.freq_hz, rgb_args->wwhite_struct.duty_cycle);
                    if (RGB_LED_wwhite_channel_speed)
                    {
                        rgb_args->wwhite_struct.channel = RGB_LED_wwhite_channel_speed->channel;
                        rgb_args->wwhite_struct.speed_mode = RGB_LED_wwhite_channel_speed->speed_mode;
                        TRACE_D("wwhite channel is %d", rgb_args->wwhite_struct.channel);
                        free(RGB_LED_wwhite_channel_speed);
                    }

                    s_ezlopi_channel_speed_t* RGB_LED_cwhite_channel_speed = ezlopi_pwm_init(rgb_args->cwhite_struct.gpio_num, rgb_args->cwhite_struct.pwm_resln, rgb_args->cwhite_struct.freq_hz, rgb_args->cwhite_struct.duty_cycle);
                    if (RGB_LED_cwhite_channel_speed)
                    {
                        rgb_args->cwhite_struct.channel = RGB_LED_cwhite_channel_speed->channel;
                        rgb_args->cwhite_struct.speed_mode = RGB_LED_cwhite_channel_speed->speed_mode;
                        TRACE_D("cwhite channel is %d", rgb_args->cwhite_struct.channel);
                        free(RGB_LED_cwhite_channel_speed);
                    }

                    RGB_LED_change_color_value(rgb_args);

                    rgb_args->RGB_LED_initialized = true;
                    ret = 1;
                }
                // else
                // {
                //     ret = -1;
                // }
            }
            // else
            // {
            //     ret = -1;
            //     ezlopi_device_free_device_by_item(item);
            // }
        }
        // else
        // {
        //     ret = -1;
        //     ezlopi_device_free_device_by_item(item);
        // }
    }

    return ret;
}

static void __prepare_device_cloud_properties(l_ezlopi_device_t* device, cJSON* cj_device)
{
    // char *device_name = NULL;
    // CJSON_GET_VALUE_STRING(cj_device, ezlopi_dev_name_str, device_name);
    // ASSIGN_DEVICE_NAME_V2(device, device_name);
    // device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();

    device->cloud_properties.category = category_dimmable_light;
    device->cloud_properties.subcategory = subcategory_dimmable_colored;
    device->cloud_properties.device_type = dev_type_dimmer_outlet;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_RGB_LED_user_args(s_rgb_args_t* rgb_args, cJSON* cj_device)
{

    rgb_args->wwhite_struct.gpio_num = 27;
    rgb_args->cwhite_struct.gpio_num = 32;
    rgb_args->red_struct.gpio_num = 33;
    rgb_args->green_struct.gpio_num = 25;
    rgb_args->blue_struct.gpio_num = 26;

    rgb_args->wwhite_struct.duty_cycle = 0;
    rgb_args->cwhite_struct.duty_cycle = 0;
    rgb_args->red_struct.duty_cycle = 0;
    rgb_args->green_struct.duty_cycle = 0;
    rgb_args->blue_struct.duty_cycle = 0;

    rgb_args->wwhite_struct.freq_hz = 10000;
    rgb_args->cwhite_struct.freq_hz = 10000;
    rgb_args->red_struct.freq_hz = 10000;
    rgb_args->green_struct.freq_hz = 10000;
    rgb_args->blue_struct.freq_hz = 10000;

    rgb_args->wwhite_struct.pwm_resln = 8;
    rgb_args->cwhite_struct.pwm_resln = 8;
    rgb_args->red_struct.pwm_resln = 8;
    rgb_args->green_struct.pwm_resln = 8;
    rgb_args->blue_struct.pwm_resln = 8;

    rgb_args->wwhite_struct.value = 0;
    rgb_args->cwhite_struct.value = 0;
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
    item->cloud_properties.item_name = ezlopi_item_name_rgbcolor;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.value_type = value_type_rgb;
    item->interface_type = EZLOPI_DEVICE_INTERFACE_PWM;
    item->interface.pwm.gpio_num = 0;
    item->interface.pwm.channel = 0;
    item->interface.pwm.value = 0;
    item->interface.pwm.duty_cycle = 0;
    item->interface.pwm.freq_hz = 10000;
    item->interface.pwm.pwm_resln = 8;
    item->user_arg = user_arg;
}

static void __prepare_RGB_LED_onoff_switch_item(l_ezlopi_item_t* item, cJSON* cj_device, void* user_arg)
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
    item->user_arg = user_arg;
}

static void __prepare_RGB_LED_dimmer_item(l_ezlopi_item_t* item, cJSON* cj_device, void* user_arg)
{
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.item_name = ezlopi_item_name_dimmer;
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
    item->user_arg = user_arg;
}

static void __prepare_RGB_LED_dimmer_up_item_properties(l_ezlopi_item_t* item, cJSON* cj_device, void* user_arg)
{
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.item_name = ezlopi_item_name_dimmer_up;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.value_type = value_type_int;
    item->interface_type = EZLOPI_DEVICE_INTERFACE_PWM;
    // item->interface.pwm.gpio_num = 0;
    // item->interface.pwm.channel = 0;
    // item->interface.pwm.value = 0;
    // item->interface.pwm.duty_cycle = 0;
    // item->interface.pwm.freq_hz = 5000;
    // item->interface.pwm.pwm_resln = 8;
    item->user_arg = user_arg;
}

static void __prepare_RGB_LED_dimmer_down_item_properties(l_ezlopi_item_t* item, cJSON* cj_device, void* user_arg)
{
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.item_name = ezlopi_item_name_dimmer_down;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.value_type = value_type_int;
    item->interface_type = EZLOPI_DEVICE_INTERFACE_PWM;
    // item->interface.pwm.gpio_num = 0;
    // item->interface.pwm.channel = 0;
    // item->interface.pwm.value = 0;
    // item->interface.pwm.duty_cycle = 0;
    // item->interface.pwm.freq_hz = 5000;
    // item->interface.pwm.pwm_resln = 8;
    item->user_arg = user_arg;
}

static void __prepare_RGB_LED_dimmer_stop_item_properties(l_ezlopi_item_t* item, cJSON* cj_device, void* user_arg)
{
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.item_name = ezlopi_item_name_dimmer_stop;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.value_type = value_type_int;
    item->interface_type = EZLOPI_DEVICE_INTERFACE_PWM;
    // item->interface.pwm.gpio_num = 0;
    // item->interface.pwm.channel = 0;
    // item->interface.pwm.value = 0;
    // item->interface.pwm.duty_cycle = 0;
    // item->interface.pwm.freq_hz = 5000;
    // item->interface.pwm.pwm_resln = 8;
    item->user_arg = user_arg;
}

static int __prepare(void* arg)
{
    int ret = 0;

    s_ezlopi_prep_arg_t* prep_arg = (s_ezlopi_prep_arg_t*)arg;
    if (prep_arg && prep_arg->cjson_device)
    {
        l_ezlopi_device_t* RGB_device = ezlopi_device_add_device(prep_arg->cjson_device);
        if (RGB_device)
        {
            s_rgb_args_t* rgb_args = malloc(sizeof(s_rgb_args_t));
            if (rgb_args)
            {
                memset(rgb_args, 0, sizeof(s_rgb_args_t));
                rgb_args->previous_dim_factor = 1.0;

                __prepare_device_cloud_properties(RGB_device, prep_arg->cjson_device);
                __prepare_RGB_LED_user_args(rgb_args, prep_arg->cjson_device);

                rgb_args->RGB_LED_item = ezlopi_device_add_item_to_device(RGB_device, device_0070_other_RGB_CW);
                if (rgb_args->RGB_LED_item)
                {
                    rgb_args->RGB_LED_item->cloud_properties.device_id = RGB_device->cloud_properties.device_id;
                    __prepare_RGB_LED_item(rgb_args->RGB_LED_item, prep_arg->cjson_device, rgb_args);
                }

                rgb_args->RGB_LED_onoff_switch_item = ezlopi_device_add_item_to_device(RGB_device, device_0070_other_RGB_CW);
                if (rgb_args->RGB_LED_onoff_switch_item)
                {
                    rgb_args->RGB_LED_onoff_switch_item->cloud_properties.device_id = RGB_device->cloud_properties.device_id;
                    __prepare_RGB_LED_onoff_switch_item(rgb_args->RGB_LED_onoff_switch_item, prep_arg->cjson_device, rgb_args);
                }

                rgb_args->RGB_LED_dimmer_item = ezlopi_device_add_item_to_device(RGB_device, device_0070_other_RGB_CW);
                if (rgb_args->RGB_LED_dimmer_item)
                {
                    rgb_args->RGB_LED_dimmer_item->cloud_properties.device_id = RGB_device->cloud_properties.device_id;
                    __prepare_RGB_LED_dimmer_item(rgb_args->RGB_LED_dimmer_item, prep_arg->cjson_device, rgb_args);
                }

                rgb_args->RGB_LED_item_dimmer_up = ezlopi_device_add_item_to_device(RGB_device, device_0070_other_RGB_CW);
                if (rgb_args->RGB_LED_item_dimmer_up)
                {
                    rgb_args->RGB_LED_item_dimmer_up->user_arg = rgb_args;
                    rgb_args->RGB_LED_item_dimmer_up->cloud_properties.device_id = RGB_device->cloud_properties.device_id;
                    __prepare_RGB_LED_dimmer_up_item_properties(rgb_args->RGB_LED_item_dimmer_up, prep_arg->cjson_device, rgb_args);
                }

                rgb_args->RGB_LED_item_dimmer_down = ezlopi_device_add_item_to_device(RGB_device, device_0070_other_RGB_CW);
                if (rgb_args->RGB_LED_item_dimmer_down)
                {
                    rgb_args->RGB_LED_item_dimmer_down->user_arg = rgb_args;
                    rgb_args->RGB_LED_item_dimmer_down->cloud_properties.device_id = RGB_device->cloud_properties.device_id;
                    __prepare_RGB_LED_dimmer_down_item_properties(rgb_args->RGB_LED_item_dimmer_down, prep_arg->cjson_device, rgb_args);
                }

                rgb_args->RGB_LED_item_dimmer_stop = ezlopi_device_add_item_to_device(RGB_device, device_0070_other_RGB_CW);
                if (rgb_args->RGB_LED_item_dimmer_stop)
                {
                    rgb_args->RGB_LED_item_dimmer_stop->user_arg = rgb_args;
                    rgb_args->RGB_LED_item_dimmer_stop->cloud_properties.device_id = RGB_device->cloud_properties.device_id;
                    __prepare_RGB_LED_dimmer_stop_item_properties(rgb_args->RGB_LED_item_dimmer_stop, prep_arg->cjson_device, rgb_args);
                }


                ret = 1;

                if (!rgb_args->RGB_LED_item &&
                    !rgb_args->RGB_LED_onoff_switch_item &&
                    !rgb_args->RGB_LED_dimmer_item &&
                    !rgb_args->RGB_LED_item_dimmer_up &&
                    !rgb_args->RGB_LED_item_dimmer_down &&
                    !rgb_args->RGB_LED_item_dimmer_stop
                    )
                {
                    free(rgb_args);
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