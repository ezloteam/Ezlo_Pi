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
 * @file    ezlopi_service_uart.h
 * @brief   Contains function declarations related to UART communication
 * @author  ezlopi_team_np
 * @version 1.0
 * @date    January 22, 2024
 */
#ifndef __EZLOPI_SERVICE_UART_H__
#define __EZLOPI_SERVICE_UART_H__

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "../../build/config/sdkconfig.h"
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
    // typedef enum e_ezlopi_item_type
    // {
    //     EZLOPI_ITEM_NONE = 0,
    //     EZLOPI_ITEM_LED = 1,
    //     EZLOPI_ITEM_RELAY = 2,
    //     EZLOPI_ITEM_PLUG = 3,
    //     EZLOPI_ITEM_SPK = 4, // SPK -> [S: Switch][P: Push Buttom][K: Key]
    //     EZLOPI_ITEM_MPU6050 =5,
    //     EZLOPI_ITEM_ADXL345 = 6,
    //     EZLOPI_ITEM_
    // } e_ezlopi_item_type_t;
/**
 * @brief Minimum length for the WiFi password
 *
 */
#define EZLOPI_WIFI_MIN_PASS_CHAR 8
/**
 * @brief WiFi connection retries
 *
 */
#define EZLOPI_WIFI_CONN_RETRY_ATTEMPT 2
/**
 * @brief Wifi reconnect interval incase of connection failed
 *
 */
#define EZLOPI_WIFI_CONN_ATTEMPT_INTERVAL 5000

    /**
     * @brief Enum for UART command
     *
     */
    typedef enum e_ezlopi_uart_cmd
    {
        EZPI_UART_CMD_RESET = 0,   /**< Command reset */
        EZPI_UART_CMD_INFO,        /**< Command device info */
        EZPI_UART_CMD_WIFI,        /**< Command WiFi */
        EZPI_UART_CMD_SET_CONFIG,  /**< Command set configurations */
        EZPI_UART_CMD_GET_CONFIG,  /**< Command get configurations */
        EZPI_UART_CMD_UART_CONFIG, /**< Command uart configuration */
        EZPI_UART_CMD_LOG_CONFIG,  /**< Command log configuration */
        EZPI_UART_CMD_SET_PROV,    /**< Command set provisioning data */
        EZPI_UART_CMD_MAX          /**< Command maximum */
    } e_ezlopi_uart_cmd_t;

    /**
     * @brief Enum for status of UART command
     *
     */
    typedef enum e_ezlopi_uart_cmd_status
    {
        EZPI_UART_CMD_STATUS_FAIL = 0, /**< Status command failed */
        EZPI_UART_CMD_STATUS_SUCCESS,  /**< Status command success */
        EZPI_UART_CMD_STATUS_MAX       /**< Status maximum */
    } e_ezlopi_uart_cmd_status_t;

    /**
     * @brief Function to initialize UART task
     *
     */
    void EZPI_SERV_uart_init(void);

#ifndef CONFIG_IDF_TARGET_ESP32
    /**
     * @brief Function to initialize USB cdc
     * @details Following tasks are performed
     *  - Start binary semaphore
     *  - Installs tiny USB driver
     *  - Initializes CDC ACM
     *
     */
    void EZPI_SERV_cdc_init();
#endif // NOT defined CONFIG_IDF_TARGET_ESP32
       /**
        * @brief Function to send data through UART or CDC
        *
        * @param len Length of the data
        * @param data Pointer to the data to transmit
        * @return int
        */
    int EZPI_SERV_uart_tx_data(int len, uint8_t *data);

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/
    /**
     * @brief Global function template example
     * Convention : Use capital letter for initial word on extern function
     * maincomponent : Main component as hal, core, service etc.
     * subcomponent : Sub component as i2c from hal, ble from service etc
     * functiontitle : Title of the function
     * eg : EZPI_hal_i2c_init()
     * @param arg
     *
     */

#ifdef __cplusplus
}
#endif

#endif // __EZLOPI_SERVICE_UART_H__

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
