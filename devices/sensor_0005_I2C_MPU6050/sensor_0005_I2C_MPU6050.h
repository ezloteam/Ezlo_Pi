
#ifndef _0005_sens_i2c_mpu6050_accel_gyro_temp_H_
#define _0005_sens_i2c_mpu6050_accel_gyro_temp_H_

// Offsets and conversion values : 1g = 9.80665 m/s^2
#define MPU6050_STANDARD_G_TO_ACCEL_CONVERSION_VALUE (9.80665f)
#define GYRO_X_OFFSET (1.6f)  // if +ve avg offset, use -neg value and vice versa
#define GYRO_Y_OFFSET (0.10f) // if +ve avg offset, use -neg value and vice versa
#define GYRO_Z_OFFSET (1.5f)  // if +ve avg offset, use -neg value and vice versa

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
#define INTR_EN_DATA_RDY (BIT_0) // When set; this bit enables the Data Ready interrupt, which occurs each time a write operation to all of the sensor registers has been completed.

// Power management (1) configuration + mode masks
#define PWR_MGMT_1_RESET (BIT_7)                               // When set, this bit resets all internal registers to their default values.(automatically clears to 0)
#define PWR_MGMT_1_SLEEP (BIT_6)                               // When set, this bit puts MPU6050 into low power sleep mode
#define PWR_MGMT_1_PLL_X_AXIS_INTERNAL_CLK_REF (BIT_0)         // When set, choose PLL with X axis gyroscope reference
#define PWR_MGMT_1_PLL_Y_AXIS_INTERNAL_CLK_REF (BIT_1)         // When set, choose PLL with Y axis gyroscope reference
#define PWR_MGMT_1_PLL_Z_AXIS_INTERNAL_CLK_REF (BIT_1 | BIT_0) // When set, choose PLL with Z axis gyroscope reference

//-----------------------------
// #CUSTOM DATA STORAGE STRUCTS
//-----------------------------
// structure to store processed data
typedef struct
{
    float temp_mpu;
    float ax, ay, az;
    float gx, gy, gz;
} mpu6050_data_t;

// structure to store unprocessed data
typedef struct
{
    int16_t raw_t;
    int16_t raw_ax, raw_ay, raw_az;
    int16_t raw_gx, raw_gy, raw_gz;
} raw_mpu6050_data_t;

//-----------------------------
// #Enumeration of MPU6050 error types
//-----------------------------
typedef enum
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
} mpu6050_err_t;

// Action function declaration
int sensor_0005_I2C_MPU6050(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg);

#endif