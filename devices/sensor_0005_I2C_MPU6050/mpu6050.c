
#include "trace.h"
#include "esp_err.h"
#include "sensor_0005_I2C_MPU6050.h"

static float acc_sen_calib_val = 0;
static float gyro_sen_calib_val = 0;

// Table mapping e_mpu6050_err_t enumerals to strings
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
// mapper function for MPU6050_err
static const char *mpu6050_err_to_str(e_mpu6050_err_t err)
{
    if (err > MPU6050_ERR_UNKNOWN)
    {
        return NULL;
    }
    else
    {
        return mpu6050_err_str[err];
    }
}

static e_mpu6050_err_t mpu6050_configure_power(l_ezlopi_item_t *item)
{
    e_mpu6050_err_t err = MPU6050_ERR_OK;
    if (item)
    {
        uint8_t write_buffer[] = {0x6B, 0x03};
        // uint8_t write_buffer[] = {REG_PWR_MGMT_1, PWR_MGMT_1_PLL_Z_AXIS_INTERNAL_CLK_REF};
        if (ESP_OK != ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, write_buffer, 2))
        {
            err = MPU6050_ERR_DRIVER_INSTALL_FAIL;
        }
        vTaskDelay(10);
    }
    return err;
}
static e_mpu6050_err_t mpu6050_configure_accelerometer(l_ezlopi_item_t *item, uint8_t flags)
{
    e_mpu6050_err_t err = MPU6050_ERR_OK;
    if (item)
    {
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
        if (ESP_OK != ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, write_buffer, 2))
        {
            err = MPU6050_ERR_DRIVER_INSTALL_FAIL;
        }
        vTaskDelay(10);
    }
    return err;
}
static e_mpu6050_err_t mpu6050_configure_gyroscope(l_ezlopi_item_t *item, uint8_t flags)
{
    e_mpu6050_err_t err = MPU6050_ERR_OK;
    if (item)
    {
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

        if (ESP_OK != ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, write_buffer, 2))
        {
            err = MPU6050_ERR_DRIVER_INSTALL_FAIL;
        }
        vTaskDelay(10);
    }
    return err;
}
static e_mpu6050_err_t mpu6050_configure_dlfp(l_ezlopi_item_t *item)
{
    e_mpu6050_err_t err = MPU6050_ERR_OK;
    if (item)
    {
        uint8_t write_buffer[] = {0x1A, 0x00};
        // uint8_t write_buffer[] = {REG_DLFP_CFG, DLFP_CFG_FILTER_0};
        if (ESP_OK != ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, write_buffer, 2))
        {
            err = MPU6050_ERR_DRIVER_INSTALL_FAIL;
        }
        vTaskDelay(10);
    }
    return err;
}
static e_mpu6050_err_t mpu6050_enable_interrupt(l_ezlopi_item_t *item)
{
    e_mpu6050_err_t err = MPU6050_ERR_OK;
    if (item)
    {
        uint8_t write_buffer[] = {0x38, 0x00};
        // uint8_t write_buffer[] = {REG_INTR_EN, INTR_EN_DATA_RDY};
        if (ESP_OK != ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, write_buffer, 2))
        {
            err = MPU6050_ERR_DRIVER_INSTALL_FAIL;
        }
        vTaskDelay(10);
    }
    return err;
}

// function to check for INTR bit before any data extraction is done from accel,temp & gyro registers
static e_mpu6050_err_t mpu6050_check_data_ready_INTR(l_ezlopi_item_t *item, uint8_t *temp)
{
    e_mpu6050_err_t err = MPU6050_ERR_OK;
    {                                               // Must request INTR_REG
        uint8_t write_buffer[] = {REG_INTR_STATUS}; // REG_INTR_STATUS;
        ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, write_buffer, 1);
        // Read -> INTR_BIT
        ezlopi_i2c_master_read_from_device(&item->interface.i2c_master, temp, 1);
        if (NULL != temp)
        {
            err = MPU6050_ERR_OK;
        }
        else
        {
            err = MPU6050_ERR_OPERATION_TIMEOUT;
        }
    }
    return err;
}

// function to initiate sensor data readings
void __mpu6050_get_data(l_ezlopi_item_t *item)
{
    if (item)
    {
        e_mpu6050_err_t err = MPU6050_ERR_OK;
        static s_raw_mpu6050_data_t RAW_DATA = {0};
        static uint8_t tmp_buf[REG_COUNT_LEN] = {0}; // 0 - 13
        uint8_t Check_Register = 0;
        uint8_t address_val = 0;

        s_mpu6050_data_t *user_data = (s_mpu6050_data_t *)item->user_arg;
        // Read specified FIFO buffer size (depends on configuration set)g
        for (uint8_t i = 0; i < REG_COUNT_LEN; i += 2)
        {
            if ((err = mpu6050_check_data_ready_INTR(item, &Check_Register)) == MPU6050_ERR_OK)
            {
                // if 'bit0' in INTR register is set ; then read procced to read :- acc,gyro & tmp registers
                if (Check_Register == BIT_0)
                { // ACCEL_X_H = 0x3B
                    address_val = (ACCEL_X_H + i);
                    ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, &address_val, 1);
                    ezlopi_i2c_master_read_from_device(&item->interface.i2c_master, (tmp_buf + i), 1);
                    // ACCEL_X_H = 0x3B +1
                    address_val = (ACCEL_X_H + i + 1);
                    ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, &address_val, 1);
                    ezlopi_i2c_master_read_from_device(&item->interface.i2c_master, (tmp_buf + i + 1), 1);
                }
            }
            else
            {
                TRACE_E("Data not ready ... Error type:- %d (%s)", err, mpu6050_err_to_str(err));
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

        user_data->ax = (RAW_DATA.raw_ax / acc_sen_calib_val) * MPU6050_STANDARD_G_TO_ACCEL_CONVERSION_VALUE; // in m/s^2
        user_data->ay = (RAW_DATA.raw_ay / acc_sen_calib_val) * MPU6050_STANDARD_G_TO_ACCEL_CONVERSION_VALUE;
        user_data->az = (RAW_DATA.raw_az / acc_sen_calib_val) * MPU6050_STANDARD_G_TO_ACCEL_CONVERSION_VALUE;
        user_data->gx = (RAW_DATA.raw_gx / gyro_sen_calib_val) - GYRO_X_OFFSET; // deg*/s
        user_data->gy = (RAW_DATA.raw_gy / gyro_sen_calib_val) - GYRO_Y_OFFSET; // deg*/s
        user_data->gz = (RAW_DATA.raw_gz / gyro_sen_calib_val) - GYRO_Z_OFFSET; // deg*/s
        user_data->tmp = ((RAW_DATA.raw_t / 340) + 36.530f);
        printf("%f\n", user_data->gx);
    }
}

e_mpu6050_err_t __mpu6050_config_device(l_ezlopi_item_t *item)
{
    e_mpu6050_err_t err = MPU6050_ERR_OK;
    if (item)
    {
        uint8_t flags = 0;

        // Initialize I2C
        // flags = PWR_MGMT_1_PLL_Z_AXIS_INTERNAL_CLK_REF; // choose PLL-z-axis; as internal clk refr
        if ((err = mpu6050_configure_power(item)) != MPU6050_ERR_OK)
        {
            TRACE_E("Initializtion unsuccessful %d", err);
            TRACE_E("%s", mpu6050_err_to_str(err));
            return err;
        }

        // Configure accelerometer sensitivity
        flags = A_CFG_2G; // 16384 steps
        if ((err = mpu6050_configure_accelerometer(item, flags)) != MPU6050_ERR_OK)
        {

            TRACE_E("Initializtion unsuccessful %d", err);
            TRACE_E("%s", mpu6050_err_to_str(err));
            return err;
        }

        // Configure gyro sensitivity
        flags = G_CFG_250; // +-250 deg/s
        if ((err = mpu6050_configure_gyroscope(item, flags)) != MPU6050_ERR_OK)
        {

            TRACE_E("Initializtion unsuccessful %d", err);
            TRACE_E("%s", mpu6050_err_to_str(err));
            return err;
        }

        // Configure the Digital-Low-Pass-Filter
        // flags = DLFP_CFG_FILTER_0; // default -> no dlfp -> 8Mhz clk ref
        if ((err = mpu6050_configure_dlfp(item)) != MPU6050_ERR_OK)
        {

            TRACE_E("Initializtion unsuccessful %d", err);
            TRACE_E("%s", mpu6050_err_to_str(err));
            return err;
        }

        // Enable interrupts after every sensor refresh
        // flags = INTR_EN_DATA_RDY;
        if ((err = mpu6050_enable_interrupt(item)) != MPU6050_ERR_OK) // DATA_RDY_EN = 1 //  occurs each-time a write operation to the sensor registers has been completed.
        {

            TRACE_E("Initializtion unsuccessful %d", err);
            TRACE_E("%s", mpu6050_err_to_str(err));
            return err;
        }
    }
    return err;
}

//----------------------------------------------------------------------------------
