#ifndef __EZLOPI_HAL_H__
#define __EZLOPI_HAL_H__

#include <stdio.h>
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"


#define ACK_CHECK_EN 0x1


SemaphoreHandle_t ezlopi_I2C_mutex;

typedef enum e_ezlopi_i2c_channel
{
    EZLOPI_I2C_0 = 0,
    EZLOPI_I2C_1,
    EZLOPI_I2C_MAX
} e_ezlopi_i2c_channel_t;

typedef struct s_ezlopi_i2c_master
{
    bool enable;
    uint32_t sda;
    uint32_t scl;
    uint32_t clock_speed;
    uint32_t channel;
    uint32_t address;
} s_ezlopi_i2c_master_t;

#define EZLOPI_I2C_MASTER_DEFAULT_CONF                                      \
    {                                                                       \
        .sda = 9, .scl = 10, .clock_speed = 100000, .channel = EZLOPI_I2C_0 \
    }

int ezlopi_i2c_master_init(s_ezlopi_i2c_master_t *i2c_master_conf);
esp_err_t write_reg(uint32_t channel, uint8_t slave_addr, uint8_t data, uint8_t register_addr);
esp_err_t read_reg(uint32_t channel, uint8_t slave_addr, uint8_t *data, uint8_t register_addr);

#endif // __EZLOPI_HAL_H__
