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
 * @file    ze08_ch2o.h
 * @brief   perform some function on ze08_ch2o
 * @author  ezlopi_team_np
 * @version 0.1
 * @date    xx
 */

#ifndef _ZE08_CH2O_H_
#define _ZE08_CH2O_H_

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <stddef.h>
#include "driver/gpio.h"
#include "ezlopi_hal_uart.h"
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

#define ZE08_BUFFER_MAXLENGTH 9
#define ZE08_CH2O_RX_PIN GPIO_NUM_45
#define ZE08_CH2O_TX_PIN GPIO_NUM_48
#define ZE08_CH2O_UART_BUAD_RATE 9600
#define DEBUG_ZE08_CH2O 0

    typedef struct ze08_ch2o_sensor_data
    {
        bool available;
        float ppm;
    } ze08_ch2o_sensor_data_t;

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/

    /**
     * @brief Intitializes ZE08-CH2O sensor
     *
     * @param ze08_uart_config Pointer to config struct
     * @param data Pointer to user-arg data.
     * @return true
     * @return false
     */
    bool ZE08_ch2o_sensor_init(s_ezlopi_uart_t *ze08_uart_config, ze08_ch2o_sensor_data_t *data);

#ifdef __cplusplus
}
#endif

#endif // _ZE08_CH2O_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/