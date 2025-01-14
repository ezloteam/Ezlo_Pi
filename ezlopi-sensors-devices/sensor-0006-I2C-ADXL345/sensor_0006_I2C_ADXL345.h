/* ===========================================================================
** Copyright (C) 2025 Ezlo Innovation Inc
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
 * @file    sensor_0006_I2C_ADXL345.h
 * @brief   perform some function on sensor_0006
 * @author  ezlopi_team_np
 * @version 0.1
 * @date    xx
 */

#ifndef _SENSOR_0006_I2C_ADXL345_H_
#define _SENSOR_0006_I2C_ADXL345_H_
/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "ezlopi_core_actions.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_errors.h"
/*******************************************************************************
 *                          C++ Declaration Wrapper
 *******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

    /*******************************************************************************
     *                          Type & Macro Declarations
     *******************************************************************************/
    // Datasheet can be found at:
    // https://www.sparkfun.com/datasheets/Sensors/Accelerometer/ADXL345.pdf

    // For reference following github repository can be consulted.
    // https://github.com/Embetronicx/incubator-nuttx-apps/tree/master/examples/etx_i2c

#define ADXL345_ADDR (0x53) // if (AD0->Vcc) then; [i2c_address->0x1D]
#define ADXL345_ODR_CNT 6
/*******************************************************************/
// REGISTER ADDRESS
/*******************************************************************/
#define ADXL345_DEVICE_ID_REGISTER (0x00)   // This is read-only register.
#define ADXL345_DATA_RATE (0x2C)            // This is read-write register.
#define ADXL345_DEVICE_POWER_CTRL (0x2D)    // This is read-write register.
#define ADXL345_INT_ENABLE_REGISTER (0x2E)  // [R+W] (check before grabing any data from registers)
#define ADXL345_DATA_FORMAT_REGISTER (0x31) // This is read-write register.
#define ADXL345_INT_SOURCE_REGISTER (0x30)  // [R] (check before grabing any data from registers)
#define ADXL345_DATA_X_0_REGISTER (0x32)    // [LSB] This is read-only register.  // little endian
#define ADXL345_DATA_X_1_REGISTER (0x33)    // [MSB] This is read-only register.
#define ADXL345_DATA_Y_0_REGISTER (0x34)    // [LSB] This is read-only register.
#define ADXL345_DATA_Y_1_REGISTER (0x35)    // [MSB] This is read-only register.
#define ADXL345_DATA_Z_0_REGISTER (0x36)    // [LSB] This is read-only register.
#define ADXL345_DATA_Z_1_REGISTER (0x37)    // [MSB] This is read-only register.

/*******************************************************************/
// Configuration values
/*******************************************************************/
// (HEX) Value to enable the "data_ready interrupt"(ie. bit7) in [0x2E—INT_ENABLE]
#define ADXL345_INT_EN (1 << 7)

// Data ready flag [ie. bit7 ] in "INT_SOURCE_REGISTER" (0x30)
#define ADXL345_DATA_READY_FLAG (1 << 7)

// Data for format register. Setting lower 4 bits only.
// D3 = 1; setting device data in full resolution, i.e 13-bits.
// D2 = 0; right justification. i.e. MSB left.
// (D1, D0) = (1, 1); for acceleration measurement up to 16g.
#define ADXL345_FORMAT_REGISTER_DATA (0x0B)

// This is defined to reset power control register, setting device in standby mode so that other device configurations can be done.
#define ADXL345_POWER_CTRL_RESET (0x00)

// This is defined to set power control register's measurement bit on so that device can initiate the
// measurement process after completion of device configuration.
#define ADXL345_POWER_CTRL_SET_TO_MEASUTEMENT (0x08)

/*******************************************************************/
// list of cosntants
/*******************************************************************/

// Following are the conversion factor of data according to the datahsheet.

// At high resolution, the conversion per LSB is 4mg/LSB.
#define ADXL345_CONVERTER_FACTOR_MG_TO_G 0.004

// 1g = 9.80665 m/s^2
#define ADXL345_STANDARD_G_TO_ACCEL_CONVERSION_VALUE 9.80665

    typedef struct s_adxl345_data
    {
        float acc_x;
        float acc_y;
        float acc_z;
    } s_adxl345_data_t;

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/

    /**
     * @brief Function to operate on actions
     *
     * @param action Current Action to Operate on
     * @param item Target-Item node
     * @param arg Arg for action
     * @param user_arg User-arg
     * @return ezlopi_error_t
     */
    ezlopi_error_t SENSOR_0006_i2c_adxl345(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

    /**
     * @brief Function to get 3-axis_value
     *
     * @param item Target Item
     */
    void ADXL345_get_axis_value(l_ezlopi_item_t *item);
    /**
     * @brief Function to configure adxl345 device
     *
     * @param item Target Item
     * @return ezlopi_error_t
     */
    ezlopi_error_t ADXL345_configure_device(l_ezlopi_item_t *item);

#ifdef __cplusplus
}
#endif

#endif // _SENSOR_0006_I2C_ADXL345_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
