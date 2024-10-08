#ifndef _SENSOR_0007_I2C_GY271_H_
#define _SENSOR_0007_I2C_GY271_H_

#include <stdbool.h>

#include "ezlopi_core_actions.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_errors.h"

// Calculation parameters
#define PI (3.1416f)
#define GY271_REG_COUNT_LEN 6 // magnetometer data is to be read in one go .
#define GY271_ADDR 0x0D // this chipset uses [QMC5883L chip]
/*******************************************************************/
// REGISTER ADDRESS
/*******************************************************************/
//
// 1. OUTPUT DATA REGISTERS . 16-bit  -> 2^16 = [-32768 ; +32767]
#define GY271_DATA_X_LSB_REGISTER 0x00 // [R] [LSB] This is read-only register.  // little endian
#define GY271_DATA_X_MSB_REGISTER 0x01 // [R] [MSB] This is read-only register.  // MSB contains (+-sign)
#define GY271_DATA_Y_LSB_REGISTER 0x02 // [R] [LSB] This is read-only register.  //
#define GY271_DATA_Y_MSB_REGISTER 0x03 // [R] [MSB] This is read-only register.
#define GY271_DATA_Z_LSB_REGISTER 0x04 // [R] [LSB] This is read-only register.
#define GY271_DATA_Z_MSB_REGISTER 0x05 // [R] [MSB] This is read-only register.

#define GY271_STATUS_REGISTER 0x06 // [R] This is read-only register.

#define GY271_DATA_TEMP_LSB_REGISTER 0x07 // [R] This is read-only register.  // sensitivity 100 LSB/c
#define GY271_DATA_TEMP_MSB_REGISTER 0x08

#define GY271_CONTROL_REGISTER_1 0x09 // [R+W]  // MODE ; ODR ; RNG ; OSR
#define GY271_CONTROL_REGISTER_2 0x0A // [R+W]  // Soft-reset ; roll-over pointer ; INT_EN

#define GY271_SET_RESET_PERIOD_REGISTER 0x0B // [R+W]   // 8-bit reset period value

/*******************************************************************/
// Configuration values
/*******************************************************************/
// Mode Control (MODE)
#define GY271_MODE_STDBY (0x00)
#define GY271_MODE_CONT (1 << 0)
// Output Data Rate (ODR)
#define GY271_ODR_10hz (0x00)
#define GY271_ODR_50hz (1 << 2)
#define GY271_ODR_100hz (1 << 3)
#define GY271_ODR_200hz (1 << 3) | (1 << 2)
// Full Scale Range (RNG)
#define GY271_RNG_2G (0x00)
#define GY271_RNG_8G (1 << 4)
// Over Sample Ratio (OSR)
#define GY271_OSR_512 (0x00)
#define GY271_OSR_256 (1 << 6)
#define GY271_OSR_128 (1 << 7)
#define GY271_OSR_64 (1 << 7) | (1 << 6)

// Define Set-Reset period [in 0x0B] + Masked Bits.
// 00-> For StandBy mode ; 01-> ForContinous Mode
#define GY271_DEFAULT_SET_RESET_PERIOD (0x01)
// Data format for control register1 [in 0x09] + Masked Bits.
//  e.g:-
//  OSR  = 01 [256]
//  RNG  = 00 [+-2G]    with 12000 LSB/G
//  ODR  = 01 [50Hz]
//  MODE = 01 [continous]
#define GY271_OPERATION_MODE1 ((GY271_OSR_128) | (GY271_RNG_2G) | (GY271_ODR_50hz) | (GY271_MODE_CONT))

// Enable INTERUPT bit from control register2 [in 0x0A] + Masked Bits.
#define GY271_INT_EN_PIN (0x00)
#define GY271_INT_DISABLE_PIN (1 << 0)

// Enable roll-over function ; data pointer automatically rolls between 00H ~ 06H, if I2C read begins at any address
#define GY271_ROL_PNT_EN (1 << 6)

// Control Register 2 : - setup
#define GY271_OPERATION_MODE2 ((GY271_INT_DISABLE_PIN) | (GY271_ROL_PNT_EN))

// Data ready flag [Masked Bit0] in "{STATUS_REGISTER - 06H}"
#define GY271_DATA_READY_FLAG (1 << 0)

// Data Skip flag [Masked Bit2] in "{STATUS_REGISTER - 06H}"
#define GY271_DATA_OVERRUN_FLAG (1 << 2)

/*******************************************************************/
// list of cosntants
/*******************************************************************/
// Following are the conversion factor of data according to the datahsheet.
#define GY271_TEMPERATURE_SENSITIVITY 100.0f // [100 LSB/*c]
// At Field_Range = +-2G (), the conversion LSB per G => 12000 LSB/Gauss.
#define GY271_CONVERSION_TO_G 12000.0f

// CUSTOM GY271 Data storage structure
typedef struct s_gy271_calib
{
    long bias_axis[3];        // (max_ + min_)/2
    long delta_axis[3];       // (max_ - min_)/2
    float delta_avg;          // (Delta_axis[0] + Delta_axis[1] + Delta_axis[2])/3
    float scale_axis[3];      // delta_avg / delta_axis
    float calibrated_axis[3]; // scale_axis[0] * ( raw_axis - bias_axis[0] )
} s_gy271_calib_t;
typedef struct s_gy271_raw_data
{
    int16_t raw_x;
    int16_t raw_y;
    int16_t raw_z;
    int16_t raw_temp;
} s_gy271_raw_data_t;
typedef struct s_gy271_data
{
    bool calibration_complete;
    s_gy271_calib_t calib_factor;
    float X;
    float Y;
    float Z;
    float T;
    int azimuth;
} s_gy271_data_t;

ezlopi_error_t __gy271_configure(l_ezlopi_item_t *item);
bool __gy271_update_value(l_ezlopi_item_t *item);
void __gy271_get_raw_max_min_values(l_ezlopi_item_t *item, int (*calibrationData)[2]);
//-----------------------------------------------------------------------------------------------

// Action
ezlopi_error_t sensor_0007_I2C_GY271(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#endif // _SENSOR_0007_I2C_GY271_H_