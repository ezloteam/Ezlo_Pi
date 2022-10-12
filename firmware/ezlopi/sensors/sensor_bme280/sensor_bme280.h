#ifndef __SENSOR_BME280_H__
#define __SENSOR_BME280_H__

#include "ezlopi_actions.h"
#include "bme280/bme280.h"
#include "bme280/bme280_defs.h"
#include "driver/i2c.h"

#define SENSOR_BME280_TAG "SENSOR BME280 TAG"

#define SENSOR_0010_BME280
#define CHIP_ID 0x76
#define ACK_CHECK_EN 0x1

/**
 * 
 * @brief Structure that contains identifier details used in example 
 * 
 * @public uint8_t dev_addr: This variable stores device address
 * @public int8_t fd: This variable stores file descriptor  
 *  
 * 
 */
typedef struct{
    uint8_t dev_addr;
    int8_t fd;
}bme280_identifier_t;

/**
 * @brief Typedef to the bme280 device structure
 */
typedef struct bme280_dev sensor_bme280_dev_t;

/**
 * @brief Typedef to the bme280 sensor data structure.
 */
typedef struct bme280_data sensor_bme280_data_t;


int8_t user_i2c_read(uint8_t reg_addr, uint8_t *data, uint32_t len, void *intf_ptr);
int8_t user_i2c_write(uint8_t reg_addr, const uint8_t *data, uint32_t len, void *intf_ptr);
void user_delay_us(uint32_t period, void *intf_ptr);
int sensor_bme280(e_ezlopi_actions_t action, void *arg);
// int sensor_bme280(e_ezlopi_actions_t action, void *arg) { return 0; }


#endif // __SENSOR_BME290_H__
