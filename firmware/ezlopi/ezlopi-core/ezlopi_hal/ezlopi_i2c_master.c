
#include "stdint.h"
#include "driver/i2c.h"
#include "ezlopi_i2c_master.h"
#include "trace.h"


static bool mutex_set = false;


int ezlopi_i2c_master_init(s_ezlopi_i2c_master_t *i2c_master_conf)
{
    int ret = 0;

    if (NULL != i2c_master_conf)
    {
        if (true == i2c_master_conf->enable)
        {
            i2c_master_conf->channel = I2C_NUM_0;

            i2c_config_t i2c_config = {
                .mode = I2C_MODE_MASTER,
                .sda_io_num = i2c_master_conf->sda,
                .scl_io_num = i2c_master_conf->scl,
                .sda_pullup_en = GPIO_PULLUP_ENABLE,
                .scl_pullup_en = GPIO_PULLUP_ENABLE,
                .master.clk_speed = i2c_master_conf->clock_speed,
            };

            if(false == mutex_set)
            {
                i2c_param_config(i2c_master_conf->channel, &i2c_config);
                i2c_driver_install(i2c_master_conf->channel, I2C_MODE_MASTER, 0, 0, 0);
                ezlopi_I2C_mutex = xSemaphoreCreateMutex();
                mutex_set = true;
            }
        }
    }

    return ret;
}

esp_err_t write_reg(uint32_t channel, uint8_t slave_addr, uint8_t data, uint8_t register_addr)
{
    esp_err_t error = ESP_OK;
    xSemaphoreTake(ezlopi_I2C_mutex, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_start(cmd));
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_write_byte(cmd, (slave_addr << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN));
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_write_byte(cmd, register_addr, ACK_CHECK_EN));
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_write_byte(cmd, data, ACK_CHECK_EN));
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_stop(cmd));
    i2c_master_cmd_begin(channel, cmd, 50 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    xSemaphoreGive(ezlopi_I2C_mutex);
    return error;
}

esp_err_t read_reg(uint32_t channel, uint8_t slave_addr, uint8_t *data, uint8_t register_addr)
{
    xSemaphoreTake(ezlopi_I2C_mutex, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_start(cmd));
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_write_byte(cmd, (slave_addr << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN));
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_write_byte(cmd, register_addr, ACK_CHECK_EN));
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_start(cmd));
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_write_byte(cmd, (slave_addr << 1) | I2C_MASTER_READ, ACK_CHECK_EN));
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_read_byte(cmd, data, I2C_MASTER_NACK));
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_stop(cmd));
    i2c_master_cmd_begin(channel, cmd, 50 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    xSemaphoreGive(ezlopi_I2C_mutex);
    return ESP_OK;
}
