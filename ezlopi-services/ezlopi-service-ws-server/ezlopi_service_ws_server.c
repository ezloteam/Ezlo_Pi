/* WebSocket Echo Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "../../build/config/sdkconfig.h"

#if defined(CONFIG_ETH_USE_ESP32_EMAC)
#include "esp_eth.h"
#endif

#include "esp_wifi.h"
#include "esp_event.h"
#include "sys/param.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_http_server.h"

#include "cjext.h"
#include "ezlopi_util_trace.h"
#include "ezlopi_cloud_constants.h"

#include "ezlopi_core_api.h"
#include "ezlopi_core_wifi.h"
#include "ezlopi_core_buffer.h"
#include "ezlopi_core_broadcast.h"
#include "ezlopi_core_event_group.h"
#include "ezlopi_core_api_methods.h"
#include "ezlopi_core_cjson_macros.h"

#include "ezlopi_service_ws_server.h"
#include "ezlopi_service_ws_server_clients.h"

#include "EZLOPI_USER_CONFIG.h"

typedef struct s_async_resp_arg
{
    int fd;
    httpd_handle_t hd;
} s_async_resp_arg_t;

static uint32_t __message_counter = 0;
static httpd_handle_t __ws_handle = NULL;
static SemaphoreHandle_t __send_lock = NULL;
static volatile e_ws_status_t __ws_status = WS_STATUS_STOPPED;

///////// Static Functions Definations
static void __stop_server(void);
static void __start_server(void);
static void __wifi_connection_event(esp_event_base_t event, int32_t event_id, void *arg);

static void __ws_async_send(void *arg);
static esp_err_t __trigger_async_send(httpd_req_t *req);
static int __respond_cjson(httpd_req_t *req, cJSON *cj_response);
static int __ws_server_send(l_ws_server_client_conn_t *client, char *data, uint32_t len);

static esp_err_t __msg_handler(httpd_req_t *req);
static int __ws_server_broadcast(char *data);

e_ws_status_t ezlopi_service_ws_server_status(void)
{
    return __ws_status;
}

void ezlopi_service_ws_server_start(void)
{
    ezlopi_core_broadcast_method_add(__ws_server_broadcast, "wss-method", 2);

    if (ezlopi_wifi_got_ip())
    {
        if (WS_STATUS_STOPPED == __ws_status)
        {
            __start_server();
        }
    }

    if (NULL == __send_lock)
    {
        __send_lock = xSemaphoreCreateMutex();
        if (__send_lock)
        {
            xSemaphoreGive(__send_lock);
        }
    }

    ezlopi_wifi_event_add(__wifi_connection_event, NULL);
    if (ezlopi_wifi_got_ip())
    {
        if (WS_STATUS_STOPPED == __ws_status)
        {
            __start_server();
        }
    }
}

void ezlopi_service_ws_server_stop(void)
{
    if (__send_lock)
    {
        if (pdTRUE == xSemaphoreTake(__send_lock, portMAX_DELAY))
        {
            vSemaphoreDelete(__send_lock);
            __send_lock = NULL;

            __stop_server();
        }
    }
}

///////// Static Functions Definations
static int __ws_server_broadcast(char *data)
{
    int ret = 0;
    if (__send_lock && pdTRUE == xSemaphoreTake(__send_lock, 0))
    {
        if (data)
        {
            ret = 1;
            l_ws_server_client_conn_t* curr_client = ezlopi_service_ws_server_clients_get_head();
            #warning "DO NOT USE printf ON PRODUCTION"
            // printf("%s and curr-client: %p\n", __func__, curr_client);

            while (curr_client)
            {
                ret = __ws_server_send(curr_client, data, strlen(data));
                if (NULL == (curr_client = curr_client->next))
                {
                    break;
                }

                vTaskDelay(1 / portTICK_RATE_MS);
            }
        }

        xSemaphoreGive(__send_lock);
    }

    return ret;
}

static void __message_upcall(httpd_req_t *req, const char *payload, uint32_t payload_len)
{
    cJSON *cj_response = ezlopi_core_api_consume(__FUNCTION__, payload, payload_len);
    if (cj_response)
    {
        cJSON_AddNumberToObject(__FUNCTION__, cj_response, ezlopi_msg_id_str, __message_counter);
        __respond_cjson(req, cj_response);
        cJSON_Delete(__FUNCTION__, cj_response);
    }
}

static void __ws_async_send(void *arg)
{
#if 1 // def CONFIG_EZPI_LOCAL_WEBSOCKET_SERVER
    static const char *data = "Async data";
    s_async_resp_arg_t *resp_arg = (s_async_resp_arg_t *)arg;

    if (resp_arg)
    {
        if (__send_lock && pdTRUE == xSemaphoreTake(__send_lock, 5000 / portTICK_RATE_MS))
        {
            httpd_ws_frame_t ws_pkt;
            memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));

            ws_pkt.len = strlen(data);
            ws_pkt.payload = (uint8_t *)data;
            ws_pkt.type = HTTPD_WS_TYPE_TEXT;

            httpd_ws_send_frame_async(resp_arg->hd, resp_arg->fd, &ws_pkt);
        }

        ezlopi_free(__FUNCTION__, resp_arg);
    }
#endif // CONFIG_EZPI_LOCAL_WEBSOCKET_SERVER
}

static esp_err_t __trigger_async_send(httpd_req_t *req)
{
    esp_err_t ret = ESP_OK;
    s_async_resp_arg_t *resp_arg = ezlopi_malloc(__FUNCTION__, sizeof(s_async_resp_arg_t));

    if (resp_arg)
    {
#warning "resp_arg needs to find out wether 'resp_arg' is freed or not";

        resp_arg->hd = req->handle;
        resp_arg->fd = httpd_req_to_sockfd(req);
        ret = httpd_queue_work(req->handle, __ws_async_send, resp_arg);
    }

    return ret;
}

static esp_err_t __msg_handler(httpd_req_t *req)
{
    esp_err_t ret = ESP_FAIL;
#if 1 // def CONFIG_EZPI_LOCAL_WEBSOCKET_SERVER

    if (__send_lock && (pdTRUE == xSemaphoreTake(__send_lock, 5000 / portTICK_RATE_MS)))
    {
        TRACE_S("WSL: -----------------------------> acquired send-lock");

        if (req->method == HTTP_GET)
        {
            TRACE_I("Handshake done, the new connection was opened, id: %p", req);
            ezlopi_service_ws_server_clients_add((void *)req->handle, httpd_req_to_sockfd(req));
            ret = ESP_OK;
        }
        else
        {
            uint8_t *buf = NULL;
            httpd_ws_frame_t ws_pkt;

            memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
            ws_pkt.type = HTTPD_WS_TYPE_TEXT;

            ret = httpd_ws_recv_frame(req, &ws_pkt, 0); // to get only rx-data length

            TRACE_D("Packet type: %d", ws_pkt.type);
            TRACE_I("frame len is %d", ws_pkt.len);

            if (ESP_OK == ret)
            {
                if (HTTPD_WS_TYPE_CLOSE == ws_pkt.type)
                {
                    TRACE_D("closing connection!");
                    ezlopi_service_ws_server_clients_remove_by_handle(req->handle);
                }
                else if (0 < ws_pkt.len)
                {
                    buf = ezlopi_malloc(__FUNCTION__, ws_pkt.len + 1);

                    if (NULL != buf)
                    {
                        ws_pkt.payload = buf;
                        ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);

                        if (ESP_OK == ret)
                        {
                            TRACE_D("Packet type: %d", ws_pkt.type);

                            if ((HTTPD_WS_TYPE_TEXT == ws_pkt.type) && (0 == strcmp((char *)ws_pkt.payload, "Trigger async")))
                            {
                                TRACE_E("ASYNC");
                                ret = __trigger_async_send(req);
                            }
                            else if (HTTPD_WS_TYPE_TEXT == ws_pkt.type)
                            {
                                __message_upcall(req, (char *)ws_pkt.payload, (uint32_t)ws_pkt.len);
                            }
                            else if (HTTPD_WS_TYPE_CLOSE == ws_pkt.type)
                            {
                                TRACE_D("closing connection!");
                                ezlopi_service_ws_server_clients_remove_by_handle(req->handle);
                            }
                            else
                            {
                                TRACE_W("WSL: packet type un-handled!");
                            }
                        }
                        else
                        {
                            TRACE_E("WSL: httpd_ws_recv_frame failed with %d", ret);
                        }

                        ezlopi_free(__FUNCTION__, buf);
                    }
                    else
                    {
                        TRACE_E("WSL: malloc failed!");
                        ret = ESP_ERR_NO_MEM;
                    }
                }
                else
                {
                    TRACE_E("WSL: httpd_ws_recv_frame failed to get frame len with %d", ret);
                }
            }
        }

        if (pdTRUE == xSemaphoreGive(__send_lock))
        {
            TRACE_S("WSL: -----------------------------> released send-lock");
        }
        else
        {
            TRACE_E("WSL: -----------------------------> release send-lock failed!");
        }
    }
    else
    {
        TRACE_E("WSL: -----------------------------> acquire send-lock failed!");
    }
#endif // CONFIG_EZPI_LOCAL_WEBSOCKET_SERVER
    return ret;
}

static void __start_server(void)
{
#if 1 // def CONFIG_EZPI_LOCAL_WEBSOCKET_SERVER
    __ws_status = WS_STATUS_STARTED;

    static const httpd_uri_t ws = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = __msg_handler,
        .user_ctx = NULL,
        .is_websocket = true,
        .handle_ws_control_frames = true,
    };

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    config.task_priority = 8;
    config.server_port = 17001;
    config.stack_size = 1024 * 4;

    TRACE_I("Starting ws-server on port: '%d'", config.server_port);

    esp_err_t err = httpd_start(&__ws_handle, &config);

    if (ESP_OK == err)
    {
        TRACE_I("WSL: Registering URI handlers");
        if (ESP_OK == httpd_register_uri_handler(__ws_handle, &ws))
        {
            __ws_status = WS_STATUS_RUNNING;
        }
    }
    else
    {
        TRACE_E("WSL: Error starting server!, err: %d", err);
    }
#endif // CONFIG_EZPI_LOCAL_WEBSOCKET_SERVER
}

static void __stop_server(void)
{
    if (__ws_handle)
    {
        TRACE_E("stopping ws-server!");
        httpd_stop(__ws_handle);
        __ws_handle = NULL;
        __ws_status = WS_STATUS_STOPPED;
    }
}

static int __respond_cjson(httpd_req_t *req, cJSON *cj_response)
{
    int ret = 0;
#ifdef CONFIG_EZPI_LOCAL_WEBSOCKET_SERVER
    if (req && cj_response)
    {
        uint32_t buffer_len = 0;
        char *data_buffer = ezlopi_core_buffer_acquire(__FUNCTION__, &buffer_len, 5000);

        if (data_buffer && buffer_len)
        {
            memset(data_buffer, 0, buffer_len);

            if (cJSON_PrintPreallocated(__FUNCTION__, cj_response, data_buffer, buffer_len, false))
            {
                httpd_ws_frame_t data_frame = {
                    .final = false,
                    .fragmented = false,
                    .len = strlen(data_buffer),
                    .payload = (uint8_t *)data_buffer,
                    .type = HTTPD_WS_TYPE_TEXT,
                };

                ret = (ESP_OK == httpd_ws_send_frame(req, &data_frame)) ? true : false;

                if (ret)
                {
                    __message_counter++;
                    TRACE_S("## WSL:WSS-SENDING >>>>>>>>>> \n%s", data_buffer);
                }
                else
                {
                    TRACE_E("## WSL:WSS-SENDING >>>>>>>>>> \n%s", data_buffer);
                }
            }

            ezlopi_core_buffer_release(__FUNCTION__);
        }
    }
#endif // CONFIG_EZPI_LOCAL_WEBSOCKET_SERVER
    return ret;
}

static int __ws_server_send(l_ws_server_client_conn_t *client, char *data, uint32_t len)
{
    int ret = 0;
#ifdef CONFIG_EZPI_LOCAL_WEBSOCKET_SERVER
    if (data && len && client && client->http_handle)
    {
        httpd_ws_frame_t frm_pkt;
        memset(&frm_pkt, 0, sizeof(httpd_ws_frame_t));

        frm_pkt.len = strlen(data);
        frm_pkt.payload = (uint8_t *)data;
        frm_pkt.type = HTTPD_WS_TYPE_TEXT;

        // TRACE_D("client-handle: %p", client->http_handle);
        // TRACE_D("client-desc: %d", client->http_descriptor);
        // TRACE_D("data[%d]: %.*s", frm_pkt.len, frm_pkt.len, frm_pkt.payload);

        if (ESP_OK == httpd_ws_send_data(client->http_handle, client->http_descriptor, &frm_pkt))
        {
            ret = 1;
            client->fail_count = 0;
            __message_counter++;

            TRACE_S("## LOCAL WSS-SENDING done >>>>>>>>>>>>>>>>>>> \n%s", data);
        }
        else
        {
            TRACE_E("## LOCAL WSS-SENDING failed >>>>>>>>>>>>>>>>>>> \n%s", data);

            ret = 0;
            client->fail_count += 1;

            if (client->fail_count > 5)
            {
                TRACE_E("fail count reached maximum!");
                ezlopi_service_ws_server_clients_remove_by_handle(client->http_handle);
            }
        }
    }
#endif // CONFIG_EZPI_LOCAL_WEBSOCKET_SERVER
    return ret;
}

static void __wifi_connection_event(esp_event_base_t event_base, int32_t event_id, void *arg)
{
    TRACE_D("event-base: %d, event-id: %d", (uint32_t)event_base, event_id);

    if (IP_EVENT == event_base)
    {
        if (IP_EVENT_STA_GOT_IP == event_id)
        {
            if (WS_STATUS_STOPPED == __ws_status)
            {
                __start_server();
            }
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

void ezlpi_service_ws_server_dummy(void)
{
    TRACE_D("I'm dummy. I do nothing.");
}

///////// Global Functions Definations
