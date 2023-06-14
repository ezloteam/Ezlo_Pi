#include <string.h>
#include <stdint.h>

#include "scenes.h"
#include "trace.h"
#include "frozen.h"
#include "cJSON.h"
#include "ezlopi_nvs.h"
#include "ezlopi_scenes.h"
#include "ezlopi_cloud_constants.h"

void scenes_list(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON *cjson_result = cJSON_AddObjectToObject(cj_response, ezlopi_result);
    if (cjson_result)
    {
        cJSON *cjson_scenes_array = ezlopi_scenes_create_cjson(ezlopi_scene_get_scenes_list());
        if (cjson_scenes_array)
        {
            if (!cJSON_AddItemToObject(cjson_result, "scenes", cjson_scenes_array))
            {
                cJSON_Delete(cjson_scenes_array);
            }
        }
    }
}

void scenes_create(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON *cjson_result = cJSON_AddObjectToObject(cj_response, ezlopi_result);

    cJSON *cj_params = cJSON_GetObjectItem(cj_request, "params");
    if (cj_params)
    {
        ezlopi_scene_add(cj_params);
        ezlopi_scene_update_nvs();
    }
}

void scenes_get(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON *cjson_result = cJSON_AddObjectToObject(cj_response, ezlopi_result);

    cJSON *cj_params = cJSON_GetObjectItem(cj_request, "params");
    if (cj_params)
    {
        cJSON *cj_ids = cJSON_GetObjectItem(cj_params, "_id");
        if (cj_ids)
        {
            uint32_t incoming_scene_id = (uint32_t)cj_ids->valuedouble;
            l_scenes_list_t *scenes_list = ezlopi_scene_get_scenes_list();

            while (scenes_list)
            {
                if (incoming_scene_id == scenes_list->_id)
                {
#warning "Fetch individual scene form scene list"
                }
                scenes_list = scenes_list->next;
            }
        }
    }
}
