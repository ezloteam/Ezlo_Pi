
#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_BLE_ENABLE

#include <string.h>
#include <ctype.h>

#include "cjext.h"
#include "lwip/ip_addr.h"
#include "esp_event_base.h"
#include "esp_chip_info.h"
#include "mbedtls/base64.h"

#include "esp_netif.h"
#include "esp_netif_ip_addr.h"
#include "esp_system.h"
#include "esp_idf_version.h"

#include "ezlopi_core_wifi.h"
#include "ezlopi_core_nvs.h"
#include "ezlopi_core_ping.h"
#include "ezlopi_core_ble_gatt.h"
#include "ezlopi_core_ble_buffer.h"
#include "ezlopi_core_ble_profile.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_event_group.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_sntp.h"
#include "ezlopi_core_info.h"
#include "ezlopi_core_reset.h"

#include "ezlopi_hal_system_info.h"

#include "ezlopi_cloud_constants.h"

#include "ezlopi_util_version.h"
#include "ezlopi_util_trace.h"

#include "ezlopi_service_ble.h"

static s_gatt_service_t* g_device_info_service = NULL;

static char* device_info_jsonify(void);
static void __add_factory_info_to_root(cJSON* root, char* key, char* value);

static void device_info_read_func(esp_gatt_value_t* value, esp_ble_gatts_cb_param_t* param);

static void EZPI_SERVICE_BLE_ezlopi_api_info(esp_gatt_value_t* value, esp_ble_gatts_cb_param_t* param);
static void EZPI_SERVICE_BLE_ezlopi_fmw_info(esp_gatt_value_t* value, esp_ble_gatts_cb_param_t* param);
static void EZPI_SERVICE_BLE_chip_info(esp_gatt_value_t* value, esp_ble_gatts_cb_param_t* param);
static void EZPI_SERVICE_BLE_firmware_sdk_info(esp_gatt_value_t* value, esp_ble_gatts_cb_param_t* param);
static void EZPI_SERVICE_BLE_device_state_info(esp_gatt_value_t* value, esp_ble_gatts_cb_param_t* param);
static void EZPI_SERVICE_BLE_serial_config_info(esp_gatt_value_t* value, esp_ble_gatts_cb_param_t* param);
static void EZPI_SERVICE_BLE_serial_config_write(esp_gatt_value_t* value, esp_ble_gatts_cb_param_t* param);
static void EZPI_SERVICE_BLE_ezlo_cloud_info(esp_gatt_value_t* value, esp_ble_gatts_cb_param_t* param);
static void EZPI_SERVICE_BLE_oem_info(esp_gatt_value_t* value, esp_ble_gatts_cb_param_t* param);
static void EZPI_SERVICE_BLE_net_info(esp_gatt_value_t* value, esp_ble_gatts_cb_param_t* param);

static void device_mac_read_func(esp_gatt_value_t* value, esp_ble_gatts_cb_param_t* param);

void ezlopi_ble_service_device_info_init(void)
{
    esp_bt_uuid_t uuid;
    esp_gatt_perm_t permission;
    esp_gatt_char_prop_t properties;

    uuid.len = ESP_UUID_LEN_16;
    uuid.uuid.uuid16 = BLE_DEVICE_INFO_SERVICE_UUID;
    g_device_info_service = ezlopi_ble_gatt_create_service(BLE_DEVICE_INFO_ID_HANDLE, &uuid);
    TRACE_W("'provisioning_service' service added to ezlopi-ble-stack");

    uuid.uuid.uuid16 = BLE_DEVICE_INFO_CHAR_UUID;
    uuid.len = ESP_UUID_LEN_16;
    permission = ESP_GATT_PERM_READ;
    properties = ESP_GATT_CHAR_PROP_BIT_READ;
    ezlopi_ble_gatt_add_characteristic(g_device_info_service, &uuid, permission, properties, device_info_read_func, NULL, NULL);
    TRACE_W("'provisioning_service' character added to ezlopi-ble-stack");

    uuid.uuid.uuid16 = EZPI_BLE_CHAR_API_VERSION_INFO_UUID;
    uuid.len = ESP_UUID_LEN_16;
    permission = ESP_GATT_PERM_READ;
    properties = ESP_GATT_CHAR_PROP_BIT_READ;
    ezlopi_ble_gatt_add_characteristic(g_device_info_service, &uuid, permission, properties, EZPI_SERVICE_BLE_ezlopi_api_info, NULL, NULL);

    uuid.uuid.uuid16 = EZPI_BLE_CHAR_FIRMWARE_INFO_UUID;
    uuid.len = ESP_UUID_LEN_16;
    permission = ESP_GATT_PERM_READ;
    properties = ESP_GATT_CHAR_PROP_BIT_READ;
    ezlopi_ble_gatt_add_characteristic(g_device_info_service, &uuid, permission, properties, EZPI_SERVICE_BLE_ezlopi_fmw_info, NULL, NULL);

    uuid.uuid.uuid16 = EZPI_BLE_CHAR_CHIP_INFO_UUID;
    uuid.len = ESP_UUID_LEN_16;
    permission = ESP_GATT_PERM_READ;
    properties = ESP_GATT_CHAR_PROP_BIT_READ;
    ezlopi_ble_gatt_add_characteristic(g_device_info_service, &uuid, permission, properties, EZPI_SERVICE_BLE_chip_info, NULL, NULL);

    uuid.uuid.uuid16 = EZPI_BLE_CHAR_FIRMWARE_SDK_INFO_UUID;
    uuid.len = ESP_UUID_LEN_16;
    permission = ESP_GATT_PERM_READ;
    properties = ESP_GATT_CHAR_PROP_BIT_READ;
    ezlopi_ble_gatt_add_characteristic(g_device_info_service, &uuid, permission, properties, EZPI_SERVICE_BLE_firmware_sdk_info, NULL, NULL);

    uuid.uuid.uuid16 = EZPI_BLE_CHAR_DEV_STATE_INFO_UUID;
    uuid.len = ESP_UUID_LEN_16;
    permission = ESP_GATT_PERM_READ;
    properties = ESP_GATT_CHAR_PROP_BIT_READ;
    ezlopi_ble_gatt_add_characteristic(g_device_info_service, &uuid, permission, properties, EZPI_SERVICE_BLE_device_state_info, NULL, NULL);

    uuid.uuid.uuid16 = EZPI_BLE_CHAR_SER_CONFIG_INFO_UUID;
    uuid.len = ESP_UUID_LEN_16;
    permission = ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE;
    properties = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE;
    ezlopi_ble_gatt_add_characteristic(g_device_info_service, &uuid, permission, properties, EZPI_SERVICE_BLE_serial_config_info, EZPI_SERVICE_BLE_serial_config_write, NULL);

    uuid.uuid.uuid16 = EZPI_BLE_CHAR_EZPI_CLOUD_INFO_UUID;
    uuid.len = ESP_UUID_LEN_16;
    permission = ESP_GATT_PERM_READ;
    properties = ESP_GATT_CHAR_PROP_BIT_READ;
    ezlopi_ble_gatt_add_characteristic(g_device_info_service, &uuid, permission, properties, EZPI_SERVICE_BLE_ezlo_cloud_info, NULL, NULL);

    uuid.uuid.uuid16 = EZPI_BLE_CHAR_OEM_INFO_UUID;
    uuid.len = ESP_UUID_LEN_16;
    permission = ESP_GATT_PERM_READ;
    properties = ESP_GATT_CHAR_PROP_BIT_READ;
    ezlopi_ble_gatt_add_characteristic(g_device_info_service, &uuid, permission, properties, EZPI_SERVICE_BLE_oem_info, NULL, NULL);

    uuid.uuid.uuid16 = EZPI_BLE_CHAR_NETWORK_INFO_UUID;
    uuid.len = ESP_UUID_LEN_16;
    permission = ESP_GATT_PERM_READ;
    properties = ESP_GATT_CHAR_PROP_BIT_READ;
    ezlopi_ble_gatt_add_characteristic(g_device_info_service, &uuid, permission, properties, EZPI_SERVICE_BLE_net_info, NULL, NULL);

}

static void ble_device_info_send_data(const cJSON* cj_response_data, esp_gatt_value_t* value, esp_ble_gatts_cb_param_t* param)
{
    char* send_data = cJSON_Print(__FUNCTION__, cj_response_data);
    if (send_data)
    {
        cJSON_Minify(send_data);

        uint32_t total_data_len = strlen(send_data);
        uint32_t max_data_buffer_size = ezlopi_ble_gatt_get_max_data_size();
        uint32_t copy_size = ((total_data_len - param->read.offset) < max_data_buffer_size) ? (total_data_len - param->read.offset) : max_data_buffer_size;

        if ((0 != total_data_len) && (total_data_len > param->read.offset))
        {
            strncpy((char*)value->value, send_data + param->read.offset, copy_size);
            value->len = copy_size;
        }
        if ((param->read.offset + copy_size) >= total_data_len)
        {
            ezlopi_free(__FUNCTION__, send_data);
            send_data = NULL;
        }
    }
    else
    {
        TRACE_E("No data to send");
        value->len = 1;
        value->value[0] = 0; // Read 0 if the device not provisioned yet.
    }
}

static void device_mac_read_func(esp_gatt_value_t* value, esp_ble_gatts_cb_param_t* param)
{
    if (value)
    {
        cJSON* cj_device_mac = cJSON_CreateObject(__FUNCTION__);
        if (cj_device_mac)
        {
            char* device_mac = ezlopi_factory_info_v3_get_ezlopi_mac();
            cJSON_AddStringToObject(__FUNCTION__, cj_device_mac, "ezlopi_mac", device_mac ? device_mac : "unknown");

            uint8_t mac[6];
            ezlopi_wifi_get_wifi_mac(mac);
            char mac_str[20];
            memset(mac_str, 0, sizeof(mac_str));
            snprintf(mac_str, 20, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            cJSON_AddStringToObject(__FUNCTION__, cj_device_mac, "wifi_mac", mac_str);

            memset(mac, 0, sizeof(mac));
            ezlopi_ble_service_get_ble_mac(mac);
            memset(mac_str, 0, sizeof(mac_str));
            snprintf(mac_str, 20, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            cJSON_AddStringToObject(__FUNCTION__, cj_device_mac, "ble_mac", mac_str);

            ble_device_info_send_data(cj_device_mac, value, param);

            cJSON_Delete(__FUNCTION__, cj_device_mac);
            ezlopi_free(__FUNCTION__, device_mac);
        }
        else
        {
            TRACE_E("Couldn't allocate memory for device mac json");
        }
    }
    else
    {
        TRACE_E("Value is empty");
    }
}

static void EZPI_SERVICE_BLE_ezlopi_api_info(esp_gatt_value_t* value, esp_ble_gatts_cb_param_t* param)
{
    if (value)
    {
        cJSON* cj_api = cJSON_CreateObject(__FUNCTION__);
        if (cj_api)
        {
            cJSON_AddStringToObject(__FUNCTION__, cj_api, "api", EZPI_VERSION_API_BLE);
            ble_device_info_send_data(cj_api, value, param);
            cJSON_Delete(__FUNCTION__, cj_api);
        }
        else
        {
            TRACE_E("Couldn't allocate memory for device status");
        }
    }
    else
    {
        TRACE_E("Value is NULL");
    }
}


static void EZPI_SERVICE_BLE_ezlopi_fmw_info(esp_gatt_value_t* value, esp_ble_gatts_cb_param_t* param)
{
    if (value)
    {
        cJSON* cj_firmware_info = cJSON_CreateObject(__FUNCTION__);
        if (cj_firmware_info)
        {
            char build_time[64];
            cJSON_AddStringToObject(__FUNCTION__, cj_firmware_info, ezlopi_version_str, VERSION_STR);
            cJSON_AddNumberToObject(__FUNCTION__, cj_firmware_info, ezlopi_build_str, BUILD);
            EZPI_CORE_sntp_epoch_to_iso8601(build_time, sizeof(build_time), (time_t)BUILD_DATE);
            cJSON_AddStringToObject(__FUNCTION__, cj_firmware_info, ezlopi_build_date_str, build_time);

            ble_device_info_send_data(cj_firmware_info, value, param);
            cJSON_Delete(__FUNCTION__, cj_firmware_info);
        }
        else
        {
            TRACE_E("Couldn't allocate memory for device status");
        }
    }
    else
    {
        TRACE_E("Value is NULL");
    }
}

static void EZPI_SERVICE_BLE_chip_info(esp_gatt_value_t* value, esp_ble_gatts_cb_param_t* param)
{
    if (value)
    {
        cJSON* cj_chip = cJSON_CreateObject(__FUNCTION__);
        if (cj_chip)
        {
            esp_chip_info_t chip_info;
            char chip_revision[10];
            esp_chip_info(&chip_info);
            sprintf(chip_revision, "%.2f", (float)(chip_info.full_revision / 100.0));
            cJSON_AddStringToObject(__FUNCTION__, cj_chip, "type", EZPI_CORE_info_get_chip_type_to_name(chip_info.model));
            cJSON_AddStringToObject(__FUNCTION__, cj_chip, "version", chip_revision);
            ble_device_info_send_data(cj_chip, value, param);
            cJSON_Delete(__FUNCTION__, cj_chip);
        }
        else
        {
            TRACE_E("Couldn't allocate memory for device status");
        }
    }
    else
    {
        TRACE_E("Value is NULL");
    }
}

static void EZPI_SERVICE_BLE_firmware_sdk_info(esp_gatt_value_t* value, esp_ble_gatts_cb_param_t* param)
{
    if (value)
    {
        cJSON* cj_firmware_sdk = cJSON_CreateObject(__FUNCTION__);
        if (cj_firmware_sdk)
        {
            cJSON_AddStringToObject(__FUNCTION__, cj_firmware_sdk, "name", "ESP-IDF");
            cJSON_AddStringToObject(__FUNCTION__, cj_firmware_sdk, "version", esp_get_idf_version());
            ble_device_info_send_data(cj_firmware_sdk, value, param);
            cJSON_Delete(__FUNCTION__, cj_firmware_sdk);
        }
        else
        {
            TRACE_E("Couldn't allocate memory for device status");
        }
    }
    else
    {
        TRACE_E("Value is NULL");
    }
}

static void EZPI_SERVICE_BLE_device_state_info(esp_gatt_value_t* value, esp_ble_gatts_cb_param_t* param)
{
    if (value)
    {
        cJSON* cj_device_state = cJSON_CreateObject(__FUNCTION__);
        if (cj_device_state)
        {

            char time_string[50];
            uint32_t tick_count_ms = xTaskGetTickCount() / portTICK_PERIOD_MS;
            EZPI_CORE_info_get_tick_to_time_name(time_string, sizeof(time_string), tick_count_ms);

            cJSON_AddStringToObject(__FUNCTION__, cj_device_state, ezlopi_uptime_str, time_string);
            cJSON_AddNumberToObject(__FUNCTION__, cj_device_state, "boot_count", ezlopi_system_info_get_boot_count());
            cJSON_AddStringToObject(__FUNCTION__, cj_device_state, "boot_reason", EZPI_CORE_info_get_esp_reset_reason_to_name(esp_reset_reason()));

            cJSON_AddStringToObject(__FUNCTION__, cj_device_state, ezlopi_flash_size_str, CONFIG_ESPTOOLPY_FLASHSIZE);

            uint8_t mac[6];
            ezlopi_wifi_get_wifi_mac(mac);
            char mac_str[20];
            memset(mac_str, 0, sizeof(mac_str));
            snprintf(mac_str, 20, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            cJSON_AddStringToObject(__FUNCTION__, cj_device_state, "wifi_mac", mac_str);

            memset(mac, 0, sizeof(mac));
            ezlopi_ble_service_get_ble_mac(mac);
            memset(mac_str, 0, sizeof(mac_str));
            snprintf(mac_str, 20, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            cJSON_AddStringToObject(__FUNCTION__, cj_device_state, "ble_mac", mac_str);

            ble_device_info_send_data(cj_device_state, value, param);

            cJSON_Delete(__FUNCTION__, cj_device_state);
        }
        else
        {
            TRACE_E("Couldn't allocate memory for device status");
        }
    }
    else
    {
        TRACE_E("Value is NULL");
    }
}

static void EZPI_SERVICE_BLE_serial_config_info(esp_gatt_value_t* value, esp_ble_gatts_cb_param_t* param)
{
    if (value)
    {
        cJSON* cj_serial_config = cJSON_CreateObject(__FUNCTION__);
        if (cj_serial_config)
        {
            uint32_t baud = EZPI_SERV_UART_BAUD_DEFAULT;
            uint32_t parity_val = EZPI_SERV_UART_PARITY_DEFAULT;
            uint32_t start_bits = EZPI_SERV_UART_START_BIT_DEFAULT;
            uint32_t stop_bits = EZPI_SERV_UART_STOP_BIT_DEFAULT;
            uint32_t frame_size = EZPI_SERV_UART_FRAME_SIZE_DEFAULT;
            uint32_t flow_control = EZPI_SERV_UART_FLOW_CTRL_DEFAULT;
            char parity[2];

            char flw_ctrl_bffr[EZPI_UART_SERV_FLW_CTRL_STR_SIZE + 1];
            flw_ctrl_bffr[EZPI_UART_SERV_FLW_CTRL_STR_SIZE] = 0;

            EZPI_CORE_nvs_read_baud(&baud);
            cJSON_AddNumberToObject(__FUNCTION__, cj_serial_config, ezlopi_baud_str, baud);

            EZPI_CORE_nvs_read_parity(&parity_val);
            parity[0] = EZPI_CORE_info_parity_to_name(parity_val);
            parity[1] = 0;
            cJSON_AddStringToObject(__FUNCTION__, cj_serial_config, ezlopi_parity_str, parity);

            EZPI_CORE_nvs_read_start_bits(&start_bits);
            cJSON_AddNumberToObject(__FUNCTION__, cj_serial_config, ezlopi_start_bits_str, start_bits);

            EZPI_CORE_nvs_read_stop_bits(&stop_bits);
            cJSON_AddNumberToObject(__FUNCTION__, cj_serial_config, ezlopi_stop_bits_str, stop_bits);

            EZPI_CORE_nvs_read_frame_size(&frame_size);
            cJSON_AddNumberToObject(__FUNCTION__, cj_serial_config, ezlopi_frame_size_str, frame_size);

            EZPI_CORE_nvs_read_flow_control(&flow_control);
            EZPI_CORE_info_get_flow_ctrl_to_name(flow_control, flw_ctrl_bffr);
            cJSON_AddStringToObject(__FUNCTION__, cj_serial_config, ezlopi_flow_control_str, flw_ctrl_bffr);

            ble_device_info_send_data(cj_serial_config, value, param);

            cJSON_Delete(__FUNCTION__, cj_serial_config);
        }
        else
        {
            TRACE_E("Couldn't allocate memory for device status");
        }
    }
    else
    {
        TRACE_E("Value is NULL");
    }
}

static void EZPI_SERVICE_BLE_serial_config_write(esp_gatt_value_t* value, esp_ble_gatts_cb_param_t* param)
{
    if (value)
    {
        if (param && param->write.len && param->write.value)
        {
            cJSON* root = cJSON_ParseWithLength(__FUNCTION__, (const char*)param->write.value, param->write.len);
            if (root)
            {
                uint32_t baud = 0;
                uint32_t parity_val = EZPI_SERV_UART_PARITY_DEFAULT;
                uint32_t start_bits = EZPI_SERV_UART_START_BIT_DEFAULT;
                uint32_t stop_bits = EZPI_SERV_UART_STOP_BIT_DEFAULT;
                uint32_t frame_size = 0;
                uint32_t flow_control_val = EZPI_SERV_UART_FLOW_CTRL_DEFAULT;

                uint32_t baud_current = EZPI_SERV_UART_BAUD_DEFAULT;
                uint32_t parity_val_current = EZPI_SERV_UART_PARITY_DEFAULT;
                uint32_t start_bits_current = EZPI_SERV_UART_START_BIT_DEFAULT;
                uint32_t stop_bits_current = EZPI_SERV_UART_STOP_BIT_DEFAULT;
                uint32_t frame_size_current = EZPI_SERV_UART_FRAME_SIZE_DEFAULT;
                uint32_t flow_control_current = EZPI_SERV_UART_FLOW_CTRL_DEFAULT;

                char str_parity[4];
                char str_flowcontrol[16];
                bool flag_new_config = false;

                CJSON_GET_VALUE_DOUBLE(root, ezlopi_baud_str, baud);
                CJSON_GET_VALUE_STRING_BY_COPY(root, ezlopi_parity_str, str_parity);
                CJSON_GET_VALUE_DOUBLE(root, ezlopi_start_bits_str, start_bits);
                CJSON_GET_VALUE_DOUBLE(root, ezlopi_stop_bits_str, stop_bits);
                CJSON_GET_VALUE_DOUBLE(root, ezlopi_frame_size_str, frame_size);
                CJSON_GET_VALUE_STRING_BY_COPY(root, ezlopi_flow_control_str, str_flowcontrol);

                EZPI_CORE_nvs_read_baud(&baud_current);
                EZPI_CORE_nvs_read_parity(&parity_val_current);
                EZPI_CORE_nvs_read_start_bits(&start_bits_current);
                EZPI_CORE_nvs_read_stop_bits(&stop_bits_current);
                EZPI_CORE_nvs_read_frame_size(&frame_size_current);
                EZPI_CORE_nvs_read_flow_control(&flow_control_current);

                if (
                    (baud_current != baud) ||
                    (parity_val_current != (uint32_t)EZPI_CORE_info_name_to_parity(str_parity)) ||
                    (start_bits_current != start_bits) ||
                    (stop_bits != stop_bits_current) ||
                    (frame_size != frame_size_current) ||
                    (flow_control_current != (uint32_t)EZPI_CORE_info_get_flw_ctrl_from_name(str_flowcontrol))
                    )
                {
                    flag_new_config = true;
                }

                if (flag_new_config)
                {
                    if ('\0' != str_parity[0])
                    {
                        parity_val = (uint32_t)EZPI_CORE_info_name_to_parity(str_parity);
                    }
                    EZPI_CORE_nvs_write_parity(parity_val);

                    if (baud)
                    {
                        EZPI_CORE_nvs_write_baud(baud);
                    }
                    else
                    {
                        baud = EZPI_SERV_UART_BAUD_DEFAULT;
                        EZPI_CORE_nvs_write_baud(baud);
                    }

                    EZPI_CORE_nvs_write_start_bits(start_bits);
                    EZPI_CORE_nvs_write_stop_bits(stop_bits);

                    if (!frame_size)
                    {
                        frame_size = EZPI_SERV_UART_FRAME_SIZE_DEFAULT;
                    }
                    EZPI_CORE_nvs_write_frame_size(frame_size);


                    if ('\0' != str_flowcontrol[0])
                    {
                        flow_control_val = (uint32_t)EZPI_CORE_info_get_flw_ctrl_from_name(str_flowcontrol);
                        TRACE_W("New Flow control: %d", flow_control_val);
                    }

                    EZPI_CORE_nvs_write_flow_control(flow_control_val);

                    TRACE_W("New config has been saved, reboot needed to apply changes.");
                    vTaskDelay(10);
                    // EZPI_CORE_reset_reboot();
                }
                else
                {
                    TRACE_W("Configuration unchanged !");
                }
            }
        }
    }
    else
    {
        TRACE_E("Value is NULL");
    }
}

static void EZPI_SERVICE_BLE_ezlo_cloud_info(esp_gatt_value_t* value, esp_ble_gatts_cb_param_t* param)
{
    if (value)
    {
        cJSON* cj_ezlopi = cJSON_CreateObject(__FUNCTION__);
        if (cj_ezlopi)
        {
            char* device_mac = ezlopi_factory_info_v3_get_ezlopi_mac();
            char* controller_uuid = ezlopi_factory_info_v3_get_device_uuid();
            char* provisioning_uuid = ezlopi_factory_info_v3_get_provisioning_uuid();
            unsigned long long serial_id = ezlopi_factory_info_v3_get_id();
            const char* device_type = ezlopi_factory_info_v3_get_device_type();

            cJSON_AddBoolToObject(__FUNCTION__, cj_ezlopi, ezlopi_provisioned_str, ezlopi_factory_info_v3_get_provisioning_status());
            cJSON_AddStringToObject(__FUNCTION__, cj_ezlopi, ezlopi_uuid_str, controller_uuid ? controller_uuid : "");
            cJSON_AddStringToObject(__FUNCTION__, cj_ezlopi, ezlopi_uuid_prov_str, provisioning_uuid ? provisioning_uuid : "");
            cJSON_AddStringToObject(__FUNCTION__, cj_ezlopi, ezlopi_type_str, device_type ? device_type : "");
            cJSON_AddNumberToObject(__FUNCTION__, cj_ezlopi, ezlopi_serial_str, serial_id);
            cJSON_AddStringToObject(__FUNCTION__, cj_ezlopi, ezlopi_mac_str, device_mac ? device_mac : "");


            ezlopi_factory_info_v3_free(device_mac);
            ezlopi_factory_info_v3_free(controller_uuid);
            ezlopi_factory_info_v3_free(provisioning_uuid);

            ble_device_info_send_data(cj_ezlopi, value, param);

            cJSON_Delete(__FUNCTION__, cj_ezlopi);
        }
        else
        {
            TRACE_E("Couldn't allocate memory for device status");
        }
    }
    else
    {
        TRACE_E("Value is NULL");
    }
}

static void EZPI_SERVICE_BLE_oem_info(esp_gatt_value_t* value, esp_ble_gatts_cb_param_t* param)
{
    if (value)
    {
        cJSON* cj_oem = cJSON_CreateObject(__FUNCTION__);
        if (cj_oem)
        {
            char* device_model = ezlopi_factory_info_v3_get_model();
            char* device_brand = ezlopi_factory_info_v3_get_brand();
            char* device_manufacturer = ezlopi_factory_info_v3_get_manufacturer();

            cJSON_AddStringToObject(__FUNCTION__, cj_oem, ezlopi_brand_str, device_brand ? device_brand : "");
            cJSON_AddStringToObject(__FUNCTION__, cj_oem, ezlopi_manufacturer_str, device_manufacturer ? device_manufacturer : "");
            cJSON_AddStringToObject(__FUNCTION__, cj_oem, ezlopi_model_str, device_model ? device_model : "");

            ble_device_info_send_data(cj_oem, value, param);

            ezlopi_factory_info_v3_free(device_model);
            ezlopi_factory_info_v3_free(device_manufacturer);
            ezlopi_factory_info_v3_free(device_brand);

            cJSON_Delete(__FUNCTION__, cj_oem);
        }
        else
        {
            TRACE_E("Couldn't allocate memory for device status");
        }
    }
    else
    {
        TRACE_E("Value is NULL");
    }
}

static void EZPI_SERVICE_BLE_net_info(esp_gatt_value_t* value, esp_ble_gatts_cb_param_t* param)
{
    if (value)
    {
        cJSON* cj_network = cJSON_CreateObject(__FUNCTION__);
        if (cj_network)
        {
            char* wifi_ssid = ezlopi_factory_info_v3_get_ssid();
            cJSON_AddStringToObject(__FUNCTION__, cj_network, ezlopi_ssid_str, wifi_ssid ? wifi_ssid : "");
            ezlopi_factory_info_v3_free(wifi_ssid);

            ezlopi_wifi_status_t* wifi_status = ezlopi_wifi_status();
            // if (wifi_status)
            {
                char* wifi_mode = EZPI_CORE_info_get_wifi_mode_to_name(wifi_status->wifi_mode);
                cJSON_AddStringToObject(__FUNCTION__, cj_network, "wifi_mode", wifi_mode ? wifi_mode : "");

                char ip_str[20];
                memset(ip_str, 0, 20);
                snprintf(ip_str, 20, IPSTR, IP2STR(&wifi_status->ip_info->ip));
                cJSON_AddStringToObject(__FUNCTION__, cj_network, "ip_sta", ip_str);

                memset(ip_str, 0, 20);
                snprintf(ip_str, 20, IPSTR, IP2STR(&wifi_status->ip_info->netmask));
                cJSON_AddStringToObject(__FUNCTION__, cj_network, "ip_nmask", ip_str);

                memset(ip_str, 0, 20);
                snprintf(ip_str, 20, IPSTR, IP2STR(&wifi_status->ip_info->gw));
                cJSON_AddStringToObject(__FUNCTION__, cj_network, "ip_gw", ip_str);

                cJSON_AddBoolToObject(__FUNCTION__, cj_network, "wifi", wifi_status->wifi_connection);

#ifdef CONFIG_EZPI_ENABLE_PING
                e_ping_status_t ping_status = ezlopi_ping_get_internet_status();
                cJSON_AddBoolToObject(__FUNCTION__, cj_network, "internet", ping_status == EZLOPI_PING_STATUS_LIVE);
#else // CONFIG_EZPI_ENABLE_PING
                cJSON_AddBoolToObject(__FUNCTION__, cj_network, "internet", 0);
#endif // CONFIG_EZPI_ENABLE_PING


                e_ezlopi_event_t events = ezlopi_get_event_bit_status();
                bool cloud_connection_status = (EZLOPI_EVENT_NMA_REG & events) == EZLOPI_EVENT_NMA_REG;
                cJSON_AddBoolToObject(__FUNCTION__, cj_network, "cloud", cloud_connection_status);

                ble_device_info_send_data(cj_network, value, param);

                cJSON_Delete(__FUNCTION__, cj_network);
                ezlopi_free(__FUNCTION__, wifi_status);
            }
        }
        else
        {
            TRACE_E("Couldn't allocate memory for device status");
        }
    }
    else
    {
        TRACE_E("Value is NULL");
    }
}


static void device_info_read_func(esp_gatt_value_t* value, esp_ble_gatts_cb_param_t* param)
{
    #warning "This Characteristics is depricated, will be removed in future release later than 3.4.7"
        static char* json_str_device_info;

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
                strncpy((char*)value->value, json_str_device_info + param->read.offset, copy_size);
                value->len = copy_size;
            }
            else
            {
                value->len = 1;
                value->value[0] = 0; // Read 0 if the device not provisioned yet.
            }

            if ((param->read.offset + copy_size) >= total_data_len)
            {
                ezlopi_free(__FUNCTION__, json_str_device_info);
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
        ezlopi_free(__FUNCTION__, json_str_device_info);
        json_str_device_info = NULL;
    }
}

static char* device_info_jsonify(void)
{
    #warning "This info will be only available till Version 3.4.6 !"

        char* device_info = NULL;
    cJSON* root = cJSON_CreateObject(__FUNCTION__);
    if (root)
    {
        cJSON_AddStringToObject(__FUNCTION__, root, ezlopi_firmware_version_str, VERSION_STR);
        cJSON_AddNumberToObject(__FUNCTION__, root, ezlopi_build_str, BUILD);
        cJSON_AddStringToObject(__FUNCTION__, root, ezlopi_chip_str, CONFIG_IDF_TARGET);
        cJSON_AddNumberToObject(__FUNCTION__, root, ezlopi_build_date_str, BUILD_DATE);
        cJSON_AddBoolToObject(__FUNCTION__, root, ezlopi_provisioned_status_str, ezlopi_factory_info_v3_get_provisioning_status());
        __add_factory_info_to_root(root, (char*)ezlopi_mac_str, ezlopi_factory_info_v3_get_ezlopi_mac());

        cJSON_AddStringToObject(__FUNCTION__, root, ezlopi_ezlopi_device_type_str, ezlopi_factory_info_v3_get_device_type());
        __add_factory_info_to_root(root, (char*)ezlopi_model_str, ezlopi_factory_info_v3_get_model());
        __add_factory_info_to_root(root, (char*)ezlopi_device_name_str, ezlopi_factory_info_v3_get_name());
        __add_factory_info_to_root(root, (char*)ezlopi_brand_str, ezlopi_factory_info_v3_get_brand());
        __add_factory_info_to_root(root, (char*)ezlopi_manufacturer_str, ezlopi_factory_info_v3_get_manufacturer());
        cJSON_AddNumberToObject(__FUNCTION__, root, ezlopi_serial_str, ezlopi_factory_info_v3_get_id());

        char* ssid = ezlopi_factory_info_v3_get_ssid();
        if (ssid)
        {
            cJSON_AddStringToObject(__FUNCTION__, root, ezlopi_wifi_ssid_str, (isprint(ssid[0])) ? ssid : ezlopi__str);
            ezlopi_free(__FUNCTION__, ssid);
        }

        cJSON_AddNumberToObject(__FUNCTION__, root, ezlopi_wifi_connection_status_str, ezlopi_wifi_got_ip());
#ifdef CONFIG_EZPI_ENABLE_PING
        uint8_t flag_internet_status = (EZLOPI_PING_STATUS_LIVE == ezlopi_ping_get_internet_status()) ? 1 : 0;
#else // CONFIG_EZPI_ENABLE_PING
        uint8_t flag_internet_status = 0;
#endif // CONFIG_EZPI_ENABLE_PING

        cJSON_AddNumberToObject(__FUNCTION__, root, ezlopi_internet_status_str, flag_internet_status);


        device_info = cJSON_PrintBuffered(__FUNCTION__, root, 4096, false);
        TRACE_D("length of 'device_info': %d", strlen(device_info));

        cJSON_Delete(__FUNCTION__, root);

        if (device_info)
        {
            TRACE_S("Created device info: %s", device_info);
        }
    }

    return device_info;
}

void __add_factory_info_to_root(cJSON* root, char* key, char* value)
{
    if (value)
    {
        if (isalpha(value[0]))
        {
            cJSON_AddStringToObject(__FUNCTION__, root, key, value);
        }
        else
        {
            cJSON_AddStringToObject(__FUNCTION__, root, key, ezlopi_unknown_str);
        }
        ezlopi_free(__FUNCTION__, value);
    }
}

#endif // CONFIG_EZPI_BLE_ENABLE