#include "math.h"
#include "trace.h"
#include "ezlopi_i2c_master.h"
#include "sensor_0007_I2C_GY271.h"

static esp_err_t activate_set_reset_period(l_ezlopi_item_t *item)
{
    esp_err_t ret = ESP_FAIL;
    if (item)
    {
        uint8_t write_buffer[] = {GY271_SET_RESET_PERIOD_REGISTER, GY271_DEFAULT_SET_RESET_PERIOD}; // REG_INTR_STATUS;
        ret = ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, write_buffer, 2);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    return ret;
}
static esp_err_t set_to_measure_mode(l_ezlopi_item_t *item)
{
    esp_err_t ret = ESP_FAIL;
    if (item)
    {
        uint8_t write_byte[] = {GY271_CONTROL_REGISTER_1, GY271_OPERATION_MODE};
        ret = ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, write_byte, 2);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    return ret;
}
static esp_err_t enable_data_ready_interrupt(l_ezlopi_item_t *item)
{
    esp_err_t ret = ESP_FAIL;
    if (item)
    {
        uint8_t write_byte[] = {GY271_CONTROL_REGISTER_2, GY271_INT_EN};
        ret = ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, write_byte, 2);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    return ret;
}

static int __gy271_Get_azimuth(float dx, float dy)
{
    // calculate azimuth
    int azi = (int)((float)(180.0 * atan2(dy, dx)) / PI);
    return ((azi <= 0) ? (azi += 360) : (azi));
}

static void __gy271_correct_data(s_gy271_raw_data_t *RAW_DATA, s_gy271_data_t *user_data)
{
    // update the calibration parameters
    user_data->calib_factor.calibrated_axis[0] = (user_data->calib_factor.scale_axis[0]) *
                                                 (float)(((long)RAW_DATA->raw_x) - (user_data->calib_factor.bias_axis[0])); // x-axis value
    user_data->calib_factor.calibrated_axis[1] = (user_data->calib_factor.scale_axis[1]) *
                                                 (float)(((long)RAW_DATA->raw_y) - (user_data->calib_factor.bias_axis[1])); // y-axis value
    user_data->calib_factor.calibrated_axis[2] = (user_data->calib_factor.scale_axis[2]) *
                                                 (float)(((long)RAW_DATA->raw_z) - (user_data->calib_factor.bias_axis[2])); // z-axis value

    // store the final data
    user_data->X = ((user_data->calib_factor.calibrated_axis[0]) / GY271_CONVERSION_TO_G);
    user_data->Y = ((user_data->calib_factor.calibrated_axis[1]) / GY271_CONVERSION_TO_G);
    user_data->Z = ((user_data->calib_factor.calibrated_axis[2]) / GY271_CONVERSION_TO_G);
    user_data->T = (((float)RAW_DATA->raw_temp) / GY271_TEMPERATURE_SENSITIVITY) + 32.53f;
    // calculate azimuth
    user_data->azimuth = __gy271_Get_azimuth((user_data->calib_factor.calibrated_axis[0]),
                                             (user_data->calib_factor.calibrated_axis[1]));
}

// function to check for INTR bit [in 0x06H], before any data extraction is done from data registers
static esp_err_t __gy271_check_INTR(l_ezlopi_item_t *item, uint8_t *temp)
{
    esp_err_t err = ESP_OK;
    // Must request INTR_REG
    uint8_t write_buffer[] = {GY271_STATUS_REGISTER}; // REG_INTR_STATUS;
    ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, write_buffer, 1);
    // Read -> INTR_BIT
    ezlopi_i2c_master_read_from_device(&item->interface.i2c_master, temp, 1);
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

int __gy271_configure(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (item)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS); // 100 ms
        ESP_ERROR_CHECK_WITHOUT_ABORT(activate_set_reset_period(item));
        ESP_ERROR_CHECK_WITHOUT_ABORT(set_to_measure_mode(item));
        ESP_ERROR_CHECK_WITHOUT_ABORT(enable_data_ready_interrupt(item));
        ret = 1;
    }
    return ret;
}

bool __gy271_update_value(l_ezlopi_item_t *item)
{
    bool valid_data = false;
    if (item)
    {
        static s_gy271_raw_data_t RAW_DATA = {0};
        static uint8_t tmp_buf[REG_COUNT_LEN] = {0}; // axis
        static uint8_t buffer_0, buffer_1;           // tempr
        uint8_t Check_Register;
        uint8_t address_val;
        esp_err_t err = ESP_OK;
        // Read specified FIFO buffer size (depends on configuration set)g

        if (ESP_OK == (err = __gy271_check_INTR(item, &Check_Register)))
        {

            // if 'bit0' in INTR register is set ; then read procced to read :- magnetometer registers
            if (Check_Register == GY271_DATA_SKIP_FLAG)
            {
                TRACE_W(" 2. Check_reg_val : {%#x}", Check_Register);
                TRACE_W(" 2. ******* DOR bit set. *******....");
                address_val = (GY271_DATA_Z_LSB_REGISTER);
                ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, &address_val, 1);
                ezlopi_i2c_master_read_from_device(&item->interface.i2c_master, (&buffer_0), 1);
                address_val = (GY271_DATA_Z_MSB_REGISTER);
                ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, &address_val, 1);
                ezlopi_i2c_master_read_from_device(&item->interface.i2c_master, (&buffer_1), 1);

                address_val = GY271_STATUS_REGISTER;
                ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, &address_val, 1);
                ezlopi_i2c_master_read_from_device(&item->interface.i2c_master, (&Check_Register), 1);
                TRACE_W(" 2. {%#x}", Check_Register);
                TRACE_W(" 2.  ****** DOR bit set.n *******...");
                if ((0 != Check_Register) | (1 != Check_Register))
                {
                    valid_data = false;
                }
            }
            if (Check_Register & GY271_DATA_READY_FLAG)
            {
                TRACE_W(" 2. Check_reg_val @ 0x00H: {%#x}", Check_Register);
                TRACE_I(" 2. 00H reading started....");
                // read the axis data
                for (uint8_t i = 0; i < (REG_COUNT_LEN); i += 2)
                {
                    // GY271_DATA_X_LSB_REGISTER = 0x00
                    address_val = (GY271_DATA_X_LSB_REGISTER + i);
                    ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, &address_val, 1);
                    ezlopi_i2c_master_read_from_device(&item->interface.i2c_master, (tmp_buf + i), 1);
                    // GY271_DATA_X_LSB_REGISTER = 0x00 +1
                    address_val = (GY271_DATA_X_LSB_REGISTER + i + 1);
                    ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, &address_val, 1);
                    ezlopi_i2c_master_read_from_device(&item->interface.i2c_master, (tmp_buf + i + 1), 1);
                }
                // read temperature data
                address_val = GY271_DATA_TEMP_LSB_REGISTER;
                ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, &address_val, 1);
                ezlopi_i2c_master_read_from_device(&item->interface.i2c_master, (&buffer_0), 1);
                address_val = GY271_DATA_TEMP_MSB_REGISTER;
                ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, &address_val, 1);
                ezlopi_i2c_master_read_from_device(&item->interface.i2c_master, (&buffer_1), 1);
                // now read the status byte 0x06H
                address_val = GY271_STATUS_REGISTER;
                ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, &address_val, 1);
                ezlopi_i2c_master_read_from_device(&item->interface.i2c_master, (&Check_Register), 1);
                TRACE_I(" 2. 06H reading ended....");
                TRACE_W(" 2. Check_reg_val @ 0x06H: {%#x}", Check_Register);
                if ((0 == Check_Register) | (1 == Check_Register))
                {
                    valid_data = true;
                }
            }
        }
        else
        {
            TRACE_W("Data not ready ... Error type:- %d ", err);
        }

        // proceed to replace the old data ; if generated data is valid
        if (valid_data)
        {
            TRACE_B("Valid data generated............. writing correct data......");
            // Configure data structure // total 8 bytes
            RAW_DATA.raw_x = (int16_t)(tmp_buf[1] << 8 | tmp_buf[0]);  // x_axis =  0x01 [msb]  & 0x00 [lsb]
            RAW_DATA.raw_y = (int16_t)(tmp_buf[3] << 8 | tmp_buf[2]);  // y_axis =  0x03        & 0x02
            RAW_DATA.raw_z = (int16_t)(tmp_buf[5] << 8 | tmp_buf[4]);  // z_axis =  0x05        & 0x04
            RAW_DATA.raw_temp = (int16_t)((buffer_1 << 8) | buffer_0); // tempr  =  buffer1     & buffer0
            __gy271_correct_data(&RAW_DATA, (s_gy271_data_t *)(item->user_arg));
        }
        else
        {
            TRACE_B("......................................Invalid data generated.");
        }
    }
    return valid_data;
}

void __gy271_get_raw_max_min_values(l_ezlopi_item_t *item, int (*calibrationData)[2])
{
    if (item)
    {
        //------------------------------------------------------------------------------
        int x = 0, y = 0, z = 0;
        uint8_t buffer_0, buffer_1;               // tempr
        uint8_t cal_tmp_buf[REG_COUNT_LEN] = {0}; // axis
        uint8_t Check_Register;
        uint8_t address_val;
        esp_err_t err = ESP_OK;
        if (ESP_OK == (err = __gy271_check_INTR(item, &Check_Register)))
        {
            // if 'bit0' in INTR register is set ; then read procced to read :- magnetometer registers
            if (Check_Register == GY271_DATA_SKIP_FLAG)
            {
                TRACE_W(" 1. FIRST_INIT_CALIB :--- Check_reg_val : {%#x}", Check_Register);
                TRACE_W(" 1. FIRST_INIT_CALIB :--*********- DOR bit set..**********...");
                address_val = (GY271_DATA_Z_LSB_REGISTER);
                ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, &address_val, 1);
                ezlopi_i2c_master_read_from_device(&item->interface.i2c_master, (&buffer_0), 1);
                address_val = (GY271_DATA_Z_MSB_REGISTER);
                ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, &address_val, 1);
                ezlopi_i2c_master_read_from_device(&item->interface.i2c_master, (&buffer_1), 1);

                address_val = GY271_STATUS_REGISTER;
                ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, &address_val, 1);
                ezlopi_i2c_master_read_from_device(&item->interface.i2c_master, (&Check_Register), 1);
                TRACE_W(" 1.{%#x}", Check_Register);
                TRACE_W(" 1. FIRST_INIT_CALIB :--*********- DOR bit set..**********...");
            }
            if (Check_Register & GY271_DATA_READY_FLAG)
            {
                TRACE_W(" 1. FIRST_INIT_CALIB :--- Check_reg_val @ 0x00H: {%#x}", Check_Register);
                TRACE_I(" 1. FIRST_INIT_CALIB :--- 00H reading started....");
                // read the axis data
                for (uint8_t i = 0; i < (REG_COUNT_LEN); i += 2)
                {
                    // GY271_DATA_X_LSB_REGISTER = 0x00
                    address_val = (GY271_DATA_X_LSB_REGISTER + i);
                    ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, &address_val, 1);
                    ezlopi_i2c_master_read_from_device(&item->interface.i2c_master, (cal_tmp_buf + i), 1);
                    // GY271_DATA_X_LSB_REGISTER = 0x00 +1
                    address_val = (GY271_DATA_X_LSB_REGISTER + i + 1);
                    ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, &address_val, 1);
                    ezlopi_i2c_master_read_from_device(&item->interface.i2c_master, (cal_tmp_buf + i + 1), 1);
                }
                // now read the status byte 0x06H
                address_val = GY271_STATUS_REGISTER;
                ezlopi_i2c_master_write_to_device(&item->interface.i2c_master, &address_val, 1);
                ezlopi_i2c_master_read_from_device(&item->interface.i2c_master, (&Check_Register), 1);
                TRACE_I(" 1. FIRST_INIT_CALIB :--- 06H reading ended....");
                TRACE_W(" 1. FIRST_INIT_CALIB :--- Check_reg_val @ 0x06H : {%#x}", Check_Register);
            }
        }
        else
        {
            TRACE_W("Data not ready ... Error type:- %d ", err);
        }

        // generate the raw_axis_values
        // Configure data structure // total 8 bytes
        x = (int16_t)(cal_tmp_buf[1] << 8 | cal_tmp_buf[0]); // x_axis =  0x01 [msb]  & 0x00 [lsb]
        y = (int16_t)(cal_tmp_buf[3] << 8 | cal_tmp_buf[2]); // y_axis =  0x03        & 0x02
        z = (int16_t)(cal_tmp_buf[5] << 8 | cal_tmp_buf[4]); // z_axis =  0x05        & 0x04

        //------------------------------------------------------------------------------
        if (x < (calibrationData[0][0])) // xmin
        {
            (calibrationData[0][0]) = x;
        }
        if (x > (calibrationData[0][1])) // xmax
        {
            (calibrationData[0][1]) = x;
        }
        if (y < (calibrationData[1][0])) // ymin
        {
            (calibrationData[1][0]) = y;
        }
        if (y > (calibrationData[1][1])) // ymax
        {
            (calibrationData[1][1]) = y;
        }
        if (z < (calibrationData[2][0])) // zmin
        {
            (calibrationData[2][0]) = z;
        }
        if (z > (calibrationData[2][1])) // zmax
        {
            (calibrationData[2][1]) = z;
        }
        //------------------------------------------------------------------------------
        TRACE_B("Calibrated :--- Xmin=%6d | Xmax=%6d | Ymin=%6d | Ymax=%6d | Zmin=%6d | Zmax=%6d ",
                calibrationData[0][0],
                calibrationData[0][1],
                calibrationData[1][0],
                calibrationData[1][1],
                calibrationData[2][0],
                calibrationData[2][1]);
    }
}
