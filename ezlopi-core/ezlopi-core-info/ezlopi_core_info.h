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
 * @file    main.c
 * @brief   perform some function on data
 * @author  John Doe
 * @version 0.1
 * @date    1st January 2024
 */

#ifndef EZLOPI_CORE_INFO_H
#define EZLOPI_CORE_INFO_H

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
     * @brief Global function template example
     * Convention : Use capital letter for initial word on extern function
     * maincomponent : Main component as hal, core, service etc.
     * subcomponent : Sub component as i2c from hal, ble from service etc
     * functiontitle : Title of the function
     * eg : EZPI_hal_i2c_init()
     * @param arg
     *
     */
    uart_parity_t EZPI_CORE_info_name_to_parity(const char* parity);
    char EZPI_CORE_info_parity_to_name(const uart_parity_t parity_val);
    void EZPI_CORE_info_get_flow_ctrl_to_name(uart_hw_flowcontrol_t flw_ctrl, char* flw_ctrl_str);
    uart_hw_flowcontrol_t EZPI_CORE_info_get_flw_ctrl_from_name(char* flw_ctrl_str);
    uart_word_length_t EZPI_CORE_info_get_frame_size(const uint32_t frame_size);
    char* EZPI_CORE_info_get_esp_reset_reason_to_name(esp_reset_reason_t reason);
    char* EZPI_CORE_info_get_chip_type_to_name(int chip_type);
    char* EZPI_CORE_info_get_wifi_mode_to_name(const wifi_mode_t mode_val);
    void EZPI_CORE_info_get_tick_to_time_name(char* time_buff, uint32_t buff_len, uint32_t ms);
    int EZPI_CORE_info_get_ble_mac(uint8_t mac[6]);

#ifdef __cplusplus
}
#endif

#endif // EZLOPI_CORE_INFO_H

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
