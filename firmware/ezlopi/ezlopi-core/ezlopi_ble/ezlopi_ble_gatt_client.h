// // /********************************************************************
// // ******************** Created by Noor_haq on 06/02/22.
// // ******************** Last modified on 06/13/22
// // ********************************************************************/

#ifndef ESP_CONFIGS_GATT_CLIENT_H
#define ESP_CONFIGS_GATT_CLIENT_H
#ifdef __cplusplus
extern "C" {
#endif

#define REMOTE_SERVICE_UUID        0x00FF
#define REMOTE_NOTIFY_CHAR_UUID    0xFF01
#define PROFILE_NUM      1
#define PROFILE_A_APP_ID 0
#define INVALID_HANDLE   0
#define REMOTE_DEVICE_NAME "EZLO_GATTS"

void GATT_CLIENT_MAIN(void);

#ifdef __cplusplus
}
#endif
#endif