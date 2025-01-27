/* ===========================================================================
** Copyright (C) 2025 Ezlo Innovation Inc
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
 * @file    sensor_0023_digitalIn_touch_switch_TTP223B.c
 * @brief   perform some function on sensor_0023
 * @author
 * @version 0.1
 * @date    xx
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "driver/gpio.h"

#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "ezlopi_service_gpioisr.h"

#include "sensor_0023_digitalIn_touch_switch_TTP223B.h"

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
static void __prepare_touch_switch_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_touch_switch_properties(l_ezlopi_item_t *item, cJSON *cj_device);
static ezlopi_error_t __init(l_ezlopi_item_t *item);
static void touch_switch_callback(void *arg);
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
/**
 * @brief Public function to interface bme280. This is used to handles all the action on the bme280 sensor and is the entry point to interface the sensor.
 *
 * @param action e_ezlopi_actions_t
 * @param arg Other arguments if needed
 * @return ezlopi_error_t
 */
ezlopi_error_t SENSOR_0023_digitalIn_touch_switch_TTP223B(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
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

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/

static ezlopi_error_t __get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item)
    {
        cJSON *cj_result = (cJSON *)arg;
        if (cj_result)
        {
            EZPI_core_valueformatter_bool_to_cjson(cj_result, item->interface.gpio.gpio_in.value, NULL);
            ret = EZPI_SUCCESS;
        }
    }

    return ret;
}

static ezlopi_error_t __init(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_ERR_INIT_DEVICE_FAILED;
    if (item)
    {

        if (GPIO_IS_VALID_GPIO(item->interface.gpio.gpio_in.gpio_num))
        {
            const gpio_config_t touch_switch_config = {
                .pin_bit_mask = (1ULL << item->interface.gpio.gpio_in.gpio_num),
                .mode = GPIO_MODE_INPUT,
                .pull_up_en = GPIO_PULLUP_DISABLE,
                .pull_down_en = (item->interface.gpio.gpio_in.pull == GPIO_PULLDOWN_ONLY) ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE,
                .intr_type = GPIO_INTR_POSEDGE,
            };

            if (0 == gpio_config(&touch_switch_config)) // ESP_OK
            {
                EZPI_service_gpioisr_register_v3(item, touch_switch_callback, 200);
                ret = EZPI_SUCCESS;
            }
        }
        else
        {
            ret = -1;
        }
    }

    return ret;
}

static void touch_switch_callback(void *arg)
{
    l_ezlopi_item_t *item = (l_ezlopi_item_t *)arg;
    item->interface.gpio.gpio_in.value = !item->interface.gpio.gpio_in.value;
    EZPI_core_device_value_updated_from_device_broadcast(item);
}

static ezlopi_error_t __prepare(void *arg)
{
    ezlopi_error_t ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;

    if (prep_arg && prep_arg->cjson_device)
    {
        l_ezlopi_device_t *touch_device = EZPI_core_device_add_device(prep_arg->cjson_device, NULL);
        if (touch_device)
        {
            __prepare_touch_switch_device_cloud_properties(touch_device, prep_arg->cjson_device);
            l_ezlopi_item_t *touch_switch_item = EZPI_core_device_add_item_to_device(touch_device, SENSOR_0023_digitalIn_touch_switch_TTP223B);
            if (touch_switch_item)
            {
                __prepare_touch_switch_properties(touch_switch_item, prep_arg->cjson_device);
                ret = EZPI_SUCCESS;
            }
            else
            {
                EZPI_core_device_free_device(touch_device);
            }
        }
    }

    return ret;
}

static void __prepare_touch_switch_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    device->cloud_properties.category = category_switch;
    device->cloud_properties.subcategory = subcategory_in_wall;
    device->cloud_properties.device_type = dev_type_switch_outlet;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_touch_switch_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    int val_ip = 0;

    CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_type_str, item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_switch;
    item->cloud_properties.value_type = value_type_bool;
    item->cloud_properties.item_id = EZPI_core_cloud_generate_item_id();
    item->cloud_properties.scale = NULL;

    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_str, item->interface.gpio.gpio_in.gpio_num);
    CJSON_GET_VALUE_INT(cj_device, ezlopi_ip_inv_str, item->interface.gpio.gpio_in.invert);
    CJSON_GET_VALUE_INT(cj_device, ezlopi_val_ip_str, val_ip);

    item->interface.gpio.gpio_in.value = (true == item->interface.gpio.gpio_in.invert) ? !val_ip : val_ip;
    item->interface.gpio.gpio_in.enable = true;
    item->interface.gpio.gpio_in.interrupt = GPIO_INTR_POSEDGE;
    item->interface.gpio.gpio_in.pull = GPIO_PULLDOWN_ONLY;
}

/*******************************************************************************
 *                          End of File
 *******************************************************************************/