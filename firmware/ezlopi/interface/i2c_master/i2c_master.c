#include "driver/i2c.h"
#include "i2c_master.h"

static uint8_t i2c_port_status[I2C_NUM_MAX] = {0, 0};

void i2c_master_init(int i2c_num, int sda, int scl, uint32_t clock_speed)
{
    if (i2c_port_status[i2c_num])
    {
        i2c_config_t i2c_config = {
            .mode = I2C_MODE_MASTER,
            .sda_io_num = sda,
            .scl_io_num = scl,
            .sda_pullup_en = GPIO_PULLUP_ENABLE,
            .scl_pullup_en = GPIO_PULLUP_ENABLE,
            .master.clk_speed = clock_speed};
        i2c_param_config(I2C_NUM_0, &i2c_config);
        i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
        i2c_port_status[i2c_num] = 1;
    }
}

void i2c_master_deinit(int i2c_num)
{
    if (1 == i2c_port_status[i2c_num])
    {
    }
}