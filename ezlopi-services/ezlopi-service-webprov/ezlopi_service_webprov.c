
#include <esp_mac.h>
#include <esp_wifi_types.h>
#include <esp_idf_version.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include "ezlopi_util_trace.h"
#include "ezlopi_cloud_constants.h"
#include "../../build/config/sdkconfig.h"

#include "ezlopi_core_api.h"
#include "ezlopi_core_http.h"
#include "ezlopi_core_wifi.h"
#include "ezlopi_core_buffer.h"
#include "ezlopi_core_api_methods.h"
#include "ezlopi_core_event_group.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_api_methods.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_processes.h"
#include "ezlopi_core_websocket_client.h"
#include "ezlopi_core_ezlopi_broadcast.h"

#include "ezlopi_service_webprov.h"
#include "EZLOPI_USER_CONFIG.h"


#if defined(CONFIG_EZPI_WEBSOCKET_CLIENT)

static uint32_t message_counter = 0;
static xTaskHandle _task_handle = NULL;
static TaskHandle_t ezlopi_update_config_notifier = NULL;

static void __config_check(void* pv);
static void __fetch_wss_endpoint(void* pv);

static void __connection_upcall(bool connected);
static void __message_upcall(const char* payload, uint32_t len);

static uint8_t __config_update(char* arg);

static int __send_str_data_to_nma_websocket(char* str_data);
static int __send_cjson_data_to_nma_websocket(cJSON* cj_data);

uint32_t ezlopi_service_web_provisioning_get_message_count(void)
{
    return message_counter;
}

void ezlopi_service_web_provisioning_init(void)
{

    TaskHandle_t ezlopi_service_web_prov_config_check_task_handle = NULL;
    xTaskCreate(__config_check, "WebProvCfgChk", EZLOPI_SERVICE_WEB_PROV_CONFIG_CHECK_TASK_DEPTH, NULL, 5, &ezlopi_service_web_prov_config_check_task_handle);
    ezpi_core_process_set_process_info(ENUM_EZLOPI_SERVICE_WEB_PROV_CONFIG_CHECK_TASK, &ezlopi_service_web_prov_config_check_task_handle, EZLOPI_SERVICE_WEB_PROV_CONFIG_CHECK_TASK_DEPTH);
    xTaskCreate(__fetch_wss_endpoint, "WebProvFetchWSS", EZLOPI_SERVICE_WEB_PROV_FETCH_WSS_TASK_DEPTH, NULL, 5, &ezlopi_update_config_notifier);
    ezpi_core_process_set_process_info(ENUM_EZLOPI_SERVICE_WEB_PROV_FETCH_WSS_TASK, &ezlopi_update_config_notifier, EZLOPI_SERVICE_WEB_PROV_FETCH_WSS_TASK_DEPTH);
}

void ezlopi_service_web_provisioning_deinit(void)
{
    if (_task_handle)
    {
        vTaskDelete(_task_handle);
    }

    ezlopi_websocket_client_kill();
}

static void __connection_upcall(bool connected)
{
    TRACE_D("wss-connection: %s", connected ? "connected" : "failed to connect");
    static bool prev_status;
    if (connected)
    {
        if (prev_status != connected)
        {
            TRACE_S("Web-socket re-connected.");
            TRACE_I("Starting registration process....");
            ezlopi_core_ezlopi_methods_registration_init();
        }
        else
        {
            TRACE_S("Web-socket connected");
        }
    }
    else
    {
        ezlopi_event_group_clear_event(EZLOPI_EVENT_NMA_REG);
    }

    prev_status = connected;
}

static void __fetch_wss_endpoint(void* pv)
{

    char* cloud_server = ezlopi_factory_info_v3_get_cloud_server();
    char* ca_certificate = ezlopi_factory_info_v3_get_ca_certificate();
    char* ssl_shared_key = ezlopi_factory_info_v3_get_ssl_shared_key();
    char* ssl_private_key = ezlopi_factory_info_v3_get_ssl_private_key();

    ezlopi_wait_for_wifi_to_connect(portMAX_DELAY);

    while (1)
    {
        uint32_t task_complete = 0;
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
        vTaskDelay(100 / portTICK_RATE_MS);

        char http_request[128];
        snprintf(http_request, sizeof(http_request), "%s/getserver?json=true", cloud_server);
        TRACE_D("http_request: %s", http_request);

        s_ezlopi_http_data_t * ws_endpoint = ezlopi_http_get_request(http_request, ssl_private_key, ssl_shared_key, ca_certificate);
        if (ws_endpoint)
        {
            if (ws_endpoint->response)
            {
                TRACE_D("ws_endpoint: %s", ws_endpoint->response); // {"uri": "wss://endpoint:port"}
                cJSON* root = cJSON_Parse(__FUNCTION__, ws_endpoint->response);
                if (root)
                {
                    cJSON* cjson_uri = cJSON_GetObjectItem(__FUNCTION__, root, "uri");
                    if (cjson_uri)
                    {
                        TRACE_D("uri: %s", cjson_uri->valuestring ? cjson_uri->valuestring : "NULL");

                        ezlopi_core_ezlopi_broadcast_method_add(__send_str_data_to_nma_websocket, "nma-websocket", 4);
                        ezlopi_websocket_client_init(cjson_uri, __message_upcall, __connection_upcall);

                        task_complete = 1;
                    }

                    cJSON_Delete(__FUNCTION__, root);
                }

                ezlopi_free(__FUNCTION__, ws_endpoint->response);
            }

            ezlopi_free(__FUNCTION__, ws_endpoint);
        }

        if (task_complete)
        {
            break;
        }

        vTaskDelay(2000 / portTICK_RATE_MS);
    }

    ezlopi_factory_info_v3_free(cloud_server);
    // ezlopi_factory_info_v3_free(ca_certificate); // allocated once for all, do not free
    // ezlopi_factory_info_v3_free(ssl_shared_key); // allocated once for all, do not free
    // ezlopi_factory_info_v3_free(ssl_private_key); // allocated once for all, do not free

    ezpi_core_process_set_is_deleted(ENUM_EZLOPI_SERVICE_WEB_PROV_FETCH_WSS_TASK);
    vTaskDelete(NULL);
}

static void __message_upcall(const char* payload, uint32_t len)
{
    static const char * __who = "webprov-message-upcall";
    cJSON* cj_response = ezlopi_core_api_consume(__who, payload, len);
    if (cj_response)
    {
        cJSON_AddNumberToObject(__FUNCTION__, cj_response, ezlopi_msg_id_str, message_counter);
        __send_cjson_data_to_nma_websocket(cj_response);

        cJSON_Delete(__FUNCTION__, cj_response);
    }
    else
    {
        TRACE_W("no response!");
    }
}

static int __send_cjson_data_to_nma_websocket(cJSON* cj_data)
{
    int ret = 0;

    if (cj_data)
    {
        uint32_t buffer_len = 0;
        char* data_buffer = ezlopi_core_buffer_acquire(&buffer_len, 5000);

        if (data_buffer && buffer_len)
        {
            TRACE_I("-----------------------------> buffer acquired!");
            memset(data_buffer, 0, buffer_len);

            if (true == cJSON_PrintPreallocated(__FUNCTION__, cj_data, data_buffer, buffer_len, false))
            {
                ret = __send_str_data_to_nma_websocket(data_buffer);
            }
            else
            {
                TRACE_E("FAILED!");
            }

            ezlopi_core_buffer_release();
            TRACE_I("-----------------------------> buffer released!");
        }
        else
        {
            TRACE_E("-----------------------------> buffer acquired failed!");
        }
    }

    return ret;
}

static int __send_str_data_to_nma_websocket(char* str_data)
{
    int ret = 0;
    if (str_data)
    {
        int retries = 3;
        while (--retries)
        {
            if (ezlopi_websocket_client_send(str_data, strlen(str_data)))
            {
                ret = 1;
                message_counter++;
                break;
            }

            vTaskDelay(50 / portTICK_PERIOD_MS);
        }

        if (ret)
        {
            TRACE_S("## WSC-SENDING done >>>>>>>>>>>>>>>>>>>\r\n%s", str_data);
        }
        else
        {
            TRACE_W("## WSC-SENDING failed >>>>>>>>>>>>>>>>>>>\r\n%s", str_data);
        }
    }

    return ret;
}

static void __config_check(void* pv)
{
    uint8_t flag_break_loop = 0;
    static uint8_t retry_count = 0;

    char* ca_certificate = ezlopi_factory_info_v3_get_ca_certificate();
    char* provision_token = ezlopi_factory_info_get_v3_provision_token();
    char* provisioning_server = ezlopi_factory_info_v3_get_provisioning_server();
    uint16_t config_version = ezlopi_factory_info_v3_get_config_version();

    if (ca_certificate && provision_token && provisioning_server)
    {
        cJSON* root_header_prov_token = cJSON_CreateObject(__FUNCTION__);
        if (root_header_prov_token)
        {
            cJSON_AddStringToObject(__FUNCTION__, root_header_prov_token, "controller-key", provision_token);

            while (1)
            {
                int prov_url_len = strlen(provisioning_server);
                if ((prov_url_len >= 5) && (0 == strcmp(&provisioning_server[prov_url_len - 5], ".com/")))
                {
                    provisioning_server[prov_url_len - 1] = '\0'; // Remove trailing "/"
                }

                char http_request_location[200];
                snprintf(http_request_location, sizeof(http_request_location), "api/v1/controller/sync?version=%d", config_version + 1);

                TRACE_D("web-prov-request: %s", http_request_location);

                ezlopi_wait_for_wifi_to_connect(portMAX_DELAY);
                s_ezlopi_http_data_t* response = ezlopi_http_post_request(provisioning_server, http_request_location, root_header_prov_token, NULL, NULL, ca_certificate);

                if (NULL != response)
                {
                    TRACE_S("Status Code : %d", response->status_code);
                    switch (response->status_code)
                    {
                    case HttpStatus_Ok:
                    {
                        // re-write all the info into the flash region
                        TRACE_S("Data : %s", response->response);
                        if (0 == __config_update(response->response))
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

                    ezlopi_factory_info_v3_free(response->response);
                    ezlopi_factory_info_v3_free(response);
                }

                if (flag_break_loop)
                {
                    xTaskNotifyGive(ezlopi_update_config_notifier);
                    break;
                }

                vTaskDelay(50000 / portTICK_RATE_MS);
            }

            cJSON_Delete(__FUNCTION__, root_header_prov_token);
            TRACE_S("root_header_prov_token - deleted");
        }
    }

    // ezlopi_factory_info_v3_free(ca_certificate); // allocated once for all, do not free
    ezlopi_factory_info_v3_free(provisioning_server);
    ezlopi_factory_info_v3_free(provision_token);

    ezpi_core_process_set_is_deleted(ENUM_EZLOPI_SERVICE_WEB_PROV_CONFIG_CHECK_TASK);
    vTaskDelete(NULL);
}

static uint8_t __config_update(char* arg)
{
    uint8_t ret = 0;
    cJSON* cj_root_prov_data = cJSON_Parse(__FUNCTION__, arg);

    if (NULL != cj_root_prov_data)
    {
        s_basic_factory_info_t config_check_factoryInfo;
        memset(&config_check_factoryInfo, 0, sizeof(s_basic_factory_info_t));

        char tmp_dev_uuid[40];
        char tmp_prov_uuid[40];
        char tmp_cloud_server[128];
        char tmp_provision_token[256];
        char tmp_provision_server[128];

        CJSON_GET_VALUE_DOUBLE(cj_root_prov_data, ezlopi_id_str, config_check_factoryInfo.id);
        CJSON_GET_VALUE_STRING_BY_COPY(cj_root_prov_data, ezlopi_uuid_str, tmp_dev_uuid);
        CJSON_GET_VALUE_STRING_BY_COPY(cj_root_prov_data, ezlopi_cloud_uuid_str, tmp_prov_uuid);
        CJSON_GET_VALUE_DOUBLE(cj_root_prov_data, ezlopi_config_version_str, config_check_factoryInfo.config_version);

        CJSON_GET_VALUE_STRING_BY_COPY(cj_root_prov_data, ezlopi_cloud_server_str, tmp_cloud_server);
        CJSON_GET_VALUE_STRING_BY_COPY(cj_root_prov_data, ezlopi_provision_token_str, tmp_provision_token);
        CJSON_GET_VALUE_STRING_BY_COPY(cj_root_prov_data, ezlopi_provision_server_str, tmp_provision_server);

        config_check_factoryInfo.device_uuid = tmp_dev_uuid;
        config_check_factoryInfo.prov_uuid = tmp_prov_uuid;

        config_check_factoryInfo.cloud_server = tmp_cloud_server;
        config_check_factoryInfo.provision_token = tmp_provision_token;
        config_check_factoryInfo.provision_server = tmp_provision_server;

        // TODO  Decide if needs parsing and storing to flash
        // if (NULL != cJSON_zwave_region_aary)
        // {
        //     if (cJSON_IsArray(cJSON_zwave_region_aary))
        //     {
        //         cJSON *cJSON_zwave_region = cJSON_GetArrayItem(cJSON_zwave_region_aary, 0); // Get the first item
        //         if (cJSON_zwave_region)
        //         {
        //             const char *zwave_region = cJSON_zwave_region->valuestring;
        //         }
        //     }
        // }

        // uint32_t provision_order = 0;
        // CJSON_GET_VALUE_DOUBLE(cj_root_prov_data, "provision_order", provision_order);

        config_check_factoryInfo.brand = NULL;
        config_check_factoryInfo.device_name = NULL;
        config_check_factoryInfo.device_type = NULL;
        config_check_factoryInfo.manufacturer = NULL;
        config_check_factoryInfo.model_number = NULL;
        // config_check_factoryInfo.prov_uuid = NULL; // NULL since it is not

        if (ezlopi_factory_info_v3_set_basic(&config_check_factoryInfo))
        {
            TRACE_S("Updated provisioning config");
            ret = 1;
        }
        else
        {
            TRACE_E("Error updating provisioning config");
        }

        cJSON * cj_ssl_private_key = cJSON_GetObjectItem(__FUNCTION__, cj_root_prov_data, ezlopi_ssl_private_key_str);
        if (cj_ssl_private_key && cj_ssl_private_key->valuestring && cj_ssl_private_key->str_value_len)
        {
            ezlopi_factory_info_v3_set_ssl_private_key(cj_ssl_private_key);
        }

        cJSON * cj_ssl_public_key = cJSON_GetObjectItem(__FUNCTION__, cj_root_prov_data, ezlopi_ssl_public_key_str);
        if (cj_ssl_public_key && cj_ssl_public_key->valuestring && cj_ssl_public_key->str_value_len)
        {
            ezlopi_factory_info_v3_set_ssl_public_key(cj_ssl_public_key);
        }

        cJSON * cj_ssl_shared_key = cJSON_GetObjectItem(__FUNCTION__, cj_root_prov_data, ezlopi_ssl_shared_key_str);
        if (cj_ssl_shared_key && cj_ssl_shared_key->valuestring && cj_ssl_shared_key->str_value_len)
        {
            ezlopi_factory_info_v3_set_ssl_shared_key(cj_ssl_shared_key);
        }

        cJSON * cj_ca_certificate = cJSON_GetObjectItem(__FUNCTION__, cj_root_prov_data, ezlopi_signing_ca_certificate_str);
        if (cj_ca_certificate && cj_ca_certificate->valuestring && cj_ca_certificate->str_value_len)
        {
            ezlopi_factory_info_v3_set_ca_cert(cj_ca_certificate);
        }

        cJSON_Delete(__FUNCTION__, cj_root_prov_data);
    }
    else
    {
        TRACE_E("Error parsing JSON.\n");
    }
    return ret;
}
#endif // CONFIG_EZPI_WEBSOCKET_CLIENT