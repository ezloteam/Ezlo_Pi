#include "string.h"
#include "time.h"

#include "cJSON.h"
#include "lwip/ip_addr.h"
#include "esp_event_base.h"
#include "mbedtls/base64.h"

#include "trace.h"

#include "ezlopi_wifi.h"
#include "ezlopi_nvs.h"
#include "ezlopi_ble_buffer.h"
#include "ezlopi_ble_gatt.h"
#include "ezlopi_ble_profile.h"
#include "ezlopi_ble_service.h"
#include "ezlopi_factory_info.h"
#include "ezlopi_devices.h"
#include "ezlopi_ble_auth.h"

#define CJ_GET_STRING(name) cJSON_GetStringValue(cJSON_GetObjectItem(root, name))
#define CJ_GET_NUMBER(name) cJSON_GetNumberValue(cJSON_GetObjectItem(root, name))

static s_gatt_service_t *g_provisioning_service;
static s_linked_buffer_t *g_provisioning_linked_buffer = NULL;

static void ezlopi_process_provisioning_info(uint8_t *value, uint32_t len);
static void provisioning_status_read_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);

static void provisioning_info_write_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);
static void provisioning_info_read_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);
static void provisioning_info_exec_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);

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
    // permission = ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE;
    // properties = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE;
    permission = ESP_GATT_PERM_WRITE;
    properties = ESP_GATT_CHAR_PROP_BIT_WRITE;
    ezlopi_ble_gatt_add_characteristic(g_provisioning_service, &uuid, permission, properties, NULL, provisioning_info_write_func, provisioning_info_exec_func);

    uuid.uuid.uuid16 = BLE_PROVISIONING_STATUS_CHAR_UUID;
    uuid.len = ESP_UUID_LEN_16;
    permission = ESP_GATT_PERM_READ;
    properties = ESP_GATT_CHAR_PROP_BIT_READ;
    ezlopi_ble_gatt_add_characteristic(g_provisioning_service, &uuid, permission, properties, provisioning_status_read_func, NULL, NULL);
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
            cJSON_AddNumberToObject(root, "version", ezlopi_factory_info_v2_get_version());
            cJSON_AddNumberToObject(root, "status", prov_stat);
        }
        else
        {
            cJSON_AddNumberToObject(root, "version", 0);
            cJSON_AddNumberToObject(root, "status", 0);
        }

        prov_status_jstr = cJSON_Print(root);
        if (prov_status_jstr)
        {
            cJSON_Minify(prov_status_jstr);
        }
    }

    return prov_status_jstr;
}

static void provisioning_status_read_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
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
                uint32_t len = CJ_GET_NUMBER("len");
                uint32_t tot_len = CJ_GET_NUMBER("total_len");
                uint32_t sequence = CJ_GET_NUMBER("sequence");
                char *data = CJ_GET_STRING("data");
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

static void provisioning_info_write_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
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
                uint32_t len = CJ_GET_NUMBER("len");
                uint32_t tot_len = CJ_GET_NUMBER("total_len");
                uint32_t sequence = CJ_GET_NUMBER("sequence");

                TRACE_D("Len: %d", len);
                TRACE_D("tot_len: %d", tot_len);
                TRACE_D("sequence: %d", sequence);

                if (sequence && len && tot_len)
                {
                    TRACE_W("Here");
                    if (((sequence - 1) * 400 + len) >= tot_len)
                    {
                        TRACE_W("Here");
                        char *decoded_data = __base64_decode_provisioning_info(tot_len); // uncommente free as well
                        // static const char *decoded_data = "{\" serial \": 100004961,\" provision_server \": \" https : // req-disp-at0m.mios.com/\",\"cloud_server\": \"https://cloud.ezlo.com:7000\",\"provision_token\": \"c25415a5f1de9e5084dab5b0c92a28d2f5f64bb0079a9ffcc312eb005ff1d2b9bb896a097a565eb79fb15334f7c296e8ca67596cde8a87255d44e2507face3c288aed92624e355207700b07071365353acca8a1c3e6ac2e5dcce519021d2bc72cc7f5c4e85ebd0e75cc84c8b56a8a378672bcd9ac6975b511b77482001ab9a35\",\"ssl_private_key\": \"-----BEGIN PRIVATE KEY-----\nMIGEAgEAMBAGByqGSM49AgEGBSuBBAAKBG0wawIBAQQgpADG/nseS5R6XDv15FcM\ntU9zNVVal8X9qzESKrLzWDehRANCAATPmLXLTDUTxBvNIm6BFhzmGirTmW+wBE8B\nAEJJb1c9zuSdSMdMMUeb89KGb3scQUZotkiH/ki/VR0+3Lvorg8v\n-----END PRIVATE KEY-----\n\",\"ssl_public_key\": \"-----BEGIN PUBLIC KEY-----\nMFYwEAYHKoZIzj0CAQYFK4EEAAoDQgAEz5i1y0w1E8QbzSJugRYc5hoq05lvsARP\nAQBCSW9XPc7knUjHTDFHm/PShm97HEFGaLZIh/5Iv1UdPty76K4PLw==\n-----END PUBLIC KEY-----\",\"ssl_shared_key\": \"\n-----BEGIN CERTIFICATE-----\nMIICDTCCAbKgAwIBAgIDAzZDMAoGCCqGSM49BAMCMIGQMQswCQYDVQQGEwJVUzEU\nMBIGA1UECAwLIE5ldyBKZXJzZXkxEDAOBgNVBAcMB0NsaWZ0b24xDzANBgNVBAoM\nBklUIE9wczEPMA0GA1UECwwGSVQgT3BzMRQwEgYDVQQDDAtlWkxPIExURCBDQTEh\nMB8GCSqGSIb3DQEJARYSc3lzYWRtaW5zQGV6bG8uY29tMCAXDTIxMTIwMjA4NDYx\nMFoYDzIyOTUwOTE2MDg0NjEwWjCBjDELMAkGA1UEBhMCVVMxEzARBgNVBAgMCk5l\ndyBKZXJzZXkxEDAOBgNVBAcMB0NsaWZ0b24xEzARBgNVBAoMCmNvbnRyb2xsZXIx\nLTArBgNVBAsMJDRjMGQ5MDYwLTUzNGMtMTFlYy1iMmQ2LThmMjYwZjUyODdmYTES\nMBAGA1UEAwwJMTAwMDA0OTYxMFYwEAYHKoZIzj0CAQYFK4EEAAoDQgAEz5i1y0w1\nE8QbzSJugRYc5hoq05lvsARPAQBCSW9XPc7knUjHTDFHm/PShm97HEFGaLZIh/5I\nv1UdPty76K4PLzAKBggqhkjOPQQDAgNJADBGAiEAseX9mwtQ2DN60jCplOIIPd2S\n+6bdgpQGiCeYNpY/sBcCIQDwduNJYOIlBGmQgH1KAk9nW5JxAzA2MsPKovjTUSFB\nyg==\n-----END CERTIFICATE-----\n\",\"signing_ca_certificate\": \"-----BEGIN CERTIFICATE-----\r\nMIICbDCCAhGgAwIBAgIJAOByzaI7aHY9MAoGCCqGSM49BAMDMIGQMQswCQYDVQQG\r\nEwJVUzEUMBIGA1UECAwLIE5ldyBKZXJzZXkxEDAOBgNVBAcMB0NsaWZ0b24xDzAN\r\nBgNVBAoMBklUIE9wczEPMA0GA1UECwwGSVQgT3BzMRQwEgYDVQQDDAtlWkxPIExU\r\nRCBDQTEhMB8GCSqGSIb3DQEJARYSc3lzYWRtaW5zQGV6bG8uY29tMCAXDTE5MDUz\r\nMTE3MDE0N1oYDzIxMTkwNTA3MTcwMTQ3WjCBkDELMAkGA1UEBhMCVVMxFDASBgNV\r\nBAgMCyBOZXcgSmVyc2V5MRAwDgYDVQQHDAdDbGlmdG9uMQ8wDQYDVQQKDAZJVCBP\r\ncHMxDzANBgNVBAsMBklUIE9wczEUMBIGA1UEAwwLZVpMTyBMVEQgQ0ExITAfBgkq\r\nhkiG9w0BCQEWEnN5c2FkbWluc0BlemxvLmNvbTBWMBAGByqGSM49AgEGBSuBBAAK\r\nA0IABHLQdhLDYsafIFY8pZh96aDGqVm6E4r8nW9s4CfdpXaa/R4CnjaVpDQI7UmQ\r\n9vVDGZn8mcmm7VjKx+TSCS0MIKOjUzBRMB0GA1UdDgQWBBRiTl8Ez1l94jaqcxbi\r\nyxkVC0FkBTAfBgNVHSMEGDAWgBRiTl8Ez1l94jaqcxbiyxkVC0FkBTAPBgNVHRMB\r\nAf8EBTADAQH/MAoGCCqGSM49BAMDA0kAMEYCIQD7EUs8j50jKFd/46Zo95NbrPYQ\r\nPtLTHH9YjUkMEkYD5gIhAMP4y7E1aB78nQrmd3IX8MM32k9dM8xT0MztR16OtsuV\r\n-----END CERTIFICATE-----\",\"uuid\": \"4c0d9060-534c-11ec-b2d6-8f260f5287fa\",\"user_id\": \"lomas\",\"device_name\": \"My Device\",\"brand\": \"NDS Thermostat\",\"manufacturer_name\": \"Great Manif\",\"model_number\": \"063DEX524\",\"uuid_provisioning\": \"b558c598-a35a-4fdc-a2cd-5462ff11455a\",\"device_type_ezlopi\": \"generic\"}";
                        if (decoded_data)
                        {
                            TRACE_W("Here");
                            cJSON *cj_config = cJSON_Parse(decoded_data);
                            if (cj_config)
                            {
                                TRACE_W("Here");
                                char *user_id = NULL;
                                CJSON_GET_VALUE_STRING(cj_config, "user_id", user_id);

                                if (user_id && (BLE_AUTH_SUCCESS == ezlopi_ble_auth_check_user_id(user_id)))
                                {
                                    s_basic_factory_info_t *ezlopi_config_basic = malloc(sizeof(s_basic_factory_info_t));
                                    if (ezlopi_config_basic)
                                    {
                                        // ezlopi_config_basic->user_id = user_id;
                                        memset(ezlopi_config_basic, 0, sizeof(s_basic_factory_info_t));
                                        TRACE_W("Here");
                                        CJSON_GET_VALUE_STRING(cj_config, "device_name", ezlopi_config_basic->device_name);
                                        CJSON_GET_VALUE_STRING(cj_config, "manufacturer_name", ezlopi_config_basic->manufacturer);
                                        CJSON_GET_VALUE_STRING(cj_config, "brand", ezlopi_config_basic->brand);
                                        CJSON_GET_VALUE_STRING(cj_config, "model_number", ezlopi_config_basic->model_number);
                                        CJSON_GET_VALUE_DOUBLE(cj_config, "serial", ezlopi_config_basic->id);
                                        CJSON_GET_VALUE_STRING(cj_config, "uuid", ezlopi_config_basic->device_uuid);
                                        CJSON_GET_VALUE_STRING(cj_config, "uuid_provisioning", ezlopi_config_basic->prov_uuid);

                                        CJSON_GET_VALUE_STRING(cj_config, "provision_server", ezlopi_config_basic->provision_server);
                                        CJSON_GET_VALUE_STRING(cj_config, "cloud_server", ezlopi_config_basic->cloud_server);
                                        CJSON_GET_VALUE_STRING(cj_config, "provision_token", ezlopi_config_basic->provision_token);
                                        CJSON_GET_VALUE_STRING(cj_config, "device_type_ezlopi", ezlopi_config_basic->device_type);

                                        TRACE_W("Here");

                                        ezlopi_factory_info_v2_set_basic(ezlopi_config_basic);
                                        free(ezlopi_config_basic);
                                    }

                                    char *ssl_private_key = NULL;
                                    TRACE_W("Here");
                                    char *ssl_shared_key = NULL;
                                    char *ssl_public_key = NULL;
                                    char *ca_certs = NULL;
                                    char *ezlopi_config = NULL;

                                    CJSON_GET_VALUE_STRING(cj_config, "ssl_private_key", ssl_private_key);
                                    CJSON_GET_VALUE_STRING(cj_config, "ssl_public_key", ssl_public_key);
                                    CJSON_GET_VALUE_STRING(cj_config, "ssl_shared_key", ssl_shared_key);
                                    CJSON_GET_VALUE_STRING(cj_config, "signing_ca_certificate", ca_certs);
                                    CJSON_GET_VALUE_STRING(cj_config, "ezlopi_config", ezlopi_config);

                                    ezlopi_factory_info_v2_set_ezlopi_config(ezlopi_config);
                                    ezlopi_factory_info_v2_set_ca_cert(ca_certs);
                                    ezlopi_factory_info_v2_set_ssl_shared_key(ssl_shared_key);
                                    ezlopi_factory_info_v2_set_ssl_private_key(ssl_private_key);
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

static void provisioning_info_read_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{
    TRACE_D("Read function called!");
}

static void provisioning_info_exec_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{
    TRACE_D("Write execute function called.");
    ezlopi_ble_buffer_accumulate_to_start(g_provisioning_linked_buffer);
    ezlopi_process_provisioning_info(g_provisioning_linked_buffer->buffer, g_provisioning_linked_buffer->len);
    ezlopi_ble_buffer_free_buffer(g_provisioning_linked_buffer);
    g_provisioning_linked_buffer = NULL;
}

static void ezlopi_process_provisioning_info(uint8_t *value, uint32_t len)
{
    if ((NULL != value) && (len > 0))
    {
        cJSON *root = cJSON_Parse((const char *)value);
        if (root)
        {
            TRACE_D("value = %s", value);
            char *user_id = cJSON_GetObjectItemCaseSensitive(root, "user_id")->valuestring;
            char *device_name = cJSON_GetObjectItemCaseSensitive(root, "device_name")->valuestring;
            char *brand = cJSON_GetObjectItemCaseSensitive(root, "brand")->valuestring;
            char *manufacturer_name = cJSON_GetObjectItemCaseSensitive(root, "manufacturer_name")->valuestring;
            char *model_number = cJSON_GetObjectItemCaseSensitive(root, "model_number")->valuestring;
            char *uuid = cJSON_GetObjectItemCaseSensitive(root, "uuid")->valuestring;
            char *uuid_provisioning = cJSON_GetObjectItemCaseSensitive(root, "uuid_provisioning")->valuestring;
            double serial = cJSON_GetObjectItemCaseSensitive(root, "serial")->valuedouble;
            char *cloud_server = cJSON_GetObjectItemCaseSensitive(root, "cloud_server")->valuestring;
            char *ssl_private_key = cJSON_GetObjectItemCaseSensitive(root, "ssl_private_key")->valuestring;
            char *ssl_public_key = cJSON_GetObjectItemCaseSensitive(root, "ssl_public_key")->valuestring;
            char *ca_cert = cJSON_GetObjectItemCaseSensitive(root, "ca_cert")->valuestring;
            char *device_type_ezlopi = cJSON_GetObjectItemCaseSensitive(root, "device_type_ezlopi")->valuestring;

            TRACE_D("************************* BLE-PROVISIONING *************************");
            TRACE_D("user_id:               %s", user_id ? user_id : "");
            TRACE_D("device_name:           %s", device_name ? device_name : "");
            TRACE_D("brand:                 %s", brand ? brand : "");
            TRACE_D("manufacturer_name:     %s", manufacturer_name ? manufacturer_name : "");
            TRACE_D("model_number:          %s", model_number ? model_number : "");
            TRACE_D("uuid:                  %s", uuid ? uuid : "");
            TRACE_D("uuid_provisioning:     %s", uuid_provisioning ? uuid_provisioning : "");
            TRACE_D("serial:                %f", serial);
            TRACE_D("cloud_server:          %s", cloud_server ? cloud_server : "");
            TRACE_D("ssl_private_key:       %s", ssl_private_key ? ssl_private_key : "");
            TRACE_D("ssl_public_key:        %s", ssl_public_key ? ssl_public_key : "");
            TRACE_D("ca_cert:               %s", ca_cert ? ca_cert : "");
            TRACE_D("device_type_ezlopi:    %s", device_type_ezlopi ? device_type_ezlopi : "");
            TRACE_D("********************************************************************");

            cJSON_Delete(root);
        }
        else
        {
            TRACE_E("Invalid json packet received!");
        }
    }
}

// ewogICAgInVzZXJfaWQiOiAibG9tYXNzdWJlZGkiLAogICAgImRldmljZV9uYW1lIjogIk15IERldmljZSIsCiAgICAiYnJhbmQiOiAiTkRTIFRoZXJtb3N0YXQiLAogICAgIm1hbnVmYWN0dXJlcl9uYW1lIjogIk5lcGFsIERpZ2l0YWwgU3lzdGVtcyIsCiAgICAibW9kZWxfbnVtYmVyIjogIjA2M0RFWDUyNCIsCiAgICAidXVpZCI6ICI2NTI2MWQ3Ni1lNTg0LTRkMzUtYWZmMS1kODRiZDA0MyIsCiAgICAidXVpZF9wcm92aXNpb25pbmciOiAiNWZlNmI0OTgtOTdiNi00NjdhLTk1OTgtYWJmMmViM2IxOTVmIiwKICAgICJzZXJpYWwiOiAxMDAwMDQwMzIsCiAgICAiY2xvdWRfc2VydmVyIjogImh0dHBzOi8vY2xvdWQuZXpsby5jb206NzAwMCIsCiAgICAic3NsX3ByaXZhdGVfa2V5IjogIi0tLS0tQkVHSU4gUFJJVkFURSBLRVktLS0tLVxuTUlHRUFnRUFNQkFHQnlxR1NNNDlBZ0VHQlN1QkJBQUtCRzB3YXdJQkFRUWdsc2RGM0srU0tUdGExSEhIMERueVxuNitoT3kxT29ab1J3c1pZY2RjeGRRYWloUkFOQ0FBUnUzRExuWnZRMXQ0aG1oZVVyUThLSm5abWRKWEUzdGw2RVxuWGk0eXpxMW9kYjI5ZFNLaU5DQmovTUo2bXVtL2RxVEhVTjY1OHZkSE5xanJXbnlXenZPNFxuLS0tLS1FTkQgUFJJVkFURSBLRVktLS0tLVxuIiwKICAgICJzc2xfcHVibGljX2tleSI6ICItLS0tLUJFR0lOIFBVQkxJQyBLRVktLS0tLVxuTUZZd0VBWUhLb1pJemowQ0FRWUZLNEVFQUFvRFFnQUVidHd5NTJiME5iZUlab1hsSzBQQ2laMlpuU1Z4TjdaZVxuaEY0dU1zNnRhSFc5dlhVaW9qUWdZL3pDZXBycHYzYWt4MURldWZMM1J6YW82MXA4bHM3enVBPT1cbi0tLS0tRU5EIFBVQkxJQyBLRVktLS0tLSIsCiAgICAiY2FfY2VydCI6ICItLS0tLUJFR0lOIHhhQ0VSVElGSUNBVEUtLS0tLVxyXG5NSUlDYkRDQ0FoR2dBd0lCQWdJSkFPQnl6YUk3YUhZOU1Bb0dDQ3FHU000OUJBTURNSUdRTVFzd0NRWURWUVFHXHJcbkV3SlZVekVVTUJJR0ExVUVDQXdMSUU1bGR5QktaWEp6WlhreEVEQU9CZ05WQkFjTUIwTnNhV1owYjI0eER6QU5cclxuQmdOVkJBb01Ca2xVSUU5d2N6RVBNQTBHQTFVRUN3d0dTVlFnVDNCek1SUXdFZ1lEVlFRRERBdGxXa3hQSUV4VVxyXG5SQ0JEUVRFaE1COEdDU3FHU0liM0RRRUpBUllTYzNsellXUnRhVzV6UUdWNmJHOHVZMjl0TUNBWERURTVNRFV6XHJcbk1URTNNREUwTjFvWUR6SXhNVGt3TlRBM01UY3dNVFEzV2pDQmtERUxNQWtHQTFVRUJoTUNWVk14RkRBU0JnTlZcclxuQkFnTUN5Qk9aWGNnU21WeWMyVjVNUkF3RGdZRFZRUUhEQWREYkdsbWRHOXVNUTh3RFFZRFZRUUtEQVpKVkNCUFxyXG5jSE14RHpBTkJnTlZCQXNNQmtsVUlFOXdjekVVTUJJR0ExVUVBd3dMWlZwTVR5Qk1WRVFnUTBFeElUQWZCZ2txXHJcbmhraUc5dzBCQ1FFV0VuTjVjMkZrYldsdWMwQmxlbXh2TG1OdmJUQldNQkFHQnlxR1NNNDlBZ0VHQlN1QkJBQUtcclxuQTBJQUJITFFkaExEWXNhZklGWThwWmg5NmFER3FWbTZFNHI4blc5czRDZmRwWGFhL1I0Q25qYVZwRFFJN1VtUVxyXG45dlZER1puOG1jbW03VmpLeCtUU0NTME1JS09qVXpCUk1CMEdBMVVkRGdRV0JCUmlUbDhFejFsOTRqYXFjeGJpXHJcbnl4a1ZDMEZrQlRBZkJnTlZIU01FR0RBV2dCUmlUbDhFejFsOTRqYXFjeGJpeXhrVkMwRmtCVEFQQmdOVkhSTUJcclxuQWY4RUJUQURBUUgvTUFvR0NDcUdTTTQ5QkFNREEwa0FNRVlDSVFEN0VVczhqNTBqS0ZkLzQ2Wm85NU5iclBZUVxyXG5QdExUSEg5WWpVa01Fa1lENWdJaEFNUDR5N0UxYUI3OG5Rcm1kM0lYOE1NMzJrOWRNOHhUME16dFIxNk90c3VWXHJcbi0tLS0tRU5EIENFUlRJRklDQVRFLS0tLS0iLAogICAgImRldmljZV90eXBlX2V6bG9waSI6ICJnZW5lcmljIgp9