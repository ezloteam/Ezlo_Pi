
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "ezlopi_util_trace.h"
#include "ezlopi_core_errors.h"

#include "ezlopi_hal_i2c_master.h"

static s_ezlopi_i2c_master_t *i2c_master_conf_ptr[I2C_NUM_MAX] = {NULL, NULL};

static SemaphoreHandle_t i2c_semaphore = NULL;

// static int ezlopi_i2c_check_channel(s_ezlopi_i2c_master_t *i2c_master_conf)
// {
//     int ret = 0;

//     for (int idx = 0; idx < I2C_NUM_MAX; idx++)
//     {
//         if (NULL != i2c_master_conf_ptr)
//         {
//         }
//     }

//     return ret;
// }

ezlopi_error_t ezlopi_i2c_master_init(s_ezlopi_i2c_master_t *i2c_master_conf)
{
    ezlopi_error_t ret = EZPI_ERR_HAL_INIT_FAILED;
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

                // TRACE_W("Frequency : %d", i2c_master_conf->clock_speed);
                // TRACE_W("Pin SDA : %d", i2c_master_conf->sda);
                // TRACE_W("Pin SCL : %d", i2c_master_conf->scl);

                ESP_ERROR_CHECK(i2c_param_config(i2c_master_conf->channel, &i2c_config));
                ESP_ERROR_CHECK(i2c_driver_install(i2c_master_conf->channel, I2C_MODE_MASTER, 0, 0, 0));
                i2c_master_conf_ptr[i2c_master_conf->channel] = i2c_master_conf;
                ret = EZPI_SUCCESS;
            }
        }

        if (NULL == i2c_semaphore)
        {
            i2c_semaphore = xSemaphoreCreateBinary();
            if (i2c_semaphore)
            {
                TRACE_E("Created I2C semaphore");
                xSemaphoreGive(i2c_semaphore);
            }
        }
    }
    return ret;
}

esp_err_t ezlopi_i2c_master_read_from_device(s_ezlopi_i2c_master_t *i2c_master_conf, uint8_t *read_buffer, uint32_t read_len)
{
    int ret = EZPI_FAILED;
    if (i2c_semaphore)
    {
        if (pdTRUE == xSemaphoreTake(i2c_semaphore, portMAX_DELAY))
        {
            esp_err_t error = i2c_master_read_from_device(i2c_master_conf->channel, i2c_master_conf->address, read_buffer, read_len, 200);
            xSemaphoreGive(i2c_semaphore);
            if (ESP_OK == error)
            {
                ret = EZPI_SUCCESS;
            }
        }
    }
    return ret;
}

esp_err_t ezlopi_i2c_master_write_to_device(s_ezlopi_i2c_master_t *i2c_master_conf, uint8_t *write_buffer, uint32_t write_len)
{
    int ret = EZPI_FAILED;
    if (i2c_semaphore)
    {
        if (pdTRUE == xSemaphoreTake(i2c_semaphore, portMAX_DELAY))
        {
            esp_err_t error = i2c_master_write_to_device(i2c_master_conf->channel, i2c_master_conf->address, write_buffer, write_len, 200);
            xSemaphoreGive(i2c_semaphore);
            if (ESP_OK == error)
            {
                ret = EZPI_SUCCESS;
            }
        }
    }
    return ret;
}

void ezlopi_i2c_master_deinit(s_ezlopi_i2c_master_t *i2c_master_conf)
{
    if (NULL != i2c_master_conf)
    {
        if (1 == i2c_master_conf_ptr[i2c_master_conf->channel]->channel)
        {
            i2c_driver_delete(i2c_master_conf->channel);
            i2c_master_conf_ptr[i2c_master_conf->channel] = NULL;
        }
    }
    if (NULL != i2c_semaphore)
    {
        vSemaphoreDelete(i2c_semaphore);
        i2c_semaphore = NULL;
    }
}
