#include "cJSON.h"
#include "trace.h"
#include "ezlopi_actions.h"
#include "ezlopi_timer.h"
#include "items.h"

#include "ezlopi_cloud.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_i2c_master.h"
#include "0044_sensor_I2C_TSL256_luminosity.h"

extern bool Check_PARTID(s_ezlopi_i2c_master_t *i2c_master);
extern void sensor_0044_tsl2561_configure_device(s_ezlopi_i2c_master_t *i2c_master);
extern uint32_t tsl2561_get_intensity_value(s_ezlopi_i2c_master_t *i2c_master);

static int __prepare(void *arg);
static int __init(l_ezlopi_item_t *item);
static int __get_cjson_value(l_ezlopi_item_t *item, void *arg);
static int __notify(l_ezlopi_item_t *item);

int sensor_TSL256_luminosity_v3(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
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
    static int count = 0;

    if (3 == ++count)
    {
        ezlopi_device_value_updated_from_device_v3(item);
        count = 0;
    }

    return ret;
}

static int __get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;
    cJSON *cjson_properties = (cJSON *)arg;
    char valueFormatted[20];
    if (cjson_properties)
    {
        uint32_t illuminance_value = tsl2561_get_intensity_value(&item->interface.i2c_master);
        snprintf(valueFormatted, 20, "%d", illuminance_value);
        cJSON_AddStringToObject(cjson_properties, "valueFormatted", valueFormatted);
        cJSON_AddNumberToObject(cjson_properties, "values", illuminance_value);
        ret = 1;
    }
    return ret;
}

static int __init(l_ezlopi_item_t *item)
{
    int ret = 0;

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

static void __prepare_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_lux;
    item->cloud_properties.value_type = value_type_illuminance;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = scales_lux;

    CJSON_GET_VALUE_INT(cj_device, "gpio_sda", item->interface.i2c_master.sda);
    CJSON_GET_VALUE_INT(cj_device, "gpio_scl", item->interface.i2c_master.scl);

    item->interface.i2c_master.enable = true;
    item->interface.i2c_master.clock_speed = 100000;
    item->interface.i2c_master.address = TSL2561_ADDRESS;
}

static int __prepare(void *arg)
{
    int ret = 0;

    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (prep_arg && prep_arg->cjson_device)
    {
        l_ezlopi_device_t *tsl256_device = ezlopi_device_add_device();
        if (tsl256_device)
        {
            __prepare_device_cloud_properties(tsl256_device, prep_arg->cjson_device);
            l_ezlopi_item_t *tsl256_item = ezlopi_device_add_item_to_device(tsl256_device, sensor_TSL256_luminosity_v3);
            if (tsl256_item)
            {
                __prepare_item_cloud_properties(tsl256_item, prep_arg->cjson_device);
            }
            else
            {
                ezlopi_device_free_device(tsl256_device);
            }
        }
        else
        {
            ezlopi_device_free_device(tsl256_device);
        }
    }

    return ret;
}