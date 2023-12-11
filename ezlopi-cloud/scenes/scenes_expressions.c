#include "trace.h"

#include "ezlopi_cloud_constants.h"
#include "ezlopi_scenes_expressions.h"

#include "scenes_expressions.h"

void scenes_expressions_added(cJSON *cj_request, cJSON *cj_response)
{
    // https://api.ezlo.com/hub/broadcasts/index.html#hubexpressionadded
}

void scenes_expressions_list(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
        cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));

        cJSON *cj_params = cJSON_GetObjectItem(cj_request, ezlopi_params_str);
        if (cj_params)
        {
            char *params_str = cJSON_Print(cj_params);
            if (params_str)
            {
                TRACE_D("expressions params: %s", params_str);
            }

            cJSON *cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
            if (cj_result)
            {
                cJSON *cj_expressions_array = cJSON_AddArrayToObject(cj_result, "expressions");
                if (cj_expressions_array)
                {
                }
            }
        }
    }
}

void scenes_expressions_set(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
        cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));

        cJSON *cj_params = cJSON_GetObjectItem(cj_request, ezlopi_params_str);
        if (cj_params)
        {
            char *params_str = cJSON_Print(cj_params);
            if (params_str)
            {
                TRACE_D("expressions params: %s", params_str);
            }

            ezlopi_scenes_expressions_add_to_head(0, cj_params);
        }
    }
}