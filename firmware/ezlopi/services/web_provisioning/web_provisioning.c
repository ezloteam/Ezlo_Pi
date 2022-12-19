#include <string.h>

#include "cJSON.h"
#include "esp_mac.h"
#include "esp_wifi_types.h"
#include "esp_idf_version.h"

#include "trace.h"
#include "data.h"
#include "devices.h"
#include "scenes.h"
#include "registration.h"
#include "favorite.h"
#include "gateways.h"
#include "info.h"
#include "modes.h"
#include "items.h"
#include "room.h"
#include "feature.h"
#include "network.h"
#include "ezlopi_websocket_client.h"

#include "ezlopi_factory_info.h"
#include "ezlopi_wifi.h"
#include "ezlopi_http.h"

static uint32_t message_counter = 0;

static void __connection_upcall(bool connected);
static void __message_upcall(const char *payload, uint32_t len);
static cJSON *__hub_reboot(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
static cJSON *__rpc_method_notfound(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
static void web_provisioning_fetch_wss_endpoint(void *pv);

typedef struct s_method_list
{
    char *method_name;
    cJSON *(*method)(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
    cJSON *(*updater)(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
} s_method_list_t;

s_method_list_t method_list[] = {
    /** Getter functions **/
    {.method_name = "hub.data.list", .method = data_list, .updater = NULL},
    {.method_name = "hub.room.list", .method = room_list, .updater = NULL},
    {.method_name = "hub.items.list", .method = items_list, .updater = NULL},
    {.method_name = "hub.scenes.list", .method = scenes_list, .updater = NULL},
    {.method_name = "hub.devices.list", .method = devices_list, .updater = NULL},
    {.method_name = "hub.favorite.list", .method = favorite_list, .updater = NULL},
    {.method_name = "hub.gateways.list", .method = gateways_list, .updater = NULL},
    {.method_name = "hub.info.get", .method = info_get, .updater = NULL},
    {.method_name = "hub.modes.get", .method = modes_get, .updater = NULL},
    {.method_name = "hub.network.get", .method = network_get, .updater = NULL}, //, .updater = NULL},
    // {.method_name = "hub.settings.list", .method = settings_list, .updater = NULL},
    // {.method_name = "hub.device.settings.list", .method = devices_settings_list, .updater = NULL},
    {.method_name = "hub.reboot", .method = __hub_reboot, .updater = NULL},

    // // /** Setter functions **/
    {.method_name = "hub.item.value.set", .method = items_set_value, .updater = items_update},
    // {.method_name = "hub.device.name.set", .method = devices_name_set, .updater = NULL},
    // {.method_name = "hub.device.setting.value.set", .method = __rpc_method_notfound, .updater = NULL},
    {.method_name = "registered", .method = registered, .updater = NULL}, // called only once so its in last

    // {.method_name = "hub.feature.status.set", .method = __rpc_method_notfound, .updater = NULL}, // documentation missing
    // {.method_name = "hub.features.list", .method = __rpc_method_notfound, .updater = NULL}, // documentation missing
};

uint32_t web_provisioning_get_message_count(void)
{
    return message_counter;
}

int web_provisioning_send_to_nma_websocket(cJSON *cjson_data)
{
    int ret = 0;
    if (cjson_data)
    {
        char *cjson_str_data = cJSON_Print(cjson_data);
        if (cjson_str_data)
        {
            cJSON_Minify(cjson_str_data);
            TRACE_B("## WSS-SENDING >>>>>>>>>>\r\n%s", cjson_str_data);
            int ret = ezlopi_websocket_client_send(cjson_str_data, strlen(cjson_str_data));
            if (ret > 0)
            {
                message_counter++;
            }

            free(cjson_str_data);
        }
    }

    return ret;
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
                    ezlopi_websocket_client_init(cjson_uri, __message_upcall, __connection_upcall);
                    break;
                }
            }
        }
    }

    vTaskDelete(NULL);
}

static void __connection_upcall(bool connected)
{
    static bool prev_status;
    if (connected)
    {
        if (!prev_status)
        {
            TRACE_B("Starting registration process....");
            registration_init();
        }
        TRACE_I("Web-socket re-connected.");
    }
    else
    {
        TRACE_E("Web-socket dis-connected!");
    }

    prev_status = connected;
}

static void __message_upcall(const char *payload, uint32_t len)
{
    cJSON *cjson_response = NULL;
    int rpc_method_found = 0;
    struct json_token method_tok = JSON_INVALID_TOKEN;

    if (json_scanf(payload, len, "{method: %T}", &method_tok))
    {
        TRACE_D("## WS Rx <<<<<<<<<<'%.*s'[%d]:\r\n%.*s", method_tok.len, method_tok.ptr, len, len, payload);

        uint32_t idx = sizeof(method_list) / sizeof(s_method_list_t);

        while (idx--)
        {
            uint32_t comp_len = strlen(method_list[idx].method_name) > method_tok.len ? strlen(method_list[idx].method_name) : method_tok.len;

            if (0 == strncmp(method_tok.ptr, method_list[idx].method_name, comp_len))
            {
                if (NULL != method_list[idx].method)
                {
                    cjson_response = method_list[idx].method(payload, len, &method_tok, message_counter);
                    if (cjson_response)
                    {
                        web_provisioning_send_to_nma_websocket(cjson_response);
                        cJSON_Delete(cjson_response);
                        cjson_response = NULL;
                    }
                    else
                    {
                        TRACE_E("Error - cjson_response: %d", (uint32_t)cjson_response);
                    }
                }

                if (NULL != method_list[idx].updater)
                {
                    cjson_response = method_list[idx].updater(payload, len, &method_tok, message_counter);
                    if (cjson_response)
                    {
                        web_provisioning_send_to_nma_websocket(cjson_response);
                        cJSON_Delete(cjson_response);
                        cjson_response = NULL;
                    }
                    else
                    {
                        TRACE_E("Error - cjson_response: %d", (uint32_t)cjson_response);
                    }
                }

                rpc_method_found = 1;
                break;
            }
        }

        if (0 == rpc_method_found)
        {
            cjson_response = __rpc_method_notfound(payload, len, &method_tok, message_counter);
            if (cjson_response)
            {
                web_provisioning_send_to_nma_websocket(cjson_response);
                cJSON_Delete(cjson_response);
                cjson_response = NULL;
            }
            else
            {
                TRACE_E("Error - cjson_response: %d", (uint32_t)cjson_response);
            }
        }
    }
    else
    {
        TRACE_E("<< WS Rx '%.*s'[%d]:\r\n%.*s", method_tok.len, method_tok.ptr, len, len, payload);
    }
}

static cJSON *__rpc_method_notfound(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    cJSON *cjson_data = cJSON_CreateObject();

    if (cjson_data)
    {
        struct json_token msg_id = JSON_INVALID_TOKEN;
        struct json_token sender = JSON_INVALID_TOKEN;

        json_scanf(payload, len, "{id:%T}", &msg_id);
        int sender_status = json_scanf(payload, len, "{sender:%T}", &sender);

        cJSON_AddNumberToObject(cjson_data, "msg_id", msg_count);
        if (msg_id.len && msg_id.ptr && (JSON_TYPE_STRING == msg_id.type))
        {
            char tmp_bff[msg_id.len + 1];
            snprintf(tmp_bff, sizeof(tmp_bff), "%.*s", msg_id.len, msg_id.ptr);
            cJSON_AddStringToObject(cjson_data, "id", tmp_bff);
        }

        if (method && method->len && method->ptr && (JSON_TYPE_STRING == method->type))
        {
            char tmp_bff[method->len + 1];
            snprintf(tmp_bff, sizeof(tmp_bff), "%.*s", method->len, method->ptr);
            cJSON_AddStringToObject(cjson_data, "method", tmp_bff);
        }

        cJSON *cjson_error = cJSON_AddObjectToObject(cjson_data, "error");
        if (cjson_error)
        {
            cJSON_AddNumberToObject(cjson_error, "code", -32602);
            cJSON_AddStringToObject(cjson_error, "data", "rpc.method.notfound");
            cJSON_AddStringToObject(cjson_error, "message", "Unknown method");
        }

        cJSON_AddObjectToObject(cjson_data, "result");

        if (sender.len && sender.ptr && (JSON_TYPE_STRING == sender.type))
        {
            char tmp_bff[sender.len + 1];
            snprintf(tmp_bff, sizeof(tmp_bff + 1), "%.*s", sender.len, sender.ptr);
            cJSON_AddStringToObject(cjson_data, "sender", tmp_bff);
        }
        else
        {
            cJSON_AddObjectToObject(cjson_data, "sender");
        }

        char *str_cjson_data = cJSON_Parse(cjson_data);
        if (str_cjson_data)
        {
            TRACE_W("%s", str_cjson_data);
            free(str_cjson_data);
        }
    }

    return cjson_data;
}

static cJSON *__hub_reboot(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    esp_restart();
    return NULL;
}
