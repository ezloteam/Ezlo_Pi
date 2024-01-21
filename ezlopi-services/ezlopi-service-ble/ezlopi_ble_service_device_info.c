#include "string.h"
#include "ctype.h"

#include "cJSON.h"
#include "lwip/ip_addr.h"
#include "esp_event_base.h"
#include "esp_chip_info.h"
#include "mbedtls/base64.h"

#include "trace.h"
#include "version.h"
#include "ezlopi_core_wifi.h"

#include "ezlopi_nvs.h"
#include "ezlopi_ping.h"
#include "ezlopi_ble_gatt.h"
#include "ezlopi_ble_buffer.h"
#include "ezlopi_ble_profile.h"
#include "ezlopi_ble_service.h"
#include "ezlopi_system_info.h"
#include "ezlopi_factory_info.h"
#include "ezlopi_cloud_constants.h"

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
            TRACE_D("ble - chunk size: %d", copy_size);

            if ((0 != total_data_len) && (total_data_len > param->read.offset))
            {
                TRACE_D("Sending: [len = %d]\r\n%.*s", copy_size, copy_size, json_str_device_info + param->read.offset);
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

        cJSON_AddStringToObject(root, ezlopi_firmware_version_str, VERSION_STR);
        cJSON_AddNumberToObject(root, ezlopi_firmware_build_str, BUILD);
        cJSON_AddStringToObject(root, ezlopi_chip_str, CONFIG_IDF_TARGET);
        // cJSON_AddStringToObject(root, "flash_size", CONFIG_ESPTOOLPY_FLASHSIZE);
        // cJSON_AddStringToObject(root, "version_idf", esp_get_idf_version());
        cJSON_AddNumberToObject(root, ezlopi_uptime_str, uptime_sec);
        cJSON_AddNumberToObject(root, ezlopi_build_date_str, BUILD_DATE);
        // cJSON_AddNumberToObject(root, "boot_count", ezlopi_system_info_get_boot_count());
        // cJSON_AddNumberToObject(root, "boot_reason", esp_reset_reason());
        cJSON_AddBoolToObject(root, ezlopi_provisioned_status_str, ezlopi_factory_info_v3_get_provisioning_status());
        cJSON_AddStringToObject(root, ezlopi_mac_str, ezlopi_factory_info_v3_get_ezlopi_mac());

        cJSON_AddStringToObject(root, ezlopi_ezlopi_device_type_str, ezlopi_factory_info_v3_get_device_type());
        __add_factory_info_to_root(root, ezlopi_model_str, ezlopi_factory_info_v3_get_model());
        __add_factory_info_to_root(root, ezlopi_device_name_str, ezlopi_factory_info_v3_get_name());
        __add_factory_info_to_root(root, ezlopi_brand_str, ezlopi_factory_info_v3_get_brand());
        __add_factory_info_to_root(root, ezlopi_manufacturer_str, ezlopi_factory_info_v3_get_manufacturer());
        cJSON_AddNumberToObject(root, ezlopi_serial_str, ezlopi_factory_info_v3_get_id());

        char *ssid = ezlopi_factory_info_v3_get_ssid();
        if (ssid)
        {
            cJSON_AddStringToObject(root, ezlopi_wifi_ssid_str, (isprint(ssid[0])) ? ssid : ezlopi__str);
        }
        // esp_netif_ip_info_t *wifi_ip_info = ezlopi_wifi_get_ip_infos();
        // cJSON_AddStringToObject(root, "wifi-ip", ip4addr_ntoa((const ip4_addr_t *)&wifi_ip_info->ip));
        // cJSON_AddStringToObject(root, "wifi-gw", ip4addr_ntoa((const ip4_addr_t *)&wifi_ip_info->gw));
        // cJSON_AddStringToObject(root, "wifi-netmask", ip4addr_ntoa((const ip4_addr_t *)&wifi_ip_info->netmask));
        cJSON_AddNumberToObject(root, ezlopi_wifi_connection_status_str, ezlopi_wifi_got_ip());
        // cJSON_AddStringToObject(root, "wifi-error", ezlopi_wifi_get_last_disconnect_reason());
        uint8_t flag_internet_status = (EZLOPI_PING_STATUS_LIVE == ezlopi_ping_get_internet_status()) ? 1 : 0;
        cJSON_AddNumberToObject(root, ezlopi_internet_status_str, flag_internet_status);

        device_info = cJSON_Print(root);
        cJSON_Delete(root);

        if (device_info)
        {
            cJSON_Minify(device_info);
            TRACE_I("Created device info: %s", device_info);
        }
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
            cJSON_AddStringToObject(root, key, ezlopi_unknown_str);
        }
        // free(value);
    }
}
