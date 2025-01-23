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
 * @file    sensor_0044_I2C_TSL256_luminosity.c
 * @brief   perform some function on sensor_0044
 * @author  xx
 * @version 0.1
 * @date    xx
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <math.h>

#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_hal_i2c_master.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "sensor_0044_I2C_TSL256_luminosity.h"
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
static ezlopi_error_t __get_cjson_value(l_ezlopi_item_t *item, void *arg);
static ezlopi_error_t __notify(l_ezlopi_item_t *item);
static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_data);

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
ezlopi_error_t SENSOR_0044_i2c_tsl256_luminosity(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    ezlopi_error_t ret = EZPI_SUCCESS;
    // printf("\n\nHERE : %s\n\n", EZPI_core_actions_to_string(action));
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
static ezlopi_error_t __notify(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_FAILED;

    // Allow only significant changes in values to be posted
    double new_value = (double)TSL2561_get_intensity_value(&item->interface.i2c_master);
    TSL256_lum_t *TSL2561_lux_data = (TSL256_lum_t *)item->user_arg;
    if (TSL2561_lux_data)
    {
        if (fabs((double)(TSL2561_lux_data->lux_val) - new_value) > 0.0001)
        {
            EZPI_core_device_value_updated_from_device_broadcast(item);
            TSL2561_lux_data->lux_val = (uint32_t)new_value;
            ret = EZPI_SUCCESS;
        }
    }

    return ret;
}
static ezlopi_error_t __get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_FAILED;
    cJSON *cj_result = (cJSON *)arg;
    if (cj_result)
    {
        TSL256_lum_t *illuminance_value = ((TSL256_lum_t *)item->user_arg);
        if (illuminance_value)
        {
            EZPI_core_valueformatter_uint32_to_cjson(cj_result, illuminance_value->lux_val, scales_lux);
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
        TSL256_lum_t *TSL2561_lux_data = (TSL256_lum_t *)item->user_arg;
        if (TSL2561_lux_data)
        {
            if (item->interface.i2c_master.enable)
            {
                if (EZPI_SUCCESS == EZPI_hal_i2c_master_init(&item->interface.i2c_master))
                {
                    TRACE_I("I2C channel is %d", item->interface.i2c_master.channel);
                    if (TSL2561_check_partid(&item->interface.i2c_master))
                    {
                        TRACE_I("TSL561 initialization finished.........");
                        SENSOR_0044_tsl2561_configure_device(&item->interface.i2c_master);
                        ret = EZPI_SUCCESS;
                    }
                }
                else
                {
                    TRACE_E("I2C init failed");
                }
            }
        }
    }
    return ret;
}
static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    device->cloud_properties.category = category_light_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}
static void __prepare_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_data)
{
    CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_type_str, item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_lux;
    item->cloud_properties.value_type = value_type_illuminance;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = scales_lux;
    item->cloud_properties.item_id = EZPI_core_cloud_generate_item_id();

    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_sda_str, item->interface.i2c_master.sda);
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_scl_str, item->interface.i2c_master.scl);
    CJSON_GET_VALUE_UINT32(cj_device, ezlopi_slave_addr_str, item->interface.i2c_master.address);

    item->interface.i2c_master.enable = true;
    item->interface.i2c_master.channel = I2C_NUM_0;
    item->interface.i2c_master.clock_speed = 100000;
    if (0x00 == item->interface.i2c_master.address)
    {
        item->interface.i2c_master.address = TSL2561_ADDRESS;
    }
    // passing the custom data_structure
    item->is_user_arg_unique = true;
    item->user_arg = user_data;
}
static ezlopi_error_t __prepare(void *arg)
{
    ezlopi_error_t ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;

    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (prep_arg && prep_arg->cjson_device)
    {
        TSL256_lum_t *TSL2561_lux_data = (TSL256_lum_t *)ezlopi_malloc(__FUNCTION__, sizeof(TSL256_lum_t));
        if (TSL2561_lux_data)
        {
            memset(TSL2561_lux_data, 0, sizeof(TSL256_lum_t));
            l_ezlopi_device_t *tsl256_device = EZPI_core_device_add_device(prep_arg->cjson_device, NULL);
            if (tsl256_device)
            {
                __prepare_device_cloud_properties(tsl256_device, prep_arg->cjson_device);

                l_ezlopi_item_t *tsl256_item = EZPI_core_device_add_item_to_device(tsl256_device, SENSOR_0044_i2c_tsl256_luminosity);
                if (tsl256_item)
                {
                    __prepare_item_cloud_properties(tsl256_item, prep_arg->cjson_device, TSL2561_lux_data);
                    ret = EZPI_SUCCESS;
                }
                else
                {
                    EZPI_core_device_free_device(tsl256_device);
                    ezlopi_free(__FUNCTION__, TSL2561_lux_data);
                }
            }
            else
            {
                ezlopi_free(__FUNCTION__, TSL2561_lux_data);
            }
        }
    }

    return ret;
}

/*******************************************************************************
 *                          End of File
 *******************************************************************************/