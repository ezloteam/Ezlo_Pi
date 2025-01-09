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
 * @file    ezlopi_core_info.h
 * @brief   perform some function on system-info
 * @author  xx
 * @version 0.1
 * @date    12th DEC 2024
 */

#ifndef _EZLOPI_CORE_INFO_H_
#define _EZLOPI_CORE_INFO_H_

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "driver/uart.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "esp_idf_version.h"
#include "ezlopi_util_version.h"
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

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/
    /**
     * @brief Function to get name from parity
     *
     * @param parity parity char
     * @return uart_parity_t
     */
    uart_parity_t EZPI_core_info_name_to_parity(const char *parity);

    /**
     * @brief Function to add parity to name
     *
     * @param parity_val parity enum
     * @return char
     */
    char EZPI_core_info_parity_to_name(const uart_parity_t parity_val);
    /**
     * @brief Function to get flow control to  name
     *
     * @param flw_ctrl Target flow control
     * @param flw_ctrl_str flow control str
     */
    void EZPI_core_info_get_flow_ctrl_to_name(uart_hw_flowcontrol_t flw_ctrl, char *flw_ctrl_str);
    /**
     * @brief Function to get enum for flow control
     *
     * @param flw_ctrl_str target flow control str
     * @return uart_hw_flowcontrol_t
     */
    uart_hw_flowcontrol_t EZPI_core_info_get_flw_ctrl_from_name(char *flw_ctrl_str);
    /**
     * @brief Function to get enum of frame size
     *
     * @param frame_size Frame size
     * @return uart_word_length_t
     */
    uart_word_length_t EZPI_core_info_get_frame_size(const uint32_t frame_size);
    /**
     * @brief Function to return a the cause of reset
     *
     * @param reason enum of reason
     * @return char*
     */
    char *EZPI_core_info_get_esp_reset_reason_to_name(esp_reset_reason_t reason);
    /**
     * @brief Function to return chip type
     *
     * @param chip_type chip-type from config
     * @return char*
     */
    char *EZPI_core_info_get_chip_type_to_name(int chip_type);
    /**
     * @brief Function to return
     *
     * @param mode_val
     * @return char*
     */
    char *EZPI_core_info_get_wifi_mode_to_name(const wifi_mode_t mode_val);
    /**
     * @brief Funciton to get current ticks
     *
     * @param time_buff Destination pointer to store the time
     * @param buff_len length of buffer
     * @param ms time in ms
     */
    void EZPI_core_info_get_tick_to_time_name(char *time_buff, uint32_t buff_len, uint32_t ms);
    /**
     * @brief Funtion to return esp-mac address
     *
     * @param mac MAC address
     * @return int
     */
    int EZPI_core_info_get_ble_mac(uint8_t mac[6]);

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_CORE_INFO_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
