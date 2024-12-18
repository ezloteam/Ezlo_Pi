
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
 * @file    ezlopi_core_wsc.h
 * @brief   perform some function on data
 * @author  Krishna Kumar Sah (krriss.shah@gmail.com)
 * @version 0.1
 * @date    29-7-2022
*/
#ifndef __WSS_H__
#define __WSS_H__

/*******************************************************************************
*                          Include Files
*******************************************************************************/
#include <stdio.h>
#include "string.h"
#include "stdbool.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "esp_wifi.h"
#include "sys/queue.h"
#include "esp_event.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_wifi_types.h"

#include "mbedtls/platform.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/esp_debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"

#include "cjext.h"
#include "sdkconfig.h"

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
    typedef void (*f_wsc_conn_upcall_t)(bool connected);
    typedef void (*f_wsc_msg_upcall_t)(const char *payload, uint32_t len);

    typedef enum e_state_wss
    {
        STATE_DATA,
        STATE_HEADER,
    } e_state_wss_t;

    typedef struct s_ssl_websocket
    {
        char url[64];
        char port[8];
        uint32_t u_port;
        char path[64];

        char *buffer;
        uint32_t buffer_len;

        bool is_connected;
        e_state_wss_t e_state;

        mbedtls_ssl_config *conf;
        mbedtls_ssl_context *ssl_ctx;
        mbedtls_net_context *server_fd;
        mbedtls_entropy_context *entropy;
        mbedtls_ctr_drbg_context *ctr_drbg;

        mbedtls_x509_crt *cacert;
        mbedtls_x509_crt *shared_cert;
        mbedtls_pk_context *private_key;

        char *str_cacert;
        char *str_private_key;
        char *str_shared_cert;

        TimerHandle_t timer;
        TaskHandle_t task_handle;
        f_wsc_msg_upcall_t message_upcall_func;
        f_wsc_conn_upcall_t connection_upcall_func;

    } s_ssl_websocket_t;

    /*******************************************************************************
    *                          Extern Data Declarations
    *******************************************************************************/

    /*******************************************************************************
    *                          Extern Function Prototypes
    *******************************************************************************/
   
    /**
     * @brief Function to destroy websocket with ssl
     *
     * @param wsc_ssl
     * @return int
     */
    int ezlopi_core_wsc_kill(s_ssl_websocket_t *wsc_ssl);
    /**
     * @brief Function to initialize websocket with ssl
     *
     * @param uri
     * @param __message_upcall
     * @param __connection_upcall
     * @return s_ssl_websocket_t*
     */
    s_ssl_websocket_t *ezlopi_core_wsc_init(cJSON *uri, f_wsc_msg_upcall_t __message_upcall, f_wsc_conn_upcall_t __connection_upcall);
    /**
     * @brief Function to send data via websocket-ssl
     *
     * @param wsc_ssl required wsc-ssl info
     * @param buf_s Target data to send
     * @param len Length of data
     * @return int
     */
    int ezlopi_core_wsc_send(s_ssl_websocket_t *wsc_ssl, char *buf_s, size_t len);
    /**
     * @brief Function to check if websocket is connected
     *
     * @param wsc_ssl required wsc-ssl info
     * @return int
     */
    int ezlopi_core_wsc_is_connected(s_ssl_websocket_t *wsc_ssl);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __WSS_H__

/*******************************************************************************
*                          End of File
*******************************************************************************/