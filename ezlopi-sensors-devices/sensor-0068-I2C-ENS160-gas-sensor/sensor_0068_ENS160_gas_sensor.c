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
 * @file    sensor_0068_ENS160_gas_sensor.c
 * @brief   perform some function on sensor_0068_ENS160
 * @author  xx
 * @version 0.1
 * @date    xx
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <stdlib.h>
#include <math.h>

#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_hal_i2c_master.h"

#include "ezlopi_cloud_keywords.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_item_name_str.h"
#include "ezlopi_cloud_scales_str.h"
#include "ezlopi_cloud_value_type_str.h"

#include "sensor_0068_ENS160_gas_sensor.h"
#include "sensor_0068_ENS160_gas_sensor_settings.h"
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
  \"dev_type\":8,\
  \"dev_name\":\"ENS160\",\
  \"id_room\":\"\",\
  \"id_item\":68,\
  \"gpio_scl\":17,\
  \"gpio_sda\":18\
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

// static uint32_t parent_device_id = 0;
static uint32_t aqi_item_id = 0;
static uint32_t voc_item_id = 0;
static uint32_t co2_item_id = 0;
/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/

ezlopi_error_t SENSOR_0068_ens160_gas_sensor(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
  ezlopi_error_t ret = EZPI_SUCCESS;
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
  if (cj_params)
  {
    ens160_t *ens160_sensor = (ens160_t *)item->user_arg;
    if (ens160_sensor)
    {
      if (item->cloud_properties.item_id == aqi_item_id)
      {
        EZPI_core_valueformatter_uint32_to_cjson(cj_params, ens160_sensor->data.aqi, item->cloud_properties.scale);
      }
      else if (item->cloud_properties.item_id == voc_item_id)
      {
        EZPI_core_valueformatter_uint32_to_cjson(cj_params, ens160_sensor->data.tvoc, item->cloud_properties.scale);
      }
      else if (item->cloud_properties.item_id == co2_item_id)
      {
        EZPI_core_valueformatter_uint32_to_cjson(cj_params, ens160_sensor->data.eco2, item->cloud_properties.scale);
      }
      ret = EZPI_SUCCESS;
    }
  }
  return ret;
}

static ezlopi_error_t __notify(l_ezlopi_item_t *item)
{
  ezlopi_error_t ret = EZPI_FAILED;
  ens160_t *ens160_sensor = (ens160_t *)item->user_arg;
  if (ens160_sensor)
  {
    if (ENS160_has_setting_changed())
    {
      float ambient_temperature = ENS160_get_ambient_temp_setting();
      float relative_humidity = ENS160_get_relative_humidity_setting();
      DFROBOT_ens160_set_temp_and_hum(ens160_sensor, ambient_temperature, relative_humidity);
    }
    ens160_t sensor_data;
    memcpy(&sensor_data, ens160_sensor, sizeof(ens160_t));
    DFROBOT_ens160_get_data(&sensor_data);
    if (item->cloud_properties.item_id == aqi_item_id)
    {
      if (fabs(sensor_data.data.aqi - ens160_sensor->data.aqi) > 0.05)
      {
        ens160_sensor->data.aqi = sensor_data.data.aqi;
        EZPI_core_device_value_updated_from_device_broadcast(item);
      }
    }
    if (item->cloud_properties.item_id == voc_item_id)
    {
      if (fabs(sensor_data.data.tvoc - ens160_sensor->data.tvoc) > 0.05)
      {
        ens160_sensor->data.tvoc = sensor_data.data.tvoc;
        EZPI_core_device_value_updated_from_device_broadcast(item);
      }
    }
    if (item->cloud_properties.item_id == co2_item_id)
    {
      if (fabs(sensor_data.data.eco2 - ens160_sensor->data.eco2) > 0.05)
      {
        ens160_sensor->data.eco2 = sensor_data.data.eco2;
        EZPI_core_device_value_updated_from_device_broadcast(item);
      }
    }
    ret = EZPI_SUCCESS;
  }
  return ret;
}

static ezlopi_error_t __init(l_ezlopi_item_t *item)
{
  ezlopi_error_t ret = EZPI_ERR_INIT_DEVICE_FAILED;

  ens160_t *ens160_sensor = (ens160_t *)item->user_arg;
  if (ens160_sensor)
  {
    if (item->interface.i2c_master.enable)
    {
      ens160_sensor->ezlopi_i2c = &item->interface.i2c_master;
      if (NO_ERR != DFROBOT_ens160_i2c_begin(ens160_sensor))
      {
        TRACE_E("Communication with device failed, please check connection");
        vTaskDelay(3000 / portTICK_PERIOD_MS);
      }
      else
      {
        TRACE_E("Begin ok!");
        DFROBOT_ens160_set_pwr_mode(ens160_sensor, ENS160_STANDARD_MODE);
        float ambient_temperature = ENS160_get_ambient_temp_setting();
        float relative_humidity = ENS160_get_relative_humidity_setting();
        // DFROBOT_ens160_set_temp_and_hum(ens160_sensor, /*temperature=*/25.0, /*humidity=*/50.0); // These should be able to be changed froms settings.
        DFROBOT_ens160_set_temp_and_hum(ens160_sensor, ambient_temperature, relative_humidity);
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

static void __prepare_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_arg)
{
  CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_type_str, item->interface_type);
  item->cloud_properties.item_id = EZPI_core_cloud_generate_item_id();
  item->cloud_properties.has_getter = true;
  item->cloud_properties.has_setter = false;
  item->cloud_properties.show = true;
  item->user_arg = user_arg;

  if (item->cloud_properties.item_name == ezlopi_item_name_volatile_organic_compound_level)
  {
    item->interface.i2c_master.clock_speed = ENS160_I2C_SPEED;
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_scl_str, item->interface.i2c_master.scl);
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_sda_str, item->interface.i2c_master.sda);
    item->interface.i2c_master.channel = I2C_NUM_0;
    item->interface.i2c_master.address = ENS160_I2C_ADDRESS;
    item->interface.i2c_master.enable = true;
  }
  else
  {
    item->interface.i2c_master.enable = false;
  }
}

static ezlopi_error_t __prepare(void *arg, void *user_arg)
{
  ezlopi_error_t ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;

  s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
  if (prep_arg)
  {
    ens160_t *ens160_sensor = (ens160_t *)ezlopi_malloc(__FUNCTION__, sizeof(ens160_t));
    if (ens160_sensor)
    {
      memset(ens160_sensor, 0, sizeof(ens160_t));
      /* Prepare AQI device. */
      l_ezlopi_device_t *parent_ens160_aqi_device = EZPI_core_device_add_device(prep_arg->cjson_device, "aqi");
      if (parent_ens160_aqi_device)
      {
        TRACE_I("Parent_ens160_aqi_device-[0x%x] ", parent_ens160_aqi_device->cloud_properties.device_id);
        __prepare_device_cloud_properties(parent_ens160_aqi_device, prep_arg->cjson_device);

        l_ezlopi_item_t *aqi_item = EZPI_core_device_add_item_to_device(parent_ens160_aqi_device, SENSOR_0068_ens160_gas_sensor);
        if (aqi_item)
        {
          aqi_item->cloud_properties.item_name = ezlopi_item_name_volatile_organic_compound_level;
          aqi_item->cloud_properties.value_type = value_type_substance_amount;
          aqi_item->cloud_properties.scale = scales_parts_per_million;
          __prepare_item_cloud_properties(aqi_item, prep_arg->cjson_device, ens160_sensor);
          aqi_item->is_user_arg_unique = true;
          aqi_item->interface.i2c_master.enable = true;
          aqi_item->interface.i2c_master.channel = I2C_NUM_0;
        }

        /* Prepare VOC device. */
        l_ezlopi_device_t *child_ens160_voc_device = EZPI_core_device_add_device(prep_arg->cjson_device, "voc");
        if (child_ens160_voc_device)
        {
          TRACE_I("Child_ens160_aqi_device-[0x%x] ", child_ens160_voc_device->cloud_properties.device_id);
          __prepare_device_cloud_properties(child_ens160_voc_device, prep_arg->cjson_device);

          l_ezlopi_item_t *voc_item = EZPI_core_device_add_item_to_device(child_ens160_voc_device, SENSOR_0068_ens160_gas_sensor);
          if (voc_item)
          {
            voc_item->cloud_properties.item_name = ezlopi_item_name_volatile_organic_compound_level;
            voc_item->cloud_properties.value_type = value_type_substance_amount;
            voc_item->cloud_properties.scale = scales_parts_per_million;
            __prepare_item_cloud_properties(voc_item, prep_arg->cjson_device, ens160_sensor);
          }
          else
          {
            EZPI_core_device_free_device(child_ens160_voc_device);
          }
        }

        /* Prepare CO2 device. */
        l_ezlopi_device_t *child_ens160_co2_device = EZPI_core_device_add_device(prep_arg->cjson_device, "co2");
        if (child_ens160_co2_device)
        {
          TRACE_I("Child_ens160_co2_device-[0x%x] ", child_ens160_co2_device->cloud_properties.device_id);
          __prepare_device_cloud_properties(child_ens160_co2_device, prep_arg->cjson_device);

          l_ezlopi_item_t *co2_item = EZPI_core_device_add_item_to_device(child_ens160_co2_device, SENSOR_0068_ens160_gas_sensor);
          if (co2_item)
          {
            co2_item->cloud_properties.item_name = ezlopi_item_name_co2_level;
            co2_item->cloud_properties.value_type = value_type_substance_amount;
            co2_item->cloud_properties.scale = scales_parts_per_million;
            __prepare_item_cloud_properties(co2_item, prep_arg->cjson_device, ens160_sensor);
          }
          else
          {
            EZPI_core_device_free_device(child_ens160_co2_device);
          }
        }

        if ((NULL == aqi_item) &&
            (NULL == child_ens160_voc_device) &&
            (NULL == child_ens160_co2_device))
        {
          EZPI_core_device_free_device(parent_ens160_aqi_device);
          ezlopi_free(__FUNCTION__, ens160_sensor);
        }
        else
        {
          ret = EZPI_SUCCESS;
          ENS160_gas_sensor_settings_init(parent_ens160_aqi_device, (void *)ens160_sensor);
        }
      }
      else
      {
        ezlopi_free(__FUNCTION__, ens160_sensor);
      }
    }
  }
  return ret;
}
/*******************************************************************************
 *                          End of File
 *******************************************************************************/