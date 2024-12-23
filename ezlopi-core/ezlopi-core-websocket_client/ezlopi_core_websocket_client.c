
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
* @file    ezlopi_core_websocket_client.c
* @brief   perform some function on websocket_client
* @author  xx
* @version 0.1
* @date    12th DEC 2024
*/

/*******************************************************************************
*                          Include Files
*******************************************************************************/

#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_core_sntp.h"
#include "ezlopi_core_errors.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_websocket_client.h"

#include "EZLOPI_USER_CONFIG.h"

/*******************************************************************************
*                          Extern Data Declarations
*******************************************************************************/

/*******************************************************************************
*                          Extern Function Declarations
*******************************************************************************/

/*******************************************************************************
*                          Type & Macro Definitions
*******************************************************************************/
#define WSS_RX_BUFFER_SIZE 1024

typedef struct s_ws_event_arg
{
    esp_websocket_client_handle_t client;
    int (*msg_upcall)(char *, uint32_t, time_t time_ms);
    void (*connection_upcall)(bool connected);
} s_ws_event_arg_t;

typedef struct s_ws_data_buffer
{
    char *buffer;
    uint32_t len;
    uint32_t tot_len;
    struct s_ws_data_buffer *next;
} s_ws_data_buffer_t;

/*******************************************************************************
*                          Static Function Prototypes
*******************************************************************************/
static void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

/*******************************************************************************
*                          Static Data Definitions
*******************************************************************************/

/*******************************************************************************
*                          Extern Data Definitions
*******************************************************************************/

/*******************************************************************************
*                          Extern Function Definitions
*******************************************************************************/
ezlopi_error_t EZPI_core_websocket_client_send(esp_websocket_client_handle_t client, char *data, uint32_t len, uint32_t timeout_ms)
{
    ezlopi_error_t ret = EZPI_FAILED;

    if ((NULL != data) && (len > 0) && (NULL != client))
    {
        if (esp_websocket_client_is_connected(client) && (len > 0) && (NULL != data))
        {
            ret = esp_websocket_client_send_text(client, data, len, 10000 / portTICK_RATE_MS);
            ret = (ret > 0) ? EZPI_SUCCESS : EZPI_FAILED;
        }
    }

    return ret;
}

bool EZPI_core_websocket_client_is_connected(esp_websocket_client_handle_t client)
{
    return esp_websocket_client_is_connected(client);
}

void EZPI_core_websocket_client_kill(esp_websocket_client_handle_t client)
{
    esp_websocket_client_stop(client);
    TRACE_S("Websocket Stopped");
    esp_websocket_client_destroy(client);
    client = NULL;
}

esp_websocket_client_handle_t EZPI_core_websocket_client_init(cJSON *uri, int (*msg_upcall)(char *, uint32_t, time_t time_ms), void (*connection_upcall)(bool connected),
    char *ca_certificate, char *ssl_private_key, char *ssl_shared_key)
{
    esp_websocket_client_handle_t client = NULL;

    if ((NULL == client) && (NULL != uri) && (NULL != uri->valuestring) && (NULL != msg_upcall))
    {
        s_ws_event_arg_t *event_arg = ezlopi_malloc(__FUNCTION__, sizeof(s_ws_event_arg_t));
        if (event_arg)
        {
            event_arg->client = client;
            event_arg->msg_upcall = msg_upcall;
            event_arg->connection_upcall = connection_upcall;
        }

        esp_websocket_client_config_t websocket_cfg = {
            .task_stack = 5120,
            .keep_alive_enable = 1,
            .uri = uri->valuestring,
            .pingpong_timeout_sec = 30,
            .cert_pem = ca_certificate,
            .client_key = ssl_private_key,
            .client_cert = ssl_shared_key,
            .buffer_size = WSS_RX_BUFFER_SIZE,
            .ping_interval_sec = EZPI_CORE_WSS_PING_INTERVAL_SEC,
        };

        TRACE_S("Connecting to %s...", websocket_cfg.uri);

        client = esp_websocket_client_init(&websocket_cfg);

        if (client)
        {
            esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)event_arg);
            esp_websocket_client_start(client);
        }
    }

    return client;
}

/*******************************************************************************
*                         Static Function Definitions
*******************************************************************************/

static void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    s_ws_event_arg_t *event_arg = (s_ws_event_arg_t *)handler_args;
    esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;
    switch (event_id)
    {
    case WEBSOCKET_EVENT_CONNECTED:
    {
        TRACE_S("WEBSOCKET_EVENT_CONNECTED");
        if (event_arg && event_arg->connection_upcall)
        {
            event_arg->connection_upcall(true);
        }
        break;
    }
    case WEBSOCKET_EVENT_DISCONNECTED:
    {
        TRACE_E("WEBSOCKET_EVENT_DISCONNECTED");
        if (event_arg && event_arg->connection_upcall)
        {
            event_arg->connection_upcall(false);
        }

        break;
    }
    case WEBSOCKET_EVENT_DATA:
    {
        // op_code = 0x01: text-data,
        if (0x01 == data->op_code)
        {
            if ((NULL != data->data_ptr) && (data->data_len > 0) &&
                (NULL != event_arg) && (NULL != event_arg->msg_upcall))
            {
                // process the data if all data is received once
                if (data->payload_len == data->data_len)
                {
                    time_t now = EZPI_core_sntp_get_current_time_sec();
                    char *tmp_buffer = ezlopi_malloc(__FUNCTION__, data->data_len + 1);
                    if (tmp_buffer)
                    {
                        tmp_buffer[data->data_len] = '\0';
                        memcpy(tmp_buffer, data->data_ptr, data->data_len);
                        if (0 == event_arg->msg_upcall(tmp_buffer, strlen(tmp_buffer), now))
                        {
                            ezlopi_free(__FUNCTION__, tmp_buffer);
                        }
                    }
                }
                else
                {
                    static time_t now = 0;
                    if (0 == now)
                    {
                        now = EZPI_core_sntp_get_current_time_sec();
                    }

                    static char *s_buffer = NULL;
                    static uint32_t chunk_count = 0;

                    if (0 == data->payload_offset)
                    {
                        if (s_buffer)
                        {
                            chunk_count = 0;
                            ezlopi_free(__FUNCTION__, s_buffer);
                            s_buffer = NULL;
                        }

                        s_buffer = ezlopi_malloc(__FUNCTION__, data->payload_len + 1);
                        if (s_buffer)
                        {
                            chunk_count = 1;
                            memcpy(s_buffer, data->data_ptr, data->data_len);
                            s_buffer[data->payload_len] = '\0';
                        }
                    }
                    else if (s_buffer)
                    {
                        chunk_count += 1;
                        memcpy(s_buffer + data->payload_offset, data->data_ptr, data->data_len);
                    }

                    if ((data->payload_offset + data->data_len) >= data->payload_len)
                    {
                        if (0 == event_arg->msg_upcall(s_buffer, data->payload_len, now))
                        {
                            ezlopi_free(__FUNCTION__, s_buffer);
                        }

                        now = 0;
                        s_buffer = NULL;
                        chunk_count = 0;
                    }
                }
            }
        }
        break;
    }
    case WEBSOCKET_EVENT_ERROR:
    {
        TRACE_E("WEBSOCKET_EVENT_ERROR");
        break;
    }
    default:
    {
        TRACE_E("Websocket event type not-implemented! value: %u", event_id);
        break;
    }
    }
}


/*******************************************************************************
*                          End of File
*******************************************************************************/