/* ===========================================================================
** Copyright (C) 2025 Ezlo Innovation Inc
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
/**
 * @file    ezlopi_hal_spi_master.c
 * @brief   perform some function on SPI
 * @author  ezlopi_team_np
 * @version 0.1
 * @date    xx
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

#include "../../build/config/sdkconfig.h"
#include "esp_intr_alloc.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_core_errors.h"

#include "ezlopi_hal_spi_master.h"

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/
typedef enum e_spi_state
{
    SPI_UNINITIALIZED = 0,
    SPI_INITIALIZED
} e_spi_state_t;

#define SPI_ERROR_CHECK(ret, err_msg) \
    {                                 \
        if (ESP_OK != ret)            \
        {                             \
            TRACE_E(err_msg);         \
            break;                    \
        }                             \
    }

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/
static e_spi_state_t spi_port_status[3] = {SPI_UNINITIALIZED, SPI_UNINITIALIZED, SPI_UNINITIALIZED};

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/

ezlopi_error_t EZPI_hal_spi_master_init(s_ezlopi_spi_master_t *spi_config)
{
    ezlopi_error_t ret = EZPI_ERR_HAL_INIT_FAILED;

    if (NULL != spi_config)
    {
        if (true == spi_config->enable)
        {
            spi_bus_config_t buscfg = {
                .miso_io_num = spi_config->miso,
                .mosi_io_num = spi_config->mosi,
                .sclk_io_num = spi_config->sck,
                .quadhd_io_num = -1,
                .quadwp_io_num = -1,
                .max_transfer_sz = spi_config->transfer_sz,
                .flags = spi_config->flags,
#if CONFIG_SPI_MASTER_ISR_IN_IRAM
                .intr_flags = ESP_INTR_FLAG_IRAM
#endif
            };

            spi_device_interface_config_t devcfg = {
                .command_bits = spi_config->command_bits,
                .address_bits = spi_config->addr_bits,
                .mode = spi_config->mode, // SPI mode 0
                .clock_speed_hz = spi_config->clock_speed_mhz * 1000000,
                .queue_size = spi_config->queue_size,
                .pre_cb = NULL,
                .post_cb = NULL,
                .input_delay_ns = 0,            // the EEPROM output the data half a SPI clock behind.
                .spics_io_num = spi_config->cs, // not used
            };

            do
            {
                ret = spi_bus_initialize(spi_config->channel, &buscfg, SPI_DMA_CH_AUTO);
                SPI_ERROR_CHECK(ret, "SPI-master 'spi_bus_initialize' failed!");

                ret = spi_bus_add_device(spi_config->channel, &devcfg, &spi_config->handle);
                SPI_ERROR_CHECK(ret, "SPI-master 'spi_bus_add_device' failed!");

                spi_port_status[spi_config->channel] = SPI_INITIALIZED;
            } while (0);
            ret = EZPI_SUCCESS;
        }
    }

    return ret;
}

int EZPI_hal_spi_master_deinit(s_ezlopi_spi_master_t *spi_config)
{
    ezlopi_error_t ret = EZPI_ERR_HAL_INIT_FAILED;

    if (NULL != spi_config)
    {
        if (SPI_INITIALIZED == spi_port_status[spi_config->channel])
        {
            if (ESP_OK != spi_bus_remove_device(spi_config->handle))
            {
                TRACE_E("SPI-master 'spi_bus_de-initialize' failed!");
            }
            spi_config->handle = NULL;
            spi_port_status[spi_config->channel] = SPI_UNINITIALIZED;
        }
        ret = EZPI_SUCCESS;
    }

    return ret;
}

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          End of File
 *******************************************************************************/