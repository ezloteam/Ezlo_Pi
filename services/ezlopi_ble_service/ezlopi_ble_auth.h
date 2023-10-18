#ifndef __EZLOPI_BLE_AUTH__
#define __EZLOPI_BLE_AUTH__

typedef enum e_auth_status
{
    BLE_AUTH_SUCCESS = 1,

    BLE_AUTH_STATUS_UNKNOWN = 0,
    BLE_AUTH_USER_ID_NOT_FOUND = -1,
    BLE_AUTH_USER_ID_NOT_MATCHED = -2,
    BLE_AUTH_USER_ID_NVS_WRITE_FAILED = -3,
} e_auth_status_t;

e_auth_status_t ezlopi_ble_auth_last_status(void);
e_auth_status_t ezlopi_ble_auth_store_user_id(char *user_id);
e_auth_status_t ezlopi_ble_auth_check_user_id(char *user_id);
char *ezlopi_ble_auth_status_to_string(e_auth_status_t status);

#endif // __EZLOPI_BLE_AUTH__
