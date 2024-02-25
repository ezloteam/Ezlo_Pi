/* WebSocket Echo Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <cJSON.h>
#include <esp_log.h>
#include <esp_eth.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <sys/param.h>
#include <esp_netif.h>
#include <esp_system.h>

#include "../../build/config/sdkconfig.h"
#include <esp_http_server.h>

#include "ezlopi_core_wifi.h"
#include "ezlopi_util_trace.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_service_ws_server.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_service_ws_server_clients.h"
#include "ezlopi_core_ezlopi_methods.h"

typedef enum e_trace_type
{
    TRACE_TYPE_NONE = 0,
    TRACE_TYPE_W, // Warning (Orange)
    TRACE_TYPE_B,
    TRACE_TYPE_D, // debug (White)
    TRACE_TYPE_I, // Info (Blue)
    TRACE_TYPE_E  // Error (Red)
} e_trace_type_t;

typedef struct s_async_resp_arg
{
    int fd;
    httpd_handle_t hd;
} s_async_resp_arg_t;

static uint32_t message_counter = 0;
static httpd_handle_t gs_ws_handle = NULL;
static e_ws_status_t gs_ws_status = WS_STATUS_STOPPED;

static void __stop_server(void);
static void __start_server(void);
static void __ws_async_send(void *arg);
static esp_err_t __msg_handler(httpd_req_t *req);
static esp_err_t __trigger_async_send(httpd_req_t *req);
static void __ws_api_handler(httpd_req_t *req, const char *payload, uint32_t payload_len);
static void __wifi_connection_event(esp_event_base_t event, int32_t event_id, void *arg);
static void __call_method_and_send_response(cJSON *cj_request, cJSON *cj_method, f_method_func_t method_func, e_trace_type_t print_type);
static void __print_sending_data(char *data_str, e_trace_type_t print_type);

e_ws_status_t ezlopi_service_ws_server_status(void)
{
    return gs_ws_status;
}

int ezlopi_service_ws_server_send_cjson(cJSON *cj_data)
{
    int ret = 0;

    if (cj_data)
    {
        cJSON *cj_srouce = cJSON_GetObjectItem(cj_data, ezlopi_source_str);
        if (cj_srouce)
        {
            uint32_t client_id = 0;

            CJSON_GET_VALUE_DOUBLE(cj_srouce, ezlopi_client_id_str, client_id)
            l_ws_server_client_conn_t *curr_client = ezlopi_service_ws_server_clients_get_by_handle((void *)client_id);
            if (curr_client)
            {
                char *data = cJSON_Print(cj_data);
                if (data)
                {
                    TRACE_D("sending: %s", data);
                    cJSON_Minify(data);
                    ret = ezlopi_service_ws_server_broadcast(data, strlen(data));

                    free(data);
                }
            }
            else
            {
                TRACE_E("NULL");
            }
        }
        else
        {
            char *data = cJSON_Print(cj_data);
            if (data)
            {
                cJSON_Minify(data);
                ret = ezlopi_service_ws_server_broadcast(data, strlen(data));
                free(data);
            }
        }
    }

    return ret;
}

int ezlopi_service_ws_server_broadcast(char *data, uint32_t len)
{
    int ret = 0;

    if (data && len)
    {
        l_ws_server_client_conn_t *curr_client = ezlopi_service_ws_server_clients_get_head();
        while (curr_client)
        {
            ret = 1;
            ezlopi_service_ws_server_send(curr_client, data, len);
            curr_client = curr_client->next;
        }
    }

    return ret;
}

int ezlopi_service_ws_server_send(l_ws_server_client_conn_t *client, char *data, uint32_t len)
{
    int ret = 0;
    if (data && len && client && client->http_handle)
    {
        httpd_ws_frame_t frm_pkt;

        memset(&frm_pkt, 0, sizeof(httpd_ws_frame_t));

        frm_pkt.len = len;
        frm_pkt.payload = (uint8_t *)data;
        frm_pkt.type = HTTPD_WS_TYPE_TEXT;

        TRACE_D("client-handle: %p", client->http_handle);
        TRACE_D("client-desc: %d", client->http_descriptor);
        TRACE_D("data: %.*s", frm_pkt.len, frm_pkt.payload);

        if (ESP_OK == httpd_ws_send_data(client->http_handle, client->http_descriptor, &frm_pkt))
        {
            ret = 1;
            client->fail_count = 0;

            TRACE_S("Done");
            __print_sending_data((char *)frm_pkt.payload, TRACE_TYPE_D);
        }
        else
        {
            client->fail_count += 1;

            TRACE_E("Failed!");
            __print_sending_data((char *)frm_pkt.payload, TRACE_TYPE_E);

            if (client->fail_count > 5)
            {
                // frm_pkt.len = 0;
                // frm_pkt.payload = "";
                // frm_pkt.type = HTTPD_WS_TYPE_CLOSE;
                // httpd_ws_send_data(client->http_handle, client->http_descriptor, &frm_pkt);
                ezlopi_service_ws_server_clients_remove_by_handle(client->http_handle);
            }
        }
    }

    return ret;
}

void ezlopi_service_ws_server_start(void)
{
    ezlopi_wifi_event_add(__wifi_connection_event, NULL);
}

void ezlopi_service_ws_server_stop(void)
{
    __stop_server();
}

static void __wifi_connection_event(esp_event_base_t event_base, int32_t event_id, void *arg)
{
    TRACE_D("event-base: %d, event-id: %d", (uint32_t)event_base, event_id);

    if (IP_EVENT == event_base)
    {
        if (IP_EVENT_STA_GOT_IP == event_id)
        {
            __start_server();
        }
        else
        {
            __stop_server();
        }
    }
    else if (WIFI_EVENT == event_base)
    {
        if (WIFI_EVENT_STA_DISCONNECTED == event_id)
        {
            __stop_server();
        }
    }
}

static void __ws_api_handler(httpd_req_t *req, const char *payload, uint32_t payload_len)
{
    TRACE_D("req: %p", req);

    if (payload && payload_len)
    {
        cJSON *cj_request = cJSON_ParseWithLength(payload, payload_len);
        if (cj_request)
        {
            cJSON *cj_src = cJSON_AddObjectToObject(cj_request, ezlopi_source_str);
            if (cj_src)
            {
                cJSON_AddNumberToObject(cj_src, ezlopi_type_str, 1);
                cJSON_AddNumberToObject(cj_src, ezlopi_client_id_str, (uint32_t)req->handle);
            }

            CJSON_TRACE("request", cj_request);

            cJSON *cj_error = cJSON_GetObjectItem(cj_request, ezlopi_error_str);
            cJSON *cj_method = cJSON_GetObjectItem(cj_request, ezlopi_method_str);

            if ((NULL == cj_error) || (cJSON_NULL == cj_error->type) || (NULL != cj_error->valuestring) ||
                ((NULL != cj_error->valuestring) && (0 == strncmp(cj_error->valuestring, ezlopi_null_str, 4))))
            {
                if ((NULL != cj_method) && (NULL != cj_method->valuestring))
                {
                    TRACE_S("## WS Rx <<<<<<<<<< '%s'\r\n%.*s", (cj_method->valuestring ? cj_method->valuestring : ezlopi__str), payload_len, payload);

                    uint32_t method_id = ezlopi_core_ezlopi_methods_search_in_list(cj_method);
                    TRACE_D("method_id: %d", method_id);

                    if (UINT32_MAX != method_id)
                    {
                        char *method_name = ezlopi_core_ezlopi_methods_get_name_by_id(method_id);
                        TRACE_D("Method:: id:[%d], name: %s", method_id, method_name ? method_name : "null");

                        f_method_func_t method = ezlopi_core_ezlopi_methods_get_by_id(method_id);
                        TRACE_D("method: %p", method);

                        if (method)
                        {
                            __call_method_and_send_response(cj_request, cj_method, method, TRACE_TYPE_D);
                        }

                        f_method_func_t updater = ezlopi_core_ezlopi_methods_get_updater_by_id(method_id);
                        TRACE_D("updater: %p", updater);

                        if (updater)
                        {
                            __call_method_and_send_response(cj_request, cj_method, updater, TRACE_TYPE_D);
                        }
                    }
                    else
                    {
                        __call_method_and_send_response(cj_request, cj_method, ezlopi_core_ezlopi_methods_rpc_method_notfound, TRACE_TYPE_E);
                    }
                }
            }
            else
            {
                TRACE_E("## WS Rx <<<<<<<<<< '%s'\r\n%.*s", (NULL != cj_method) ? (cj_method->valuestring ? cj_method->valuestring : ezlopi__str) : ezlopi__str, payload_len, payload);
                TRACE_E("cj_error: %p, cj_error->type: %u, cj_error->value_string: %s", cj_error, cj_error->type, cj_error ? (cj_error->valuestring ? cj_error->valuestring : ezlopi_null_str) : ezlopi_null_str);
            }

            cJSON_Delete(cj_request);
        }
        else
        {
            TRACE_W("Invalid json packet!");
        }
    }
}

static void __ws_async_send(void *arg)
{
    static const char *data = "Async data";
    s_async_resp_arg_t *resp_arg = (s_async_resp_arg_t *)arg;

    if (resp_arg)
    {
        httpd_ws_frame_t ws_pkt;
        memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));

        ws_pkt.len = strlen(data);
        ws_pkt.payload = (uint8_t *)data;
        ws_pkt.type = HTTPD_WS_TYPE_TEXT;

        httpd_ws_send_frame_async(resp_arg->hd, resp_arg->fd, &ws_pkt);
        free(resp_arg);
    }
}

static esp_err_t __trigger_async_send(httpd_req_t *req)
{
    esp_err_t ret = ESP_OK;
    s_async_resp_arg_t *resp_arg = malloc(sizeof(s_async_resp_arg_t));

    if (resp_arg)
    {
        resp_arg->hd = req->handle;
        resp_arg->fd = httpd_req_to_sockfd(req);
        ret = httpd_queue_work(req->handle, __ws_async_send, resp_arg);
    }

    return ret;
}

static esp_err_t __msg_handler(httpd_req_t *req)
{
    esp_err_t ret = ESP_OK;

    if (req->method == HTTP_GET)
    {
        TRACE_I("Handshake done, the new connection was opened, id: %p", req);
        ezlopi_service_ws_server_clients_add((void *)req->handle, httpd_req_to_sockfd(req));
    }
    else
    {
        uint8_t *buf = NULL;
        httpd_ws_frame_t ws_pkt;

        memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
        ws_pkt.type = HTTPD_WS_TYPE_TEXT;

        ret = httpd_ws_recv_frame(req, &ws_pkt, 0);

        if ((ESP_OK == ret) && (0 < ws_pkt.len))
        {
            TRACE_I("frame len is %d", ws_pkt.len);
            buf = malloc(ws_pkt.len + 1);

            if (NULL != buf)
            {
                ws_pkt.payload = buf;
                ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);

                if (ESP_OK == ret)
                {
                    TRACE_D("Packet type: %d", ws_pkt.type);
                    TRACE_D("Message: %.*s", ws_pkt.len, ws_pkt.payload);

                    if ((HTTPD_WS_TYPE_TEXT == ws_pkt.type) && (0 == strcmp((char *)ws_pkt.payload, "Trigger async")))
                    {
                        ret = __trigger_async_send(req);
                    }
                    else if (HTTPD_WS_TYPE_TEXT == ws_pkt.type)
                    {
                        __ws_api_handler(req, (char *)ws_pkt.payload, (uint32_t)ws_pkt.len);
                    }
                    else if (HTTPD_WS_TYPE_CLOSE == ws_pkt.type)
                    {
                        TRACE_D("closing connection!");
                        ezlopi_service_ws_server_clients_remove_by_handle(req);
                    }
                    else
                    {
                        TRACE_W("packet type un-handled!");
                    }
                }
                else
                {
                    TRACE_E("httpd_ws_recv_frame failed with %d", ret);
                }

                free(buf);
            }
            else
            {
                TRACE_E("malloc failed!");
                ret = ESP_ERR_NO_MEM;
            }
        }
        else
        {
            TRACE_E("httpd_ws_recv_frame failed to get frame len with %d", ret);
        }
    }

    return ret;
}

static void __start_server(void)
{
    static const httpd_uri_t ws = {
        .uri = "/ws",
        .method = HTTP_GET,
        .handler = __msg_handler,
        .user_ctx = NULL,
        .is_websocket = true,
        .handle_ws_control_frames = true,
    };

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.task_priority = 8;
    config.stack_size = 1024 * 8;

    TRACE_I("Starting ws-server on port: '%d'", config.server_port);

    esp_err_t err = httpd_start(&gs_ws_handle, &config);

    if (ESP_OK == err)
    {
        TRACE_I("Registering URI handlers");
        if (ESP_OK == httpd_register_uri_handler(gs_ws_handle, &ws))
        {
            gs_ws_status = WS_STATUS_RUNNING;
        }
    }
    else
    {
        TRACE_E("Error starting server!, err: %d", err);
    }
}

static void __stop_server(void)
{
    if (gs_ws_handle)
    {
        TRACE_E("stopping ws-server!");
        httpd_stop(gs_ws_handle);
        gs_ws_handle = NULL;
        gs_ws_status = WS_STATUS_STOPPED;
    }
}

static void __call_method_and_send_response(cJSON *cj_request, cJSON *cj_method, f_method_func_t method_func, e_trace_type_t print_type)
{
    if (method_func)
    {
        if (ezlopi_core_elzlopi_methods_check_method_register(method_func))
        {
            method_func(cj_request, NULL);
        }
        else
        {
            cJSON *cj_response = cJSON_CreateObject();
            if (NULL != cj_response)
            {
                cJSON_AddNumberToObject(cj_response, ezlopi_msg_id_str, message_counter);
                cJSON_AddItemReferenceToObject(cj_response, ezlopi_sender_str, cJSON_GetObjectItem(cj_request, ezlopi_sender_str));
                cJSON_AddNullToObject(cj_response, ezlopi_error_str);

                method_func(cj_request, cj_response);
                CJSON_TRACE("cj_response", cj_response);

                ezlopi_service_ws_server_send_cjson(cj_response);
                // char *data = cJSON_Print(cj_request);
                // if (data)
                // {
                //     cJSON_Minify(data);
                //     ezlopi_service_ws_server_send();
                //     free(data);
                // }
                cJSON_Delete(cj_response);
            }
            else
            {
                TRACE_E("Error - cj_response: %d", (uint32_t)cj_response);
            }
        }
    }
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
    case TRACE_TYPE_B:
    {
        TRACE_I("## WSS-SENDING >>>>>>>>>>>>>>>>>>>\r\n%s", data_str);
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
        TRACE_S("## WSS-SENDING >>>>>>>>>>\r\n%s", data_str);
        break;
    }
    default:
    {
        TRACE_E("## WSS-SENDING >>>>>>>>>>\r\n%s", data_str);
        break;
    }
    }
}
