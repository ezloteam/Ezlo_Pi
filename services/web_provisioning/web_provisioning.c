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
#include "settings.h"
#include "scenes_scripts.h"
#include "scenes_expressions.h"

#include "ezlopi_wifi.h"
#include "ezlopi_http.h"
#include "ezlopi_factory_info.h"
#include "ezlopi_cloud_constants.h"

#include "web_provisioning.h"
#include "ezlopi_websocket_client.h"

static uint32_t message_counter = 0;

static char *cloud_server = NULL;
static char *ca_certificate = NULL;
static char *ssl_shared_key = NULL;
static char *ssl_private_key = NULL;

static void __connection_upcall(bool connected);
static void __message_upcall(const char *payload, uint32_t len);
static void __rpc_method_notfound(cJSON *cj_request, cJSON *cj_response);
static void __hub_reboot(cJSON *cj_request, cJSON *cj_response);
static void __fetch_wss_endpoint(void *pv);
static void __print_sending_data(char *data_str, e_trace_type_t print_type);

typedef void (*f_method_func_t)(cJSON *cj_request, cJSON *cj_response);
typedef struct s_method_list_v2
{
    char *method_name;
    f_method_func_t method;
    f_method_func_t updater;
} s_method_list_v2_t;

static const s_method_list_v2_t method_list_v2[] = {
#define CLOUD_METHOD(name, func, updater_func) {.method_name = name, .method = func, .updater = updater_func},
#include "web_provisioning_macro.h"
#undef CLOUD_METHOD
    {.method_name = NULL, .method = NULL, .updater = NULL},
};

uint32_t web_provisioning_get_message_count(void)
{
    return message_counter;
}

int web_provisioning_send_str_data_to_nma_websocket(char *str_data, e_trace_type_t print_type)
{
    int ret = 0;
    if (str_data)
    {
        int retries = 3;
        while (--retries)
        {
            if (ezlopi_websocket_client_send(str_data, strlen(str_data)) > 0)
            {
                ret = 1;
                message_counter++;
                break;
            }
        }

        if (ret)
        {
            __print_sending_data(str_data, print_type);
        }
        else
        {
            __print_sending_data(str_data, TRACE_TYPE_W);
        }
    }

    return ret;
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

                int retries = 3;
                while (--retries)
                {
                    ret = ezlopi_websocket_client_send(cjson_str_data, strlen(cjson_str_data));
                    if (ret > 0)
                    {
                        ret = 1;
                        message_counter++;
                        break;
                    }
                }

                if (ret)
                {
                    __print_sending_data(cjson_str_data, print_type);
                }
                else
                {
                    __print_sending_data(cjson_data, TRACE_TYPE_W);
                }

                free(cjson_str_data);
            }
        }
    }

    return ret;
}

void web_provisioning_init(void)
{
    xTaskCreate(__fetch_wss_endpoint, "web-provisioning fetch wss endpoint", 3 * 2048, NULL, 5, NULL);
}

static void __fetch_wss_endpoint(void *pv)
{
    s_ezlopi_http_data_t *ws_endpoint = NULL;

    while (1)
    {
        ezlopi_wait_for_wifi_to_connect(UINT32_MAX);

        cloud_server = ezlopi_factory_info_v2_get_cloud_server();
        ca_certificate = ezlopi_factory_info_v2_get_ca_certificate();
        ssl_shared_key = ezlopi_factory_info_v2_get_ssl_shared_key();
        ssl_private_key = ezlopi_factory_info_v2_get_ssl_private_key();

        char http_request[128];
        snprintf(http_request, sizeof(http_request), "%s/getserver?json=true", cloud_server);
        TRACE_D("http_request: %s", http_request);
        ws_endpoint = ezlopi_http_get_request(http_request, ssl_private_key, ssl_shared_key, ca_certificate);

        if (ws_endpoint)
        {
            if (ws_endpoint->response)
            {
                TRACE_D("ws_endpoint: %s", ws_endpoint->response); // {"uri": "wss://endpoint:port"}
                cJSON *root = cJSON_Parse(ws_endpoint->response);
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

                free(ws_endpoint->response);
            }

            free(ws_endpoint);
        }

        vTaskDelay(2000 / portTICK_RATE_MS);
    }

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
    uint32_t idx = 0;

    while (method_list_v2[idx].method_name)
    {
        uint32_t request_method_name_len = strlen(method->valuestring);
        uint32_t list_method_name_len = strlen(method_list_v2[idx].method_name);
        uint32_t comp_len = list_method_name_len > request_method_name_len ? list_method_name_len : request_method_name_len;
        if (0 == strncmp(method->valuestring, method_list_v2[idx].method_name, comp_len))
        {
            found_method = 1;
            break;
        }
        idx++;
    }

    return (found_method ? idx : UINT32_MAX);
}

static void __call_method_and_send_response(cJSON *cj_request, cJSON *cj_method, f_method_func_t method_func, e_trace_type_t print_type)
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
                cJSON_AddNullToObject(cj_response, ezlopi_error_str);

                method_func(cj_request, cj_response);

                char *data_to_send = cJSON_Print(cj_response);
                cJSON_Delete(cj_response);

                if (data_to_send)
                {
                    cJSON_Minify(data_to_send);
                    web_provisioning_send_str_data_to_nma_websocket(data_to_send, print_type);
                    free(data_to_send);
                }
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
    cJSON *cj_request = cJSON_ParseWithLength(payload, len);

    if (cj_request)
    {
        cJSON *cj_error = cJSON_GetObjectItem(cj_request, ezlopi_error_str);
        cJSON *cj_method = cJSON_GetObjectItem(cj_request, ezlopi_key_method_str);

        if ((NULL == cj_error) || (cJSON_NULL == cj_error->type) || (NULL != cj_error->valuestring) || ((NULL != cj_error->valuestring) && (0 == strncmp(cj_error->valuestring, ezlopi_null_str, 4))))
        {
            if ((NULL != cj_method) && (NULL != cj_method->valuestring))
            {
                TRACE_I("## WS Rx <<<<<<<<<< '%s'\r\n%.*s", (cj_method->valuestring ? cj_method->valuestring : ezlopi__str), len, payload);

                uint32_t method_idx = __search_method_in_list(cj_method);

                if (UINT32_MAX != method_idx)
                {
                    TRACE_D("Method[%d]: %s", method_idx, method_list_v2[method_idx].method_name);
                    __call_method_and_send_response(cj_request, cj_method, method_list_v2[method_idx].method, TRACE_TYPE_D);
                    __call_method_and_send_response(cj_request, cj_method, method_list_v2[method_idx].updater, TRACE_TYPE_D);
                }
                else
                {
                    __call_method_and_send_response(cj_request, cj_method, __rpc_method_notfound, TRACE_TYPE_E);
                }
            }
        }
        else
        {
            TRACE_E("## WS Rx <<<<<<<<<< '%s'\r\n%.*s", (NULL != cj_method) ? (cj_method->valuestring ? cj_method->valuestring : ezlopi__str) : ezlopi__str, len, payload);
            TRACE_E("cj_error: %p, cj_error->type: %u, cj_error->value_string: %s", cj_error, cj_error->type, cj_error ? (cj_error->valuestring ? cj_error->valuestring : ezlopi_null_str) : ezlopi_null_str);
        }

        cJSON_Delete(cj_request);
    }
}

static void __rpc_method_notfound(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON *cjson_error = cJSON_AddObjectToObject(cj_response, ezlopi_error_str);
    if (cjson_error)
    {
        cJSON_AddNumberToObject(cjson_error, ezlopi_code_str, -32602);
        cJSON_AddStringToObject(cjson_error, ezlopi_data_str, ezlopi_rpc_method_notfound_str);
        cJSON_AddStringToObject(cjson_error, ezlopi_message_str, ezlopi_Unknown_method_str);
    }

    cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
}

static void __hub_reboot(cJSON *cj_request, cJSON *cj_response)
{
    esp_restart();
}

static void __print_sending_data(char *data_str, e_trace_type_t print_type)
{
    switch (print_type)
    {
    case TRACE_TYPE_W:
    {
        TRACE_W("## WSS-SENDING >>>>>>>>>>>>>>>>>>>\r\n%s", data_str);
        break;
    }
    case TRACE_TYPE_D:
    {
        TRACE_D("## WSS-SENDING >>>>>>>>>>>>>>>>>>>\r\n%s", data_str);
        break;
    }
    case TRACE_TYPE_E:
    {
        TRACE_E("## WSS-SENDING  >>>>>>>>>>>>>>>>>>>\r\n%s", data_str);
        break;
    }
    case TRACE_TYPE_I:
    {
        TRACE_I("## WSS-SENDING >>>>>>>>>>\r\n%s", data_str);
        break;
    }
    default:
    {
        TRACE_E("## WSS-SENDING >>>>>>>>>>\r\n%s", data_str);
        break;
    }
    }
}