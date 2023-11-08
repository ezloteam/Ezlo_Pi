#include <cJSON.h>
#include <items.h>
#include <math.h>

#include "trace.h"

#include "ezlopi_timer.h"
#include "ezlopi_cloud.h"
#include "ezlopi_actions.h"
#include "ezlopi_i2c_master.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_valueformatter.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_device_value_updated.h"

#include "sensor_0044_I2C_TSL256_luminosity.h"
//-----------------------------------------------------------------------

static int __prepare(void *arg);
static int __init(l_ezlopi_item_t *item);
static int __get_cjson_value(l_ezlopi_item_t *item, void *arg);
static int __notify(l_ezlopi_item_t *item);
static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_data);

int sensor_0044_I2C_TSL256_luminosity(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
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

static int __notify(l_ezlopi_item_t *item)
{
    int ret = 0;

    // Allow only significant changes in values to be posted
    double new_value = (double)tsl2561_get_intensity_value(&item->interface.i2c_master);
    TSL256_lum_t *TSL2561_lux_data = (TSL256_lum_t *)item->user_arg;
    if (fabs((double)(TSL2561_lux_data->lux_val) - new_value) > 0.0001)
    {
        // Allow only significant changes in values to be posted
        double new_value = (double)tsl2561_get_intensity_value(&item->interface.i2c_master);
        TSL256_lum_t *TSL2561_lux_data = (TSL256_lum_t *)item->user_arg;
        if (fabs((double)(TSL2561_lux_data->lux_val) - new_value) > 0.0001)
        {
            ezlopi_device_value_updated_from_device_v3(item);
            TSL2561_lux_data->lux_val = (uint32_t)new_value;
        }
    }

    return ret;
}

static int __get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;
    cJSON *cjson_properties = (cJSON *)arg;
    if (cjson_properties)
    {
        TSL256_lum_t *illuminance_value = ((TSL256_lum_t *)item->user_arg);
        char *valueFormatted = ezlopi_valueformatter_int((int)(illuminance_value->lux_val));
        if (valueFormatted)
        {
            cJSON_AddStringToObject(cjson_properties, "valueFormatted", valueFormatted);
            free(valueFormatted);
        }
        cJSON_AddNumberToObject(cjson_properties, "values", (int)illuminance_value->lux_val);
        cJSON_AddStringToObject(cjson_properties, "scales", item->cloud_properties.scale);
        ret = 1;
    }
    return ret;
}

static int __init(l_ezlopi_item_t *item)
{
    int ret = 0;

    if (item->interface.i2c_master.enable)
    {
        ezlopi_i2c_master_init(&item->interface.i2c_master);
        TRACE_B("I2C channel is %d", item->interface.i2c_master.channel);
        if (Check_PARTID(&item->interface.i2c_master))
        {
            TRACE_B("TSL561 initialization finished.........");
            sensor_0044_tsl2561_configure_device(&item->interface.i2c_master);
        }
        else
        {
            TRACE_E("TSL561 not found!....... Please Restart!! or Check your I2C connection...");
        }
    }
    return ret;
}

static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    char *device_name = NULL;
    CJSON_GET_VALUE_STRING(cj_device, "dev_name", device_name);
    ASSIGN_DEVICE_NAME_V2(device, device_name);
    device->cloud_properties.category = category_light_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
}

static void __prepare_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_data)
{
    CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_lux;
    item->cloud_properties.value_type = value_type_illuminance;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = scales_lux;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    CJSON_GET_VALUE_INT(cj_device, "gpio_sda", item->interface.i2c_master.sda);
    CJSON_GET_VALUE_INT(cj_device, "gpio_scl", item->interface.i2c_master.scl);
    CJSON_GET_VALUE_INT(cj_device, "slave_addr", item->interface.i2c_master.address);

    item->interface.i2c_master.enable = true;
    item->interface.i2c_master.clock_speed = 100000;
    if (NULL == item->interface.i2c_master.address)
    {
        item->interface.i2c_master.address = TSL2561_ADDRESS;
    }
    // passing the custom data_structure
    item->user_arg = user_data;
}

static int __prepare(void *arg)
{
    int ret = 0;

    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (prep_arg && prep_arg->cjson_device)
    {
        TSL256_lum_t *TSL2561_lux_data = (TSL256_lum_t *)malloc(sizeof(TSL256_lum_t));
        if (NULL != TSL2561_lux_data)
        {
            memset(TSL2561_lux_data, 0, sizeof(TSL256_lum_t));
            l_ezlopi_device_t *tsl256_device = ezlopi_device_add_device();
            if (tsl256_device)
            {
                __prepare_device_cloud_properties(tsl256_device, prep_arg->cjson_device);

                l_ezlopi_item_t *tsl256_item = ezlopi_device_add_item_to_device(tsl256_device, sensor_0044_I2C_TSL256_luminosity);
                if (tsl256_item)
                {
                    __prepare_item_cloud_properties(tsl256_item, prep_arg->cjson_device, TSL2561_lux_data);
                }
                else
                {
                    ezlopi_device_free_device(tsl256_device);
                    free(TSL2561_lux_data);
                }
            }
            else
            {
                ezlopi_device_free_device(tsl256_device);
                free(TSL2561_lux_data);
            }
            ret = 1;
        }
    }

    return ret;
}
