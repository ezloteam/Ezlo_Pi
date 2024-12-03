
#include "cjext.h"
#include "ezlopi_core_nvs.h"
#include "EZLOPI_USER_CONFIG.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_offline_login.h"

#define BYPASS_LOGIN 0

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
#if (1 == BYPASS_LOGIN)
        logged_in = true;
#else  // BYPASS_LOGIN == 0
        cJSON *cj_user = cJSON_GetObjectItem(__FUNCTION__, cj_params, "user");
        cJSON *cj_token = cJSON_GetObjectItem(__FUNCTION__, cj_params, "token");
        if (cj_user && cj_token && (cJSON_IsString(cj_user)) && (cJSON_IsString(cj_token)))
        {

            // char *stored_uesr_id = ezlopi_nvs_read_user_id_str();
            // if (NULL != stored_uesr_id)
            {
                // if (0 == strncmp(stored_uesr_id, cj_user->valuestring, strlen(stored_uesr_id)))
                {
                    const char *password_saved = ezlopi_factory_info_v3_get_local_key();
                    if (NULL != password_saved)
                    {
                        TRACE_D("password: %s", password_saved);
                        if (0 == strncmp(password_saved, cj_token->valuestring, strlen(password_saved)))
                        {
                            logged_in = true;
                        }
                        else
                        {
                            error = EZPI_ERR_INVALID_CREDENTIALS;
                        }
                        ezlopi_factory_info_v3_free(password_saved);
                    }
                    else
                    {
                        error = EZPI_ERR_INVALID_CREDENTIALS;
                    }
                }
                // else
                // {
                //     error = EZPI_ERR_WRONG_PARAM;
                // }
                // ezlopi_free(__FUNCTION__, stored_uesr_id);
            }
        }
        else
        {
            error = EZPI_ERR_WRONG_PARAM;
        }
#endif // BYPASS_LOGIN == 0
    }

    return error;
}

ezlopi_error_t ezlopi_core_offline_logout_perform()
{
    ezlopi_error_t error = EZPI_FAILED;
    if (logged_in)
    {
        logged_in = false;
        error = EZPI_SUCCESS;
    }
    return error;
}

bool is_user_logged_in()
{
    return logged_in;
}
