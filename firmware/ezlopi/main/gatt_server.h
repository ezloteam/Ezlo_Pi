// /********************************************************************
// ******************** Created by Noor_haq on 06/02/22.
// ******************** Last modified on 06/13/22
// ********************************************************************/

#ifndef ESP_CONFIGS_GATT_SERVER_H
#define ESP_CONFIGS_GATT_SERVER_H
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

    void GATT_SERVER_MAIN(void);

#ifdef __cplusplus
}
#endif
#endif