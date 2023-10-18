#include "ezlopi_cloud.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "ezlopi_cloud_scales_str.h"
#include "ezlopi_valueformatter.h"

#include "ezlopi_i2c_master.h"
#include "trace.h"

#include "sensor_0029_I2C_GXHTC3.h"

typedef struct s_gxhtc3_value
{
    float temperature;
    float humidity;
} s_gxhtc3_value_t;

static const float ideal_value = 65536.0f;

static int __prepare(void *arg);
static int __init(l_ezlopi_item_t *item);
static int __notify(l_ezlopi_item_t *item);
static int __get_cjson_value(l_ezlopi_item_t *item, void *arg);

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
        ret = __init(item);
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

static void __read_value_from_sensor(l_ezlopi_item_t *item)
{
    if (item->user_arg)
    {
        s_gxhtc3_value_t *value_ptr = (s_gxhtc3_value_t *)item->user_arg;
        uint8_t wakeup_cmd[] = {0x35, 0x17};
        uint8_t temp_measure_cmd[] = {0x7C, 0xA2}; // temperature first-clock stretching

        ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, wakeup_cmd, 2);
        vTaskDelay(1);
        ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, temp_measure_cmd, 2);
        vTaskDelay(5);
        uint8_t read_buffer[6];
        ezlopi_i2c_master_read_from_device(&item->interface.i2c_master, read_buffer, 6);
        dump("read_buffer", read_buffer, 0, 6);

        value_ptr->temperature = 100.0 * (read_buffer[0] << 8 | read_buffer[1]) / 65536.0;
        value_ptr->humidity = -45.0 + 175 * (read_buffer[0] << 8 | read_buffer[1]) / 65536.0;

        TRACE_B("Temperature: %f *C", value_ptr->temperature);
        TRACE_B("Humidity: %f %%", value_ptr->humidity);
    }
}

static int __get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;

    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        s_gxhtc3_value_t *value_ptr = (s_gxhtc3_value_t *)item->user_arg;

        if (value_type_temperature == item->cloud_properties.value_type)
        {
            if (ideal_value == value_ptr->temperature)
            {
                __read_value_from_sensor(item);
            }

            cJSON_AddNumberToObject(cj_result, "value", value_ptr->temperature);
            char *valueFormatted = ezlopi_valueformatter_float(value_ptr->temperature);
            cJSON_AddStringToObject(cj_result, "valueFormatted", valueFormatted);
            free(valueFormatted);
            cJSON_AddStringToObject(cj_result, "scale", "celsius");

            value_ptr->temperature = ideal_value;
        }
        else if (value_type_humidity == item->cloud_properties.value_type)
        {
            if (ideal_value == value_ptr->humidity)
            {
                __read_value_from_sensor(item);
            }

            cJSON_AddNumberToObject(cj_result, "value", value_ptr->humidity);
            char *valueFormatted = ezlopi_valueformatter_float(value_ptr->humidity);
            cJSON_AddStringToObject(cj_result, "valueFormatted", valueFormatted);
            free(valueFormatted);
            cJSON_AddStringToObject(cj_result, "scale", "percent");

            value_ptr->humidity = ideal_value;
        }
    }

    return ret;
}

static int __init(l_ezlopi_item_t *item)
{
    return ezlopi_i2c_master_init(&item->interface.i2c_master);
}

static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    char *device_name = NULL;
    CJSON_GET_VALUE_STRING(cj_device, "dev_name", device_name);

    ASSIGN_DEVICE_NAME_V2(device, device_name);
    device->cloud_properties.category = category_humidity;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
}

static void __prepare_temperature_item_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_temp;
    item->cloud_properties.value_type = value_type_temperature;
    item->cloud_properties.scale = scales_celsius;
    item->cloud_properties.show = true;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    item->interface_type = EZLOPI_DEVICE_INTERFACE_I2C_MASTER;
    item->interface.i2c_master.enable = 1;
    item->interface.i2c_master.channel = 0;
    item->interface.i2c_master.clock_speed = 100000;
    CJSON_GET_VALUE_INT(cj_device, "gpio_scl", item->interface.i2c_master.scl);
    CJSON_GET_VALUE_INT(cj_device, "gpio_sda", item->interface.i2c_master.sda);
    CJSON_GET_VALUE_INT(cj_device, "slave_addr", item->interface.i2c_master.address);
}

static void __prepare_humidity_item_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    CJSON_GET_VALUE_INT(cj_device, "dev_type", item->interface_type);
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_humidity;
    item->cloud_properties.value_type = value_type_humidity;
    item->cloud_properties.show = true;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    item->interface_type = EZLOPI_DEVICE_INTERFACE_I2C_MASTER;
    item->interface.i2c_master.enable = 1;
    item->interface.i2c_master.channel = 0;
    item->interface.i2c_master.clock_speed = 100000;
    CJSON_GET_VALUE_INT(cj_device, "gpio_scl", item->interface.i2c_master.scl);
    CJSON_GET_VALUE_INT(cj_device, "gpio_sda", item->interface.i2c_master.sda);
    CJSON_GET_VALUE_INT(cj_device, "slave_addr", item->interface.i2c_master.address);
}

static int __prepare(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;

    if (prep_arg && prep_arg->cjson_device)
    {
        l_ezlopi_device_t *device = ezlopi_device_add_device();
        if (device)
        {
            __prepare_device_cloud_properties(device, prep_arg->cjson_device);
            l_ezlopi_item_t *item_temperature = ezlopi_device_add_item_to_device(device, sensor_0029_I2C_GXHTC3);
            if (item_temperature)
            {
                __prepare_temperature_item_properties(item_temperature, prep_arg->cjson_device);
            }

            l_ezlopi_item_t *item_humdity = ezlopi_device_add_item_to_device(device, sensor_0029_I2C_GXHTC3);
            if (item_humdity)
            {
                __prepare_humidity_item_properties(item_humdity, prep_arg->cjson_device);
            }

            if ((NULL == item_humdity) && (NULL == item_temperature))
            {
                ezlopi_device_free_device(device);
            }
            else
            {
                s_gxhtc3_value_t *value_ptr = malloc(sizeof(s_gxhtc3_value_t));
                if (value_ptr)
                {
                    value_ptr->humidity = ideal_value;
                    value_ptr->temperature = ideal_value;
                    if (item_humdity)
                    {
                        item_humdity->user_arg = (void *)value_ptr;
                    }
                    if (item_temperature)
                    {
                        item_temperature->user_arg = (void *)value_ptr;
                    }
                }
            }
        }
    }
    return ret;
}

static int __notify(l_ezlopi_item_t *item)
{
    return ezlopi_device_value_updated_from_device_v3(item);
}
