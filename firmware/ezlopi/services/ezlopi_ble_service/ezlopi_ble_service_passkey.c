#include "string.h"

#include "lwip/ip_addr.h"
#include "cJSON.h"

#include "trace.h"
#include "ezlopi_wifi.h"

#include "ezlopi_nvs.h"
#include "ezlopi_ble_gap.h"
#include "ezlopi_ble_gatt.h"
#include "ezlopi_ble_profile.h"
#include "ezlopi_nvs.h"

#include "ezlopi_ble_service.h"
#include "ezlopi_ble_buffer.h"

static void passkey_write_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);

void ezlopi_ble_service_passkey_init(void)
{
    esp_bt_uuid_t uuid;
    esp_gatt_perm_t permission;
    esp_gatt_char_prop_t properties;
    s_gatt_service_t *service;

    uuid.len = ESP_UUID_LEN_16;
    uuid.uuid.uuid16 = BLE_PASSKEY_SERVICE_UUID;
    service = ezlopi_ble_gatt_create_service(BLE_PASSKEY_SERVICE_HANDLE, &uuid);

    uuid.uuid.uuid16 = BLE_PASSKEY_CHAR_PASSKEY_UUID;
    uuid.len = ESP_UUID_LEN_16;
    permission = ESP_GATT_PERM_WRITE;
    properties = ESP_GATT_CHAR_PROP_BIT_WRITE;
    ezlopi_ble_gatt_add_characteristic(service, &uuid, permission, properties, NULL, passkey_write_func, NULL);
}

static void passkey_write_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{
    if (param->write.len == 4)
    {
        uint32_t passkey = *((uint32_t *)param->write.value);
        if (passkey < 1000000)
        {
            TRACE_D("New passkey: %d", passkey);
            ezlopi_ble_gap_set_passkey(passkey);
            ezlopi_nvs_write_ble_passkey(passkey);
            ezlopi_ble_gap_dissociate_bonded_devices();
        }
    }
}