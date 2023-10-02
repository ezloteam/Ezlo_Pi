#ifndef _0006_SENS_I2C_ADXL345_H_
#define _0006_SENS_I2C_ADXL345_H_

#include "ezlopi_actions.h"
#include "ezlopi_i2c_master.h"
#include "ezlopi_devices.h"

// Datasheet can be found at:
// https://www.sparkfun.com/datasheets/Sensors/Accelerometer/ADXL345.pdf

// For reference following github repository can be consulted.
// https://github.com/Embetronicx/incubator-nuttx-apps/tree/master/examples/etx_i2c

#define ADXL345_ADDR 0x53 // if (AD0->Vcc) then; [i2c_address->0x1D]

/*******************************************************************/
// REGISTER ADDRESS
/*******************************************************************/
#define ADXL345_DEVICE_ID_REGISTER 0x00   // This is read-only register.
#define ADXL345_DATA_RATE 0x2C            // This is read-write register.
#define ADXL345_DEVICE_POWER_CTRL 0x2D    // This is read-write register.
#define ADXL345_INT_ENABLE_REGISTER 0x2E  // [R+W] (check before grabing any data from registers)
#define ADXL345_DATA_FORMAT_REGISTER 0x31 // This is read-write register.
#define ADXL345_INT_SOURCE_REGISTER 0x30  // [R] (check before grabing any data from registers)
#define ADXL345_DATA_X_0_REGISTER 0x32    // [LSB] This is read-only register.  // little endian
#define ADXL345_DATA_X_1_REGISTER 0x33    // [MSB] This is read-only register.
#define ADXL345_DATA_Y_0_REGISTER 0x34    // [LSB] This is read-only register.
#define ADXL345_DATA_Y_1_REGISTER 0x35    // [MSB] This is read-only register.
#define ADXL345_DATA_Z_0_REGISTER 0x36    // [LSB] This is read-only register.
#define ADXL345_DATA_Z_1_REGISTER 0x37    // [MSB] This is read-only register.

/*******************************************************************/
// Configuration values
/*******************************************************************/
// (HEX) Value to enable the "data_ready interrupt"(ie. bit7) in [0x2E—INT_ENABLE]
#define ADXL345_INT_EN 0x80

// Data ready flag [ie. bit7 ] in "INT_SOURCE_REGISTER" (0x30)
#define ADXL345_DATA_READY_FLAG (1 << 7)

// Data for format register. Setting lower 4 bits only.
// D3 = 1; setting device data in full resolution, i.e 13-bits.
// D2 = 0; right justification. i.e. MSB left.
// (D1, D0) = (1, 1); for acceleration measurement up to 16g.
#define ADXL345_FORMAT_REGISTER_DATA 0x0B

// This is defined to reset power control register, setting device in standby mode so that other device configurations can be done.
#define ADXL345_POWER_CTRL_RESET 0x00

// This is defined to set power control register's measurement bit on so that device can initiate the
// measurement process after completion of device configuration.
#define ADXL345_POWER_CTRL_SET_TO_MEASUTEMENT 0x08

/*******************************************************************/
// list of cosntants
/*******************************************************************/

// Following are the conversion factor of data according to the datahsheet.

// At high resolution, the conversion per LSB is 4mg/LSB.
#define ADXL345_CONVERTER_FACTOR_MG_TO_G 0.004

// 1g = 9.80665 m/s^2
#define ADXL345_STANDARD_G_TO_ACCEL_CONVERSION_VALUE 9.80665

// Action function
int sensor_0006_I2C_ADXL345(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg);

#endif // _0006_SENS_I2C_ADXL345_H_