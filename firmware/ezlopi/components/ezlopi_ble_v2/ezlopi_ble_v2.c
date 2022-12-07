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
#include "ezlopi_ble_buffer.h"

static void ezlopi_ble_set_wifi_profile(void);
static void ezlopi_ble_start_secure_gatt_server(void);

void ezlopi_ble_v2_init(void)
{
    char ble_device_name[32];
    s_ezlopi_factory_info_t *factory = ezlopi_factory_info_get_info();
    snprintf(ble_device_name, sizeof(ble_device_name), "ezlopi_%llu", factory->id);

    ezlopi_ble_set_wifi_profile();
    ezlopi_ble_profile_print();

    static esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    CHECK_PRINT_ERROR(esp_bt_controller_init(&bt_cfg), "initialize controller failed");
    CHECK_PRINT_ERROR(esp_bt_controller_enable(ESP_BT_MODE_BLE), "enable controller failed");
    CHECK_PRINT_ERROR(esp_bluedroid_init(), "init bluetooth failed");
    CHECK_PRINT_ERROR(esp_bluedroid_enable(), "enable bluetooth failed");
    CHECK_PRINT_ERROR(esp_ble_gatts_register_callback(ezlopi_ble_gatts_event_handler), "gatts register error, error code");
    CHECK_PRINT_ERROR(esp_ble_gap_register_callback(ezlopi_ble_gap_event_handler), "gap register error");
    CHECK_PRINT_ERROR(esp_ble_gap_set_device_name(ble_device_name), "Set device name failed!");

    CHECK_PRINT_ERROR(esp_ble_gatts_app_register(0), "gatts app register error");
    CHECK_PRINT_ERROR(esp_ble_gatt_set_local_mtu(517), "set local  MTU failed");
    ezlopi_ble_start_secure_gatt_server();

    // CHECK_PRINT_ERROR(esp_ble_gatts_app_register(1), "gatts app register error");
    // CHECK_PRINT_ERROR(esp_ble_gatts_app_register(PROFILE_WIFI_STATUS_APP_ID), "gatts app register error");
    // CHECK_PRINT_ERROR(esp_ble_gatts_app_register(PROFILE_WIFI_ERROR_APP_ID), "gatts app register error");
}

static s_linked_buffer_t *wifi_creds_linked_buffer = NULL;

void wifi_creds_parse_and_connect(uint8_t *value, uint32_t len)
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

void wifi_creds_write_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{
    TRACE_D("Write function called!");
    if (0 == param->write.is_prep) // Data received in single packet
    {
        dump("GATT_WRITE_EVT value", param->write.value, 0, param->write.len);

        if (param->write.len == 2)
        {
            uint16_t descr_value = (param->write.value[1] << 8) | param->write.value[0];
            if (descr_value == 0x0001)
            {
            }
            else if (descr_value == 0x0002)
            {
            }
            else if (descr_value == 0x0000)
            {
                TRACE_I("notify/indicate disable");
            }
            else
            {
                TRACE_E("unknown descr value");
            }
        }
        else if ((NULL != param->write.value) && (param->write.len > 2))
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

void wifi_creds_read_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{
    static char *json_str_wifi_info;
    TRACE_W("1");

    if (NULL == json_str_wifi_info)
    {
        TRACE_W("2");
        char wifi_creds[64];
        memset(wifi_creds, 0, sizeof(wifi_creds));
        ezlopi_nvs_read_wifi(wifi_creds, sizeof(wifi_creds));

        cJSON *cjson_wifi_info = cJSON_CreateObject();
        if (cjson_wifi_info)
        {
            TRACE_W("3");
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
                TRACE_W("4");
                cJSON_Minify(json_str_wifi_info);
                printf("WiFi read: %s", json_str_wifi_info);
            }

            cJSON_Delete(cjson_wifi_info);
        }
    }

    if (NULL != json_str_wifi_info)
    {
        TRACE_W("5");
        if (value)
        {
            TRACE_W("6");
            if ((0 != strlen(json_str_wifi_info)) && (strlen(json_str_wifi_info) > param->read.offset))
            {

                TRACE_W("7");
                strncpy((char *)value->value, json_str_wifi_info + param->read.offset, ESP_GATT_MAX_ATTR_LEN);
                value->len = strlen((const char *)value->value);
            }
            else
            {
                TRACE_W("8");
                value->len = 1;
                value->value[0] = 0; // Read 0 if the device not provisioned yet.
            }

            TRACE_W("param->read.offset: %d | %d : ", param->read.offset, strlen(json_str_wifi_info));

            if (param->read.offset >= (strlen(json_str_wifi_info) - 1))
            {
                TRACE_W("11");
                free(json_str_wifi_info);
                json_str_wifi_info = NULL;
            }
        }
    }
    else
    {
        TRACE_W("9");
        if (value)
        {
            TRACE_W("10");
            value->len = 1;
            value->value[0] = 0; // Read 0 if the device not provisioned yet.
        }
    }
}

void wifi_creds_write_exec_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{
    TRACE_D("Write execute function called.");
    ezlopi_ble_buffer_accumulate_to_start(wifi_creds_linked_buffer);
    wifi_creds_parse_and_connect(wifi_creds_linked_buffer->buffer, wifi_creds_linked_buffer->len);
    ezlopi_ble_buffer_free_buffer(wifi_creds_linked_buffer);
    wifi_creds_linked_buffer = NULL;
}

static void ezlopi_ble_set_wifi_profile(void)
{
    // 77880001-d229-11e4-8689-0002a5d5c51b
    esp_bt_uuid_t uuid = {.len = ESP_UUID_LEN_128, .uuid.uuid128 = {0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xEE, 0x00, 0x00, 0x00}};
    s_gatt_service_t *service = ezlopi_ble_gatt_create_service(0, &uuid);

    uuid.len = ESP_UUID_LEN_16;
    uuid.uuid.uuid16 = 0xEE01;
    esp_gatt_perm_t permission = ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE;
    esp_gatt_char_prop_t properties = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
    s_gatt_char_t *character = ezlopi_ble_gatt_add_characteristic(service, &uuid, permission, properties, wifi_creds_read_func, wifi_creds_write_func, wifi_creds_write_exec_func);

    uuid.len = ESP_UUID_LEN_16;
    uuid.uuid.uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
    s_gatt_descr_t *descriptor = ezlopi_ble_gatt_add_descriptor(character, &uuid, permission, NULL, NULL, NULL);

#if 0
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
#endif
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
