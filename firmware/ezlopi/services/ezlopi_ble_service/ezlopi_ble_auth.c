#include "string.h"
#include "ezlopi_nvs.h"
#include "ezlopi_ble_auth.h"

static e_auth_status_t last_auth_status = BLE_AUTH_STATUS_UNKNOWN;

e_auth_status_t ezlopi_ble_auth_last_status(void)
{
    return last_auth_status;
}

e_auth_status_t ezlopi_ble_auth_store_user_id(char *user_id)
{
    if (user_id)
    {
        if (1 == ezlopi_nvs_write_user_id_str(user_id))
        {
            last_auth_status = BLE_AUTH_SUCCESS;
        }
        else
        {
            last_auth_status = BLE_AUTH_USER_ID_NVS_WRITE_FAILED;
        }
    }

    return last_auth_status;
}

e_auth_status_t ezlopi_ble_auth_check_user_id(char *user_id)
{
    char *stored_user_id = NULL;

    if (1 == ezlopi_nvs_read_user_id_str(&stored_user_id))
    {

        if (stored_user_id && user_id)
        {
            if (strncmp(stored_user_id, user_id, strlen(stored_user_id)))
            {
                last_auth_status = BLE_AUTH_SUCCESS;
            }
            else
            {
                last_auth_status = BLE_AUTH_USER_ID_NOT_MATCHED;
            }
        }
    }
    else
    {
        last_auth_status = BLE_AUTH_USER_ID_NOT_FOUND;
    }

    return last_auth_status;
}

char *ezlopi_ble_auth_status_to_string(e_auth_status_t status)
{
    char *ret = NULL;

    switch (status)
    {
    case BLE_AUTH_STATUS_UNKNOWN:
    {
        ret = "BLE_AUTH_STATUS_UNKNOWN";
        break;
    }
    case BLE_AUTH_SUCCESS:
    {
        ret = "BLE_AUTH_SUCCESS";
        break;
    }
    case BLE_AUTH_USER_ID_NOT_FOUND:
    {
        ret = "BLE_AUTH_USER_ID_NOT_FOUND";
        break;
    }
    case BLE_AUTH_USER_ID_NOT_MATCHED:
    {
        ret = "BLE_AUTH_USER_ID_NOT_MATCHED";
        break;
    }
    case BLE_AUTH_USER_ID_NVS_WRITE_FAILED:
    {
        ret = "BLE_AUTH_USER_ID_NVS_WRITE_FAILED";
        break;
    }
    default:
    {
        ret = "BLE_AUTH_STATUS_UNKNOWN";
        break;
    }
    }

    return ret;
}

// int ezlopi_ble_auth_
