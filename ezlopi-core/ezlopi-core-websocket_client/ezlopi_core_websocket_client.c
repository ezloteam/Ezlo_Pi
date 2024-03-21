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

static esp_websocket_client_handle_t client = NULL;
static void (*__msg_upcall)(const char*, uint32_t) = NULL;

typedef struct s_ws_event_arg
{
    esp_websocket_client_handle_t client;
    void (*msg_upcall)(const char*, uint32_t);
    void (*connection_upcall)(bool connected);
} s_ws_event_arg_t;

typedef struct s_ws_data_buffer
{
    char* buffer;
    uint32_t len;
    uint32_t tot_len;
    struct s_ws_data_buffer* next;
} s_ws_data_buffer_t;

// static void ezlopi_ws_data_buffer_free(s_ws_data_buffer_t *buffer);
// static s_ws_data_buffer_t *ezlopi_ws_data_buffer_create(char *data, uint32_t len);
// static s_ws_data_buffer_t *ezlopi_ws_data_buffer_add(s_ws_data_buffer_t *head_buffer, s_ws_data_buffer_t *data_buffer);

static void websocket_event_handler(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data);

int ezlopi_websocket_client_send(char* data, uint32_t len)
{
    int ret = 0;

    if ((NULL != data) && (len > 0) && (NULL != client))
    {
        if (esp_websocket_client_is_connected(client) && (len > 0) && (NULL != data))
        {
            ret = esp_websocket_client_send_text(client, data, len, portMAX_DELAY);
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

esp_websocket_client_handle_t ezlopi_websocket_client_init(cJSON* uri, void (*msg_upcall)(const char*, uint32_t), void (*connection_upcall)(bool connected))
{

    if ((NULL == client) && (NULL != uri) && (NULL != uri->valuestring) && (NULL != msg_upcall))
    {
        char* ca_cert = ezlopi_factory_info_v3_get_ca_certificate();
        char* ssl_shared = ezlopi_factory_info_v3_get_ssl_shared_key();
        char* ssl_priv = ezlopi_factory_info_v3_get_ssl_private_key();
        __msg_upcall = msg_upcall;

        static s_ws_event_arg_t event_arg;
        event_arg.client = client;
        event_arg.msg_upcall = msg_upcall;
        event_arg.connection_upcall = connection_upcall;

        esp_websocket_client_config_t websocket_cfg = {
            .uri = uri->valuestring,
            .task_stack = 6 * 1024,
            .buffer_size = 6 * 1024,
            .cert_pem = ca_cert,
            .client_cert = ssl_shared,
            .client_key = ssl_priv,
            .pingpong_timeout_sec = 21,
            .keep_alive_enable = 1,
            .ping_interval_sec = 10,
        };

        TRACE_S("Connecting to %s...", websocket_cfg.uri);

        client = esp_websocket_client_init(&websocket_cfg);
        if (client)
        {
            esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void*)&event_arg);
            esp_websocket_client_start(client);
        }
        else
        {
            free(ca_cert);
            free(ssl_shared);
            free(ssl_priv);
        }
    }
    else
    {
        TRACE_I("Client already active!");
    }

    return client;
}

static void websocket_event_handler(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data)
{
    s_ws_event_arg_t* event_arg = (s_ws_event_arg_t*)handler_args;
    esp_websocket_event_data_t* data = (esp_websocket_event_data_t*)event_data;
    switch (event_id)
    {
    case WEBSOCKET_EVENT_CONNECTED:
    {
        TRACE_S("WEBSOCKET_EVENT_CONNECTED");
        if (event_arg && event_arg->connection_upcall)
        {
            event_arg->connection_upcall(1);
        }
        break;
    }
    case WEBSOCKET_EVENT_DISCONNECTED:
    {
        TRACE_S("WEBSOCKET_EVENT_DISCONNECTED");
        if (event_arg && event_arg->connection_upcall)
        {
            event_arg->connection_upcall(0);
        }
        break;
    }
    case WEBSOCKET_EVENT_DATA:
    {
        if ((NULL != data->data_ptr) && (data->data_len > 0) && (NULL != event_arg) && (NULL != event_arg->msg_upcall))
        {
            event_arg->msg_upcall(data->data_ptr, data->data_len);
        }
        break;
    }
    case WEBSOCKET_EVENT_ERROR:
    {
        TRACE_S("WEBSOCKET_EVENT_ERROR");
        break;
    }
    default:
    {
        TRACE_W("Websocket event type not-implemented! value: %u", event_id);
        break;
    }
    }
}

// static s_ws_data_buffer_t *ezlopi_ws_data_buffer_create(char *data, uint32_t len)
// {
//     s_ws_data_buffer_t *new_buffer = malloc(sizeof(s_ws_data_buffer_t));
//     if (new_buffer)
//     {
//         memset(new_buffer, 0, sizeof(s_ws_data_buffer_t));
//         new_buffer->buffer = malloc(len + 1);

//         if (new_buffer->buffer)
//         {
//             new_buffer->len = len;
//             memcpy(new_buffer->buffer, data, len);
//             new_buffer->buffer[len] = '\0';
//         }
//         else
//         {
//             free(new_buffer);
//             new_buffer = NULL;
//         }
//     }

//     return new_buffer;
// }

// static s_ws_data_buffer_t *ezlopi_ws_data_buffer_add(s_ws_data_buffer_t *head_buffer, s_ws_data_buffer_t *data_buffer)
// {
//     if (data_buffer)
//     {
//         if (head_buffer)
//         {
//             s_ws_data_buffer_t *curr_head = head_buffer;
//             while (curr_head->next)
//             {
//                 curr_head = curr_head->next;
//             }
//             curr_head->next = data_buffer;
//         }
//         else
//         {
//             head_buffer = data_buffer;
//         }
//     }

//     return head_buffer;
// }

// static void ezlopi_ws_data_buffer_free(s_ws_data_buffer_t *buffer)
// {
//     if (buffer)
//     {
//         ezlopi_ws_data_buffer_free(buffer->next);
//         if (buffer->buffer)
//         {
//             free(buffer->buffer);
//         }
//         free(buffer);
//     }
// }
