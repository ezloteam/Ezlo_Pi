#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "cJSON.h"
#include "lwip/ip_addr.h"

#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"

#include "trace.h"
#include "sdkconfig.h"
#include "ezlopi_nvs.h"
#include "ezlopi_wifi.h"
#include "ezlopi_factory_info.h"

#include "ezlopi_ble_gap.h"
#include "ezlopi_ble_gatt.h"
#include "ezlopi_ble_profile.h"

#include "ezlopi_ble_service.h"
#include "ezlopi_ble_buffer.h"

extern void ezlopi_ble_service_user_id_init(void);
extern void ezlopi_ble_service_passkey_init(void);
extern void ezlopi_ble_service_wifi_profile_init(void);

static void ezlopi_ble_basic_init(void);
static void ezlopi_ble_start_secure_gatt_server(void);

void ezlopi_ble_service_init(void)
{
    ezlopi_ble_service_wifi_profile_init();
    ezlopi_ble_service_passkey_init();
    ezlopi_ble_service_user_id_init();

    ezlopi_ble_profile_print();
    ezlopi_ble_basic_init();

    CHECK_PRINT_ERROR(esp_ble_gatts_app_register(WIFI_CREDS_SERVICE_HANDLE), "gatts app-0 register error");
    CHECK_PRINT_ERROR(esp_ble_gatts_app_register(WIFI_STATUS_SERVICE_HANDLE), "gatts app-1 register error");
    CHECK_PRINT_ERROR(esp_ble_gatts_app_register(WIFI_ERROR_SERVICE_HANDLE), "gatts app-2 register error");
    CHECK_PRINT_ERROR(esp_ble_gatts_app_register(BLE_PASSKEY_SERVICE_HANDLE), "gatts app-3 register error");
    CHECK_PRINT_ERROR(esp_ble_gatts_app_register(BLE_USER_ID_SERVICE_HANDLE), "gatts app-4 register error");
    CHECK_PRINT_ERROR(esp_ble_gatt_set_local_mtu(517), "set local  MTU failed");
    ezlopi_ble_start_secure_gatt_server();
}

static void ezlopi_ble_start_secure_gatt_server(void)
{
    const uint32_t default_passkey = 123456;
    uint32_t passkey;
    ezlopi_nvs_read_ble_passkey(&passkey);
    passkey = (0 == passkey) ? default_passkey : passkey;
    passkey = (passkey > 999999) ? default_passkey : passkey;
    TRACE_D("Ble passkey: %d", passkey);

    const esp_ble_auth_req_t auth_req = ESP_LE_AUTH_REQ_SC_MITM_BOND; // ESP_LE_AUTH_REQ_BOND_MITM;
    const esp_ble_io_cap_t iocap = ESP_IO_CAP_OUT;
    const uint8_t key_size = 16;
    const uint8_t auth_option = ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_DISABLE;
    const uint8_t oob_support = ESP_BLE_OOB_ENABLE; // ESP_BLE_OOB_DISABLE;
    const uint8_t init_key = (ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
    const uint8_t rsp_key = (ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);

    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_SET_STATIC_PASSKEY, &passkey, sizeof(uint32_t)), "failed -set - ESP_BLE_SM_SET_STATIC_PASSKEY");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req, sizeof(uint8_t)), "failed -set - ESP_BLE_SM_AUTHEN_REQ_MODE");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap, sizeof(uint8_t)), "failed -set - ESP_BLE_SM_IOCAP_MODE");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_MAX_KEY_SIZE, &key_size, sizeof(uint8_t)), "failed -set - ESP_BLE_SM_MAX_KEY_SIZE");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH, &auth_option, sizeof(uint8_t)), "failed -set - ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_OOB_SUPPORT, &oob_support, sizeof(uint8_t)), "failed -set - ESP_BLE_SM_OOB_SUPPORT");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_SET_INIT_KEY, &init_key, sizeof(uint8_t)), "failed -set - ESP_BLE_SM_SET_INIT_KEY");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY, &rsp_key, sizeof(uint8_t)), "failed -set - ESP_BLE_SM_SET_RSP_KEY");
}

static void ezlopi_ble_basic_init(void)
{
    char ble_device_name[32];
    s_ezlopi_factory_info_t *factory = ezlopi_factory_info_get_info();
    snprintf(ble_device_name, sizeof(ble_device_name), "ezlopi_%llu", factory->id);

    static esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    CHECK_PRINT_ERROR(esp_bt_controller_init(&bt_cfg), "initialize controller failed");
    CHECK_PRINT_ERROR(esp_bt_controller_enable(ESP_BT_MODE_BLE), "enable controller failed");
    CHECK_PRINT_ERROR(esp_bluedroid_init(), "init bluetooth failed");
    CHECK_PRINT_ERROR(esp_bluedroid_enable(), "enable bluetooth failed");
    CHECK_PRINT_ERROR(esp_ble_gatts_register_callback(ezlopi_ble_gatts_event_handler), "gatts register error, error code");
    CHECK_PRINT_ERROR(esp_ble_gap_register_callback(ezlopi_ble_gap_event_handler), "gap register error");
    CHECK_PRINT_ERROR(esp_ble_gap_set_device_name(ble_device_name), "Set device name failed!");
}