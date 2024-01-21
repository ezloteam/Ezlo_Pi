#include "trace.h"

#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_scenes_expressions.h"

#include "scenes_expressions.h"

void scenes_expressions_added(cJSON *cj_request, cJSON *cj_response)
{
    // https://api.ezlo.com/hub/broadcasts/index.html#hubexpressionadded
}

void scenes_expressions_delete(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
        cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
        cJSON_AddObjectToObject(cj_response, ezlopi_result_str);

        cJSON *cj_params = cJSON_GetObjectItem(cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON *cj_expression_name = cJSON_GetObjectItem(cj_params, ezlopi_name_str);
            if (cj_expression_name && cj_expression_name->valuestring)
            {
                ezlopi_scenes_expressions_delete_by_name(cj_expression_name->valuestring);
            }
        }
    }
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
            CJSON_TRACE("exp-params", cj_params);

            cJSON *cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
            if (cj_result)
            {
                cJSON *cj_expressions_array = cJSON_AddArrayToObject(cj_result, ezlopi_expressions_str);
                if (cj_expressions_array)
                {
                    ezlopi_scenes_expressions_list_cjson(cj_expressions_array, cj_params);
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
        cJSON_AddObjectToObject(cj_response, ezlopi_result_str);

        cJSON *cj_params = cJSON_GetObjectItem(cj_request, ezlopi_params_str);
        if (cj_params)
        {
            CJSON_TRACE("expressions params", cj_params);
            ezlopi_scenes_expressions_add_to_head(0, cj_params);
        }
    }
}