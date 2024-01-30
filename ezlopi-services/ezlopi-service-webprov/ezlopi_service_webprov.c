
#include "esp_mac.h"
#include "esp_wifi_types.h"
#include "esp_idf_version.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_core_wifi.h"
#include "ezlopi_core_http.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_event_group.h"
#include "ezlopi_core_websocket_client.h"

#include "ezlopi_cloud_data.h"
#include "ezlopi_cloud_devices.h"
#include "ezlopi_cloud_scenes.h"
#include "ezlopi_cloud_registration.h"
#include "ezlopi_cloud_favorite.h"
#include "ezlopi_cloud_gateways.h"
#include "ezlopi_cloud_info.h"
#include "ezlopi_cloud_modes.h"
#include "ezlopi_cloud_modes_updaters.h"
#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_room.h"
#include "ezlopi_cloud_network.h"
#include "ezlopi_cloud_ota.h"
#include "ezlopi_cloud_settings.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_cloud_scenes_scripts.h"
#include "ezlopi_cloud_scenes_expressions.h"

#include "ezlopi_service_webprov.h"

static uint32_t message_counter = 0;
static xTaskHandle _task_handle = NULL;

static TaskHandle_t ezlopi_update_config_notifier = NULL;

static void __connection_upcall(bool connected);
static void __message_upcall(const char *payload, uint32_t len);
static void __rpc_method_notfound(cJSON *cj_request, cJSON *cj_response);
static void __hub_reboot(cJSON *cj_request, cJSON *cj_response);
static void __fetch_wss_endpoint(void *pv);
static void web_provisioning_config_check(void *pv);
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
#include "ezlopi_api_url_macros.h"
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
                    __print_sending_data(cjson_str_data, TRACE_TYPE_W);
                }

                free(cjson_str_data);
            }
        }
    }

    return ret;
}

void web_provisioning_init(void)
{
    xTaskCreate(web_provisioning_config_check, "web-provisioning config check", 4 * 2048, NULL, 5, NULL);
    xTaskCreate(__fetch_wss_endpoint, "web-provisioning fetch wss endpoint", 3 * 2048, NULL, 5, &ezlopi_update_config_notifier);
}

static uint8_t web_provisioning_config_update(void *arg)
{
    cJSON *root_prov_data = cJSON_Parse((char *)arg);
    uint8_t ret = 0;
    if (NULL != root_prov_data)
    {

        cJSON *cJSON_id = cJSON_GetObjectItem(root_prov_data, "id");
        cJSON *cJSON_uuid = cJSON_GetObjectItem(root_prov_data, "uuid");
        cJSON *cJSON_cloud_uuid = cJSON_GetObjectItem(root_prov_data, "cloud_uuid");
        cJSON *cJSON_order_uuid = cJSON_GetObjectItem(root_prov_data, "order_uuid");
        cJSON *cJSON_config_version = cJSON_GetObjectItem(root_prov_data, "config_version");
        // cJSON *cJSON_zwave_region_aary = cJSON_GetObjectItem(root_prov_data, "zwave_region");
        cJSON *cJSON_provision_server = cJSON_GetObjectItem(root_prov_data, "provision_server");
        cJSON *cJSON_cloud_server = cJSON_GetObjectItem(root_prov_data, "cloud_server");
        cJSON *cJSON_provision_token = cJSON_GetObjectItem(root_prov_data, "provision_token");
        // cJSON *cJSON_provision_order = cJSON_GetObjectItem(root_prov_data, "provision_order");
        cJSON *cJSON_ssl_private_key = cJSON_GetObjectItem(root_prov_data, "ssl_private_key");
        // cJSON *cJSON_ssl_public_key = NULL; // cJSON_GetObjectItem(root_prov_data, "ssl_public_key");
        cJSON *cJSON_ssl_shared_key = cJSON_GetObjectItem(root_prov_data, "ssl_shared_key");
        cJSON *cJSON_signing_ca_certificate = cJSON_GetObjectItem(root_prov_data, "signing_ca_certificate");

        s_basic_factory_info_t *config_check_factoryInfo = malloc(sizeof(s_basic_factory_info_t));

        if (NULL != cJSON_id)
        {
            const uint64_t id = cJSON_id->valueint;
            TRACE_I("id: %lld", id);
            config_check_factoryInfo->id = id;
        }
        else
        {
            config_check_factoryInfo->id = 0;
        }

        if (NULL != cJSON_uuid)
        {
            const char *uuid = cJSON_uuid->valuestring;
            TRACE_I("uuid: %s", uuid);
            config_check_factoryInfo->device_uuid = (char *)uuid;
        }
        else
        {
            config_check_factoryInfo->device_uuid = NULL;
        }

        if (NULL != cJSON_cloud_uuid)
        {
            // const char *cloud_uuid = cJSON_cloud_uuid->valuestring;
            // TRACE_I("cloud_uuid: %s", cloud_uuid);
        }
        else
        {
        }
        if (NULL != cJSON_order_uuid)
        {
            // const char *order_uuid = cJSON_order_uuid->valuestring;
            // TRACE_I("order_uuid: %s", order_uuid);
        }

        if (NULL != cJSON_config_version)
        {
            const uint16_t config_version = cJSON_config_version->valueint;
            TRACE_I("config_version: %d", config_version);
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
            const char *provision_server = cJSON_provision_server->valuestring;
            TRACE_I("provision_server: %s", provision_server);
            config_check_factoryInfo->provision_server = (char *)provision_server;
        }
        else
        {
            config_check_factoryInfo->provision_server = NULL;
        }
        if (NULL != cJSON_cloud_server)
        {
            const char *cloud_server = cJSON_cloud_server->valuestring;
            TRACE_I("cloud_server: %s", cloud_server);
            config_check_factoryInfo->cloud_server = (char *)cloud_server;
        }
        else
        {
            config_check_factoryInfo->cloud_server = NULL;
        }

        if (NULL != cJSON_provision_token)
        {
            const char *provision_token = cJSON_provision_token->valuestring;
            TRACE_I("provision_token: %s", provision_token);
            config_check_factoryInfo->provision_token = (char *)provision_token;
        }
        else
        {
            config_check_factoryInfo->provision_token = NULL;
        }

        // TODO Decide about its usefulness
        // if (NULL != cJSON_provision_order)
        // {
        //     const uint32_t provision_order = cJSON_provision_order->valueint;
        //     TRACE_I("provision_order: %d", provision_order);
        // }
        if (NULL != cJSON_ssl_private_key)
        {
            const char *ssl_private_key = cJSON_ssl_private_key->valuestring;
            TRACE_I("ssl_private_key: %s", ssl_private_key);
            ezlopi_factory_info_v3_set_ssl_private_key(ssl_private_key);
        }

        // if (NULL != cJSON_ssl_public_key)
        // {
        //     const char *ssl_public_key = cJSON_ssl_public_key->valuestring;
        //     TRACE_I("ssl_public_key: %s", ssl_public_key);
        //     ezlopi_factory_info_v2_set_ssl_public_key(ssl_public_key);
        // }
        if (NULL != cJSON_ssl_shared_key)
        {
            const char *ssl_shared_key = cJSON_ssl_shared_key->valuestring;
            TRACE_I("ssl_shared_key: %s", ssl_shared_key);
            ezlopi_factory_info_v3_set_ssl_shared_key(ssl_shared_key);
        }

        if (NULL != cJSON_signing_ca_certificate)
        {
            const char *signing_ca_certificate = cJSON_signing_ca_certificate->valuestring;
            TRACE_I("signing_ca_certificate: %s", signing_ca_certificate);
            ezlopi_factory_info_v3_set_ca_cert(signing_ca_certificate);
        }

        config_check_factoryInfo->device_name = NULL;
        config_check_factoryInfo->manufacturer = NULL;
        config_check_factoryInfo->brand = NULL;
        config_check_factoryInfo->model_number = NULL;
        config_check_factoryInfo->device_type = NULL;
        config_check_factoryInfo->prov_uuid = NULL; // NULL since it is not

        if (ezlopi_factory_info_v3_set_basic(config_check_factoryInfo))
        {
            TRACE_I("Updated provisioning config");
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

static void web_provisioning_config_check(void *pv)
{
    s_ezlopi_http_data_t *response = malloc(sizeof(s_ezlopi_http_data_t));
    char *ca_certificate = NULL;
    char *provision_token = NULL;
    char *provisioning_server = NULL;
    static uint8_t retry_count = 0;
    uint8_t flag_break_loop = 0;

    ca_certificate = ezlopi_factory_info_v3_get_ca_certificate();
    provision_token = ezlopi_factory_info_get_v3_provision_token();
    provisioning_server = ezlopi_factory_info_v3_get_provisioning_server();
    uint16_t config_version = ezlopi_factory_info_v3_get_config_version();

    while (1)
    {
        ezlopi_wait_for_wifi_to_connect(portMAX_DELAY);

        TRACE_D("water_mark: %d", uxTaskGetStackHighWaterMark(NULL));

        cJSON *root_header_prov_token = cJSON_CreateObject();

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
            snprintf(http_request_location, sizeof(http_request_location), "api/v1/controller/sync?version=%d", config_version); // add config_version instead of 1
            response = ezlopi_http_post_request(provisioning_server, http_request_location, root_header_prov_token, NULL, NULL, ca_certificate);
            if (NULL != response)
            {
                TRACE_I("Statuc Code : %d", response->status_code);

                switch (response->status_code)
                {
                case HttpStatus_Ok:
                {
                    // re-write all the info into the flash region
                    TRACE_I("Data : %s", response->response);
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
                        TRACE_I("Config data not changed !");
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

void web_provisioning_deinit(void)
{
    if (_task_handle)
    {
        vTaskDelete(_task_handle);
    }

    ezlopi_websocket_client_kill();
}

static void __fetch_wss_endpoint(void *pv)
{
    s_ezlopi_http_data_t *ws_endpoint = NULL;

    while (1)
    {

        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
        ezlopi_wait_for_wifi_to_connect(portMAX_DELAY);

        vTaskDelay(100 / portTICK_RATE_MS);

        char *cloud_server = ezlopi_factory_info_v3_get_cloud_server();
        char *ca_certificate = ezlopi_factory_info_v3_get_ca_certificate();
        char *ssl_shared_key = ezlopi_factory_info_v3_get_ssl_shared_key();
        char *ssl_private_key = ezlopi_factory_info_v3_get_ssl_private_key();

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
        free(cloud_server);
        free(ca_certificate);
        free(ssl_shared_key);
        free(ssl_private_key);
        vTaskDelay(2000 / portTICK_RATE_MS);
    }

    vTaskDelete(NULL);
}

static void __connection_upcall(bool connected)
{
    TRACE_D("connected: %d", connected);
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
        ezlopi_event_group_clear_event(EZLOPI_EVENT_NMA_REG);
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
    web_provisioning_deinit();
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