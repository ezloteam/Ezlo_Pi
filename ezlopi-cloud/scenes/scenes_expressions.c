#include "ezlopi_cloud_constants.h"

#include "scenes_expressions.h"

void scenes_expressions_added(cJSON *cj_request, cJSON *cj_response)
{
    // https://api.ezlo.com/hub/broadcasts/index.html#hubexpressionadded
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
        }
    }
}