
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_main.h"

#include "sdkconfig.h"
#include "ble_config.h"

static uint8_t ble_manufacturer[BLE_MANUFACTURER_DATA_LEN] = BLE_MANUFACTURER_DATA;

/* BLE GAP advertising data; TODO: figure out meaning of the flags */
static esp_ble_adv_data_t ble_adv_data = {
    .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = true,
    .min_interval = BLE_ADV_MIN_INTERVAL,
    .max_interval = BLE_ADV_MAX_INTERVAL,
    .appearance = BLE_APPEARANCE,
    .manufacturer_len = BLE_MANUFACTURER_DATA_LEN,
    .p_manufacturer_data = (uint8_t *)ble_manufacturer,
    .service_data_len = BLE_SERVICE_DATA_LEN,
    .p_service_data = BLE_SERVICE_DATA,
    .service_uuid_len = BLE_SERVICE_UUID_SIZE,
    .p_service_uuid = ble_service_uuid128,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

/* This function initialises the GAP data.
 * ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT is sent to gap_event_handler() below
 * afterwards.
 */
void gaps_init(void)
{
    esp_err_t ret;

    char device_name[BLE_DEVICE_NAME_LEN];
    snprintf(device_name, BLE_DEVICE_NAME_LEN, "EG Teacher's Button %d", *gatts_char[GATTS_BUTTON_NUMBER_CHAR_POS].char_val->attr_value); // copy configured button number into the device name
    esp_ble_gap_set_device_name(device_name);

    ret = esp_ble_gap_config_adv_data(&ble_adv_data);
    ESP_LOGI(GATTS_TAG, "esp_ble_gap_config_adv_data %d", ret);
}