#include <stdio.h>
#include "driver/i2c.h"

#include "mpu6050.h"
#include "i2c_master_interface.h"

#define I2C_MASTER_SCL_IO 22        /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO 23        /*!< gpio number for I2C master data  */
#define I2C_MASTER_FREQ_HZ 100000   /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */

#define WRITE_BIT I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ   /*!< I2C master read */
#define ACK_CHECK_EN 0x1           /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0          /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                /*!< I2C ack value */
#define NACK_VAL 0x1               /*!< I2C nack value */

/*MPU6050 register addresses */

#define MPU6050_REG_POWER 0x6B
#define MPU6050_REG_ACCEL_CONFIG 0x1C
#define MPU6050_REG_GYRO_CONFIG 0x1B

/*These are the addresses of mpu6050 from which you will fetch accelerometer x,y,z high and low values */
#define MPU6050_REG_ACC_X_HIGH 0x3B
#define MPU6050_REG_ACC_X_LOW 0x3C
#define MPU6050_REG_ACC_Y_HIGH 0x3D
#define MPU6050_REG_ACC_Y_LOW 0x3E
#define MPU6050_REG_ACC_Z_HIGH 0x3F
#define MPU6050_REG_ACC_Z_LOW 0x40

/*These are the addresses of mpu6050 from which you will fetch gyro x,y,z high and low values */

#define MPU6050_REG_GYRO_X_HIGH 0x43
#define MPU6050_REG_GYRO_X_LOW 0x44
#define MPU6050_REG_GYRO_Y_HIGH 0x45
#define MPU6050_REG_GYRO_Y_LOW 0x46
#define MPU6050_REG_GYRO_Z_HIGH 0x47
#define MPU6050_REG_GYRO_Z_LOW 0x48

/*MPU6050 address and who am i register*/

#define MPU6050_SLAVE_ADDR 0x68
#define who_am_i 0x75

uint8_t buffer[14];
size_t data_len = 12;
SemaphoreHandle_t print_mux = NULL;
uint16_t pbuffer[7];
uint16_t accel_x;
uint16_t accel_y;
uint16_t accel_z;
uint16_t gyro_x;
uint16_t gyro_y;
uint16_t gyro_z;

esp_err_t MPU6050_i2c_master_init(uint8_t SDA, uint8_t SCL)
{
    i2c_master_interface_init(I2C_NUM_0, SDA, SCL, 100000);
    return ESP_OK;
}

esp_err_t mpu6050_wake(void)
{
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MPU6050_SLAVE_ADDR << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, MPU6050_REG_POWER, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, 0x00, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

esp_err_t i2c_master_sensor_test(uint8_t length, uint8_t *data, uint16_t timeout)
{
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MPU6050_SLAVE_ADDR << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, MPU6050_REG_ACC_X_HIGH, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    vTaskDelay(30 / portTICK_RATE_MS);
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MPU6050_SLAVE_ADDR << 1 | READ_BIT, ACK_CHECK_EN);
    i2c_master_read(cmd, data, length - 1, ACK_VAL);
    i2c_master_read(cmd, data, 1, NACK_VAL);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

uint16_t accel_x_value_read(void)
{
    return accel_x;
}

uint16_t accel_y_value_read(void)
{
    return accel_y;
}

uint16_t accel_z_value_read(void)
{
    return accel_z;
}

uint16_t gyro_x_value_read(void)
{
    return gyro_x;
}

uint16_t gyro_y_value_read(void)
{
    return gyro_y;
}

uint16_t gyro_z_value_read(void)
{
    return gyro_z;
}

void MPU6050_disp_buff(uint8_t *buf, int len)
{
    int i;

    pbuffer[0] = (int)((buf[0] << 8) | buf[1]);
    pbuffer[1] = (int)((buf[2] << 8) | buf[3]);
    pbuffer[2] = (int)((buf[4] << 8) | buf[5]);
    pbuffer[3] = (int)((buf[6] << 8) | buf[7]);
    pbuffer[4] = (int)((buf[8] << 8) | buf[9]);
    pbuffer[5] = (int)((buf[10] << 8) | buf[11]);
    pbuffer[6] = (int)((buf[12] << 8) | buf[13]);
    accel_x = pbuffer[0];
    accel_y = pbuffer[1];
    accel_z = pbuffer[2];
    gyro_x = pbuffer[3];
    gyro_y = pbuffer[4];
    gyro_z = pbuffer[5];

    for (i = 0; i < 7; i++)
    {
        printf("%d ", pbuffer[i]);
    }
    printf("\n");
}

void MPU_TASK()
{
    int ret;
    ret = i2c_master_sensor_test(14, &buffer[0], 0);
    if (ret == ESP_ERR_TIMEOUT)
    {
        printf("\n I2C Timeout");
    }
    else if (ret == ESP_OK)
    {
        printf("*******************\n");
        printf("TASK: MASTER READ SENSOR( MPU6050 )\n");
        printf("*******************\n");
    }
    else
    {
        printf("\n No ack, sensor not connected...skip...");
    }
    // vTaskDelay(500 / portTICK_RATE_MS);
    MPU6050_disp_buff(&buffer[0], 14);
    //  vTaskDelay(30 / portTICK_RATE_MS);
}