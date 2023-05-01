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
#include "firmware.h"
#include "ezlopi_websocket_client.h"

#include "ezlopi_wifi.h"
#include "ezlopi_http.h"
#include "ezlopi_factory_info.h"
#include "ezlopi_cloud_constants.h"

#include "web_provisioning.h"

static uint32_t message_counter = 0;

static void __connection_upcall(bool connected);
static void __message_upcall(const char *payload, uint32_t len);
static void __rpc_method_notfound(cJSON *cj_request, cJSON *cj_response);
static void __hub_reboot(cJSON *cj_request, cJSON *cj_response);
static void web_provisioning_fetch_wss_endpoint(void *pv);

#if 0
typedef struct s_method_list
{
    char *method_name;
    cJSON *(*method)(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
    cJSON *(*updater)(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
} s_method_list_t;

static const s_method_list_t method_list[] = {
    /** Getter functions **/
    {.method_name = "hub.data.list", .method = data_list, .updater = NULL},
    {.method_name = "hub.room.list", .method = room_list, .updater = NULL},
    {.method_name = "hub.items.list", .method = items_list, .updater = NULL},
    // {.method_name = "hub.scenes.list", .method = scenes_list, .updater = NULL},
    // {.method_name = "hub.devices.list", .method = devices_list, .updater = NULL},
    // {.method_name = "hub.favorite.list", .method = favorite_list, .updater = NULL},
    // {.method_name = "hub.gateways.list", .method = gateways_list, .updater = NULL},
    // {.method_name = "hub.info.get", .method = info_get, .updater = NULL},
    // {.method_name = "hub.modes.get", .method = modes_get, .updater = NULL},
    // {.method_name = "hub.network.get", .method = network_get, .updater = NULL}, //, .updater = NULL},
    // // {.method_name = "hub.settings.list", .method = settings_list, .updater = NULL},
    // // {.method_name = "hub.device.settings.list", .method = devices_settings_list, .updater = NULL},
    // {.method_name = "hub.reboot", .method = __hub_reboot, .updater = NULL},

    // // // /** Setter functions **/
    // {.method_name = "hub.item.value.set", .method = items_set_value, .updater = items_update},
    // // {.method_name = "hub.device.name.set", .method = devices_name_set, .updater = NULL},
    // // {.method_name = "hub.device.setting.value.set", .method = __rpc_method_notfound, .updater = NULL},
    // {.method_name = "registered", .method = registered, .updater = NULL}, // called only once so its in last

    // // {.method_name = "hub.feature.status.set", .method = __rpc_method_notfound, .updater = NULL}, // documentation missing
    // // {.method_name = "hub.features.list", .method = __rpc_method_notfound, .updater = NULL}, // documentation missing
};
#endif

typedef void (*f_method_func_t)(cJSON *cj_request, cJSON *cj_response);
typedef struct s_method_list_v2
{
    char *method_name;
    f_method_func_t method;
    f_method_func_t updater;
} s_method_list_v2_t;

static const s_method_list_v2_t method_list_v2[] = {
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
    {.method_name = "hub.network.get", .method = network_get, .updater = NULL},
    {.method_name = "hub.firmware.update.start", .method = firmware_update_start, .updater = NULL},
    {.method_name = "cloud.firmware.info.get", .method = firmware_info_get, .updater = NULL},
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

int web_provisioning_send_to_nma_websocket(cJSON *cjson_data, e_trace_type_t print_type)
{
    int ret = 0;
    if (ezlopi_websocket_client_is_connected())
    {
        if (cjson_data)
        {
            char *cjson_str_data = cJSON_Print(cjson_data);
            if (cjson_str_data)
            {
                cJSON_Minify(cjson_str_data);
                switch (print_type)
                {
                case TRACE_TYPE_B:
                {
                    TRACE_B("## WSS-SENDING >>>>>>>>>>\r\n%s", cjson_str_data);
                    break;
                }
                case TRACE_TYPE_E:
                {
                    TRACE_E("## WSS-SENDING >>>>>>>>>>\r\n%s", cjson_str_data);
                    break;
                }

                case TRACE_TYPE_I:
                {
                    TRACE_I("## WSS-SENDING >>>>>>>>>>\r\n%s", cjson_str_data);
                    break;
                }
                default:
                    break;
                }

                int ret = ezlopi_websocket_client_send(cjson_str_data, strlen(cjson_str_data));
                if (ret > 0)
                {
                    message_counter++;
                }

                free(cjson_str_data);
            }
        }
    }
    return ret;
}

void web_provisioning_init(void)
{
    xTaskCreate(web_provisioning_fetch_wss_endpoint, "web-provisioning fetch wss endpoint", 3 * 2048, NULL, 5, NULL);
}

static char *cloud_server = NULL;
static char *ca_certificate = NULL;
static char *ssl_shared_key = NULL;
static char *ssl_private_key = NULL;
static void web_provisioning_fetch_wss_endpoint(void *pv)
{
    char *ws_endpoint = NULL;

    while (1)
    {
        UBaseType_t water_mark = uxTaskGetStackHighWaterMark(NULL);
        TRACE_D("water_mark: %d", water_mark);

        ezlopi_wait_for_wifi_to_connect();

        cloud_server = ezlopi_factory_info_v2_get_cloud_server();
        ca_certificate = ezlopi_factory_info_v2_get_ca_certificate();
        ssl_shared_key = ezlopi_factory_info_v2_get_ssl_shared_key();
        ssl_private_key = ezlopi_factory_info_v2_get_ssl_private_key();

        char http_request[128];
        snprintf(http_request, sizeof(http_request), "%s/getserver?json=true", cloud_server);
        ws_endpoint = ezlopi_http_get_request(http_request, ssl_private_key, ssl_shared_key, ca_certificate);

        if (ws_endpoint)
        {
            TRACE_D("ws_endpoint: %s", ws_endpoint); // {"uri": "wss://endpoint:port"}
            TRACE_D("http_request: %s", http_request); 
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

            free(ws_endpoint);
        }

        vTaskDelay(2000 / portTICK_RATE_MS);
    }

#if 0
    while (1)
    {
        if (-1 != ezlopi_event_group_wait_for_event(EZLOPI_EVENT_OTA, 30 * 1000, 1))
        {
            TRACE_D("Sending firmware check request...");
            cJSON *firmware_info_request = firmware_send_firmware_query_to_nma_server(message_counter);
            if (NULL != firmware_info_request)
            {
                web_provisioning_send_to_nma_websocket(firmware_info_request, TRACE_TYPE_B);
                cJSON_Delete(firmware_info_request);
                firmware_info_request = NULL;
            }
        }

        vTaskDelay(30 * 1000 / portTICK_RATE_MS);
    }
#endif

    vTaskDelete(NULL);
}

static void __connection_upcall(bool connected)
{
    static bool prev_status;
    if (connected)
    {
        if (prev_status != connected)
        {
            TRACE_I("Web-socket re-connected.");
            TRACE_B("Starting registration process....");
            registration_init();
        }
    }
    else
    {
        TRACE_E("Web-socket dis-connected!");
    }

    prev_status = connected;
}

static uint32_t __search_method_in_list(cJSON *method)
{
    uint32_t found_method = 0;
    uint32_t idx = sizeof(method_list_v2) / sizeof(s_method_list_v2_t);

    while (idx--)
    {
        uint32_t request_method_name_len = strlen(method->valuestring);
        uint32_t list_method_name_len = strlen(method_list_v2[idx].method_name);
        uint32_t comp_len = list_method_name_len > request_method_name_len ? list_method_name_len : request_method_name_len;
        if (0 == strncmp(method->valuestring, method_list_v2[idx].method_name, comp_len))
        {
            found_method = 1;
            break;
        }
    }

    return (found_method ? idx : UINT32_MAX);
}

static void __call_method_func_and_send_response(cJSON *cj_request, cJSON *cj_method, f_method_func_t method_func, e_trace_type_t print_type)
{
    if (method_func)
    {
        if (registered == method_func)
        {
            method_func(cj_request, NULL);
        }
        else
        {
            cJSON *cj_response = cJSON_CreateObject();
            if (NULL != cj_response)
            {
                cJSON *cj_sender = cJSON_GetObjectItem(cj_request, ezlopi_sender_str);

                cJSON_AddNumberToObject(cj_response, ezlopi_msg_id_str, message_counter);
                cJSON_AddItemReferenceToObject(cj_response, ezlopi_sender_str, cj_sender);
                cJSON_AddNullToObject(cj_response, "error");

                method_func(cj_request, cj_response);

                web_provisioning_send_to_nma_websocket(cj_response, print_type);
                cJSON_Delete(cj_response);
            }
            else
            {
                TRACE_E("Error - cj_response: %d", (uint32_t)cj_response);
            }
        }
    }
}

static void __message_upcall(const char *payload, uint32_t len)
{
    // if (payload && len)
    // {
    //     TRACE_D("payload:: len: %d, data: %.*s", len, len, payload);
    // }

    cJSON *cj_request = cJSON_ParseWithLength(payload, len);

    if (cj_request)
    {
        cJSON *cj_error = cJSON_GetObjectItem(cj_request, "error");
        cJSON *cj_method = cJSON_GetObjectItem(cj_request, "method");

        if ((NULL == cj_error) || (NULL == cj_error->valuestring) || (0 == strncmp(cj_error->valuestring, "null", 4)))
        {
            if ((NULL != cj_method) && (NULL != cj_method->valuestring))
            {
                TRACE_D("## WS Rx <<<<<<<<<< '%s'\r\n%.*s", (cj_method->valuestring ? cj_method->valuestring : ""), len, payload);
                uint32_t method_idx = __search_method_in_list(cj_method);
                if (UINT32_MAX != method_idx)
                {
                    __call_method_func_and_send_response(cj_request, cj_method, method_list_v2[method_idx].method, TRACE_TYPE_B);
                    __call_method_func_and_send_response(cj_request, cj_method, method_list_v2[method_idx].updater, TRACE_TYPE_B);
                }
                else
                {
                    __call_method_func_and_send_response(cj_request, cj_method, __rpc_method_notfound, TRACE_TYPE_E);
                }
            }
        }
        else
        {
            TRACE_E("## WS Rx <<<<<<<<<<'%s'\r\n%.*s", (cj_method->valuestring ? cj_method->valuestring : ""), len, payload);
        }

        cJSON_Delete(cj_request);
    }
}

static void __rpc_method_notfound(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON *cjson_error = cJSON_AddObjectToObject(cj_response, "error");
    if (cjson_error)
    {
        cJSON_AddNumberToObject(cjson_error, "code", -32602);
        cJSON_AddStringToObject(cjson_error, "data", "rpc.method.notfound");
        cJSON_AddStringToObject(cjson_error, "message", "Unknown method");
    }

    cJSON_AddObjectToObject(cj_response, ezlopi_result);
}

static void __hub_reboot(cJSON *cj_request, cJSON *cj_response)
{
    esp_restart();
    return NULL;
}
