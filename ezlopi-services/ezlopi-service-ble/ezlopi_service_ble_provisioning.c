
#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_BLE_ENABLE

#include <string.h>
#include <time.h>

#include "cjext.h"
#include "lwip/ip_addr.h"
#include "esp_event_base.h"
#include "mbedtls/base64.h"
#include "esp_gatt_common_api.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_wifi.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_ble_gatt.h"
#include "ezlopi_core_ble_buffer.h"
#include "ezlopi_core_ble_profile.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_event_group.h"

#include "ezlopi_cloud_constants.h"

#include "ezlopi_service_ble_ble_auth.h"
#include "ezlopi_service_ble.h"
#include "EZLOPI_USER_CONFIG.h"
#include "ezlopi_core_buffer.h"

#define CJ_GET_STRING(name) cJSON_GetStringValue(cJSON_GetObjectItem(__FUNCTION__, root, name))
#define CJ_GET_NUMBER(name) cJSON_GetNumberValue(cJSON_GetObjectItem(__FUNCTION__, root, name))
static s_gatt_service_t *g_provisioning_service;
static s_linked_buffer_t *g_provisioning_linked_buffer = NULL;

#ifdef EZPI_SERV_BLE_ENABLE_READ_PROV
static char *__provisioning_info_jsonify(void);
static char *__provisioning_info_base64(void);
static void __provisioning_info_read_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);
#endif // EZPI_SERV_BLE_ENABLE_READ_PROV

#ifdef EZPI_SERV_BLE_ENABLE_STAT_PROV
static void __provisioning_status_read_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);
#endif // EZPI_SERV_BLE_ENABLE_STAT_PROV

static void __provisioning_info_write_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);
static char *__base64_decode_provisioning_info(uint32_t total_size);

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

#ifdef EZPI_SERV_BLE_ENABLE_READ_PROV
    permission = ESP_GATT_PERM_WRITE | ESP_GATT_PERM_READ;
    properties = ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY | ESP_GATT_CHAR_PROP_BIT_INDICATE;
    ezlopi_ble_gatt_add_characteristic(g_provisioning_service, &uuid, permission, properties, __provisioning_info_read_func, __provisioning_info_write_func, NULL); // reliable-write is not implemented for now
#else                                                                                                                                                               // EZPI_SERV_BLE_ENABLE_READ_PROV
    permission = ESP_GATT_PERM_WRITE;
    properties = ESP_GATT_CHAR_PROP_BIT_WRITE;
    ezlopi_ble_gatt_add_characteristic(g_provisioning_service, &uuid, permission, properties, NULL, __provisioning_info_write_func, NULL); // reliable-write is not implemented for now
#endif                                                                                                                                                              // EZPI_SERV_BLE_ENABLE_READ_PROV

#ifdef EZPI_SERV_BLE_ENABLE_STAT_PROV
    uuid.uuid.uuid16 = BLE_PROVISIONING_STATUS_CHAR_UUID;
    uuid.len = ESP_UUID_LEN_16;
    permission = ESP_GATT_PERM_READ;
    properties = ESP_GATT_CHAR_PROP_BIT_READ;
    ezlopi_ble_gatt_add_characteristic(g_provisioning_service, &uuid, permission, properties, __provisioning_status_read_func, NULL, NULL);
#endif // EZPI_SERV_BLE_ENABLE_STAT_PROV
}

#ifdef EZPI_SERV_BLE_ENABLE_STAT_PROV
static char *__provisioning_status_jsonify(void)
{
    char *prov_status_jstr = NULL;
    cJSON *root = cJSON_CreateObject(__FUNCTION__);
    if (root)
    {
        uint32_t prov_stat = ezlopi_factory_info_v3_get_provisioning_status();
        if (1 == prov_stat)
        {
            cJSON_AddNumberToObject(__FUNCTION__, root, ezlopi_version_str, ezlopi_factory_info_v3_get_version());
            cJSON_AddNumberToObject(__FUNCTION__, root, ezlopi_status_str, prov_stat);
        }
        else
        {
            cJSON_AddNumberToObject(__FUNCTION__, root, ezlopi_version_str, 0);
            cJSON_AddNumberToObject(__FUNCTION__, root, ezlopi_status_str, 0);
        }

        char tmp_buffer[32];
        snprintf(tmp_buffer, sizeof(tmp_buffer), "%08x", ezlopi_nvs_config_info_version_number_get());
        cJSON_AddStringToObject(__FUNCTION__, root, ezlopi_config_id_str, tmp_buffer);
        cJSON_AddNumberToObject(__FUNCTION__, root, ezlopi_config_time_str, ezlopi_nvs_config_info_update_time_get());

        prov_status_jstr = cJSON_PrintBuffered(__FUNCTION__, root, 256, false);

        cJSON_Delete(__FUNCTION__, root);
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
                ezlopi_free(__FUNCTION__, prov_status_jstr);
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

#endif // EZPI_SERV_BLE_ENABLE_READ_PROV

static void __provisioning_info_write_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{
    // TRACE_D("Write function called!");

    TRACE_D("GATT_WRITE_EVT value: %.*s", param->write.len, param->write.value);

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
            cJSON *root = cJSON_ParseWithLength(__FUNCTION__, (const char *)param->write.value, param->write.len);
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
                            cJSON *cj_config = cJSON_ParseWithRef(__FUNCTION__, decoded_data);

                            if (cj_config)
                            {
                                char user_id[32];
                                CJSON_GET_VALUE_STRING_BY_COPY(cj_config, ezlopi_user_id_str, user_id);

                                if (BLE_AUTH_SUCCESS == ezlopi_ble_auth_check_user_id(user_id))
                                {
                                    s_basic_factory_info_t *ezlopi_config_basic = ezlopi_malloc(__FUNCTION__, sizeof(s_basic_factory_info_t));
                                    if (ezlopi_config_basic)
                                    {
                                        memset(ezlopi_config_basic, 0, sizeof(s_basic_factory_info_t));

                                        char device_name[32];
                                        char manufacturer[32];
                                        char brand[32];
                                        char model_number[32];
                                        char device_uuid[40];
                                        char prov_uuid[40];
                                        char device_mac[32];
                                        char provision_server[128];
                                        char cloud_server[128];
                                        char provision_toke[256];
                                        char device_type[32];

                                        memset(device_name, 0, sizeof(device_name));
                                        memset(manufacturer, 0, sizeof(manufacturer));
                                        memset(brand, 0, sizeof(brand));
                                        memset(model_number, 0, sizeof(model_number));
                                        memset(device_uuid, 0, sizeof(device_uuid));
                                        memset(prov_uuid, 0, sizeof(prov_uuid));
                                        memset(device_mac, 0, sizeof(device_mac));
                                        memset(provision_server, 0, sizeof(provision_server));
                                        memset(cloud_server, 0, sizeof(cloud_server));
                                        memset(provision_toke, 0, sizeof(provision_toke));
                                        memset(device_type, 0, sizeof(device_type));

                                        CJSON_GET_VALUE_DOUBLE(cj_config, ezlopi_serial_str, ezlopi_config_basic->id);
                                        CJSON_GET_VALUE_DOUBLE(cj_config, ezlopi_version_str, ezlopi_config_basic->config_version);

                                        CJSON_GET_VALUE_STRING_BY_COPY(cj_config, ezlopi_device_name_str, device_name);
                                        CJSON_GET_VALUE_STRING_BY_COPY(cj_config, ezlopi_manufacturer_name_str, manufacturer);
                                        CJSON_GET_VALUE_STRING_BY_COPY(cj_config, ezlopi_brand_str, brand);
                                        CJSON_GET_VALUE_STRING_BY_COPY(cj_config, ezlopi_model_number_str, model_number);
                                        CJSON_GET_VALUE_STRING_BY_COPY(cj_config, ezlopi_uuid_str, device_uuid);
                                        CJSON_GET_VALUE_STRING_BY_COPY(cj_config, ezlopi_uuid_provisioning_str, prov_uuid);
                                        CJSON_GET_VALUE_STRING_BY_COPY(cj_config, ezlopi_mac_str, device_mac);
                                        CJSON_GET_VALUE_STRING_BY_COPY(cj_config, ezlopi_provision_server_str, provision_server);
                                        CJSON_GET_VALUE_STRING_BY_COPY(cj_config, ezlopi_cloud_server_str, cloud_server);
                                        CJSON_GET_VALUE_STRING_BY_COPY(cj_config, ezlopi_provision_token_str, provision_toke);
                                        CJSON_GET_VALUE_STRING_BY_COPY(cj_config, ezlopi_device_type_ezlopi_str, device_type);

                                        ezlopi_config_basic->device_name = device_name;
                                        ezlopi_config_basic->manufacturer = manufacturer;
                                        ezlopi_config_basic->brand = brand;
                                        ezlopi_config_basic->model_number = model_number;
                                        ezlopi_config_basic->device_uuid = device_uuid;
                                        ezlopi_config_basic->prov_uuid = prov_uuid;
                                        ezlopi_config_basic->device_mac = device_mac;
                                        ezlopi_config_basic->provision_server = provision_server;
                                        ezlopi_config_basic->cloud_server = cloud_server;
                                        ezlopi_config_basic->provision_token = provision_toke;
                                        ezlopi_config_basic->device_type = device_type;

                                        ezlopi_factory_info_v3_set_basic(ezlopi_config_basic);
                                        uint32_t version_no = ezlopi_nvs_config_info_version_number_get() + 1;
                                        ezlopi_nvs_config_info_version_number_set(version_no);

                                        time_t now;
                                        time(&now);
                                        TRACE_D("time now{size: %u}: %lu", sizeof(time_t), now);
                                        ezlopi_nvs_config_info_update_time_set(now);

                                        ezlopi_free(__FUNCTION__, ezlopi_config_basic);
                                    }

                                    ezlopi_factory_info_v3_set_ca_cert(cJSON_GetObjectItem(__FUNCTION__, cj_config, ezlopi_signing_ca_certificate_str));
                                    ezlopi_factory_info_v3_set_ssl_shared_key(cJSON_GetObjectItem(__FUNCTION__, cj_config, ezlopi_ssl_shared_key_str));
                                    ezlopi_factory_info_v3_set_ssl_private_key(cJSON_GetObjectItem(__FUNCTION__, cj_config, ezlopi_ssl_private_key_str));
                                }
                                else
                                {
                                    TRACE_E("User verification failed!");

                                    char *curr_user_id = ezlopi_nvs_read_user_id_str();
                                    if (curr_user_id)
                                    {
                                        TRACE_D("current user: %s", curr_user_id);
                                        ezlopi_free(__FUNCTION__, curr_user_id);
                                    }
                                }

                                cJSON_Delete(__FUNCTION__, cj_config);
                            }

                            ezlopi_free(__FUNCTION__, decoded_data);
                            decoded_data = NULL;
                        }

                        ezlopi_ble_buffer_free_buffer(g_provisioning_linked_buffer);
                        g_provisioning_linked_buffer = NULL;
                    }
                }
            }
        }
    }
}

#ifdef EZPI_SERV_BLE_ENABLE_READ_PROV
static void __provisioning_info_read_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{
    // TRACE_D("Read function called!");

    static const uint32_t _data_size = 400;
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
                uint32_t copy_size = total_data_len - (g_provisioning_sequence_no * _data_size);
                copy_size = (copy_size > _data_size) ? _data_size : copy_size;

                cJSON *cj_response = cJSON_CreateObject(__FUNCTION__);
                if (cj_response)
                {
                    char data_buffer[_data_size + 4];
                    snprintf(data_buffer, sizeof(data_buffer), "%.*s", copy_size, g_provisioning_info_base64 + (g_provisioning_sequence_no * _data_size));

                    cJSON_AddNumberToObject(__FUNCTION__, cj_response, ezlopi_len_str, copy_size);
                    cJSON_AddNumberToObject(__FUNCTION__, cj_response, ezlopi_total_len_str, total_data_len);
                    cJSON_AddNumberToObject(__FUNCTION__, cj_response, ezlopi_sequence_str, g_provisioning_sequence_no);
                    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_data_str, data_buffer);

                    const uint32_t buffer_len = 512;
                    char json_to_str_buffer[buffer_len];
                    memset(json_to_str_buffer, 0, buffer_len);
                    status = cJSON_PrintPreallocated(__FUNCTION__, cj_response, json_to_str_buffer, buffer_len, false);
                    cJSON_Delete(__FUNCTION__, cj_response);

                    if (true == status)
                    {
                        TRACE_D("data: %s", json_to_str_buffer);

                        if ((0 != total_data_len) && (total_data_len >= ((g_provisioning_sequence_no * _data_size) + copy_size)))
                        {
                            value->len = strlen(json_to_str_buffer);
                            strncpy((char *)value->value, json_to_str_buffer, value->len + 1);

                            TRACE_I("data: %s", (char *)value->value);

                            g_provisioning_sequence_no += 1;
                            status = 0;

                            if (copy_size < _data_size) // Done reading
                            {
                                status = 1; // non negative for done reading
                                ezlopi_free(__FUNCTION__, g_provisioning_info_base64);
                                g_provisioning_info_base64 = NULL;
                            }
                        }
                        else
                        {
                            TRACE_W("Check value: %d", ((g_provisioning_sequence_no * _data_size) + copy_size));
                            TRACE_W("total_data_len: %d", total_data_len);
                            status = -4;
                        }
                    }
                    else
                    {
                        status = -3;
                    }
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

                CHECK_PRINT_ERROR(esp_ble_gatt_set_local_mtu(g_required_ble_prov_buffer_size), "set local  MTU failed");
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
                TRACE_S("Done Transmitting.");
            }

            g_provisioning_sequence_no = 0;
            g_provisioning_number_of_sequence = 0;

            if (g_provisioning_info_base64)
            {
                ezlopi_free(__FUNCTION__, g_provisioning_info_base64);
                g_provisioning_info_base64 = NULL;
            }
        }
    }
    else
    {
        TRACE_E("VALUE IS NULL");
    }
}

#endif // EZPI_SERV_BLE_ENABLE_READ_PROV

static char *__base64_decode_provisioning_info(uint32_t total_size)
{
    char *decoded_config_json = NULL;
    char *base64_buffer = ezlopi_malloc(__FUNCTION__, total_size + 1);

    TRACE_W("tatal data length: %d", total_size);

    if (base64_buffer)
    {
        uint32_t pos = 0;
        s_linked_buffer_t *tmp_prov_buffer = g_provisioning_linked_buffer;

        while (tmp_prov_buffer)
        {
            cJSON *root = cJSON_ParseWithLength(__FUNCTION__, (const char *)tmp_prov_buffer->buffer, tmp_prov_buffer->len);
            if (root)
            {
                uint32_t len = CJ_GET_NUMBER(ezlopi_len_str);
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

        uint32_t buffer_len = 0;
        decoded_config_json = ezlopi_malloc(__FUNCTION__, total_size);
        // decoded_config_json = ezlopi_core_buffer_acquire(__FUNCTION__, &buffer_len, 5000);

        if (decoded_config_json)
        {
            if (buffer_len >= total_size)
            {
                size_t o_len = 0;
                bzero(decoded_config_json, total_size);
                mbedtls_base64_decode((uint8_t *)decoded_config_json, (size_t)total_size, &o_len, (uint8_t *)base64_buffer, strlen(base64_buffer));
                TRACE_D("Decoded data: %s", decoded_config_json);
            }
            else
            {
                TRACE_E("decoding buffer underflow!");
            }
        }
        else
        {
            TRACE_E("decoding bufffer acquire failed!");
        }

        ezlopi_free(__FUNCTION__, base64_buffer);
    }

    return decoded_config_json;
}

#ifdef EZPI_SERV_BLE_ENABLE_READ_PROV
static char *__provisioning_info_jsonify(void)
{
    char *str_json_prov_info = NULL;

    cJSON *cj_prov_info = cJSON_CreateObject(__FUNCTION__);
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
        cJSON_AddStringToObject(__FUNCTION__, cj_prov_info, ezlopi_config_id_str, tmp_buffer);
        cJSON_AddNumberToObject(__FUNCTION__, cj_prov_info, ezlopi_config_time_str, ezlopi_nvs_config_info_update_time_get());
        cJSON_AddStringToObject(__FUNCTION__, cj_prov_info, ezlopi_device_name_str, device_name);
        cJSON_AddStringToObject(__FUNCTION__, cj_prov_info, ezlopi_brand_str, brand);
        cJSON_AddStringToObject(__FUNCTION__, cj_prov_info, ezlopi_manufacturer_name_str, manufacturer_name);
        cJSON_AddStringToObject(__FUNCTION__, cj_prov_info, ezlopi_model_number_str, model_number);
        cJSON_AddStringToObject(__FUNCTION__, cj_prov_info, ezlopi_uuid_str, uuid);
        cJSON_AddStringToObject(__FUNCTION__, cj_prov_info, ezlopi_uuid_provisioning_str, uuid_provisioning);
        cJSON_AddNumberToObject(__FUNCTION__, cj_prov_info, ezlopi_serial_str, ezlopi_factory_info_v3_get_id());
        cJSON_AddStringToObject(__FUNCTION__, cj_prov_info, ezlopi_cloud_server_str, cloud_server);
        cJSON_AddStringToObject(__FUNCTION__, cj_prov_info, ezlopi_ssl_private_key_str, ssl_private_key);
        cJSON_AddStringToObject(__FUNCTION__, cj_prov_info, ezlopi_ssl_shared_key_str, ssl_shared_key);
        cJSON_AddStringToObject(__FUNCTION__, cj_prov_info, ezlopi_ca_cert_str, ca_cert);
        cJSON_AddStringToObject(__FUNCTION__, cj_prov_info, ezlopi_device_type_ezlopi_str, ezlopi_factory_info_v3_get_device_type());

        ezlopi_factory_info_v3_free(device_name);
        ezlopi_factory_info_v3_free(brand);
        ezlopi_factory_info_v3_free(manufacturer_name);
        ezlopi_factory_info_v3_free(model_number);
        ezlopi_factory_info_v3_free(uuid);
        ezlopi_factory_info_v3_free(uuid_provisioning);
        ezlopi_factory_info_v3_free(cloud_server);
        // ezlopi_factory_info_v3_free(ssl_private_key); // allocated once for all, do not free
        // ezlopi_factory_info_v3_free(ssl_shared_key); // allocated once for all, do not free
        // ezlopi_factory_info_v3_free(ca_cert); // allocated once for all, do not free

        str_json_prov_info = cJSON_PrintBuffered(__FUNCTION__, cj_prov_info, 6 * 1024, false);
        cJSON_Delete(__FUNCTION__, cj_prov_info);
    }

    return str_json_prov_info;
}

static char *__provisioning_info_base64(void)
{
    char *base64_data = NULL;
    char *str_provisioning_data = __provisioning_info_jsonify();

    if (str_provisioning_data)
    {
        const uint32_t base64_data_len = 6 * 1024;
        base64_data = ezlopi_malloc(__FUNCTION__, base64_data_len);

        if (base64_data)
        {
            uint32_t out_put_len = 0;
            TRACE_D("str_provisioning_data[len: %d]: %s", strlen(str_provisioning_data), str_provisioning_data);

            int ret = mbedtls_base64_encode((unsigned char *)base64_data, base64_data_len, &out_put_len,
                                            (const unsigned char *)str_provisioning_data, strlen(str_provisioning_data));

            if (0 == out_put_len)
            {
                ezlopi_free(__FUNCTION__, base64_data);
                base64_data = NULL;
            }
        }

        ezlopi_free(__FUNCTION__, str_provisioning_data);
    }

    return base64_data;
}

#endif // CONFIG_EZPI_BLE_ENABLE

#endif // EZPI_SERV_BLE_ENABLE_READ_PROV