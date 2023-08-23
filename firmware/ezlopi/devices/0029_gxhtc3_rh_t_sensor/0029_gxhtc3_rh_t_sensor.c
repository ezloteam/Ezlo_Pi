#include "ezlopi_cloud.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "ezlopi_cloud_scales_str.h"

#include "ezlopi_i2c_master.h"
#include "trace.h"

#include "0029_gxhtc3_rh_t_sensor.h"

static int __preapare(void *arg);
static int __init(l_ezlopi_item_t *item);
static int __notify(l_ezlopi_item_t *item);
static int __get_cjson_value(l_ezlopi_item_t *item, void *arg);

int gxhtc3_rht_sensor_v3(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = __preapare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = __init(item);
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

#if 1
static void wgxhtc3_sensor_prepare(void *arg);
static int gxhtc3_notify(s_ezlopi_device_properties_t *properties, void *args);
static s_ezlopi_device_properties_t *wgxhtc3_sensor_prepare_temperature(cJSON *cjson_device);
static s_ezlopi_device_properties_t *wgxhtc3_sensor_prepare_relative_humidity(cJSON *cjson_device);
static void wgxhtc3_sensor_init(s_ezlopi_device_properties_t *properties, void *user_arg);

static void wgxhtc3_sensor_prepare_v3(void *arg);
static void wgxhtc3_sensor_prepare_device_cloud_properties_temperature(l_ezlopi_device_t *device, cJSON *cjson_device);
static void wgxhtc3_sensor_prepare_device_cloud_properties_humidity(l_ezlopi_device_t *device, cJSON *cjson_device);
static void wgxhtc3_sensor_prepare_temperature_item_properties(l_ezlopi_item_t *item, cJSON *cjson_device);
static void wgxhtc3_sensor_prepare_humidity_item_properties(l_ezlopi_item_t *item, cJSON *cjson_device);

int gxhtc3_rh_t_sensor(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg)
{
    int ret = 0;
    static uint32_t count;

    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        wgxhtc3_sensor_prepare(arg);
        wgxhtc3_sensor_prepare_v3(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        wgxhtc3_sensor_init(properties, NULL);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        gxhtc3_notify(properties, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        if (count++ > 5) // every 5 seconds
        {
            ezlopi_device_value_updated_from_device(properties);
            count = 0;
        }
        break;
    }
    default:
        break;
    }

    return ret;
}

static float temperature = 0.0f;
static float relative_humidity = 0.0f;

static void wgxhtc3_sensor_read_sensor_data(s_ezlopi_device_properties_t *properties)
{
    uint8_t wakeup_cmd[] = {0x35, 0x17};
    uint8_t temp_measure_cmd[] = {0x7C, 0xA2}; // temperature first-clock stretching

    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, wakeup_cmd, 2);
    vTaskDelay(100);
    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, temp_measure_cmd, 2);
    vTaskDelay(500);
    uint8_t read_buffer[6];
    ezlopi_i2c_master_read_from_device(&properties->interface.i2c_master, read_buffer, 6);
    dump("read_buffer", read_buffer, 0, 6);

    temperature = 100.0 * (read_buffer[0] << 8 | read_buffer[1]) / 65536.0;
    relative_humidity = -45.0 + 175 * (read_buffer[0] << 8 | read_buffer[1]) / 65536.0;

    TRACE_B("Temperature: %f *C", temperature);
    TRACE_B("Humidity: %f %%", relative_humidity);
}

static int gxhtc3_notify(s_ezlopi_device_properties_t *properties, void *args)
{
    wgxhtc3_sensor_read_sensor_data(properties);
    cJSON *cjson_properties = (cJSON *)args;

    if (cjson_properties)
    {
        if (category_temperature == properties->ezlopi_cloud.category)
        {
            TRACE_D("Temperature is: %f *C", temperature);
            cJSON_AddNumberToObject(cjson_properties, "value", temperature);
            cJSON_AddStringToObject(cjson_properties, "scale", "celsius");
        }
        if (category_humidity == properties->ezlopi_cloud.category)
        {
            TRACE_D("Humidity is: %f %%", relative_humidity);
            cJSON_AddNumberToObject(cjson_properties, "value", relative_humidity);
            cJSON_AddStringToObject(cjson_properties, "scale", "percent");
        }
    }

    return 1;
}

static void wgxhtc3_sensor_prepare_v3(void *arg)
{
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;

    if (prep_arg && prep_arg->cjson_device)
    {
        l_ezlopi_device_t *device = ezlopi_device_add_device();
        if (device)
        {
            wgxhtc3_sensor_prepare_device_cloud_properties_temperature(device, prep_arg->cjson_device);
            l_ezlopi_item_t *item = ezlopi_device_add_item_to_device(device, prep_arg->device->func);
            if (item)
            {
                wgxhtc3_sensor_prepare_temperature_item_properties(item, prep_arg->cjson_device);
            }
            else
            {
                ezlopi_device_free_device(device);
            }
        }

        device = ezlopi_device_add_device();
        if (device)
        {
            wgxhtc3_sensor_prepare_device_cloud_properties_humidity(device, prep_arg->cjson_device);
            l_ezlopi_item_t *item = ezlopi_device_add_item_to_device(device, prep_arg->device->func);
            if (item)
            {
                wgxhtc3_sensor_prepare_humidity_item_properties(item, prep_arg->cjson_device);
            }
            else
            {
                ezlopi_device_free_device(device);
            }
        }
    }
}

static void wgxhtc3_sensor_prepare_device_cloud_properties_temperature(l_ezlopi_device_t *device, cJSON *cjson_device)
{
    char *device_name = NULL;
    CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);

    ASSIGN_DEVICE_NAME_V2(device, device_name);
    device->cloud_properties.category = category_temperature;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
}

static void wgxhtc3_sensor_prepare_device_cloud_properties_humidity(l_ezlopi_device_t *device, cJSON *cjson_device)
{
    char *device_name = NULL;
    CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);

    ASSIGN_DEVICE_NAME_V2(device, device_name);
    device->cloud_properties.category = category_humidity;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
}

static void wgxhtc3_sensor_prepare_temperature_item_properties(l_ezlopi_item_t *item, cJSON *cjson_device)
{
    int tmp_var = 0;
    CJSON_GET_VALUE_INT(cjson_device, "dev_type", item->interface_type);
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
    CJSON_GET_VALUE_INT(cjson_device, "gpio_scl", item->interface.i2c_master.scl);
    CJSON_GET_VALUE_INT(cjson_device, "gpio_sda", item->interface.i2c_master.sda);
    CJSON_GET_VALUE_INT(cjson_device, "slave_addr", item->interface.i2c_master.address);
}

static void wgxhtc3_sensor_prepare_humidity_item_properties(l_ezlopi_item_t *item, cJSON *cjson_device)
{
    int tmp_var = 0;
    CJSON_GET_VALUE_INT(cjson_device, "dev_type", item->interface_type);
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
    CJSON_GET_VALUE_INT(cjson_device, "gpio_scl", item->interface.i2c_master.scl);
    CJSON_GET_VALUE_INT(cjson_device, "gpio_sda", item->interface.i2c_master.sda);
    CJSON_GET_VALUE_INT(cjson_device, "slave_addr", item->interface.i2c_master.address);
}

static void wgxhtc3_sensor_prepare(void *arg)
{
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;

    if (prep_arg && prep_arg->cjson_device)
    {
        s_ezlopi_device_properties_t *wgxhtc3_temp_prop = wgxhtc3_sensor_prepare_temperature(prep_arg->cjson_device);
        s_ezlopi_device_properties_t *wgxhtc3_humid_prop = wgxhtc3_sensor_prepare_relative_humidity(prep_arg->cjson_device);

        if (wgxhtc3_temp_prop)
        {
            ezlopi_devices_list_add(prep_arg->device, wgxhtc3_temp_prop, NULL);
        }

        if (wgxhtc3_humid_prop)
        {
            ezlopi_devices_list_add(prep_arg->device, wgxhtc3_humid_prop, NULL);
        }
    }
}

static s_ezlopi_device_properties_t *wgxhtc3_sensor_prepare_temperature(cJSON *cjson_device)
{
    s_ezlopi_device_properties_t *gxhtc3_properties = malloc(sizeof(s_ezlopi_device_properties_t));
    if (gxhtc3_properties)
    {
        memset(gxhtc3_properties, 0, sizeof(s_ezlopi_device_properties_t));

        char *device_name = NULL;
        CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
        ASSIGN_DEVICE_NAME(gxhtc3_properties, device_name);
        gxhtc3_properties->ezlopi_cloud.category = category_temperature;
        gxhtc3_properties->ezlopi_cloud.subcategory = subcategory_not_defined;
        gxhtc3_properties->ezlopi_cloud.item_name = ezlopi_item_name_temp;
        gxhtc3_properties->ezlopi_cloud.device_type = dev_type_sensor;
        gxhtc3_properties->ezlopi_cloud.value_type = value_type_temperature;
        gxhtc3_properties->ezlopi_cloud.has_getter = true;
        gxhtc3_properties->ezlopi_cloud.has_setter = false;
        gxhtc3_properties->ezlopi_cloud.reachable = true;
        gxhtc3_properties->ezlopi_cloud.battery_powered = false;
        gxhtc3_properties->ezlopi_cloud.show = true;
        gxhtc3_properties->ezlopi_cloud.room_name[0] = '\0';
        gxhtc3_properties->ezlopi_cloud.device_id = ezlopi_cloud_generate_device_id();
        gxhtc3_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
        gxhtc3_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();

        gxhtc3_properties->interface_type = EZLOPI_DEVICE_INTERFACE_I2C_MASTER;
        gxhtc3_properties->interface.i2c_master.enable = 1;
        gxhtc3_properties->interface.i2c_master.channel = 0;
        gxhtc3_properties->interface.i2c_master.clock_speed = 100000;
        CJSON_GET_VALUE_INT(cjson_device, "gpio_scl", gxhtc3_properties->interface.i2c_master.scl);
        CJSON_GET_VALUE_INT(cjson_device, "gpio_sda", gxhtc3_properties->interface.i2c_master.sda);
        CJSON_GET_VALUE_INT(cjson_device, "slave_addr", gxhtc3_properties->interface.i2c_master.address);
    }

    return gxhtc3_properties;
}

static s_ezlopi_device_properties_t *wgxhtc3_sensor_prepare_relative_humidity(cJSON *cjson_device)
{
    s_ezlopi_device_properties_t *gxhtc3_properties = malloc(sizeof(s_ezlopi_device_properties_t));
    if (gxhtc3_properties)
    {
        memset(gxhtc3_properties, 0, sizeof(s_ezlopi_device_properties_t));
        gxhtc3_properties->interface_type = EZLOPI_DEVICE_INTERFACE_ANALOG_INPUT;

        char *device_name = NULL;
        CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
        ASSIGN_DEVICE_NAME(gxhtc3_properties, device_name);
        gxhtc3_properties->ezlopi_cloud.category = category_humidity;
        gxhtc3_properties->ezlopi_cloud.subcategory = subcategory_not_defined;
        gxhtc3_properties->ezlopi_cloud.item_name = ezlopi_item_name_humidity;
        gxhtc3_properties->ezlopi_cloud.device_type = dev_type_sensor;
        gxhtc3_properties->ezlopi_cloud.value_type = value_type_humidity;
        gxhtc3_properties->ezlopi_cloud.has_getter = true;
        gxhtc3_properties->ezlopi_cloud.has_setter = false;
        gxhtc3_properties->ezlopi_cloud.reachable = true;
        gxhtc3_properties->ezlopi_cloud.battery_powered = false;
        gxhtc3_properties->ezlopi_cloud.show = true;
        gxhtc3_properties->ezlopi_cloud.room_name[0] = '\0';
        gxhtc3_properties->ezlopi_cloud.device_id = ezlopi_cloud_generate_device_id();
        gxhtc3_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
        gxhtc3_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();

        gxhtc3_properties->interface_type = EZLOPI_DEVICE_INTERFACE_I2C_MASTER;
        gxhtc3_properties->interface.i2c_master.enable = 1;
        gxhtc3_properties->interface.i2c_master.channel = 0;
        gxhtc3_properties->interface.i2c_master.clock_speed = 100000;
        CJSON_GET_VALUE_INT(cjson_device, "gpio_scl", gxhtc3_properties->interface.i2c_master.scl);
        CJSON_GET_VALUE_INT(cjson_device, "gpio_sda", gxhtc3_properties->interface.i2c_master.sda);
        CJSON_GET_VALUE_INT(cjson_device, "slave_addr", gxhtc3_properties->interface.i2c_master.address);
    }

    return gxhtc3_properties;
}

static void wgxhtc3_sensor_init(s_ezlopi_device_properties_t *properties, void *user_arg)
{
    ezlopi_i2c_master_init(&properties->interface.i2c_master);
}
#endif