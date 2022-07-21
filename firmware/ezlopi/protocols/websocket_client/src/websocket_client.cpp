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

#include "esp_websocket_client.h"
#include "protocol_examples_common.h"

#include "factory_info.h"
#include "websocket_client.h"
#include "debug.h"

using namespace std;

typedef struct s_ws_event_arg
{
    esp_websocket_client_handle_t client;
    void (*upcall)(const char *, uint32_t);
    /* data */
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
        // static const string send_hello = "Hello from client!";
        // esp_websocket_client_send_text(event_arg->client, send_hello.c_str(), send_hello.length(), portMAX_DELAY);
        break;
    }
    case WEBSOCKET_EVENT_DISCONNECTED:
    {
        TRACE_I("WEBSOCKET_EVENT_DISCONNECTED");
        break;
    }
    case WEBSOCKET_EVENT_DATA:
    {
        if (data->data_ptr && data->data_len)
        {
            event_arg->upcall(data->data_ptr, data->data_len);
        }
        // TRACE_I("WEBSOCKET_EVENT_DATA");
        // TRACE_I("Received opcode=%d", data->op_code);
        // TRACE_W("Received=%.*s", data->data_len, (char *)data->data_ptr);
        // TRACE_W("Total payload length=%d, data_len=%d, current payload offset=%d\r\n", data->payload_len, data->data_len, data->payload_offset);
        break;
    }
    case WEBSOCKET_EVENT_ERROR:
    {
        TRACE_I("WEBSOCKET_EVENT_ERROR");
        break;
    }
    }
}

int websocket_client::send(std::string &_str)
{
    int ret = false;
    if (esp_websocket_client_is_connected(client) && _str.length())
    {
        ret = esp_websocket_client_send_text(client, _str.c_str(), _str.length(), portMAX_DELAY);
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
    return ret;
}

esp_websocket_client_handle_t websocket_client::websocket_app_start(string &uri, void (*upcall)(const char *, uint32_t))
{
    if (!client)
    {
        static s_ws_event_arg_t event_arg = {
            .upcall = upcall,
        };

        factory_info *factory = factory_info::get_instance();

        esp_websocket_client_config_t websocket_cfg = {
            .uri = uri.c_str(),
            .task_stack = 10 * 1024,
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
