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
* @file    sensor_0006_I2C_ADXL345.c
* @brief   perform some function on sensor_0006
* @author  xx
* @version 0.1
* @date    xx
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

#include "ezlopi_hal_i2c_master.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "sensor_0006_I2C_ADXL345.h"
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
static void __prepare_item_cloud_properties(l_ezlopi_item_t *item, void *user_data);
static void __prepare_item_interface_properties(l_ezlopi_item_t *item, cJSON *cj_device);

/*******************************************************************************
*                          Static Data Definitions
*******************************************************************************/

/*******************************************************************************
*                          Extern Data Definitions
*******************************************************************************/

/*******************************************************************************
*                          Extern Function Definitions
*******************************************************************************/
ezlopi_error_t SENSOR_0006_I2C_ADXL345(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
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
static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    device->cloud_properties.category = category_level_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
    device->cloud_properties.device_type = dev_type_sensor;
}

static void __prepare_item_cloud_properties(l_ezlopi_item_t *item, void *user_data)
{
    if (item)
    {
        item->cloud_properties.item_id = EZPI_core_cloud_generate_item_id();
        item->cloud_properties.has_getter = true;
        item->cloud_properties.has_setter = false;
        item->cloud_properties.show = true;
        item->cloud_properties.scale = scales_meter_per_square_second;
        item->cloud_properties.value_type = value_type_acceleration;
        item->user_arg = user_data;
        if (ezlopi_item_name_acceleration_x_axis == item->cloud_properties.item_name)
        {
            item->is_user_arg_unique = true;
        }
    }
}
static void __prepare_item_interface_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    if (item && cj_device)
    {
        if (ezlopi_item_name_acceleration_x_axis == item->cloud_properties.item_name)
        {
            item->interface.i2c_master.enable = true;
            item->interface.i2c_master.channel = I2C_NUM_0;
            CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_dev_type_str, item->interface_type);
            CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_sda_str, item->interface.i2c_master.sda);
            CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_scl_str, item->interface.i2c_master.scl);
            CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_slave_addr_str, item->interface.i2c_master.address);
            item->interface.i2c_master.clock_speed = 100000;
            if (0 == item->interface.i2c_master.address)
            {
                item->interface.i2c_master.address = ADXL345_ADDR;
            }
        }
        else
        {
            item->interface.i2c_master.enable = false;
        }
    }
}

static ezlopi_error_t __prepare(void *arg)
{
    ezlopi_error_t ret = EZPI_SUCCESS;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (prep_arg && prep_arg->cjson_device)
    {
        cJSON *cj_device = prep_arg->cjson_device;
        s_adxl345_data_t *user_data = (s_adxl345_data_t *)ezlopi_malloc(__FUNCTION__, sizeof(s_adxl345_data_t));
        if (NULL != user_data)
        {
            memset(user_data, 0, sizeof(s_adxl345_data_t));
            l_ezlopi_device_t *adxl345_parent_x_device = EZPI_core_device_add_device(cj_device, "acc_x");
            if (adxl345_parent_x_device)
            {
                TRACE_I("Parent_adxl345_acc-x-[0x%x] ", adxl345_parent_x_device->cloud_properties.device_id);
                __prepare_device_cloud_properties(adxl345_parent_x_device, cj_device);
                l_ezlopi_item_t *x_item = EZPI_core_device_add_item_to_device(adxl345_parent_x_device, sensor_0006_I2C_ADXL345);
                if (x_item)
                {
                    x_item->cloud_properties.item_name = ezlopi_item_name_acceleration_x_axis;
                    __prepare_item_cloud_properties(x_item, user_data);
                    __prepare_item_interface_properties(x_item, cj_device);
                }

                l_ezlopi_device_t *adxl345_child_y_device = EZPI_core_device_add_device(cj_device, "acc_y");
                if (adxl345_child_y_device)
                {
                    TRACE_I("child_mpu6050_acc-y-[0x%x] ", adxl345_child_y_device->cloud_properties.device_id);
                    __prepare_device_cloud_properties(adxl345_child_y_device, cj_device);

                    l_ezlopi_item_t *y_item = EZPI_core_device_add_item_to_device(adxl345_child_y_device, sensor_0006_I2C_ADXL345);
                    if (y_item)
                    {
                        y_item->cloud_properties.item_name = ezlopi_item_name_acceleration_y_axis;
                        __prepare_item_cloud_properties(y_item, user_data);
                        __prepare_item_interface_properties(y_item, cj_device);
                    }
                    else
                    {
                        ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
                        EZPI_core_device_free_device(adxl345_child_y_device);
                    }
                }

                l_ezlopi_device_t *adxl345_child_z_device = EZPI_core_device_add_device(cj_device, "acc_z");
                if (adxl345_child_z_device)
                {
                    TRACE_I("child_mpu6050_acc-z-[0x%x] ", adxl345_child_z_device->cloud_properties.device_id);
                    __prepare_device_cloud_properties(adxl345_child_z_device, cj_device);

                    l_ezlopi_item_t *z_item = EZPI_core_device_add_item_to_device(adxl345_child_z_device, sensor_0006_I2C_ADXL345);
                    if (z_item)
                    {
                        z_item->cloud_properties.item_name = ezlopi_item_name_acceleration_z_axis;
                        __prepare_item_cloud_properties(z_item, user_data);
                        __prepare_item_interface_properties(z_item, cj_device);
                    }
                    else
                    {
                        ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
                        EZPI_core_device_free_device(adxl345_child_z_device);
                    }
                }

                if ((NULL == x_item) &&
                    (NULL == adxl345_child_y_device) &&
                    (NULL == adxl345_child_z_device))
                {
                    EZPI_core_device_free_device(adxl345_parent_x_device);
                    ezlopi_free(__FUNCTION__, user_data);
                    ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
                }
            }
            else
            {
                ezlopi_free(__FUNCTION__, user_data);
                ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
            }
        }
        else
        {
            ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
        }
    }
    else
    {
        ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
    }

    return ret;
}

static ezlopi_error_t __init(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_SUCCESS;
    if (item)
    {
        s_adxl345_data_t *user_data = (s_adxl345_data_t *)item->user_arg;
        if (user_data)
        {
            if (item->interface.i2c_master.enable)
            {
                EZPI_hal_i2c_master_init(&item->interface.i2c_master);
                if (0 == __adxl345_configure_device(item)) // ESP_OK
                {
                    TRACE_S("Configuration Complete...");
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
    return ret;
}

static ezlopi_error_t __get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        if (cj_result)
        {
            float acceleration_value = 0;
            s_adxl345_data_t *user_data = (s_adxl345_data_t *)item->user_arg;
            if (user_data)
            {
                if (ezlopi_item_name_acceleration_x_axis == item->cloud_properties.item_name)
                {
                    acceleration_value = (user_data->acc_x * ADXL345_CONVERTER_FACTOR_MG_TO_G * ADXL345_STANDARD_G_TO_ACCEL_CONVERSION_VALUE);
                    EZPI_core_valueformatter_float_to_cjson(cj_result, acceleration_value, item->cloud_properties.scale);
                }
                if (ezlopi_item_name_acceleration_y_axis == item->cloud_properties.item_name)
                {
                    acceleration_value = (user_data->acc_y * ADXL345_CONVERTER_FACTOR_MG_TO_G * ADXL345_STANDARD_G_TO_ACCEL_CONVERSION_VALUE);
                    EZPI_core_valueformatter_float_to_cjson(cj_result, acceleration_value, item->cloud_properties.scale);
                }
                if (ezlopi_item_name_acceleration_z_axis == item->cloud_properties.item_name)
                {
                    acceleration_value = (user_data->acc_z * ADXL345_CONVERTER_FACTOR_MG_TO_G * ADXL345_STANDARD_G_TO_ACCEL_CONVERSION_VALUE);
                    EZPI_core_valueformatter_float_to_cjson(cj_result, acceleration_value, item->cloud_properties.scale);
                }
                ret = EZPI_SUCCESS;
            }
        }
    }
    return ret;
}

static ezlopi_error_t __notify(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_FAILED;
    static float __prev[3] = { 0 };
    if (item)
    {
        s_adxl345_data_t *user_data = (s_adxl345_data_t *)item->user_arg;
        if (user_data)
        {
            if (ezlopi_item_name_acceleration_x_axis == item->cloud_properties.item_name)
            {
                __prev[0] = user_data->acc_x;
                __adxl345_get_axis_value(item);
                if (fabs((__prev[0] - user_data->acc_x) > 0.5))
                {
                    EZPI_core_device_value_updated_from_device_broadcast(item);
                }
            }
            if (ezlopi_item_name_acceleration_y_axis == item->cloud_properties.item_name)
            {
                __prev[1] = user_data->acc_y;
                if (fabs((__prev[1] - user_data->acc_x) > 0.5))
                {
                    EZPI_core_device_value_updated_from_device_broadcast(item);
                }
            }
            if (ezlopi_item_name_acceleration_z_axis == item->cloud_properties.item_name)
            {
                __prev[2] = user_data->acc_z;
                if (fabs((__prev[2] - user_data->acc_x) > 0.5))
                {
                    EZPI_core_device_value_updated_from_device_broadcast(item);
                }
            }
            ret = EZPI_SUCCESS;
        }
    }
    return ret;
}


/*******************************************************************************
*                          End of File
*******************************************************************************