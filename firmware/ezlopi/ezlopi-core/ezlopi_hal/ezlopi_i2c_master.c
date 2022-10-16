
#include "stdint.h"
#include "driver/i2c.h"
#include "ezlopi_i2c_master.h"

typedef enum e_i2c_state
{
    I2C_UNINITIALIZED = 0,
    I2C_INITIALIZED
} e_i2c_state_t;

static uint8_t i2c_port_status[I2C_NUM_MAX] = {I2C_UNINITIALIZED, I2C_UNINITIALIZED};

void ezlopi_i2c_master_init(s_ezlopi_i2c_master_t *i2c_master_conf)
{
    if (NULL != i2c_master_conf)
    {
        if (true == i2c_master_conf->enable)
        {
            if (0 == i2c_port_status[i2c_master_conf->channel])
            {
                i2c_config_t i2c_config = {
                    .mode = I2C_MODE_MASTER,
                    .sda_io_num = i2c_master_conf->sda,
                    .scl_io_num = i2c_master_conf->scl,
                    .sda_pullup_en = GPIO_PULLUP_ENABLE,
                    .scl_pullup_en = GPIO_PULLUP_ENABLE,
                    .master.clk_speed = i2c_master_conf->clock_speed,
                };

                i2c_param_config(I2C_NUM_0, &i2c_config);
                i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
                i2c_port_status[i2c_master_conf->channel] = I2C_INITIALIZED;
            }
        }
    }
}

void ezlopi_i2c_master_deinit(s_ezlopi_i2c_master_t *i2c_master_conf)
{
    if (NULL != i2c_master_conf)
    {
        if (1 == i2c_port_status[i2c_master_conf->channel])
        {
            i2c_port_status[i2c_master_conf->channel] = I2C_UNINITIALIZED;
        }
    }
}