/* ===========================================================================
** Copyright (C) 2024 Ezlo Innovation Inc
**
** Under EZLO AVAILABLE SOURCE LICENSE (EASL) AGREEMENT
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/
/**
 * @file    device_0038_other_RGB.c
 * @brief   perform some function on device_0038
 * @author  Lomas Subedi
 *          Riken Maharjan
 *          Nabin Dangi
 * @version 0.1
 * @date    xx
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_hal_pwm.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "device_0038_other_RGB.h"
#include "EZLOPI_USER_CONFIG.h"

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/

typedef struct s_rgb_args
{
    bool RGB_LED_initialized;
    l_ezlopi_item_t *RGB_LED_item;
    l_ezlopi_item_t *RGB_LED_dimmer_item;
    l_ezlopi_item_t *RGB_LED_onoff_switch_item;

    float brightness;
    s_ezlopi_pwm_t red_struct;
    s_ezlopi_pwm_t green_struct;
    s_ezlopi_pwm_t blue_struct;
    float previous_dim_factor;
} s_rgb_args_t;

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
static ezlopi_error_t __prepare(void *arg);
static ezlopi_error_t __init(l_ezlopi_item_t *item);
static ezlopi_error_t __set_cjson_value(l_ezlopi_item_t *item, void *arg);
static ezlopi_error_t __get_cjson_value(l_ezlopi_item_t *item, void *arg);

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
ezlopi_error_t DEVICE_0038_other_rgb(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
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
    {
        ret = __get_cjson_value(item, arg);
        break;
    }
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

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/

static ezlopi_error_t RGB_LED_change_color_value(s_rgb_args_t *rgb_args)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (rgb_args)
    {
        TRACE_D("Brightness value is %d, %d, %d", (uint8_t)(rgb_args->red_struct.value * rgb_args->brightness), (uint8_t)(rgb_args->green_struct.value * rgb_args->brightness),
                (uint8_t)(rgb_args->blue_struct.value * rgb_args->brightness));

        EZPI_hal_pwm_change_duty(rgb_args->red_struct.channel, rgb_args->red_struct.speed_mode, (uint8_t)(rgb_args->red_struct.value * rgb_args->brightness));
        EZPI_hal_pwm_change_duty(rgb_args->green_struct.channel, rgb_args->green_struct.speed_mode, (uint8_t)(rgb_args->green_struct.value * rgb_args->brightness));
        EZPI_hal_pwm_change_duty(rgb_args->blue_struct.channel, rgb_args->blue_struct.speed_mode, (uint8_t)(rgb_args->blue_struct.value * rgb_args->brightness));
        ret = EZPI_SUCCESS;
    }
    return ret;
}

// static int __get_cjson_value(l_ezlopi_item_t* item, void* arg)
// {
//     ezlopi_error_t ret = EZPI_FAILED;
//     if (item && arg)
//     {
//         cJSON* cj_properties = (cJSON*)arg;
//         s_rgb_args_t* rgb_args = (s_rgb_args_t*)item->user_arg;
//         if ((NULL != cj_properties) && (NULL != rgb_args))
//         {
//             if (ezlopi_item_name_rgbcolor == item->cloud_properties.item_name)
//             {
//                 cJSON* color_values = cJSON_AddObjectToObject(__FUNCTION__, cj_properties, ezlopi_value_str);
//                 if (color_values)
//                 {
//                     cJSON_AddNumberToObject(__FUNCTION__, color_values, ezlopi_red_str, rgb_args->red_struct.value);
//                     cJSON_AddNumberToObject(__FUNCTION__, color_values, ezlopi_green_str, rgb_args->green_struct.value);
//                     cJSON_AddNumberToObject(__FUNCTION__, color_values, ezlopi_blue_str, rgb_args->blue_struct.value);
//                     char formatted_rgb_value[32];
//                     snprintf(formatted_rgb_value, sizeof(formatted_rgb_value), "#%02x%02x%02x", rgb_args->red_struct.value, rgb_args->green_struct.value, rgb_args->blue_struct.value);
//                     cJSON_AddStringToObject(__FUNCTION__, cj_properties, ezlopi_valueFormatted_str, formatted_rgb_value);
//                     ret = EZPI_SUCCESS;
//                 }
//             }
//             else if (ezlopi_item_name_switch == item->cloud_properties.item_name)
//             {
//                 EZPI_core_valueformatter_bool_to_cjson(cj_properties, rgb_args->brightness, item->cloud_properties.scale);
//             }
//             else if (ezlopi_item_name_dimmer == item->cloud_properties.item_name)
//             {
//                 int dim_percentage = (int)(rgb_args->brightness * 100);
//                 cJSON_AddNumberToObject(__FUNCTION__, cj_properties, ezlopi_minValue_str, 0);
//                 cJSON_AddNumberToObject(__FUNCTION__, cj_properties, ezlopi_maxValue_str, 100);
//                 EZPI_core_valueformatter_int32_to_cjson(cj_properties, dim_percentage, item->cloud_properties.scale);
//             }
//         }
//     }
//     return ret;
// }

static ezlopi_error_t __get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item && arg)
    {
        cJSON *cj_properties = (cJSON *)arg;
        s_rgb_args_t *rgb_args = (s_rgb_args_t *)item->user_arg;

        if ((NULL != cj_properties) && (NULL != rgb_args))
        {
            if (ezlopi_item_name_rgbcolor == item->cloud_properties.item_name)
            {
                cJSON *color_values = cJSON_AddObjectToObject(__FUNCTION__, cj_properties, ezlopi_value_str);
                if (color_values)
                {
                    cJSON_AddNumberToObject(__FUNCTION__, color_values, ezlopi_red_str, rgb_args->red_struct.value);
                    cJSON_AddNumberToObject(__FUNCTION__, color_values, ezlopi_green_str, rgb_args->green_struct.value);
                    cJSON_AddNumberToObject(__FUNCTION__, color_values, ezlopi_blue_str, rgb_args->blue_struct.value);

                    char formatted_rgb_value[32];
                    snprintf(formatted_rgb_value, sizeof(formatted_rgb_value), "#%02x%02x%02x", rgb_args->red_struct.value, rgb_args->green_struct.value, rgb_args->blue_struct.value);
                    cJSON_AddStringToObject(__FUNCTION__, cj_properties, ezlopi_valueFormatted_str, formatted_rgb_value);
                    ret = EZPI_SUCCESS;
                }
            }
            else if (ezlopi_item_name_switch == item->cloud_properties.item_name)
            {
                EZPI_core_valueformatter_bool_to_cjson(cj_properties, rgb_args->brightness, NULL);
                ret = EZPI_SUCCESS;
            }
            else if (ezlopi_item_name_dimmer == item->cloud_properties.item_name)
            {
                int dim_percentage = (int)(rgb_args->brightness * 100);
                EZPI_core_valueformatter_int32_to_cjson(cj_properties, dim_percentage, item->cloud_properties.scale);
                ret = EZPI_SUCCESS;
            }
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
        s_rgb_args_t *rgb_args = (s_rgb_args_t *)item->user_arg;
        if ((NULL != cjson_params) && (NULL != rgb_args))
        {
            if (ezlopi_item_name_rgbcolor == item->cloud_properties.item_name)
            {
                cJSON *cjson_params_rgb_values = cJSON_GetObjectItem(__FUNCTION__, cjson_params, ezlopi_value_str);

#ifdef CONFIG_EZPI_UTIL_TRACE_EN
                CJSON_TRACE("cjson_params_rgb_values", cjson_params_rgb_values);
#endif

                if (cjson_params_rgb_values)
                {
                    CJSON_GET_VALUE_INT(cjson_params_rgb_values, ezlopi_red_str, rgb_args->red_struct.value);
                    CJSON_GET_VALUE_INT(cjson_params_rgb_values, ezlopi_green_str, rgb_args->green_struct.value);
                    CJSON_GET_VALUE_INT(cjson_params_rgb_values, ezlopi_blue_str, rgb_args->blue_struct.value);
                }

                RGB_LED_change_color_value(rgb_args);
                ret = EZPI_SUCCESS;
            }
            if (ezlopi_item_name_switch == item->cloud_properties.item_name)
            {
                int led_state = 0;
                CJSON_GET_VALUE_INT(cjson_params, ezlopi_value_str, led_state);
                rgb_args->previous_dim_factor = ((0 == led_state) ? rgb_args->brightness : rgb_args->previous_dim_factor);
                rgb_args->brightness = ((0 == led_state) ? 0.0 : ((0 == rgb_args->previous_dim_factor) ? 1.0 : rgb_args->previous_dim_factor));
                TRACE_D("Brightness value is %d, %d, %d", (uint8_t)(rgb_args->red_struct.value * rgb_args->brightness), (uint8_t)(rgb_args->green_struct.value * rgb_args->brightness),
                        (uint8_t)(rgb_args->blue_struct.value * rgb_args->brightness));
                RGB_LED_change_color_value(rgb_args);
                EZPI_core_device_value_updated_from_device_broadcast(rgb_args->RGB_LED_dimmer_item);
                ret = EZPI_SUCCESS;
            }
            if (ezlopi_item_name_dimmer == item->cloud_properties.item_name)
            {
                int dim_percent = 0;
                CJSON_GET_VALUE_INT(cjson_params, ezlopi_value_str, dim_percent);
                float dim_brightness_factor = dim_percent / 100.0;
                TRACE_D("dim_percent %d, dim_brightness_factor is %f", dim_percent, dim_brightness_factor);
                rgb_args->brightness = dim_brightness_factor;
                RGB_LED_change_color_value(rgb_args);
                EZPI_core_device_value_updated_from_device_broadcast(rgb_args->RGB_LED_onoff_switch_item);
                ret = EZPI_SUCCESS;
            }
        }
    }
    return ret;
}

static ezlopi_error_t __init(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_ERR_INIT_DEVICE_FAILED;

    if (item)
    {
        s_rgb_args_t *rgb_args = (s_rgb_args_t *)item->user_arg;
        if (rgb_args)
        {
            if (GPIO_IS_VALID_GPIO(rgb_args->red_struct.gpio_num) &&
                GPIO_IS_VALID_GPIO(rgb_args->green_struct.gpio_num) &&
                GPIO_IS_VALID_GPIO(rgb_args->blue_struct.gpio_num))
            {
                if (false == rgb_args->RGB_LED_initialized)
                {
                    s_ezlopi_channel_speed_t *RGB_LED_red_channel_speed = EZPI_hal_pwm_init(rgb_args->red_struct.gpio_num, rgb_args->red_struct.pwm_resln, rgb_args->red_struct.freq_hz, rgb_args->red_struct.duty_cycle);
                    if (RGB_LED_red_channel_speed)
                    {
                        rgb_args->red_struct.channel = RGB_LED_red_channel_speed->channel;
                        rgb_args->red_struct.speed_mode = RGB_LED_red_channel_speed->speed_mode;
                        TRACE_D("red channel is %d", rgb_args->red_struct.channel);
                        ezlopi_free(__FUNCTION__, RGB_LED_red_channel_speed);
                    }

                    s_ezlopi_channel_speed_t *RGB_LED_green_channel_speed = EZPI_hal_pwm_init(rgb_args->green_struct.gpio_num, rgb_args->green_struct.pwm_resln, rgb_args->green_struct.freq_hz, rgb_args->green_struct.duty_cycle);
                    if (RGB_LED_green_channel_speed)
                    {
                        rgb_args->green_struct.channel = RGB_LED_green_channel_speed->channel;
                        rgb_args->green_struct.speed_mode = RGB_LED_green_channel_speed->speed_mode;
                        TRACE_D("green channel is %d", rgb_args->green_struct.channel);
                        ezlopi_free(__FUNCTION__, RGB_LED_green_channel_speed);
                    }

                    s_ezlopi_channel_speed_t *RGB_LED_blue_channel_speed = EZPI_hal_pwm_init(rgb_args->blue_struct.gpio_num, rgb_args->blue_struct.pwm_resln, rgb_args->blue_struct.freq_hz, rgb_args->blue_struct.duty_cycle);
                    if (RGB_LED_blue_channel_speed)
                    {
                        rgb_args->blue_struct.channel = RGB_LED_blue_channel_speed->channel;
                        rgb_args->blue_struct.speed_mode = RGB_LED_blue_channel_speed->speed_mode;
                        TRACE_D("blue channel is %d", rgb_args->blue_struct.channel);
                        ezlopi_free(__FUNCTION__, RGB_LED_blue_channel_speed);
                    }

                    RGB_LED_change_color_value(rgb_args);

                    rgb_args->RGB_LED_initialized = true;
                    ret = EZPI_SUCCESS;
                }
            }
        }
    }
    return ret;
}

static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    device->cloud_properties.category = category_dimmable_light;
    device->cloud_properties.subcategory = subcategory_dimmable_colored;
    device->cloud_properties.device_type = dev_type_dimmer_outlet;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_RGB_LED_user_args(s_rgb_args_t *rgb_args, cJSON *cj_device)
{
    CJSON_GET_VALUE_INT(cj_device, ezlopi_gpio1_str, rgb_args->red_struct.gpio_num);
    CJSON_GET_VALUE_INT(cj_device, ezlopi_gpio2_str, rgb_args->green_struct.gpio_num);
    CJSON_GET_VALUE_INT(cj_device, ezlopi_gpio3_str, rgb_args->blue_struct.gpio_num);

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

static void __prepare_RGB_LED_item(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg)
{
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.item_id = EZPI_core_cloud_generate_item_id();
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

static void __prepare_RGB_LED_onoff_switch_item(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg)
{
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.item_id = EZPI_core_cloud_generate_item_id();
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

static void __prepare_RGB_LED_dimmer_item(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg)
{
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.item_id = EZPI_core_cloud_generate_item_id();
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

static ezlopi_error_t __prepare(void *arg)
{
    ezlopi_error_t ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;

    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (prep_arg && prep_arg->cjson_device)
    {
        l_ezlopi_device_t *RGB_device = EZPI_core_device_add_device(prep_arg->cjson_device, NULL);
        if (RGB_device)
        {
            s_rgb_args_t *rgb_args = ezlopi_malloc(__FUNCTION__, sizeof(s_rgb_args_t));
            if (rgb_args)
            {
                memset(rgb_args, 0, sizeof(s_rgb_args_t));
                rgb_args->previous_dim_factor = 1.0;

                __prepare_device_cloud_properties(RGB_device, prep_arg->cjson_device);
                __prepare_RGB_LED_user_args(rgb_args, prep_arg->cjson_device);

                rgb_args->RGB_LED_item = EZPI_core_device_add_item_to_device(RGB_device, DEVICE_0038_other_rgb);
                if (rgb_args->RGB_LED_item)
                {
                    __prepare_RGB_LED_item(rgb_args->RGB_LED_item, prep_arg->cjson_device, rgb_args);
                }

                rgb_args->RGB_LED_onoff_switch_item = EZPI_core_device_add_item_to_device(RGB_device, DEVICE_0038_other_rgb);
                if (rgb_args->RGB_LED_onoff_switch_item)
                {
                    __prepare_RGB_LED_onoff_switch_item(rgb_args->RGB_LED_onoff_switch_item, prep_arg->cjson_device, rgb_args);
                }

                rgb_args->RGB_LED_dimmer_item = EZPI_core_device_add_item_to_device(RGB_device, DEVICE_0038_other_rgb);
                if (rgb_args->RGB_LED_dimmer_item)
                {
                    __prepare_RGB_LED_dimmer_item(rgb_args->RGB_LED_dimmer_item, prep_arg->cjson_device, rgb_args);
                }
                //-----------------------------------------------------------------------------------------------------
                if (!rgb_args->RGB_LED_item && !rgb_args->RGB_LED_onoff_switch_item && !rgb_args->RGB_LED_dimmer_item)
                {
                    ezlopi_free(__FUNCTION__, rgb_args);
                    EZPI_core_device_free_device(RGB_device);
                }
                else
                {
                    ret = EZPI_SUCCESS;
                }
            }
            else
            {
                EZPI_core_device_free_device(RGB_device);
            }
        }
    }

    return ret;
}

/*******************************************************************************
 *                          End of File
 *******************************************************************************/