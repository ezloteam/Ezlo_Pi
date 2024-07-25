
#include <stdlib.h>
#include <math.h>

#include "ezlopi_util_trace.h"

#include "ezlopi_core_devices_list.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_device_value_updated.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_errors.h"

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

static uint32_t parent_device_id = 0;
static uint32_t aqi_item_id = 0;
static uint32_t voc_item_id = 0;
static uint32_t co2_item_id = 0;

static ezlopi_error_t __prepare(void* arg, void* user_arg);
static ezlopi_error_t __init(l_ezlopi_item_t* item);
static ezlopi_error_t __get_cjson_value(l_ezlopi_item_t* item, void* arg);
static ezlopi_error_t __notify(l_ezlopi_item_t* item);

ezlopi_error_t sensor_0068_ENS160_gas_sensor(e_ezlopi_actions_t action, l_ezlopi_item_t* item, void* arg, void* user_arg)
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

static ezlopi_error_t __get_cjson_value(l_ezlopi_item_t* item, void* arg)
{
  ezlopi_error_t ret = EZPI_FAILED;

  cJSON* cj_params = (cJSON*)arg;
  if (cj_params)
  {
    ens160_t* ens160_sensor = (ens160_t*)item->user_arg;
    if (ens160_sensor)
    {
      if (item->cloud_properties.item_id == aqi_item_id)
      {
        ezlopi_valueformatter_uint32_to_cjson(item, cj_params, ens160_sensor->data.aqi);
      }
      else if (item->cloud_properties.item_id == voc_item_id)
      {
        ezlopi_valueformatter_uint32_to_cjson(item, cj_params, ens160_sensor->data.tvoc);
      }
      else if (item->cloud_properties.item_id == co2_item_id)
      {
        ezlopi_valueformatter_uint32_to_cjson(item, cj_params, ens160_sensor->data.eco2);
      }
      ret = EZPI_SUCCESS;
    }
  }
  return ret;
}

static ezlopi_error_t __notify(l_ezlopi_item_t* item)
{
  ezlopi_error_t ret = EZPI_FAILED;
  ens160_t* ens160_sensor = (ens160_t*)item->user_arg;
  if (ens160_sensor)
  {
    if (has_setting_changed())
    {
      float ambient_temperature = get_ambient_temperature_setting();
      float relative_humidity = get_relative_humidity_setting();
      dfrobot_ens160_set_temp_and_hum(ens160_sensor, ambient_temperature, relative_humidity);
    }
    ens160_t sensor_data;
    memcpy(&sensor_data, ens160_sensor, sizeof(ens160_t));
    dfrobot_ens160_get_data(&sensor_data);
    if (item->cloud_properties.item_id == aqi_item_id)
    {
      if (fabs(sensor_data.data.aqi - ens160_sensor->data.aqi) > 0.05)
      {
        ens160_sensor->data.aqi = sensor_data.data.aqi;
        ezlopi_device_value_updated_from_device_broadcast(item);
      }
    }
    if (item->cloud_properties.item_id == voc_item_id)
    {
      if (fabs(sensor_data.data.tvoc - ens160_sensor->data.tvoc) > 0.05)
      {
        ens160_sensor->data.tvoc = sensor_data.data.tvoc;
        ezlopi_device_value_updated_from_device_broadcast(item);
      }
    }
    if (item->cloud_properties.item_id == co2_item_id)
    {
      if (fabs(sensor_data.data.eco2 - ens160_sensor->data.eco2) > 0.05)
      {
        ens160_sensor->data.eco2 = sensor_data.data.eco2;
        ezlopi_device_value_updated_from_device_broadcast(item);
      }
    }
    ret = EZPI_SUCCESS;
  }
  return ret;
}

static ezlopi_error_t __init(l_ezlopi_item_t* item)
{
  ezlopi_error_t ret = EZPI_ERR_INIT_DEVICE_FAILED;

  ens160_t* ens160_sensor = (ens160_t*)item->user_arg;
  if (ens160_sensor)
  {
    if (item->interface.i2c_master.enable)
    {
      ens160_sensor->ezlopi_i2c = &item->interface.i2c_master;
      if (NO_ERR != dfrobot_ens160_i2c_begin(ens160_sensor))
      {
        TRACE_E("Communication with device failed, please check connection");
        vTaskDelay(3000 / portTICK_PERIOD_MS);
      }
      else
      {
        TRACE_E("Begin ok!");
        dfrobot_ens160_set_pwr_mode(ens160_sensor, ENS160_STANDARD_MODE);
        float ambient_temperature = get_ambient_temperature_setting();
        float relative_humidity = get_relative_humidity_setting();
        // dfrobot_ens160_set_temp_and_hum(ens160_sensor, /*temperature=*/25.0, /*humidity=*/50.0); // These should be able to be changed froms settings.
        dfrobot_ens160_set_temp_and_hum(ens160_sensor, ambient_temperature, relative_humidity);
        ret = EZPI_SUCCESS;
      }
    }
  }
  return ret;
}

static void __prepare_device_cloud_properties(l_ezlopi_device_t* device, cJSON* cj_device)
{
  device->cloud_properties.category = category_level_sensor;
  device->cloud_properties.subcategory = subcategory_not_defined;
  device->cloud_properties.device_type = dev_type_sensor;
  device->cloud_properties.info = NULL;
  device->cloud_properties.device_type_id = NULL;
}

static void __prepare_item_cloud_properties(l_ezlopi_item_t* item, cJSON* cj_device, void* user_arg)
{
  CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_dev_type_str, item->interface_type);
  item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
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

static ezlopi_error_t __prepare(void* arg, void* user_arg)
{
  ezlopi_error_t ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;

  s_ezlopi_prep_arg_t* prep_arg = (s_ezlopi_prep_arg_t*)arg;
  if (prep_arg)
  {
    ens160_t* ens160_sensor = (ens160_t*)ezlopi_malloc(__FUNCTION__, sizeof(ens160_t));
    if (ens160_sensor)
    {
      memset(ens160_sensor, 0, sizeof(ens160_t));
      /* Prepare AQI device. */
      l_ezlopi_device_t* parent_ens160_aqi_device = ezlopi_device_add_device(prep_arg->cjson_device, "aqi");
      if (parent_ens160_aqi_device)
      {
        ret = EZPI_SUCCESS;
        TRACE_I("Parent_ens160_aqi_device-[0x%x] ", parent_ens160_aqi_device->cloud_properties.device_id);
        __prepare_device_cloud_properties(parent_ens160_aqi_device, prep_arg->cjson_device);

        l_ezlopi_item_t* aqi_item = ezlopi_device_add_item_to_device(parent_ens160_aqi_device, sensor_0068_ENS160_gas_sensor);
        if (aqi_item)
        {
          aqi_item->cloud_properties.item_name = ezlopi_item_name_volatile_organic_compound_level;
          aqi_item->cloud_properties.value_type = value_type_substance_amount;
          aqi_item->cloud_properties.scale = scales_parts_per_million;
          __prepare_item_cloud_properties(aqi_item, prep_arg->cjson_device, ens160_sensor);
          aqi_item->is_user_arg_unique = true;
          aqi_item->interface.i2c_master.enable = true;
        }

        /* Prepare VOC device. */
        l_ezlopi_device_t* child_ens160_voc_device = ezlopi_device_add_device(prep_arg->cjson_device, "voc");
        if (child_ens160_voc_device)
        {
          TRACE_I("Child_ens160_aqi_device-[0x%x] ", child_ens160_voc_device->cloud_properties.device_id);
          __prepare_device_cloud_properties(child_ens160_voc_device, prep_arg->cjson_device);

          child_ens160_voc_device->cloud_properties.parent_device_id = parent_ens160_aqi_device->cloud_properties.device_id;
          l_ezlopi_item_t* voc_item = ezlopi_device_add_item_to_device(child_ens160_voc_device, sensor_0068_ENS160_gas_sensor);
          if (voc_item)
          {
            voc_item->cloud_properties.item_name = ezlopi_item_name_volatile_organic_compound_level;
            voc_item->cloud_properties.value_type = value_type_substance_amount;
            voc_item->cloud_properties.scale = scales_parts_per_million;
            __prepare_item_cloud_properties(voc_item, prep_arg->cjson_device, ens160_sensor);
          }
          else
          {
            ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
            ezlopi_device_free_device(child_ens160_voc_device);
          }
        }

        /* Prepare CO2 device. */
        l_ezlopi_device_t* child_ens160_co2_device = ezlopi_device_add_device(prep_arg->cjson_device, "co2");
        if (child_ens160_co2_device)
        {
          TRACE_I("Child_ens160_co2_device-[0x%x] ", child_ens160_co2_device->cloud_properties.device_id);
          __prepare_device_cloud_properties(child_ens160_co2_device, prep_arg->cjson_device);

          child_ens160_co2_device->cloud_properties.parent_device_id = parent_ens160_aqi_device->cloud_properties.device_id;
          l_ezlopi_item_t* co2_item = ezlopi_device_add_item_to_device(child_ens160_co2_device, sensor_0068_ENS160_gas_sensor);
          if (co2_item)
          {
            co2_item->cloud_properties.item_name = ezlopi_item_name_co2_level;
            co2_item->cloud_properties.value_type = value_type_substance_amount;
            co2_item->cloud_properties.scale = scales_parts_per_million;
            __prepare_item_cloud_properties(co2_item, prep_arg->cjson_device, ens160_sensor);
          }
          else
          {
            ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
            ezlopi_device_free_device(child_ens160_co2_device);
          }
        }

        if ((NULL == aqi_item) &&
          (NULL == child_ens160_voc_device) &&
          (NULL == child_ens160_co2_device))
        {
          ezlopi_device_free_device(parent_ens160_aqi_device);
          ezlopi_free(__FUNCTION__, ens160_sensor);
          ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
        }
        else
        {
          sensor_0068_gas_sensor_settings_initialize(parent_ens160_aqi_device, (void*)ens160_sensor);
        }
      }
      else
      {
        ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
        ezlopi_free(__FUNCTION__, ens160_sensor);
      }
    }
    else
    {
      ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
    }
  }
  return ret;
}
