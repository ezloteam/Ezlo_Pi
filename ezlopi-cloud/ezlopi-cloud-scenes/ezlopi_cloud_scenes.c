#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include <string.h>
#include <stdint.h>

#include "ezlopi_util_trace.h"
#include "cjext.h"
#include "ezlopi_cloud_scenes.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_scenes_v2.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_service_meshbot.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_core_scenes_operators.h"
#include "ezlopi_core_scenes_notifications.h"
#include "ezlopi_core_scenes_when_methods_helper_functions.h"

void scenes_list(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        ezlopi_scenes_get_list_v2(cJSON_AddArrayToObject(__FUNCTION__, cj_result, ezlopi_scenes_str));
    }
}

void scenes_create(cJSON* cj_request, cJSON* cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);

    cJSON* cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        uint32_t new_scene_id = ezlopi_store_new_scene_v2(cj_params);
        TRACE_D("new-scene-id: %08x", new_scene_id);

        if (new_scene_id)
        {
            char tmp_buff[32];
            snprintf(tmp_buff, sizeof(tmp_buff), "%08x", new_scene_id);
            cJSON_AddStringToObject(__FUNCTION__, cj_request, ezlopi__id_str, tmp_buff);
            ezlopi_scenes_new_scene_populate(cj_params, new_scene_id);
        }
    }
}

void scenes_get(cJSON* cj_request, cJSON* cj_response)
{

    cJSON* cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON* cj_ids = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi__id_str);
        if (cj_ids && cj_ids->valuestring)
        {
            char* scene_str = ezlopi_nvs_read_str(cj_ids->valuestring);
            if (scene_str)
            {
                cJSON_AddRawToObject(__FUNCTION__, cj_response, ezlopi_result_str, scene_str);
                ezlopi_free(__FUNCTION__, scene_str);
            }
        }
    }
}

void scenes_edit(cJSON* cj_request, cJSON* cj_response)
{

    cJSON* cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON* cj_eo = cJSON_GetObjectItem(__FUNCTION__, cj_params, "eo");
        if (cj_eo)
        {
            CJSON_TRACE("scene-edit eo", cj_eo);

            cJSON* cj_id = cJSON_GetObjectItem(__FUNCTION__, cj_eo, ezlopi__id_str);
            if (cj_id && cj_id->valuestring)
            {
                if (cj_id && cj_id->valuestring)
                {
                    uint32_t u_id = strtoul(cj_id->valuestring, NULL, 16);
                    ezlopi_scene_edit_by_id(u_id, cj_eo);
                }
            }
        }
    }
}

void scenes_delete(cJSON* cj_request, cJSON* cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);

    cJSON* cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON* cj_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi__id_str);
        if (cj_id && cj_id->valuestring)
        {
            uint32_t u_id = strtoul(cj_id->valuestring, NULL, 16);
            ezlopi_nvs_delete_stored_data_by_id(u_id);
            ezlopi_scenes_depopulate_by_id_v2(u_id);
            ezlopi_scenes_remove_id_from_list_v2(u_id);
        }
    }
}

void scenes_status_get(cJSON* cj_request, cJSON* cj_response)
{

    cJSON* cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON* cj_scene_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_sceneId_str);
        if (cj_scene_id && cj_scene_id->valuestring)
        {
            char* scene_str = ezlopi_nvs_read_str(cj_scene_id->valuestring);
            if (scene_str)
            {
                cJSON_AddRawToObject(__FUNCTION__, cj_response, ezlopi_result_str, scene_str);
                ezlopi_free(__FUNCTION__, scene_str);
            }
        }
    }
}

void scenes_run(cJSON* cj_request, cJSON* cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);

    cJSON* cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON* cj_scene_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_sceneId_str);
        if (cj_scene_id && cj_scene_id->valuestring)
        {
            uint32_t u32_scene_id = strtoul(cj_scene_id->valuestring, NULL, 16);
            ezlopi_scenes_service_run_by_id(u32_scene_id);
        }
    }
}

void scenes_enable_set(cJSON* cj_request, cJSON* cj_response)
{

    cJSON* cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON* cj_scene_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi__id_str);
        if (cj_scene_id && cj_scene_id->valuestring)
        {
            uint32_t scene_id = strtoul(cj_scene_id->valuestring, NULL, 16);
            if (scene_id)
            {
                bool enabled_flag = false;
                CJSON_GET_VALUE_BOOL(cj_params, ezlopi_enabled_str, enabled_flag);
                char* scene_str = ezlopi_nvs_read_str(cj_scene_id->valuestring);

                if (scene_str)
                {
                    cJSON* cj_scene = cJSON_Parse(__FUNCTION__, scene_str);
                    ezlopi_free(__FUNCTION__, scene_str);

                    if (cj_scene)
                    {
                        cJSON_DeleteItemFromObject(__FUNCTION__, cj_scene, ezlopi_enabled_str);
                        cJSON_AddBoolToObject(__FUNCTION__, cj_scene, ezlopi_enabled_str, enabled_flag);

                        char* updated_scene_str = cJSON_PrintBuffered(__FUNCTION__, cj_scene, 2048, false);
                        cJSON_Delete(__FUNCTION__, cj_scene);

                        if (updated_scene_str)
                        {
                            TRACE_D("updated-scene: %s", updated_scene_str);
                            ezlopi_nvs_write_str(updated_scene_str, strlen(updated_scene_str), cj_scene_id->valuestring);

                            ezlopi_free(__FUNCTION__, updated_scene_str);
                        }
                    }
                }

                l_scenes_list_v2_t* scene_node = ezlopi_scenes_get_by_id_v2(scene_id);
                if (scene_node)
                {
                    scene_node->enabled = enabled_flag;
                    if (false == scene_node->enabled)
                    {
                        ezlopi_meshobot_service_stop_scene(scene_node);
                    }
                    else if (true == scene_node->enabled)
                    {
                        ezlopi_meshbot_service_start_scene(scene_node);
                    }
                }
            }
        }
    }

    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void scenes_notification_add(cJSON* cj_request, cJSON* cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);

    cJSON* cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON* cj_user_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_userId_str);
        cJSON* cj_scene_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_notifications_str);

        if (cj_scene_id && cj_scene_id->valuestring && cj_user_id && cj_user_id->valuestring)
        {
            char* scene_str = ezlopi_nvs_read_str(cj_scene_id->valuestring);
            if (scene_str)
            {
                cJSON* cj_scene = cJSON_Parse(__FUNCTION__, scene_str);
                ezlopi_free(__FUNCTION__, scene_str);

                if (cj_scene)
                {
                    cJSON* cj_user_notifications = cJSON_GetObjectItem(__FUNCTION__, cj_scene, ezlopi_user_notifications_str);
                    if (cj_user_notifications)
                    {
                        cJSON_AddItemReferenceToArray(__FUNCTION__, cj_user_notifications, cj_user_id);
                    }

                    CJSON_TRACE("updated-scene", cj_scene);
                    char* updated_scene_str = cJSON_PrintBuffered(__FUNCTION__, cj_scene, 2048, false);
                    cJSON_Delete(__FUNCTION__, cj_scene);

                    if (updated_scene_str)
                    {
                        ezlopi_nvs_write_str(updated_scene_str, strlen(updated_scene_str), cj_scene_id->valuestring);
                        ezlopi_free(__FUNCTION__, updated_scene_str);
                    }
                }
            }

            uint32_t scene_id = strtoul(cj_scene_id->valuestring, NULL, 16);

            if (scene_id)
            {
                l_scenes_list_v2_t* scene_node = ezlopi_scenes_get_scenes_head_v2();
                while (scene_node)
                {
                    if (scene_id == scene_node->_id)
                    {
                        ezlopi_scene_add_users_in_notifications(scene_node, cj_user_id);
                        break;
                    }
                    scene_node = scene_node->next;
                }
            }
        }
    }
}

void scenes_notification_remove(cJSON* cj_request, cJSON* cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);

    cJSON* cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON* cj_user_id_del = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_userId_str);
        cJSON* cj_scene_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_notifications_str);

        if (cj_scene_id && cj_scene_id->valuestring && cj_user_id_del && cj_user_id_del->valuestring)
        {
            char* scene_str = ezlopi_nvs_read_str(cj_scene_id->valuestring);
            if (scene_str)
            {
                cJSON* cj_scene = cJSON_Parse(__FUNCTION__, scene_str);
                ezlopi_free(__FUNCTION__, scene_str);

                if (cj_scene)
                {
                    cJSON* cj_user_notifications = cJSON_GetObjectItem(__FUNCTION__, cj_scene, ezlopi_user_notifications_str);
                    if (cj_user_notifications && cj_user_id_del)
                    {
                        uint32_t idx = 0;
                        cJSON* cj_user_id = NULL;
                        while (NULL != (cj_user_id = cJSON_GetArrayItem(cj_user_notifications, idx)))
                        {
                            if (0 == strcmp(cj_user_id->valuestring, cj_user_id_del->valuestring))
                            {
                                cJSON_DeleteItemFromArray(__FUNCTION__, cj_user_notifications, idx);
                            }
                            idx++;
                        }
                    }

                    CJSON_TRACE("updated-scene", cj_scene);
                    char* updated_scene_str = cJSON_PrintBuffered(__FUNCTION__, cj_scene, 2048, false);
                    cJSON_Delete(__FUNCTION__, cj_scene);

                    if (updated_scene_str)
                    {
                        ezlopi_nvs_write_str(updated_scene_str, strlen(updated_scene_str), cj_scene_id->valuestring);
                        ezlopi_free(__FUNCTION__, updated_scene_str);
                    }
                }
            }

            uint32_t scene_id = strtoul(cj_scene_id->valuestring, NULL, 16);
            if (scene_id)
            {
                l_scenes_list_v2_t* scene_node = ezlopi_scenes_get_scenes_head_v2();
                while (scene_node)
                {
                    if (scene_node->_id == scene_id)
                    {
                        if (0 == strcmp(scene_node->user_notifications->user_id, cj_user_id_del->valuestring))
                        {
                            l_user_notification_v2_t* user_id_del = scene_node->user_notifications;
                            scene_node->user_notifications = scene_node->user_notifications->next;
                            user_id_del->next = NULL;
                            ezlopi_scenes_delete_user_notifications(user_id_del);
                        }
                        else
                        {
                            l_user_notification_v2_t* user_node = scene_node->user_notifications;
                            while (user_node->next)
                            {
                                if (0 == strcmp(user_node->user_id, cj_user_id_del->valuestring))
                                {
                                    l_user_notification_v2_t* user_id_del = user_node;
                                    user_node = user_node->next;
                                    user_id_del->next = NULL;
                                    ezlopi_scenes_delete_user_notifications(user_id_del);
                                    break;
                                }

                                user_node = user_node->next;
                            }
                        }

                        break;
                    }
                    scene_node = scene_node->next;
                }
            }
        }
    }
}

void scenes_block_status_reset(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    cJSON* cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_result && cj_params)
    {
        cJSON *cj_scene_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_sceneId_str);
        cJSON *cj_block_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_blockId_str);
        if (cj_scene_id && (NULL != cj_scene_id->valuestring) && cj_block_id && (NULL != cj_block_id->valuestring))
        {
            ezlopi_core_scene_set_reset_latch(cj_scene_id->valuestring, cj_block_id->valuestring, false);
        }
    }
}

////// updater for scene
////// useful for 'hub.scenes.enabled.set'
void scene_changed(cJSON* cj_request, cJSON* cj_response)
{
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_sender_str);
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_error_str);

    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_scene_changed_str);

    cJSON* cj_method = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_method_str);
    cJSON_AddItemToObject(__FUNCTION__, cj_response, ezlopi_changed_by_str, cJSON_Duplicate(__FUNCTION__, cj_method, cJSON_True));

    cJSON* cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON* cj_scene_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi__id_str);
        if (cj_scene_id && cj_scene_id->valuestring)
        {
            char* scene_str = ezlopi_nvs_read_str(cj_scene_id->valuestring);
            if (scene_str)
            {
                cJSON_AddRawToObject(__FUNCTION__, cj_response, ezlopi_result_str, scene_str);
            }
        }
    }
}

void scene_added(cJSON* cj_request, cJSON* cj_response)
{
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_sender_str);
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_error_str);

    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_scene_added_str);

    cJSON* new_scene_id = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi__id_str);
    if (new_scene_id && new_scene_id->valuestring)
    {
        char* new_scene = ezlopi_nvs_read_str(new_scene_id->valuestring);
        if (new_scene)
        {
            cJSON_AddRawToObject(__FUNCTION__, cj_response, ezlopi_result_str, new_scene);
            ezlopi_free(__FUNCTION__, new_scene);
        }
    }
}

void scene_deleted(cJSON* cj_request, cJSON* cj_response)
{
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_sender_str);
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_error_str);

    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_scene_deleted_str);

    cJSON* cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON* tmp__id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi__id_str);
        if (tmp__id)
        {
            cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
            if (cj_result)
            {
                cJSON_AddItemToObject(__FUNCTION__, cj_result, ezlopi__id_str, cJSON_Duplicate(__FUNCTION__, tmp__id, pdTRUE));
                cJSON_AddBoolToObject(__FUNCTION__, cj_result, ezlopi_syncNotification_str, true);
            }
        }
    }
}
#endif  // CONFIG_EZPI_SERV_ENABLE_MESHBOTS