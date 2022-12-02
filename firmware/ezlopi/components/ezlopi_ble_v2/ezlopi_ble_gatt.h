#ifndef __EZLOPI_BLE_GATT_H__
#define __EZLOPI_BLE_GATT_H__

#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "ezlopi_ble_config.h"

void ezlopi_ble_gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

#endif // __EZLOPI_BLE_GATT_H__
