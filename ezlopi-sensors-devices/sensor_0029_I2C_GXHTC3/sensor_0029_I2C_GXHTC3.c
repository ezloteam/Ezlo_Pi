#include "trace.h"

#include <math.h>
#include "ezlopi_core_cloud.h"
#include "ezlopi_hal_i2c_master.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_devices_list.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_core_device_value_updated.h"

#include "sensor_0029_I2C_GXHTC3.h"

static const float ideal_value = 65536.0f;

static int __prepare(void *arg);
static int __init(l_ezlopi_item_t *item);
static int __notify(l_ezlopi_item_t *item);
static int __get_cjson_value(l_ezlopi_item_t *item, void *arg);
static int __get_cjson_update_value(l_ezlopi_item_t *item);

static uint8_t compare_float_values(float a, float b)
{
    return (fabs(a - b) > FLOAT_EPSILON);
}

static int gxhtc3_sensor_init(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (item)
    {
        s_gxhtc3_value_t *gxhtce_val = (s_gxhtc3_value_t *)item->user_arg;
        if (gxhtce_val)
        {
            if (NULL == gxhtce_val->gxhtc3)
            {
                if (item->interface.i2c_master.enable)
                {
                    ezlopi_i2c_master_init(&item->interface.i2c_master);
                    gxhtce_val->gxhtc3 = GXHTC3_init(item->interface.i2c_master.channel, item->interface.i2c_master.address);
                    if (gxhtce_val->gxhtc3)
                    {
                        if (gxhtce_val->gxhtc3->id.status)
                        {
                            TRACE_E("GXHTC3 Chip ID: 0x%x", gxhtce_val->gxhtc3->id.id);
                        }
                        else
                        {
                            TRACE_E("GXHTC3 Chip ID not ready!");
                        }
                    }
                }
            }
            else
            {
                ret = 1;
            }
        }
        else
        {
            ret = 1;
        }
    }
    else
    {
        ret = 1;
    }

    return ret;
}
static int gxhtc3_sensor_get_cjson_value_temp(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;

    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        s_gxhtc3_value_t *value_ptr = (s_gxhtc3_value_t *)item->user_arg;

        cJSON_AddNumberToObject(cj_result, ezlopi_value_str, value_ptr->temperature);
        char *valueFormatted = ezlopi_valueformatter_float(value_ptr->temperature);
        if (valueFormatted)
        {
            cJSON_AddStringToObject(cj_result, ezlopi_valueformatted_str, valueFormatted);
            free(valueFormatted);
        }
        cJSON_AddStringToObject(cj_result, ezlopi_scale_str, scales_celsius);
    }

    return ret;
}

int sensor_0029_I2C_GXHTC3(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
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
        ret = gxhtc3_sensor_init(item);
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
        __get_cjson_update_value(item);
        break;
    }
    default:
    {
        break;
    }
    }
    return ret;
}

static int __get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;

    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        s_gxhtc3_value_t *value_ptr = (s_gxhtc3_value_t *)item->user_arg;

        if (value_ptr)
        {
            if (value_type_temperature == item->cloud_properties.value_type)
            {

                cJSON_AddNumberToObject(cj_result, ezlopi_value_str, value_ptr->temperature);
                char *valueFormatted = ezlopi_valueformatter_float(value_ptr->temperature);
                if (valueFormatted)
                {
                    cJSON_AddStringToObject(cj_result, ezlopi_valueformatted_str, valueFormatted);
                    free(valueFormatted);
                }
                cJSON_AddStringToObject(cj_result, ezlopi_scale_str, scales_celsius);
            }
            else if (value_type_humidity == item->cloud_properties.value_type)
            {
                cJSON_AddNumberToObject(cj_result, ezlopi_value_str, value_ptr->humidity);
                char *valueFormatted = ezlopi_valueformatter_float(value_ptr->humidity);
                if (valueFormatted)
                {
                    cJSON_AddStringToObject(cj_result, ezlopi_valueformatted_str, valueFormatted);
                    free(valueFormatted);
                }
                cJSON_AddStringToObject(cj_result, ezlopi_scale_str, scales_percent);
            }
        }
    }

    return ret;
}

static int __get_cjson_update_value(l_ezlopi_item_t *item)
{
    int ret = 0;

    if (item)
    {
        s_gxhtc3_value_t *value_ptr = (s_gxhtc3_value_t *)item->user_arg;

        if (value_ptr && value_ptr->gxhtc3)
        {
            if (GXHTC3_read_sensor(value_ptr->gxhtc3))
            {

                if (value_type_temperature == item->cloud_properties.value_type)
                {
                    if (compare_float_values(value_ptr->temperature, value_ptr->gxhtc3->reading_temp_c))
                    {
                        value_ptr->temperature = value_ptr->gxhtc3->reading_temp_c;
                        ezlopi_device_value_updated_from_device_v3(item);
                    }
                }
                else if (value_type_humidity == item->cloud_properties.value_type)
                {
                    if (compare_float_values(value_ptr->humidity, value_ptr->gxhtc3->reading_rh))
                    {
                        value_ptr->humidity = value_ptr->gxhtc3->reading_rh;
                        ezlopi_device_value_updated_from_device_v3(item);
                    }
                }
            }
            else
            {
                TRACE_E("Error reading from sensor");
                ret = 0;
            }
        }
    }

    return ret;
}

static void __prepare_device_cloud_properties_temp(l_ezlopi_device_t *device, cJSON *cj_device)
{
    char *device_name = NULL;
    CJSON_GET_VALUE_STRING(cj_device, "dev_name", device_name);

    ASSIGN_DEVICE_NAME_V2(device, device_name);
    device->cloud_properties.category = category_temperature;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
}

static void __prepare_device_cloud_properties_hum(l_ezlopi_device_t *device, cJSON *cj_device)
{
    char *device_name = NULL;
    CJSON_GET_VALUE_STRING(cj_device, "dev_name", device_name);

    ASSIGN_DEVICE_NAME_V2(device, device_name);
    device->cloud_properties.category = category_humidity;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
}

static void __prepare_temperature_item_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_type_str, item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_temp;
    item->cloud_properties.value_type = value_type_temperature;
    item->cloud_properties.scale = scales_celsius;
    item->cloud_properties.show = true;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    item->interface_type = EZLOPI_DEVICE_INTERFACE_I2C_MASTER;
    item->interface.i2c_master.enable = true;
    item->interface.i2c_master.channel = 0;
    item->interface.i2c_master.clock_speed = 400000;
    CJSON_GET_VALUE_INT(cj_device, ezlopi_gpio_scl_str, item->interface.i2c_master.scl);
    CJSON_GET_VALUE_INT(cj_device, ezlopi_gpio_sda_str, item->interface.i2c_master.sda);
    CJSON_GET_VALUE_INT(cj_device, "slave_addr", item->interface.i2c_master.address);
}

static void __prepare_humidity_item_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_type_str, item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_humidity;
    item->cloud_properties.value_type = value_type_humidity;
    item->cloud_properties.show = true;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.scale = scales_percent;

    item->interface_type = EZLOPI_DEVICE_INTERFACE_I2C_MASTER;
    item->interface.i2c_master.enable = false;
    item->interface.i2c_master.channel = 0;
    item->interface.i2c_master.clock_speed = 400000;
    CJSON_GET_VALUE_INT(cj_device, ezlopi_gpio_scl_str, item->interface.i2c_master.scl);
    CJSON_GET_VALUE_INT(cj_device, ezlopi_gpio_sda_str, item->interface.i2c_master.sda);
    CJSON_GET_VALUE_INT(cj_device, "slave_addr", item->interface.i2c_master.address);
}

static int __prepare(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;

    if (prep_arg && prep_arg->cjson_device)
    {

        s_gxhtc3_value_t *value_ptr = malloc(sizeof(s_gxhtc3_value_t));
        if (value_ptr)
        {
            memset(value_ptr, 0, sizeof(s_gxhtc3_value_t));
            l_ezlopi_device_t *device_temp = ezlopi_device_add_device(prep_arg->cjson_device);
            if (device_temp)
            {
                __prepare_device_cloud_properties_temp(device_temp, prep_arg->cjson_device);
                l_ezlopi_item_t *item_temperature = ezlopi_device_add_item_to_device(device_temp, sensor_0029_I2C_GXHTC3);
                if (item_temperature)
                {
                    item_temperature->cloud_properties.device_id = device_temp->cloud_properties.device_id;
                    __prepare_temperature_item_properties(item_temperature, prep_arg->cjson_device);
                    value_ptr->temperature = ideal_value;
                    item_temperature->user_arg = (void *)value_ptr;
                }
                else
                {
                    ezlopi_device_free_device(device_temp);
                    free(value_ptr);
                }
            }
            l_ezlopi_device_t *device_hum = ezlopi_device_add_device(prep_arg->cjson_device);
            if (device_hum)
            {
                __prepare_device_cloud_properties_hum(device_hum, prep_arg->cjson_device);
                l_ezlopi_item_t *item_humdity = ezlopi_device_add_item_to_device(device_hum, sensor_0029_I2C_GXHTC3);
                if (item_humdity)
                {
                    item_humdity->cloud_properties.device_id = device_hum->cloud_properties.device_id;
                    __prepare_humidity_item_properties(item_humdity, prep_arg->cjson_device);
                    value_ptr->humidity = ideal_value;
                    item_humdity->user_arg = (void *)value_ptr;
                }
                else
                {
                    ezlopi_device_free_device(device_hum);
                    free(value_ptr);
                }
            }
        }
    }
    return ret;
}
