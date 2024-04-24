
#include <esp_mac.h>
#include <esp_wifi_types.h>
#include <esp_idf_version.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "ezlopi_util_trace.h"
#include "ezlopi_cloud_constants.h"

#include "ezlopi_core_http.h"
#include "ezlopi_core_wifi.h"
#include "ezlopi_core_event_group.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_ezlopi_methods.h"
#include "ezlopi_core_processes.h"
#include "ezlopi_core_websocket_client.h"

#include "ezlopi_service_webprov.h"

static uint32_t message_counter = 0;
static xTaskHandle _task_handle = NULL;

static TaskHandle_t ezlopi_update_config_notifier = NULL;

static void __config_check(void* pv);
static void __fetch_wss_endpoint(void* pv);
static void __connection_upcall(bool connected);
static void __hub_reboot(cJSON* cj_request, cJSON* cj_response);
static void __message_upcall(const char* payload, uint32_t len);
static void __rpc_method_notfound(cJSON* cj_request, cJSON* cj_response);
static void __print_sending_data(char* data_str, e_trace_type_t print_type);

uint32_t ezlopi_service_web_provisioning_get_message_count(void)
{
    return message_counter;
}

int ezlopi_service_web_provisioning_send_str_data_to_nma_websocket(char* str_data, e_trace_type_t print_type)
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

int ezlopi_service_web_provisioning_send_to_nma_websocket(cJSON* cjson_data, e_trace_type_t print_type)
{
    int ret = 0;
    if (ezlopi_websocket_client_is_connected())
    {
        if (cjson_data)
        {
            char* cjson_str_data = cJSON_Print(cjson_data);
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
    TaskHandle_t ezlopi_service_web_prov_config_check_task_handle = NULL;
    xTaskCreate(__config_check, "WebProvCfgChk", EZLOPI_SERVICE_WEB_PROV_CONFIG_CHECK_TASK_DEPTH, NULL, 5, &ezlopi_service_web_prov_config_check_task_handle);
    ezlopi_core_process_set_process_info(ENUM_EZLOPI_SERVICE_WEB_PROV_CONFIG_CHECK_TASK, &ezlopi_service_web_prov_config_check_task_handle, EZLOPI_SERVICE_WEB_PROV_CONFIG_CHECK_TASK_DEPTH);
    xTaskCreate(__fetch_wss_endpoint, "WebProvFetchWSS", EZLOPI_SERVICE_WEB_PROV_FETCH_WSS_TASK_DEPTH, NULL, 5, &ezlopi_update_config_notifier);
    ezlopi_core_process_set_process_info(ENUM_EZLOPI_SERVICE_WEB_PROV_FETCH_WSS_TASK, &ezlopi_update_config_notifier, EZLOPI_SERVICE_WEB_PROV_FETCH_WSS_TASK_DEPTH);
}

static uint8_t web_provisioning_config_update(void* arg)
{
    cJSON* root_prov_data = cJSON_Parse((char*)arg);
    uint8_t ret = 0;
    if (NULL != root_prov_data)
    {

        cJSON* cJSON_id = cJSON_GetObjectItem(root_prov_data, "id");
        cJSON* cJSON_uuid = cJSON_GetObjectItem(root_prov_data, "uuid");
        cJSON* cJSON_cloud_uuid = cJSON_GetObjectItem(root_prov_data, "cloud_uuid");
        // cJSON* cJSON_order_uuid = cJSON_GetObjectItem(root_prov_data, "order_uuid");
        cJSON* cJSON_config_version = cJSON_GetObjectItem(root_prov_data, "config_version");
        // cJSON *cJSON_zwave_region_aary = cJSON_GetObjectItem(root_prov_data, "zwave_region");
        cJSON* cJSON_provision_server = cJSON_GetObjectItem(root_prov_data, "provision_server");
        cJSON* cJSON_cloud_server = cJSON_GetObjectItem(root_prov_data, "cloud_server");
        cJSON* cJSON_provision_token = cJSON_GetObjectItem(root_prov_data, "provision_token");
        // cJSON *cJSON_provision_order = cJSON_GetObjectItem(root_prov_data, "provision_order");
        cJSON* cJSON_ssl_private_key = cJSON_GetObjectItem(root_prov_data, "ssl_private_key");
        cJSON* cJSON_ssl_public_key = cJSON_GetObjectItem(root_prov_data, "ssl_public_key");
        cJSON* cJSON_ssl_shared_key = cJSON_GetObjectItem(root_prov_data, "ssl_shared_key");
        cJSON* cJSON_signing_ca_certificate = cJSON_GetObjectItem(root_prov_data, "signing_ca_certificate");

        s_basic_factory_info_t* config_check_factoryInfo = malloc(sizeof(s_basic_factory_info_t));

        if (NULL != cJSON_id)
        {
            const uint64_t id = cJSON_id->valueint;
            TRACE_S("id: %lld", id);
            config_check_factoryInfo->id = id;
        }
        else
        {
            config_check_factoryInfo->id = 0;
        }

        if (NULL != cJSON_uuid)
        {
            #warning "Lomas need to check this"
                const char* uuid = cJSON_uuid->valuestring;
            TRACE_I("uuid: %s", uuid);
            config_check_factoryInfo->device_uuid = (char*)uuid;
        }
        else
        {
            config_check_factoryInfo->device_uuid = NULL;
        }

        if (NULL != cJSON_cloud_uuid)
        {
            #warning "Lomas need to check this"
                const char* cloud_uuid = cJSON_cloud_uuid->valuestring;
            TRACE_S("cloud_uuid: %s", cloud_uuid);
            config_check_factoryInfo->prov_uuid = cloud_uuid;
        }
        else
        {
        }
        // if (NULL != cJSON_order_uuid)
        // {
        //     // const char *order_uuid = cJSON_order_uuid->valuestring;
        //     // TRACE_S("order_uuid: %s", order_uuid);
        // }

        if (NULL != cJSON_config_version)
        {
            const uint16_t config_version = cJSON_config_version->valueint;
            TRACE_S("config_version: %d", config_version);
            config_check_factoryInfo->config_version = config_version;
        }
        else
        {
            config_check_factoryInfo->config_version = 0;
        }

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
        if (NULL != cJSON_provision_server)
        {
            const char* provision_server = cJSON_provision_server->valuestring;
            TRACE_S("provision_server: %s", provision_server);
            config_check_factoryInfo->provision_server = (char*)provision_server;
        }
        else
        {
            config_check_factoryInfo->provision_server = NULL;
        }
        if (NULL != cJSON_cloud_server)
        {
            const char* cloud_server = cJSON_cloud_server->valuestring;
            TRACE_S("cloud_server: %s", cloud_server);
            config_check_factoryInfo->cloud_server = (char*)cloud_server;
        }
        else
        {
            config_check_factoryInfo->cloud_server = NULL;
        }

        if (NULL != cJSON_provision_token)
        {
            const char* provision_token = cJSON_provision_token->valuestring;
            TRACE_S("provision_token: %s", provision_token);
            config_check_factoryInfo->provision_token = (char*)provision_token;
        }
        else
        {
            config_check_factoryInfo->provision_token = NULL;
        }

        // TODO Decide about its usefulness
        // if (NULL != cJSON_provision_order)
        // {
        //     const uint32_t provision_order = cJSON_provision_order->valueint;
        //     TRACE_S("provision_order: %d", provision_order);
        // }
        if (NULL != cJSON_ssl_private_key)
        {
            const char* ssl_private_key = cJSON_ssl_private_key->valuestring;
            TRACE_S("ssl_private_key: %s", ssl_private_key);
            ezlopi_factory_info_v3_set_ssl_private_key(ssl_private_key);
        }

        if (NULL != cJSON_ssl_public_key)
        {
            const char* ssl_public_key = cJSON_ssl_public_key->valuestring;
            TRACE_S("ssl_public_key: %s", ssl_public_key);
            ezlopi_factory_info_v3_set_ssl_public_key(ssl_public_key);
        }
        if (NULL != cJSON_ssl_shared_key)
        {
            const char* ssl_shared_key = cJSON_ssl_shared_key->valuestring;
            TRACE_S("ssl_shared_key: %s", ssl_shared_key);
            ezlopi_factory_info_v3_set_ssl_shared_key(ssl_shared_key);
        }

        if (NULL != cJSON_signing_ca_certificate)
        {
            const char* signing_ca_certificate = cJSON_signing_ca_certificate->valuestring;
            TRACE_S("signing_ca_certificate: %s", signing_ca_certificate);
            ezlopi_factory_info_v3_set_ca_cert(signing_ca_certificate);
        }

        config_check_factoryInfo->device_name = NULL;
        config_check_factoryInfo->manufacturer = NULL;
        config_check_factoryInfo->brand = NULL;
        config_check_factoryInfo->model_number = NULL;
        config_check_factoryInfo->device_type = NULL;
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
        cJSON_Delete(root_prov_data);
    }
    else
    {
        TRACE_E("Error parsing JSON.\n");
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
                    if (0 == web_provisioning_config_update(response->response))
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
    ezlopi_core_process_set_is_deleted(ENUM_EZLOPI_SERVICE_WEB_PROV_CONFIG_CHECK_TASK);
    vTaskDelete(NULL);
}

void ezlopi_service_web_provisioning_deinit(void)
{
    if (_task_handle)
    {
        vTaskDelete(_task_handle);
    }

    ezlopi_websocket_client_kill();
}

static void __fetch_wss_endpoint(void* pv)
{
    uint32_t task_complete = 0;
    s_ezlopi_http_data_t* ws_endpoint = NULL;
    esp_websocket_client_handle_t ezlopi_wss_client = NULL;

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
    ezlopi_core_process_set_is_deleted(ENUM_EZLOPI_SERVICE_WEB_PROV_FETCH_WSS_TASK);
    vTaskDelete(NULL);
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

static void __call_method_and_send_response(cJSON* cj_request, cJSON* cj_method, f_method_func_t method_func, e_trace_type_t print_type)
{
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

                char* data_to_send = cJSON_Print(cj_response);
                cJSON_Delete(cj_response);

                if (data_to_send)
                {
                    cJSON_Minify(data_to_send);
                    ezlopi_service_web_provisioning_send_str_data_to_nma_websocket(data_to_send, print_type);
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
                    char* method_name = ezlopi_core_ezlopi_methods_get_name_by_id(method_id);
                    TRACE_D("Method[%d]: %s", method_id, method_name ? method_name : "null");

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
            TRACE_E("## WS Rx <<<<<<<<<< '%s'\r\n%.*s", (NULL != cj_method) ? (cj_method->valuestring ? cj_method->valuestring : ezlopi__str) : ezlopi__str, len, payload);
            TRACE_E("cj_error: %p, cj_error->type: %u, cj_error->value_string: %s", cj_error, cj_error->type, cj_error ? (cj_error->valuestring ? cj_error->valuestring : ezlopi_null_str) : ezlopi_null_str);
        }

        cJSON_Delete(cj_request);
    }
}

static void __print_sending_data(char* data_str, e_trace_type_t print_type)
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