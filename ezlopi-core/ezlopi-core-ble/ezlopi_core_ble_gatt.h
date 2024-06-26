#ifndef _EZLOPI_CORE_BLE_GATT_H_
#define _EZLOPI_CORE_BLE_GATT_H_

#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_BLE_ENABLE

#include "ezlopi_core_ble_config.h"

uint16_t ezlopi_ble_gatt_get_max_data_size(void);
void ezlopi_ble_gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t* param);
void ezlopi_ble_gatts_characteristic_notify(s_gatt_service_t* service, s_gatt_char_t* characteristics, esp_gatt_value_t* value);

#endif // CONFIG_EZPI_BLE_ENABLE

#endif // _EZLOPI_CORE_BLE_GATT_H_
