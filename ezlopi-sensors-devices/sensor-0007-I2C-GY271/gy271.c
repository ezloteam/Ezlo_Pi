/* ===========================================================================
** Copyright (C) 2024 Ezlo Innovation Inc
**
** Under EZLO AVAILABLE SOURCE LICENSE (EASL) AGREEMENT
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/
/**
 * @file    gy271.c
 * @brief   perform some function on gy271
 * @author  xx
 * @version 0.1
 * @date    xx
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <math.h>
#include "esp_err.h"
#include "ezlopi_util_trace.h"
#include "time.h"

#include "ezlopi_core_errors.h"

#include "ezlopi_hal_i2c_master.h"
#include "sensor_0007_I2C_GY271.h"

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
static esp_err_t activate_set_reset_period(l_ezlopi_item_t *item);
static esp_err_t set_to_measure_mode(l_ezlopi_item_t *item);
static esp_err_t enable_data_ready_interrupt(l_ezlopi_item_t *item);
static int __gy271_Get_azimuth(float dx, float dy);
static void __gy271_correct_data(s_gy271_raw_data_t *RAW_DATA, s_gy271_data_t *user_data);
static esp_err_t __gy271_check_status(l_ezlopi_item_t *item, uint8_t *temp);
/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
ezlopi_error_t GY271_configure_init(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item)
    {
        ret = activate_set_reset_period(item);
        ret = set_to_measure_mode(item);
        ret = enable_data_ready_interrupt(item);
        ret = EZPI_SUCCESS;
    }
    return ret;
}

bool GY271_update_value(l_ezlopi_item_t *item)
{
    bool valid_data = false;
    if (item)
    {
        s_gy271_raw_data_t RAW_DATA = {0};
        static uint8_t buffer_0, buffer_1;           // tempr
        static uint8_t tmp_buf[GY271_REG_COUNT_LEN]; // axis
        volatile uint8_t Check_Register = 0;
        volatile uint8_t address_val = 0;
        esp_err_t err = ESP_OK;

        if (ESP_OK == (err = __gy271_check_status(item, (uint8_t *)&Check_Register)))
        {
            // TRACE_S(" (2) Check_reg_val @ 0x06H: {%#x}", Check_Register);
            if (Check_Register == GY271_DATA_OVERRUN_FLAG)
            {
                TRACE_W(" (2) Status :- Before DOR reset {%#x}", Check_Register);
                TRACE_W(" (2) :--*********- DOR bit set..**********...");
                address_val = (GY271_DATA_Z_MSB_REGISTER);
                EZPI_hal_i2c_master_write_to_device(&item->interface.i2c_master, (uint8_t *)&address_val, 1);
                EZPI_hal_i2c_master_read_from_device(&item->interface.i2c_master, (&buffer_1), 1);

                address_val = GY271_STATUS_REGISTER;
                EZPI_hal_i2c_master_write_to_device(&item->interface.i2c_master, (uint8_t *)&address_val, 1);
                EZPI_hal_i2c_master_read_from_device(&item->interface.i2c_master, (uint8_t *)(&Check_Register), 1);
                TRACE_W(" (2) Status :- after DOR reset {%#x}", Check_Register);
                TRACE_W(" (2) :--*********- DOR bit reset **********...");
            }

            if (Check_Register & GY271_DATA_READY_FLAG)
            {
                // TRACE_S(" (2) ...[07H & 08H] reading started....");
                address_val = GY271_DATA_TEMP_LSB_REGISTER;
                EZPI_hal_i2c_master_write_to_device(&item->interface.i2c_master, (uint8_t *)&address_val, 1);
                EZPI_hal_i2c_master_read_from_device(&item->interface.i2c_master, (&buffer_0), 1);
                address_val = GY271_DATA_TEMP_MSB_REGISTER;
                EZPI_hal_i2c_master_write_to_device(&item->interface.i2c_master, (uint8_t *)&address_val, 1);
                EZPI_hal_i2c_master_read_from_device(&item->interface.i2c_master, (&buffer_1), 1);

                // TRACE_S(" (2) ...[00~05H] reading started....");
                address_val = GY271_DATA_X_LSB_REGISTER;
                EZPI_hal_i2c_master_write_to_device(&item->interface.i2c_master, (uint8_t *)&address_val, 1);
                EZPI_hal_i2c_master_read_from_device(&item->interface.i2c_master, (tmp_buf), GY271_REG_COUNT_LEN);

                // now read the status byte 0x06H
                address_val = GY271_STATUS_REGISTER;
                EZPI_hal_i2c_master_write_to_device(&item->interface.i2c_master, (uint8_t *)&address_val, 1);
                EZPI_hal_i2c_master_read_from_device(&item->interface.i2c_master, (uint8_t *)(&Check_Register), 1);
                // TRACE_S(" (2) :--- 06H reading ended....");
                // TRACE_S(" (2) :--- Check_reg_val @ 0x06H : {%#x}", Check_Register);

                valid_data = true;
            }
            else
            {
                TRACE_E(" Data Not Ready ...");
                for (uint8_t try = 20; (try > 0) && (!(Check_Register & GY271_DATA_READY_FLAG)); try--)
                {
                    address_val = GY271_STATUS_REGISTER;
                    EZPI_hal_i2c_master_write_to_device(&item->interface.i2c_master, (uint8_t *)&address_val, 1);
                    EZPI_hal_i2c_master_read_from_device(&item->interface.i2c_master, (uint8_t *)(&Check_Register), 1);

                    if ((Check_Register & GY271_DATA_READY_FLAG)) // 1.5 sec
                    {
                        TRACE_W("Reg @ 0x06H: -> {%#x}", Check_Register);
                        break;
                    }
                    TRACE_W("Extracting.... Reg @ 0x06H: -> {%#x}", Check_Register);
                }
            }
        }
        else
        {
            TRACE_E(".......... Unable to access 'registers'. ");
        }

        if (valid_data)
        {
            RAW_DATA.raw_x = (int16_t)(tmp_buf[1] << 8 | tmp_buf[0]);  // x_axis =  0x01 [msb]  & 0x00 [lsb]
            RAW_DATA.raw_y = (int16_t)(tmp_buf[3] << 8 | tmp_buf[2]);  // y_axis =  0x03        & 0x02
            RAW_DATA.raw_z = (int16_t)(tmp_buf[5] << 8 | tmp_buf[4]);  // z_axis =  0x05        & 0x04
            RAW_DATA.raw_temp = (int16_t)((buffer_1 << 8) | buffer_0); // tempr  =  buffer1     & buffer0
            __gy271_correct_data(&RAW_DATA, (s_gy271_data_t *)(item->user_arg));
        }
        else
        {
            TRACE_E("......................Invalid data generated.\n");
        }
    }
    return valid_data;
}

void GY271_get_raw_max_min_values(l_ezlopi_item_t *item, int (*calibrationData)[2])
{
    if (item)
    {
        //------------------------------------------------------------------------------
        int x = 0, y = 0, z = 0;
        uint8_t buffer_1;                               // tempr
        uint8_t cal_tmp_buf[GY271_REG_COUNT_LEN] = {0}; // axis
        uint8_t Check_Register;
        uint8_t address_val;
        esp_err_t err = ESP_OK;
        if (ESP_OK == (err = __gy271_check_status(item, &Check_Register)))
        {
            // if 'bit0' in INTR register is set ; then read procced to read :- magnetometer registers
            if (Check_Register == GY271_DATA_OVERRUN_FLAG)
            {
                TRACE_W(" 1. FIRST_INIT_CALIB :--*********- DOR bit set..**********...");
                TRACE_S(" 1. Status :- Before DOR reset : {%#x}", Check_Register);
                address_val = (GY271_DATA_Z_MSB_REGISTER);
                EZPI_hal_i2c_master_write_to_device(&item->interface.i2c_master, &address_val, 1);
                EZPI_hal_i2c_master_read_from_device(&item->interface.i2c_master, (&buffer_1), 1);

                address_val = GY271_STATUS_REGISTER;
                EZPI_hal_i2c_master_write_to_device(&item->interface.i2c_master, &address_val, 1);
                EZPI_hal_i2c_master_read_from_device(&item->interface.i2c_master, (&Check_Register), 1);
                TRACE_S(" 1. Status :- After DOR reset {%#x}", Check_Register);
                TRACE_W(" 1. FIRST_INIT_CALIB :--*********- DOR bit reset **********...");
            }
            if (Check_Register & GY271_DATA_READY_FLAG)
            {
                TRACE_W(" 1. FIRST_INIT_CALIB :--- Check_reg_val @ 0x06H: {%#x}", Check_Register);
                TRACE_S(" 1. FIRST_INIT_CALIB :--- [00~05H] reading started....");

                address_val = (GY271_DATA_X_LSB_REGISTER);
                EZPI_hal_i2c_master_write_to_device(&item->interface.i2c_master, &address_val, 1);
                EZPI_hal_i2c_master_read_from_device(&item->interface.i2c_master, (cal_tmp_buf), GY271_REG_COUNT_LEN);

                address_val = GY271_STATUS_REGISTER;
                EZPI_hal_i2c_master_write_to_device(&item->interface.i2c_master, &address_val, 1);
                EZPI_hal_i2c_master_read_from_device(&item->interface.i2c_master, (&Check_Register), 1);
                TRACE_S(" 1. FIRST_INIT_CALIB :--- 06H reading ended....");
                TRACE_W(" 1. FIRST_INIT_CALIB :--- Check_reg_val @ 0x06H : {%#x}", Check_Register);
            }
        }
        else
        {
            TRACE_W("Data not ready ... ['__gy271_check_status' = ESP_FAIL]");
        }

        // generate the raw_axis_values
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
        TRACE_I("Calibrated :--- Xmin=%6d | Xmax=%6d | Ymin=%6d | Ymax=%6d | Zmin=%6d | Zmax=%6d \n",
                calibrationData[0][0],
                calibrationData[0][1],
                calibrationData[1][0],
                calibrationData[1][1],
                calibrationData[2][0],
                calibrationData[2][1]);
    }
}

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/

static esp_err_t activate_set_reset_period(l_ezlopi_item_t *item)
{
    esp_err_t ret = ESP_FAIL;
    if (item)
    {
        uint8_t write_buffer1[2] = {0x0A, 0x80};
        ret = EZPI_hal_i2c_master_write_to_device(&item->interface.i2c_master, write_buffer1, 2);
        uint8_t write_buffer2[2] = {GY271_SET_RESET_PERIOD_REGISTER, GY271_DEFAULT_SET_RESET_PERIOD}; // REG_INTR_STATUS;
        ret = EZPI_hal_i2c_master_write_to_device(&item->interface.i2c_master, write_buffer2, 2);
    }
    return ret;
}
static esp_err_t set_to_measure_mode(l_ezlopi_item_t *item)
{
    esp_err_t ret = ESP_FAIL;
    if (item)
    {
        uint8_t write_buffer[2] = {GY271_CONTROL_REGISTER_1, GY271_OPERATION_MODE1};
        ret = EZPI_hal_i2c_master_write_to_device(&item->interface.i2c_master, write_buffer, 2);
    }
    return ret;
}
static esp_err_t enable_data_ready_interrupt(l_ezlopi_item_t *item)
{
    esp_err_t ret = ESP_FAIL;
    if (item)
    {
        uint8_t write_buffer[2] = {GY271_CONTROL_REGISTER_2, GY271_OPERATION_MODE2};
        ret = EZPI_hal_i2c_master_write_to_device(&item->interface.i2c_master, write_buffer, 2);
    }
    return ret;
}

static int __gy271_Get_azimuth(float dx, float dy)
{
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
    user_data->azimuth = __gy271_Get_azimuth((user_data->calib_factor.calibrated_axis[0]),
                                             (user_data->calib_factor.calibrated_axis[1]));

    // TRACE_W("Final Data :--- X = %.2fG | Y = %.2fG | Z = %.2fG | T = %.2f*C | AZI = %d*deg ",
    //         user_data->X,
    //         user_data->Y,
    //         user_data->Z,
    //         user_data->T,
    //         user_data->azimuth);
}

static esp_err_t __gy271_check_status(l_ezlopi_item_t *item, uint8_t *temp)
{
    esp_err_t err = ESP_OK;
    uint8_t write_buffer[] = {GY271_STATUS_REGISTER};
    EZPI_hal_i2c_master_write_to_device(&item->interface.i2c_master, write_buffer, 1);
    EZPI_hal_i2c_master_read_from_device(&item->interface.i2c_master, temp, 1);
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

/*******************************************************************************
 *                          End of File
 *******************************************************************************/