#ifndef _EZLOPI_CORE_BLE_GAP_H_
#define _EZLOPI_CORE_BLE_GAP_H_

#include "esp_gap_ble_api.h"
#include "ezlopi_core_ble_config.h"

#define EZLOPI_BLE_ENALBE_PASSKEY 0
#define EZLOPI_BLE_ENALBE_PAIRING 1

void ezlopi_ble_setup_adv_config(void);
void ezlopi_ble_gap_set_passkey(uint32_t passkey);
void ezlopi_ble_gap_dissociate_bonded_devices(void);
void ezlopi_ble_gap_config_adv_data(void);
void ezlopi_ble_gap_config_scan_rsp_data(void);
void ezlopi_ble_gap_start_advertising(void);
void ezlopi_ble_gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);

#endif //  _EZLOPI_CORE_BLE_GAP_H_