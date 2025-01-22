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
 * @file    ezlopi_hal_i2c_master.h
 * @brief   perform some function on I2C
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 *          Lomas Subedi
 *          Riken Maharjan
 *          Nabin Dangi
 * @version 0.1
 * @date    xx
 */

#ifndef _EZLOPI_HAL_I2C_MASTER_H_
#define _EZLOPI_HAL_I2C_MASTER_H_

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <stdio.h>
#include "driver/i2c.h"
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

#define EZLOPI_I2C_MASTER_DEFAULT_CONF \
    {                                  \
        .sda = 9, .scl = 10, .clock_speed = 100000, .channel = EZLOPI_I2C_0}
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

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/
    /**
     * @brief Function to initialize I2C
     *
     * @param i2c_master_conf Configuration info
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_hal_i2c_master_init(s_ezlopi_i2c_master_t *i2c_master_conf);
    /**
     * @brief Function to de-initialize I2C
     *
     * @param i2c_master_conf Configuration info
     */
    void EZPI_hal_i2c_master_deinit(s_ezlopi_i2c_master_t *i2c_master_conf);
    /**
     * @brief Function to send data via I2C (Master to slave)
     *
     * @param i2c_master_conf I2C configuration
     * @param write_buffer Pointer to buffer containing data
     * @param write_len Length of data
     * @return esp_err_t
     */

    esp_err_t EZPI_hal_i2c_master_write_to_device(s_ezlopi_i2c_master_t *i2c_master_conf, uint8_t *write_buffer, uint32_t write_len);
    /**
     * @brief Function to read data via I2C (slave to Master)
     *
     * @param i2c_master_conf I2C configuration
     * @param read_buffer Pointer to buffer whach will store extracted data
     * @param read_len Length of data
     * @return esp_err_t
     */
    esp_err_t EZPI_hal_i2c_master_read_from_device(s_ezlopi_i2c_master_t *i2c_master_conf, uint8_t *read_buffer, uint32_t read_len);
#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_HAL_I2C_MASTER_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
