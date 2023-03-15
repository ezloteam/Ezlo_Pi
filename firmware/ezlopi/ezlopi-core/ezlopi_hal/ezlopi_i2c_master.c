
#include "stdint.h"
#include "driver/i2c.h"
#include "ezlopi_i2c_master.h"

static s_ezlopi_i2c_master_t *i2c_master_conf_ptr[I2C_NUM_MAX] = {NULL, NULL};
static int ezlopi_i2c_check_channel(s_ezlopi_i2c_master_t *i2c_master_conf)
{
    int ret = 0;

    for (int idx = 0; idx < I2C_NUM_MAX; idx++)
    {
        if (NULL != i2c_master_conf_ptr)
        {
        }
    }

    return ret;
}

int ezlopi_i2c_master_init(s_ezlopi_i2c_master_t *i2c_master_conf)
{
    int ret = 0;

    if (NULL != i2c_master_conf)
    {
        if (true == i2c_master_conf->enable)
        {
            if (NULL == i2c_master_conf_ptr[i2c_master_conf->channel])
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
                i2c_master_conf_ptr[i2c_master_conf->channel] = i2c_master_conf;
            }
        }
    }

    return ret;
}

void ezlopi_i2c_master_read_from_device(s_ezlopi_i2c_master_t *i2c_master_conf, uint8_t *read_buffer, uint32_t read_len)
{
    i2c_master_read_from_device(i2c_master_conf->channel, i2c_master_conf->address, read_buffer, read_len, 200);
}

void ezlopi_i2c_master_write_to_device(s_ezlopi_i2c_master_t *i2c_master_conf, uint8_t *write_buffer, uint32_t write_len)
{
    i2c_master_write_to_device(i2c_master_conf->channel, i2c_master_conf->address, write_buffer, write_len, 200);
}

void ezlopi_i2c_master_deinit(s_ezlopi_i2c_master_t *i2c_master_conf)
{
    if (NULL != i2c_master_conf)
    {
        if (1 == i2c_master_conf_ptr[i2c_master_conf->channel])
        {
            i2c_driver_delete(i2c_master_conf->channel);
            i2c_master_conf_ptr[i2c_master_conf->channel] = NULL;
        }
    }
}
