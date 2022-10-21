#include <string.h>

#include "cJSON.h"
#include "esp_mac.h"
#include "esp_wifi_types.h"
#include "esp_idf_version.h"

#include "trace.h"
// #include "frozen.h"
// #include "web_provisioning.h"
// #include "wss.h"
#include "data.h"
// #include "devices.h"
// #include "scenes.h"
#include "registeration.h"
// #include "favorite.h"
// #include "gateways.h"
// #include "info.h"
// #include "modes.h"
// #include "items.h"
// #include "room.h"
// #include "settings.h"
// #include "network.h"

#include "ezlopi_factory_info.h"
#include "ezlopi_wifi.h"
#include "ezlopi_http.h"
#include "ezlopi_wss.h"

static uint32_t message_counter = 0;

static void __message_upcall(const char *payload, uint32_t len);
static char *__hub_reboot(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
static char *__rpc_method_notfound(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
static void web_provisioning_fetch_wss_endpoint(void *pv);

typedef struct s_method_list
{
    char *method_name;
    char *(*method)(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
    char *(*updater)(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
} s_method_list_t;

s_method_list_t method_list[] = {
    /** Getter functions **/
    {.method_name = "hub.data.list", .method = data_list, .updater = NULL},
    // {.method_name = "hub.room.list", .method = room_list, .updater = NULL},
    // {.method_name = "hub.items.list", .method = items_list, .updater = NULL},
    // {.method_name = "hub.scenes.list", .method = scenes_list, .updater = NULL},
    // {.method_name = "hub.devices.list", .method = devices_list, .updater = NULL},
    // {.method_name = "hub.favorite.list", .method = favorite_list, .updater = NULL},
    // {.method_name = "hub.gateways.list", .method = gateways_list, .updater = NULL},
    // {.method_name = "hub.info.get", .method = info_get, .updater = NULL},
    // {.method_name = "hub.modes.get", .method = modes_get, .updater = NULL},
    // {.method_name = "hub.network.get", .method = network_get, .updater = NULL}, //, .updater = NULL},
    // {.method_name = "hub.settings.list", .method = settings_list, .updater = NULL},
    // {.method_name = "hub.device.settings.list", .method = devices_settings_list, .updater = NULL},
    // {.method_name = "hub.reboot", .method = __hub_reboot, .updater = NULL},

    // // /** Setter functions **/
    // {.method_name = "hub.item.value.set", .method = items_set_value, .updater = items_update},
    // {.method_name = "hub.device.name.set", .method = devices_name_set, .updater = NULL},
    // {.method_name = "hub.device.setting.value.set", .method = __rpc_method_notfound, .updater = NULL},
    {.method_name = "registered", .method = registered, .updater = NULL}, // called only once so its in last

    // {.method_name = "hub.feature.status.set", .method = __rpc_method_notfound, .updater = NULL}, // documentation missing
    // {.method_name = "hub.features.list", .method = __rpc_method_notfound, .updater = NULL},      // documentation missing
};

uint32_t web_provisioning_get_message_count(void)
{
    return message_counter;
}

void web_provisioning_send_to_nma_websocket(char *data)
{
    if (data)
    {
        // ws_client.send(data);
        // TRACE_D("WSS-SENDING: %s", data);
        wss_client_send(data, strlen(data));
        message_counter++;
    }
}

void web_provisioning_init(void)
{
    xTaskCreate(web_provisioning_fetch_wss_endpoint, "web-provisioning fetch wss endpoint", 3 * 2048, NULL, 5, NULL);
}

static void web_provisioning_fetch_wss_endpoint(void *pv)
{
    s_ezlopi_factory_info_t *factory = ezlopi_factory_info_get_info();
    char *ws_endpoint = NULL;

    while (1)
    {
        UBaseType_t water_mark = uxTaskGetStackHighWaterMark(NULL);
        TRACE_D("water_mark: %d", water_mark);

        ezlopi_wait_for_wifi_to_connect();

        char http_request[128];
        snprintf(http_request, sizeof(http_request), "%s/getserver?json=true", factory->cloud_server);
        ws_endpoint = ezlopi_http_get_request(http_request, factory->ssl_private_key, factory->ssl_shared_key, factory->ca_certificate);

        if (ws_endpoint)
        {
            TRACE_D("ws_endpoint: %s", ws_endpoint);
            cJSON *root = cJSON_Parse(ws_endpoint);
            if (root)
            {
                cJSON *cjson_uri = cJSON_GetObjectItem(root, "uri");
                if (cjson_uri)
                {
                    TRACE_D("uri: %s", cjson_uri->valuestring ? cjson_uri->valuestring : "NULL");
                    ezlopi_client_init(cjson_uri->valuestring, __message_upcall);
                    registeration_init();
                    break;
                }
            }
        }
    }

    vTaskDelete(NULL);
}

static void __message_upcall(const char *payload, uint32_t len)
{
    char *j_response = NULL;
    int rpc_method_found = 0;
    struct json_token method_tok = JSON_INVALID_TOKEN;

    if (json_scanf(payload, len, "{method: %T}", &method_tok))
    {
        TRACE_D("<< WS Rx '%.*s'[%d]:\r\n%.*s", method_tok.len, method_tok.ptr, len, len, payload);

        uint32_t idx = sizeof(method_list) / sizeof(s_method_list_t);

        while (idx--)
        {
            uint32_t comp_len = strlen(method_list[idx].method_name) > method_tok.len ? strlen(method_list[idx].method_name) : method_tok.len;

            if (0 == strncmp(method_tok.ptr, method_list[idx].method_name, comp_len))
            {
                if (NULL != method_list[idx].method)
                {
                    j_response = method_list[idx].method(payload, len, &method_tok, message_counter);
                }

                if (NULL != method_list[idx].updater)
                {
                    j_response = method_list[idx].updater(payload, len, &method_tok, message_counter);
                }

                rpc_method_found = 1;
                break;
            }
        }

        if (0 == rpc_method_found)
        {
            j_response = __rpc_method_notfound(payload, len, &method_tok, message_counter);
        }

        if (j_response)
        {
            wss_client_send(j_response, strlen(j_response));
            free(j_response);
            j_response = NULL;
        }
        else
        {
            TRACE_E("Error - j_response: %d", (uint32_t)j_response);
        }
    }
    else
    {
        TRACE_E("<< WS Rx '%.*s'[%d]:\r\n%.*s", method_tok.len, method_tok.ptr, len, len, payload);
    }
}

static char *__rpc_method_notfound(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    uint32_t buf_len = 300;
    char *data_buf = (char *)malloc(buf_len);

    if (data_buf)
    {
        memset(data_buf, 0, buf_len);
        struct json_token msg_id = JSON_INVALID_TOKEN;
        struct json_token sender = JSON_INVALID_TOKEN;
        static const char *rpc_notfound = "{\"msg_id\":%d,\"id\":\"%.*s\",\"method\":\"%.*s\",\"error\":{\"code\":-32602,\"data\":\"rpc.method.notfound\",\"message\":\"Unknown method\"},\"result\":{}, \"sender\":%.*s}";

        json_scanf(payload, len, "{id:%T}", &msg_id);
        int sender_status = json_scanf(payload, len, "{sender:%T}", &sender);

        snprintf(data_buf, buf_len, rpc_notfound, msg_count, msg_id.len, msg_id.ptr, method->len, method->ptr, sender_status ? sender.len : 2, sender_status ? sender.ptr : "{}");
        TRACE_W(">> WSS-Tx: [%d]\r\n%s", strlen(data_buf), data_buf);
    }

    return data_buf;
}

static char *__hub_reboot(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    esp_restart();
    return NULL;
}
