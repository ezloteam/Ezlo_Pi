#include "string.h"

#include "cJSON.h"
#include "lwip/ip_addr.h"
#include "esp_event_base.h"

#include "trace.h"
#include "ezlopi_wifi.h"

#include "ezlopi_nvs.h"
#include "ezlopi_ble_gatt.h"
#include "ezlopi_ble_profile.h"

#include "ezlopi_ble_service.h"
#include "ezlopi_ble_buffer.h"

static s_linked_buffer_t *wifi_creds_linked_buffer = NULL;
static char *wifi_creds_jsonify(void);
static void wifi_creds_write_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);
static void wifi_creds_read_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);
static void wifi_creds_write_exec_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);
static void wifi_creds_parse_and_connect(uint8_t *value, uint32_t len);
static void wifi_connection_status_read_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);
static void wifi_connection_error_read_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);

static void wifi_event_notify_upcall(esp_event_base_t event, void *arg);

static s_gatt_service_t *wifi_ble_service;

void ezlopi_ble_service_wifi_profile_init(void)
{
    esp_bt_uuid_t uuid;
    esp_gatt_perm_t permission;
    esp_gatt_char_prop_t properties;

    // wifi credentials
    uuid.len = ESP_UUID_LEN_16;
    uuid.uuid.uuid16 = BLE_WIFI_SERVICE_UUID;
    wifi_ble_service = ezlopi_ble_gatt_create_service(BLE_WIFI_SERVICE_HANDLE, &uuid);

    uuid.uuid.uuid16 = BLE_WIFI_CHAR_CREDS_UUID;
    uuid.len = ESP_UUID_LEN_16;
    permission = ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE;
    properties = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE;
    ezlopi_ble_gatt_add_characteristic(wifi_ble_service, &uuid, permission, properties, wifi_creds_read_func, wifi_creds_write_func, wifi_creds_write_exec_func);

    // wifi connection status
    uuid.len = ESP_UUID_LEN_16;
    uuid.uuid.uuid16 = BLE_WIFI_CHAR_STATUS_UUID;
    permission = ESP_GATT_PERM_READ;
    properties = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
    ezlopi_ble_gatt_add_characteristic(wifi_ble_service, &uuid, permission, properties, wifi_connection_status_read_func, NULL, NULL);

    // wifi error
    uuid.len = ESP_UUID_LEN_16;
    uuid.uuid.uuid16 = BLE_WIFI_CHAR_ERROR_UUID;
    permission = ESP_GATT_PERM_READ;
    properties = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
    ezlopi_ble_gatt_add_characteristic(wifi_ble_service, &uuid, permission, properties, wifi_connection_error_read_func, NULL, NULL);

    // ezlopi_wifi_event_add(wifi_event_notify_upcall, NULL);
}

static void wifi_event_notify_upcall(esp_event_base_t event, void *arg)
{
    esp_gatt_value_t value;
    wifi_connection_status_read_func(&value, NULL);
    ezlopi_ble_gatts_characteristic_notify(wifi_ble_service, wifi_ble_service->characteristics, &value);

    wifi_connection_error_read_func(&value, NULL);
    ezlopi_ble_gatts_characteristic_notify(wifi_ble_service, wifi_ble_service->characteristics, &value);
}

static void wifi_connection_status_read_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{
    if (value)
    {
        value->len = 1;
        if (ezlopi_wifi_got_ip())
        {
            value->value[0] = 1;
        }
        else
        {
            value->value[0] = 0;
        }
    }
}

static void wifi_connection_error_read_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{
    if (value)
    {
        value->len = strlen(ezlopi_wifi_get_last_disconnect_reason());
        snprintf((char *)value->value, ezlopi_ble_gatt_get_max_data_size(), "%s", ezlopi_wifi_get_last_disconnect_reason());
    }
}

static void wifi_creds_write_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{
    TRACE_D("Write function called!");
    if (0 == param->write.is_prep) // Data received in single packet
    {
        dump("GATT_WRITE_EVT value", param->write.value, 0, param->write.len);
        if ((NULL != param->write.value) && (param->write.len > 0))
        {
            wifi_creds_parse_and_connect(param->write.value, param->write.len);
        }
    }
    else
    {
        if (NULL == wifi_creds_linked_buffer)
        {
            wifi_creds_linked_buffer = ezlopi_ble_buffer_create(param);
        }
        else
        {
            ezlopi_ble_buffer_add_to_buffer(wifi_creds_linked_buffer, param);
        }
    }
}

static void wifi_creds_read_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{
    static char *json_str_wifi_info;

    if (NULL == json_str_wifi_info)
    {
        json_str_wifi_info = wifi_creds_jsonify();
    }

    if (NULL != json_str_wifi_info)
    {
        if (value)
        {
            uint32_t total_data_len = strlen(json_str_wifi_info);
            uint32_t max_data_buffer_size = ezlopi_ble_gatt_get_max_data_size();
            uint32_t copy_size = ((total_data_len - param->read.offset) < max_data_buffer_size) ? (total_data_len - param->read.offset) : max_data_buffer_size;
            if ((0 != total_data_len) && (total_data_len > param->read.offset))
            {
                strncpy((char *)value->value, json_str_wifi_info + param->read.offset, copy_size);
                value->len = copy_size;
            }
            else
            {
                value->len = 1;
                value->value[0] = 0; // Read 0 if the device not provisioned yet.
            }

            if ((param->read.offset + copy_size) >= total_data_len)
            {
                free(json_str_wifi_info);
                json_str_wifi_info = NULL;
            }
        }
    }
    else
    {
        if (value)
        {
            value->len = 1;
            value->value[0] = 0; // Read 0 if the device not provisioned yet.
        }
    }
}

static void wifi_creds_write_exec_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{
    TRACE_D("Write execute function called.");
    ezlopi_ble_buffer_accumulate_to_start(wifi_creds_linked_buffer);
    wifi_creds_parse_and_connect(wifi_creds_linked_buffer->buffer, wifi_creds_linked_buffer->len);
    ezlopi_ble_buffer_free_buffer(wifi_creds_linked_buffer);
    wifi_creds_linked_buffer = NULL;
}

static void wifi_creds_parse_and_connect(uint8_t *value, uint32_t len)
{
    if ((NULL != value) && (len > 0))
    {
        cJSON *root = cJSON_Parse((const char *)value);
        if (root)
        {
            char *ssid = cJSON_GetObjectItemCaseSensitive(root, "SSID")->valuestring;
            char *password = cJSON_GetObjectItemCaseSensitive(root, "PSD")->valuestring;
            ezlopi_wifi_connect(ssid, password);
            cJSON_Delete(root);
        }
    }
}

static char *wifi_creds_jsonify(void)
{
    char *json_str_wifi_info = NULL;
    char wifi_creds[64];
    memset(wifi_creds, 0, sizeof(wifi_creds));
    ezlopi_nvs_read_wifi(wifi_creds, sizeof(wifi_creds));

    cJSON *cjson_wifi_info = cJSON_CreateObject();
    if (cjson_wifi_info)
    {
        if (strlen(wifi_creds) >= 32)
        {
            wifi_creds[31] = '\0';
        }
        cJSON_AddStringToObject(cjson_wifi_info, "SSID", &wifi_creds[0]);
        cJSON_AddStringToObject(cjson_wifi_info, "PSD", "********");

        esp_netif_ip_info_t *wifi_ip_info = ezlopi_wifi_get_ip_infos();
        cJSON_AddStringToObject(cjson_wifi_info, "ip", ip4addr_ntoa((const ip4_addr_t *)&wifi_ip_info->ip));
        cJSON_AddStringToObject(cjson_wifi_info, "gw", ip4addr_ntoa((const ip4_addr_t *)&wifi_ip_info->gw));
        cJSON_AddStringToObject(cjson_wifi_info, "netmask", ip4addr_ntoa((const ip4_addr_t *)&wifi_ip_info->netmask));

        json_str_wifi_info = cJSON_Print(cjson_wifi_info);
        if (json_str_wifi_info)
        {
            cJSON_Minify(json_str_wifi_info);
        }

        cJSON_Delete(cjson_wifi_info);
    }

    return json_str_wifi_info;
}
