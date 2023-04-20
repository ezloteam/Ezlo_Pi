#include "string.h"

#include "cJSON.h"
#include "lwip/ip_addr.h"
#include "esp_event_base.h"
#include "esp_chip_info.h"
#include "mbedtls/base64.h"

#include "trace.h"
#include "version.h"
#include "ezlopi_wifi.h"

#include "ezlopi_nvs.h"
#include "ezlopi_ble_gatt.h"
#include "ezlopi_ble_profile.h"

#include "ezlopi_ble_buffer.h"
#include "ezlopi_ble_service.h"
#include "ezlopi_factory_info.h"
#include "ezlopi_system_info.h"

static s_gatt_service_t *g_device_info_service = NULL;

static char *device_info_jsonify(void);
static void __add_factory_info_to_root(cJSON *root, char *key, char *value);
static void device_info_read_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);

void ezlopi_ble_service_device_info_init(void)
{
    esp_bt_uuid_t uuid;
    esp_gatt_perm_t permission;
    esp_gatt_char_prop_t properties;

    uuid.len = ESP_UUID_LEN_16;
    uuid.uuid.uuid16 = BLE_DEVICE_INFO_SERVICE_UUID;
    g_device_info_service = ezlopi_ble_gatt_create_service(BLE_DEVICE_INFO_ID_HANDLE, &uuid);
    TRACE_W("'provisioning_service' service added to list");

    uuid.uuid.uuid16 = BLE_DEVICE_INFO_CHAR_UUID;
    uuid.len = ESP_UUID_LEN_16;
    permission = ESP_GATT_PERM_READ;
    properties = ESP_GATT_CHAR_PROP_BIT_READ;
    ezlopi_ble_gatt_add_characteristic(g_device_info_service, &uuid, permission, properties, device_info_read_func, NULL, NULL);
    TRACE_W("'provisioning_service' service added to list");
}

static void device_info_read_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{
    static char *json_str_device_info;

    if (NULL == json_str_device_info)
    {
        json_str_device_info = device_info_jsonify();
    }

    if (value)
    {
        if (json_str_device_info)
        {
            uint32_t total_data_len = strlen(json_str_device_info);
            uint32_t max_data_buffer_size = ezlopi_ble_gatt_get_max_data_size();
            uint32_t copy_size = ((total_data_len - param->read.offset) < max_data_buffer_size) ? (total_data_len - param->read.offset) : max_data_buffer_size;

            if ((0 != total_data_len) && (total_data_len > param->read.offset))
            {
                TRACE_D("Sending: %.*s", copy_size, json_str_device_info + param->read.offset);
                strncpy((char *)value->value, json_str_device_info + param->read.offset, copy_size);
                value->len = copy_size;
            }
            else
            {
                value->len = 1;
                value->value[0] = 0; // Read 0 if the device not provisioned yet.
            }

            if ((param->read.offset + copy_size) >= total_data_len)
            {
                free(json_str_device_info);
                json_str_device_info = NULL;
            }
        }
        else
        {
            value->len = 1;
            value->value[0] = 0; // Read 0 if the device not provisioned yet.
        }
    }
    else
    {
        free(json_str_device_info);
        json_str_device_info = NULL;
    }
}

static char *device_info_jsonify(void)
{
    char *device_info = NULL;
    cJSON *root = cJSON_CreateObject();
    if (root)
    {

        uint64_t uptime_us = esp_timer_get_time();
        uint64_t uptime_sec = uptime_us / 1000000;

        cJSON_AddStringToObject(root, "firmware_version", VERSION_STR);
        cJSON_AddNumberToObject(root, "firmware_build", BUILD);
        cJSON_AddStringToObject(root, "chip", CONFIG_IDF_TARGET);
        cJSON_AddStringToObject(root, "flash_size", CONFIG_ESPTOOLPY_FLASHSIZE);
        cJSON_AddStringToObject(root, "version_idf", esp_get_idf_version());
        cJSON_AddNumberToObject(root, "uptime", uptime_sec);
        cJSON_AddNumberToObject(root, "build_date", BUILD_DATE);
        cJSON_AddNumberToObject(root, "boot_count", ezlopi_system_info_get_boot_count());
        cJSON_AddNumberToObject(root, "boot_reason", esp_reset_reason());
        cJSON_AddBoolToObject(root, "provisioned_status", ezlopi_nvs_get_provisioning_status());
        uint8_t ble_mac[6];
        esp_read_mac(ble_mac, ESP_MAC_BT);
        char mac_string[32];
        snprintf(mac_string, sizeof(mac_string), "%02x:%02x:%02x:%02x:%02x:%02x",
                 mac_string[0], mac_string[1], mac_string[2], mac_string[3], mac_string[4], mac_string[5]);
        cJSON_AddStringToObject(root, "mac", mac_string);

        __add_factory_info_to_root(root, "ezlopi_device_type", ezlopi_factory_info_v2_get_device_type());
        __add_factory_info_to_root(root, "model", ezlopi_factory_info_v2_get_model());
        __add_factory_info_to_root(root, "device_name", ezlopi_factory_info_v2_get_name());
        __add_factory_info_to_root(root, "brand", ezlopi_factory_info_v2_get_brand());
        __add_factory_info_to_root(root, "manufacturer", ezlopi_factory_info_v2_get_manufacturer());

        // ssid, ips
        // manufacturer
        // model
        // device-type
        // device-name
        // brand

        device_info = cJSON_Print(root);
        if (device_info)
        {
            cJSON_Minify(device_info);
            TRACE_I("Created device info: %s", device_info);
        }

        cJSON_Delete(root);
    }

    return device_info;
}

void __add_factory_info_to_root(cJSON *root, char *key, char *value)
{
    if (value)
    {
        if (isalpha(value[0]))
        {
            cJSON_AddStringToObject(root, key, value);
        }
        else
        {
            cJSON_AddStringToObject(root, key, "unknown");
        }
        free(value);
    }
}