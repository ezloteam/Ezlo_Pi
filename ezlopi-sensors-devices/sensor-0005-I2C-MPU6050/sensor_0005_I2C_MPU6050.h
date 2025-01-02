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
 * @file    sensor_0005_I2C_MPU6050.h
 * @brief   perform some function on sensor_0005
 * @author  xx
 * @version 0.1
 * @date    xx
*/

#ifndef _SENSOR_0005_I2C_MPU6050_H_
#define _SENSOR_0005_I2C_MPU6050_H_


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

#define MPU6050_REG_COUNT_LEN 14 // the accelerometer , temperature and gyrodata is to be in one go .

    // #define RECALIBRATE_ITERAION_COUNT 1000
#define CALIBRATION_SAMPLES 200

#define MPU6050_STANDARD_G_TO_ACCEL_CONVERSION_VALUE (9.80665f) // Offsets and conversion values : 1g = 9.80665 m/s^2

// defination to allow the i2c to check for ack
#define MPU6050_ADDR (0x68)

//-------------------------
// CUSTOM BIT MASKS
//-------------------------
// defination for bit
#define BIT_0 (1 << 0)
#define BIT_1 (1 << 1)
#define BIT_2 (1 << 2)
#define BIT_3 (1 << 3)
#define BIT_4 (1 << 4)
#define BIT_5 (1 << 5)
#define BIT_6 (1 << 6)
#define BIT_7 (1 << 7)

//-------------------------
// #REGISTER ADDRESSES
//-------------------------
// [R] List of Accelerometer Register Address
#define ACCEL_X_H (0x3B)
#define ACCEL_X_L (0x3C)
#define ACCEL_Y_H (0x3D)
#define ACCEL_Y_L (0x3E)
#define ACCEL_Z_H (0x3F)
#define ACCEL_Z_L (0x40)
// [R] List of Temperature Register Address
#define TEMP_H (0x41)
#define TEMP_L (0x42)
// [R] List of Gyrometer Register Address
#define GYRO_X_H (0x43)
#define GYRO_X_L (0x44)
#define GYRO_Y_H (0x45)
#define GYRO_Y_L (0x46)
#define GYRO_Z_H (0x47)
#define GYRO_Z_L (0x48)
// List of Configuration Register Address
#define REG_SAMPLE_RATE_DIV (0x19) // [R+W] Sample-rate divider register address [Register 25]
#define REG_DLFP_CFG (0x1A)        // [R+W] DLFP configuration register [Register 26]
#define REG_G_CFG (0x1B)           // [R+W] Gyrometer configuration register [Register 27]
#define REG_A_CFG (0x1C)           // [R+W] Accelerometer configuration register [Register 28]
#define REG_INTR_EN (0x38)         // [R+W] Interrupt enable register [Register 56]
#define REG_INTR_STATUS (0x3A)     //   [R] Interrupt status register [Register 58]
#define REG_PWR_MGMT_1 (0x6B)      // [R+W] Power management(1) register [Register 107]

//-------------------------
// #CONFIGURATION MODES
//-------------------------
// DLFP configuration + mode masks
#define DLFP_CFG_FILTER_0 (0x00)          // A{260Hz,0.0ms} G{256Hz 0.98ms} Fs=8kHz
#define DLFP_CFG_FILTER_1 (BIT_0)         // A{184Hz,2.0ms} G{188Hz 1.9ms}  Fs=1kHz
#define DLFP_CFG_FILTER_2 (BIT_1)         // A{94Hz, 3.0ms} G{98Hz  2.8ms}  Fs=1kHz
#define DLFP_CFG_FILTER_3 (BIT_1 | BIT_0) // A{44Hz, 4.9ms} G{42Hz, 4.8ms}  Fs=1kHz
#define DLFP_CFG_FILTER_4 (BIT_2)         // A{21Hz, 8.5ms} G{20Hz, 8.3ms}  Fs=1kHz
#define DLFP_CFG_FILTER_5 (BIT_2 | BIT_0) // A{10Hz,13.8ms} G{10Hz,13.4ms}  Fs=1kHz
#define DLFP_CFG_FILTER_6 (BIT_2 | BIT_1) // A{ 5Hz,19.0ms} G{ 5Hz,18.6ms}  Fs=1kHz

// Gyrometer configuration + mode masks
#define G_CFG_250 (0x00)           //[+-250 deg/s]
#define G_CFG_500 (BIT_3)          //[+-500 deg/s]
#define G_CFG_1000 (BIT_4)         //[+-1000 deg/s]
#define G_CFG_2000 (BIT_3 | BIT_4) //[+-2000 deg/s]

// Accelerometer configuration + mode masks
#define A_CFG_2G (0x00)           //[+-2G]
#define A_CFG_4G (BIT_3)          //[+-4G]
#define A_CFG_8G (BIT_4)          //[+-8G]
#define A_CFG_16G (BIT_3 | BIT_4) //[+-16G]

// Interrupt enable configuration + mode masks
#define INTR_EN_DATA_RDY (BIT_0)  // When set; this bit enables the Data Ready interrupt, which occurs each time a write operation to all of the sensor registers has been completed.
#define DATA_RDY_INT_FLAG (BIT_0) // when intr is generated this flag is high

// Power management (1) configuration + mode masks
#define PWR_MGMT_1_RESET (BIT_7)                               // When set, this bit resets all internal registers to their default values.(automatically clears to 0)
#define PWR_MGMT_1_SLEEP (BIT_6)                               // When set, this bit puts MPU6050 into low power sleep mode
#define PWR_MGMT_1_PLL_X_AXIS_INTERNAL_CLK_REF (BIT_0)         // When set, choose PLL with X axis gyroscope reference
#define PWR_MGMT_1_PLL_Y_AXIS_INTERNAL_CLK_REF (BIT_1)         // When set, choose PLL with Y axis gyroscope reference
#define PWR_MGMT_1_PLL_Z_AXIS_INTERNAL_CLK_REF (BIT_1 | BIT_0) // When set, choose PLL with Z axis gyroscope reference


//-----------------------------
// #CUSTOM DATA STORAGE STRUCTS
//-----------------------------

// #### Custom structure to store processed data
    typedef struct s_mpu6050_data
    {
        bool calibration_complete;
        // uint16_t extract_counts;
        float tmp;
        float ax, ay, az; // m/s2
        float gx, gy, gz; // rpm
        float gyro_x_offset, gyro_y_offset, gyro_z_offset; // raw
        float acc_mpu6050_calib_val;
        float gyro_mpu6050_calib_val;
    } s_mpu6050_data_t;

    // #### Custom structure to store raw data
    typedef struct s_raw_mpu6050_data
    {
        int16_t raw_t;
        int16_t raw_ax, raw_ay, raw_az;
        int16_t raw_gx, raw_gy, raw_gz;
    } s_raw_mpu6050_data_t;

    //-----------------------------------------
    // #### Enumeration of MPU6050 error types
    //-----------------------------------------
    typedef enum e_mpu6050_err
    {
        MPU6050_ERR_OK = 0,              // No error
        MPU6050_ERR_PARAM_CFG_FAIL,      // i2c_param_config() error
        MPU6050_ERR_DRIVER_INSTALL_FAIL, // i2c_driver_install() error
        MPU6050_ERR_INVALID_ARGUMENT,    // invalid parameter to function
        MPU6050_ERR_NO_SLAVE_ACK,        // No acknowledgment from slave
        MPU6050_ERR_INVALID_STATE,       // Driver not installed / not i2c master
        MPU6050_ERR_OPERATION_TIMEOUT,   // Bus busy,
        MPU6050_ERR_UNKNOWN,             // Unknown error
        MPU6050_ERR_MAX
    } e_mpu6050_err_t;

#if 0
    // typedef enum e_sensor_i2c_mpu6050_item_ids
    // {
    //     SENSOR_I2C_MPU6050_ITEM_ACCELERATION_X,
    //     SENSOR_I2C_MPU6050_ITEM_ACCELERATION_Y,
    //     SENSOR_I2C_MPU6050_ITEM_ACCELERATION_Z,
    //     SENSOR_I2C_MPU6050_ITEM_TEMPERATURE,
    //     SENSOR_I2C_MPU6050_ITEM_GYRO_X,
    //     SENSOR_I2C_MPU6050_ITEM_GYRO_Y,
    //     SENSOR_I2C_MPU6050_ITEM_GYRO_Z,
    //     SENSOR_I2C_MPU6050_ITEM_MAX
    // } e_sensor_i2c_mpu6050_item_ids_t;
#endif

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
    ezlopi_error_t SENSOR_0005_i2c_mpu6050(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

    /**
     * @brief Function to get mpu6050 data
     *
     * @param item Target Item
     */
    void MPU6050_get_data(l_ezlopi_item_t *item);

    /**
     * @brief Function to configure device
     *
     * @param item Target Item
     * @return e_mpu6050_err_t
     */
    e_mpu6050_err_t MPU6050_config_device(l_ezlopi_item_t *item);



#ifdef __cplusplus
}
#endif

#endif //_SENSOR_0005_I2C_MPU6050_H_

/*******************************************************************************
*                          End of File
*******************************************************************************/