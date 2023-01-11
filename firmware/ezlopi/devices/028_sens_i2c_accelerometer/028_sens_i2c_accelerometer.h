
#ifndef _028_SENS_I2C_ACCELEROMETER_H_
#define _028_SENS_I2C_ACCELEROMETER_H_


#include "ezlopi_actions.h"
#include "ezlopi_i2c_master.h"
#include "ezlopi_devices.h"

// Datasheet can be found at:
// https://www.sparkfun.com/datasheets/Sensors/Accelerometer/ADXL345.pdf

// For reference following github repository can be consulted.
// https://github.com/Embetronicx/incubator-nuttx-apps/tree/master/examples/etx_i2c

#define SLAVE_ADDR 0x1D


#define ADXL345_DEVICE_ID_REGISTER                      0x00    // This is read-only register.
#define ADXL345_DATA_RATE                               0x2C    // This is read-write register.
#define ADXL345_DEVICE_POWER_CTRL                       0x2D    // This is read-write register.
#define ADXL345_DATA_FORMAT_REGISTER                    0x31    // This is read-write register.
#define ADXL345_DATA_X_0_REGISTER                       0x32    // This is read-only register.
#define ADXL345_DATA_X_1_REGISTER                       0x33    // This is read-only register.
#define ADXL345_DATA_Y_0_REGISTER                       0x34    // This is read-only register.
#define ADXL345_DATA_Y_1_REGISTER                       0x35    // This is read-only register.
#define ADXL345_DATA_Z_0_REGISTER                       0x36    // This is read-only register.
#define ADXL345_DATA_Z_1_REGISTER                       0x37    // This is read-only register.


// Data for format register. Setting lower 4 bits only.
// D3 = 1; setting device data in full resolution, i.e 13-bits.
// D2 = 1; Left justification. i.e. MSB left.
// (D1, D0) = (1, 1); for acceleration measurement up to 16g.
#define ADXL345_FORMAT_REGISTER_DATA                    0x0F

// This is defined to reset power control register, setting device in standby mode so that other device configurations can be done.
#define ADXL345_POWER_CTRL_RESET                        0x00

// This is defined to set power control register's measurement bit on so that device can initiate the 
//measurement process after conpletion of device configuration.
#define ADXL345_POWER_CTRL_SET_TO_MEASUTEMENT           0x08  


// Following are the conversion factor of data according to the datahsheet.

// At high resolution, the conversion per LSB is 4mg/LSB.
#define ADXL345_CONVERTER_FACTOR_MG_TO_G                0.004

// 1g = 9.80665 m/s^2
#define STANDARD_G_TO_ACCEL_CONVERSION_VALUE           9.80665 

typedef struct {
    float x_value;
    float y_value;
    float z_value;
}sensor_i2c_accelerometer_values_t;

int sensor_i2c_accelerometer(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg);


#endif // _028_SENS_I2C_ACCELEROMETER_H_

