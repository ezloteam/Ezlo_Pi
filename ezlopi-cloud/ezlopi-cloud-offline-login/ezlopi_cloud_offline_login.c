#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "ezlopi_core_offline_login.h"

#include "ezlopi_core_devices_list.h"
#include "ezlopi_cloud_constants.h"

static void ezlopi_cloud_prepare_error(cJSON *cj_response, int error_code, const char *error_message, const char *error_data)
{
    cJSON *cj_error = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_error_str);
    if (cj_error)
    {
        cJSON_AddNumberToObject(__FUNCTION__, cj_error, ezlopi_code_str, error_code);
        cJSON_AddStringToObject(__FUNCTION__, cj_error, ezlopi_message_str, error_message);
        cJSON_AddStringToObject(__FUNCTION__, cj_error, ezlopi_data_str, error_data);
    }
}

void EZPI_CLOUD_offline_login(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            ezlopi_error_t error = EZPI_core_offline_login_perform(cj_params);
            if (EZPI_ERR_WRONG_PARAM == error)
            {
                ezlopi_cloud_prepare_error(cj_response, -32602, "Wrong params", "rpc.params.invalid");
            }
            else if (EZPI_FAILED == error)
            {
                ezlopi_cloud_prepare_error(cj_response, -32500, "Already logged in", "user.login.alreadylogged");
            }
            else if (EZPI_ERR_INVALID_CREDENTIALS == error)
            {
                ezlopi_cloud_prepare_error(cj_response, -32500, "Bad password", "user.login.badpassword");
            }
        }
        else
        {
            ezlopi_cloud_prepare_error(cj_response, -32600, "Bad request", "rpc.params.notfound");
        }
    }
}
