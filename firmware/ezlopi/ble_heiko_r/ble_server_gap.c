
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "bt.h"
#include "bta_api.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_main.h"

#include "sdkconfig.h"
#include "ble_config.h"

/* This function initialises the GAP data.
 * ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT is sent to gap_event_handler() below
 * afterwards.
 */
void gaps_init(void)
{
    esp_err_t ret;

    char device_name[BLE_DEVICE_NAME_LEN];
    sprintf(device_name, "EG Teacher's Button %02u", *gatts_char[GATTS_BUTTON_NUMBER_CHAR_POS].char_val->attr_value); // copy configured button number into the device name
    esp_ble_gap_set_device_name(device_name);

    ret = esp_ble_gap_config_adv_data(&ble_adv_data);
    ESP_LOGI(GATTS_TAG, "esp_ble_gap_config_adv_data %d", ret);
}