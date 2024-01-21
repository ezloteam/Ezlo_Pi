#include "string.h"
#include "time.h"

#include "cJSON.h"
#include "lwip/ip_addr.h"
#include "esp_event_base.h"
#include "mbedtls/base64.h"

#include "trace.h"

#include "ezlopi_nvs.h"
#include "ezlopi_core_wifi.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_ble_gatt.h"
#include "ezlopi_ble_auth.h"
#include "ezlopi_ble_buffer.h"
#include "ezlopi_ble_profile.h"
#include "ezlopi_ble_service.h"
#include "ezlopi_factory_info.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_cloud_constants.h"

#include "esp_gatt_common_api.h"

#define CJ_GET_STRING(name) cJSON_GetStringValue(cJSON_GetObjectItem(root, name))
#define CJ_GET_NUMBER(name) cJSON_GetNumberValue(cJSON_GetObjectItem(root, name))

static s_gatt_service_t *g_provisioning_service;
static s_linked_buffer_t *g_provisioning_linked_buffer = NULL;

static char *__provisioning_info_jsonify(void);
static char *__provisioning_info_base64(void);
static char *__base64_decode_provisioning_info(uint32_t total_size);

static void __provisioning_info_write_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);
static void __provisioning_info_read_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);

static void __provisioning_status_read_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);

#if 0
static void __process_provisioning_info(uint8_t *value, uint32_t len);
static void __provisioning_info_exec_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);
#endif

void ezlopi_ble_service_provisioning_init(void)
{
    esp_bt_uuid_t uuid;
    esp_gatt_perm_t permission;
    esp_gatt_char_prop_t properties;

    uuid.len = ESP_UUID_LEN_16;
    uuid.uuid.uuid16 = BLE_PROVISIONING_SERVICE_UUID;
    g_provisioning_service = ezlopi_ble_gatt_create_service(BLE_PROVISIONING_ID_HANDLE, &uuid);

    uuid.uuid.uuid16 = BLE_PROVISIONING_CHAR_UUID;
    uuid.len = ESP_UUID_LEN_16;
    permission = ESP_GATT_PERM_WRITE | ESP_GATT_PERM_READ;
    properties = ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY | ESP_GATT_CHAR_PROP_BIT_INDICATE;
    ezlopi_ble_gatt_add_characteristic(g_provisioning_service, &uuid, permission, properties, __provisioning_info_read_func, __provisioning_info_write_func, NULL); // reliable-write is not implemented for now

    uuid.uuid.uuid16 = BLE_PROVISIONING_STATUS_CHAR_UUID;
    uuid.len = ESP_UUID_LEN_16;
    permission = ESP_GATT_PERM_READ;
    properties = ESP_GATT_CHAR_PROP_BIT_READ;
    ezlopi_ble_gatt_add_characteristic(g_provisioning_service, &uuid, permission, properties, __provisioning_status_read_func, NULL, NULL);
}

static char *__provisioning_status_jsonify(void)
{
    char *prov_status_jstr = NULL;
    cJSON *root = cJSON_CreateObject();
    if (root)
    {
        uint32_t prov_stat = ezlopi_nvs_get_provisioning_status();
        if (1 == prov_stat)
        {
            cJSON_AddNumberToObject(root, ezlopi_version_str, ezlopi_factory_info_v3_get_version());
            cJSON_AddNumberToObject(root, ezlopi_status_str, prov_stat);
        }
        else
        {
            cJSON_AddNumberToObject(root, ezlopi_version_str, 0);
            cJSON_AddNumberToObject(root, ezlopi_status_str, 0);
        }

        char tmp_buffer[32];
        snprintf(tmp_buffer, sizeof(tmp_buffer), "%08x", ezlopi_nvs_config_info_version_number_get());
        cJSON_AddStringToObject(root, ezlopi_config_id_str, tmp_buffer);
        cJSON_AddNumberToObject(root, ezlopi_config_time_str, ezlopi_nvs_config_info_update_time_get());

        prov_status_jstr = cJSON_Print(root);
        cJSON_Delete(root);

        if (prov_status_jstr)
        {
            cJSON_Minify(prov_status_jstr);
        }
    }

    return prov_status_jstr;
}

static void __provisioning_status_read_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{
    if (value)
    {
        static char *prov_status_jstr;
        if (NULL == prov_status_jstr)
        {
            prov_status_jstr = __provisioning_status_jsonify();
        }

        if (NULL != prov_status_jstr)
        {
            uint32_t total_data_len = strlen(prov_status_jstr);
            uint32_t max_data_buffer_size = ezlopi_ble_gatt_get_max_data_size();
            uint32_t copy_size = ((total_data_len - param->read.offset) < max_data_buffer_size) ? (total_data_len - param->read.offset) : max_data_buffer_size;

            if ((0 != total_data_len) && (total_data_len > param->read.offset))
            {
                strncpy((char *)value->value, prov_status_jstr + param->read.offset, copy_size);
                value->len = copy_size;
            }
            else
            {
                value->len = 1;
                value->value[0] = 0; // Read 0 if the device not provisioned yet.
            }

            if ((param->read.offset + copy_size) >= total_data_len)
            {
                free(prov_status_jstr);
                prov_status_jstr = NULL;
            }
        }
        else
        {
            TRACE_E("Unable to create json string");
            value->len = 1;
            value->value[0] = 0; // Read 0 if the device not provisioned yet.
        }
    }
    else
    {
        TRACE_E("VALUE IS NULL");
    }
}

static void __provisioning_info_write_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{
    TRACE_D("Write function called!");
    TRACE_D("GATT_WRITE_EVT value: %.*s", param->write.len, param->write.value);

    ezlopi_nvs_set_provisioning_status();

    if (NULL == g_provisioning_linked_buffer)
    {
        g_provisioning_linked_buffer = ezlopi_ble_buffer_create(param);
    }
    else
    {
        ezlopi_ble_buffer_add_to_buffer(g_provisioning_linked_buffer, param);
    }

    if (g_provisioning_linked_buffer)
    {
        if ((NULL != param->write.value) && (param->write.len > 0))
        {
            cJSON *root = cJSON_ParseWithLength((const char *)param->write.value, param->write.len);
            if (root)
            {

                uint32_t len = CJ_GET_NUMBER(ezlopi_len_str);
                uint32_t tot_len = CJ_GET_NUMBER(ezlopi_total_len_str);
                uint32_t sequence = CJ_GET_NUMBER(ezlopi_sequence_str);

                TRACE_D("Len: %d", len);
                TRACE_D("tot_len: %d", tot_len);
                TRACE_D("sequence: %d", sequence);

                if (sequence && len && tot_len)
                {
                    if (((sequence - 1) * 400 + len) >= tot_len)
                    {
                        char *decoded_data = __base64_decode_provisioning_info(tot_len); // uncommente f
                        if (decoded_data)
                        {
                            cJSON *cj_config = cJSON_Parse(decoded_data);
                            if (cj_config)
                            {
                                char *user_id = NULL;
                                CJSON_GET_VALUE_STRING(cj_config, ezlopi_user_id_str, user_id);

                                if (user_id && (BLE_AUTH_SUCCESS == ezlopi_ble_auth_check_user_id(user_id)))
                                {
                                    s_basic_factory_info_t *ezlopi_config_basic = malloc(sizeof(s_basic_factory_info_t));
                                    if (ezlopi_config_basic)
                                    {
                                        // ezlopi_config_basic->user_id = user_id;
                                        memset(ezlopi_config_basic, 0, sizeof(s_basic_factory_info_t));

                                        CJSON_GET_VALUE_STRING(cj_config, ezlopi_device_name_str, ezlopi_config_basic->device_name);
                                        CJSON_GET_VALUE_STRING(cj_config, ezlopi_manufacturer_name_str, ezlopi_config_basic->manufacturer);
                                        CJSON_GET_VALUE_STRING(cj_config, ezlopi_brand_str, ezlopi_config_basic->brand);
                                        CJSON_GET_VALUE_STRING(cj_config, ezlopi_model_number_str, ezlopi_config_basic->model_number);
                                        CJSON_GET_VALUE_DOUBLE(cj_config, ezlopi_serial_str, ezlopi_config_basic->id);
                                        CJSON_GET_VALUE_STRING(cj_config, ezlopi_uuid_str, ezlopi_config_basic->device_uuid);
                                        CJSON_GET_VALUE_STRING(cj_config, ezlopi_uuid_provisioning_str, ezlopi_config_basic->prov_uuid);

                                        CJSON_GET_VALUE_STRING(cj_config, ezlopi_mac_str, ezlopi_config_basic->device_mac);

                                        CJSON_GET_VALUE_STRING(cj_config, ezlopi_provision_server_str, ezlopi_config_basic->provision_server);
                                        CJSON_GET_VALUE_STRING(cj_config, ezlopi_cloud_server_str, ezlopi_config_basic->cloud_server);
                                        CJSON_GET_VALUE_STRING(cj_config, ezlopi_provision_token_str, ezlopi_config_basic->provision_token);
                                        CJSON_GET_VALUE_STRING(cj_config, ezlopi_device_type_ezlopi_str, ezlopi_config_basic->device_type);

                                        ezlopi_factory_info_v3_set_basic(ezlopi_config_basic);
                                        uint32_t version_no = ezlopi_nvs_config_info_version_number_get() + 1;
                                        ezlopi_nvs_config_info_version_number_set(version_no);

                                        time_t now;
                                        time(&now);
                                        TRACE_D("time now{size: %u}: %lu", sizeof(time_t), now);
                                        ezlopi_nvs_config_info_update_time_set(now);

                                        free(ezlopi_config_basic);
                                    }

                                    char *ca_certs = NULL;
                                    char *ssl_shared_key = NULL;
                                    char *ssl_private_key = NULL;

                                    CJSON_GET_VALUE_STRING(cj_config, ezlopi_ssl_private_key_str, ssl_private_key);
                                    // CJSON_GET_VALUE_STRING(cj_config, "ssl_public_key", ssl_public_key);
                                    CJSON_GET_VALUE_STRING(cj_config, ezlopi_ssl_shared_key_str, ssl_shared_key);
                                    CJSON_GET_VALUE_STRING(cj_config, ezlopi_signing_ca_certificate_str, ca_certs);

                                    ezlopi_factory_info_v3_set_ca_cert(ca_certs);
                                    ezlopi_factory_info_v3_set_ssl_shared_key(ssl_shared_key);
                                    ezlopi_factory_info_v3_set_ssl_private_key(ssl_private_key);
                                    // ezlopi_factory_info_v3_set_ssl_public_key(ssl_public_key);
                                }
                                else
                                {
                                    TRACE_E("User varification failed!");

                                    char *curr_user_id = ezlopi_nvs_read_user_id_str();
                                    if (curr_user_id)
                                    {
                                        TRACE_D("current user: %s", curr_user_id);
                                        free(curr_user_id);
                                    }
                                }

                                cJSON_Delete(cj_config);
                            }

                            free(decoded_data);
                        }

                        ezlopi_ble_buffer_free_buffer(g_provisioning_linked_buffer);
                        g_provisioning_linked_buffer = NULL;
                    }
                }
            }
        }
    }
}

static void __provisioning_info_read_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{
    TRACE_D("Read function called!");

    static char *g_provisioning_info_base64;
    static uint32_t g_provisioning_sequence_no;
    static time_t g_provisioning_last_read_time;
    static uint32_t g_provisioning_number_of_sequence;

    static const time_t gc_provisioning_read_timeout_s = 2;
    static const uint32_t g_required_ble_prov_buffer_size = 517;

    // timeout logic
    int status = -1; // success for non negative, failed for negative
    time_t time_now = 0;
    time(&time_now);
    if ((time_now - g_provisioning_last_read_time) >= gc_provisioning_read_timeout_s)
    {
        g_provisioning_sequence_no = 0;
    }

    time(&g_provisioning_last_read_time);

    if (value)
    {
        if (NULL == g_provisioning_info_base64)
        {
            g_provisioning_info_base64 = __provisioning_info_base64();

            g_provisioning_sequence_no = 0;
            g_provisioning_number_of_sequence = strlen(g_provisioning_info_base64) / ezlopi_ble_gatt_get_max_data_size();
            g_provisioning_number_of_sequence = (strlen(g_provisioning_info_base64) % ezlopi_ble_gatt_get_max_data_size()) ? (g_provisioning_number_of_sequence + 1) : g_provisioning_number_of_sequence;
        }

        if (NULL != g_provisioning_info_base64)
        {
            if (ezlopi_ble_gatt_get_max_data_size() >= g_required_ble_prov_buffer_size)
            {

                uint32_t total_data_len = strlen(g_provisioning_info_base64);
                uint32_t copy_size = total_data_len - (g_provisioning_sequence_no * 400);
                copy_size = (copy_size > 400) ? 400 : copy_size;

                TRACE_B("copy_size: %d", copy_size);
                TRACE_B("total_data_len: %d", total_data_len);

                cJSON *cj_response = cJSON_CreateObject();
                if (cj_response)
                {
                    static char data_buffer[400 + 1];
                    snprintf(data_buffer, sizeof(data_buffer), "%.*s", copy_size, g_provisioning_info_base64 + (g_provisioning_sequence_no * 400));

                    cJSON_AddNumberToObject(cj_response, ezlopi_len_str, copy_size);
                    cJSON_AddNumberToObject(cj_response, ezlopi_total_len_str, total_data_len);
                    cJSON_AddNumberToObject(cj_response, ezlopi_sequence_str, g_provisioning_sequence_no);
                    cJSON_AddStringToObject(cj_response, ezlopi_data_str, data_buffer);

                    char *send_data = cJSON_Print(cj_response);
                    if (send_data)
                    {
                        TRACE_D("data: %s", send_data);
                        cJSON_Minify(send_data);

                        if ((0 != total_data_len) && (total_data_len >= ((g_provisioning_sequence_no * 400) + copy_size)))
                        {
                            value->len = strlen(send_data);
                            strncpy((char *)value->value, send_data, value->len + 1);

                            TRACE_B("data: %s", (char *)value->value);

                            g_provisioning_sequence_no += 1;
                            status = 0;

                            if (copy_size < 400) // Done reading
                            {
                                status = 1; // non negative for done reading
                                free(g_provisioning_info_base64);
                                g_provisioning_info_base64 = NULL;
                            }
                        }
                        else
                        {
                            TRACE_W("Check value: %d", ((g_provisioning_sequence_no * 400) + copy_size));
                            TRACE_W("total_data_len: %d", total_data_len);
                            status = -4;
                        }

                        free(send_data);
                    }
                    else
                    {
                        status = -3;
                    }

                    cJSON_Delete(cj_response);
                }
                else
                {
                    status = -2;
                }
            }
            else
            {
                TRACE_E("MTU size must be greater than %d!", g_required_ble_prov_buffer_size);
                TRACE_W("call SET-MTU API from client stack!");

                CHECK_PRINT_ERROR(esp_ble_gatt_set_local_mtu(517), "set local  MTU failed");
                status = -2;
            }
        }

        if (status)
        {
            if (status < 0)
            {
                TRACE_E("Error found: %d", status);
                value->len = 1;
                value->value[0] = 0; // Read 0 if the device not provisioned yet.
            }
            else
            {
                TRACE_I("Done Transmitting.");
            }

            g_provisioning_sequence_no = 0;
            g_provisioning_number_of_sequence = 0;

            if (g_provisioning_info_base64)
            {
                free(g_provisioning_info_base64);
                g_provisioning_info_base64 = NULL;
            }
        }
    }
    else
    {
        TRACE_E("VALUE IS NULL");
    }
}

#if 0
static void __provisioning_info_exec_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{
    if (g_provisioning_linked_buffer)
    {
        TRACE_D("Write execute function called.");
        ezlopi_ble_buffer_accumulate_to_start(g_provisioning_linked_buffer);
        __process_provisioning_info(g_provisioning_linked_buffer->buffer, g_provisioning_linked_buffer->len);
        ezlopi_ble_buffer_free_buffer(g_provisioning_linked_buffer);
        g_provisioning_linked_buffer = NULL;
    }
}

static void __process_provisioning_info(uint8_t *value, uint32_t len)
{
    if ((NULL != value) && (len > 0))
    {
        cJSON *root = cJSON_Parse((const char *)value);
        if (root)
        {
            TRACE_D("value = %s", value);
            char *user_id = cJSON_GetObjectItemCaseSensitive(root, ezlopi_user_id_str)->valuestring;
            char *device_name = cJSON_GetObjectItemCaseSensitive(root, ezlopi_device_name_str)->valuestring;
            char *brand = cJSON_GetObjectItemCaseSensitive(root, ezlopi_brand_str)->valuestring;
            char *manufacturer_name = cJSON_GetObjectItemCaseSensitive(root, ezlopi_manufacturer_name_str)->valuestring;
            char *model_number = cJSON_GetObjectItemCaseSensitive(root, ezlopi_model_number_str)->valuestring;
            char *uuid = cJSON_GetObjectItemCaseSensitive(root, ezlopi_uuid_str)->valuestring;
            char *uuid_provisioning = cJSON_GetObjectItemCaseSensitive(root, ezlopi_uuid_provisioning_str)->valuestring;
            double serial = cJSON_GetObjectItemCaseSensitive(root, ezlopi_serial_str)->valuedouble;
            char *cloud_server = cJSON_GetObjectItemCaseSensitive(root, ezlopi_cloud_server_str)->valuestring;
            char *ssl_private_key = cJSON_GetObjectItemCaseSensitive(root, ezlopi_ssl_private_key_str)->valuestring;
            char *ssl_public_key = cJSON_GetObjectItemCaseSensitive(root, "ssl_public_key")->valuestring;
            char *ca_cert = cJSON_GetObjectItemCaseSensitive(root, ezlopi_ca_cert_str)->valuestring;
            char *device_type_ezlopi = cJSON_GetObjectItemCaseSensitive(root, ezlopi_device_type_ezlopi_str)->valuestring;

            TRACE_D("************************* BLE-PROVISIONING *************************");
            TRACE_D("user_id:               %s", user_id ? user_id : ezlopi__str);
            TRACE_D("device_name:           %s", device_name ? device_name : ezlopi__str);
            TRACE_D("brand:                 %s", brand ? brand : ezlopi__str);
            TRACE_D("manufacturer_name:     %s", manufacturer_name ? manufacturer_name : ezlopi__str);
            TRACE_D("model_number:          %s", model_number ? model_number : ezlopi__str);
            TRACE_D("uuid:                  %s", uuid ? uuid : ezlopi__str);
            TRACE_D("uuid_provisioning:     %s", uuid_provisioning ? uuid_provisioning : ezlopi__str);
            TRACE_D("serial:                %f", serial);
            TRACE_D("cloud_server:          %s", cloud_server ? cloud_server : ezlopi__str);
            TRACE_D("ssl_private_key:       %s", ssl_private_key ? ssl_private_key : ezlopi__str);
            TRACE_D("ssl_public_key:        %s", ssl_public_key ? ssl_public_key : ezlopi__str);
            TRACE_D("ca_cert:               %s", ca_cert ? ca_cert : ezlopi__str);
            TRACE_D("device_type_ezlopi:    %s", device_type_ezlopi ? device_type_ezlopi : ezlopi__str);
            TRACE_D("********************************************************************");

            cJSON_Delete(root);
        }
        else
        {
            TRACE_E("Invalid json packet received!");
        }
    }
}
#endif

static char *__base64_decode_provisioning_info(uint32_t total_size)
{
    char *decoded_config_json = NULL;
    char *base64_buffer = malloc(total_size + 1);

    if (base64_buffer)
    {
        uint32_t pos = 0;
        s_linked_buffer_t *tmp_prov_buffer = g_provisioning_linked_buffer;

        while (tmp_prov_buffer)
        {
            // TRACE_W("tmp_prov_buffer->buffer[%d]: %.*s", tmp_prov_buffer->len, tmp_prov_buffer->len, (char *)tmp_prov_buffer->buffer);
            cJSON *root = cJSON_ParseWithLength((const char *)tmp_prov_buffer->buffer, tmp_prov_buffer->len);
            if (root)
            {
                uint32_t len = CJ_GET_NUMBER(ezlopi_len_str);
                // uint32_t tot_len = CJ_GET_NUMBER(ezlopi_total_len_str);
                // uint32_t sequence = CJ_GET_NUMBER(ezlopi_sequence_str);
                char *data = CJ_GET_STRING(ezlopi_data_str);
                if (data)
                {
                    memcpy(base64_buffer + pos, data, len);
                    pos += len;
                    base64_buffer[pos] = '\0';
                }
                else
                {
                    TRACE_E("DATA IS NULL");
                }
            }
            else
            {
                TRACE_E("Failed to parse");
            }

            tmp_prov_buffer = tmp_prov_buffer->next;
        }

        TRACE_D("base64_buffer: %s", base64_buffer);

        decoded_config_json = malloc(total_size);
        if (decoded_config_json)
        {
            size_t o_len = 0;
            bzero(decoded_config_json, total_size);
            mbedtls_base64_decode((uint8_t *)decoded_config_json, (size_t)total_size, &o_len, (uint8_t *)base64_buffer, strlen(base64_buffer));
            TRACE_D("Decoded data: %s", decoded_config_json);
        }
        else
        {
            TRACE_E("mALLOC FAILED");
        }

        free(base64_buffer);
    }

    return decoded_config_json;
}

static char *__provisioning_info_jsonify(void)
{
    char *str_json_prov_info = NULL;

    cJSON *cj_prov_info = cJSON_CreateObject();
    if (cj_prov_info)
    {
        char tmp_buffer[32];
        char *device_name = ezlopi_factory_info_v3_get_name();
        char *brand = ezlopi_factory_info_v3_get_brand();
        char *manufacturer_name = ezlopi_factory_info_v3_get_manufacturer();
        char *model_number = ezlopi_factory_info_v3_get_model();
        char *uuid = ezlopi_factory_info_v3_get_device_uuid();
        char *uuid_provisioning = ezlopi_factory_info_v3_get_provisioning_uuid();
        char *cloud_server = ezlopi_factory_info_v3_get_cloud_server();
        char *ssl_private_key = ezlopi_factory_info_v3_get_ssl_private_key();
        char *ssl_shared_key = ezlopi_factory_info_v3_get_ssl_shared_key();
        char *ca_cert = ezlopi_factory_info_v3_get_ca_certificate();

        snprintf(tmp_buffer, sizeof(tmp_buffer), "%08x", ezlopi_nvs_config_info_version_number_get());
        cJSON_AddStringToObject(cj_prov_info, ezlopi_config_id_str, tmp_buffer);
        cJSON_AddNumberToObject(cj_prov_info, ezlopi_config_time_str, ezlopi_nvs_config_info_update_time_get());
        cJSON_AddStringToObject(cj_prov_info, ezlopi_device_name_str, device_name);
        cJSON_AddStringToObject(cj_prov_info, ezlopi_brand_str, brand);
        cJSON_AddStringToObject(cj_prov_info, ezlopi_manufacturer_name_str, manufacturer_name);
        cJSON_AddStringToObject(cj_prov_info, ezlopi_model_number_str, model_number);
        cJSON_AddStringToObject(cj_prov_info, ezlopi_uuid_str, uuid);
        cJSON_AddStringToObject(cj_prov_info, ezlopi_uuid_provisioning_str, uuid_provisioning);
        cJSON_AddNumberToObject(cj_prov_info, ezlopi_serial_str, ezlopi_factory_info_v3_get_id());
        cJSON_AddStringToObject(cj_prov_info, ezlopi_cloud_server_str, cloud_server);
        cJSON_AddStringToObject(cj_prov_info, ezlopi_ssl_private_key_str, ssl_private_key);
        cJSON_AddStringToObject(cj_prov_info, ezlopi_ssl_shared_key_str, ssl_shared_key);
        cJSON_AddStringToObject(cj_prov_info, ezlopi_ca_cert_str, ca_cert);
        cJSON_AddStringToObject(cj_prov_info, ezlopi_device_type_ezlopi_str, ezlopi_factory_info_v3_get_device_type());

        ezlopi_factory_info_v3_free(device_name);
        ezlopi_factory_info_v3_free(brand);
        ezlopi_factory_info_v3_free(manufacturer_name);
        ezlopi_factory_info_v3_free(model_number);
        ezlopi_factory_info_v3_free(uuid);
        ezlopi_factory_info_v3_free(uuid_provisioning);
        ezlopi_factory_info_v3_free(cloud_server);
        ezlopi_factory_info_v3_free(ssl_private_key);
        ezlopi_factory_info_v3_free(ssl_shared_key);
        ezlopi_factory_info_v3_free(ca_cert);

        str_json_prov_info = cJSON_Print(cj_prov_info);
        cJSON_Delete(cj_prov_info);

        if (str_json_prov_info)
        {
            cJSON_Minify(str_json_prov_info);
        }
    }

    return str_json_prov_info;
}

static char *__provisioning_info_base64(void)
{
    const uint32_t base64_data_len = 5120;
    char *base64_data = malloc(base64_data_len);
    if (base64_data)
    {
        uint32_t out_put_len = 0;
        char *str_provisioning_data = __provisioning_info_jsonify();
        if (str_provisioning_data)
        {
            TRACE_D("str_provisioning_data[len: %d]: %s", strlen(str_provisioning_data), str_provisioning_data);

            int ret = mbedtls_base64_encode((unsigned char *)base64_data, base64_data_len, &out_put_len, (const unsigned char *)str_provisioning_data, strlen(str_provisioning_data));

            TRACE_D("'mbedtls_base64_encode' returned: %04x", ret);
            free(str_provisioning_data);
        }

        if (0 == out_put_len)
        {
            free(base64_data);
            base64_data = NULL;
        }

        TRACE_D("out-put-len: %d", out_put_len);
        TRACE_D("base64_data[len: %d]: %s", strlen(base64_data), base64_data);
    }

    return base64_data;
}
