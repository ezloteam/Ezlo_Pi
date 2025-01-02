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
* @file    sensor_0021_UART_MB1013.c
* @brief   perform some function on sensor_0021
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

#include "ezlopi_hal_uart.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "sensor_0021_UART_MB1013.h"
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
typedef struct s_mb1013_args
{
    float current_value;
    float previous_value;
} s_mb1013_args_t;

/*******************************************************************************
*                          Static Function Prototypes
*******************************************************************************/

/*******************************************************************************
*                          Static Data Definitions
*******************************************************************************/
static ezlopi_error_t __prepare(void *arg);
static ezlopi_error_t __init(l_ezlopi_item_t *item);
static ezlopi_error_t __notify(l_ezlopi_item_t *item);
static ezlopi_error_t __get_value_cjson(l_ezlopi_item_t *item, void *arg);

/*******************************************************************************
*                          Extern Data Definitions
*******************************************************************************/

/*******************************************************************************
*                          Extern Function Definitions
*******************************************************************************/

ezlopi_error_t SENSOR_0021_uart_mb1013(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
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

/*******************************************************************************
*                         Static Function Definitions
*******************************************************************************/

static ezlopi_error_t __get_value_cjson(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item && arg)
    {
        s_mb1013_args_t *mb1013_args = item->user_arg;
        if (mb1013_args)
        {
            cJSON *cj_result = (cJSON *)arg;
            EZPI_core_valueformatter_float_to_cjson(cj_result, mb1013_args->current_value, scales_meter);
            ret = EZPI_SUCCESS;
        }
    }
    return ret;
}

static void __uart_data_upcall(uint8_t *buffer, uint32_t output_len, s_ezlopi_uart_object_handle_t uart_object_handle)
{
    if (buffer && output_len && uart_object_handle)
    {
        l_ezlopi_item_t *item = (l_ezlopi_item_t *)uart_object_handle->arg;
        if (item)
        {
            s_mb1013_args_t *s_mb1013_args = (s_mb1013_args_t *)item->user_arg;
            if (s_mb1013_args)
            {
                int idx = 0;
                TRACE_D("BUFFER-DATA-LEN: %d", output_len);
                while (idx < output_len)
                {
                    // dump("rx-buffer", buffer, idx, 6);
                    if ('R' == buffer[idx] && '\r' == buffer[idx + 5])
                    {
                        s_mb1013_args->current_value = atoi((const char *)&buffer[idx + 1]) / 10.0;
                        TRACE_D("range: %f", s_mb1013_args->current_value);
                        break;
                    }
                    idx++;
                }
            }
        }
    }
}

static ezlopi_error_t __init(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_ERR_INIT_DEVICE_FAILED;
    if (item)
    {
        s_mb1013_args_t *mb1013_args = (s_mb1013_args_t *)item->user_arg;
        if (mb1013_args)
        {
            if (GPIO_IS_VALID_GPIO(item->interface.uart.tx) && GPIO_IS_VALID_GPIO(item->interface.uart.rx))
            {
                s_ezlopi_uart_object_handle_t ezlopi_uart_object_handle = EZPI_hal_uart_init(item->interface.uart.baudrate, item->interface.uart.tx, item->interface.uart.rx, __uart_data_upcall, item);
                item->interface.uart.channel = EZPI_hal_uart_get_channel(ezlopi_uart_object_handle);
                ret = EZPI_SUCCESS;
            }
        }
    }
    return ret;
}

static void __setup_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    device->cloud_properties.category = category_level_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __setup_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    item->cloud_properties.show = true;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.item_name = ezlopi_item_name_distance;
    item->cloud_properties.value_type = value_type_length;
    item->cloud_properties.scale = scales_centi_meter;
    item->cloud_properties.item_id = EZPI_core_cloud_generate_item_id();
}

static void __setup_item_interface_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    item->interface_type = EZLOPI_DEVICE_INTERFACE_UART;
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_baud_str, item->interface.uart.baudrate);
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_tx_str, item->interface.uart.tx);
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_rx_str, item->interface.uart.rx);
}

static ezlopi_error_t __prepare(void *arg)
{
    ezlopi_error_t ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (arg)
    {
        cJSON *cjson_device = prep_arg->cjson_device;
        if (cjson_device)
        {
            l_ezlopi_device_t *device = EZPI_core_device_add_device(prep_arg->cjson_device, NULL);
            if (device)
            {
                __setup_device_cloud_properties(device, cjson_device);
                l_ezlopi_item_t *item = EZPI_core_device_add_item_to_device(device, SENSOR_0021_uart_mb1013);
                if (item)
                {
                    __setup_item_cloud_properties(item, cjson_device);
                    __setup_item_interface_properties(item, cjson_device);

                    s_mb1013_args_t *mb1030_args = ezlopi_malloc(__FUNCTION__, sizeof(s_mb1013_args_t));
                    if (mb1030_args)
                    {
                        mb1030_args->current_value = 0.0;
                        mb1030_args->previous_value = 0.0;
                        item->is_user_arg_unique = true;
                        item->user_arg = mb1030_args;
                        ret = EZPI_SUCCESS;
                    }
                }
                else
                {
                    EZPI_core_device_free_device(device);
                }
            }
        }
    }

    return ret;
}

static ezlopi_error_t __notify(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item && item->user_arg)
    {
        s_mb1013_args_t *mb1013_args = (s_mb1013_args_t *)item->user_arg;
        if (mb1013_args)
        {
            if (abs(mb1013_args->current_value - mb1013_args->previous_value) > 0.2) // accuracy of 0.5cm (i.e. 5mm)
            {
                EZPI_core_device_value_updated_from_device_broadcast(item);
                mb1013_args->previous_value = mb1013_args->current_value;
                ret = EZPI_SUCCESS;
            }
        }
    }

    return ret;
}

/*******************************************************************************
*                          End of File
*******************************************************************************/