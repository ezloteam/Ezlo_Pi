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

#include "sensor_0044_I2C_TSL256_luminosity.h"
#include "EZLOPI_USER_CONFIG.h"
//-----------------------------------------------------------------------

static ezlopi_error_t __prepare(void* arg);
static ezlopi_error_t __init(l_ezlopi_item_t* item);
static ezlopi_error_t __get_cjson_value(l_ezlopi_item_t* item, void* arg);
static ezlopi_error_t __notify(l_ezlopi_item_t* item);
static void __prepare_device_cloud_properties(l_ezlopi_device_t* device, cJSON* cj_device);
static void __prepare_item_cloud_properties(l_ezlopi_item_t* item, cJSON* cj_device, void* user_data);

ezlopi_error_t sensor_0044_I2C_TSL256_luminosity(e_ezlopi_actions_t action, l_ezlopi_item_t* item, void* arg, void* user_arg)
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

static ezlopi_error_t __notify(l_ezlopi_item_t* item)
{
    ezlopi_error_t ret = EZPI_FAILED;

    // Allow only significant changes in values to be posted
    double new_value = (double)tsl2561_get_intensity_value(&item->interface.i2c_master);
    TSL256_lum_t* TSL2561_lux_data = (TSL256_lum_t*)item->user_arg;
    if (TSL2561_lux_data)
    {
        if (fabs((double)(TSL2561_lux_data->lux_val) - new_value) > 0.0001)
        {
            ezlopi_device_value_updated_from_device_broadcast(item);
            TSL2561_lux_data->lux_val = (uint32_t)new_value;
            ret = EZPI_SUCCESS;
        }
    }

    return ret;
}

static ezlopi_error_t __get_cjson_value(l_ezlopi_item_t* item, void* arg)
{
    ezlopi_error_t ret = EZPI_FAILED;
    cJSON* cj_result = (cJSON*)arg;
    if (cj_result)
    {
        TSL256_lum_t* illuminance_value = ((TSL256_lum_t*)item->user_arg);
        if (illuminance_value)
        {
            ezlopi_valueformatter_uint32_to_cjson(cj_result, illuminance_value->lux_val, scales_lux);
            ret = EZPI_SUCCESS;
        }
    }
    return ret;
}

static ezlopi_error_t __init(l_ezlopi_item_t* item)
{
    ezlopi_error_t ret = EZPI_ERR_INIT_DEVICE_FAILED;

    if (item)
    {
        TSL256_lum_t* TSL2561_lux_data = (TSL256_lum_t*)item->user_arg;
        if (TSL2561_lux_data)
        {
            if (item->interface.i2c_master.enable)
            {
                ezlopi_i2c_master_init(&item->interface.i2c_master);
                TRACE_I("I2C channel is %d", item->interface.i2c_master.channel);
                if (Check_PARTID(&item->interface.i2c_master))
                {
                    TRACE_I("TSL561 initialization finished.........");
                    sensor_0044_tsl2561_configure_device(&item->interface.i2c_master);
                    ret = EZPI_SUCCESS;
                }
            }
        }
    }
    return ret;
}

static void __prepare_device_cloud_properties(l_ezlopi_device_t* device, cJSON* cj_device)
{
    device->cloud_properties.category = category_light_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_item_cloud_properties(l_ezlopi_item_t* item, cJSON* cj_device, void* user_data)
{
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_dev_type_str, item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_lux;
    item->cloud_properties.value_type = value_type_illuminance;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = scales_lux;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_sda_str, item->interface.i2c_master.sda);
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_scl_str, item->interface.i2c_master.scl);
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_slave_addr_str, item->interface.i2c_master.address);

    item->interface.i2c_master.enable = true;
    item->interface.i2c_master.clock_speed = 100000;
    if (0x00 == item->interface.i2c_master.address)
    {
        item->interface.i2c_master.address = TSL2561_ADDRESS;
    }
    // passing the custom data_structure
    item->is_user_arg_unique = true;
    item->user_arg = user_data;
}

static ezlopi_error_t __prepare(void* arg)
{
    ezlopi_error_t ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;

    s_ezlopi_prep_arg_t* prep_arg = (s_ezlopi_prep_arg_t*)arg;
    if (prep_arg && prep_arg->cjson_device)
    {
        TSL256_lum_t* TSL2561_lux_data = (TSL256_lum_t*)ezlopi_malloc(__FUNCTION__, sizeof(TSL256_lum_t));
        if (TSL2561_lux_data)
        {
            memset(TSL2561_lux_data, 0, sizeof(TSL256_lum_t));
            l_ezlopi_device_t* tsl256_device = ezlopi_device_add_device(prep_arg->cjson_device, NULL);
            if (tsl256_device)
            {
                __prepare_device_cloud_properties(tsl256_device, prep_arg->cjson_device);

                l_ezlopi_item_t* tsl256_item = ezlopi_device_add_item_to_device(tsl256_device, sensor_0044_I2C_TSL256_luminosity);
                if (tsl256_item)
                {
                    __prepare_item_cloud_properties(tsl256_item, prep_arg->cjson_device, TSL2561_lux_data);
                    ret = EZPI_SUCCESS;
                }
                else
                {
                    ezlopi_device_free_device(tsl256_device);
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
