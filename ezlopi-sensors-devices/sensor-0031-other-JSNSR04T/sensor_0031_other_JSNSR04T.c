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
 * @file    sensor_0031_other_JSNSR04T.c
 * @brief   perform some function on sensor_0031
 * @author  xx
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

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "jsn_sr04t.h"
#include "sensor_0031_other_JSNSR04T.h"
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

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
static ezlopi_error_t __prepare(void *arg);
static ezlopi_error_t __init(l_ezlopi_item_t *item);
static ezlopi_error_t __notify(l_ezlopi_item_t *item);
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
ezlopi_error_t SENSOR_0031_other_jsnsr04t(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
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
        __get_cjson_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        __notify(item);
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
static ezlopi_error_t __notify(l_ezlopi_item_t *item)
{
    return EZPI_core_device_value_updated_from_device_broadcast(item);
}

static ezlopi_error_t __get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        jsn_sr04t_config_t *tmp_config = (jsn_sr04t_config_t *)item->user_arg;
        if (tmp_config)
        {
            jsn_sr04t_data_t jsn_sr04t_data;
            ret = JSN_sr04t_measurement(tmp_config, &jsn_sr04t_data);
            if (ret)
            {
                float distance = (jsn_sr04t_data.distance_cm / 100.0f);
                EZPI_core_valueformatter_float_to_cjson(cj_result, distance, scales_meter);
                ret = EZPI_SUCCESS;
            }
            else
            {
                ESP_LOGE("JSN_SR04T_V3", "ERROR in getting measurement: ret=%d", ret);
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
        jsn_sr04t_config_t *jsn_sr04t_config = ezlopi_malloc(__FUNCTION__, sizeof(jsn_sr04t_config_t));
        if (jsn_sr04t_config)
        {
            jsn_sr04t_config_t tmp_config = (jsn_sr04t_config_t)JSN_SR04T_CONFIG_DEFAULT();
            tmp_config.trigger_gpio_num = item->interface.gpio.gpio_out.gpio_num;
            tmp_config.echo_gpio_num = item->interface.gpio.gpio_in.gpio_num;
            tmp_config.rmt_channel = 4;

            memcpy(jsn_sr04t_config, &tmp_config, sizeof(jsn_sr04t_config_t));
            item->user_arg = (void *)jsn_sr04t_config;

            if (ESP_OK == JSN_sr04t_init(jsn_sr04t_config))
            {
                TRACE_S("JSN_SR04T initialized");
                ret = EZPI_SUCCESS;
            }
        }
    }

    return ret;
}

static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    device->cloud_properties.category = category_level_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_type_str, item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_distance;
    item->cloud_properties.value_type = value_type_length;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.show = true;
    item->cloud_properties.item_id = EZPI_core_cloud_generate_item_id();
}

static void __prepare_item_interface_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    item->interface_type = EZLOPI_DEVICE_INTERFACE_DIGITAL_OUTPUT;

    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio1_str, item->interface.gpio.gpio_out.gpio_num);
    item->interface.gpio.gpio_out.enable = true;
    item->interface.gpio.gpio_out.interrupt = GPIO_INTR_DISABLE;
    item->interface.gpio.gpio_out.invert = EZLOPI_GPIO_LOGIC_NONINVERTED;
    item->interface.gpio.gpio_out.mode = GPIO_MODE_OUTPUT;
    item->interface.gpio.gpio_out.pull = GPIO_PULLDOWN_ONLY;
    item->interface.gpio.gpio_out.value = 0;

    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio2_str, item->interface.gpio.gpio_in.gpio_num);
    item->interface.gpio.gpio_in.enable = true;
    item->interface.gpio.gpio_in.interrupt = GPIO_INTR_DISABLE;
    item->interface.gpio.gpio_in.invert = EZLOPI_GPIO_LOGIC_NONINVERTED;
    item->interface.gpio.gpio_in.mode = GPIO_MODE_INPUT;
    item->interface.gpio.gpio_in.pull = GPIO_PULLDOWN_DISABLE;
    item->interface.gpio.gpio_in.value = 0;
}

static ezlopi_error_t __prepare(void *arg)
{
    ezlopi_error_t ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;

    if (prep_arg && prep_arg->cjson_device)
    {
        l_ezlopi_device_t *device = EZPI_core_device_add_device(prep_arg->cjson_device, NULL);
        if (device)
        {
            __prepare_device_cloud_properties(device, prep_arg->cjson_device);
            l_ezlopi_item_t *item_temperature = EZPI_core_device_add_item_to_device(device, SENSOR_0031_other_jsnsr04t);
            if (item_temperature)
            {
                __prepare_item_cloud_properties(item_temperature, prep_arg->cjson_device);
                __prepare_item_interface_properties(item_temperature, prep_arg->cjson_device);
                ret = EZPI_SUCCESS;
            }
            else
            {
                EZPI_core_device_free_device(device);
            }
        }
    }
    return ret;
}

/*******************************************************************************
 *                          End of File
 *******************************************************************************/