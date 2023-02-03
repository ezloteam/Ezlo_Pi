#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"

#include "ezlopi_i2c_master.h"
#include "trace.h"

#include "0029_gxhtc3_rh_t_sensor.h"

static void wgxhtc3_sensor_prepare(void *arg);
static int gxhtc3_notify(s_ezlopi_device_properties_t *properties, void *args);
static s_ezlopi_device_properties_t *wgxhtc3_sensor_prepare_temperature(cJSON *cjson_device);
static s_ezlopi_device_properties_t *wgxhtc3_sensor_prepare_relative_humidity(cJSON *cjson_device);
static void wgxhtc3_sensor_init(s_ezlopi_device_properties_t *properties, void *user_arg);

int water_leak_sensor(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg)
{
    int ret = 0;
    static uint32_t count;

    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        wgxhtc3_sensor_prepare(arg);
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

float temperature = 0.0;
float relative_humidity = 0.0;

static void wgxhtc3_sensor_read_sensor_data(s_ezlopi_device_properties_t *properties)
{
    uint8_t wakeup_cmd[] = {0x35, 0x17};
    uint8_t temp_measure_cmd[] = {0x7C, 0xA2}; // temperature first-clock stretching

    ezlopi_i2c_master_write_to_device(properties, wakeup_cmd, 2);
    vTaskDelay(100);
    ezlopi_i2c_master_write_to_device(properties, temp_measure_cmd, 2);
    vTaskDelay(500);
    uint8_t read_buffer[6];
    ezlopi_i2c_master_read_from_device(properties, read_buffer, 6);
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
            TRACE_E("Temperature is: %f *C", temperature);
            cJSON_AddNumberToObject(cjson_properties, "value", temperature);
            cJSON_AddStringToObject(cjson_properties, "scale", "celsius");
        }
        if (category_humidity == properties->ezlopi_cloud.category)
        {
            TRACE_E("Humidity is: %f %%", relative_humidity);
            cJSON_AddNumberToObject(cjson_properties, "value", relative_humidity);
            cJSON_AddStringToObject(cjson_properties, "scale", "percent");
        }
    }

    return 1;
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
        gxhtc3_properties->ezlopi_cloud.device_id = ezlopi_device_generate_device_id();
        gxhtc3_properties->ezlopi_cloud.room_id = ezlopi_device_generate_room_id();
        gxhtc3_properties->ezlopi_cloud.item_id = ezlopi_device_generate_item_id();

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
        gxhtc3_properties->ezlopi_cloud.device_id = ezlopi_device_generate_device_id();
        gxhtc3_properties->ezlopi_cloud.room_id = ezlopi_device_generate_room_id();
        gxhtc3_properties->ezlopi_cloud.item_id = ezlopi_device_generate_item_id();

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
