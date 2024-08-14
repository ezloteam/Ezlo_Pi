

#include "ezlopi_util_trace.h"

#include "ezlopi_core_setting_commands.h"

#include "ezlopi_cloud_constants.h"
#include "ezlopi_cloud_setting_commands.h"


void setting_value_set(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON* cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            ezlopi_core_setting_commands_process(cj_params);
        }
    }
}

void settings_list(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if(cj_result)
    {
        ezlopi_core_setting_commands_populate_settings(cj_result);
    }
}


