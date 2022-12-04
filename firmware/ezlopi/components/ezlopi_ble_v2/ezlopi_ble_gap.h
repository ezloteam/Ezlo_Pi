#ifndef __EZLOPI_BLE_GAP_H__
#define __EZLOPI_BLE_GAP_H__

#include "esp_gap_ble_api.h"
#include "esp_bt_defs.h"
#include "ezlopi_ble_config.h"

void ezlopi_ble_gap_start_advertising(void);
void ezlopi_ble_gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);

#endif //  __EZLOPI_BLE_GAP_H__
