#include "ble_server.h"
#include "ble_config.h"
#include "ble_gap_server_gap.h"

#include "esp_bt.h"

void ble_app_init(void)
{
    static esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
    esp_bt_controller_init(&bt_cfg);
    esp_bt_controller_enable(ESP_BT_MODE_BLE);
    esp_bluedroid_init();
    esp_bluedroid_enable();
    esp_ble_gatts_register_callback(gatts_event_handler);
    esp_ble_gap_register_callback(gaps_event_handler);
    esp_ble_gatts_app_register(0);
}