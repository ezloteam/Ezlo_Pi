#include "cJSON.h"
#include "math.h"
#include "trace.h"
#include "ezlopi_core_timer.h"
#include "ezlopi_hal_i2c_master.h"
#include "ezlopi_core_devices_list.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_core_device_value_updated.h"
#include "sensor_0006_I2C_ADXL345.h"

static int __prepare(void *arg);
static int __init(l_ezlopi_item_t *item);
static int __get_cjson_value(l_ezlopi_item_t *item, void *arg);
static int __notify(l_ezlopi_item_t *item);
static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_item_cloud_properties(l_ezlopi_item_t *item, void *user_data);
static void __prepare_item_interface_properties(l_ezlopi_item_t *item, cJSON *cj_device);

int sensor_0006_I2C_ADXL345(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
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

static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    char *device_name = NULL;
    CJSON_GET_VALUE_STRING(cj_device, "dev_name", device_name);
    ASSIGN_DEVICE_NAME_V2(device, device_name);
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
    device->cloud_properties.category = category_level_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
    device->cloud_properties.device_type = dev_type_sensor;
}
static void __prepare_item_cloud_properties(l_ezlopi_item_t *item, void *user_data)
{
    if (item)
    {
        item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
        item->cloud_properties.has_getter = true;
        item->cloud_properties.has_setter = false;
        item->cloud_properties.show = true;
        item->cloud_properties.scale = scales_meter_per_square_second;
        item->cloud_properties.value_type = value_type_acceleration;
        item->user_arg = user_data;
    }
}
static void __prepare_item_interface_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    if (item && cj_device)
    {
        if (ezlopi_item_name_acceleration_x_axis == item->cloud_properties.item_name)
        {
            item->interface.i2c_master.enable = true;
            CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type);
            CJSON_GET_VALUE_INT(cj_device, "gpio_sda", item->interface.i2c_master.sda);
            CJSON_GET_VALUE_INT(cj_device, "gpio_scl", item->interface.i2c_master.scl);
            CJSON_GET_VALUE_INT(cj_device, "slave_addr", item->interface.i2c_master.address);
            item->interface.i2c_master.clock_speed = 100000;
            if (NULL == item->interface.i2c_master.address)
            {
                item->interface.i2c_master.address = ADXL345_ADDR;
            }
        }
        else
        {
            item->interface.i2c_master.enable = false;
        }
    }
}

static int __prepare(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (prep_arg && prep_arg->cjson_device)
    {
        cJSON *cj_device = prep_arg->cjson_device;
        s_adxl345_data_t *user_data = (s_adxl345_data_t *)malloc(sizeof(s_adxl345_data_t));
        if (NULL != user_data)
        {
            memset(user_data, 0, sizeof(s_adxl345_data_t));
            l_ezlopi_device_t *adxl345_device = ezlopi_device_add_device(cj_device);
            if (adxl345_device)
            {
                __prepare_device_cloud_properties(adxl345_device, cj_device);
                l_ezlopi_item_t *x_item = ezlopi_device_add_item_to_device(adxl345_device, sensor_0006_I2C_ADXL345);
                if (x_item)
                {
                    x_item->cloud_properties.item_name = ezlopi_item_name_acceleration_x_axis;
                    __prepare_item_cloud_properties(x_item, user_data);
                    __prepare_item_interface_properties(x_item, cj_device);
                }
                l_ezlopi_item_t *y_item = ezlopi_device_add_item_to_device(adxl345_device, sensor_0006_I2C_ADXL345);
                if (y_item)
                {
                    y_item->cloud_properties.item_name = ezlopi_item_name_acceleration_y_axis;
                    __prepare_item_cloud_properties(y_item, user_data);
                    __prepare_item_interface_properties(y_item, cj_device);
                }
                l_ezlopi_item_t *z_item = ezlopi_device_add_item_to_device(adxl345_device, sensor_0006_I2C_ADXL345);
                if (z_item)
                {
                    z_item->cloud_properties.item_name = ezlopi_item_name_acceleration_z_axis;
                    __prepare_item_cloud_properties(z_item, user_data);
                    __prepare_item_interface_properties(z_item, cj_device);
                }

                if ((NULL == x_item) && (NULL == y_item) && (NULL == z_item))
                {
                    ezlopi_device_free_device(adxl345_device);
                    free(user_data);
                }
            }
            else
            {
                ezlopi_device_free_device(adxl345_device);
                free(user_data);
            }
            ret = 1;
        }
    }

    return ret;
}

static int __init(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (item)
    {
        if (item->interface.i2c_master.enable)
        {
            ezlopi_i2c_master_init(&item->interface.i2c_master);
            if (0 == __adxl345_configure_device(item))
            {
                TRACE_I("Configuration Complete...");
            }
        }
    }
    return ret;
}

static int __get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;
    cJSON *cj_result = (cJSON *)arg;
    if (cj_result && item)
    {
        float acceleration_value;
        s_adxl345_data_t *user_data = (s_adxl345_data_t *)item->user_arg;
        if (ezlopi_item_name_acceleration_x_axis == item->cloud_properties.item_name)
        {
            acceleration_value = (user_data->acc_x * ADXL345_CONVERTER_FACTOR_MG_TO_G * ADXL345_STANDARD_G_TO_ACCEL_CONVERSION_VALUE);
            cJSON_AddNumberToObject(cj_result, "value", acceleration_value);
            char *valueFormatted = ezlopi_valueformatter_float(acceleration_value);
            cJSON_AddStringToObject(cj_result, "valueFormatted", valueFormatted);
            free(valueFormatted);
        }
        if (ezlopi_item_name_acceleration_y_axis == item->cloud_properties.item_name)
        {
            acceleration_value = (user_data->acc_y * ADXL345_CONVERTER_FACTOR_MG_TO_G * ADXL345_STANDARD_G_TO_ACCEL_CONVERSION_VALUE);
            cJSON_AddNumberToObject(cj_result, "value", acceleration_value);
            char *valueFormatted = ezlopi_valueformatter_float(acceleration_value);
            cJSON_AddStringToObject(cj_result, "valueFormatted", valueFormatted);
            free(valueFormatted);
        }
        if (ezlopi_item_name_acceleration_z_axis == item->cloud_properties.item_name)
        {
            acceleration_value = (user_data->acc_z * ADXL345_CONVERTER_FACTOR_MG_TO_G * ADXL345_STANDARD_G_TO_ACCEL_CONVERSION_VALUE);
            cJSON_AddNumberToObject(cj_result, "value", acceleration_value);
            char *valueFormatted = ezlopi_valueformatter_float(acceleration_value);
            cJSON_AddStringToObject(cj_result, "valueFormatted", valueFormatted);
            free(valueFormatted);
        }
        ret = 1;
    }
    return ret;
}

static int __notify(l_ezlopi_item_t *item)
{
    int ret = 0;
    static float __prev[3] = {0};
    if (item)
    {
        s_adxl345_data_t *user_data = (s_adxl345_data_t *)item->user_arg;
        if (ezlopi_item_name_acceleration_x_axis == item->cloud_properties.item_name)
        {
            __prev[0] = user_data->acc_x;
            __adxl345_get_axis_value(item);
            if (fabs((__prev[0] - user_data->acc_x) > 0.5))
            {
                ezlopi_device_value_updated_from_device_v3(item);
            }
        }
        if (ezlopi_item_name_acceleration_y_axis == item->cloud_properties.item_name)
        {
            __prev[1] = user_data->acc_y;
            if (fabs((__prev[1] - user_data->acc_x) > 0.5))
            {
                ezlopi_device_value_updated_from_device_v3(item);
            }
        }
        if (ezlopi_item_name_acceleration_z_axis == item->cloud_properties.item_name)
        {
            __prev[2] = user_data->acc_z;
            if (fabs((__prev[2] - user_data->acc_x) > 0.5))
            {
                ezlopi_device_value_updated_from_device_v3(item);
            }
        }
        ret = 1;
    }
    return ret;
}
