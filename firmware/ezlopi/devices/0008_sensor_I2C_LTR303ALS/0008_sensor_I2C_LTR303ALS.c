
#include <string.h>
#include "sdkconfig.h"

#include "cJSON.h"
#include "trace.h"
#include "ezlopi_timer.h"
#include "items.h"

#include "ezlopi_cloud.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_devices.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_i2c_master.h"
#include "ezlopi_spi_master.h"

#include "0008_sensor_I2C_LTR303ALS.h"
#include "ALS_LTR303.h"
#include "stdlib.h"

#if 0 // v2.x
static int count = 5;

static int ltr303_prepare_sensor(void *arg);
static s_ezlopi_device_properties_t *ltr303_ambient_sensor_prepare_properties(cJSON *cjson_device);
static int ltr303_ambient_sensor_init(s_ezlopi_device_properties_t *properties);
static int ltr303_ambient_sensor_update_values(s_ezlopi_device_properties_t *properties);
static int ltr303_ambient_sensor_get_value_cjson(s_ezlopi_device_properties_t *properties, void *arg);
#endif

static int __prepare(void *arg);
static int __init(l_ezlopi_item_t *item);
static int __notify(l_ezlopi_item_t *item);
static int __get_value_cjson(l_ezlopi_item_t *item, void *arg);

int sensor_0008_I2C_LTR303ALS_v3(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
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
    char valueFormatted[20];
    if (cj_param && als_ltr303_data)
    {
        snprintf(valueFormatted, 20, "%.3f", als_ltr303_data->lux);
        cJSON_AddStringToObject(cj_param, "valueFormatted", valueFormatted);
        cJSON_AddNumberToObject(cj_param, "value", als_ltr303_data->lux);
    }

    return ret;
}

static int __notify(l_ezlopi_item_t *item)
{
    int ret = 0;

    static int count = 0;
    if (3 == ++count)
    {
        ltr303_data_t *als_ltr303_data = (ltr303_data_t *)item->user_arg;
        if (als_ltr303_data)
        {
            if (ESP_OK == ltr303_get_val(als_ltr303_data))
            {
                ezlopi_device_value_updated_from_device_v3(item);
            }
            count = 0;
        }
    }

    return ret;
}

static int __init(l_ezlopi_item_t *item)
{
    int ret = 0;

    ltr303_setup(item->interface.i2c_master.sda, item->interface.i2c_master.scl, true);

    return ret;
}

static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_params)
{
    char *device_name = NULL;
    CJSON_GET_VALUE_STRING(cj_params, "dev_name", device_name);
    ASSIGN_DEVICE_NAME_V2(device, device_name);

    device->cloud_properties.category = category_light_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
}

static void __prepare_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_param)
{
    CJSON_GET_VALUE_INT(cj_param, "dev_type", item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_lux;
    item->cloud_properties.value_type = value_type_illuminance;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = scales_lux;

    CJSON_GET_VALUE_INT(cj_param, "gpio_sda", item->interface.i2c_master.sda);
    CJSON_GET_VALUE_INT(cj_param, "gpio_scl", item->interface.i2c_master.scl);

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
        l_ezlopi_device_t *als_ltr303_device = ezlopi_device_add_device();
        if (als_ltr303_device)
        {
            __prepare_device_cloud_properties(als_ltr303_device, prep_arg->cjson_device);
            l_ezlopi_item_t *als_ltr303_item = ezlopi_device_add_item_to_device(als_ltr303_device, sensor_0008_I2C_LTR303ALS_v3);
            if (als_ltr303_item)
            {
                __prepare_item_cloud_properties(als_ltr303_item, prep_arg->cjson_device);
            }
            else
            {
                ezlopi_device_free_device(als_ltr303_device);
            }
        }
        else
        {
            ezlopi_device_free_device(als_ltr303_device);
        }
    }

    return ret;
}

#if 0 // v2.x
static int ltr303_ambient_sensor_update_values(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;
    ltr303_data_t *ltr303_lux_val = (ltr303_data_t *)properties->user_arg;

    if (ltr303_lux_val)
    {
        if (ltr303_get_val(ltr303_lux_val) == ESP_OK)
        {
            TRACE_D(" lux: %f\n", ltr303_lux_val->lux);
        }
    }
    return ret;
}

static int ltr303_ambient_sensor_get_value_cjson(s_ezlopi_device_properties_t *properties, void *arg)
{
    int ret = 0;

    cJSON *cjson_properties = (cJSON *)arg;
    ltr303_data_t *ltr303_lux_val = (ltr303_data_t *)properties->user_arg;

    if (cjson_properties && ltr303_lux_val)
    {
        char formatted_value[10];
        snprintf(formatted_value, 10, "%.2f", ltr303_lux_val->lux);
        cJSON_AddStringToObject(cjson_properties, "valueFormatted", formatted_value);
        cJSON_AddNumberToObject(cjson_properties, "value", ltr303_lux_val->lux);
        cJSON_AddStringToObject(cjson_properties, "scale", "lux");
    }

    return ret;
}
#endif