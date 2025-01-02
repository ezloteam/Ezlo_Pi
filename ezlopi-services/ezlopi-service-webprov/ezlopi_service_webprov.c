

/**
 * @file    ezlopi_service_webprov.c
 * @brief
 * @author
 * @version
 * @date
 */
/* ===========================================================================
** Copyright (C) 2024 Ezlo Innovation Inc
**
** Under EZLO AVAILABLE SOURCE LICENSE (EASL) AGREEMENT
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/

#include "../../build/config/sdkconfig.h"
#include "EZLOPI_USER_CONFIG.h"

#include <esp_mac.h>
#include <esp_wifi_types.h>
#include <esp_idf_version.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <freertos/queue.h>

#include "ezlopi_util_trace.h"
#include "ezlopi_cloud_constants.h"

#include "ezlopi_core_api.h"

#include "ezlopi_core_sntp.h"
#include "ezlopi_core_http.h"
#include "ezlopi_core_wifi.h"
#include "ezlopi_core_reset.h"
#include "ezlopi_core_buffer.h"
#include "ezlopi_core_processes.h"
#include "ezlopi_core_broadcast.h"
#include "ezlopi_core_api_methods.h"
#include "ezlopi_core_event_group.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_websocket_client.h"

#include "ezlopi_service_otel.h"
#include "ezlopi_service_webprov.h"

#define TEST_PROV 0

#if (1 == TEST_PROV)
#include "ezlopi_test_prov.h"
#endif

#if defined(CONFIG_EZPI_WEBSOCKET_CLIENT)

typedef struct
{
    char *payload;
    time_t time_stamp;
    uint32_t tick_count;
} s_rx_message_t;

static uint32_t message_counter = 0;
static xTaskHandle _task_handle = NULL;
static QueueHandle_t _wss_message_queue = NULL;
static esp_websocket_client_handle_t wss_client = NULL;
static TaskHandle_t __web_socket_initialize_handler = NULL;

static void __provision_check(void *pv);
static int __provision_update(char *arg);
static void __fetch_wss_endpoint(void *pv);

static void __connection_upcall(bool connected);
static void __message_process_cjson(cJSON *cj_request, time_t time_stamp);
static int __message_upcall(char *payload, uint32_t len, time_t time_stamp);

static int __send_cjson_data_to_nma_websocket(cJSON *cj_data);
static ezlopi_error_t __send_str_data_to_nma_websocket(char *str_data);

bool EZPI_service_webprov_is_connected(void)
{
    return esp_websocket_client_is_connected(wss_client);
}

uint32_t EZPI_service_web_provisioning_get_message_count(void)
{
    return message_counter;
}

void EZPI_service_web_provisioning_init(void)
{
    TaskHandle_t ezlopi_service_web_prov_config_check_task_handle = NULL;
    xTaskCreate(__provision_check, "WebProvCfgChk", EZLOPI_SERVICE_WEB_PROV_CONFIG_CHECK_TASK_DEPTH, NULL, 4, &ezlopi_service_web_prov_config_check_task_handle);

#if defined(CONFIG_FREERTOS_USE_TRACE_FACILITY)
    EZPI_core_process_set_process_info(ENUM_EZLOPI_SERVICE_WEB_PROV_CONFIG_CHECK_TASK, &ezlopi_service_web_prov_config_check_task_handle, EZLOPI_SERVICE_WEB_PROV_CONFIG_CHECK_TASK_DEPTH);
#endif

    _wss_message_queue = xQueueCreate(10, sizeof(s_rx_message_t *));

    xTaskCreate(__fetch_wss_endpoint, "WebProvFetchWSS", EZLOPI_SERVICE_WEB_PROV_FETCH_WSS_TASK_DEPTH, NULL, 4, &__web_socket_initialize_handler);
#if defined(CONFIG_FREERTOS_USE_TRACE_FACILITY)
    EZPI_core_process_set_process_info(ENUM_EZLOPI_SERVICE_WEB_PROV_FETCH_WSS_TASK, &__web_socket_initialize_handler, EZLOPI_SERVICE_WEB_PROV_FETCH_WSS_TASK_DEPTH);
#endif
}

void EZPI_service_web_provisioning_deinit(void)
{
    if (_task_handle)
    {
        vTaskDelete(_task_handle);
    }

    EZPI_core_websocket_client_kill(wss_client);
}

static void __connection_upcall(bool connected)
{
    static int prev_status; // 0: never connected, 1: Not-connected, 2: connected
    if (connected)
    {
        if (0 == prev_status)
        {
            TRACE_OTEL(ENUM_EZLOPI_TRACE_SEVERITY_INFO, "NMA-server connected.");
            TRACE_S("Web-socket Connected.");
        }
        else
        {
            TRACE_OTEL(ENUM_EZLOPI_TRACE_SEVERITY_INFO, "NMA-server re-connected.");
            TRACE_S("Web-socket Re-connected.");
        }

        prev_status = 2;
        TRACE_I("Starting registration process....");
        EZPI_core_ezlopi_methods_registration_init();
    }
    else
    {
        prev_status = 1;
        EZPI_core_event_group_clear_event(EZLOPI_EVENT_NMA_REG);
        TRACE_OTEL(ENUM_EZLOPI_TRACE_SEVERITY_WARNING, "NMA-server dis-connected!");
    }
}

static void __fetch_wss_endpoint(void *pv)
{
    char *ca_certificate = EZPI_core_factory_info_v3_get_ca_certificate();
    char *ssl_shared_key = EZPI_core_factory_info_v3_get_ssl_shared_key();
    char *ssl_private_key = EZPI_core_factory_info_v3_get_ssl_private_key();
    char *cloud_server = EZPI_core_factory_info_v3_get_cloud_server();

    EZPI_core_wait_for_wifi_to_connect(portMAX_DELAY);
    vTaskDelay(2);

    while (1)
    {
        uint32_t task_complete = 0;

        if (ca_certificate && ssl_shared_key && ssl_private_key && cloud_server)
        {
            char http_request[128];
            snprintf(http_request, sizeof(http_request), "%s?json=true", cloud_server);
            TRACE_D("http_request: %s", http_request);

            s_ezlopi_http_data_t *ws_endpoint = EZPI_core_http_get_request(http_request, ssl_private_key, ssl_shared_key, ca_certificate);
            // s_ezlopi_http_data_t * ws_endpoint = EZPI_core_http_get_request(http_request, NULL, NULL, NULL);

            if (ws_endpoint)
            {
                if (ws_endpoint->response)
                {
                    TRACE_D("ws_endpoint: %s", ws_endpoint->response); // {"uri": "wss://endpoint:port"}

                    cJSON *cj_nma_uri = cJSON_Parse(__FUNCTION__, ws_endpoint->response);
                    if (cj_nma_uri)
                    {
                        cJSON *cjson_uri = cJSON_GetObjectItem(__FUNCTION__, cj_nma_uri, "uri");
                        if (cjson_uri)
                        {
                            TRACE_OTEL(ENUM_EZLOPI_TRACE_SEVERITY_INFO, "NMA-server: %s.", cjson_uri->valuestring ? cjson_uri->valuestring : ezlopi_null_str);

                            EZPI_core_broadcast_method_add(__send_str_data_to_nma_websocket, "nma-websocket", 4);
                            wss_client = EZPI_core_websocket_client_init(cjson_uri, __message_upcall, __connection_upcall,
                                                                         ca_certificate, ssl_private_key, ssl_shared_key);
                            task_complete = 1;
                        }

                        cJSON_Delete(__FUNCTION__, cj_nma_uri);
                    }

                    ezlopi_free(__FUNCTION__, ws_endpoint->response);
                }

                ezlopi_free(__FUNCTION__, ws_endpoint);
            }
        }

        if (task_complete)
        {
            while (_wss_message_queue)
            {
                s_rx_message_t *rx_message = NULL;
                xQueueReceive(_wss_message_queue, &rx_message, 100 / portTICK_RATE_MS);

                if (rx_message)
                {
                    char *id_str = NULL;
                    char *error_str = NULL;
                    char *method_str = NULL;

                    time_t time_stamp = rx_message->time_stamp;
                    uint32_t tick_count = rx_message->tick_count;

                    if (rx_message->payload)
                    {
                        cJSON *cj_request = cJSON_Parse(__FUNCTION__, rx_message->payload);
                        if (cj_request)
                        {
#ifdef CONFIG_EZPI_OPENTELEMETRY_ENABLE_TRACES
                            id_str = ezlopi_service_otel_fetch_string_value_from_cjson(cj_request, ezlopi_id_str);
                            error_str = ezlopi_service_otel_fetch_string_value_from_cjson(cj_request, ezlopi_error_str);
                            method_str = ezlopi_service_otel_fetch_string_value_from_cjson(cj_request, ezlopi_method_str);
#endif
#if 0
                            cJSON *cj_id = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_id_str);
                            if (cj_id && cj_id->valuestring && (cj_id->type == cJSON_String) && cj_id->str_value_len)
                            {
                                id_str = ezlopi_malloc(__FUNCTION__, cj_id->str_value_len + 1);
                                if (id_str)
                                {
                                    snprintf(id_str, cj_id->str_value_len + 1, "%.*s", cj_id->str_value_len, cj_id->valuestring);
                                }
                            }

                            cJSON *cj_error = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_error_str);
                            if (cj_error && cj_error->valuestring && (cj_error->type == cJSON_String) && cj_error->str_value_len)
                            {
                                error_str = ezlopi_malloc(__FUNCTION__, cj_error->str_value_len + 1);
                                if (error_str)
                                {
                                    snprintf(error_str, cj_error->str_value_len + 1, "%.*s", cj_error->str_value_len, cj_error->valuestring);
                                }
                            }

                            cJSON *cj_method = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_method_str);
                            if (cj_method)
                            {
                                method_str = ezlopi_malloc(__FUNCTION__, cj_method->str_value_len + 1);
                                if (method_str)
                                {
                                    snprintf(method_str, cj_method->str_value_len + 1, "%.*s", cj_method->str_value_len, cj_method->valuestring);
                                }

                                ezlopi_free(__FUNCTION__, rx_message->payload);
                                rx_message->payload = NULL;
                            }
#endif
                            if (NULL == method_str)
                            {
                                TRACE_OTEL(ENUM_EZLOPI_TRACE_SEVERITY_ERROR, "web-provisioning [method: null], payload: %s", rx_message->payload);
                            }

                            __message_process_cjson(cj_request, rx_message->time_stamp);
                            cJSON_Delete(__FUNCTION__, cj_request);
                        }

                        ezlopi_free(__FUNCTION__, rx_message->payload);
                        rx_message->payload = NULL;
                    }

                    ezlopi_free(__FUNCTION__, rx_message);
                    rx_message = NULL;

#ifdef CONFIG_EZPI_OPENTELEMETRY_ENABLE_TRACES
                    s_otel_trace_t *trace_obj = ezlopi_malloc(__FUNCTION__, sizeof(s_otel_trace_t));
                    if (trace_obj)
                    {
                        memset(trace_obj, 0, sizeof(s_otel_trace_t));

                        trace_obj->kind = E_OTEL_KIND_CLIENT;
                        trace_obj->start_time = time_stamp;
                        trace_obj->tick_count = tick_count;
                        trace_obj->end_time = EZPI_core_sntp_get_current_time_sec();

                        trace_obj->free_heap = esp_get_free_heap_size();
                        trace_obj->heap_watermark = esp_get_minimum_free_heap_size();

                        trace_obj->id = id_str;
                        trace_obj->error = error_str;
                        trace_obj->method = method_str;

                        id_str = NULL;
                        error_str = NULL;
                        method_str = NULL;

                        if (0 == ezlopi_service_otel_add_trace_to_telemetry_queue(trace_obj))
                        {
                            id_str = trace_obj->id;
                            error_str = trace_obj->error;
                            method_str = trace_obj->method;
                            ezlopi_free(__FUNCTION__, trace_obj);
                        }
                    }

                    ezlopi_free(__FUNCTION__, id_str);
                    ezlopi_free(__FUNCTION__, error_str);
                    ezlopi_free(__FUNCTION__, method_str);
#endif
                }

                vTaskDelay(1);
            }

            break;
        }

        vTaskDelay(2000 / portTICK_RATE_MS);
    }

    EZPI_core_factory_info_v3_free(cloud_server);

#if defined(CONFIG_FREERTOS_USE_TRACE_FACILITY)
    EZPI_core_process_set_is_deleted(ENUM_EZLOPI_SERVICE_WEB_PROV_FETCH_WSS_TASK);
#endif
    vTaskDelete(NULL);
}

static void __message_process_cjson(cJSON *cj_request, time_t time_stamp)
{
    if (cj_request)
    {
        cJSON *cj_response = EZPI_core_api_consume_cjson(__FUNCTION__, cj_request, time_stamp);

#ifdef CONFIG_EZPI_UTIL_TRACE_EN
        time_t now = EZPI_core_sntp_get_current_time_sec();
        TRACE_D("time to process: %lu", (now - time_stamp));
#endif

        if (cj_response)
        {
            cJSON_AddNumberToObject(__FUNCTION__, cj_response, ezlopi_msg_id_str, message_counter);
            __send_cjson_data_to_nma_websocket(cj_response);
            cJSON_Delete(__FUNCTION__, cj_response);

#ifdef CONFIG_EZPI_UTIL_TRACE_EN
            now = EZPI_core_sntp_get_current_time_sec();
            TRACE_D("time to reply: %lu", (now - time_stamp));
        }
        else
        {
            TRACE_W("no response!");
#endif
        }
    }
}

static int __message_upcall(char *payload, uint32_t len, time_t time_stamp)
{
    int ret = 0;

    if (_wss_message_queue)
    {
        s_rx_message_t *rx_message = ezlopi_malloc(__FUNCTION__, sizeof(s_rx_message_t));
        if (rx_message)
        {

            if (pdTRUE == xQueueIsQueueFullFromISR(_wss_message_queue))
            {
                s_rx_message_t *stale_data = NULL;
                xQueueReceive(_wss_message_queue, &stale_data, 5);
                if (stale_data)
                {
                    ezlopi_free(__FUNCTION__, stale_data->payload);
                    ezlopi_free(__FUNCTION__, stale_data);
                }
            }

            rx_message->payload = payload;
            rx_message->time_stamp = time_stamp;
            rx_message->tick_count = xTaskGetTickCount();

            if (pdTRUE == xQueueSend(_wss_message_queue, &rx_message, 5))
            {
                ret = 1;
            }
            else
            {
                ezlopi_free(__FUNCTION__, rx_message);
            }
        }
    }

    return ret;
}

static int __send_cjson_data_to_nma_websocket(cJSON *cj_data)
{
    int ret = 0;

    if (cj_data)
    {
        uint32_t buffer_len = 0;
        char *data_buffer = EZPI_core_buffer_acquire(__FUNCTION__, &buffer_len, 5000);

        if (data_buffer && buffer_len)
        {
            memset(data_buffer, 0, buffer_len);

            if (true == cJSON_PrintPreallocated(__FUNCTION__, cj_data, data_buffer, buffer_len, false))
            {
                // trace_warning("data-buffer: %s", data_buffer);
                ret = __send_str_data_to_nma_websocket(data_buffer);

                if (EZPI_SUCCESS == ret)
                {
                    TRACE_S("NMA-send:\r\n%s", data_buffer);
                }
                else
                {
                    TRACE_E("NMA-send:\r\n%s", data_buffer);
                }
            }
            else
            {
                TRACE_E("FAILED!");
            }

            EZPI_core_buffer_release(__FUNCTION__);
        }
    }

    return ret;
}

static ezlopi_error_t __send_str_data_to_nma_websocket(char *str_data)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (str_data && EZPI_core_websocket_client_is_connected(wss_client))
    {
        int retries = 3;
        while (--retries)
        {
            if (EZPI_SUCCESS == EZPI_core_websocket_client_send(wss_client, str_data, strlen(str_data), 3000))
            {
                ret = EZPI_SUCCESS;
                message_counter++;
                break;
            }

            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    }

    return ret;
}

static void __provision_check(void *pv)
{
    uint8_t flag_break_loop = 0;
    static uint8_t retry_count = 0;
    static uint8_t retry_count_failure = 0;

#if (1 == TEST_PROV)
    char *ca_certificate = test_ca_cert;
    char *ssl_private_key = test_ssl_pvt_key;
    char *ssl_shared_key = test_ssl_shared_key;

    char *provision_token = test_prov_token;
    uint16_t config_version = test_version_num;
#else
    char *ssl_private_key = EZPI_core_factory_info_v3_get_ssl_private_key();
    char *ssl_shared_key = EZPI_core_factory_info_v3_get_ssl_shared_key();
    char *ca_certificate = EZPI_core_factory_info_v3_get_ca_certificate();
    char *provision_token = EZPI_core_factory_info_v3_get_provision_token();
    uint16_t config_version = EZPI_core_factory_info_v3_get_config_version();
#endif

    if (ssl_private_key && ssl_shared_key && ca_certificate && provision_token)
    {
        while (1)
        {
            char http_request_location[500];
#if (1 == TEST_PROV)
            snprintf(http_request_location, sizeof(http_request_location), "https://ezlopiesp32.up.mios.com/provision-sync?token=%s&version=%d", provision_token, config_version ? config_version + 1 : 1);
#else
            snprintf(http_request_location, sizeof(http_request_location), "https://ezlopiesp32.up.mios.com/provision-sync?token=%s&version=%d", provision_token, config_version ? config_version : 1);
#endif

            EZPI_core_wait_for_wifi_to_connect(portMAX_DELAY);
            s_ezlopi_http_data_t *response = EZPI_core_http_get_request(http_request_location, NULL, NULL, NULL);

            if (NULL != response)
            {
                switch (response->status_code)
                {
                case HttpStatus_Ok:
                {
                    if (response->response)
                    {
                        int _update_ret = __provision_update(response->response);

                        if (_update_ret > 0)
                        {
                            EZPI_core_reset_reboot();
                        }
                        else if (_update_ret < 0)
                        {
                            retry_count++;
                            if (retry_count >= 5)
                            {
                                flag_break_loop = 1;
                            }
                        }
                        else
                        {
                            flag_break_loop = 1;
                            TRACE_W("Data not available on cloud!");
                            TRACE_OTEL(ENUM_EZLOPI_TRACE_SEVERITY_WARNING, "Config data not available!");
                        }
                    }

                    break;
                }
                default:
                {
                    if (304 == response->status_code) // HTTP Status not modified
                    {
                        TRACE_S("Config data not changed !");
                        TRACE_OTEL(ENUM_EZLOPI_TRACE_SEVERITY_INFO, "Config data not changed.");
                        flag_break_loop = 1;
                    }
                    break;
                }
                }

                EZPI_core_factory_info_v3_free(response->response);
                EZPI_core_factory_info_v3_free(response);
            }
            else
            {
                if (++retry_count_failure >= 10)
                {
                    break;
                }
            }

            if (flag_break_loop)
            {
                TRACE_D("Terminating provison-check task!");
                TRACE_OTEL(ENUM_EZLOPI_TRACE_SEVERITY_INFO, "Terminating provison-check task!");
                xTaskNotifyGive(__web_socket_initialize_handler);
                break;
            }

            vTaskDelay(100 / portTICK_RATE_MS);
        }

        vTaskDelay(5000 / portTICK_RATE_MS);
    }

#if (0 == TEST_PROV)
    EZPI_core_factory_info_v3_free(provision_token);
#endif

#if defined(CONFIG_FREERTOS_USE_TRACE_FACILITY)
    EZPI_core_process_set_is_deleted(ENUM_EZLOPI_SERVICE_WEB_PROV_CONFIG_CHECK_TASK);
#endif
    vTaskDelete(NULL);
}

static int __provision_update(char *arg)
{
    int ret = 0;
    cJSON *cj_root_prov_data = cJSON_Parse(__FUNCTION__, arg);

    if (NULL != cj_root_prov_data)
    {
        cJSON *cj_root_data = cJSON_GetObjectItem(__FUNCTION__, cj_root_prov_data, ezlopi_data_str);
        cJSON *cj_error_code = cJSON_GetObjectItem(__FUNCTION__, cj_root_prov_data, "error_code");

        if (NULL != cj_root_data)
        {
            s_basic_factory_info_t config_check_factoryInfo;
            memset(&config_check_factoryInfo, 0, sizeof(s_basic_factory_info_t));

            char tmp_id_str[20];
            char tmp_dev_uuid[40];
            char tmp_prov_uuid[40];
            char tmp_cloud_server[128];
            char tmp_provision_token[300];

            CJSON_GET_VALUE_STRING_BY_COPY(cj_root_data, ezlopi_id_str, tmp_id_str);
            CJSON_GET_VALUE_STRING_BY_COPY(cj_root_data, ezlopi_uuid_str, tmp_dev_uuid);
            CJSON_GET_VALUE_STRING_BY_COPY(cj_root_data, ezlopi_cloud_uuid_str, tmp_prov_uuid);
            CJSON_GET_VALUE_DOUBLE(cj_root_data, ezlopi_version_str, config_check_factoryInfo.config_version);
            CJSON_GET_VALUE_STRING_BY_COPY(cj_root_data, ezlopi_coordinator_url_str, tmp_cloud_server);
            CJSON_GET_VALUE_STRING_BY_COPY(cj_root_data, ezlopi_provision_token_str, tmp_provision_token);

            config_check_factoryInfo.id = strtoul(tmp_id_str, NULL, 10);
            config_check_factoryInfo.device_uuid = tmp_dev_uuid;
            config_check_factoryInfo.prov_uuid = tmp_prov_uuid;

            config_check_factoryInfo.cloud_server = tmp_cloud_server;
            config_check_factoryInfo.provision_token = tmp_provision_token;
            config_check_factoryInfo.provision_server = NULL;

#if 0 // unused for now, but can be used in future
      // TODO  Decide if needs parsing and storing to flash
            if (NULL != cJSON_zwave_region_aary)
            {
                if (cJSON_IsArray(cJSON_zwave_region_aary))
                {
                    cJSON *cJSON_zwave_region = cJSON_GetArrayItem(cJSON_zwave_region_aary, 0); // Get the first item
                    if (cJSON_zwave_region)
                    {
                        const char *zwave_region = cJSON_zwave_region->valuestring;
                    }
                }
            }

            uint32_t provision_order = 0;
            CJSON_GET_VALUE_DOUBLE(cj_root_prov_data, "provision_order", provision_order);
#endif

            config_check_factoryInfo.brand = NULL;
            config_check_factoryInfo.device_name = NULL;
            config_check_factoryInfo.device_type = NULL;
            config_check_factoryInfo.manufacturer = NULL;
            config_check_factoryInfo.model_number = NULL;

            if (EZPI_core_factory_info_v3_set_basic(&config_check_factoryInfo))
            {
                TRACE_S("Updated basic config");
                ret = 1;
            }
            else
            {
                TRACE_E("Error updating basic config");
            }

            cJSON *cj_ssl_private_key = cJSON_GetObjectItem(__FUNCTION__, cj_root_data, ezlopi_ssl_private_key_str);
            if (cj_ssl_private_key && cj_ssl_private_key->valuestring && cj_ssl_private_key->str_value_len)
            {
                EZPI_core_factory_info_v3_set_ssl_private_key(cj_ssl_private_key);
            }

            cJSON *cj_ssl_public_key = cJSON_GetObjectItem(__FUNCTION__, cj_root_data, ezlopi_ssl_public_key_str);
            if (cj_ssl_public_key && cj_ssl_public_key->valuestring && cj_ssl_public_key->str_value_len)
            {
                EZPI_core_factory_info_v3_set_ssl_public_key(cj_ssl_public_key);
            }

            cJSON *cj_ssl_shared_key = cJSON_GetObjectItem(__FUNCTION__, cj_root_data, ezlopi_ssl_shared_key_str);
            if (cj_ssl_shared_key && cj_ssl_shared_key->valuestring && cj_ssl_shared_key->str_value_len)
            {
                EZPI_core_factory_info_v3_set_ssl_shared_key(cj_ssl_shared_key);
            }

            cJSON *cj_ca_certificate = cJSON_GetObjectItem(__FUNCTION__, cj_root_data, ezlopi_signing_ca_certificate_str);
            if (cj_ca_certificate && cj_ca_certificate->valuestring && cj_ca_certificate->str_value_len)
            {
                EZPI_core_factory_info_v3_set_ca_cert(cj_ca_certificate);
            }
        }
        else if (cj_error_code && cj_error_code->string)
        {
            ret = 0;
        }
        else
        {
            ret = -1;
            TRACE_E("key \"%s\" not found.", cj_root_prov_data);
        }

        cJSON_Delete(__FUNCTION__, cj_root_prov_data);
    }
    else
    {
        ret = -1;
        TRACE_E("Failed parsing JSON!");
    }

    return ret;
}

#endif // CONFIG_EZPI_WEBSOCKET_CLIENT
