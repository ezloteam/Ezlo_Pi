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
    permission = ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE;
    properties = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE;
    ezlopi_ble_gatt_add_characteristic(g_provisioning_service, &uuid, permission, properties, provisioning_info_read_func, provisioning_info_write_func, provisioning_info_exec_func);

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
        cJSON_AddNumberToObject(root, "version", ezlopi_factory_info_v2_get_version());
        cJSON_AddNumberToObject(root, "time", ezlopi_nvs_get_provisioning_time());

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

static void __process_provisioning_info(uint32_t total_size)
{
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

        char *decoded_config_json = malloc(total_size);
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
}

static void provisioning_info_write_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{
    TRACE_D("Write function called!");
    TRACE_D("GATT_WRITE_EVT value: %.*s", param->write.len, param->write.value);

    time_t now;
    time(&now);
    TRACE_D("Provisioning time: %ld", now);
    ezlopi_nvs_set_provisioning_time(now);

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
                    if (((sequence - 1) * 400 + len) >= tot_len)
                    {
                        __process_provisioning_info(tot_len);
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
