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
* @file    sensor_0008_I2C_LTR303ALS.c
* @brief   perform some function on sensor_0008
* @author  xx
* @version 0.1
* @date    xx
*/

/*******************************************************************************
*                          Include Files
*******************************************************************************/

#include "../../build/config/sdkconfig.h"
#include <math.h>
#include "ezlopi_util_trace.h"

// #include "ezlopi_core_timer.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"
#include "ezlopi_core_errors.h"

#include "ezlopi_hal_i2c_master.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "ALS_LTR303.h"
#include "sensor_0008_I2C_LTR303ALS.h"
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
static ezlopi_error_t __get_value_cjson(l_ezlopi_item_t *item, void *arg);

/*******************************************************************************
*                          Static Data Definitions
*******************************************************************************/

/*******************************************************************************
*                          Extern Data Definitions
*******************************************************************************/

/*******************************************************************************
*                          Extern Function Definitions
*******************************************************************************/

ezlopi_error_t SENSOR_0008_I2C_LTR303ALS(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
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
    if (item)
    {
        cJSON *cj_param = (cJSON *)arg;
        ltr303_data_t *als_ltr303_data = (ltr303_data_t *)item->user_arg;
        if (cj_param && als_ltr303_data)
        {
            EZPI_core_valueformatter_double_to_cjson(cj_param, als_ltr303_data->lux, scales_lux);
            ret = EZPI_SUCCESS;
        }
    }
    return ret;
}

static ezlopi_error_t __notify(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item)
    {
        ltr303_data_t *als_ltr303_data = (ltr303_data_t *)item->user_arg;
        if (als_ltr303_data)
        {
            ltr303_data_t temp_data;
            if (ESP_OK == ltr303_get_val(&temp_data))
            {
                if (fabs(als_ltr303_data->lux - temp_data.lux) > 0.2)
                {
                    als_ltr303_data->lux = temp_data.lux;
                    EZPI_core_device_value_updated_from_device_broadcast(item);
                    ret = EZPI_SUCCESS;
                }
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
        ltr303_data_t *als_ltr303_data = (ltr303_data_t *)item->user_arg;
        if (als_ltr303_data)
        {
            if (item->interface.i2c_master.enable)
            {
                if (ESP_OK == ltr303_setup(item->interface.i2c_master.sda, item->interface.i2c_master.scl, true))
                {
                    ltr303_get_val(als_ltr303_data);
                    ret = EZPI_SUCCESS;
                }
            }
        }
    }
    return ret;
}

static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_params)
{
    device->cloud_properties.category = category_light_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_item_properties(l_ezlopi_item_t *item, cJSON *cj_param)
{
    CJSON_GET_VALUE_DOUBLE(cj_param, ezlopi_dev_type_str, item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_id = EZPI_core_cloud_generate_item_id();
    item->cloud_properties.item_name = ezlopi_item_name_lux;
    item->cloud_properties.value_type = value_type_illuminance;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = scales_lux;

    CJSON_GET_VALUE_GPIO(cj_param, ezlopi_gpio_sda_str, item->interface.i2c_master.sda);
    CJSON_GET_VALUE_GPIO(cj_param, ezlopi_gpio_scl_str, item->interface.i2c_master.scl);

    item->interface.i2c_master.enable = true;
    item->interface.i2c_master.clock_speed = 100000;
    item->interface.i2c_master.address = LTR303_ADDR;
    item->interface.i2c_master.channel = I2C_NUM_0;
    ltr303_data_t *als_ltr303_data = (ltr303_data_t *)ezlopi_malloc(__FUNCTION__, sizeof(ltr303_data_t));
    if (als_ltr303_data)
    {
        memset(als_ltr303_data, 0, sizeof(ltr303_data_t));
        item->user_arg = (void *)als_ltr303_data;
    }
}

static ezlopi_error_t __prepare(void *arg)
{
    ezlopi_error_t ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;

    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (prep_arg && prep_arg->cjson_device)
    {
        l_ezlopi_device_t *als_ltr303_device = EZPI_core_device_add_device(prep_arg->cjson_device, NULL);
        if (als_ltr303_device)
        {
            __prepare_device_cloud_properties(als_ltr303_device, prep_arg->cjson_device);
            l_ezlopi_item_t *als_ltr303_item = EZPI_core_device_add_item_to_device(als_ltr303_device, sensor_0008_I2C_LTR303ALS);
            if (als_ltr303_item)
            {
                __prepare_item_properties(als_ltr303_item, prep_arg->cjson_device);
                ret = EZPI_SUCCESS;
            }
            else
            {
                EZPI_core_device_free_device(als_ltr303_device);
            }
        }
    }

    return ret;
}
/*******************************************************************************
*                          End of File
*******************************************************************************/