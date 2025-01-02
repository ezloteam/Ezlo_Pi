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
/**
 * @file    ezlopi_hal_spi_master.h
 * @brief   perform some function on SPI
 * @author  xx
 * @version 0.1
 * @date    xx
*/

#ifndef _EZLOPI_HAL_SPI_MASTER_H_
#define _EZLOPI_HAL_SPI_MASTER_H_

/*******************************************************************************
*                          Include Files
*******************************************************************************/

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "driver/spi_master.h"

#include "ezlopi_core_errors.h" 
/*******************************************************************************
*                          C++ Declaration Wrapper
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

    /*******************************************************************************
    *                          Type & Macro Declarations
    *******************************************************************************/
    typedef enum e_ezlopi_spi_channel
    {
        EZLOPI_SPI_1 = 0,
        EZLOPI_SPI_2,
        EZLOPI_SPI_3,
        EZLOPI_SPI_MAX
    } e_ezlopi_spi_channel_t;

    typedef enum e_ezlopi_spi_mode
    {
        EZLOPI_SPI_CPOL_LOW_CPHA_LOW = 0, // CPOL_LOW(polarity): Logic low, CPHA_LOW(phase) - Data sampled on rising edge and shifted out on the falling edge
        EZLOPI_SPI_CPOL_LOW_CPHA_HIGH,    // CPOL_LOWapolarity): Logic low, CPHA_HIGH(phase) - Data sampled on the falling edge and shifted out on the rising edge
        EZLOPI_SPI_CPOL_HIGH_CPHA_LOW,    // CPOL_HIGH(polarity): Logic high, CPHA_LOW(phase) - Data sampled on the rising edge and shifted out on the falling edge
        EZLOPI_SPI_CPOL_HIGH_CPHA_HIGH    // CPOL_HIGH(polarity): Logic high, CPHA_HIGH(phase) - Data sampled on the falling edge and shifted out on the rising edge
    } e_ezlopi_spi_mode_t;

    typedef struct s_ezlopi_spi_master
    {
        bool enable;
        int cs;
        int miso;
        int mosi;
        int sck;
        uint32_t mode;
        uint32_t clock_speed_mhz;
        uint8_t addr_bits;
        uint8_t command_bits;
        uint32_t transfer_sz;
        uint32_t queue_size;
        uint32_t flags;
        e_ezlopi_spi_channel_t channel;
        spi_device_handle_t handle;

    } s_ezlopi_spi_master_t;

    /*******************************************************************************
    *                          Extern Data Declarations
    *******************************************************************************/

    /*******************************************************************************
    *                          Extern Function Prototypes
    *******************************************************************************/

    /**
     * @brief Function to initialize SPI master
     *
     * @param spi_config Configuation for SPI
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_hal_spi_master_init(s_ezlopi_spi_master_t *spi_config);
    /**
     * @brief Function to remove SPI from system
     *
     * @param spi_config
     * @return int
     */
    int EZPI_hal_spi_master_deinit(s_ezlopi_spi_master_t *spi_config);

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_HAL_SPI_MASTER_H_


/*******************************************************************************
*                          End of File
*******************************************************************************/