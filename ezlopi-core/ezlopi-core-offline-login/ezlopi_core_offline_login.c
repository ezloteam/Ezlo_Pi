
#include "cjext.h"
#include "ezlopi_core_nvs.h"
#include "EZLOPI_USER_CONFIG.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_offline_login.h"

static bool logged_in = false;

ezlopi_error_t ezlopi_core_offline_login_perform(cJSON *cj_params)
{
    ezlopi_error_t error = EZPI_SUCCESS;
    if (logged_in)
    {
        error = EZPI_FAILED;
    }
    else
    {
        cJSON *cj_user = cJSON_GetObjectItem(__FUNCTION__, cj_params, "user");
        cJSON *cj_token = cJSON_GetObjectItem(__FUNCTION__, cj_params, "token");
        if (cj_user && cj_token && (cJSON_IsString(cj_user)) && (cJSON_IsString(cj_token)))
        {
            const char *stored_uesr_id = ezlopi_nvs_read_user_id_str();
            if (NULL != stored_uesr_id)
            {
                printf("\n\n UserID: %s\nToken: %s \nStored id: %s\n\n", cj_user->valuestring, cj_token->valuestring, stored_uesr_id);
                if(0 == strncmp(stored_uesr_id, cj_user->valuestring, strlen(stored_uesr_id)))
                {
                    // const char *password_saved = "Hs87Kns41106743NMS2";
                    const char *password_saved = ezlopi_factory_info_v3_get_local_key();
                    if(0 == strncmp(password_saved, cj_token->valuestring, strlen(password_saved)))
                    {
                        logged_in = true;
                    }
                    else 
                    {
                        error = EZPI_ERR_INVALID_CREDENTIALS;
                    }
                }
                ezlopi_free(__FUNCTION__, stored_uesr_id);
            }
        }
        else
        {
            error = EZPI_ERR_JOSN_PARAMS_NOT_FOUND;
        }
    }
    return error;
}
