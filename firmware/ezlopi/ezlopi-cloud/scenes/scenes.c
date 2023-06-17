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
        cJSON *cjson_scenes_array = ezlopi_scenes_create_cjson_scene_list(ezlopi_scene_get_scenes_list());
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

    uint32_t tmp_added_result = 0;

    cJSON *cj_params = cJSON_GetObjectItem(cj_request, "params");
    if (cj_params)
    {
        cJSON *cj_ids = cJSON_GetObjectItem(cj_params, "_id");
        if (cj_ids && cj_ids->valuestring)
        {
            uint32_t incoming_scene_id = strtoul(cj_ids->valuestring, NULL, 16);
            l_scenes_list_t *scenes_list = ezlopi_scene_get_scenes_list();

            while (scenes_list)
            {
                if (incoming_scene_id == scenes_list->_id)
                {
                    cJSON *cj_scene = ezlopi_ezlopi_scenes_create_cjson_scene(scenes_list);
                    if (cj_scene)
                    {
                        if (cJSON_AddItemToObject(cj_response, ezlopi_result, cj_scene))
                        {
                            tmp_added_result = 1;
                        }
                        else
                        {
                            cJSON_Delete(cj_scene);
                        }
                    }
                }
                scenes_list = scenes_list->next;
            }
        }
    }

    if (0 == tmp_added_result)
    {
        cJSON_AddObjectToObject(cj_response, ezlopi_result);
    }
}

void scenes_edit(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));

    cJSON *cj_params = cJSON_GetObjectItem(cj_request, "params");
    if (cj_params)
    {
        cJSON *cj_id = cJSON_GetObjectItem(cj_params, "_id");
        if (cj_id && cj_id->valuestring)
        {
            uint32_t u_id = strtoul(cj_id->valuestring, NULL, 16);
            cJSON *cj_eo = cJSON_GetObjectItem(cj_params, "eo");
            if (cj_eo)
            {
                ezlopi_scenes_update_by_id(u_id, cj_eo);
            }
        }
    }
}

void scenes_delete(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON_AddObjectToObject(cj_response, "result");

    cJSON *cj_params = cJSON_GetObjectItem(cj_request, "params");
    if (cj_params)
    {
        cJSON *cj_id = cJSON_GetObjectItem(cj_params, "_id");
        if (cj_id && cj_id->valuestring)
        {
            uint32_t u_id = strtoul(cj_id->valuestring, NULL, 16);
            ezlopi_scenes_delete_by_id(u_id);
        }
    }
}

void scenes_status_get(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON_AddObjectToObject(cj_response, "result");

    TRACE_E("Not implemented!");

    cJSON *cj_params = cJSON_GetObjectItem(cj_request, "params");
    if (cj_params)
    {
        cJSON *cj_scene_id = cJSON_GetObjectItem(cj_params, "sceneId");
        if (cj_scene_id && cj_scene_id->valuestring)
        {
            uint32_t u_id = strtoul(cj_scene_id->valuestring, NULL, 16);
        }
    }
}

void scenes_blocks_list(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi__id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));

    cJSON *cj_result = cJSON_AddObjectToObject(cj_response, "result");
    if (cj_result)
    {
        cJSON *cj_paramas = cJSON_GetObjectItem(cj_request, "params");
        if (cj_paramas && cj_paramas->valuestring)
        {
            cJSON *cj_block_type = cJSON_GetObjectItem(cj_paramas, "bockType");
            if (cj_block_type && cj_block_type->valuestring)
            {
                cJSON *cj_block_array = NULL;
                e_scenes_block_type_t block_type = SCENE_BLOCK_TYPE_NONE;
                if (strncmp("when", cj_block_type->valuestring, 4))
                {
                    cj_block_array = cJSON_AddArrayToObject(cj_result, "when");
                    block_type = SCENE_BLOCK_TYPE_WHEN;
                }
                else if (strncmp("then", cj_block_type->valuestring, 4))
                {
                    cj_block_array = cJSON_AddArrayToObject(cj_result, "then");
                    block_type = SCENE_BLOCK_TYPE_THEN;
                }

                if (cj_block_array && block_type)
                {
                    cJSON *cj_devices_array = cJSON_GetObjectItem(cj_paramas, "devices");
                    if (cj_devices_array && (cJSON_Array == cj_devices_array->type))
                    {
                        int device_id_idx = 0;
                        cJSON *cj_device_id = NULL;
                        while (NULL != (cj_device_id = cJSON_GetArrayItem(cj_devices_array, device_id_idx++)))
                        {
                            uint32_t device_id = strtoul(cj_device_id->valuestring, NULL, 16);
                            l_scenes_list_t *req_scene = ezlopi_scenes_get_by_id(device_id);

                            if (req_scene)
                            {
                                switch (block_type)
                                {
                                case SCENE_BLOCK_TYPE_WHEN:
                                {
                                    l_when_block_t *curr_when_block = req_scene->when;
                                    while (curr_when_block)
                                    {
                                        cJSON *cj_when_block = ezlopi_scenes_cjson_create_when_block(req_scene->when);
                                        if (cj_when_block)
                                        {
                                            if (!cJSON_AddItemToArray(cj_block_array, cj_when_block))
                                            {
                                                cJSON_Delete(cj_when_block);
                                            }
                                        }
                                        curr_when_block = curr_when_block->next;
                                    }

                                    break;
                                }
                                case SCENE_BLOCK_TYPE_THEN:
                                {
                                    l_then_block_t *curr_then_block = req_scene->then;
                                    while (curr_then_block)
                                    {
                                        cJSON *cj_then_block = ezlopi_scenes_cjson_create_then_block(req_scene->then);
                                        if (cj_then_block)
                                        {
                                            if (!cJSON_AddItemToArray(cj_block_array, cj_then_block))
                                            {
                                                cJSON_Delete(cj_then_block);
                                            }
                                        }
                                        curr_then_block = curr_then_block->next;
                                    }
                                    break;
                                }
                                default:
                                {
                                    break;
                                }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
