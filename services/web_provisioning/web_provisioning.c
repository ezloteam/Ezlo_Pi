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
#include "network.h"
#include "firmware.h"
#include "settings.h"
#include "ezlopi_websocket_client.h"

#include "ezlopi_wifi.h"
#include "ezlopi_http.h"
#include "ezlopi_factory_info.h"
#include "ezlopi_cloud_constants.h"

#include "web_provisioning.h"

static uint32_t message_counter = 0;
static TaskHandle_t ezlopi_update_config_notifier = NULL;
static void __connection_upcall(bool connected);
static void __message_upcall(const char *payload, uint32_t len);
static void __rpc_method_notfound(cJSON *cj_request, cJSON *cj_response);
static void __hub_reboot(cJSON *cj_request, cJSON *cj_response);
static void web_provisioning_fetch_wss_endpoint(void *pv);
static void web_provisioning_config_check(void *pv);
static uint8_t web_provisioning_config_update(void *arg);

#if 0
typedef struct s_method_list
{
    char *method_name;
    cJSON *(*method)(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
    cJSON *(*updater)(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);
} s_method_list_t;

static const s_method_list_t method_list[] = {
    /** Getter functions **/
    {.method_name = "hub.data.list", .method = data_list, .updater = NULL},
    {.method_name = "hub.room.list", .method = room_list, .updater = NULL},
    {.method_name = "hub.items.list", .method = items_list, .updater = NULL},
    // {.method_name = "hub.scenes.list", .method = scenes_list, .updater = NULL},
    // {.method_name = "hub.devices.list", .method = devices_list, .updater = NULL},
    // {.method_name = "hub.favorite.list", .method = favorite_list, .updater = NULL},
    // {.method_name = "hub.gateways.list", .method = gateways_list, .updater = NULL},
    // {.method_name = "hub.info.get", .method = info_get, .updater = NULL},
    // {.method_name = "hub.modes.get", .method = modes_get, .updater = NULL},
    // {.method_name = "hub.network.get", .method = network_get, .updater = NULL}, //, .updater = NULL},
    // // {.method_name = "hub.settings.list", .method = settings_list, .updater = NULL},
    // // {.method_name = "hub.device.settings.list", .method = devices_settings_list, .updater = NULL},
    // {.method_name = "hub.reboot", .method = __hub_reboot, .updater = NULL},

    // // // /** Setter functions **/
    // {.method_name = "hub.item.value.set", .method = items_set_value, .updater = items_update},
    // // {.method_name = "hub.device.name.set", .method = devices_name_set, .updater = NULL},
    // // {.method_name = "hub.device.setting.value.set", .method = __rpc_method_notfound, .updater = NULL},
    // {.method_name = "registered", .method = registered, .updater = NULL}, // called only once so its in last

    // // {.method_name = "hub.feature.status.set", .method = __rpc_method_notfound, .updater = NULL}, // documentation missing
    // // {.method_name = "hub.features.list", .method = __rpc_method_notfound, .updater = NULL}, // documentation missing
};
#endif

typedef void (*f_method_func_t)(cJSON *cj_request, cJSON *cj_response);
typedef struct s_method_list_v2
{
    char *method_name;
    f_method_func_t method;
    f_method_func_t updater;
} s_method_list_v2_t;

static const s_method_list_v2_t method_list_v2[] = {
    /** Getter functions **/
    {.method_name = "hub.data.list", .method = data_list, .updater = NULL},
    {.method_name = "hub.room.list", .method = room_list, .updater = NULL},
    {.method_name = "hub.items.list", .method = items_list, .updater = NULL},
    {.method_name = "hub.scenes.list", .method = scenes_list, .updater = NULL},
    {.method_name = "hub.devices.list", .method = devices_list, .updater = NULL},
    {.method_name = "hub.favorite.list", .method = favorite_list, .updater = NULL},
    {.method_name = "hub.gateways.list", .method = gateways_list, .updater = NULL},
    {.method_name = "hub.info.get", .method = info_get, .updater = NULL},
    {.method_name = "hub.modes.get", .method = modes_get, .updater = NULL},
    {.method_name = "hub.network.get", .method = network_get, .updater = NULL},
    {.method_name = "hub.firmware.update.start", .method = firmware_update_start, .updater = NULL},
    {.method_name = "cloud.firmware.info.get", .method = firmware_info_get, .updater = NULL},
    {.method_name = "hub.settings.list", .method = ezlopi_settings_list, .updater = NULL},
    {.method_name = "hub.device.settings.list", .method = ezlopi_device_settings_list, .updater = NULL},
    {.method_name = "hub.reboot", .method = __hub_reboot, .updater = NULL},

    // // /** Setter functions **/
    {.method_name = "hub.item.value.set", .method = items_set_value, .updater = items_update},
    // {.method_name = "hub.device.name.set", .method = devices_name_set, .updater = NULL},
    {.method_name = "hub.setting.value.set", .method = ezlopi_settings_value_set, .updater = ezlopi_settings_value_set_response},
    {.method_name = "hub.device.setting.value.set", .method = ezlopi_device_settings_value_set, .updater = NULL},
    {.method_name = "hub.device.setting.reset", .method = ezlopi_device_settings_reset, .updater = NULL},
    {.method_name = "registered", .method = registered, .updater = NULL}, // called only once so its in last

    {.method_name = "hub.feature.status.set", .method = __rpc_method_notfound, .updater = NULL}, // documentation missing
    {.method_name = "hub.features.list", .method = __rpc_method_notfound, .updater = NULL},      // documentation missing
};

uint32_t web_provisioning_get_message_count(void)
{
    return message_counter;
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
                switch (print_type)
                {
                case TRACE_TYPE_B:
                {
                    TRACE_B("## WSS-SENDING >>>>>>>>>>\r\n%s", cjson_str_data);
                    break;
                }
                case TRACE_TYPE_E:
                {
                    TRACE_E("## WSS-SENDING >>>>>>>>>>\r\n%s", cjson_str_data);
                    break;
                }

                case TRACE_TYPE_I:
                {
                    TRACE_I("## WSS-SENDING >>>>>>>>>>\r\n%s", cjson_str_data);
                    break;
                }
                default:
                    break;
                }

                int ret = ezlopi_websocket_client_send(cjson_str_data, strlen(cjson_str_data));
                if (ret > 0)
                {
                    message_counter++;
                }

                free(cjson_str_data);
            }
        }
    }
    return ret;
}

void web_provisioning_init(void)
{
    // xTaskCreate(web_provisioning_config_check, "web-provisioning config check", 4 * 2048, NULL, 5, NULL);
    xTaskCreate(web_provisioning_fetch_wss_endpoint, "web-provisioning fetch wss endpoint", 3 * 2048, NULL, 5, &ezlopi_update_config_notifier);
}

static void web_provisioning_config_check(void *pv)
{
    char *ws_endpoint = NULL;
    s_ezlopi_http_data_t *response = malloc(sizeof(s_ezlopi_http_data_t));
    char *ca_certificate = NULL;
    char *provision_token = NULL;
    char *provisioning_server = NULL;
    static uint8_t retry_count = 0;
    uint8_t flag_break_loop = 0;

    ca_certificate = ezlopi_factory_info_v2_get_ca_certificate();
    provision_token = ezlopi_factory_info_get_v2_provision_token();
    provisioning_server = ezlopi_factory_info_v2_get_provisioning_server();
    uint16_t config_version = ezlopi_factory_info_v2_get_config_version();

    while (1)
    {
        UBaseType_t water_mark = uxTaskGetStackHighWaterMark(NULL);

        TRACE_D("water_mark: %d", water_mark);

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
            uint16_t http_status;
            response = ezlopi_http_post_request(provisioning_server, http_request_location, root_header_prov_token, NULL, NULL, ca_certificate);
            if (NULL != response)
            {
                switch (response->status_code)
                {
                    TRACE_I("Statuc Code : %d", response->status_code);
                case HttpStatus_Ok:
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
                case 304: // HTTP Status not modified
                    TRACE_I("Config data not changed !");
                    flag_break_loop = 1;
                    break;
                default:
                    break;
                }
                free(response->response);
                free(response);
            }
            else
            {
                free(response);
                TRACE_E("Here");
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

    free(provision_token);
    free(provisioning_server);
    vTaskDelete(NULL);
}

static void web_provisioning_fetch_wss_endpoint(void *pv)
{
    char *ws_endpoint = NULL;
    char *cloud_server = NULL;
    char *ca_certificate = NULL;
    char *ssl_shared_key = NULL;
    char *ssl_private_key = NULL;

    while (1)
    {
        UBaseType_t water_mark = uxTaskGetStackHighWaterMark(NULL);
        TRACE_D("water_mark: %d", water_mark);

        // ulTaskNotifyTake(pdFALSE, portMAX_DELAY);

        // ezlopi_wait_for_wifi_to_connect();

        cloud_server = ezlopi_factory_info_v2_get_cloud_server();
        ca_certificate = ezlopi_factory_info_v2_get_ca_certificate();
        ssl_shared_key = ezlopi_factory_info_v2_get_ssl_shared_key();
        ssl_private_key = ezlopi_factory_info_v2_get_ssl_private_key();

        // TRACE_I("cloud_server: %s", cloud_server);
        // TRACE_I("ca_certificate: %s", ca_certificate);
        // TRACE_I("ssl_shared_key: %s", ssl_shared_key);
        // TRACE_I("ssl_private_key: %s", ssl_private_key);

        char http_request[128];
        snprintf(http_request, sizeof(http_request), "%s/getserver?json=true", cloud_server);
        s_ezlopi_http_data_t *response = malloc(sizeof(s_ezlopi_http_data_t));
        if (NULL != response)
        {

            response = ezlopi_http_get_request(http_request, ssl_private_key, ssl_shared_key, ca_certificate);
            if (response)
            {
                ws_endpoint = response->response;
                if (ws_endpoint)
                {
                    TRACE_D("ws_endpoint: %s", ws_endpoint); // {"uri": "wss://endpoint:port"}
                    TRACE_D("http_request: %s", http_request);
                    cJSON *root = cJSON_Parse(ws_endpoint);
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
                    free(ws_endpoint);
                }
            }

            free(response);
        }
        else
        {
            TRACE_E("Error : Memory allocation failed ");
            break;
        }

        vTaskDelay(2000 / portTICK_RATE_MS);
    }
    free(cloud_server);
    vTaskDelete(NULL);
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
        cJSON *cJSON_zwave_region_aary = cJSON_GetObjectItem(root_prov_data, "zwave_region");
        cJSON *cJSON_provision_server = cJSON_GetObjectItem(root_prov_data, "provision_server");
        cJSON *cJSON_cloud_server = cJSON_GetObjectItem(root_prov_data, "cloud_server");
        cJSON *cJSON_provision_token = cJSON_GetObjectItem(root_prov_data, "provision_token");
        cJSON *cJSON_provision_order = cJSON_GetObjectItem(root_prov_data, "provision_order");
        cJSON *cJSON_ssl_private_key = cJSON_GetObjectItem(root_prov_data, "ssl_private_key");
        cJSON *cJSON_ssl_public_key = NULL; // cJSON_GetObjectItem(root_prov_data, "ssl_public_key");
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
            config_check_factoryInfo->device_uuid = uuid;
        }
        else
        {
            config_check_factoryInfo->device_uuid = NULL;
        }

        if (NULL != cJSON_cloud_uuid)
        {
            const char *cloud_uuid = cJSON_cloud_uuid->valuestring;
            TRACE_I("cloud_uuid: %s", cloud_uuid);
        }
        else
        {
        }
        if (NULL != cJSON_order_uuid)
        {
            const char *order_uuid = cJSON_order_uuid->valuestring;
            TRACE_I("order_uuid: %s", order_uuid);
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
        if (NULL != cJSON_provision_server)
        {
            const char *provision_server = cJSON_provision_server->valuestring;
            TRACE_I("provision_server: %s", provision_server);
            config_check_factoryInfo->provision_server = provision_server;
        }
        else
        {
            config_check_factoryInfo->provision_server = NULL;
        }
        if (NULL != cJSON_cloud_server)
        {
            const char *cloud_server = cJSON_cloud_server->valuestring;
            TRACE_I("cloud_server: %s", cloud_server);
            config_check_factoryInfo->cloud_server = cloud_server;
        }
        else
        {
            config_check_factoryInfo->cloud_server = NULL;
        }

        if (NULL != cJSON_provision_token)
        {
            const char *provision_token = cJSON_provision_token->valuestring;
            TRACE_I("provision_token: %s", provision_token);
            config_check_factoryInfo->provision_token = provision_token;
        }
        else
        {
            config_check_factoryInfo->provision_token = NULL;
        }

        if (NULL != cJSON_provision_order)
        {
            const uint32_t provision_order = cJSON_provision_order->valueint;
            TRACE_I("provision_order: %d", provision_order);
        }
        if (NULL != cJSON_ssl_private_key)
        {
            const char *ssl_private_key = cJSON_ssl_private_key->valuestring;
            TRACE_I("ssl_private_key: %s", ssl_private_key);
            ezlopi_factory_info_v2_set_ssl_private_key(ssl_private_key);
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
            ezlopi_factory_info_v2_set_ssl_shared_key(ssl_shared_key);
        }

        if (NULL != cJSON_signing_ca_certificate)
        {
            const char *signing_ca_certificate = cJSON_signing_ca_certificate->valuestring;
            TRACE_I("signing_ca_certificate: %s", signing_ca_certificate);
            ezlopi_factory_info_v2_set_ca_cert(signing_ca_certificate);
        }

        config_check_factoryInfo->device_name = NULL;
        config_check_factoryInfo->manufacturer = NULL;
        config_check_factoryInfo->brand = NULL;
        config_check_factoryInfo->model_number = NULL;
        config_check_factoryInfo->device_type = NULL;
        config_check_factoryInfo->prov_uuid = NULL; // NULL since it is not

        if (ezlopi_factory_info_v2_set_basic(config_check_factoryInfo))
        {
            TRACE_I("Updated provisioning config");
            ret = 1;
        }
        else
        {
            TRACE_E("Error updating provisioning config");
        }

        free(config_check_factoryInfo);
        cJSON_free(root_prov_data);
        cJSON_free(cJSON_id);
        cJSON_free(cJSON_uuid);
        cJSON_free(cJSON_cloud_uuid);
        cJSON_free(cJSON_order_uuid);
        cJSON_free(cJSON_config_version);
        cJSON_free(cJSON_zwave_region_aary);
        cJSON_free(cJSON_provision_server);
        cJSON_free(cJSON_cloud_server);
        cJSON_free(cJSON_provision_token);
        cJSON_free(cJSON_provision_order);
        cJSON_free(cJSON_ssl_private_key);
        cJSON_free(cJSON_ssl_public_key);
        cJSON_free(cJSON_ssl_shared_key);
        cJSON_free(cJSON_signing_ca_certificate);
    }
    else
    {
        TRACE_E("Error parsing JSON.\n");
    }

    return ret;
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
    uint32_t idx = sizeof(method_list_v2) / sizeof(s_method_list_v2_t);

    while (idx--)
    {
        uint32_t request_method_name_len = strlen(method->valuestring);
        uint32_t list_method_name_len = strlen(method_list_v2[idx].method_name);
        uint32_t comp_len = list_method_name_len > request_method_name_len ? list_method_name_len : request_method_name_len;
        if (0 == strncmp(method->valuestring, method_list_v2[idx].method_name, comp_len))
        {
            found_method = 1;
            break;
        }
    }

    return (found_method ? idx : UINT32_MAX);
}

static void __call_method_func_and_send_response(cJSON *cj_request, cJSON *cj_method, f_method_func_t method_func, e_trace_type_t print_type)
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
                cJSON_AddNullToObject(cj_response, "error");

                method_func(cj_request, cj_response);

                web_provisioning_send_to_nma_websocket(cj_response, print_type);
                cJSON_Delete(cj_response);
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
    // if (payload && len)
    // {
    //     TRACE_D("payload:: len: %d, data: %.*s", len, len, payload);
    // }

    cJSON *cj_request = cJSON_ParseWithLength(payload, len);

    if (cj_request)
    {
        cJSON *cj_error = cJSON_GetObjectItem(cj_request, "error");
        cJSON *cj_method = cJSON_GetObjectItem(cj_request, "method");

        if ((NULL == cj_error) || (NULL == cj_error->valuestring) || (0 == strncmp(cj_error->valuestring, "null", 4)))
        {
            if ((NULL != cj_method) && (NULL != cj_method->valuestring))
            {
                TRACE_D("## WS Rx <<<<<<<<<< '%s'\r\n%.*s", (cj_method->valuestring ? cj_method->valuestring : ""), len, payload);
                uint32_t method_idx = __search_method_in_list(cj_method);
                if (UINT32_MAX != method_idx)
                {
                    __call_method_func_and_send_response(cj_request, cj_method, method_list_v2[method_idx].method, TRACE_TYPE_B);
                    __call_method_func_and_send_response(cj_request, cj_method, method_list_v2[method_idx].updater, TRACE_TYPE_B);
                }
                else
                {
                    __call_method_func_and_send_response(cj_request, cj_method, __rpc_method_notfound, TRACE_TYPE_E);
                }
            }
        }
        else
        {
            TRACE_E("## WS Rx <<<<<<<<<<'%s'\r\n%.*s", (cj_method->valuestring ? cj_method->valuestring : ""), len, payload);
        }

        cJSON_Delete(cj_request);
    }
}

static void __rpc_method_notfound(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON *cjson_error = cJSON_AddObjectToObject(cj_response, "error");
    if (cjson_error)
    {
        cJSON_AddNumberToObject(cjson_error, "code", -32602);
        cJSON_AddStringToObject(cjson_error, "data", "rpc.method.notfound");
        cJSON_AddStringToObject(cjson_error, "message", "Unknown method");
    }

    cJSON_AddObjectToObject(cj_response, ezlopi_result);
}

static void __hub_reboot(cJSON *cj_request, cJSON *cj_response)
{
    esp_restart();
    return NULL;
}