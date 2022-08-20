

#ifndef _MPU6050_H_
#define _MPU6050_H_


esp_err_t MPU6050_i2c_master_init(uint8_t SDA, uint8_t SCL);
esp_err_t mpu6050_wake(void);
esp_err_t i2c_master_sensor_test(uint8_t length, uint8_t *data, uint16_t timeout);
uint16_t accel_x_value_read(void);
uint16_t accel_y_value_read(void);
uint16_t accel_z_value_read(void);
uint16_t gyro_x_value_read(void);
uint16_t gyro_y_value_read(void);
uint16_t gyro_z_value_read(void);
void MPU_TASK();

#endif