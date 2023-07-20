#include "ezlopi_cloud.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"

#include "ezlopi_i2c_master.h"
#include "trace.h"

#include "sensor_0005_I2C_MPU6050.h"

/*************************************************************************************************/
/*                              DEFINES                                                     */
/*************************************************************************************************/

#define REG_COUNT_LEN 14 // the accelerometer , temperature and gyrodata is to be in one go .
static float acc_sen_calib_val = 0;
static float gyro_sen_calib_val = 0;

// Table mapping mpu6050_err_t enumerals to strings
static const char *mpu6050_err_str[MPU6050_ERR_MAX] = {
    [MPU6050_ERR_OK] = "No error",
    [MPU6050_ERR_PARAM_CFG_FAIL] = "i2c_param_config() error",
    [MPU6050_ERR_DRIVER_INSTALL_FAIL] = "i2c_driver_install() error",
    [MPU6050_ERR_INVALID_ARGUMENT] = "invalid parameter to function",
    [MPU6050_ERR_NO_SLAVE_ACK] = "No acknowledgment from slave",
    [MPU6050_ERR_INVALID_STATE] = "Driver not installed / not i2c master",
    [MPU6050_ERR_OPERATION_TIMEOUT] = "Timeout; Bus busy",
    [MPU6050_ERR_UNKNOWN] = "Unknown error",
};

#define ADD_PROPERTIES_DEVICE_LIST(device_id, category, subcategory, item_name, value_type, cjson_device)                       \
    {                                                                                                                           \
        s_ezlopi_device_properties_t *_properties = i2c_mpu6050_sensor_prepare_properties(device_id, category, subcategory,     \
                                                                                          item_name, value_type, cjson_device); \
        if (NULL != _properties)                                                                                                \
        {                                                                                                                       \
            add_device_to_list(prep_arg, _properties, NULL);                                                                    \
        }                                                                                                                       \
    }

/*************************************************************************************************/
//
//
//
//
//
//
//
//
/*************************************************************************************************/
/*                              DECLARATIONS                                                     */
/*************************************************************************************************/

// (#) Data-extraction
static void Correct_accel_temp_gyro(raw_mpu6050_data_t *RAW_DATA, mpu6050_data_t *data_p);
static mpu6050_err_t mpu6050_check_data_ready_INTR(s_ezlopi_device_properties_t *properties, uint8_t *temp);
static void i2c_mpu6050_sensor_read_sensor_data(s_ezlopi_device_properties_t *properties, mpu6050_data_t *data_p);

// 1) prepare
static void i2c_mpu6050_sensor_prepare(void *arg);
static s_ezlopi_device_properties_t *i2c_mpu6050_sensor_prepare_properties(uint32_t DEVICE_ID, const char *CATEGORY, const char *SUB_CATEGORY, const char *ITEM_NAME, const char *VALUE_TYPE, cJSON *cjson_device);

// 2) initialize
static mpu6050_err_t mpu6050_configure_power(s_ezlopi_device_properties_t *properties);
static mpu6050_err_t mpu6050_configure_accelerometer(s_ezlopi_device_properties_t *properties, uint8_t flags);
static mpu6050_err_t mpu6050_configure_gyroscope(s_ezlopi_device_properties_t *properties, uint8_t flags);
static mpu6050_err_t mpu6050_configure_dlfp(s_ezlopi_device_properties_t *properties);
static mpu6050_err_t mpu6050_enable_interrupt(s_ezlopi_device_properties_t *properties);
static int i2c_MPU6050_sensor_configure_device(s_ezlopi_device_properties_t *properties, void *user_arg);
static void i2c_mpu6050_sensor_init(s_ezlopi_device_properties_t *properties, void *user_arg);

// 3) notify
static void i2c_mpu6050_sensor_notify(s_ezlopi_device_properties_t *properties, void *args);

// 4) Action
static void i2c_mpu6050_sensor_notify(s_ezlopi_device_properties_t *properties, void *args);
/***********************************************************************************************/
//
//
//
//
//
//
//

//----------------------------------------------------------------------------------
// (4) List of "ACTION FUNCTIONS"
//----------------------------------------------------------------------------------
int sensor_0005_I2C_MPU6050(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg)
{
    int ret = 0;
    static uint32_t count;

    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        i2c_mpu6050_sensor_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        i2c_mpu6050_sensor_init(properties, NULL);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        i2c_mpu6050_sensor_notify(properties, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        ezlopi_device_value_updated_from_device(properties);
        break;
    }
    default:
        break;
    }

    return ret;
}

//
/*************************************************************************************************/
/*                              DEFINATION                                                       */
/*************************************************************************************************/

//----------------------------------------------------------------------------------
// (#) DATA-EXTRACTION FUNCTIONS
//----------------------------------------------------------------------------------

// mapper function for MPU6050_err
const char *mpu6050_err_to_str(mpu6050_err_t err)
{
    if (err > MPU6050_ERR_UNKNOWN)
    {
        return NULL;
    }
    else
    {
        /*send the error message corresponding to index value pointed by [mpu6050_err_str]*/
        return mpu6050_err_str[err];
    }
}
// function to calibrate the raw data
static void Correct_accel_temp_gyro(raw_mpu6050_data_t *RAW_DATA, mpu6050_data_t *data_p)
{
    data_p->ax = (RAW_DATA->raw_ax / acc_sen_calib_val) * MPU6050_STANDARD_G_TO_ACCEL_CONVERSION_VALUE; // in m/s^2
    data_p->ay = (RAW_DATA->raw_ay / acc_sen_calib_val) * MPU6050_STANDARD_G_TO_ACCEL_CONVERSION_VALUE;
    data_p->az = (RAW_DATA->raw_az / acc_sen_calib_val) * MPU6050_STANDARD_G_TO_ACCEL_CONVERSION_VALUE;
    data_p->gx = (RAW_DATA->raw_gx / gyro_sen_calib_val) - GYRO_X_OFFSET; // deg*/s
    data_p->gy = (RAW_DATA->raw_gy / gyro_sen_calib_val) - GYRO_Y_OFFSET; // deg*/s
    data_p->gz = (RAW_DATA->raw_gz / gyro_sen_calib_val) - GYRO_Z_OFFSET; // deg*/s
    data_p->temp_mpu = ((RAW_DATA->raw_t / 340) + 36.530f);

    // TRACE_I("ax->%.2f", data_p->ax);
    // TRACE_I("ay->%.2f", data_p->ay);
    // TRACE_I("az->%.2f", data_p->az);
    // TRACE_I("gx->%.2f", data_p->gx);
    // TRACE_I("gy->%.2f", data_p->gy);
    // TRACE_I("gz->%.2f", data_p->gz);
    // TRACE_I("T->%.2f", data_p->temp_mpu);
}
// function to check for INTR bit before any data extraction is done from accel,temp & gyro registers
static mpu6050_err_t mpu6050_check_data_ready_INTR(s_ezlopi_device_properties_t *properties, uint8_t *temp)
{
    mpu6050_err_t err = MPU6050_ERR_OK;
    // Must request INTR_REG
    uint8_t write_buffer[] = {REG_INTR_STATUS}; // REG_INTR_STATUS;
    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, write_buffer, 1);
    // Read -> INTR_BIT
    ezlopi_i2c_master_read_from_device(&properties->interface.i2c_master, temp, 1);
    if (NULL != temp)
    {
        err = MPU6050_ERR_OK;
    }
    else
    {
        err = MPU6050_ERR_OPERATION_TIMEOUT;
    }
    return err;
}
// function to initiate sensor data readings
static void i2c_mpu6050_sensor_read_sensor_data(s_ezlopi_device_properties_t *properties, mpu6050_data_t *data_p)
{
    static raw_mpu6050_data_t RAW_DATA = {0};
    static uint8_t tmp_buf[REG_COUNT_LEN] = {0}; // 0 - 13
    mpu6050_err_t err = MPU6050_ERR_OK;
    uint8_t Check_Register;
    uint8_t address_val;

    // Read specified FIFO buffer size (depends on configuration set)g
    for (uint8_t i = 0; i < REG_COUNT_LEN; i += 2)
    {
        if ((err = mpu6050_check_data_ready_INTR(properties, &Check_Register)) == MPU6050_ERR_OK)
        {
            // if 'bit0' in INTR register is set ; then read procced to read :- acc,gyro & tmp registers
            if (Check_Register == BIT_0)
            { // ACCEL_X_H = 0x3B
                address_val = (ACCEL_X_H + i);
                ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, &address_val, 1);
                ezlopi_i2c_master_read_from_device(&properties->interface.i2c_master, (tmp_buf + i), 1);
                // ACCEL_X_H = 0x3B +1
                address_val = (ACCEL_X_H + i + 1);
                ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, &address_val, 1);
                ezlopi_i2c_master_read_from_device(&properties->interface.i2c_master, (tmp_buf + i + 1), 1);
            }
        }
        else
        {
            TRACE_E("Data not ready @reg{%x}... Error type:- %d (%s)", (ACCEL_X_H + i), err, mpu6050_err_to_str(err));
        }
    }
    // Configure data structure // total 14 bytes
    RAW_DATA.raw_ax = (int16_t)(tmp_buf[0] << 8 | tmp_buf[1]);   // acc_x = 59(0x3B) [msb] & 60(0x3C) [lsb]
    RAW_DATA.raw_ay = (int16_t)(tmp_buf[2] << 8 | tmp_buf[3]);   // acc_y = 61 & 62
    RAW_DATA.raw_az = (int16_t)(tmp_buf[4] << 8 | tmp_buf[5]);   // acc_z = 63 & 64
    RAW_DATA.raw_t = (int16_t)(tmp_buf[6] << 8 | tmp_buf[7]);    // tp = 65 & 66
    RAW_DATA.raw_gx = (int16_t)(tmp_buf[8] << 8 | tmp_buf[9]);   // gx = 67 & 68
    RAW_DATA.raw_gy = (int16_t)(tmp_buf[10] << 8 | tmp_buf[11]); // gy = 69 & 70
    RAW_DATA.raw_gz = (int16_t)(tmp_buf[12] << 8 | tmp_buf[13]); // gz = 71 & 72

    Correct_accel_temp_gyro(&RAW_DATA, data_p);
    return err;
}

//----------------------------------------------------------------------------------
// (1) PREPARATION FUNCTIONS
//----------------------------------------------------------------------------------
static s_ezlopi_device_properties_t *i2c_mpu6050_sensor_prepare_properties(uint32_t DEVICE_ID, const char *CATEGORY, const char *SUB_CATEGORY, const char *ITEM_NAME, const char *VALUE_TYPE, cJSON *cjson_device)
{
    s_ezlopi_device_properties_t *i2c_mpu6050_properties = malloc(sizeof(s_ezlopi_device_properties_t));
    if (i2c_mpu6050_properties)
    {
        memset(i2c_mpu6050_properties, 0, sizeof(s_ezlopi_device_properties_t));
        char *device_name = NULL;
        if (ezlopi_item_name_acceleration_x_axis == ITEM_NAME)
        {
            device_name = "MPU6050 Acceleration-X";
        }
        if (ezlopi_item_name_acceleration_y_axis == ITEM_NAME)
        {
            device_name = "MPU6050 Acceleration-Y";
        }
        if (ezlopi_item_name_acceleration_z_axis == ITEM_NAME)
        {
            device_name = "MPU6050 Acceleration-Z";
        }
        // if (ezlopi_item_name_acceleration_x_axis == ITEM_NAME)
        // {
        //     device_name = "MPU6050 Gyroscope-X";
        // }
        // if (ezlopi_item_name_gyroscope_y_axis == ITEM_NAME)
        // {
        //     device_name = "MPU6050 Gyroscope-Y";
        // }
        // if (ezlopi_item_name_gyroscope_z_axis == ITEM_NAME)
        // {
        //     device_name = "MPU6050 Gyroscope-Z";
        // }

        CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
        ASSIGN_DEVICE_NAME(i2c_mpu6050_properties, device_name);
        i2c_mpu6050_properties->ezlopi_cloud.category = CATEGORY;
        i2c_mpu6050_properties->ezlopi_cloud.subcategory = SUB_CATEGORY;
        i2c_mpu6050_properties->ezlopi_cloud.item_name = ITEM_NAME;
        i2c_mpu6050_properties->ezlopi_cloud.device_type = dev_type_sensor;
        i2c_mpu6050_properties->ezlopi_cloud.value_type = VALUE_TYPE;
        i2c_mpu6050_properties->ezlopi_cloud.has_getter = true;
        i2c_mpu6050_properties->ezlopi_cloud.has_setter = false;
        i2c_mpu6050_properties->ezlopi_cloud.reachable = true;
        i2c_mpu6050_properties->ezlopi_cloud.battery_powered = false;
        i2c_mpu6050_properties->ezlopi_cloud.show = true;
        i2c_mpu6050_properties->ezlopi_cloud.room_name[0] = '\0';
        i2c_mpu6050_properties->ezlopi_cloud.device_id = DEVICE_ID;
        i2c_mpu6050_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
        i2c_mpu6050_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();

        i2c_mpu6050_properties->interface_type = EZLOPI_DEVICE_INTERFACE_I2C_MASTER;
        i2c_mpu6050_properties->interface.i2c_master.enable = 1;
        i2c_mpu6050_properties->interface.i2c_master.address = MPU6050_ADDR;
        i2c_mpu6050_properties->interface.i2c_master.clock_speed = 100000;

        CJSON_GET_VALUE_INT(cjson_device, "gpio_scl", i2c_mpu6050_properties->interface.i2c_master.scl);
        CJSON_GET_VALUE_INT(cjson_device, "gpio_sda", i2c_mpu6050_properties->interface.i2c_master.sda);
    }
    return i2c_mpu6050_properties;
}

static int add_device_to_list(s_ezlopi_prep_arg_t *prep_arg, s_ezlopi_device_properties_t *sensor_device_properties, void *user_arg)
{
    int ret = 0;
    if (sensor_device_properties)
    {
        if (0 == ezlopi_devices_list_add(prep_arg->device, sensor_device_properties, user_arg))
        {
            free(sensor_device_properties);
        }
        else
        {
            ret = 1;
        }
    }
    return ret;
}

static void i2c_mpu6050_sensor_prepare(void *arg)
{
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;

    if ((NULL != prep_arg) && (NULL != (prep_arg->cjson_device)))
    {
        uint32_t device_id = 0;
        device_id = ezlopi_cloud_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(device_id, category_generic_sensor, subcategory_not_defined, ezlopi_item_name_acceleration_x_axis, value_type_int, prep_arg->cjson_device);
        device_id = ezlopi_cloud_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(device_id, category_generic_sensor, subcategory_not_defined, ezlopi_item_name_acceleration_y_axis, value_type_int, prep_arg->cjson_device);
        device_id = ezlopi_cloud_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(device_id, category_generic_sensor, subcategory_not_defined, ezlopi_item_name_acceleration_z_axis, value_type_int, prep_arg->cjson_device);

        device_id = ezlopi_cloud_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(device_id, category_temperature, subcategory_not_defined, ezlopi_item_name_temp, value_type_temperature, prep_arg->cjson_device);

        // device_id = ezlopi_cloud_generate_device_id();
        // ADD_PROPERTIES_DEVICE_LIST(device_id, category_generic_sensor, subcategory_not_defined, "ezlopi_item_name_gyroscope_x_axis", "deg/s", prep_arg->cjson_device))
        // device_id = ezlopi_cloud_generate_device_id();
        // ADD_PROPERTIES_DEVICE_LIST(device_id, category_generic_sensor, subcategory_not_defined, "ezlopi_item_name_gyroscope_y_axis", "deg/s", prep_arg->cjson_device))
        // device_id = ezlopi_cloud_generate_device_id();
        // ADD_PROPERTIES_DEVICE_LIST(device_id, category_generic_sensor, subcategory_not_defined, "ezlopi_item_name_gyroscope_z_axis", "deg/s", prep_arg->cjson_device))
    }
}
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
// (2) INITIALIZATION FUNCTIONS (CONFIGURATIONS)
//----------------------------------------------------------------------------------
static mpu6050_err_t mpu6050_configure_power(s_ezlopi_device_properties_t *properties)
{
    mpu6050_err_t err = MPU6050_ERR_OK;
    uint8_t write_buffer[] = {0x6B, 0x03};
    // uint8_t write_buffer[] = {REG_PWR_MGMT_1, PWR_MGMT_1_PLL_Z_AXIS_INTERNAL_CLK_REF};
    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, write_buffer, 2);
    vTaskDelay(10);
    return err;
}
static mpu6050_err_t mpu6050_configure_accelerometer(s_ezlopi_device_properties_t *properties, uint8_t flags)
{
    mpu6050_err_t err = MPU6050_ERR_OK;
    uint8_t write_buffer[] = {0x1C, 0x00};
    // uint8_t write_buffer[] = {REG_A_CFG, A_CFG_2G};
    switch (flags)
    {
    case A_CFG_2G:
        acc_sen_calib_val = 16384.0f; //	{16384 LSB/g}
        break;
    case A_CFG_4G:
        acc_sen_calib_val = 8192.0f; //	{8192 LSB/g}
        break;
    case A_CFG_8G:
        acc_sen_calib_val = 4096.0f; //	{4096 LSB/g}
        break;
    case A_CFG_16G:
        acc_sen_calib_val = 2048.0f; //	{2048 LSB/g}
        break;
    default:
        break;
    }
    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, write_buffer, 2);
    vTaskDelay(10);
    return err;
}
static mpu6050_err_t mpu6050_configure_gyroscope(s_ezlopi_device_properties_t *properties, uint8_t flags)
{
    mpu6050_err_t err = MPU6050_ERR_OK;
    uint8_t write_buffer[] = {0x1B, 0x00};
    // uint8_t write_buffer[] = {REG_G_CFG, G_CFG_250};
    switch (flags)
    {
    case G_CFG_250:
        gyro_sen_calib_val = 131.0f; //	{131 LSB/deg/s}
        break;
    case G_CFG_500:
        gyro_sen_calib_val = 65.5f; //	{65.5 LSB/deg/s}
        break;
    case G_CFG_1000:
        gyro_sen_calib_val = 32.8f; //	{32.8 LSB/deg/s}
        break;
    case G_CFG_2000:
        gyro_sen_calib_val = 16.4f; //	{16.4 LSB/deg/s}
        break;
    default:
        break;
    }

    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, write_buffer, 2);
    vTaskDelay(10);
    return err;
}
static mpu6050_err_t mpu6050_configure_dlfp(s_ezlopi_device_properties_t *properties)
{
    mpu6050_err_t err = MPU6050_ERR_OK;
    uint8_t write_buffer[] = {0x1A, 0x00};
    // uint8_t write_buffer[] = {REG_DLFP_CFG, DLFP_CFG_FILTER_0};
    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, write_buffer, 2);
    vTaskDelay(10);
    return err;
}
static mpu6050_err_t mpu6050_enable_interrupt(s_ezlopi_device_properties_t *properties)
{
    mpu6050_err_t err = MPU6050_ERR_OK;
    uint8_t write_buffer[] = {0x38, 0x00};
    // uint8_t write_buffer[] = {REG_INTR_EN, INTR_EN_DATA_RDY};
    ezlopi_i2c_master_write_to_device(&properties->interface.i2c_master, write_buffer, 2);
    vTaskDelay(10);
    return err;
}

// ---> 2.a)configuring MPU6050 for correct readings
static int i2c_MPU6050_sensor_configure_device(s_ezlopi_device_properties_t *properties, void *args)
{
    mpu6050_err_t err = MPU6050_ERR_OK;
    uint8_t flags = 0;

    // Initialize I2C
    // flags = PWR_MGMT_1_PLL_Z_AXIS_INTERNAL_CLK_REF; // choose PLL-z-axis; as internal clk refr
    if ((err = mpu6050_configure_power(properties)) != MPU6050_ERR_OK)
    {

        TRACE_E("Initializtion unsuccessful %d", err);
        TRACE_E("%s", mpu6050_err_to_str(err));
        return (int)err;
    }

    // Configure accelerometer sensitivity
    flags = A_CFG_2G; // 16384 steps
    if ((err = mpu6050_configure_accelerometer(properties, flags)) != MPU6050_ERR_OK)
    {

        TRACE_E("Initializtion unsuccessful %d", err);
        TRACE_E("%s", mpu6050_err_to_str(err));
        return (int)err;
    }

    // Configure gyro sensitivity
    flags = G_CFG_250; // +-250 deg/s
    if ((err = mpu6050_configure_gyroscope(properties, flags)) != MPU6050_ERR_OK)
    {

        TRACE_E("Initializtion unsuccessful %d", err);
        TRACE_E("%s", mpu6050_err_to_str(err));
        return (int)err;
    }

    // Configure the Digital-Low-Pass-Filter
    // flags = DLFP_CFG_FILTER_0; // default -> no dlfp -> 8Mhz clk ref
    if ((err = mpu6050_configure_dlfp(properties)) != MPU6050_ERR_OK)
    {

        TRACE_E("Initializtion unsuccessful %d", err);
        TRACE_E("%s", mpu6050_err_to_str(err));
        return (int)err;
    }

    // Enable interrupts after every sensor refresh
    // flags = INTR_EN_DATA_RDY;
    if ((err = mpu6050_enable_interrupt(properties)) != MPU6050_ERR_OK) // DATA_RDY_EN = 1 //  occurs each-time a write operation to the sensor registers has been completed.
    {

        TRACE_E("Initializtion unsuccessful %d", err);
        TRACE_E("%s", mpu6050_err_to_str(err));
        return (int)err;
    }

    // NOTE
    if (MPU6050_ERR_OK == err)
    {
        TRACE_I("Initializtion Successful ...");
    }
    return 0;
}

// ---> 2.b)INITIALIZATION FUNCTION for "initialize action"
static void i2c_mpu6050_sensor_init(s_ezlopi_device_properties_t *properties, void *user_arg)
{
    static bool guard_i2c = false;
    // configuring the i2c
    if (!guard_i2c)
    {
        guard_i2c = true;
        TRACE_B("i2c_mster is %p", &properties->interface.i2c_master);
        ezlopi_i2c_master_init(&properties->interface.i2c_master);
        TRACE_I("I2C initialized to channel %d", properties->interface.i2c_master.channel);
        // configure the MPU6050
        i2c_MPU6050_sensor_configure_device(properties, user_arg);
    }
}
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
// (3) NOTIFICATION FUNCTIONS for "notify action"
//----------------------------------------------------------------------------------
static void i2c_mpu6050_sensor_notify(s_ezlopi_device_properties_t *properties, void *args)
{
    float acceleration_value = 0;
    float temperature_value = 0;
    // float gyroscope_value = 0;
    mpu6050_data_t data_val = {0};

    // read data from sensor
    i2c_mpu6050_sensor_read_sensor_data(properties, &data_val);

    // create json packet
    cJSON *cjson_properties = (cJSON *)args;
    if (cjson_properties)
    {
        if (ezlopi_item_name_acceleration_x_axis == properties->ezlopi_cloud.item_name)
        {
            acceleration_value = (data_val.ax);
            TRACE_I("This is data from Register: ax =  %.2f m/s^2", acceleration_value);
            cJSON_AddNumberToObject(cjson_properties, "value", acceleration_value);
            cJSON_AddStringToObject(cjson_properties, "scale", "meter_per_square_second");
        }
        if (ezlopi_item_name_acceleration_y_axis == properties->ezlopi_cloud.item_name)
        {
            acceleration_value = (data_val.ay);
            TRACE_I("This is data from Register: ay =  %.2f m/s^2", acceleration_value);
            cJSON_AddNumberToObject(cjson_properties, "value", acceleration_value);
            cJSON_AddStringToObject(cjson_properties, "scale", "meter_per_square_second");
        }
        if (ezlopi_item_name_acceleration_z_axis == properties->ezlopi_cloud.item_name)
        {
            acceleration_value = (data_val.az);
            TRACE_I("This is data from Register: az =  %.2f m/s^2", acceleration_value);
            cJSON_AddNumberToObject(cjson_properties, "value", acceleration_value);
            cJSON_AddStringToObject(cjson_properties, "scale", "meter_per_square_second");
        }
        if (category_temperature == properties->ezlopi_cloud.category)
        {
            temperature_value = (data_val.temp_mpu);
            TRACE_I("Temperature is: %.2f *C", temperature_value);
            cJSON_AddNumberToObject(cjson_properties, "value", temperature_value);
            cJSON_AddStringToObject(cjson_properties, "scale", "celsius");
        }

        // if (ezlopi_item_name_gyroscope_x_axis == properties->ezlopi_cloud.item_name)
        // {
        //     gyroscope_value = (data_val.gx);
        //     TRACE_I("This is data from Register: gx =  %.2f m/s^2", gyroscope_value);
        //     cJSON_AddNumberToObject(cjson_properties, "value", gyroscope_value);
        //     cJSON_AddStringToObject(cjson_properties, "scale", "meter_per_square_second");
        // }
        // if (ezlopi_item_name_gyroscope_y_axis == properties->ezlopi_cloud.item_name)
        // {
        //     gyroscope_value = (data_val.gy);
        //     TRACE_I("This is data from Register: gy =  %.2f m/s^2", gyroscope_value);
        //     cJSON_AddNumberToObject(cjson_properties, "value", gyroscope_value);
        //     cJSON_AddStringToObject(cjson_properties, "scale", "meter_per_square_second");
        // }
        // if (ezlopi_item_name_gyroscope_z_axis == properties->ezlopi_cloud.item_name)
        // {
        //     gyroscope_value = (data_val.gz);
        //     TRACE_I("This is data from Register: gz =  %.2f m/s^2", gyroscope_value);
        //     cJSON_AddNumberToObject(cjson_properties, "value", gyroscope_value);
        //     cJSON_AddStringToObject(cjson_properties, "scale", "meter_per_square_second");
        // }
    }
}

/***************************************  END  **********************************************************/