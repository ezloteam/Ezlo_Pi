

#include "trace.h"
#include "028_sens_i2c_accelerometer.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_timer.h"
#include "cJSON.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "ezlopi_device_value_updated.h"


#define ADD_PROPERTIES_DEVICE_LIST(device_id, category, subcategory, item_name, value_type, cjson_device)                  \
    {                                                                                                                      \
        s_ezlopi_device_properties_t *_properties = sensor_i2c_accelerometer_prepare_properties(device_id, category, subcategory,     \
                                                                                     item_name, value_type, cjson_device); \
        if (NULL != _properties)                                                                                           \
        {                                                                                                                  \
            add_device_to_list(prep_arg, _properties, NULL);                                                               \
        }                                                                                                                  \
    }

static int sensor_i2c_accelerometer_prepare(void *arg);
static s_ezlopi_device_properties_t *sensor_i2c_accelerometer_prepare_properties(uint32_t dev_id, const char *category, const char *sub_category, const char *item_name, const char *value_type, cJSON *cjson_device);
static int add_device_to_list(s_ezlopi_prep_arg_t *prep_arg, s_ezlopi_device_properties_t *sensor_bme_device_properties, void *user_arg);
static int sensor_i2c_accelerometer_init(s_ezlopi_device_properties_t *properties, void *user_arg);
static int sensor_i2c_accelerometer_read_value_from_sensor(s_ezlopi_device_properties_t *properties);
static int sensor_i2c_accelerometer_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args);
static int sensor_i2c_accelerometer_configure_device(s_ezlopi_device_properties_t *properties, void *args);


static uint8_t get_device_id(s_ezlopi_device_properties_t *properties);
static uint8_t data_formatting(s_ezlopi_device_properties_t *properties);
static bool set_to_measure_mode(s_ezlopi_device_properties_t *properties);
static bool reset_measure_mode(s_ezlopi_device_properties_t *properties);
static int16_t get_x_axis_value(s_ezlopi_device_properties_t *properties);
static int16_t get_y_axis_value(s_ezlopi_device_properties_t *properties);
static int16_t get_z_axis_value(s_ezlopi_device_properties_t *properties);



int sensor_i2c_accelerometer(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg)
{
    switch (action)
    {
        case EZLOPI_ACTION_PREPARE:
        {
            sensor_i2c_accelerometer_prepare(arg);
            break;
        }
        case EZLOPI_ACTION_INITIALIZE:
        {
            sensor_i2c_accelerometer_init(properties, user_arg);
            break;
        }
        case EZLOPI_ACTION_GET_EZLOPI_VALUE:
        {
            sensor_i2c_accelerometer_get_value_cjson(properties, arg);
            break;
        }
        case EZLOPI_ACTION_NOTIFY_1000_MS:
        {
            ezlopi_device_value_updated_from_device(properties);
            break;
        }
        default:
        {
            break;
        }
    }
    return 0;
}


static int sensor_i2c_accelerometer_prepare(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;

    if ((NULL != prep_arg) && (NULL != prep_arg->cjson_device))
    {
        uint32_t device_id = ezlopi_device_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(device_id, category_level_sensor, subcategory_navigation, ezlopi_item_name_acceleration_x_axis, value_type_acceleration, prep_arg->cjson_device);
        device_id = ezlopi_device_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(device_id, category_level_sensor, subcategory_navigation, ezlopi_item_name_acceleration_y_axis, value_type_acceleration, prep_arg->cjson_device);
        device_id = ezlopi_device_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(device_id, category_level_sensor, subcategory_navigation, ezlopi_item_name_acceleration_z_axis, value_type_acceleration, prep_arg->cjson_device);
    }
    return ret;
}


static int add_device_to_list(s_ezlopi_prep_arg_t *prep_arg, s_ezlopi_device_properties_t *sensor_bme_device_properties, void *user_arg)
{
    int ret = 0;
    if (sensor_bme_device_properties)
    {
        if (0 == ezlopi_devices_list_add(prep_arg->device, sensor_bme_device_properties, user_arg))
        {
            free(sensor_bme_device_properties);
        }
        else
        {
            ret = 1;
        }
    }
    return ret;
}

static s_ezlopi_device_properties_t *sensor_i2c_accelerometer_prepare_properties(uint32_t dev_id, const char *category, const char *sub_category, const char *item_name, const char *value_type, cJSON *cjson_device)
{
    s_ezlopi_device_properties_t *sensor_i2c_accelerometer_properties = NULL;

    if ((NULL != cjson_device))
    {
        sensor_i2c_accelerometer_properties = malloc(sizeof(s_ezlopi_device_properties_t));
        if (sensor_i2c_accelerometer_properties)
        {
            memset(sensor_i2c_accelerometer_properties, 0, sizeof(s_ezlopi_device_properties_t));
            sensor_i2c_accelerometer_properties->interface_type = EZLOPI_DEVICE_INTERFACE_I2C_MASTER;

            char *device_name = NULL;
            CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
            ASSIGN_DEVICE_NAME(sensor_i2c_accelerometer_properties, device_name);
            sensor_i2c_accelerometer_properties->ezlopi_cloud.category = category;
            sensor_i2c_accelerometer_properties->ezlopi_cloud.subcategory = sub_category;
            sensor_i2c_accelerometer_properties->ezlopi_cloud.item_name = item_name;
            sensor_i2c_accelerometer_properties->ezlopi_cloud.device_type = dev_type_sensor;
            sensor_i2c_accelerometer_properties->ezlopi_cloud.value_type = value_type;
            sensor_i2c_accelerometer_properties->ezlopi_cloud.has_getter = true;
            sensor_i2c_accelerometer_properties->ezlopi_cloud.has_setter = false;
            sensor_i2c_accelerometer_properties->ezlopi_cloud.reachable = true;
            sensor_i2c_accelerometer_properties->ezlopi_cloud.battery_powered = false;
            sensor_i2c_accelerometer_properties->ezlopi_cloud.show = true;
            sensor_i2c_accelerometer_properties->ezlopi_cloud.room_name[0] = '\0';
            sensor_i2c_accelerometer_properties->ezlopi_cloud.device_id = dev_id;
            sensor_i2c_accelerometer_properties->ezlopi_cloud.room_id = ezlopi_device_generate_room_id();
            sensor_i2c_accelerometer_properties->ezlopi_cloud.item_id = ezlopi_device_generate_item_id();

            CJSON_GET_VALUE_INT(cjson_device, "gpio_scl", sensor_i2c_accelerometer_properties->interface.i2c_master.scl);
            CJSON_GET_VALUE_INT(cjson_device, "gpio_sda", sensor_i2c_accelerometer_properties->interface.i2c_master.sda);

            sensor_i2c_accelerometer_properties->interface.i2c_master.enable = true;
            sensor_i2c_accelerometer_properties->interface.i2c_master.clock_speed = 100000;
            sensor_i2c_accelerometer_properties->interface.i2c_master.address = SLAVE_ADDR;

            sensor_i2c_accelerometer_values_t accelerometer_values = {
                .x_value = 0.0,
                .y_value = 0.0,
                .z_value = 0.0,
            };
            sensor_i2c_accelerometer_properties->user_arg = &accelerometer_values;
        }
    }
    return sensor_i2c_accelerometer_properties;
}

static int sensor_i2c_accelerometer_init(s_ezlopi_device_properties_t *properties, void *user_arg)
{
    int ret = 0;
    ezlopi_i2c_master_init(&properties->interface.i2c_master);

    TRACE_I("I2C initialized to channel %d", properties->interface.i2c_master.channel);
    sensor_i2c_accelerometer_configure_device(properties, user_arg);
    return ret;
}

static int sensor_i2c_accelerometer_configure_device(s_ezlopi_device_properties_t *properties, void *args)
{
    int ret = 0;
    ESP_ERROR_CHECK_WITHOUT_ABORT(reset_measure_mode(properties));
    ESP_ERROR_CHECK_WITHOUT_ABORT(data_formatting(properties));
    ESP_ERROR_CHECK_WITHOUT_ABORT(get_device_id(properties));
    ESP_ERROR_CHECK_WITHOUT_ABORT(set_to_measure_mode(properties));
    return ret;
}

static int sensor_i2c_accelerometer_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args)
{
    int ret = 0;
    sensor_i2c_accelerometer_read_value_from_sensor(properties);
    sensor_i2c_accelerometer_values_t* sensor_data = (sensor_i2c_accelerometer_values_t*)properties->user_arg; 
    cJSON *cjson_properties = (cJSON *)args;

    if (cjson_properties)
    {
        if (ezlopi_item_name_acceleration_x_axis == properties->ezlopi_cloud.item_name)
        {
            // TRACE_E("sensor_data->x_value is: %f", sensor_data->x_value);
            cJSON_AddNumberToObject(cjson_properties, "value", sensor_data->x_value);
            cJSON_AddStringToObject(cjson_properties, "scale", "meter_per_square_second");
        }
        if (ezlopi_item_name_acceleration_y_axis == properties->ezlopi_cloud.item_name)
        {
            // TRACE_E("sensor_data->y_value is: %f", sensor_data->y_value);
            cJSON_AddNumberToObject(cjson_properties, "value", sensor_data->y_value);
            cJSON_AddStringToObject(cjson_properties, "scale", "meter_per_square_second");
        }
        if (ezlopi_item_name_acceleration_z_axis == properties->ezlopi_cloud.item_name)
        {
            // TRACE_E("sensor_data->z_value is: %f", sensor_data->z_value);
            cJSON_AddNumberToObject(cjson_properties, "value", sensor_data->z_value);
            cJSON_AddStringToObject(cjson_properties, "scale", "meter_per_square_second");
        }

        ret = 1;
    }
    return ret;
}

static int sensor_i2c_accelerometer_read_value_from_sensor(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;
    sensor_i2c_accelerometer_values_t* sensor_data = (sensor_i2c_accelerometer_values_t*)properties->user_arg; 

    if(NULL != sensor_data)
    {
        int16_t x_value = get_x_axis_value(properties);
        int16_t y_value = get_y_axis_value(properties);
        int16_t z_value = get_z_axis_value(properties);

        sensor_data->x_value = (x_value * ADXL345_CONVERTER_FACTOR_MG_TO_G * STANDARD_G_TO_ACCEL_CONVERSION_VALUE) / 8.1;
        sensor_data->y_value = (y_value * ADXL345_CONVERTER_FACTOR_MG_TO_G * STANDARD_G_TO_ACCEL_CONVERSION_VALUE) / 8.1;
        sensor_data->z_value = (z_value * ADXL345_CONVERTER_FACTOR_MG_TO_G * STANDARD_G_TO_ACCEL_CONVERSION_VALUE) / 8.1;
        TRACE_B("(x, y, z) = (%f, %f, %f)", sensor_data->x_value, sensor_data->y_value, sensor_data->z_value);
    }
    
    return ret;
}


static uint8_t get_device_id(s_ezlopi_device_properties_t *properties)
{
    uint8_t dev_id = 0;
    read_reg(properties->interface.i2c_master.channel, properties->interface.i2c_master.address, &dev_id, ADXL345_DEVICE_ID_REGISTER);
    return dev_id;
}


static uint8_t data_formatting(s_ezlopi_device_properties_t *properties)
{
    uint8_t data_format = ADXL345_FORMAT_REGISTER_DATA;
    uint8_t format = 0;
    ESP_ERROR_CHECK_WITHOUT_ABORT(write_reg(properties->interface.i2c_master.channel, properties->interface.i2c_master.address, data_format, ADXL345_DATA_FORMAT_REGISTER));
    ESP_ERROR_CHECK_WITHOUT_ABORT(read_reg(properties->interface.i2c_master.channel, properties->interface.i2c_master.address, &format, ADXL345_DATA_FORMAT_REGISTER));
    TRACE_B("The data format register value is 0x%x", format);
    return format;
}

static bool set_to_measure_mode(s_ezlopi_device_properties_t *properties)
{
    uint8_t measure_mode = 0;
    ESP_ERROR_CHECK_WITHOUT_ABORT(write_reg(properties->interface.i2c_master.channel, properties->interface.i2c_master.address, ADXL345_POWER_CTRL_SET_TO_MEASUTEMENT, ADXL345_DEVICE_POWER_CTRL));
    ESP_ERROR_CHECK_WITHOUT_ABORT(read_reg(properties->interface.i2c_master.channel, properties->interface.i2c_master.address, &measure_mode, ADXL345_DEVICE_POWER_CTRL));
    TRACE_B("Device set to measure mode with value 0x%x", measure_mode);
    return ESP_OK;
}

static bool reset_measure_mode(s_ezlopi_device_properties_t *properties)
{
    uint8_t measure_mode = 0;
    ESP_ERROR_CHECK_WITHOUT_ABORT(write_reg(properties->interface.i2c_master.channel, properties->interface.i2c_master.address, ADXL345_POWER_CTRL_RESET, ADXL345_DEVICE_POWER_CTRL));
    ESP_ERROR_CHECK_WITHOUT_ABORT(read_reg(properties->interface.i2c_master.channel, properties->interface.i2c_master.address, &measure_mode, ADXL345_DEVICE_POWER_CTRL));
    TRACE_B("Device reset to measure mode with value 0x%x", measure_mode);
    return ESP_OK;
}


static int16_t get_x_axis_value(s_ezlopi_device_properties_t *properties)
{
    uint8_t buffer_0, buffer_1;
    read_reg(properties->interface.i2c_master.channel, properties->interface.i2c_master.address, &buffer_0, ADXL345_DATA_X_0_REGISTER);
    read_reg(properties->interface.i2c_master.channel, properties->interface.i2c_master.address, &buffer_1, ADXL345_DATA_X_1_REGISTER);
    // TRACE_B("x-axis: (buffer_0, buffer_1) = (%x, %x)", buffer_0, buffer_1);
    int16_t x_data = (buffer_1 << 8) | buffer_0;
    // TRACE_B("x_data is %d", x_data);
    return x_data;
}

static int16_t get_y_axis_value(s_ezlopi_device_properties_t *properties)
{
    uint8_t buffer_0, buffer_1;
    read_reg(properties->interface.i2c_master.channel, properties->interface.i2c_master.address, &buffer_0, ADXL345_DATA_Y_0_REGISTER);
    read_reg(properties->interface.i2c_master.channel, properties->interface.i2c_master.address, &buffer_1, ADXL345_DATA_Y_1_REGISTER);
    // TRACE_B("y-axis: (buffer_0, buffer_1) = (%x, %x)", buffer_0, buffer_1);
    int16_t y_data = (buffer_1 << 8) | buffer_0;
    // TRACE_B("y_data is %d", y_data);
    return y_data;
}

static int16_t get_z_axis_value(s_ezlopi_device_properties_t *properties)
{
    uint8_t buffer_0, buffer_1;
    read_reg(properties->interface.i2c_master.channel, properties->interface.i2c_master.address, &buffer_0, ADXL345_DATA_Z_0_REGISTER);
    read_reg(properties->interface.i2c_master.channel, properties->interface.i2c_master.address, &buffer_1, ADXL345_DATA_Z_1_REGISTER);
    // TRACE_B("z-axis: (buffer_0, buffer_1) = (%x, %x)", buffer_0, buffer_1);
    int16_t z_data = (buffer_1 << 8) | buffer_0;
    // TRACE_B("z_data is %d", z_data);
    
    return z_data;
}

