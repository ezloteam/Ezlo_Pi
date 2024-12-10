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
 * @file    main.c
 * @brief   perform some function on data
 * @author  John Doe
 * @version 0.1
 * @date    1st January 2024
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <math.h>
#include "ezlopi_util_trace.h"

// #include "ezlopi_core_timer.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"
#include "ezlopi_core_errors.h"

#include "ezlopi_hal_pwm.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "device_0022_PWM_dimmable_lamp.h"
#include "EZLOPI_USER_CONFIG.h"

typedef struct s_dimmable_bulb_properties
{
    bool dimmable_bulb_initialized;
    uint32_t previous_brightness_value;
    uint32_t current_brightness_value;
    l_ezlopi_item_t *item_dimmer;
    l_ezlopi_item_t *item_dimmer_up;
    l_ezlopi_item_t *item_dimmer_down;
    l_ezlopi_item_t *item_dimmer_stop;
    l_ezlopi_item_t *item_dimmer_switch;

} s_dimmable_bulb_properties_t;

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
static ezlopi_error_t __prepare(void *arg);
static ezlopi_error_t __init(l_ezlopi_item_t *item);
static ezlopi_error_t __list_cjson_value(l_ezlopi_item_t *item, void *arg);
static ezlopi_error_t __get_cjson_value(l_ezlopi_item_t *item, void *arg);
static ezlopi_error_t __set_cjson_value(l_ezlopi_item_t *item, void *arg);

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/

/**
 * @brief Global/extern function template example
 * Convention : Use capital letter for initial word on extern function
 * @param arg
 */
ezlopi_error_t device_0022_PWM_dimmable_lamp(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
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
        __list_cjson_value(item, arg);
        break;
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
 *                          Static Function Definitions
 *******************************************************************************/
static ezlopi_error_t __set_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_FAILED;

    cJSON *device_details = (cJSON *)arg;
    if (device_details)
    {
        s_dimmable_bulb_properties_t *dimmable_bulb_arg = (s_dimmable_bulb_properties_t *)item->user_arg;
        if (dimmable_bulb_arg)
        {
            if (ezlopi_item_name_dimmer == item->cloud_properties.item_name)
            {
                int value = 0;
                CJSON_GET_VALUE_DOUBLE(device_details, ezlopi_value_str, value);
                int target_value = (int)ceil(((value * 4095.0) / 100.0));
                ezlopi_pwm_change_duty(item->interface.pwm.channel, item->interface.pwm.speed_mode, target_value);
                dimmable_bulb_arg->previous_brightness_value = dimmable_bulb_arg->current_brightness_value;
                dimmable_bulb_arg->current_brightness_value = target_value;
                ezlopi_device_value_updated_from_device_broadcast(dimmable_bulb_arg->item_dimmer);
                ezlopi_device_value_updated_from_device_broadcast(dimmable_bulb_arg->item_dimmer_switch);
                ret = EZPI_SUCCESS;
            }
            else if (ezlopi_item_name_switch == item->cloud_properties.item_name)
            {
                bool switch_state = false;
                CJSON_GET_VALUE_BOOL(device_details, ezlopi_value_str, switch_state);
                dimmable_bulb_arg->previous_brightness_value = (false == switch_state) ? dimmable_bulb_arg->current_brightness_value : dimmable_bulb_arg->previous_brightness_value;
                dimmable_bulb_arg->current_brightness_value = (false == switch_state) ? 0 : (0 == dimmable_bulb_arg->previous_brightness_value ? 4095 : dimmable_bulb_arg->previous_brightness_value);
                ezlopi_pwm_change_duty(dimmable_bulb_arg->item_dimmer->interface.pwm.channel, dimmable_bulb_arg->item_dimmer->interface.pwm.speed_mode, dimmable_bulb_arg->current_brightness_value);
                ezlopi_device_value_updated_from_device_broadcast(dimmable_bulb_arg->item_dimmer);
                ezlopi_device_value_updated_from_device_broadcast(dimmable_bulb_arg->item_dimmer_switch);
                ret = EZPI_SUCCESS;
            }
        }
    }

    return ret;
}

static ezlopi_error_t __list_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_FAILED;

    cJSON *cj_properties = (cJSON *)arg;
    if (cj_properties && item && item->user_arg)
    {
        s_dimmable_bulb_properties_t *dimmable_bulb_arg = (s_dimmable_bulb_properties_t *)item->user_arg;
        if (ezlopi_item_name_dimmer == item->cloud_properties.item_name)
        {
            cJSON_AddNumberToObject(__FUNCTION__, cj_properties, ezlopi_minValue_str, 0);
            cJSON_AddNumberToObject(__FUNCTION__, cj_properties, ezlopi_maxValue_str, 100);

            int dimmable_value_percentage = (int)floor(((dimmable_bulb_arg->current_brightness_value * 100.0) / 4095.0));
            ezlopi_valueformatter_int32_to_cjson(cj_properties, dimmable_value_percentage, item->cloud_properties.scale);
        }
        else if (ezlopi_item_name_dimmer_up == item->cloud_properties.item_name)
        {
            ezlopi_valueformatter_int32_to_cjson(cj_properties, 0, item->cloud_properties.scale);
        }
        else if (ezlopi_item_name_dimmer_down == item->cloud_properties.item_name)
        {
            ezlopi_valueformatter_int32_to_cjson(cj_properties, 0, item->cloud_properties.scale);
        }
        else if (ezlopi_item_name_dimmer_stop == item->cloud_properties.item_name)
        {
            ezlopi_valueformatter_int32_to_cjson(cj_properties, 0, item->cloud_properties.scale);
        }
        else if (ezlopi_item_name_switch == item->cloud_properties.item_name)
        {
            ezlopi_valueformatter_bool_to_cjson(cj_properties, dimmable_bulb_arg->current_brightness_value, item->cloud_properties.scale);
        }
        ret = EZPI_SUCCESS;
    }

    return ret;
}


static ezlopi_error_t __get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_FAILED;

    cJSON *cj_properties = (cJSON *)arg;
    if (cj_properties && item && item->user_arg)
    {
        s_dimmable_bulb_properties_t *dimmable_bulb_arg = (s_dimmable_bulb_properties_t *)item->user_arg;
        if (ezlopi_item_name_dimmer == item->cloud_properties.item_name)
        {
            int dimmable_value_percentage = (int)floor(((dimmable_bulb_arg->current_brightness_value * 100.0) / 4095.0));
            ezlopi_valueformatter_int32_to_cjson(cj_properties, dimmable_value_percentage, item->cloud_properties.scale);
        }
        else if (ezlopi_item_name_dimmer_up == item->cloud_properties.item_name)
        {
            ezlopi_valueformatter_int32_to_cjson(cj_properties, 0, item->cloud_properties.scale);
        }
        else if (ezlopi_item_name_dimmer_down == item->cloud_properties.item_name)
        {
            ezlopi_valueformatter_int32_to_cjson(cj_properties, 0, item->cloud_properties.scale);
        }
        else if (ezlopi_item_name_dimmer_stop == item->cloud_properties.item_name)
        {
            ezlopi_valueformatter_int32_to_cjson(cj_properties, 0, item->cloud_properties.scale);
        }
        else if (ezlopi_item_name_switch == item->cloud_properties.item_name)
        {
            ezlopi_valueformatter_bool_to_cjson(cj_properties, dimmable_bulb_arg->current_brightness_value, item->cloud_properties.scale);
        }
        ret = EZPI_SUCCESS;
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
            s_dimmable_bulb_properties_t *dimmable_bulb_arg = (s_dimmable_bulb_properties_t *)item->user_arg;
            if (dimmable_bulb_arg)
            {
                if (0 == dimmable_bulb_arg->dimmable_bulb_initialized)
                {
                    s_ezlopi_channel_speed_t *ezlopi_dimmable_channel_speed = ezlopi_pwm_init(item->interface.pwm.gpio_num, item->interface.pwm.pwm_resln,
                        item->interface.pwm.freq_hz, item->interface.pwm.duty_cycle);
                    if (ezlopi_dimmable_channel_speed)
                    {
                        item->interface.pwm.channel = ezlopi_dimmable_channel_speed->channel;
                        TRACE_I("Channel is %d", item->interface.pwm.channel);
                        item->interface.pwm.speed_mode = ezlopi_dimmable_channel_speed->speed_mode;
                        dimmable_bulb_arg->current_brightness_value = item->interface.pwm.duty_cycle;
                        dimmable_bulb_arg->previous_brightness_value = item->interface.pwm.duty_cycle;
                        dimmable_bulb_arg->dimmable_bulb_initialized = true;
                        ezlopi_pwm_change_duty(item->interface.pwm.channel, item->interface.pwm.speed_mode, item->interface.pwm.duty_cycle);
                    }
                    else
                    {
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

static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    device->cloud_properties.category = category_dimmable_light;
    device->cloud_properties.subcategory = subcategory_dimmable_bulb;
    device->cloud_properties.device_type = dev_type_dimmer_outlet;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_dimmer_item_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_dev_type_str, item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.show = true;
    item->cloud_properties.item_name = ezlopi_item_name_dimmer;
    item->cloud_properties.value_type = value_type_int;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.scale = NULL;

    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_str, item->interface.pwm.gpio_num);
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_duty_cycle_str, item->interface.pwm.duty_cycle);
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_freq_hz_str, item->interface.pwm.freq_hz);
    item->interface.pwm.pwm_resln = 12;
}

static void __prepare_dimmer_up_item_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_dev_type_str, item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.show = true;
    item->cloud_properties.item_name = ezlopi_item_name_dimmer_up;
    item->cloud_properties.value_type = value_type_int;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.scale = NULL;

    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_str, item->interface.pwm.gpio_num);
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_duty_cycle_str, item->interface.pwm.duty_cycle);
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_freq_hz_str, item->interface.pwm.freq_hz);
    item->interface.pwm.pwm_resln = 12;
}

static void __prepare_dimmer_down_item_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_dev_type_str, item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.show = true;
    item->cloud_properties.item_name = ezlopi_item_name_dimmer_down;
    item->cloud_properties.value_type = value_type_int;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.scale = NULL;

    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_str, item->interface.pwm.gpio_num);
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_duty_cycle_str, item->interface.pwm.duty_cycle);
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_freq_hz_str, item->interface.pwm.freq_hz);
    item->interface.pwm.pwm_resln = 12;
}

static void __prepare_dimmer_stop_item_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_dev_type_str, item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.show = true;
    item->cloud_properties.item_name = ezlopi_item_name_dimmer_stop;
    item->cloud_properties.value_type = value_type_int;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.scale = NULL;

    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_str, item->interface.pwm.gpio_num);
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_duty_cycle_str, item->interface.pwm.duty_cycle);
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_freq_hz_str, item->interface.pwm.freq_hz);
    item->interface.pwm.pwm_resln = 12;
}

static void __prepare_dimmer_switch_item_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    item->interface_type = EZLOPI_DEVICE_INTERFACE_DIGITAL_OUTPUT;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.show = true;
    item->cloud_properties.item_name = ezlopi_item_name_switch;
    item->cloud_properties.value_type = value_type_bool;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.scale = NULL;

    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_str, item->interface.gpio.gpio_out.gpio_num);
    item->interface.gpio.gpio_in.enable = false;

    item->interface.gpio.gpio_out.enable = true;
    item->interface.gpio.gpio_out.interrupt = false;
    item->interface.gpio.gpio_out.invert = false;
    item->interface.gpio.gpio_out.mode = GPIO_MODE_OUTPUT;
    item->interface.gpio.gpio_out.pull = false;
    item->interface.gpio.gpio_out.value = true;
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
            ret = 1;
            __prepare_device_cloud_properties(device, prep_arg->cjson_device);

            s_dimmable_bulb_properties_t *dimmable_bulb_arg = ezlopi_malloc(__FUNCTION__, sizeof(s_dimmable_bulb_properties_t));
            if (dimmable_bulb_arg)
            {
                dimmable_bulb_arg->current_brightness_value = 0;
                dimmable_bulb_arg->previous_brightness_value = 4095;
                dimmable_bulb_arg->dimmable_bulb_initialized = false;

                dimmable_bulb_arg->item_dimmer = ezlopi_device_add_item_to_device(device, device_0022_PWM_dimmable_lamp);
                if (dimmable_bulb_arg->item_dimmer)
                {
                    dimmable_bulb_arg->item_dimmer->user_arg = dimmable_bulb_arg;
                    dimmable_bulb_arg->item_dimmer->is_user_arg_unique = true;
                    __prepare_dimmer_item_properties(dimmable_bulb_arg->item_dimmer, prep_arg->cjson_device);
                }

                dimmable_bulb_arg->item_dimmer_up = ezlopi_device_add_item_to_device(device, device_0022_PWM_dimmable_lamp);
                if (dimmable_bulb_arg->item_dimmer_up)
                {
                    dimmable_bulb_arg->item_dimmer_up->user_arg = dimmable_bulb_arg;
                    dimmable_bulb_arg->item_dimmer_up->is_user_arg_unique = true;
                    __prepare_dimmer_up_item_properties(dimmable_bulb_arg->item_dimmer_up, prep_arg->cjson_device);
                }

                dimmable_bulb_arg->item_dimmer_down = ezlopi_device_add_item_to_device(device, device_0022_PWM_dimmable_lamp);
                if (dimmable_bulb_arg->item_dimmer_down)
                {
                    dimmable_bulb_arg->item_dimmer_down->user_arg = dimmable_bulb_arg;
                    dimmable_bulb_arg->item_dimmer_down->is_user_arg_unique = true;
                    __prepare_dimmer_down_item_properties(dimmable_bulb_arg->item_dimmer_down, prep_arg->cjson_device);
                }

                dimmable_bulb_arg->item_dimmer_stop = ezlopi_device_add_item_to_device(device, device_0022_PWM_dimmable_lamp);
                if (dimmable_bulb_arg->item_dimmer_stop)
                {
                    dimmable_bulb_arg->item_dimmer_stop->user_arg = dimmable_bulb_arg;
                    dimmable_bulb_arg->item_dimmer_stop->is_user_arg_unique = true;
                    __prepare_dimmer_stop_item_properties(dimmable_bulb_arg->item_dimmer_stop, prep_arg->cjson_device);
                }

                dimmable_bulb_arg->item_dimmer_switch = ezlopi_device_add_item_to_device(device, device_0022_PWM_dimmable_lamp);
                if (dimmable_bulb_arg->item_dimmer_switch)
                {
                    dimmable_bulb_arg->item_dimmer_switch->user_arg = dimmable_bulb_arg;
                    dimmable_bulb_arg->item_dimmer_switch->is_user_arg_unique = true;
                    __prepare_dimmer_switch_item_properties(dimmable_bulb_arg->item_dimmer_switch, prep_arg->cjson_device);
                }

                if ((NULL == dimmable_bulb_arg->item_dimmer) ||
                    (NULL == dimmable_bulb_arg->item_dimmer_up) ||
                    (NULL == dimmable_bulb_arg->item_dimmer_down) ||
                    (NULL == dimmable_bulb_arg->item_dimmer_stop) ||
                    (NULL == dimmable_bulb_arg->item_dimmer_switch))
                {
                    ezlopi_device_free_device(device);
                    ezlopi_free(__FUNCTION__, dimmable_bulb_arg);
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

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
