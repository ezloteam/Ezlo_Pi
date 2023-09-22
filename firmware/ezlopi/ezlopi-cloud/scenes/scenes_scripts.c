#include "cJSON.h"

#include "ezlopi_cloud_constants.h"
#include "scenes_scripts.h"

void scenes_scripts_add(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON *cjson_result = cJSON_AddObjectToObject(cj_response, ezlopi_result);
    if (cjson_result)
    {
        cJSON *cjson_scenes_array = ezlopi_scenes_create_cjson_scene_list(ezlopi_scenes_get_scenes_list());
        if (cjson_scenes_array)
        {
            if (!cJSON_AddItemToObject(cjson_result, "scenes", cjson_scenes_array))
            {
                cJSON_Delete(cjson_scenes_array);
            }
        }
    }
}
