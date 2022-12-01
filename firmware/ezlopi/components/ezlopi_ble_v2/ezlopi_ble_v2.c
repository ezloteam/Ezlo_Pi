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

#include "ezlopi_ble_v2.h"

static void ezlopi_ble_start_secure_gatt_server(void);

void ezlopi_ble_v2_init(void)
{
    static esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    CHECK_PRINT_ERROR(esp_bt_controller_init(&bt_cfg), "initialize controller failed");
    CHECK_PRINT_ERROR(esp_bt_controller_enable(ESP_BT_MODE_BLE), "enable controller failed");
    CHECK_PRINT_ERROR(esp_bluedroid_init(), "init bluetooth failed");
    CHECK_PRINT_ERROR(esp_bluedroid_enable(), "enable bluetooth failed");
    CHECK_PRINT_ERROR(esp_ble_gatts_register_callback(ezlopi_ble_gatts_event_handler), "gatts register error, error code");
    CHECK_PRINT_ERROR(esp_ble_gap_register_callback(ezlopi_ble_gap_event_handler), "gap register error");
    CHECK_PRINT_ERROR(esp_ble_gatt_set_local_mtu(517), "set local  MTU failed");

    ezlopi_ble_start_secure_gatt_server();

    esp_bt_uuid_t uuid = {.len = ESP_UUID_LEN_128, .uuid.uuid128 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}};
    s_gatt_service_t *service = ezlopi_ble_gatt_create_service(&uuid);
    uuid.len = ESP_UUID_LEN_16;
    uuid.uuid.uuid16 = 0x2902;
    s_gatt_char_t *character = ezlopi_ble_gatt_add_characteristic(service, &uuid, 0, 0);

    uuid.uuid.uuid16 = 0x2903;
    s_gatt_descr_t *descriptor = ezlopi_ble_gatt_add_descriptor(character, &uuid, 0);

    uuid.uuid.uuid16 = 0x2904;
    descriptor = ezlopi_ble_gatt_add_descriptor(character, &uuid, 0);

    uuid.uuid.uuid16 = 0x1902;
    character = ezlopi_ble_gatt_add_characteristic(service, &uuid, 0, 0);

    uuid.uuid.uuid16 = 0x1903;
    descriptor = ezlopi_ble_gatt_add_descriptor(character, &uuid, 0);

    uuid.uuid.uuid16 = 0x1904;
    descriptor = ezlopi_ble_gatt_add_descriptor(character, &uuid, 0);

    uuid.len = ESP_UUID_LEN_128;
    uuid.uuid.uuid128[0] = 1;
    uuid.uuid.uuid128[1] = 1;
    uuid.uuid.uuid128[2] = 1;
    uuid.uuid.uuid128[3] = 1;
    uuid.uuid.uuid128[4] = 1;
    uuid.uuid.uuid128[5] = 1;
    uuid.uuid.uuid128[6] = 1;
    uuid.uuid.uuid128[7] = 1;
    uuid.uuid.uuid128[8] = 1;
    uuid.uuid.uuid128[9] = 1;
    service = ezlopi_ble_gatt_create_service(&uuid);
    uuid.len = ESP_UUID_LEN_16;
    uuid.uuid.uuid16 = 0x5502;
    character = ezlopi_ble_gatt_add_characteristic(service, &uuid, 0, 0);

    uuid.uuid.uuid16 = 0x5503;
    descriptor = ezlopi_ble_gatt_add_descriptor(character, &uuid, 0);

    uuid.uuid.uuid16 = 0x5504;
    descriptor = ezlopi_ble_gatt_add_descriptor(character, &uuid, 0);

    uuid.uuid.uuid16 = 0x1502;
    character = ezlopi_ble_gatt_add_characteristic(service, &uuid, 0, 0);

    uuid.uuid.uuid16 = 0x1503;
    descriptor = ezlopi_ble_gatt_add_descriptor(character, &uuid, 0);

    uuid.uuid.uuid16 = 0x1504;
    descriptor = ezlopi_ble_gatt_add_descriptor(character, &uuid, 0);

    CHECK_PRINT_ERROR(esp_ble_gatts_app_register(0), "gatts app register error");
    // CHECK_PRINT_ERROR(esp_ble_gatts_app_register(PROFILE_WIFI_STATUS_APP_ID), "gatts app register error");
    // CHECK_PRINT_ERROR(esp_ble_gatts_app_register(PROFILE_WIFI_ERROR_APP_ID), "gatts app register error");
}

static void ezlopi_ble_start_secure_gatt_server(void)
{
    const uint32_t passkey = 123456;
    const esp_ble_auth_req_t auth_req = ESP_LE_AUTH_REQ_BOND_MITM;
    const esp_ble_io_cap_t iocap = ESP_IO_CAP_OUT;
    const uint8_t key_size = 16;
    const uint8_t auth_option = ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_DISABLE;
    const uint8_t oob_support = ESP_BLE_OOB_DISABLE;
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