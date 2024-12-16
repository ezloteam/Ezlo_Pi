
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
    time_t time_ms;
    char *payload;
} s_rx_message_t;

static uint32_t message_counter = 0;
static xTaskHandle _task_handle = NULL;
static esp_websocket_client_handle_t wss_client = NULL;
static QueueHandle_t _wss_message_queue = NULL;
static TaskHandle_t __web_socket_initialize_handler = NULL;

static int __provision_update(char *arg);
static void __provision_check(void *pv);
static void __fetch_wss_endpoint(void *pv);

static void __connection_upcall(bool connected);
static int __message_upcall(char *payload, uint32_t len, time_t time_ms);
static void __message_process_cjson(cJSON *cj_request, time_t time_ms);

static ezlopi_error_t __send_str_data_to_nma_websocket(char *str_data);
static int __send_cjson_data_to_nma_websocket(cJSON *cj_data);

bool ezlopi_service_webprov_is_connected(void)
{
    return esp_websocket_client_is_connected(wss_client);
}

uint32_t ezlopi_service_web_provisioning_get_message_count(void)
{
    return message_counter;
}

void ezlopi_service_web_provisioning_init(void)
{
    TaskHandle_t ezlopi_service_web_prov_config_check_task_handle = NULL;
    xTaskCreate(__provision_check, "WebProvCfgChk", EZLOPI_SERVICE_WEB_PROV_CONFIG_CHECK_TASK_DEPTH, NULL, 4, &ezlopi_service_web_prov_config_check_task_handle);
    ezlopi_core_process_set_process_info(ENUM_EZLOPI_SERVICE_WEB_PROV_CONFIG_CHECK_TASK, &ezlopi_service_web_prov_config_check_task_handle, EZLOPI_SERVICE_WEB_PROV_CONFIG_CHECK_TASK_DEPTH);

    _wss_message_queue = xQueueCreate(10, sizeof(s_rx_message_t *));

    xTaskCreate(__fetch_wss_endpoint, "WebProvFetchWSS", EZLOPI_SERVICE_WEB_PROV_FETCH_WSS_TASK_DEPTH, NULL, 4, &__web_socket_initialize_handler);
    ezlopi_core_process_set_process_info(ENUM_EZLOPI_SERVICE_WEB_PROV_FETCH_WSS_TASK, &__web_socket_initialize_handler, EZLOPI_SERVICE_WEB_PROV_FETCH_WSS_TASK_DEPTH);
}

void ezlopi_service_web_provisioning_deinit(void)
{
    if (_task_handle)
    {
        vTaskDelete(_task_handle);
    }

    ezlopi_websocket_client_kill(wss_client);
}

static void __connection_upcall(bool connected)
{
    static int prev_status; // 0: never connected, 1: Not-connected, 2: connected
    if (connected)
    {
        if (0 == prev_status)
        {
            TRACE_S("Web-socket Connected.");
        }
        else
        {
            TRACE_S("Web-socket Re-connected.");
        }

        prev_status = 2;
        TRACE_I("Starting registration process....");
        ezlopi_core_ezlopi_methods_registration_init();
    }
    else
    {
        prev_status = 1;
        ezlopi_event_group_clear_event(EZLOPI_EVENT_NMA_REG);
    }
}

static void __fetch_wss_endpoint(void *pv)
{
    char *ca_certificate = ezlopi_factory_info_v3_get_ca_certificate();
    char *ssl_shared_key = ezlopi_factory_info_v3_get_ssl_shared_key();
    char *ssl_private_key = ezlopi_factory_info_v3_get_ssl_private_key();
    char *cloud_server = ezlopi_factory_info_v3_get_cloud_server();

    ezlopi_wait_for_wifi_to_connect(portMAX_DELAY);
    vTaskDelay(2);

    while (1)
    {
        uint32_t task_complete = 0;

        if (ca_certificate && ssl_shared_key && ssl_private_key && cloud_server)
        {
            char http_request[128];
            snprintf(http_request, sizeof(http_request), "%s?json=true", cloud_server);
            TRACE_D("http_request: %s", http_request);

            s_ezlopi_http_data_t *ws_endpoint = ezlopi_http_get_request(http_request, ssl_private_key, ssl_shared_key, ca_certificate);
            // s_ezlopi_http_data_t * ws_endpoint = ezlopi_http_get_request(http_request, NULL, NULL, NULL);

            if (ws_endpoint)
            {
                if (ws_endpoint->response)
                {
                    TRACE_D("ws_endpoint: %s", ws_endpoint->response); // {"uri": "wss://endpoint:port"}
                    cJSON *root = cJSON_Parse(__FUNCTION__, ws_endpoint->response);
                    if (root)
                    {
                        cJSON *cjson_uri = cJSON_GetObjectItem(__FUNCTION__, root, "uri");
                        if (cjson_uri)
                        {
                            TRACE_D("uri: %s", cjson_uri->valuestring ? cjson_uri->valuestring : "NULL");
                            ezlopi_core_broadcast_method_add(__send_str_data_to_nma_websocket, "nma-websocket", 4);
                            wss_client = ezlopi_websocket_client_init(cjson_uri, __message_upcall, __connection_upcall,
                                                                      ca_certificate, ssl_private_key, ssl_shared_key);
                            task_complete = 1;
                        }

                        cJSON_Delete(__FUNCTION__, root);
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
                BaseType_t ret = xQueueReceive(_wss_message_queue, &rx_message, 100 / portTICK_RATE_MS);

                if (rx_message && (ret == pdTRUE))
                {
                    cJSON *cj_method_dup = NULL;

                    if (rx_message->payload)
                    {
                        cJSON *cj_request = cJSON_Parse(__FUNCTION__, rx_message->payload);
                        if (cj_request)
                        {
                            cJSON *cj_method = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_method_str);

                            if (cj_method)
                            {
                                cj_method_dup = cJSON_Duplicate(__FUNCTION__, cj_method, true);

                                printf("web-provisioning [method: %.*s]\r\n%s\r\n", cj_method->str_value_len, cj_method->valuestring, rx_message->payload);
                                // TRACE_D("rx_message->payload [method: %.*s]\r\n%s", cj_method->str_value_len, cj_method->valuestring, rx_message->payload);
                                ezlopi_free(__FUNCTION__, rx_message->payload);
                                rx_message->payload = NULL;
                            }
                            else
                            {
                                printf("web-provisioning [method: null]\r\n%s\r\n", rx_message->payload);
                                // TRACE_E("rx_message->payload [method: null]\r\n%s", rx_message->payload);
                            }

                            __message_process_cjson(cj_request, rx_message->time_ms);

                            cJSON_Delete(__FUNCTION__, cj_request);
                        }

                        ezlopi_free(__FUNCTION__, rx_message->payload);
                        rx_message->payload = NULL;
                    }

                    ezlopi_free(__FUNCTION__, rx_message);

#if 1
                    if (cj_method_dup)
                    {
                        cJSON *cj_trace_telemetry = cJSON_CreateObject(__FUNCTION__);
                        if (cj_trace_telemetry)
                        {
                            time_t now = 0;

                            if (false == cJSON_AddItemToObject(__FUNCTION__, cj_trace_telemetry, ezlopi_method_str, cj_method_dup))
                            {
                                cJSON_Delete(__FUNCTION__, cj_method_dup);
                            }

                            cJSON_AddNumberToObject(__FUNCTION__, cj_trace_telemetry, ezlopi_kind_str, 1);
                            cJSON_AddNumberToObject(__FUNCTION__, cj_trace_telemetry, ezlopi_startTime_str, rx_message->time_ms);

                            time(&now);
                            cJSON_AddNumberToObject(__FUNCTION__, cj_trace_telemetry, ezlopi_endTime_str, now);

                            if (0 == ezlopi_service_otel_add_trace_to_telemetry_queue(cj_trace_telemetry))
                            {
                                cJSON_Delete(__FUNCTION__, cj_trace_telemetry);
                            }
                        }
                        else
                        {
                            cJSON_Delete(__FUNCTION__, cj_method_dup);
                        }
                    }
#endif
                }

                vTaskDelay(1);
            }

            break;
        }

        vTaskDelay(2000 / portTICK_RATE_MS);
    }

    ezlopi_factory_info_v3_free(cloud_server);

    ezlopi_core_process_set_is_deleted(ENUM_EZLOPI_SERVICE_WEB_PROV_FETCH_WSS_TASK);
    vTaskDelete(NULL);
}

static void __message_process_cjson(cJSON *cj_request, time_t time_ms)
{
    if (cj_request)
    {
        time_t now;
        cJSON *cj_response = ezlopi_core_api_consume_cjson(__FUNCTION__, cj_request);

        time(&now);
        TRACE_D("time to process: %lu", now - time_ms);

        if (cj_response)
        {
            cJSON_AddNumberToObject(__FUNCTION__, cj_response, ezlopi_msg_id_str, message_counter);
            __send_cjson_data_to_nma_websocket(cj_response);

            time(&now);
            TRACE_D("time to reply: %lu", now - time_ms);

            cJSON_Delete(__FUNCTION__, cj_response);
        }
        else
        {
            TRACE_W("no response!");
        }
    }
}

static int __message_upcall(char *payload, uint32_t len, time_t time_ms)
{
    int ret = 0;

    if (_wss_message_queue)
    {
        s_rx_message_t *rx_message = ezlopi_malloc(__FUNCTION__, sizeof(s_rx_message_t));
        if (rx_message)
        {
            if (pdTRUE == xQueueIsQueueFullFromISR(_wss_message_queue))
            {
                char *stale_data = NULL;
                xQueueReceive(_wss_message_queue, &stale_data, 5);
                if (stale_data)
                {
                    ezlopi_free(__FUNCTION__, stale_data);
                }
            }

            rx_message->payload = payload;
            rx_message->time_ms = time_ms;

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
        char *data_buffer = ezlopi_core_buffer_acquire(__FUNCTION__, &buffer_len, 5000);

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

            ezlopi_core_buffer_release(__FUNCTION__);
        }
    }

    return ret;
}

static ezlopi_error_t __send_str_data_to_nma_websocket(char *str_data)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (str_data && ezlopi_websocket_client_is_connected(wss_client))
    {
        int retries = 3;
        while (--retries)
        {
            if (EZPI_SUCCESS == ezlopi_websocket_client_send(wss_client, str_data, strlen(str_data), 3000))
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
    char *ssl_private_key = ezlopi_factory_info_v3_get_ssl_private_key();
    char *ssl_shared_key = ezlopi_factory_info_v3_get_ssl_shared_key();
    char *ca_certificate = ezlopi_factory_info_v3_get_ca_certificate();
    char *provision_token = ezlopi_factory_info_get_v3_provision_token();
    uint16_t config_version = ezlopi_factory_info_v3_get_config_version();
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

            ezlopi_wait_for_wifi_to_connect(portMAX_DELAY);
            s_ezlopi_http_data_t *response = ezlopi_http_get_request(http_request_location, NULL, NULL, NULL);

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
                            EZPI_CORE_reset_reboot();
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
                        }

                        ezlopi_factory_info_v3_free(response->response);
                    }

                    break;
                }
                default:
                {
                    if (304 == response->status_code) // HTTP Status not modified
                    {
                        TRACE_S("Config data not changed !");
                        flag_break_loop = 1;
                    }
                    break;
                }
                }

                ezlopi_factory_info_v3_free(response);
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
                xTaskNotifyGive(__web_socket_initialize_handler);
                break;
            }

            vTaskDelay(100 / portTICK_RATE_MS);
        }

        vTaskDelay(5000 / portTICK_RATE_MS);
    }

#if (0 == TEST_PROV)
    ezlopi_factory_info_v3_free(provision_token);
#endif

    ezlopi_core_process_set_is_deleted(ENUM_EZLOPI_SERVICE_WEB_PROV_CONFIG_CHECK_TASK);
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

            if (ezlopi_factory_info_v3_set_basic(&config_check_factoryInfo))
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
                ezlopi_factory_info_v3_set_ssl_private_key(cj_ssl_private_key);
            }

            cJSON *cj_ssl_public_key = cJSON_GetObjectItem(__FUNCTION__, cj_root_data, ezlopi_ssl_public_key_str);
            if (cj_ssl_public_key && cj_ssl_public_key->valuestring && cj_ssl_public_key->str_value_len)
            {
                ezlopi_factory_info_v3_set_ssl_public_key(cj_ssl_public_key);
            }

            cJSON *cj_ssl_shared_key = cJSON_GetObjectItem(__FUNCTION__, cj_root_data, ezlopi_ssl_shared_key_str);
            if (cj_ssl_shared_key && cj_ssl_shared_key->valuestring && cj_ssl_shared_key->str_value_len)
            {
                ezlopi_factory_info_v3_set_ssl_shared_key(cj_ssl_shared_key);
            }

            cJSON *cj_ca_certificate = cJSON_GetObjectItem(__FUNCTION__, cj_root_data, ezlopi_signing_ca_certificate_str);
            if (cj_ca_certificate && cj_ca_certificate->valuestring && cj_ca_certificate->str_value_len)
            {
                ezlopi_factory_info_v3_set_ca_cert(cj_ca_certificate);
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
