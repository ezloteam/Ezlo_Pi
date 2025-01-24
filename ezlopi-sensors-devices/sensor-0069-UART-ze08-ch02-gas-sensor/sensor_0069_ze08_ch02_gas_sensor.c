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
 * @file    sensor_0069_ze08_ch02_gas_sensor.c
 * @brief   perform some function on sensor_0069
 * @author  xx
 * @version 0.1
 * @date    xx
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <stdlib.h>
#include <string.h>

#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_cloud_constants.h"

#include "ze08_ch2o.h"
#include "sensor_0069_ze08_ch02_gas_sensor.h"
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

/*
{\
    \"dev_type\": 6,\
    \"dev_name\": \"ZE08CH02 gas sensor\",\
    \"id_room\": 35,\
    \"id_item\": 69,\
    \"gpio_tx\": 48,\
    \"gpio_rx\": 45,\
    \"baud\": 9600\
}\
*/
/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/

static ezlopi_error_t __prepare(void *arg, void *user_arg);
static ezlopi_error_t __init(l_ezlopi_item_t *item);
static ezlopi_error_t __get_cjson_value(l_ezlopi_item_t *item, void *arg);
static ezlopi_error_t __notify(l_ezlopi_item_t *item);

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/

ezlopi_error_t SENSOR_0069_ze08_ch02_gas_sensor(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    ezlopi_error_t ret = EZPI_SUCCESS;
    // TRACE_D("%s", EZPI_core_actions_to_string(action));
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = __prepare(arg, user_arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = __init(item);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = __get_cjson_value(item, arg);
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
static ezlopi_error_t __get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_FAILED;

    cJSON *cj_params = (cJSON *)arg;
    ze08_ch2o_sensor_data_t *ze08_ch2o_sensor = (ze08_ch2o_sensor_data_t *)item->user_arg;

    if (ze08_ch2o_sensor && cj_params)
    {
        EZPI_core_valueformatter_float_to_cjson(cj_params, ze08_ch2o_sensor->ppm, scales_parts_per_million);
        ret = EZPI_SUCCESS;
    }
    return ret;
}

static ezlopi_error_t __notify(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_FAILED;

    ze08_ch2o_sensor_data_t *ze08_ch2o_sensor = (ze08_ch2o_sensor_data_t *)item->user_arg;
    if (ze08_ch2o_sensor && ze08_ch2o_sensor->available)
    {
        EZPI_core_device_value_updated_from_device_broadcast(item);
        ze08_ch2o_sensor->available = false;
        ret = EZPI_SUCCESS;
    }
    return ret;
}

static ezlopi_error_t __init(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_ERR_INIT_DEVICE_FAILED;
    ze08_ch2o_sensor_data_t *ze08_ch2o_sensor = (ze08_ch2o_sensor_data_t *)item->user_arg;
    if (ze08_ch2o_sensor)
    {
        if (item->interface.uart.enable)
        {
            if (false != ZE08_ch2o_sensor_init(&item->interface.uart, ze08_ch2o_sensor))
            {
                ret = EZPI_SUCCESS;
            }
        }
    }
    return ret;
}

static void __prepare_ze08_ch2o_sensor_device_cloud_properties(l_ezlopi_device_t *device)
{
    device->cloud_properties.category = category_level_sensor;
    device->cloud_properties.subcategory = subcategory_particulate_matter;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.device_type_id = NULL;
    device->cloud_properties.info = NULL;
}

static void __prepare_ze08_ch2o_sensor_ppm_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg)
{
    item->interface_type = EZLOPI_DEVICE_INTERFACE_UART;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_id = EZPI_core_cloud_generate_item_id();
    item->cloud_properties.item_name = ezlopi_item_name_particulate_matter_1;
    item->cloud_properties.value_type = value_type_substance_amount;
    item->cloud_properties.scale = scales_parts_per_million;

    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_tx_str, item->interface.uart.tx);
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_rx_str, item->interface.uart.rx);
    CJSON_GET_VALUE_UINT32(cj_device, ezlopi_baud_str, item->interface.uart.baudrate);
    item->interface.uart.enable = true;
    item->user_arg = user_arg;
}

static ezlopi_error_t __prepare(void *arg, void *user_arg)
{
    ezlopi_error_t ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;

    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (prep_arg)
    {
        ze08_ch2o_sensor_data_t *ze08_ch2o_sensor = (ze08_ch2o_sensor_data_t *)ezlopi_malloc(__FUNCTION__, sizeof(ze08_ch2o_sensor_data_t));
        if (ze08_ch2o_sensor)
        {
            memset(ze08_ch2o_sensor, 0, sizeof(ze08_ch2o_sensor_data_t));
            l_ezlopi_device_t *ze08_ch2o_sensor_device = EZPI_core_device_add_device(prep_arg->cjson_device, NULL);
            if (ze08_ch2o_sensor_device)
            {
                memset(ze08_ch2o_sensor_device, 0, sizeof(l_ezlopi_device_t));
                __prepare_ze08_ch2o_sensor_device_cloud_properties(ze08_ch2o_sensor_device);
                l_ezlopi_item_t *ze08_ch02_ppm_item = EZPI_core_device_add_item_to_device(ze08_ch2o_sensor_device, SENSOR_0069_ze08_ch02_gas_sensor);
                if (ze08_ch02_ppm_item)
                {
                    __prepare_ze08_ch2o_sensor_ppm_item_cloud_properties(ze08_ch02_ppm_item, prep_arg->cjson_device, ze08_ch2o_sensor);
                    ret = EZPI_SUCCESS;
                }
                else
                {
                    EZPI_core_device_free_device(ze08_ch2o_sensor_device);
                    ezlopi_free(__FUNCTION__, ze08_ch2o_sensor);
                }
            }
            else
            {
                ezlopi_free(__FUNCTION__, ze08_ch2o_sensor);
            }
        }
    }

    return ret;
}

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
