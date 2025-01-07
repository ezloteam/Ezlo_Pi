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
 * @file    sensor_0029_I2C_GXHTC3.c
 * @brief   perform some function on sensor_0029
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

#include "sensor_0029_I2C_GXHTC3.h"
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

static uint8_t compare_float_values(float a, float b);
static ezlopi_error_t gxhtc3_sensor_init(l_ezlopi_item_t *item);
// static int gxhtc3_sensor_get_cjson_value_temp(l_ezlopi_item_t *item, void *arg);
static ezlopi_error_t __get_cjson_value(l_ezlopi_item_t *item, void *arg);
static ezlopi_error_t __get_cjson_update_value(l_ezlopi_item_t *item);
static void __prepare_device_cloud_properties_temp(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_device_cloud_properties_hum(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_temperature_item_properties(l_ezlopi_item_t *item, cJSON *cj_device);
static void __prepare_humidity_item_properties(l_ezlopi_item_t *item, cJSON *cj_device);
static ezlopi_error_t __prepare(void *arg);
/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
ezlopi_error_t SENSOR_0029_i2c_gxhtc3(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
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
        ret = gxhtc3_sensor_init(item);
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
        __get_cjson_update_value(item);
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
static uint8_t compare_float_values(float a, float b)
{
    return (fabs(a - b) > FLOAT_EPSILON);
}

static ezlopi_error_t gxhtc3_sensor_init(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_ERR_INIT_DEVICE_FAILED;
    if (item)
    {
        s_gxhtc3_value_t *gxhtce_val = (s_gxhtc3_value_t *)item->user_arg;
        if (gxhtce_val)
        {
            if ((NULL == gxhtce_val->gxhtc3) && (item->interface.i2c_master.enable))
            {
                if (EZPI_SUCCESS == EZPI_hal_i2c_master_init(&item->interface.i2c_master))
                {
                    gxhtce_val->gxhtc3 = GXHTC3_init(item->interface.i2c_master.channel, item->interface.i2c_master.address);
                    if (gxhtce_val->gxhtc3)
                    {
                        if (gxhtce_val->gxhtc3->id.status)
                        {
                            TRACE_E("GXHTC3 Chip ID: 0x%x", gxhtce_val->gxhtc3->id.id);
                            ret = EZPI_SUCCESS;
                        }
                        else
                        {
                            TRACE_E("GXHTC3 Chip ID not ready!");
                        }
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

// static int gxhtc3_sensor_get_cjson_value_temp(l_ezlopi_item_t* item, void* arg)
// {
//     int ret = 0;
//     if (item && arg)
//     {
//         cJSON* cj_result = (cJSON*)arg;
//         s_gxhtc3_value_t* value_ptr = (s_gxhtc3_value_t*)item->user_arg;
//         EZPI_core_valueformatter_float_to_cjson(cj_result, value_ptr->temperature, item->cloud_properties.scale);
//     }
//     return ret;
// }

static ezlopi_error_t __get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_FAILED;

    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        s_gxhtc3_value_t *value_ptr = (s_gxhtc3_value_t *)item->user_arg;

        if (value_ptr)
        {
            if (value_type_temperature == item->cloud_properties.value_type)
            {
                EZPI_core_valueformatter_float_to_cjson(cj_result, value_ptr->temperature, scales_celsius);
                ret = EZPI_SUCCESS;
            }
            else if (value_type_humidity == item->cloud_properties.value_type)
            {
                EZPI_core_valueformatter_float_to_cjson(cj_result, value_ptr->humidity, scales_percent);
                ret = EZPI_SUCCESS;
            }
        }
    }

    return ret;
}

static ezlopi_error_t __get_cjson_update_value(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_FAILED;

    if (item)
    {
        s_gxhtc3_value_t *value_ptr = (s_gxhtc3_value_t *)item->user_arg;

        if (value_ptr && value_ptr->gxhtc3)
        {
            if (GXHTC3_read_sensor(value_ptr->gxhtc3))
            {

                if (value_type_temperature == item->cloud_properties.value_type)
                {
                    if (compare_float_values(value_ptr->temperature, value_ptr->gxhtc3->reading_temp_c))
                    {
                        value_ptr->temperature = value_ptr->gxhtc3->reading_temp_c;
                        EZPI_core_device_value_updated_from_device_broadcast(item);
                        ret = EZPI_SUCCESS;
                    }
                }
                else if (value_type_humidity == item->cloud_properties.value_type)
                {
                    if (compare_float_values(value_ptr->humidity, value_ptr->gxhtc3->reading_rh))
                    {
                        value_ptr->humidity = value_ptr->gxhtc3->reading_rh;
                        EZPI_core_device_value_updated_from_device_broadcast(item);
                        ret = EZPI_SUCCESS;
                    }
                }
            }
        }
    }

    return ret;
}

static void __prepare_device_cloud_properties_temp(l_ezlopi_device_t *device, cJSON *cj_device)
{
    device->cloud_properties.category = category_temperature;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_device_cloud_properties_hum(l_ezlopi_device_t *device, cJSON *cj_device)
{
    device->cloud_properties.category = category_humidity;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_temperature_item_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_dev_type_str, item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_temp;
    item->cloud_properties.value_type = value_type_temperature;
    item->cloud_properties.scale = scales_celsius;
    item->cloud_properties.show = true;
    item->cloud_properties.item_id = EZPI_core_cloud_generate_item_id();

    item->interface_type = EZLOPI_DEVICE_INTERFACE_I2C_MASTER;
    item->interface.i2c_master.enable = true;
    item->interface.i2c_master.channel = I2C_NUM_0;
    item->interface.i2c_master.clock_speed = 400000;
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_scl_str, item->interface.i2c_master.scl);
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_sda_str, item->interface.i2c_master.sda);
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_slave_addr_str, item->interface.i2c_master.address);
}

static void __prepare_humidity_item_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_dev_type_str, item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_humidity;
    item->cloud_properties.value_type = value_type_humidity;
    item->cloud_properties.show = true;
    item->cloud_properties.item_id = EZPI_core_cloud_generate_item_id();
    item->cloud_properties.scale = scales_percent;

    item->interface_type = EZLOPI_DEVICE_INTERFACE_I2C_MASTER;
    item->interface.i2c_master.enable = false;
    item->interface.i2c_master.channel = 0;
    item->interface.i2c_master.clock_speed = 400000;
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_scl_str, item->interface.i2c_master.scl);
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_sda_str, item->interface.i2c_master.sda);
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_slave_addr_str, item->interface.i2c_master.address);
}

static ezlopi_error_t __prepare(void *arg)
{
    ezlopi_error_t ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;

    if (prep_arg && prep_arg->cjson_device)
    {
        s_gxhtc3_value_t *value_ptr = ezlopi_malloc(__FUNCTION__, sizeof(s_gxhtc3_value_t));
        if (value_ptr)
        {
            memset(value_ptr, 0, sizeof(s_gxhtc3_value_t));

            l_ezlopi_device_t *parent_device_temp = EZPI_core_device_add_device(prep_arg->cjson_device, "temp");
            if (parent_device_temp)
            {
                TRACE_I("Parent_temp_device-[0x%x] ", parent_device_temp->cloud_properties.device_id);
                __prepare_device_cloud_properties_temp(parent_device_temp, prep_arg->cjson_device);
                l_ezlopi_item_t *item_temperature = EZPI_core_device_add_item_to_device(parent_device_temp, SENSOR_0029_i2c_gxhtc3);
                if (item_temperature)
                {
                    __prepare_temperature_item_properties(item_temperature, prep_arg->cjson_device);
                    value_ptr->temperature = 65536.0f;
                    item_temperature->is_user_arg_unique = true;
                    item_temperature->user_arg = (void *)value_ptr;
                }

                l_ezlopi_device_t *child_device_hum = EZPI_core_device_add_device(prep_arg->cjson_device, "humi");
                if (child_device_hum)
                {
                    TRACE_I("Child_humidity_device-[0x%x] ", child_device_hum->cloud_properties.device_id);
                    __prepare_device_cloud_properties_hum(child_device_hum, prep_arg->cjson_device);

                    l_ezlopi_item_t *item_humdity = EZPI_core_device_add_item_to_device(child_device_hum, SENSOR_0029_i2c_gxhtc3);
                    if (item_humdity)
                    {
                        __prepare_humidity_item_properties(item_humdity, prep_arg->cjson_device);
                        value_ptr->humidity = 65536.0f;
                        item_temperature->is_user_arg_unique = false;
                        item_humdity->user_arg = (void *)value_ptr; // affected if 'value_pts' is already freed?
                    }
                    else
                    {
                        EZPI_core_device_free_device(child_device_hum);
                    }
                }

                if ((NULL == item_temperature) &&
                    (NULL == child_device_hum))
                {
                    ezlopi_free(__FUNCTION__, value_ptr);
                    EZPI_core_device_free_device(parent_device_temp);
                }
                else
                {
                    ret = EZPI_SUCCESS;
                }
            }
            else
            {
                ezlopi_free(__FUNCTION__, value_ptr);
            }
        }
    }
    return ret;
}

/*******************************************************************************
 *                          End of File
 *******************************************************************************/