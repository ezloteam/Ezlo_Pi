
#include "../../build/config/sdkconfig.h"
#include <math.h>
#include "ezlopi_util_trace.h"

#include "ezlopi_core_timer.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_hal_i2c_master.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "ALS_LTR303.h"
#include "sensor_0008_I2C_LTR303ALS.h"
#include "EZLOPI_USER_CONFIG.h"

static int __prepare(void* arg);
static int __init(l_ezlopi_item_t* item);
static int __notify(l_ezlopi_item_t* item);
static int __get_value_cjson(l_ezlopi_item_t* item, void* arg);

int sensor_0008_I2C_LTR303ALS(e_ezlopi_actions_t action, l_ezlopi_item_t* item, void* arg, void* user_arg)
{
    int ret = 0;
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

static int __get_value_cjson(l_ezlopi_item_t* item, void* arg)
{
    int ret = 0;
    if (item)
    {
        cJSON* cj_param = (cJSON*)arg;
        ltr303_data_t* als_ltr303_data = (ltr303_data_t*)item->user_arg;
        if (cj_param && als_ltr303_data)
        {
            ezlopi_valueformatter_double_to_cjson(item, cj_param, als_ltr303_data->lux);
        }
    }
    return ret;
}

static int __notify(l_ezlopi_item_t* item)
{
    int ret = 0;
    if (item)
    {
        ltr303_data_t* als_ltr303_data = (ltr303_data_t*)item->user_arg;
        if (als_ltr303_data)
        {
            ltr303_data_t temp_data;
            if (ESP_OK == ltr303_get_val(&temp_data))
            {
                if (fabs(als_ltr303_data->lux - temp_data.lux) > 0.2)
                {
                    als_ltr303_data->lux = temp_data.lux;
                    ezlopi_device_value_updated_from_device_broadcast(item);
                }
            }
        }
    }

    return ret;
}

static int __init(l_ezlopi_item_t* item)
{
    int ret = 0;
    if (item)
    {
        ltr303_data_t* als_ltr303_data = (ltr303_data_t*)item->user_arg;
        if (als_ltr303_data)
        {
            if (item->interface.i2c_master.enable)
            {
                if (ESP_OK == ltr303_setup(item->interface.i2c_master.sda, item->interface.i2c_master.scl, true))
                {
                    ltr303_get_val(als_ltr303_data);
                    ret = 1;
                }
                else
                {
                    ret = -1;
                }
            }
        }
        else
        {
            ret = -1;
        }
    }

    return ret;
}

static void __prepare_device_cloud_properties(l_ezlopi_device_t* device, cJSON* cj_params)
{
    device->cloud_properties.category = category_light_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_item_properties(l_ezlopi_item_t* item, cJSON* cj_param)
{
    CJSON_GET_VALUE_DOUBLE(cj_param, ezlopi_dev_type_str, item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.item_name = ezlopi_item_name_lux;
    item->cloud_properties.value_type = value_type_illuminance;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = scales_lux;

    CJSON_GET_VALUE_DOUBLE(cj_param, ezlopi_gpio_sda_str, item->interface.i2c_master.sda);
    CJSON_GET_VALUE_DOUBLE(cj_param, ezlopi_gpio_scl_str, item->interface.i2c_master.scl);

    item->interface.i2c_master.enable = true;
    item->interface.i2c_master.clock_speed = 100000;
    item->interface.i2c_master.address = LTR303_ADDR;
    ltr303_data_t* als_ltr303_data = (ltr303_data_t*)malloc(__FUNCTION__, sizeof(ltr303_data_t));
    if (als_ltr303_data)
    {
        memset(als_ltr303_data, 0, sizeof(ltr303_data_t));
        item->user_arg = (void*)als_ltr303_data;
    }
}

static int __prepare(void* arg)
{
    int ret = 0;

    s_ezlopi_prep_arg_t* prep_arg = (s_ezlopi_prep_arg_t*)arg;
    if (prep_arg && prep_arg->cjson_device)
    {
        l_ezlopi_device_t* als_ltr303_device = ezlopi_device_add_device(prep_arg->cjson_device, NULL);
        if (als_ltr303_device)
        {
            ret = 1;
            __prepare_device_cloud_properties(als_ltr303_device, prep_arg->cjson_device);
            l_ezlopi_item_t* als_ltr303_item = ezlopi_device_add_item_to_device(als_ltr303_device, sensor_0008_I2C_LTR303ALS);
            if (als_ltr303_item)
            {
                __prepare_item_properties(als_ltr303_item, prep_arg->cjson_device);
            }
            else
            {
                ezlopi_device_free_device(als_ltr303_device);
                ret = -1;
            }
        }
        else
        {
            ret = -1;
        }
    }

    return ret;
}
