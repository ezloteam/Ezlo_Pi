
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
 * @file    ezlopi_core_websocket_client.h
 * @brief   perform some function on websocket_client
 * @author  ezlopi_team_np
 * @version 0.1
 * @date    12th DEC 2024
 */

#ifndef _EZLOPI_CORE_WEBSOCKET_CLIENT_H_
#define _EZLOPI_CORE_WEBSOCKET_CLIENT_H_
/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_log.h"
#include "../../build/config/sdkconfig.h"
#include "cjext.h"

#include "ezlopi_core_errors.h"

#include "esp_websocket_client.h"

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
     * @brief  Function to send websocket client request
     *
     * @param client websocket client structure
     * @param data  Target request-data
     * @param len Length of data
     * @param timeout_ms Timeout in ms
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_websocket_client_send(esp_websocket_client_handle_t client, char *data, uint32_t len, uint32_t timeout_ms);
    /**
     * @brief  Function go initialize websocket client service
     *
     * @param uri Pointer to request url
     * @param msg_upcall Function ptr to callback_msg_method
     * @param connection_upcall  Function ptr to callback_connection_method
     * @param ca_cert Certificate creds
     * @param ssl_private SSL-privatet key
     * @param ssl_shared SSL-shared key
     * @return esp_websocket_client_handle_t
     */
    esp_websocket_client_handle_t EZPI_core_websocket_client_init(cJSON *uri, int (*msg_upcall)(char *, uint32_t, time_t time_stamp), void (*connection_upcall)(bool connected),
                                                                  char *ca_cert, char *ssl_private, char *ssl_shared);
    /**
     * @brief Function to get websocket connection status
     *
     * @param client Target client
     * @return true
     * @return false
     */
    bool EZPI_core_websocket_client_is_connected(esp_websocket_client_handle_t client);
    /**
     * @brief Function to destroy websocket client
     *
     * @param client  Target Client
     */
    void EZPI_core_websocket_client_kill(esp_websocket_client_handle_t client);

#if 0
    class websocket_client
    {
    private:
    public:
        /**
         * @brief Initiates the websocket client and return the object of the client
         * @brief Right now instanciate only one client. If called 'websocket_app_start' function - returns running client instance
         *
         * @return esp_websocket_client_handle_t* object of the initiated client
         */
        esp_websocket_client_handle_t websocket_app_start(std::string &uri, void (*upcall)(const char *, uint32_t));

        /**
         * @brief      Write textual data to the WebSocket connection (data send with WS OPCODE=01, i.e. text)
         *
         * @param[in]  string -> text to send
         *
         * @return
         *     - Number of data was sent
         *     - (-1) if any errors
         */
        int send(std::string &_str);
        int send(char *c_str);

        bool is_connected(void);

        /**
         * @brief Kills the current websocket instance and set to null
         *
         */
        void websocket_client_kill(void);
    };
#endif

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_CORE_WEBSOCKET_CLIENT_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
