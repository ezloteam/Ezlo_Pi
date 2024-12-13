/* ESP HTTP Client Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_websocket_client.h"
#include "ezlopi_core_errors.h"

#include "EZLOPI_USER_CONFIG.h"

#define WSS_RX_BUFFER_SIZE 1024

static esp_websocket_client_handle_t client = NULL;

typedef struct s_ws_event_arg
{
    esp_websocket_client_handle_t client;
    int (*msg_upcall)(const char *, uint32_t, time_t time_ms);
    // void (*msg_upcall)(const char *, uint32_t);
    void (*connection_upcall)(bool connected);
} s_ws_event_arg_t;

typedef struct s_ws_data_buffer
{
    char *buffer;
    uint32_t len;
    uint32_t tot_len;
    struct s_ws_data_buffer *next;
} s_ws_data_buffer_t;

static void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

ezlopi_error_t ezlopi_websocket_client_send(char *data, uint32_t len)
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

bool ezlopi_websocket_client_is_connected(void)
{
    return esp_websocket_client_is_connected(client);
}

void ezlopi_websocket_client_kill(void)
{
    esp_websocket_client_stop(client);
    TRACE_S("Websocket Stopped");
    esp_websocket_client_destroy(client);
    client = NULL;
}

// esp_websocket_client_handle_t ezlopi_websocket_client_init(cJSON *uri, void (*msg_upcall)(const char *, uint32_t), void (*connection_upcall)(bool connected))
esp_websocket_client_handle_t ezlopi_websocket_client_init(cJSON *uri, int (*msg_upcall)(const char *, uint32_t, time_t time_ms), void (*connection_upcall)(bool connected))
{
    if ((NULL == client) && (NULL != uri) && (NULL != uri->valuestring) && (NULL != msg_upcall))
    {
        char *ca_cert = ezlopi_factory_info_v3_get_ca_certificate();
        char *ssl_priv = ezlopi_factory_info_v3_get_ssl_private_key();
        char *ssl_shared = ezlopi_factory_info_v3_get_ssl_shared_key();

        static s_ws_event_arg_t event_arg;
        event_arg.client = client;
        event_arg.msg_upcall = msg_upcall;
        event_arg.connection_upcall = connection_upcall;

        esp_websocket_client_config_t websocket_cfg = {
            .uri = uri->valuestring,
            .task_stack = 5 * 1024,
            .buffer_size = WSS_RX_BUFFER_SIZE,
            // .task_stack = EZPI_CORE_WSS_TASK_STACK_SIZE,
            // .buffer_size = EZPI_CORE_WSS_DATA_BUFFER_SIZE,
            .cert_pem = ca_cert,
            .client_key = ssl_priv,
            .client_cert = ssl_shared,
            .keep_alive_enable = 1,
            .ping_interval_sec = EZPI_CORE_WSS_PING_INTERVAL_SEC,
            // .pingpong_timeout_sec = EZPI_CORE_WSS_PING_PONG_TIMEOUT_SEC,
            .pingpong_timeout_sec = 30,
        };

        TRACE_S("Connecting to %s...", websocket_cfg.uri);

        client = esp_websocket_client_init(&websocket_cfg);
        // client = esp_websocket_client_init(&websocket_cfg);

        if (client)
        {
            esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)&event_arg);
            esp_websocket_client_start(client);
        }
    }
    else
    {
        TRACE_I("Client already active!");
    }

    return client;
}

static void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    s_ws_event_arg_t *event_arg = (s_ws_event_arg_t *)handler_args;
    esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;
    switch (event_id)
    {
    case WEBSOCKET_EVENT_CONNECTED:
    {
        TRACE_W("free-heap:     %.02f KB", esp_get_free_heap_size() / 1024.0);
        TRACE_S("WEBSOCKET_EVENT_CONNECTED");
        if (event_arg && event_arg->connection_upcall)
        {
            event_arg->connection_upcall(true);
        }
        break;
    }
    case WEBSOCKET_EVENT_DISCONNECTED:
    {
        TRACE_W("free-heap:     %.02f KB", esp_get_free_heap_size() / 1024.0);
        TRACE_E("WEBSOCKET_EVENT_DISCONNECTED");
        if (event_arg && event_arg->connection_upcall)
        {
            event_arg->connection_upcall(false);
        }

        break;
    }
    case WEBSOCKET_EVENT_DATA:
    {
        if (0x01 == data->op_code) // op_code = 0x01: text-data,
        {

            if ((NULL != data->data_ptr) && (data->data_len > 0) &&
                (NULL != event_arg) && (NULL != event_arg->msg_upcall))
            {
                if (data->payload_len == data->data_len) // process the data if all data is received once
                {
                    time_t now;
                    time(&now);

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
                        time(&now);
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

                        s_buffer = NULL;
                        chunk_count = 0;
                    }
                }
            }
            // TRACE_D("----------------------------------------------------------------------------------------------------------------");
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
