#include "cJSON.h"

#include "ezlopi_cloud_constants.h"
#include "scenes_scripts.h"
#include "ezlopi_scenes_scripts.h"

void scenes_scripts_add(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));

    cJSON *cj_params = cJSON_GetObjectItem(cj_request, ezlopi_params);
    if (cj_params)
    {
        uint32_t script_id = ezlopi_scenes_scripts_add_to_head(0, cj_params);

        cJSON *cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result);
        if (cj_result)
        {
            if (script_id)
            {
                char tmp_id_str[32];
                snprintf(tmp_id_str, sizeof(tmp_id_str), "%08x", script_id);
                cJSON_AddStringToObject(cj_result, ezlopi__id_str, tmp_id_str);
            }
        }
    }
}
