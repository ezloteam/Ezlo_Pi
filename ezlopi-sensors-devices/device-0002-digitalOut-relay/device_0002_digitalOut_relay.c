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
 * @file    device_0002_digitalOut_relay.c
 * @brief   perform some function on device_0002
 * @authors Krishna Kumar Sah (work.krishnasah@gmail.com)
 *          Lomas Subedi
 *          Nabin Dangi
 *          Riken Maharjan
 * @version 1.0
 * @date    June 7th, 2023 6:14 PM
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "../../build/config/sdkconfig.h"

#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_hal_gpio.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "ezlopi_service_gpioisr.h"

#include "device_0002_digitalOut_relay.h"

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
static ezlopi_error_t __set_value(l_ezlopi_item_t *item, void *arg);
static ezlopi_error_t __get_value_cjson(l_ezlopi_item_t *item, void *arg);

static void __interrupt_upcall(void *arg);
static void __toggle_gpio(l_ezlopi_item_t *item);
static void __write_gpio_value(l_ezlopi_item_t *item);
static void __set_gpio_value(l_ezlopi_item_t *item, int value);

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
ezlopi_error_t DEVICE_0002_digitalOut_relay(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    ezlopi_error_t error = 0;

    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        error = __prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        error = __init(item);
        break;
    }
    case EZLOPI_ACTION_SET_VALUE:
    {
        error = __set_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        error = __get_value_cjson(item, arg);
        break;
    }

    default:
    {
        break;
    }
    }

    return error;
}

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/
static void __setup_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cjson_device)
{
    device->cloud_properties.category = category_switch;
    device->cloud_properties.subcategory = subcategory_relay;
    device->cloud_properties.device_type = dev_type_switch_inwall;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __setup_item_properties(l_ezlopi_item_t *item, cJSON *cjson_device)
{
    int tmp_var = 0;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.item_name = ezlopi_item_name_switch;
    item->cloud_properties.value_type = value_type_bool;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.item_id = EZPI_core_cloud_generate_item_id();

    CJSON_GET_VALUE_INT(cjson_device, ezlopi_dev_type_str, item->interface_type);

    CJSON_GET_VALUE_BOOL(cjson_device, ezlopi_is_ip_str, item->interface.gpio.gpio_in.enable);

    if (item->interface.gpio.gpio_in.enable)
    {
        CJSON_GET_VALUE_GPIO(cjson_device, ezlopi_gpio_in_str, item->interface.gpio.gpio_in.gpio_num);
        CJSON_GET_VALUE_INT(cjson_device, ezlopi_ip_inv_str, item->interface.gpio.gpio_in.invert);
        CJSON_GET_VALUE_INT(cjson_device, ezlopi_val_ip_str, item->interface.gpio.gpio_in.value);
        CJSON_GET_VALUE_INT(cjson_device, ezlopi_pullup_ip_str, tmp_var);
        item->interface.gpio.gpio_in.pull = tmp_var ? GPIO_PULLUP_ONLY : GPIO_PULLDOWN_ONLY;
        item->interface.gpio.gpio_in.interrupt = GPIO_INTR_DISABLE;
    }
    // else
    // {
    //     item->interface.gpio.gpio_in.interrupt = GPIO_INTR_DISABLE;
    // }

    item->interface.gpio.gpio_out.enable = true;
    CJSON_GET_VALUE_GPIO(cjson_device, ezlopi_gpio_out_str, item->interface.gpio.gpio_out.gpio_num);

    CJSON_GET_VALUE_INT(cjson_device, ezlopi_op_inv_str, item->interface.gpio.gpio_out.invert);
    CJSON_GET_VALUE_INT(cjson_device, ezlopi_val_op_str, item->interface.gpio.gpio_out.value);
    CJSON_GET_VALUE_INT(cjson_device, ezlopi_pullup_op_str, tmp_var);
    item->interface.gpio.gpio_out.interrupt = GPIO_INTR_DISABLE;
    item->interface.gpio.gpio_out.pull = tmp_var ? GPIO_PULLUP_ONLY : GPIO_PULLDOWN_ONLY;
}

static ezlopi_error_t __prepare(void *arg)
{
    ezlopi_error_t error = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (arg)
    {
        cJSON *cjson_device = prep_arg->cjson_device;
        if (cjson_device)
        {
            l_ezlopi_device_t *device = EZPI_core_device_add_device(cjson_device, NULL);
            if (device)
            {
                __setup_device_cloud_properties(device, cjson_device);
                l_ezlopi_item_t *item = EZPI_core_device_add_item_to_device(device, DEVICE_0002_digitalOut_relay);
                if (item)
                {
                    __setup_item_properties(item, cjson_device);
                    error = EZPI_SUCCESS;
                }
                else
                {
                    EZPI_core_device_free_device(device);
                }
            }
        }
    }

    return error;
}

static ezlopi_error_t __init(l_ezlopi_item_t *item)
{
    ezlopi_error_t error = EZPI_ERR_INIT_DEVICE_FAILED;
    if (item)
    {
        if (GPIO_IS_VALID_GPIO(item->interface.gpio.gpio_out.gpio_num) &&
            (255 != item->interface.gpio.gpio_out.gpio_num))
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
            if (0 == gpio_config(&io_conf))
            {
                // digital_io_write_gpio_value(item);
                __write_gpio_value(item);
                error = EZPI_SUCCESS;
            }
        }

        if (item->interface.gpio.gpio_in.enable)
        {
            if (GPIO_IS_VALID_GPIO(item->interface.gpio.gpio_in.gpio_num) &&
                (-1 != item->interface.gpio.gpio_in.gpio_num) &&
                (255 != item->interface.gpio.gpio_in.gpio_num))
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
                if (0 == gpio_config(&io_conf))
                {
                    EZPI_service_gpioisr_register_v3(item, __interrupt_upcall, 1000);
                    error = EZPI_ERR_INIT_DEVICE_FAILED;
                }
            }
        }
    }
    else
    {
        TRACE_E("Error argument!");
    }

    return error;
}

static ezlopi_error_t __get_value_cjson(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t error = EZPI_FAILED;
    if (item && arg)
    {
        cJSON *cj_propertise = (cJSON *)arg;
        if (cj_propertise)
        {
            EZPI_core_valueformatter_bool_to_cjson(cj_propertise, item->interface.gpio.gpio_out.value, NULL);
            error = EZPI_SUCCESS;
        }
    }
    return error;
}

static void __set_gpio_value(l_ezlopi_item_t *item, int value)
{
    gpio_set_level(item->interface.gpio.gpio_out.gpio_num, value);
    item->interface.gpio.gpio_out.value = value;
}

static ezlopi_error_t __set_value(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t error = EZPI_FAILED;
    if (item && arg)
    {
        cJSON *cjson_params = (cJSON *)arg;

        if (NULL != cjson_params)
        {
            int value = 0;
            cJSON *cj_value = cJSON_GetObjectItem(__FUNCTION__, cjson_params, ezlopi_value_str);
            if (cj_value)
            {
                switch (cj_value->type)
                {
                case cJSON_False:
                    value = 0;
                    break;
                case cJSON_True:
                    value = 1;
                    break;
                case cJSON_Number:
                    value = cj_value->valuedouble;
                    break;

                default:
                    break;
                }
            }

            TRACE_S("item_name: %s", item->cloud_properties.item_name);
            TRACE_S("gpio_num: %d", item->interface.gpio.gpio_out.gpio_num);
            TRACE_S("item_id: 0x%08x", item->cloud_properties.item_id);
            TRACE_S("prev value: %d", item->interface.gpio.gpio_out.value);
            TRACE_S("cur value: %d", value);

            if (255 != item->interface.gpio.gpio_out.gpio_num)
            {
                if (GPIO_IS_VALID_GPIO(item->interface.gpio.gpio_out.gpio_num))
                {
                    __set_gpio_value(item, value);
                    EZPI_core_device_value_updated_from_device_broadcast(item);
                    error = EZPI_SUCCESS;
                }
            }
            else
            {
                // in case of master switch
                l_ezlopi_device_t *curr_device = EZPI_core_device_get_head();
                while (curr_device)
                {
                    l_ezlopi_item_t *curr_item = curr_device->items;
                    while (curr_item)
                    {
                        if ((EZLOPI_DEVICE_INTERFACE_DIGITAL_OUTPUT == curr_item->interface_type) && (255 != curr_item->interface.gpio.gpio_out.gpio_num))
                        {
                            TRACE_D("GPIO-pin: %d", curr_item->interface.gpio.gpio_out.gpio_num);
                            TRACE_D("value: %d", value);
                            __set_gpio_value(curr_item, value);
                            EZPI_core_device_value_updated_from_device_broadcast(curr_item);
                            error = EZPI_SUCCESS;
                        }
                        curr_item = curr_item->next;
                    }
                    curr_device = curr_device->next;
                }

                item->interface.gpio.gpio_out.value = value;
                EZPI_core_device_value_updated_from_device_broadcast(item);
            }
        }
    }
    return error;
}

static void __write_gpio_value(l_ezlopi_item_t *item)
{
    uint32_t write_value = (0 == item->interface.gpio.gpio_out.invert) ? item->interface.gpio.gpio_out.value : (item->interface.gpio.gpio_out.value ? 0 : 1);
    gpio_set_level(item->interface.gpio.gpio_out.gpio_num, write_value);
}

static void __interrupt_upcall(void *arg)
{
    l_ezlopi_item_t *item = (l_ezlopi_item_t *)arg;
    if (item)
    {
        TRACE_D("%d -> Got interrupt!", xTaskGetTickCount());
        __toggle_gpio(item);
        EZPI_core_device_value_updated_from_device_broadcast(item);
    }
}

static void __toggle_gpio(l_ezlopi_item_t *item)
{
    uint32_t write_value = !(item->interface.gpio.gpio_out.value);
    gpio_set_level(item->interface.gpio.gpio_out.gpio_num, write_value);
    item->interface.gpio.gpio_out.value = write_value;
}

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
