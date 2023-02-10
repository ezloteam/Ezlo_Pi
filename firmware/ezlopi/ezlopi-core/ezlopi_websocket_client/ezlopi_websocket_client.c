/* ESP HTTP Client Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_event.h"
#include "sdkconfig.h"
#include "esp_system.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "cJSON.h"

#include "esp_websocket_client.h"
#include "protocol_examples_common.h"

#include "ezlopi_factory_info.h"
#include "ezlopi_websocket_client.h"
#include "trace.h"

typedef struct s_ws_event_arg
{
    esp_websocket_client_handle_t client;
    void (*msg_upcall)(const char *, uint32_t);
    void (*connection_upcall)(bool connected);
} s_ws_event_arg_t;

esp_websocket_client_handle_t client = NULL;

static void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    s_ws_event_arg_t *event_arg = (s_ws_event_arg_t *)handler_args;
    esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;
    switch (event_id)
    {
    case WEBSOCKET_EVENT_CONNECTED:
    {
        TRACE_I("WEBSOCKET_EVENT_CONNECTED");
        if (event_arg && event_arg->connection_upcall)
        {
            event_arg->connection_upcall(1);
        }
        break;
    }
    case WEBSOCKET_EVENT_DISCONNECTED:
    {
        TRACE_I("WEBSOCKET_EVENT_DISCONNECTED");
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
        TRACE_I("WEBSOCKET_EVENT_ERROR");
        break;
    }
    default:
    {
        TRACE_W("Websocket event type not-implemented! value: %u", event_id);
        break;
    }
    }
}

int ezlopi_websocket_client_send(char *data, uint32_t len)
{
    int ret = 0;

    if ((NULL != data) && (len > 0))
    {
        if (esp_websocket_client_is_connected(client) && (len > 0) && (NULL != data))
        {
            ret = esp_websocket_client_send_text(client, data, len, portMAX_DELAY);
        }
    }

    return ret;
}

esp_websocket_client_handle_t ezlopi_websocket_client_init(cJSON *uri, void (*msg_upcall)(const char *, uint32_t), void (*connection_upcall)(bool connected))
{
    if ((NULL == client) && (NULL != uri) && (NULL != uri->valuestring) && (NULL != msg_upcall))
    {
        static s_ws_event_arg_t event_arg;
        event_arg.client = client;
        event_arg.msg_upcall = msg_upcall;
        event_arg.connection_upcall = connection_upcall;

        // s_ezlopi_factory_info_t *factory = ezlopi_factory_info_get_info();

        esp_websocket_client_config_t websocket_cfg = {
            .uri = uri->valuestring,
            .task_stack = 8 * 1024,
            // .cert_pem = factory->ca_certificate,
            .cert_pem = ezlopi_factory_info_v2_get_ca_certificate(), // this needs to be freed after use
            // .client_cert = factory->ssl_shared_key,
            .client_cert = ezlopi_factory_info_v2_get_ssl_shared_key(), // this needs to be freed after use
            // .client_key = factory->ssl_private_key,
            .client_key = ezlopi_factory_info_v2_get_ssl_private_key(), // this needs to be freed after use
            .pingpong_timeout_sec = 20,
            .keep_alive_enable = 1,
            .ping_interval_sec = 10,
        };

        TRACE_I("Connecting to %s...", websocket_cfg.uri);

        client = esp_websocket_client_init(&websocket_cfg);
        esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)&event_arg);
        esp_websocket_client_start(client);

        ezlopi_factory_info_v2_free(websocket_cfg.cert_pem);
        ezlopi_factory_info_v2_free(websocket_cfg.client_cert);
        ezlopi_factory_info_v2_free(websocket_cfg.client_key);
        websocket_cfg.cert_pem = NULL;
        websocket_cfg.client_cert = NULL;
        websocket_cfg.client_key = NULL;
    }
    else
    {
        TRACE_I("Client already active!");
    }

    return client;
}

bool ezlopi_websocket_client_is_connected(void)
{
    return esp_websocket_client_is_connected(client);
}

void ezlopi_websocket_client_kill(void)
{
    esp_websocket_client_stop(client);
    TRACE_I("Websocket Stopped");
    esp_websocket_client_destroy(client);
    client = NULL;
}

#if 0
using namespace std;

int websocket_client::send(std::string &_str)
{
    int ret = false;
    if (esp_websocket_client_is_connected(client) && _str.length())
    {
        ret = esp_websocket_client_send_text(client, _str.c_str(), _str.length(), portMAX_DELAY);
    }
    else
    {
        TRACE_E("ERROR: websocket client is not connected!");
    }
    return ret;
}

int websocket_client::send(char *c_str)
{
    int ret = false;
    if (esp_websocket_client_is_connected(client))
    {
        ret = esp_websocket_client_send_text(client, c_str, strlen(c_str), portMAX_DELAY);
    }
    else
    {
        TRACE_E("ERROR: websocket is not connected!");
    }
    return ret;
}

esp_websocket_client_handle_t websocket_client::websocket_app_start(string &uri, void (*msg_upcall)(const char *, uint32_t))
{
    if (!client)
    {
        static s_ws_event_arg_t event_arg = {
            .msg_upcall = msg_upcall,
        };

        // ezlopi_factory_info *factory = ezlopi_factory_info::get_instance();
        s_ezlopi_factory_info_t *factory = ezlopi_factory_info_get_info();

        esp_websocket_client_config_t websocket_cfg = {
            .uri = uri.c_str(),
            .task_stack = 8 * 1024,
            .cert_pem = factory->ca_certificate,
            .client_cert = factory->ssl_shared_key,
            .client_key = factory->ssl_private_key,
            .pingpong_timeout_sec = 20,
            .keep_alive_enable = 1,
            .ping_interval_sec = 10,
        };

        TRACE_I("Connecting to %s...", websocket_cfg.uri);

        client = esp_websocket_client_init(&websocket_cfg);
        esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)&event_arg);
        esp_websocket_client_start(client);
        event_arg.client = client;
    }
    else
    {
        TRACE_I("Client already active!");
    }

    return client;
}

bool websocket_client::is_connected(void)
{
    return esp_websocket_client_is_connected(client);
}

void websocket_client::websocket_client_kill(void)
{
    esp_websocket_client_stop(client);
    TRACE_I("Websocket Stopped");
    esp_websocket_client_destroy(client);
    client = NULL;
}
#endif
