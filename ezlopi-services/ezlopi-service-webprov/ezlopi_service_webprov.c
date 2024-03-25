
#include <esp_mac.h>
#include <esp_wifi_types.h>
#include <esp_idf_version.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include "ezlopi_util_trace.h"
#include "ezlopi_cloud_constants.h"

#include "ezlopi_core_http.h"
#include "ezlopi_core_wifi.h"
#include "ezlopi_core_event_group.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_ezlopi_methods.h"
#include "ezlopi_core_websocket_client.h"
#include "ezlopi_core_ezlopi_broadcast.h"

#include "ezlopi_service_webprov.h"

static uint32_t message_counter = 0;
static xSemaphoreHandle sg_web_prov_lock = NULL;
static xTaskHandle _task_handle = NULL;

static TaskHandle_t ezlopi_update_config_notifier = NULL;

static void __config_check(void* pv);
static uint8_t __config_update(void* arg);
static void __fetch_wss_endpoint(void* pv);
static void __connection_upcall(bool connected);
static int __send_str_data_to_nma_websocket(char* str_data);
static void __message_upcall(const char* payload, uint32_t len);
static void __print_sending_data(char* data_str, e_trace_type_t print_type);

uint32_t ezlopi_service_web_provisioning_get_message_count(void)
{
    return message_counter;
}

int ezlopi_service_web_provisioning_send_to_nma_websocket(cJSON* cjson_data, e_trace_type_t print_type)
{
    int ret = 0;
    if (ezlopi_websocket_client_is_connected())
    {
        if (cjson_data)
        {
            char* cjson_str_data = cJSON_PrintBuffered(cjson_data, 256, false);

            if (cjson_str_data)
            {
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
                    __print_sending_data(cjson_str_data, TRACE_TYPE_W);
                }

                free(cjson_str_data);
            }
        }
    }

    return ret;
}

void ezlopi_service_web_provisioning_init(void)
{
    xTaskCreate(__config_check, "web-provisioning config check", 4 * 2048, NULL, 5, NULL);
    xTaskCreate(__fetch_wss_endpoint, "web-provisioning fetch wss endpoint", 3 * 2048, NULL, 5, &ezlopi_update_config_notifier);
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

                        sg_web_prov_lock = xSemaphoreCreateMutex();
                        if (sg_web_prov_lock)
                        {
                            xSemaphoreGive(sg_web_prov_lock);
                        }

                        ezlopi_core_ezlopi_broadcast_method_add(__send_str_data_to_nma_websocket, 4);
                        ezlopi_websocket_client_init(cjson_uri, __message_upcall, __connection_upcall);

                        task_complete = 1;
                    }
                }
            }
        }

        if (task_complete)
        {
            if (ws_endpoint->response)
                free(ws_endpoint->response);
            if (ws_endpoint)
                free(ws_endpoint);
            break;
        }

        free(cloud_server);
        free(ca_certificate);
        free(ssl_shared_key);
        free(ssl_private_key);
        vTaskDelay(2000 / portTICK_RATE_MS);
        vTaskDelay(2000 / portTICK_RATE_MS);
    }

    vTaskDelete(NULL);
}

static int __call_method_and_send_response(cJSON* cj_request, cJSON* cj_method, f_method_func_t method_func, e_trace_type_t print_type)
{
    int ret = 0;
    if (method_func)
    {
        if (ezlopi_core_elzlopi_methods_check_method_register(method_func))
        {
            method_func(cj_request, NULL);
        }
        else
        {
            cJSON* cj_response = cJSON_CreateObject();
            if (NULL != cj_response)
            {
                cJSON* cj_sender = cJSON_GetObjectItem(cj_request, ezlopi_sender_str);

                cJSON_AddNumberToObject(cj_response, ezlopi_msg_id_str, message_counter);
                cJSON_AddItemReferenceToObject(cj_response, ezlopi_sender_str, cj_sender);
                cJSON_AddNullToObject(cj_response, ezlopi_error_str);

                method_func(cj_request, cj_response);

                if (sg_web_prov_lock)
                {
                    if (pdTRUE == xSemaphoreTake(sg_web_prov_lock, 2000 / portTICK_RATE_MS))
                    {
                        char* data_to_send = cJSON_PrintBuffered(cj_response, 10 * 1024, false);
                        cJSON_Delete(cj_response);

                        if (data_to_send)
                        {
                            ret = __send_str_data_to_nma_websocket(data_to_send);
                            free(data_to_send);
                        }

                        xSemaphoreGive(sg_web_prov_lock);
                    }
                    else
                    {
                        cJSON_Delete(cj_response);
                    }
                }
                else
                {
                    char* data_to_send = cJSON_PrintBuffered(cj_response, 10 * 1024, false);
                    cJSON_Delete(cj_response);

                    if (data_to_send)
                    {
                        ret = __send_str_data_to_nma_websocket(data_to_send);
                        free(data_to_send);
                    }
                }
            }
            else
            {
                TRACE_E("Error - cj_response: %d", (uint32_t)cj_response);
            }
        }
    }

    return ret;
}

static void __message_upcall(const char* payload, uint32_t len)
{
    cJSON* cj_request = cJSON_ParseWithLength(payload, len);

    if (cj_request)
    {
        cJSON* cj_error = cJSON_GetObjectItem(cj_request, ezlopi_error_str);
        cJSON* cj_method = cJSON_GetObjectItem(cj_request, ezlopi_method_str);

        if ((NULL == cj_error) || (cJSON_NULL == cj_error->type) || (NULL != cj_error->valuestring) || ((NULL != cj_error->valuestring) && (0 == strncmp(cj_error->valuestring, ezlopi_null_str, 4))))
        {
            if ((NULL != cj_method) && (NULL != cj_method->valuestring))
            {
                TRACE_S("## WS Rx <<<<<<<<<< '%s'\r\n%.*s", (cj_method->valuestring ? cj_method->valuestring : ezlopi__str), len, payload);

                uint32_t method_id = ezlopi_core_ezlopi_methods_search_in_list(cj_method);

                if (UINT32_MAX != method_id)
                {
#if (1 == ENABLE_TRACE)
                    char* method_name = ezlopi_core_ezlopi_methods_get_name_by_id(method_id);
                    TRACE_D("Method[%d]: %s", method_id, method_name ? method_name : "null");
#endif

                    f_method_func_t method = ezlopi_core_ezlopi_methods_get_by_id(method_id);
                    if (method)
                    {
                        __call_method_and_send_response(cj_request, cj_method, method, TRACE_TYPE_D);
                    }

                    f_method_func_t updater = ezlopi_core_ezlopi_methods_get_updater_by_id(method_id);
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
#if (1 == ENABLE_TRACE)
            TRACE_E("## WS Rx <<<<<<<<<< '%s'\r\n%.*s", (NULL != cj_method) ? (cj_method->valuestring ? cj_method->valuestring : ezlopi__str) : ezlopi__str, len, payload);
            TRACE_E("cj_error: %p, cj_error->type: %u, cj_error->value_string: %s", cj_error, cj_error->type, cj_error ? (cj_error->valuestring ? cj_error->valuestring : ezlopi_null_str) : ezlopi_null_str);
#endif
        }

        cJSON_Delete(cj_request);
    }
}

static int __send_str_data_to_nma_websocket(char* str_data)
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
            TRACE_D("## WSS-SENDING >>>>>>>>>>>>>>>>>>>\r\n%s", str_data);
        }
        else
        {
            TRACE_W("## WSS-SENDING >>>>>>>>>>>>>>>>>>>\r\n%s", str_data);
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
        cJSON* root_header_prov_token = cJSON_CreateObject();
        cJSON_AddStringToObject(root_header_prov_token, "controller-key", provision_token);

        if (NULL != provisioning_server)
        {
            int prov_url_len = strlen(provisioning_server);

            if (prov_url_len >= 5 && strcmp(&provisioning_server[prov_url_len - 5], ".com/") == 0)
            {
                provisioning_server[prov_url_len - 1] = '\0'; // Remove trailing "/"
            }
        }
        else
        {
            break;
            xTaskNotifyGive(ezlopi_update_config_notifier);
        }

        if ((NULL != ca_certificate) && (NULL != provision_token) && (NULL != provisioning_server))
        {
            char http_request_location[200];

            snprintf(http_request_location, sizeof(http_request_location), "api/v1/controller/sync?version=%d", config_version);
            response = ezlopi_http_post_request(provisioning_server, http_request_location, root_header_prov_token, NULL, NULL, ca_certificate);

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

                free(response->response);
                free(response);
            }
            else
            {
                free(response);
            }

            if (flag_break_loop)
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

        vTaskDelay(50000 / portTICK_RATE_MS);
    }

    free(ca_certificate);
    free(provision_token);
    free(provisioning_server);
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

            CJSON_GET_VALUE_STRING(root_prov_data, ezlopi_provision_server_str, config_check_factoryInfo->provision_server);
            CJSON_GET_VALUE_STRING(root_prov_data, ezlopi_cloud_server_str, config_check_factoryInfo->cloud_server);
            CJSON_GET_VALUE_STRING(root_prov_data, ezlopi_provision_token_str, config_check_factoryInfo->provision_token);

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

static void __print_sending_data(char* data_str, e_trace_type_t print_type)
{
#if (1 == ENABLE_TRACE)
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
#endif
}