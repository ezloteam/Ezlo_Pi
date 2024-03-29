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

#include "sensor_0044_I2C_TSL256_luminosity.h"
//-----------------------------------------------------------------------

static int __prepare(void* arg);
static int __init(l_ezlopi_item_t* item);
static int __get_cjson_value(l_ezlopi_item_t* item, void* arg);
static int __notify(l_ezlopi_item_t* item);
static void __prepare_device_cloud_properties(l_ezlopi_device_t* device, cJSON* cj_device);
static void __prepare_item_cloud_properties(l_ezlopi_item_t* item, cJSON* cj_device, void* user_data);

int sensor_0044_I2C_TSL256_luminosity(e_ezlopi_actions_t action, l_ezlopi_item_t* item, void* arg, void* user_arg)
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
        __get_cjson_value(item, arg);
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

static int __notify(l_ezlopi_item_t* item)
{
    int ret = 0;

    // Allow only significant changes in values to be posted
    double new_value = (double)tsl2561_get_intensity_value(&item->interface.i2c_master);
    TSL256_lum_t* TSL2561_lux_data = (TSL256_lum_t*)item->user_arg;
    if (TSL2561_lux_data)
    {
        if (fabs((double)(TSL2561_lux_data->lux_val) - new_value) > 0.0001)
        {
            ezlopi_device_value_updated_from_device_v3(item);
            TSL2561_lux_data->lux_val = (uint32_t)new_value;
        }
    }

    return ret;
}

static int __get_cjson_value(l_ezlopi_item_t* item, void* arg)
{
    int ret = 0;
    cJSON* cjson_properties = (cJSON*)arg;
    if (cjson_properties)
    {
        TSL256_lum_t* illuminance_value = ((TSL256_lum_t*)item->user_arg);
        if (illuminance_value)
        {
            char* valueFormatted = ezlopi_valueformatter_int((int)(illuminance_value->lux_val));
            if (valueFormatted)
            {
                cJSON_AddStringToObject(cjson_properties, ezlopi_valueFormatted_str, valueFormatted);
                free(valueFormatted);
            }
            cJSON_AddNumberToObject(cjson_properties, ezlopi_value_str, (int)illuminance_value->lux_val);
            cJSON_AddStringToObject(cjson_properties, ezlopi_scale_str, item->cloud_properties.scale);
            ret = 1;
        }
    }
    return ret;
}

static int __init(l_ezlopi_item_t* item)
{
    int ret = 0;

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
                    ret = 1;
                }
                // else
                // {
                //     ret = -1;
                //     free(item->user_arg); // this will free ; memory address linked to all items
                //     item->user_arg = NULL;
                //     // ezlopi_device_free_device_by_item(item);
                //     TRACE_E("TSL561 not found!....... Please Restart!! or Check your I2C connection...");
                // }
            }
        }
        // else
        // {
        //     ret = -1;
        //     ezlopi_device_free_device_by_item(item);
        // }
    }
    return ret;
}

static void __prepare_device_cloud_properties(l_ezlopi_device_t* device, cJSON* cj_device)
{
    // char *device_name = NULL;
    // CJSON_GET_VALUE_STRING(cj_device, ezlopi_dev_name_str, device_name);
    // ASSIGN_DEVICE_NAME_V2(device, device_name);
    // device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();

    device->cloud_properties.category = category_light_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __prepare_item_cloud_properties(l_ezlopi_item_t* item, cJSON* cj_device, void* user_data)
{
    CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_type_str, item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_lux;
    item->cloud_properties.value_type = value_type_illuminance;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = scales_lux;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    CJSON_GET_VALUE_INT(cj_device, ezlopi_gpio_sda_str, item->interface.i2c_master.sda);
    CJSON_GET_VALUE_INT(cj_device, ezlopi_gpio_scl_str, item->interface.i2c_master.scl);
    CJSON_GET_VALUE_INT(cj_device, "slave_addr", item->interface.i2c_master.address);

    item->interface.i2c_master.enable = true;
    item->interface.i2c_master.clock_speed = 100000;
    if (0x00 == item->interface.i2c_master.address)
    {
        item->interface.i2c_master.address = TSL2561_ADDRESS;
    }
    // passing the custom data_structure
    item->user_arg = user_data;
}

static int __prepare(void* arg)
{
    int ret = 0;

    s_ezlopi_prep_arg_t* prep_arg = (s_ezlopi_prep_arg_t*)arg;
    if (prep_arg && prep_arg->cjson_device)
    {
        TSL256_lum_t* TSL2561_lux_data = (TSL256_lum_t*)malloc(sizeof(TSL256_lum_t));
        if (TSL2561_lux_data)
        {
            memset(TSL2561_lux_data, 0, sizeof(TSL256_lum_t));
            l_ezlopi_device_t* tsl256_device = ezlopi_device_add_device(prep_arg->cjson_device);
            if (tsl256_device)
            {
                __prepare_device_cloud_properties(tsl256_device, prep_arg->cjson_device);

                l_ezlopi_item_t* tsl256_item = ezlopi_device_add_item_to_device(tsl256_device, sensor_0044_I2C_TSL256_luminosity);
                if (tsl256_item)
                {
                    tsl256_item->cloud_properties.device_id = tsl256_device->cloud_properties.device_id;
                    __prepare_item_cloud_properties(tsl256_item, prep_arg->cjson_device, TSL2561_lux_data);
                    ret = 1;
                }
                else
                {
                    ret = -1;
                    ezlopi_device_free_device(tsl256_device);
                    free(TSL2561_lux_data);
                }
            }
            else
            {
                ret = -1;
                free(TSL2561_lux_data);
            }
        }
    }

    return ret;
}
