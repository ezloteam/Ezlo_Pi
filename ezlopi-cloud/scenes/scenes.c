#include <string.h>
#include <stdint.h>

#include "scenes.h"
#include "trace.h"
#include "cJSON.h"
#include "ezlopi_nvs.h"
#include "ezlopi_scenes_v2.h"
#include "ezlopi_devices.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_scenes_operators.h"
#include "ezlopi_meshbot_service.h"

void scenes_list(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON *cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_scene_array = cJSON_AddArrayToObject(cj_result, "scenes");
        ezlopi_scenes_get_list_v2(cj_scene_array);
    }
}

void scenes_create(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));

    cJSON *cj_params = cJSON_GetObjectItem(cj_request, ezlopi_params_str);
    if (cj_params)
    {
        uint32_t new_scene_id = ezlopi_store_new_scene_v2(cj_params);
        if (new_scene_id)
        {
            ezlopi_scenes_new_scene_populate(cj_params, new_scene_id);
        }
    }
}

void scenes_get(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));

    cJSON *cj_params = cJSON_GetObjectItem(cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON *cj_ids = cJSON_GetObjectItem(cj_params, "_id");
        if (cj_ids && cj_ids->valuestring)
        {
            char *scene_str = ezlopi_nvs_read_str(cj_ids->valuestring);

            if (scene_str)
            {
                cJSON *cj_scene = cJSON_Parse(scene_str);
                if (cj_scene)
                {
                    if (!cJSON_AddItemToObject(cj_response, "result", cj_scene))
                    {
                        cJSON_Delete(cj_scene);
                    }
                }

                free(scene_str);
            }
        }
    }
}

void scenes_edit(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));

    cJSON *cj_params = cJSON_GetObjectItem(cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON *cj_id = cJSON_GetObjectItem(cj_params, "_id");
        if (cj_id && cj_id->valuestring)
        {
            uint32_t u_id = strtoul(cj_id->valuestring, NULL, 16);
            cJSON *cj_eo = cJSON_GetObjectItem(cj_params, "eo");
            if (cj_eo)
            {
                // ezlopi_scenes_update_by_id(u_id, cj_eo);
            }
        }
    }
}

// void scenes_edit(cJSON *cj_request, cJSON *cj_response)
// {
//     cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
//     cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
//
//     cJSON *cj_params = cJSON_GetObjectItem(cj_request, ezlopi_params_str);
//     if (cj_params)
//     {
//         cJSON *cj_id = cJSON_GetObjectItem(cj_params, "_id");
//         if (cj_id && cj_id->valuestring)
//         {
//             uint32_t u_id = strtoul(cj_id->valuestring, NULL, 16);
//             cJSON *cj_eo = cJSON_GetObjectItem(cj_params, "eo");
//             if (cj_eo)
//             {
//                 ezlopi_scenes_update_by_id(u_id, cj_eo);
//             }
//         }
//     }
// }

void scenes_delete(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON_AddObjectToObject(cj_response, ezlopi_result_str);

    cJSON *cj_params = cJSON_GetObjectItem(cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON *cj_id = cJSON_GetObjectItem(cj_params, "_id");
        if (cj_id && cj_id->valuestring)
        {
            uint32_t u_id = strtoul(cj_id->valuestring, NULL, 16);
            ezlopi_nvs_delete_stored_script(u_id);
            ezlopi_scenes_depopulate_by_id_v2(u_id);
            ezlopi_scenes_remove_id_from_list_v2(u_id);
        }
    }
}

void scenes_status_get(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON_AddObjectToObject(cj_response, ezlopi_result_str);

    cJSON *cj_params = cJSON_GetObjectItem(cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON *cj_scene_id = cJSON_GetObjectItem(cj_params, "sceneId");
        if (cj_scene_id && cj_scene_id->valuestring)
        {
            uint32_t u_id = strtoul(cj_scene_id->valuestring, NULL, 16);
            l_scenes_list_v2_t *scene_node = ezlopi_scenes_get_by_id_v2(u_id);
            if (scene_node)
            {
                // scene_node->status;
            }
        }
    }
}

void scenes_run(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON *cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);

    cJSON *cj_params = cJSON_GetObjectItem(cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON *cj_scene_id = cJSON_GetObjectItem(cj_params, "sceneId");
        if (cj_scene_id && cj_scene_id->valuestring)
        {
            uint32_t u32_scene_id = strtoul(cj_scene_id->valuestring, NULL, 16);
            ezlopi_scenes_service_run_by_id(u32_scene_id);
        }
    }
}

void scenes_blocks_list(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi__id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));

    cJSON *cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_paramas = cJSON_GetObjectItem(cj_request, ezlopi_params_str);
        if (cj_paramas)
        {
            cJSON *cj_block_type = cJSON_GetObjectItem(cj_paramas, "blockType");
            if (cj_block_type && cj_block_type->valuestring)
            {
                cJSON *cj_block_array = NULL;
                e_scenes_block_type_v2_t block_type = SCENE_BLOCK_TYPE_NONE;
                if (0 == strncmp("when", cj_block_type->valuestring, 4))
                {
                    cj_block_array = cJSON_AddArrayToObject(cj_result, "when");
                    block_type = SCENE_BLOCK_TYPE_WHEN;
                }
                else if (0 == strncmp("then", cj_block_type->valuestring, 4))
                {
                    cj_block_array = cJSON_AddArrayToObject(cj_result, "then");
                    block_type = SCENE_BLOCK_TYPE_THEN;
                } else if (0 == strncmp(""))

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
                            l_scenes_list_v2_t *req_scene = ezlopi_scenes_get_by_id_v2(device_id);

                            if (req_scene)
                            {
                                switch (block_type)
                                {
                                case SCENE_BLOCK_TYPE_WHEN:
                                {
                                    l_when_block_v2_t *curr_when_block = req_scene->when_block;
                                    while (curr_when_block)
                                    {
                                        cJSON *cj_when_block = NULL;
                                        // ezlopi_scenes_cjson_create_when_block(req_scene->when_block);
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
#warning "Check if else block is required or not"
                                case SCENE_BLOCK_TYPE_THEN:
                                {
                                    l_action_block_v2_t *curr_then_block = req_scene->then_block;
                                    while (curr_then_block)
                                    {
                                        cJSON *cj_then_block = NULL;
                                        // ezlopi_scenes_cjson_create_then_block(req_scene->then_block);
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
                                case SCENE_BLOCK_TYPE_ELSE:
                                {
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
