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
 * @file    ezlopi_hal_uart.h
 * @brief   perform some function on UART
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 *          Lomas Subedi
 *          Riken Maharjan
 *          Nabin Dangi
 * @version 0.1
 * @date    xx
 */

#ifndef _EZLOPI_HAL_UART_H_
#define _EZLOPI_HAL_UART_H_

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "esp_err.h"
#include "EZLOPI_USER_CONFIG.h"

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
    typedef int ezlo_uart_channel_t;

    typedef enum e_ezlopi_uart_channel
    {
        EZLOPI_UART_CHANNEL_UNDEFINED = -1,
        EZLOPI_UART_CHANNEL_0 = UART_NUM_0,
        EZLOPI_UART_CHANNEL_1 = UART_NUM_1,
#if UART_NUM_MAX > 2
        EZLOPI_UART_CHANNEL_2 = UART_NUM_2,
#endif
#define EZLOPI_UART_CHANNEL_MAX UART_NUM_MAX
    } e_ezlopi_uart_channel_t;

    typedef struct s_ezlopi_uart_object *s_ezlopi_uart_object_handle_t;
    typedef void (*__uart_upcall)(uint8_t *buffer, uint32_t output_len, s_ezlopi_uart_object_handle_t uart_object_handle);

    typedef struct s_ezlopi_uart
    {
        ezlo_uart_channel_t channel;
        uint32_t baudrate;
        int tx;
        int rx;
        bool enable;
    } s_ezlopi_uart_t;

    struct s_ezlopi_uart_object
    {
        void *arg;
        s_ezlopi_uart_t ezlopi_uart;
        __uart_upcall upcall;
        QueueHandle_t ezlopi_uart_queue_handle;
        TaskHandle_t taskHandle;
    };

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/
    /**
     * @brief Function to setup UART on a target pin
     *
     * @param baudrate Target Baudrate
     * @param tx TX-pin
     * @param rx RX-pin
     * @param upcall Callback function for uart events
     * @param arg User-Args
     * @return s_ezlopi_uart_object_handle_t
     */
    s_ezlopi_uart_object_handle_t EZPI_hal_uart_init(uint32_t baudrate, uint32_t tx, uint32_t rx, __uart_upcall upcall, void *arg);
    /**
     * @brief Function to get the channels of UART
     *
     * @param ezlopi_uart_object_handle UART obj handle
     * @return ezlo_uart_channel_t
     */
    ezlo_uart_channel_t EZPI_hal_uart_get_channel(s_ezlopi_uart_object_handle_t ezlopi_uart_object_handle);
    /**
     * @brief Function to initialize UART service in main-file
     *
     */
    void EZPI_uart_main_init(void);

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_HAL_UART_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/