
#ifndef _EZLOPI_SERVICE_BLE_H_
#define _EZLOPI_SERVICE_BLE_H_

#ifdef CONFIG_EZPI_BLE_ENABLE

#define CHECK_PRINT_ERROR(x, msg)                                    \
    {                                                                \
        if (x)                                                       \
        {                                                            \
            TRACE_E("%s %s: %s", __func__, msg, esp_err_to_name(x)); \
            return;                                                  \
        }                                                            \
    }

#define BLE_WIFI_SERVICE_HANDLE 0
#define BLE_WIFI_SERVICE_UUID 0x00E1
#define BLE_WIFI_CHAR_CREDS_UUID 0xE101
#define BLE_WIFI_CHAR_STATUS_UUID 0xE102
#define BLE_WIFI_CHAR_ERROR_UUID 0xE103

#define BLE_SECURITY_SERVICE_HANDLE 1
#define BLE_SECURITY_SERVICE_UUID 0x00E2
#define BLE_SECURITY_CHAR_PASSKEY_UUID 0xE201
#define BLE_SECURITY_FACTORY_RESET_CHAR_UUID 0xE202
#define BLE_SECURITY_RESET_CHAR_UUID 0xE203

#define BLE_PROVISIONING_ID_HANDLE 2
#define BLE_PROVISIONING_SERVICE_UUID 0x00E3
#define BLE_PROVISIONING_CHAR_UUID 0xE301
#define BLE_PROVISIONING_STATUS_CHAR_UUID 0xE302

#define BLE_DEVICE_INFO_ID_HANDLE 3
#define BLE_DEVICE_INFO_SERVICE_UUID 0x00E4
#define BLE_DEVICE_INFO_CHAR_UUID 0xE401

#define EZPI_BLE_CHAR_API_VERSION_INFO_UUID 0xE402      // EzloPi Firmware Info
#define EZPI_BLE_CHAR_FIRMWARE_INFO_UUID 0xE403         // EzloPi Firmware Info
#define EZPI_BLE_CHAR_CHIP_INFO_UUID 0xE404             // EzloPi Chip Info
#define EZPI_BLE_CHAR_FIRMWARE_SDK_INFO_UUID 0xE405     // EzloPi firmware sdk Info
#define EZPI_BLE_CHAR_DEV_STATE_INFO_UUID 0xE406        // EzloPi device state Info
#define EZPI_BLE_CHAR_SER_CONFIG_INFO_UUID 0xE407       // EzloPi serial config Info
#define EZPI_BLE_CHAR_EZPI_CLOUD_INFO_UUID 0xE408       // EzloPi ezlopi cloud Info
#define EZPI_BLE_CHAR_OEM_INFO_UUID 0xE409              // EzloPi oem Info
#define EZPI_BLE_CHAR_NETWORK_INFO_UUID 0xE40A          // EzloPi Network Info

#define BLE_DYNAMIC_CONFIG_HANDLE 4
#define BLE_DYNAMIC_CONFIG_SERVICE_UUID 0x00E5
#define BLE_DYNAMIC_CONFIG_CHAR_UUID 0xE501


void ezlopi_ble_service_init(void);
int ezlopi_ble_service_get_ble_mac(uint8_t mac[6]);

#endif // CONFIG_EZPI_BLE_ENABLE

#endif //  _EZLOPI_SERVICE_BLE_H_
