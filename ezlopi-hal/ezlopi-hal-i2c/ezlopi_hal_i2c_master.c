/**
 * @file    main.c
 * @brief   perform some function on data
 * @author  John Doe
 * @version 0.1
 * @date    1st January 2024
 */
/* ===========================================================================
** Copyright (C) 2024 Ezlo Innovation Inc
**
** Under EZLO AVAILABLE SOURCE LICENSE (EASL) AGREEMENT
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "ezlopi_util_trace.h"
#include "ezlopi_core_errors.h"

#include "ezlopi_hal_i2c_master.h"

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/
static s_ezlopi_i2c_master_t *i2c_master_conf_ptr[I2C_NUM_MAX] = {NULL, NULL};
static SemaphoreHandle_t i2c_semaphore = NULL;

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/

/**
 * @brief Global/extern function template example
 * Convention : Use capital letter for initial word on extern function
 * @param arg
 */
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

/*******************************************************************************
 *                          Static Function Definitions
 *******************************************************************************/

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

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
