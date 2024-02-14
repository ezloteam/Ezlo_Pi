
#include "sdkconfig.h"
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

static int __prepare(void *arg);
static int __init(l_ezlopi_item_t *item);
static int __notify(l_ezlopi_item_t *item);
static int __get_value_cjson(l_ezlopi_item_t *item, void *arg);

int sensor_0008_I2C_LTR303ALS(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        __prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        __init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        __get_value_cjson(item, arg);
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

static int __get_value_cjson(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;

    cJSON *cj_param = (cJSON *)arg;
    ltr303_data_t *als_ltr303_data = (ltr303_data_t *)item->user_arg;
    if (cj_param && als_ltr303_data)
    {
        cJSON_AddNumberToObject(cj_param, ezlopi_value_str, als_ltr303_data->lux);
        char *valueFormatted = ezlopi_valueformatter_double(als_ltr303_data->lux);
        cJSON_AddStringToObject(cj_param, ezlopi_valueFormatted_str, valueFormatted);
        free(valueFormatted);
    }

    return ret;
}

static int __notify(l_ezlopi_item_t *item)
{
    int ret = 0;

    ltr303_data_t *als_ltr303_data = (ltr303_data_t *)item->user_arg;
    if (als_ltr303_data)
    {
        ltr303_data_t temp_data;
        if (ESP_OK == ltr303_get_val(&temp_data))
        {
            if (fabs(als_ltr303_data->lux - temp_data.lux) > 0.2)
            {
                als_ltr303_data->lux = temp_data.lux;
                ezlopi_device_value_updated_from_device_v3(item);
            }
        }
    }

    return ret;
}

static int __init(l_ezlopi_item_t *item)
{
    int ret = 0;

    ltr303_data_t *als_ltr303_data = (ltr303_data_t *)item->user_arg;
    if (item)
    {
        if ((item->interface.i2c_master.enable) && (NULL != als_ltr303_data))
        {
            ltr303_setup(item->interface.i2c_master.sda, item->interface.i2c_master.scl, true);
            ltr303_get_val(als_ltr303_data);
            ret = 1;
        }
        if (0 == ret)
        {
            ret = -1;
            if (item->user_arg)
            {
                free(item->user_arg);
                item->user_arg = NULL;
            }
        }
    }

    return ret;
}

static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_params)
{
    // char *device_name = NULL;
    // CJSON_GET_VALUE_STRING(cj_params, ezlopi_dev_name_str, device_name);
    // ASSIGN_DEVICE_NAME_V2(device, device_name);
    // device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();

    device->cloud_properties.category = category_light_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_item_properties(l_ezlopi_item_t *item, cJSON *cj_param)
{
    CJSON_GET_VALUE_INT(cj_param, ezlopi_dev_type_str, item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.item_name = ezlopi_item_name_lux;
    item->cloud_properties.value_type = value_type_illuminance;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = scales_lux;

    CJSON_GET_VALUE_INT(cj_param, ezlopi_gpio_sda_str, item->interface.i2c_master.sda);
    CJSON_GET_VALUE_INT(cj_param, ezlopi_gpio_scl_str, item->interface.i2c_master.scl);

    item->interface.i2c_master.enable = true;
    item->interface.i2c_master.clock_speed = 100000;
    item->interface.i2c_master.address = LTR303_ADDR;
    ltr303_data_t *als_ltr303_data = (ltr303_data_t *)malloc(sizeof(ltr303_data_t));
    if (als_ltr303_data)
    {
        memset(als_ltr303_data, 0, sizeof(ltr303_data_t));
        item->user_arg = (void *)als_ltr303_data;
    }
}

static int __prepare(void *arg)
{
    int ret = 0;

    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (prep_arg && prep_arg->cjson_device)
    {
        l_ezlopi_device_t *als_ltr303_device = ezlopi_device_add_device(prep_arg->cjson_device);
        if (als_ltr303_device)
        {
            __prepare_device_cloud_properties(als_ltr303_device, prep_arg->cjson_device);
            l_ezlopi_item_t *als_ltr303_item = ezlopi_device_add_item_to_device(als_ltr303_device, sensor_0008_I2C_LTR303ALS);
            if (als_ltr303_item)
            {
                als_ltr303_item->cloud_properties.device_id = als_ltr303_device->cloud_properties.device_id;
                __prepare_item_properties(als_ltr303_item, prep_arg->cjson_device);
            }
            else
            {
                ezlopi_device_free_device(als_ltr303_device);
                ret = -1;
            }
        }
    }

    return ret;
}
