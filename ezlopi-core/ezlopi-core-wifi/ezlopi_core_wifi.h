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
    typedef void (*f_ezlopi_wifi_event_upcall)(esp_event_base_t event, int32_t event_id, void* arg);

    typedef struct ll_ezlopi_wifi_event_upcall
    {
        void* arg;
        f_ezlopi_wifi_event_upcall upcall;
        struct ll_ezlopi_wifi_event_upcall* next;
    } ll_ezlopi_wifi_event_upcall_t;

    typedef struct ezlopi_wifi_status
    {
        bool wifi_connection;
        esp_netif_ip_info_t* ip_info;
        wifi_mode_t wifi_mode;
    } ezlopi_wifi_status_t;

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
    int ezlopi_wifi_got_ip(void);
    void ezlopi_wifi_initialize(void);
    esp_err_t ezlopi_wifi_connect(const char* ssid, const char* pass);
    void ezlopi_wifi_connect_from_id_bin(void);
    ezlopi_error_t ezlopi_wait_for_wifi_to_connect(uint32_t wait_time_ms);
    esp_netif_ip_info_t* ezlopi_wifi_get_ip_infos(void);
    const char* ezlopi_wifi_get_last_disconnect_reason(void);
    int ezlopi_wifi_try_connect(cJSON *cj_network);

    ezlopi_wifi_status_t* ezlopi_wifi_status(void);
    int get_auth_mode_str(char auth_str[50], wifi_auth_mode_t mode);

    void ezlopi_wifi_event_add(f_ezlopi_wifi_event_upcall upcall, void* arg);

    int ezlopi_wifi_get_wifi_mac(uint8_t mac[6]);

    void ezlopi_wifi_scan_start();
    void ezlopi_wifi_scan_stop();

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_CORE_WIFI_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
