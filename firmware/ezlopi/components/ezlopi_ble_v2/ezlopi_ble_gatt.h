#ifndef __EZLOPI_BLE_GATT_H__
#define __EZLOPI_BLE_GATT_H__
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "ezlopi_ble_config.h"

s_gatt_service_t *ezlopi_ble_gatt_create_service(esp_bt_uuid_t *service_uuid);
s_gatt_char_t *ezlopi_ble_gatt_add_characteristic(s_gatt_service_t *service_obj, esp_bt_uuid_t *uuid, esp_gatt_perm_t permission, esp_gatt_char_prop_t properties);
s_gatt_descr_t *ezlopi_ble_gatt_add_descriptor(s_gatt_char_t *charcteristic, esp_bt_uuid_t *uuid, esp_gatt_perm_t permission);

void ezlopi_ble_gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

void ezlopi_ble_gatt_print_profiles(void);

#endif // __EZLOPI_BLE_GATT_H__
