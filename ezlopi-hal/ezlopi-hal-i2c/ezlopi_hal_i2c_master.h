#ifndef _EZLOPI_HAL_I2C_MASTER_H_
#define _EZLOPI_HAL_I2C_MASTER_H_

#include <stdio.h>

#include "driver/i2c.h"

#include "ezlopi_core_errors.h"

typedef enum e_ezlopi_i2c_channel
{
    EZLOPI_I2C_0 = 0,
    EZLOPI_I2C_1,
    EZLOPI_I2C_MAX
} e_ezlopi_i2c_channel_t;

typedef struct s_ezlopi_i2c_master
{
    bool enable;
    int sda;
    int scl;
    uint32_t clock_speed;
    uint32_t channel;
    uint32_t address;
} s_ezlopi_i2c_master_t;

#define EZLOPI_I2C_MASTER_DEFAULT_CONF \
    {                                  \
        .sda = 9, .scl = 10, .clock_speed = 100000, .channel = EZLOPI_I2C_0}

ezlopi_error_t ezlopi_i2c_master_init(s_ezlopi_i2c_master_t *i2c_master_conf);
void ezlopi_i2c_master_deinit(s_ezlopi_i2c_master_t *i2c_master_conf);

esp_err_t ezlopi_i2c_master_write_to_device(s_ezlopi_i2c_master_t *i2c_master_conf, uint8_t *write_buffer, uint32_t write_len);
esp_err_t ezlopi_i2c_master_read_from_device(s_ezlopi_i2c_master_t *i2c_master_conf, uint8_t *read_buffer, uint32_t read_len);

#endif // _EZLOPI_HAL_I2C_MASTER_H_
