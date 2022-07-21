#include <string>
#include <iostream>
#include <map>
#include <vector>

#include "esp_mac.h"
#include "esp_wifi_types.h"
#include "esp_idf_version.h"

#include "debug.h"
#include "frozen.h"
#include "web_provisioning.h"
#include "websocket_client.h"
#include "data.h"
#include "devices.h"
#include "scenes.h"
#include "registeration.h"
#include "favorite.h"
#include "gateways.h"
#include "info.h"
#include "modes.h"
#include "items.h"
#include "room.h"
#include "settings.h"

websocket_client ws_client;
static uint32_t message_counter = 0;
web_provisioning *web_provisioning::web_provisioning_ = nullptr;

extern string set_device_name(const char *data, uint32_t len, struct json_token *method_tok, uint32_t msg_count);
extern string set_device_value(const char *data, uint32_t len, struct json_token *method_tok, uint32_t msg_count);
extern string respond_hub_network_get(const char *data, uint32_t len, struct json_token *method_tok, uint32_t msg_count);
static string rpc_method_notfound(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
static string hub_reboot(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
typedef struct s_method_list
{
    string method_name;
    string (*method)(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
    string (*updater)(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
} s_method_list_t;

#warning "Todo: use vector-map instead of array"
s_method_list method_list[] = {
    /** Getter functions **/
    {.method_name = "hub.data.list", .method = data::list, .updater = NULL},
    {.method_name = "hub.room.list", .method = room::list, .updater = NULL},
    {.method_name = "hub.items.list", .method = items::list, .updater = NULL},
    {.method_name = "hub.scenes.list", .method = scenes::list, .updater = NULL},
    {.method_name = "hub.devices.list", .method = devices::list, .updater = NULL},
    {.method_name = "hub.favorite.list", .method = favorite::list, .updater = NULL},
    {.method_name = "hub.gateways.list", .method = gateways::list, .updater = NULL},
    {.method_name = "hub.info.get", .method = info::get, .updater = NULL},
    {.method_name = "hub.modes.get", .method = modes::get, .updater = NULL},
    {.method_name = "hub.network.get", .method = respond_hub_network_get, .updater = NULL},
    {.method_name = "hub.settings.list", .method = settings::list, .updater = NULL},
    {.method_name = "hub.device.settings.list", .method = devices::settings_list, .updater = NULL},
    {.method_name = "hub.reboot", .method = hub_reboot, .updater = NULL},

    /** Setter functions **/
    {.method_name = "hub.item.value.set", .method = items::set_value, .updater = items::update},
    {.method_name = "hub.device.name.set", .method = set_device_name, .updater = NULL},
    {.method_name = "hub.device.setting.value.set", .method = set_device_value, .updater = NULL},
    {.method_name = "registered", .method = registeration::registered, .updater = NULL}, // called only once so its in last

    {.method_name = "hub.feature.status.set", .method = rpc_method_notfound, .updater = NULL}, // documentation missing
    {.method_name = "hub.features.list", .method = rpc_method_notfound, .updater = NULL},      // documentation missing
};

void web_provisioning::send_to_nma_websocket(string &data)
{
    if (data.length())
    {
        ws_client.send(data);
        message_counter++;
    }
}

static string hub_reboot(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    string ret = "";
    char send_buf[200];
    struct json_token msg_id = JSON_INVALID_TOKEN;
    struct json_token sender = JSON_INVALID_TOKEN;

    json_scanf(payload, len, "{id:%T}", &msg_id);
    json_scanf(payload, len, "{sender:%T}", &sender);

    static const char *reboot_frmt = "{\"id\":\"%.*s\",\"method\":\"%.*s\",\"msg_id\":%u,\"result\":{},\"error\":null,\"sender\":%.*s}";
    snprintf(send_buf, sizeof(send_buf), reboot_frmt,
             msg_id.len, msg_id.ptr,
             method->len, method->ptr,
             msg_count,
             sender.len ? sender.len : 2, sender.len ? sender.ptr : "{}");
    ret = send_buf;
    TRACE_I(">> WSS-Tx: [%d]\r\n%s", strlen(send_buf), send_buf);
    TRACE_W("Device will restart in 3 seconds...");
    vTaskDelay(3000);
    esp_restart();
    return ret;
}

void web_provisioning::message_upcall(const char *payload, uint32_t len)
{
    std::string j_response = "";
    int rpc_method_found = 0;
    struct json_token method_tok = JSON_INVALID_TOKEN;

    if (json_scanf(payload, len, "{method: %T}", &method_tok))
    {
        TRACE_D("<< WS Rx '%.*s'[%d]:\r\n%.*s", method_tok.len, method_tok.ptr, len, len, payload);
        string method_str = "";
        method_str.assign(method_tok.ptr, method_tok.len);

        uint32_t idx = sizeof(method_list) / sizeof(s_method_list_t);

        while (idx--)
        {
            uint32_t comp_len = method_list[idx].method_name.length() > method_str.length() ? method_list[idx].method_name.length() : method_str.length();

            if (0 == strncmp(method_str.c_str(), method_list[idx].method_name.c_str(), comp_len))
            {
                if (NULL != method_list[idx].method)
                {
                    j_response = method_list[idx].method(payload, len, &method_tok, message_counter);
                    send_to_nma_websocket(j_response);
                }

                if (NULL != method_list[idx].updater)
                {
                    j_response = method_list[idx].updater(payload, len, &method_tok, message_counter);
                    send_to_nma_websocket(j_response);
                }

                rpc_method_found = 1;
                break;
            }
        }

        if (0 == rpc_method_found)
        {
            j_response = rpc_method_notfound(payload, len, &method_tok, message_counter);
            send_to_nma_websocket(j_response);
        }
    }
    else
    {
        TRACE_E("<< WS Rx '%.*s'[%d]:\r\n%.*s", method_tok.len, method_tok.ptr, len, len, payload);
    }
}

void web_provisioning::init(string &ws_uri)
{
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 4, 1)
    ws_client.websocket_app_start(ws_uri, message_upcall);
#else
#error "Minimum IDF version required is v4.4.1!"
#endif
    registeration *reg = registeration::get_instance();
    reg->init(&ws_client);
}

uint32_t web_provisioning::get_message_count(void)
{
    message_counter++;
    return message_counter;
}

web_provisioning *web_provisioning::get_instance(void)
{
    if (nullptr == web_provisioning_)
    {
        web_provisioning_ = new web_provisioning();
    }

    return web_provisioning_;
}

static string rpc_method_notfound(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    string ret = "";
    char data_buf[300] = {'\0'};
    struct json_token msg_id = JSON_INVALID_TOKEN;
    struct json_token sender = JSON_INVALID_TOKEN;
    static const char *rpc_notfound = "{\"msg_id\":%d,\"id\":\"%.*s\",\"method\":\"%.*s\",\"error\":{\"code\":-32602,\"data\":\"rpc.method.notfound\",\"message\":\"Unknown method\"},\"result\":{}, \"sender\":%.*s}";

    json_scanf(payload, len, "{id:%T}", &msg_id);
    int sender_status = json_scanf(payload, len, "{sender:%T}", &sender);

    snprintf(data_buf, sizeof(data_buf), rpc_notfound, msg_count, msg_id.len, msg_id.ptr, method->len, method->ptr, sender_status ? sender.len : 2, sender_status ? sender.ptr : "{}");
    TRACE_W(">> WSS-Tx: [%d]\r\n%s", strlen(data_buf), data_buf);
    ret = data_buf;
    return ret;
}