
#include "../../build/config/sdkconfig.h"

#include <esp_mac.h>
#include <esp_wifi_types.h>
#include <esp_idf_version.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include "ezlopi_util_trace.h"
#include "ezlopi_cloud_constants.h"

#include "ezlopi_core_api.h"
#include "ezlopi_core_http.h"
#include "ezlopi_core_wifi.h"
#include "ezlopi_core_buffer.h"
#include "ezlopi_core_api_methods.h"
#include "ezlopi_core_event_group.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_processes.h"
#include "ezlopi_core_websocket_client.h"
#include "ezlopi_core_ezlopi_broadcast.h"

#include "ezlopi_service_webprov.h"


#if defined(CONFIG_EZPI_WEBSOCKET_CLIENT)

static uint32_t message_counter = 0;
static xTaskHandle _task_handle = NULL;
static TaskHandle_t ezlopi_update_config_notifier = NULL;

static void __config_check(void* pv);
static void __fetch_wss_endpoint(void* pv);

static void __connection_upcall(bool connected);
static void __message_upcall(const char* payload, uint32_t len);

static uint8_t __config_update(void* arg);

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
    ezlopi_core_process_set_process_info(ENUM_EZLOPI_SERVICE_WEB_PROV_CONFIG_CHECK_TASK, &ezlopi_service_web_prov_config_check_task_handle, EZLOPI_SERVICE_WEB_PROV_CONFIG_CHECK_TASK_DEPTH);
    xTaskCreate(__fetch_wss_endpoint, "WebProvFetchWSS", EZLOPI_SERVICE_WEB_PROV_FETCH_WSS_TASK_DEPTH, NULL, 5, &ezlopi_update_config_notifier);
    ezlopi_core_process_set_process_info(ENUM_EZLOPI_SERVICE_WEB_PROV_FETCH_WSS_TASK, &ezlopi_update_config_notifier, EZLOPI_SERVICE_WEB_PROV_FETCH_WSS_TASK_DEPTH);
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
    TRACE_I("wss-connection: %s", connected ? "connected" : "failed to connect");
    static bool prev_status;
    if (connected)
    {
        if (prev_status != connected)
        {
            TRACE_S("Web-socket re-connected.");
            TRACE_I("Starting registration process....");
            ezlopi_core_ezlopi_methods_registration_init();
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
    uint32_t task_complete = 0;
    s_ezlopi_http_data_t* ws_endpoint = NULL;

    while (1)
    {
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
        ezlopi_wait_for_wifi_to_connect(portMAX_DELAY);
        vTaskDelay(100 / portTICK_RATE_MS);

        char* cloud_server = ezlopi_factory_info_v3_get_cloud_server();
        char* ca_certificate = ezlopi_factory_info_v3_get_ca_certificate();
        char* ssl_shared_key = ezlopi_factory_info_v3_get_ssl_shared_key();
        char* ssl_private_key = ezlopi_factory_info_v3_get_ssl_private_key();

        char http_request[128];
        snprintf(http_request, sizeof(http_request), "%s/getserver?json=true", cloud_server);
        TRACE_D("http_request: %s", http_request);
        ws_endpoint = ezlopi_http_get_request(http_request, ssl_private_key, ssl_shared_key, ca_certificate);

        if (ws_endpoint)
        {
            if (ws_endpoint->response)
            {
                TRACE_D("ws_endpoint: %s", ws_endpoint->response); // {"uri": "wss://endpoint:port"}
                cJSON* root = cJSON_Parse(ws_endpoint->response);
                if (root)
                {
                    cJSON* cjson_uri = cJSON_GetObjectItem(root, "uri");
                    if (cjson_uri)
                    {
                        TRACE_D("uri: %s", cjson_uri->valuestring ? cjson_uri->valuestring : "NULL");
                        ezlopi_core_ezlopi_broadcast_method_add(__send_str_data_to_nma_websocket, "nma-websocket", 4);
                        ezlopi_websocket_client_init(cjson_uri, __message_upcall, __connection_upcall);
                        task_complete = 1;
                    }
                }
            }
        }

        if (task_complete)
        {
            if (ws_endpoint)
            {
                if (ws_endpoint->response)
                {
                    free(ws_endpoint->response);
                }

                free(ws_endpoint);
            }

            break;
        }

        free(cloud_server);
        free(ca_certificate);
        free(ssl_shared_key);
        free(ssl_private_key);
        vTaskDelay(500 / portTICK_RATE_MS);
    }
    ezlopi_core_process_set_is_deleted(ENUM_EZLOPI_SERVICE_WEB_PROV_FETCH_WSS_TASK);
    vTaskDelete(NULL);
}

static void __message_upcall(const char* payload, uint32_t len)
{
    cJSON* cj_response = ezlopi_core_api_consume(payload, len);
    if (cj_response)
    {
        cJSON_AddNumberToObject(cj_response, ezlopi_msg_id_str, message_counter);
        __send_cjson_data_to_nma_websocket(cj_response);
        cJSON_Delete(cj_response);
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
            // TRACE_I("-----------------------------> buffer acquired!");
            memset(data_buffer, 0, buffer_len);

            if (true == cJSON_PrintPreallocated(cj_data, data_buffer, buffer_len, false))
            {
                ret = __send_str_data_to_nma_websocket(data_buffer);
            }
            else
            {
                TRACE_E("FAILED!");
            }

            ezlopi_core_buffer_release();
            // TRACE_I("-----------------------------> buffer released!");
        }
        else
        {
            // TRACE_E("-----------------------------> buffer acquired failed!");
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
            // TRACE_S("## WSC-SENDING done >>>>>>>>>>>>>>>>>>>\r\n%s", str_data);
        }
        else
        {
            // TRACE_W("## WSC-SENDING failed >>>>>>>>>>>>>>>>>>>\r\n%s", str_data);
        }
    }

    return ret;
}

static void __config_check(void* pv)
{
    uint8_t flag_break_loop = 0;
    static uint8_t retry_count = 0;
    s_ezlopi_http_data_t* response = NULL;
    char* ca_certificate = ezlopi_factory_info_v3_get_ca_certificate();
    char* provision_token = ezlopi_factory_info_get_v3_provision_token();
    char* provisioning_server = ezlopi_factory_info_v3_get_provisioning_server();
    uint16_t config_version = ezlopi_factory_info_v3_get_config_version();

    TRACE_D("water_mark: %d", uxTaskGetStackHighWaterMark(NULL));

    while (1)
    {
        ezlopi_wait_for_wifi_to_connect(portMAX_DELAY);

        if (NULL != provisioning_server)
        {
            int prov_url_len = strlen(provisioning_server);

            if (prov_url_len >= 5 && strcmp(&provisioning_server[prov_url_len - 5], ".com/") == 0)  provisioning_server[prov_url_len - 1] = '\0'; // Remove trailing "/"

            if ((NULL != ca_certificate) && (NULL != provision_token))
            {

                cJSON* root_header_prov_token = cJSON_CreateObject();

                if (root_header_prov_token)
                {
                    char http_request_location[200];
                    snprintf(http_request_location, sizeof(http_request_location), "api/v1/controller/sync?version=%d", config_version);
                    cJSON_AddStringToObject(root_header_prov_token, "controller-key", provision_token);
                    response = ezlopi_http_post_request(provisioning_server, http_request_location, root_header_prov_token, NULL, NULL, ca_certificate);

                    if (NULL != response)
                    {
                        TRACE_S("Status Code : %d", response->status_code);

                        switch (response->status_code)
                        {
                        case HttpStatus_Ok:
                        {
                            // re-write all the info into the flash region
                            // TRACE_S("Data : %s", response->response);
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

                        free(response->response);
                        free(response);
                    }
                    cJSON_Delete(root_header_prov_token);
                }
                else
                {
                    xTaskNotifyGive(ezlopi_update_config_notifier);
                    break;
                }
            }
            else
            {
                xTaskNotifyGive(ezlopi_update_config_notifier);
                break;
            }
        }
        else
        {
            xTaskNotifyGive(ezlopi_update_config_notifier);
            break;
        }
        if (flag_break_loop)
        {
            xTaskNotifyGive(ezlopi_update_config_notifier);
            break;
        }
    }
    free(ca_certificate);
    free(provision_token);
    free(provisioning_server);
    ezlopi_core_process_set_is_deleted(ENUM_EZLOPI_SERVICE_WEB_PROV_CONFIG_CHECK_TASK);
    vTaskDelete(NULL);
}


static uint8_t __config_update(void* arg)
{
    cJSON* root_prov_data = cJSON_Parse((char*)arg);
    uint8_t ret = 0;
    if (NULL != root_prov_data)
    {
        s_basic_factory_info_t* config_check_factoryInfo = malloc(sizeof(s_basic_factory_info_t));

        if (config_check_factoryInfo)
        {
            memset(config_check_factoryInfo, 0, sizeof(s_basic_factory_info_t));

            CJSON_GET_VALUE_DOUBLE(root_prov_data, ezlopi_id_str, config_check_factoryInfo->id);
            CJSON_GET_VALUE_STRING(root_prov_data, ezlopi_uuid_str, config_check_factoryInfo->device_uuid);
            CJSON_GET_VALUE_STRING(root_prov_data, ezlopi_cloud_uuid_str, config_check_factoryInfo->prov_uuid);
            CJSON_GET_VALUE_DOUBLE(root_prov_data, ezlopi_config_version_str, config_check_factoryInfo->config_version);

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

            CJSON_GET_VALUE_STRING(root_prov_data, ezlopi_cloud_server_str, config_check_factoryInfo->cloud_server);
            CJSON_GET_VALUE_STRING(root_prov_data, ezlopi_provision_token_str, config_check_factoryInfo->provision_token);
            CJSON_GET_VALUE_STRING(root_prov_data, ezlopi_provision_server_str, config_check_factoryInfo->provision_server);

            // uint32_t provision_order = 0;
            // CJSON_GET_VALUE_DOUBLE(root_prov_data, "provision_order", provision_order);

            config_check_factoryInfo->brand = NULL;
            config_check_factoryInfo->device_name = NULL;
            config_check_factoryInfo->device_type = NULL;
            config_check_factoryInfo->manufacturer = NULL;
            config_check_factoryInfo->model_number = NULL;
            // config_check_factoryInfo->prov_uuid = NULL; // NULL since it is not

            if (ezlopi_factory_info_v3_set_basic(config_check_factoryInfo))
            {
                TRACE_S("Updated provisioning config");
                ret = 1;
            }
            else
            {
                TRACE_E("Error updating provisioning config");
            }

            free(config_check_factoryInfo);
        }

        const char* ssl_private_key = NULL;
        CJSON_GET_VALUE_STRING(root_prov_data, ezlopi_ssl_private_key_str, ssl_private_key);
        ezlopi_factory_info_v3_set_ssl_private_key(ssl_private_key);

        const char* ssl_public_key = NULL;
        CJSON_GET_VALUE_STRING(root_prov_data, ezlopi_ssl_public_key_str, ssl_public_key);
        ezlopi_factory_info_v3_set_ssl_public_key(ssl_public_key);

        const char* ssl_shared_key = NULL;
        CJSON_GET_VALUE_STRING(root_prov_data, ezlopi_ssl_shared_key_str, ssl_shared_key);
        ezlopi_factory_info_v3_set_ssl_shared_key(ssl_shared_key);

        const char* signing_ca_certificate = NULL;
        CJSON_GET_VALUE_STRING(root_prov_data, ezlopi_signing_ca_certificate_str, signing_ca_certificate);
        ezlopi_factory_info_v3_set_ca_cert(signing_ca_certificate);

        cJSON_Delete(root_prov_data);
    }
    else
    {
        TRACE_E("Error parsing JSON.\n");
    }
    return ret;
}
#endif // CONFIG_EZPI_WEBSOCKET_CLIENT