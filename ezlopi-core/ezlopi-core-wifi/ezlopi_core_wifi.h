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
 * @file    ezlopi_core_wifi.h
 * @brief   perform some function on wifi-operations
 * @author  ezlopi_team_np
 * @version 0.1
 * @date    12th DEC 2024
 */

#ifndef _EZLOPI_CORE_WIFI_H_
#define _EZLOPI_CORE_WIFI_H_

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

#include "esp_netif_types.h"
#include "esp_wifi_types.h"
#include "cjext.h"

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
    typedef void (*f_ezlopi_wifi_event_upcall)(esp_event_base_t event, int32_t event_id, void *arg);

    typedef struct ll_ezlopi_wifi_event_upcall
    {
        void *arg;
        f_ezlopi_wifi_event_upcall upcall;
        struct ll_ezlopi_wifi_event_upcall *next;
    } ll_ezlopi_wifi_event_upcall_t;

    typedef struct EZPI_core_wifi_status
    {
        bool wifi_connection;
        esp_netif_ip_info_t *ip_info;
        wifi_mode_t wifi_mode;
    } ezlopi_wifi_status_t;

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/

    /**
     * @brief Function to get wifi IP
     *
     * @return int
     */
    int EZPI_core_wifi_got_ip(void);
    /**
     * @brief Functiion to initialize ezlopi-wifi-service
     *
     */
    void EZPI_wifi_initialize(void);
    /**
     * @brief Function to trigger a wifi connect
     *
     * @param ssid Required ssid
     * @param pass Required password
     * @return esp_err_t
     */
    esp_err_t EZPI_core_wifi_connect(const char *ssid, const char *pass);
    /**
     * @brief Function that connects to local wifi using internal creds
     *
     */
    void EZPI_core_wifi_connect_from_id_bin(void);
    /**
     * @brief Function to get ip infos
     *
     * @return esp_netif_ip_info_t*
     */
    esp_netif_ip_info_t *EZPI_core_wifi_get_ip_infos(void);
    /**
     * @brief Function to get last wifi-disconnect error
     *
     * @return const char*
     */
    const char *EZPI_core_wifi_get_last_disconnect_reason(void);
    /**
     * @brief Function to trigger a new wifi-connection
     *
     * @param cj_network Object containing credential to Target network
     * @return int
     */
    int EZPI_core_wifi_try_new_connect(cJSON *cj_network);
    /**
     * @brief Function to check for wifi connection event
     *
     * @param wait_time_ms Time to wait for wifi-connection success
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_wait_for_wifi_to_connect(uint32_t wait_time_ms);
    /**
     * @brief Function to return wifi-status
     *
     * @return ezlopi_wifi_status_t*
     */
    ezlopi_wifi_status_t *EZPI_core_wifi_status(void);
    /**
     * @brief Function to get the auth_mode str object
     *
     * @param auth_str Buffer that stores extracted auth mode
     * @param mode Enum of the mode
     * @return int
     */
    int EZPI_core_wifi_get_auth_mode_str(char auth_str[50], wifi_auth_mode_t mode);
    /**
     * @brief Function to add new wifi event
     *
     * @param upcall
     * @param arg
     */
    void EZPI_core_wifi_event_add(f_ezlopi_wifi_event_upcall upcall, void *arg);
    /**
     * @brief Function to get wifi mac
     *
     * @param mac Buffer to store MAC
     * @return int
     */
    int EZPI_core_wifi_get_wifi_mac(uint8_t mac[6]);
    /**
     * @brief Function to trigger a wifi-scan
     *
     */
    void EZPI_core_wifi_scan_start();
    /**
     * @brief Function to stop wifi-scan task
     *
     */
    void EZPI_core_wifi_scan_stop();

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_CORE_WIFI_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
