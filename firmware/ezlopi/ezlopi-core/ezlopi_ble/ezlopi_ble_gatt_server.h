// /********************************************************************
// ******************** Created by Noor_haq on 06/02/22.
// ******************** Last modified on 06/13/22
// ********************************************************************/

#ifndef __EZLOPI_BLE_GATT_SERVER_H__
#define __EZLOPI_BLE_GATT_SERVER_H__
#include "esp_gatts_api.h"

#ifdef __cplusplus
extern "C"
{
#endif
#define GATTS_SERVICE_UUID_TEST_A 0x00FF
#define GATTS_CHAR_UUID_TEST_A 0xFF01
#define GATTS_DESCR_UUID_TEST_A 0x3333
#define GATTS_NUM_HANDLE_TEST_A 4

#define GATTS_SERVICE_UUID_TEST_B 0x00EE
#define GATTS_CHAR_UUID_TEST_B 0xEE01
#define GATTS_DESCR_UUID_TEST_B 0x2222
#define GATTS_NUM_HANDLE_TEST_B 4

#define TEST_MANUFACTURER_DATA_LEN 17

    typedef struct gatts_profile_inst
    {
        esp_gatts_cb_t gatts_cb;
        uint16_t gatts_if;
        uint16_t app_id;
        uint16_t conn_id;
        uint16_t service_handle;
        esp_gatt_srvc_id_t service_id;
        uint16_t char_handle;
        esp_bt_uuid_t char_uuid;
        esp_gatt_perm_t perm;
        esp_gatt_char_prop_t property;
        uint16_t descr_handle;
        esp_bt_uuid_t descr_uuid;
    } gatts_profile_inst_t;

    typedef struct
    {
        uint8_t *prepare_buf;
        int prepare_len;
    } prepare_type_env_t;

    void GATT_SERVER_MAIN(void);

#ifdef __cplusplus
}
#endif

#endif // __EZLOPI_BLE_GATT_SERVER_H__
