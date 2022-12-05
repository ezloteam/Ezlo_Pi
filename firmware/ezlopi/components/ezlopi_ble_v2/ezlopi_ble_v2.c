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

#include "ezlopi_ble_v2.h"

static void ezlopi_ble_set_profiles(void);
static void ezlopi_ble_set_scan_params(void);
static void ezlopi_ble_start_secure_gatt_server(void);

static uint8_t manufacturer[] = {'e', 'z', 'l', 'o', 'p', 'i'};
static esp_ble_adv_data_t adv_data =
    {
        .set_scan_rsp = false,
        .include_name = true,
        .include_txpower = true,
        .min_interval = 0x0006,
        .max_interval = 0x0010,
        .appearance = 0x00,
        .manufacturer_len = sizeof(manufacturer),
        .p_manufacturer_data = manufacturer,
        .service_data_len = 0,
        .p_service_data = NULL,
        .service_uuid_len = 0,
        .p_service_uuid = NULL,
        .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

void ezlopi_ble_v2_init(void)
{
    ezlopi_ble_set_profiles();

    static esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    CHECK_PRINT_ERROR(esp_bt_controller_init(&bt_cfg), "initialize controller failed");
    CHECK_PRINT_ERROR(esp_bt_controller_enable(ESP_BT_MODE_BLE), "enable controller failed");
    CHECK_PRINT_ERROR(esp_bluedroid_init(), "init bluetooth failed");
    CHECK_PRINT_ERROR(esp_bluedroid_enable(), "enable bluetooth failed");
    CHECK_PRINT_ERROR(esp_ble_gatts_register_callback(ezlopi_ble_gatts_event_handler), "gatts register error, error code");
    CHECK_PRINT_ERROR(esp_ble_gap_register_callback(ezlopi_ble_gap_event_handler), "gap register error");
    CHECK_PRINT_ERROR(esp_ble_gatt_set_local_mtu(517), "set local  MTU failed");
    CHECK_PRINT_ERROR(esp_ble_gap_set_device_name("ezlopi_krishna"), "Set device name faile!");
    CHECK_PRINT_ERROR(esp_ble_gap_config_adv_data(&adv_data), "config advertising data failed!");

    ezlopi_ble_start_secure_gatt_server();

    ezlopi_ble_set_scan_params();

    CHECK_PRINT_ERROR(esp_ble_gatts_app_register(0), "gatts app register error");
    // CHECK_PRINT_ERROR(esp_ble_gatts_app_register(1), "gatts app register error");
    // CHECK_PRINT_ERROR(esp_ble_gatts_app_register(PROFILE_WIFI_STATUS_APP_ID), "gatts app register error");
    // CHECK_PRINT_ERROR(esp_ble_gatts_app_register(PROFILE_WIFI_ERROR_APP_ID), "gatts app register error");
}

void write_func(esp_attr_value_t *value)
{
    TRACE_D("Write function called!");
}

void read_func(esp_attr_value_t *value)
{
    TRACE_D("Read function called!");
}

static void ezlopi_ble_set_profiles(void)
{
    esp_bt_uuid_t uuid = {.len = ESP_UUID_LEN_128, .uuid.uuid128 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}};
    s_gatt_service_t *service = ezlopi_ble_gatt_create_service(0, &uuid);

    uuid.len = ESP_UUID_LEN_16;
    uuid.uuid.uuid16 = 0x2902;
    esp_gatt_perm_t permission = ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE;
    esp_gatt_char_prop_t properties = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
    s_gatt_char_t *character = ezlopi_ble_gatt_add_characteristic(service, &uuid, permission, properties, read_func, write_func);

    uuid.len = ESP_UUID_LEN_16;
    uuid.uuid.uuid16 = 0x2903;
    s_gatt_descr_t *descriptor = ezlopi_ble_gatt_add_descriptor(character, &uuid, permission, read_func, write_func);

    // uuid.uuid.uuid16 = 0x2904;
    // descriptor = ezlopi_ble_gatt_add_descriptor(character, &uuid, permission, read_func, write_func);

    // uuid.uuid.uuid16 = 0x1902;
    // character = ezlopi_ble_gatt_add_characteristic(service, &uuid, permission, properties, read_func, write_func);

    // uuid.uuid.uuid16 = 0x1903;
    // descriptor = ezlopi_ble_gatt_add_descriptor(character, &uuid, permission, read_func, write_func);

    // uuid.uuid.uuid16 = 0x1904;
    // descriptor = ezlopi_ble_gatt_add_descriptor(character, &uuid, permission, read_func, write_func);

    // uuid.len = ESP_UUID_LEN_128;
    // uuid.uuid.uuid128[0] = 1;
    // uuid.uuid.uuid128[1] = 1;
    // uuid.uuid.uuid128[2] = 1;
    // uuid.uuid.uuid128[3] = 1;
    // uuid.uuid.uuid128[4] = 1;
    // uuid.uuid.uuid128[5] = 1;
    // service = ezlopi_ble_gatt_create_service(1, &uuid);

    // uuid.len = ESP_UUID_LEN_16;
    // uuid.uuid.uuid16 = 0x5502;
    // character = ezlopi_ble_gatt_add_characteristic(service, &uuid, permission, properties, read_func, write_func);

    // uuid.uuid.uuid16 = 0x5503;
    // descriptor = ezlopi_ble_gatt_add_descriptor(character, &uuid, permission, read_func, write_func);

    // uuid.uuid.uuid16 = 0x5504;
    // descriptor = ezlopi_ble_gatt_add_descriptor(character, &uuid, permission, read_func, write_func);

    // uuid.uuid.uuid16 = 0x1502;
    // character = ezlopi_ble_gatt_add_characteristic(service, &uuid, permission, properties, read_func, write_func);

    // uuid.uuid.uuid16 = 0x1503;
    // descriptor = ezlopi_ble_gatt_add_descriptor(character, &uuid, permission, read_func, write_func);

    // uuid.uuid.uuid16 = 0x1504;
    // descriptor = ezlopi_ble_gatt_add_descriptor(character, &uuid, permission, read_func, write_func);

    ezlopi_ble_profile_print();
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

static void ezlopi_ble_set_scan_params(void)
{ // scan response data
    static esp_ble_adv_data_t scan_rsp_data = {
        .set_scan_rsp = true,
        .include_name = true,
        .include_txpower = true,
        .appearance = 0x00,
        .manufacturer_len = sizeof(manufacturer), // TEST_MANUFACTURER_DATA_LEN,
        .p_manufacturer_data = manufacturer,      //&manufacturer[0],
        .service_data_len = 0,
        .p_service_data = NULL,
        .service_uuid_len = 0,
        .p_service_uuid = NULL,
        .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
    };

    int all_service_uuid_len = 0;
    s_gatt_service_t *service_head = ezlopi_ble_profile_get_head();
    while (service_head)
    {
        all_service_uuid_len += service_head->service_id.id.uuid.len;
        service_head = service_head->next;
    }

    if (all_service_uuid_len)
    {
        uint8_t *service_uuids = malloc(ezlopi_ble_gatt_number_of_services() * ESP_UUID_LEN_128);
        if (service_uuids)
        {
            int uuid_pos = 0;
            service_head = ezlopi_ble_profile_get_head();
            while (service_head)
            {
                memcpy(&service_uuids[uuid_pos], &service_head->service_id.id.uuid.uuid.uuid128, service_head->service_id.id.uuid.len);
                uuid_pos += service_head->service_id.id.uuid.len;
                service_head = service_head->next;
            }

            scan_rsp_data.service_uuid_len = all_service_uuid_len;
            scan_rsp_data.p_service_uuid = service_uuids;
            esp_ble_gap_config_adv_data(&scan_rsp_data);
        }
    }
}