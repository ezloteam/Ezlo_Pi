#ifndef __I2C_MASTER_H__
#define __I2C_MASTER_H__

#include "driver/i2c.h"

/**
 * @brief 'i2c_master_init' initiliazes the given i2c port in master mode
 *
 * @param i2c_num: I2C port number (options: 0, 1)
 * @param sda: I2C SDA(Serial Data) pin
 * @param scl: I2C SCL (Serial Clock) pin
 * @param clock_speed for i
 */
void i2c_master_interface_init(int i2c_num, int sda, int scl, uint32_t clock_speed);
void i2c_master_interface_deinit(int i2c_num);

#endif // __I2C_MASTER_H__
