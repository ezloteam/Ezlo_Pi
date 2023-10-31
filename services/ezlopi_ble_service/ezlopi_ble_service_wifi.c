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
#include "ezlopi_factory_info.h"
#include "ezlopi_ble_auth.h"
#include "ezlopi_ping.h"

static s_linked_buffer_t *wifi_creds_linked_buffer = NULL;

static void wifi_creds_write_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);
static void wifi_creds_write_exec_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);
static void wifi_creds_parse_and_connect(uint8_t *value, uint32_t len);
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
    permission = ESP_GATT_PERM_WRITE;
    properties = ESP_GATT_CHAR_PROP_BIT_WRITE;
    ezlopi_ble_gatt_add_characteristic(wifi_ble_service, &uuid, permission, properties, NULL, wifi_creds_write_func, wifi_creds_write_exec_func);
}

static void wifi_creds_write_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{
    if (0 == param->write.is_prep) // Data received in single packet
    {
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

static void wifi_creds_write_exec_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{
    if (wifi_creds_linked_buffer)
    {
        ezlopi_ble_buffer_accumulate_to_start(wifi_creds_linked_buffer);
        wifi_creds_parse_and_connect(wifi_creds_linked_buffer->buffer, wifi_creds_linked_buffer->len);
        ezlopi_ble_buffer_free_buffer(wifi_creds_linked_buffer);
        wifi_creds_linked_buffer = NULL;
    }
}

static void wifi_creds_parse_and_connect(uint8_t *value, uint32_t len)
{
    if ((NULL != value) && (len > 0))
    {
        cJSON *root = cJSON_Parse((const char *)value);
        if (root)
        {
            cJSON *cj_user_id = cJSON_GetObjectItemCaseSensitive(root, "user_id");
            cJSON *cj_ssid = cJSON_GetObjectItemCaseSensitive(root, "wifi_ssid");
            cJSON *cj_password = cJSON_GetObjectItemCaseSensitive(root, "wifi_password");

            if (cj_user_id && cj_password && cj_ssid)
            {
                char *ssid = cj_ssid->valuestring;
                char *password = cj_password->valuestring;
                char *user_id_str = cj_user_id->valuestring;

                if (user_id_str && ssid && password)
                {
                    e_auth_status_t l_ble_auth_status = ezlopi_ble_auth_check_user_id(user_id_str);

                    if (BLE_AUTH_SUCCESS == l_ble_auth_status)
                    {
                        ezlopi_wifi_connect(ssid, password);
                        ezlopi_factory_info_v2_set_wifi(ssid, password);
                    }
                    else if (BLE_AUTH_USER_ID_NOT_FOUND == l_ble_auth_status)
                    {
                        ezlopi_wifi_connect(ssid, password);
                        ezlopi_factory_info_v2_set_wifi(ssid, password);
                        ezlopi_ble_auth_store_user_id(user_id_str);
                    }
                }
            }

            cJSON_Delete(root);
        }
    }
}
