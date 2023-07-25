
#include "trace.h"
#include "esp_err.h"
#include "ezlopi_i2c_master.h"
#include "sensor_0006_I2C_ADXL345.h"
#include "ezlopi_timer.h"
#include "cJSON.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "ezlopi_device_value_updated.h"

//------------------------------------------------------------------------------
#define ADD_PROPERTIES_DEVICE_LIST(device_id, category, subcategory, item_name, value_type, cjson_device)                             \
    {                                                                                                                                 \
        s_ezlopi_device_properties_t *_properties = sensor_i2c_accelerometer_prepare_properties(device_id, category, subcategory,     \
                                                                                                item_name, value_type, cjson_device); \
        if (NULL != _properties)                                                                                                      \
        {                                                                                                                             \
            add_device_to_list(prep_arg, _properties, NULL);                                                                          \
        }                                                                                                                             \
    }

//------------------------------------------------------------------------------

static int sensor_i2c_accelerometer_prepare(void *arg);
static s_ezlopi_device_properties_t *sensor_i2c_accelerometer_prepare_properties(uint32_t dev_id, const char *category, const char *sub_category, const char *item_name, const char *value_type, cJSON *cjson_device);
static int add_device_to_list(s_ezlopi_prep_arg_t *prep_arg, s_ezlopi_device_properties_t *sensor_ADXL345_device_properties, void *user_arg);
static int sensor_i2c_accelerometer_init(s_ezlopi_device_properties_t *properties, void *user_arg);
static int sensor_i2c_accelerometer_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args);
static int sensor_i2c_accelerometer_configure_device(s_ezlopi_device_properties_t *properties, void *args);

static esp_err_t get_device_id(s_ezlopi_device_properties_t *properties);
static esp_err_t data_formatting(s_ezlopi_device_properties_t *properties);
static esp_err_t set_to_measure_mode(s_ezlopi_device_properties_t *properties);
static esp_err_t reset_measure_mode(s_ezlopi_device_properties_t *properties);
static esp_err_t enable_data_ready_interrupt(s_ezlopi_device_properties_t *properties);

static int16_t get_adxl345_x_axis_value(s_ezlopi_device_properties_t *properties);
static int16_t get_adxl345_y_axis_value(s_ezlopi_device_properties_t *properties);
static int16_t get_adxl345_z_axis_value(s_ezlopi_device_properties_t *properties);

//------------------------------------------------------------------------------

int sensor_0006_I2C_ADXL345(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg)
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

//------------------------------------------------------------------------------

static int sensor_i2c_accelerometer_prepare(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;

    if ((NULL != prep_arg) && (NULL != prep_arg->cjson_device))
    {
        uint32_t device_id = ezlopi_cloud_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(device_id, category_generic_sensor, subcategory_not_defined, ezlopi_item_name_acceleration_x_axis, value_type_acceleration, prep_arg->cjson_device);
        device_id = ezlopi_cloud_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(device_id, category_generic_sensor, subcategory_not_defined, ezlopi_item_name_acceleration_y_axis, value_type_acceleration, prep_arg->cjson_device);
        device_id = ezlopi_cloud_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(device_id, category_generic_sensor, subcategory_not_defined, ezlopi_item_name_acceleration_z_axis, value_type_acceleration, prep_arg->cjson_device);
    }
    return ret;
}

static int add_device_to_list(s_ezlopi_prep_arg_t *prep_arg, s_ezlopi_device_properties_t *sensor_ADXL345_device_properties, void *user_arg)
{
    int ret = 0;
    if (sensor_ADXL345_device_properties)
    {
        if (0 == ezlopi_devices_list_add(prep_arg->device, sensor_ADXL345_device_properties, user_arg))
        {
            free(sensor_ADXL345_device_properties);
        }
        else
        {
            ret = 1;
        }
    }
    return ret;
}

static s_ezlopi_device_properties_t *sensor_i2c_accelerometer_prepare_properties(uint32_t DEV_ID, const char *CATEGORY, const char *SUB_CATEGORY, const char *ITEM_NAME, const char *VALUE_TYPE, cJSON *cjson_device)
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
            if (ezlopi_item_name_acceleration_x_axis == ITEM_NAME)
            {
                device_name = "ADXL345 Acceleration-X";
            }
            if (ezlopi_item_name_acceleration_y_axis == ITEM_NAME)
            {
                device_name = "ADXL345 Acceleration-Y";
            }
            if (ezlopi_item_name_acceleration_z_axis == ITEM_NAME)
            {
                device_name = "ADXL345 Acceleration-Z";
            }
            // CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
            ASSIGN_DEVICE_NAME(sensor_i2c_accelerometer_properties, device_name);
            sensor_i2c_accelerometer_properties->ezlopi_cloud.category = CATEGORY;
            sensor_i2c_accelerometer_properties->ezlopi_cloud.subcategory = SUB_CATEGORY;
            sensor_i2c_accelerometer_properties->ezlopi_cloud.item_name = ITEM_NAME;
            sensor_i2c_accelerometer_properties->ezlopi_cloud.device_type = dev_type_sensor;
            sensor_i2c_accelerometer_properties->ezlopi_cloud.value_type = VALUE_TYPE;
            sensor_i2c_accelerometer_properties->ezlopi_cloud.has_getter = true;
            sensor_i2c_accelerometer_properties->ezlopi_cloud.has_setter = false;
            sensor_i2c_accelerometer_properties->ezlopi_cloud.reachable = true;
            sensor_i2c_accelerometer_properties->ezlopi_cloud.battery_powered = false;
            sensor_i2c_accelerometer_properties->ezlopi_cloud.show = true;
            sensor_i2c_accelerometer_properties->ezlopi_cloud.room_name[0] = '\0';
            sensor_i2c_accelerometer_properties->ezlopi_cloud.device_id = DEV_ID;
            sensor_i2c_accelerometer_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
            sensor_i2c_accelerometer_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();

            CJSON_GET_VALUE_INT(cjson_device, "gpio_scl", sensor_i2c_accelerometer_properties->interface.i2c_master.scl);
            CJSON_GET_VALUE_INT(cjson_device, "gpio_sda", sensor_i2c_accelerometer_properties->interface.i2c_master.sda);

            sensor_i2c_accelerometer_properties->interface.i2c_master.enable = true;
            sensor_i2c_accelerometer_properties->interface.i2c_master.clock_speed = 100000;
            sensor_i2c_accelerometer_properties->interface.i2c_master.address = ADXL345_ADDR;
        }
    }
    return sensor_i2c_accelerometer_properties;
}

static int sensor_i2c_accelerometer_init(s_ezlopi_device_properties_t *properties, void *user_arg)
{
    int ret = 0;
    static bool guard = false;
    if (!guard)
    {
        guard = true;
        ezlopi_i2c_master_init(&properties->interface.i2c_master);

        // TRACE_I("I2C initialized to channel %d", properties->interface.i2c_master.channel);
        sensor_i2c_accelerometer_configure_device(properties, user_arg);
    }
    return ret;
}

static int sensor_i2c_accelerometer_configure_device(s_ezlopi_device_properties_t *properties, void *args)
{
    int ret = 0;
    ESP_ERROR_CHECK_WITHOUT_ABORT(reset_measure_mode(properties));
    ESP_ERROR_CHECK_WITHOUT_ABORT(data_formatting(properties));
    ESP_ERROR_CHECK_WITHOUT_ABORT(get_device_id(properties));
    ESP_ERROR_CHECK_WITHOUT_ABORT(set_to_measure_mode(properties));
    ESP_ERROR_CHECK_WITHOUT_ABORT(enable_data_ready_interrupt(properties));
    return ret;
}

static int sensor_i2c_accelerometer_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args)
{
    int ret = 0;
    cJSON *cjson_properties = (cJSON *)args;
    float acceleration_value;
    char valueFormatted[20];
    if (cjson_properties)
    {
        if (ezlopi_item_name_acceleration_x_axis == properties->ezlopi_cloud.item_name)
        {
            acceleration_value = (get_adxl345_x_axis_value(properties) * ADXL345_CONVERTER_FACTOR_MG_TO_G * ADXL345_STANDARD_G_TO_ACCEL_CONVERSION_VALUE);
            snprintf(valueFormatted, 20, "%.2f", acceleration_value);
            TRACE_I("X-axis : %.2f", acceleration_value);
            cJSON_AddStringToObject(cjson_properties, "valueFormatted", valueFormatted);
            cJSON_AddNumberToObject(cjson_properties, "value", acceleration_value);
            cJSON_AddStringToObject(cjson_properties, "scale", "meter_per_square_second");
        }
        if (ezlopi_item_name_acceleration_y_axis == properties->ezlopi_cloud.item_name)
        {
            acceleration_value = (get_adxl345_y_axis_value(properties) * ADXL345_CONVERTER_FACTOR_MG_TO_G * ADXL345_STANDARD_G_TO_ACCEL_CONVERSION_VALUE);
            snprintf(valueFormatted, 20, "%.2f", acceleration_value);
            TRACE_I("Y-axis : %.2f", acceleration_value);
            cJSON_AddStringToObject(cjson_properties, "valueFormatted", valueFormatted);
            cJSON_AddNumberToObject(cjson_properties, "value", acceleration_value);
            cJSON_AddStringToObject(cjson_properties, "scale", "meter_per_square_second");
        }
        if (ezlopi_item_name_acceleration_z_axis == properties->ezlopi_cloud.item_name)
        {
            acceleration_value = (get_adxl345_z_axis_value(properties) * ADXL345_CONVERTER_FACTOR_MG_TO_G * ADXL345_STANDARD_G_TO_ACCEL_CONVERSION_VALUE);
            snprintf(valueFormatted, 20, "%.2f", acceleration_value);
            TRACE_I("Z-axis : %.2f", acceleration_value);
            cJSON_AddStringToObject(cjson_properties, "valueFormatted", valueFormatted);
            cJSON_AddNumberToObject(cjson_properties, "value", acceleration_value);
            cJSON_AddStringToObject(cjson_properties, "scale", "meter_per_square_second");
        }

        ret = 1;
    }
    return ret;
}
//------------------------------------------------------------------------------

static esp_err_t get_device_id(s_ezlopi_device_properties_t *properties)
{
    uint8_t dev_id = 0;
    uint8_t write_buffer[] = {ADXL345_DEVICE_ID_REGISTER}; // REG_INTR_STATUS;
    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, write_buffer, 1);
    ezlopi_i2c_master_read_from_device(&properties->interface.i2c_master, &dev_id, 1);
    vTaskDelay(10);
    // TRACE_B("The device id is %d", dev_id);
    return ESP_OK;
}
static esp_err_t data_formatting(s_ezlopi_device_properties_t *properties)
{
    uint8_t write_byte[] = {ADXL345_DATA_FORMAT_REGISTER, ADXL345_FORMAT_REGISTER_DATA};
    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, write_byte, 2);
    vTaskDelay(10);
    return ESP_OK;
}
static esp_err_t set_to_measure_mode(s_ezlopi_device_properties_t *properties)
{
    uint8_t write_byte[] = {ADXL345_DEVICE_POWER_CTRL, ADXL345_POWER_CTRL_SET_TO_MEASUTEMENT};
    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, write_byte, 2);
    vTaskDelay(10);
    return ESP_OK;
}
static esp_err_t enable_data_ready_interrupt(s_ezlopi_device_properties_t *properties)
{
    uint8_t write_byte[] = {ADXL345_INT_ENABLE_REGISTER, ADXL345_INT_EN};
    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, write_byte, 2);
    vTaskDelay(10);
    return ESP_OK;
}
static esp_err_t reset_measure_mode(s_ezlopi_device_properties_t *properties)
{
    uint8_t write_byte[] = {ADXL345_DEVICE_POWER_CTRL, ADXL345_POWER_CTRL_RESET};
    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, write_byte, 2);
    vTaskDelay(10);
    return ESP_OK;
}

//------------------------------------------------------------------------------

// function to check for INTR bit before any data extraction is done from acceleration registers
static esp_err_t adxl345_check_data_ready_INTR(s_ezlopi_device_properties_t *properties, uint8_t *temp)
{
    esp_err_t err = ESP_OK;
    // Must request INTR_REG
    uint8_t write_buffer[] = {ADXL345_INT_SOURCE_REGISTER}; // REG_INTR_STATUS;
    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, write_buffer, 1);
    // Read -> INTR_BIT
    ezlopi_i2c_master_read_from_device(&properties->interface.i2c_master, temp, 1);
    if (NULL != temp)
    {
        err = ESP_OK;
    }
    else
    {
        err = ESP_ERR_TIMEOUT;
    }
    return err;
}

static int16_t get_adxl345_x_axis_value(s_ezlopi_device_properties_t *properties)
{
    uint8_t buffer_0, buffer_1;
    uint8_t Check_Register = 0;
    uint8_t address_val;
    esp_err_t err = ESP_OK;

    /*
     * extract data from register 0 (LSB)
     */
    if ((err = adxl345_check_data_ready_INTR(properties, &Check_Register)) == ESP_OK)
    {
        // if 'bit7' in INTR register is set ; then read procced to read :- acc registers
        if ((Check_Register & (1 << 7)))
        {
            address_val = ADXL345_DATA_X_0_REGISTER;
            ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, &address_val, 1);
            ezlopi_i2c_master_read_from_device(&properties->interface.i2c_master, (&buffer_0), 1);
            address_val = ADXL345_DATA_X_1_REGISTER;
            ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, &address_val, 1);
            ezlopi_i2c_master_read_from_device(&properties->interface.i2c_master, (&buffer_1), 1);
        }
    }
    else
    {
        TRACE_E("Data not ready @reg{%x}... Error type:-ESP_ERR_%d ", ADXL345_DATA_X_0_REGISTER, (err));
    }

    int16_t x_data = (int16_t)((buffer_1 << 8) | buffer_0);
    // TRACE_B("x_data is %d", x_data);
    return x_data;
}

static int16_t get_adxl345_y_axis_value(s_ezlopi_device_properties_t *properties)
{
    uint8_t buffer_0, buffer_1;
    uint8_t Check_Register = 0;
    uint8_t address_val;
    esp_err_t err = ESP_OK;
    /*
     * extract data from register 0 (LSB)
     */
    if ((err = adxl345_check_data_ready_INTR(properties, &Check_Register)) == ESP_OK)
    {
        // if 'bit7' in INTR register is set ; then read procced to read :- acc registers
        if ((Check_Register & (1 << 7)))
        {
            address_val = ADXL345_DATA_Y_0_REGISTER;
            ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, &address_val, 1);
            ezlopi_i2c_master_read_from_device(&properties->interface.i2c_master, (&buffer_0), 1);
            address_val = ADXL345_DATA_Y_1_REGISTER;
            ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, &address_val, 1);
            ezlopi_i2c_master_read_from_device(&properties->interface.i2c_master, (&buffer_1), 1);
        }
    }
    else
    {
        TRACE_E("Data not ready @reg{%x}... Error type:-ESP_ERR_%d ", ADXL345_DATA_Y_0_REGISTER, (err));
    }

    int16_t y_data = (int16_t)((buffer_1 << 8) | buffer_0);
    // TRACE_B("y_data is %d", y_data);
    return y_data;
}

static int16_t get_adxl345_z_axis_value(s_ezlopi_device_properties_t *properties)
{
    uint8_t buffer_0, buffer_1;
    uint8_t Check_Register = 0;
    uint8_t address_val;
    esp_err_t err = ESP_OK;
    /*
     * extract data from register 0 (LSB)
     */
    if ((err = adxl345_check_data_ready_INTR(properties, &Check_Register)) == ESP_OK)
    {
        // if 'bit7' in INTR register is set ; then read procced to read :- acc registers
        if ((Check_Register & (1 << 7)))
        {
            address_val = ADXL345_DATA_Z_0_REGISTER;
            ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, &address_val, 1);
            ezlopi_i2c_master_read_from_device(&properties->interface.i2c_master, (&buffer_0), 1);
            address_val = ADXL345_DATA_Z_1_REGISTER;
            ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, &address_val, 1);
            ezlopi_i2c_master_read_from_device(&properties->interface.i2c_master, (&buffer_1), 1);
        }
    }
    else
    {
        TRACE_E("Data not ready @reg{%x}... Error type:-ESP_ERR_%d ", ADXL345_DATA_Z_0_REGISTER, (err));
    }

    int16_t z_data = (int16_t)((buffer_1 << 8) | buffer_0);
    // TRACE_B("z_data is %d", z_data);

    return z_data;
}
