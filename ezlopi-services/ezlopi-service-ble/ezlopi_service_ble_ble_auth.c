
#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_BLE_ENABLE
// #TODO ADD guard for key enable
#include <string.h>

#include "ezlopi_util_trace.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_errors.h"

#include "ezlopi_service_ble_ble_auth.h"

static e_auth_status_t sg_last_auth_status = BLE_AUTH_STATUS_UNKNOWN;

e_auth_status_t ezlopi_ble_auth_last_status(void)
{
    return sg_last_auth_status;
}

e_auth_status_t ezlopi_ble_auth_store_user_id(char* user_id)
{
    if (user_id)
    {
        TRACE_D("here");
        if (EZPI_SUCCESS == ezlopi_nvs_write_user_id_str(user_id))
        {
            sg_last_auth_status = BLE_AUTH_SUCCESS;
        }
        else
        {
            sg_last_auth_status = BLE_AUTH_USER_ID_NVS_WRITE_FAILED;
        }
    }

    return sg_last_auth_status;
}

e_auth_status_t ezlopi_ble_auth_check_user_id(char* user_id)
{
    char* stored_user_id = ezlopi_nvs_read_user_id_str();

    if (NULL != stored_user_id)
    {
        if (user_id)
        {
            TRACE_D("stored_user_id: %s, user_id: %s", stored_user_id, user_id);
            if (0 == strncmp(stored_user_id, user_id, strlen(stored_user_id)))
            {
                TRACE_S("Authenticated.");
                sg_last_auth_status = BLE_AUTH_SUCCESS;
            }
            else
            {
                sg_last_auth_status = BLE_AUTH_USER_ID_NOT_MATCHED;
            }
        }

        ezlopi_free(__FUNCTION__, stored_user_id);
    }
    else
    {
        ezlopi_nvs_write_user_id_str(user_id);
        // sg_last_auth_status = BLE_AUTH_USER_ID_NOT_FOUND;
        sg_last_auth_status = BLE_AUTH_SUCCESS;
    }

    return sg_last_auth_status;
}

char* ezlopi_ble_auth_status_to_string(e_auth_status_t status)
{
    char* ret = "BLE_AUTH_STATUS_UNKNOWN";

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

#endif // CONFIG_EZPI_BLE_ENABLE
