#ifndef __EZLOPI_BLE_GATT_H__
#define __EZLOPI_BLE_GATT_H__

#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "ezlopi_ble_config.h"

uint16_t ezlopi_ble_gatt_get_max_data_size(void);
void ezlopi_ble_gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
void ezlopi_ble_gatts_characteristic_notify(s_gatt_service_t *service, s_gatt_char_t *characteristics, esp_gatt_value_t *value);

#endif // __EZLOPI_BLE_GATT_H__