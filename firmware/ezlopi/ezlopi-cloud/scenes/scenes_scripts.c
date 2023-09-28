#include "cJSON.h"

#include "ezlopi_cloud_constants.h"
#include "scenes_scripts.h"

void scenes_scripts_add(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));

    cJSON *cj_params = cJSON_GetObjectItem(cj_request, ezlopi_params);
    if (cj_params)
    {
        cJSON *cj_name = cJSON_GetObjectItem(cj_params, "name");
        cJSON *cj_code = cJSON_GetObjectItem(cj_params, "code");

        if (cj_name && cj_code)
        {
            
        }

        cJSON *cjson_result = cJSON_AddObjectToObject(cj_response, ezlopi_result);
        if (cjson_result)
        {

        }
    }
}
