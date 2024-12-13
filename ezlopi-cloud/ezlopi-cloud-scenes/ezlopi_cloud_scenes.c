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
#include "ezlopi_core_scenes_populate.h"
#include "ezlopi_core_scenes_delete.h"
#include "ezlopi_core_scenes_operators.h"
#include "ezlopi_core_scenes_notifications.h"
#include "ezlopi_core_scenes_then_methods_helper_func.h"
#include "ezlopi_core_scenes_when_methods_helper_functions.h"

#include "ezlopi_cloud_constants.h"
void scenes_list(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str); // For NULL broadcast
    if (cj_result)
    {
        EZPI_core_scenes_get_list_v2(cJSON_AddArrayToObject(__FUNCTION__, cj_result, ezlopi_scenes_str));
    }
}

void scenes_create(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        uint32_t new_scene_id = EZPI_core_scenes_store_new_scene_v2(cj_params);
        TRACE_D("new-scene-id: %08x", new_scene_id);

        if (new_scene_id)
        {
            char tmp_buff[32];
            snprintf(tmp_buff, sizeof(tmp_buff), "%08x", new_scene_id);
            cJSON_AddStringToObject(__FUNCTION__, cj_request, ezlopi__id_str, tmp_buff); // this is for (reply_broadcast)
            EZPI_core_scenes_new_scene_populate(cj_params, new_scene_id);

            // Trigger new-scene to 'start'
            ezlopi_meshbot_service_start_scene(EZPI_core_scenes_get_by_id_v2(new_scene_id));
        }
    }
}

void scenes_get(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON *cj_ids = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi__id_str);
        if (cj_ids && cj_ids->valuestring)
        {
            char *scene_str = ezlopi_nvs_read_str(cj_ids->valuestring);
            if (scene_str)
            {
                cJSON_AddRawToObject(__FUNCTION__, cj_response, ezlopi_result_str, scene_str);
                ezlopi_free(__FUNCTION__, scene_str);
            }
        }
    }

    // Finally, Make sure 'result : {}' is present in broadcast
    if (NULL == cJSON_GetObjectItem(__FUNCTION__, cj_response, ezlopi_result_str))
    {
        cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    }
}

void scenes_edit(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON *cj_eo = cJSON_GetObjectItem(__FUNCTION__, cj_params, "eo");
        cJSON *cj_id = cJSON_GetObjectItem(__FUNCTION__, cj_eo, ezlopi__id_str);
        // CJSON_TRACE("scene-edit eo", cj_eo);
        if (cj_eo && (cj_id && cj_id->valuestring))
        {
            uint32_t u_id = strtoul(cj_id->valuestring, NULL, 16);
            EZPI_core_scenes_edit_by_id(u_id, cj_eo);
        }
    }
}

void scenes_delete(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON *cj_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi__id_str);
        if (cj_id && cj_id->valuestring)
        {
            uint32_t u_id = strtoul(cj_id->valuestring, NULL, 16);
            ezlopi_nvs_delete_stored_data_by_id(u_id);
            EZPI_core_scenes_remove_id_from_list_v2(u_id);
            EZPI_core_scenes_depopulate_by_id_v2(u_id);
        }
    }
}

void scenes_status_get(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON *cj_scene_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_sceneId_str);
        if (cj_scene_id && cj_scene_id->valuestring)
        {
            char *scene_str = ezlopi_nvs_read_str(cj_scene_id->valuestring);
            if (scene_str)
            {
                cJSON_AddRawToObject(__FUNCTION__, cj_response, ezlopi_result_str, scene_str);
                ezlopi_free(__FUNCTION__, scene_str);
            }
        }
    }

    // Finally, Make sure 'result : {}' is present in broadcast
    if (NULL == cJSON_GetObjectItem(__FUNCTION__, cj_response, ezlopi_result_str))
    {
        cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    }
}

void scenes_run(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str); // For NULL broadcast

    cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON *cj_scene_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_sceneId_str);
        if (cj_scene_id && cj_scene_id->valuestring)
        {
            uint32_t u32_scene_id = strtoul(cj_scene_id->valuestring, NULL, 16);
            ezlopi_scenes_service_run_by_id(u32_scene_id);
        }
    }
}

void scenes_enable_set(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON *cj_scene_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi__id_str);
        if (cj_scene_id && cj_scene_id->valuestring)
        {
            uint32_t scene_id = strtoul(cj_scene_id->valuestring, NULL, 16);
            if (scene_id)
            {
                bool enabled_flag = false;
                CJSON_GET_VALUE_BOOL(cj_params, ezlopi_enabled_str, enabled_flag);
                char *scene_str = ezlopi_nvs_read_str(cj_scene_id->valuestring);

                if (scene_str)
                {
                    cJSON *cj_scene = cJSON_Parse(__FUNCTION__, scene_str);
                    ezlopi_free(__FUNCTION__, scene_str);

                    if (cj_scene)
                    {
                        cJSON_DeleteItemFromObject(__FUNCTION__, cj_scene, ezlopi_enabled_str);
                        cJSON_AddBoolToObject(__FUNCTION__, cj_scene, ezlopi_enabled_str, enabled_flag);

                        char *updated_scene_str = cJSON_PrintBuffered(__FUNCTION__, cj_scene, 4096, false);
                        cJSON_Delete(__FUNCTION__, cj_scene);

                        if (updated_scene_str)
                        {
                            TRACE_D("updated-scene: %s", updated_scene_str);
                            ezlopi_nvs_write_str(updated_scene_str, strlen(updated_scene_str), cj_scene_id->valuestring);

                            ezlopi_free(__FUNCTION__, updated_scene_str);
                        }
                    }
                }

                l_scenes_list_v2_t *scene_node = EZPI_core_scenes_get_by_id_v2(scene_id);
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
}

void scenes_notification_add(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON *cj_user_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_userId_str);
        cJSON *cj_scene_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_notifications_str);

        if (cj_scene_id && cj_scene_id->valuestring && cj_user_id && cj_user_id->valuestring)
        {
            char *scene_str = ezlopi_nvs_read_str(cj_scene_id->valuestring);
            if (scene_str)
            {
                cJSON *cj_scene = cJSON_Parse(__FUNCTION__, scene_str);
                ezlopi_free(__FUNCTION__, scene_str);

                if (cj_scene)
                {
                    cJSON *cj_user_notifications = cJSON_GetObjectItem(__FUNCTION__, cj_scene, ezlopi_user_notifications_str);
                    if (cj_user_notifications)
                    {
                        cJSON_AddItemReferenceToArray(__FUNCTION__, cj_user_notifications, cj_user_id);
                    }

                    CJSON_TRACE("updated-scene", cj_scene);
                    char *updated_scene_str = cJSON_PrintBuffered(__FUNCTION__, cj_scene, 4096, false);
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
                l_scenes_list_v2_t *scene_node = EZPI_core_scenes_get_scene_head_v2();
                while (scene_node)
                {
                    if (scene_id == scene_node->_id)
                    {
                        EZPI_core_scenes_add_users_in_notifications(scene_node, cj_user_id);
                        break;
                    }
                    scene_node = scene_node->next;
                }
            }
        }
    }
}

void scenes_notification_remove(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON *cj_user_id_del = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_userId_str);
        cJSON *cj_scene_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_notifications_str);

        if (cj_scene_id && cj_scene_id->valuestring && cj_user_id_del && cj_user_id_del->valuestring)
        {
            char *scene_str = ezlopi_nvs_read_str(cj_scene_id->valuestring);
            if (scene_str)
            {
                cJSON *cj_scene = cJSON_Parse(__FUNCTION__, scene_str);
                ezlopi_free(__FUNCTION__, scene_str);

                if (cj_scene)
                {
                    cJSON *cj_user_notifications = cJSON_GetObjectItem(__FUNCTION__, cj_scene, ezlopi_user_notifications_str);
                    if (cj_user_notifications && cj_user_id_del)
                    {
                        uint32_t idx = 0;
                        cJSON *cj_user_id = NULL;
                        cJSON_ArrayForEach(cj_user_id, cj_user_notifications)
                        {
                            if (0 == strcmp(cj_user_id->valuestring, cj_user_id_del->valuestring))
                            {
                                cJSON_DeleteItemFromArray(__FUNCTION__, cj_user_notifications, idx);
                            }
                            idx++;
                        }
                    }

                    CJSON_TRACE("updated-scene", cj_scene);
                    char *updated_scene_str = cJSON_PrintBuffered(__FUNCTION__, cj_scene, 4096, false);
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
                l_scenes_list_v2_t *scene_node = EZPI_core_scenes_get_by_id_v2(scene_id);
                if (scene_node)
                {
                    if (0 == strcmp(scene_node->user_notifications->user_id, cj_user_id_del->valuestring))
                    {
                        l_user_notification_v2_t *user_id_del = scene_node->user_notifications;
                        scene_node->user_notifications = scene_node->user_notifications->next;
                        user_id_del->next = NULL;
                        EZPI_scenes_delete_user_notifications(user_id_del);
                    }
                    else
                    {
                        l_user_notification_v2_t *user_node = scene_node->user_notifications;
                        while (user_node->next)
                        {
                            if (0 == strcmp(user_node->user_id, cj_user_id_del->valuestring))
                            {
                                l_user_notification_v2_t *user_id_del = user_node;
                                user_node = user_node->next;
                                user_id_del->next = NULL;
                                EZPI_scenes_delete_user_notifications(user_id_del);
                                break;
                            }

                            user_node = user_node->next;
                        }
                    }
                }
            }
        }
    }
}

void scenes_room_set(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON *cj_scene_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_sceneId_str);
        cJSON *cj_room_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_roomId_str);
        if ((cj_scene_id && cj_scene_id->valuestring) && (cj_room_id && cj_room_id->valuestring))
        {
            char *scene_str = ezlopi_nvs_read_str(cj_scene_id->valuestring);
            if (scene_str)
            {
                cJSON *cj_scene = cJSON_Parse(__FUNCTION__, scene_str);
                ezlopi_free(__FUNCTION__, scene_str);

                if (cj_scene)
                {
                    cJSON_DeleteItemFromObject(__FUNCTION__, cj_scene, ezlopi_parent_id_str); // deletes if present
                    cJSON_AddStringToObject(__FUNCTION__, cj_scene, ezlopi_parent_id_str, cj_room_id->valuestring);

                    char *updated_scene_str = cJSON_PrintBuffered(__FUNCTION__, cj_scene, 4096, false);
                    cJSON_Delete(__FUNCTION__, cj_scene);
                    if (updated_scene_str)
                    {
                        ezlopi_nvs_write_str(updated_scene_str, strlen(updated_scene_str), cj_scene_id->valuestring);
                        ezlopi_free(__FUNCTION__, updated_scene_str);
                    }
                }
            }
        }
    }

    // Finally, Make sure 'result : {}' is present in broadcast
    if (NULL == cJSON_GetObjectItem(__FUNCTION__, cj_response, ezlopi_result_str))
    {
        cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    }
}

void scenes_time_list(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        EZPI_core_scenes_get_time_list(cJSON_AddArrayToObject(__FUNCTION__, cj_result, ezlopi_timeScenes_str));
    }
}

void scenes_house_modes_set(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str); // For NULL Broadcast
    cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_result && cj_params)
    {
        cJSON *cj_scene_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi__id_str);
        cJSON *cj_house_mode_arr = cJSON_GetObjectItem(__FUNCTION__, cj_params, "houseModes");

        if (cj_scene_id && cj_scene_id->valuestring && cj_house_mode_arr && cJSON_IsArray(cj_house_mode_arr))
        {
            char *scene_str = ezlopi_nvs_read_str(cj_scene_id->valuestring);
            if (scene_str)
            {
                cJSON *cj_scene = cJSON_Parse(__FUNCTION__, scene_str);
                ezlopi_free(__FUNCTION__, scene_str);

                if (cj_scene)
                {
                    cJSON_DeleteItemFromObject(__FUNCTION__, cj_scene, ezlopi_house_modes_str); // deletes if present

                    cJSON_AddItemToObject(__FUNCTION__, cj_scene, ezlopi_house_modes_str, cJSON_Duplicate(__FUNCTION__, cj_house_mode_arr, true));

                    cJSON *cj_test = cJSON_GetObjectItem(__FUNCTION__, cj_scene, ezlopi_house_modes_str);
                    if (cj_test)
                    {
                        CJSON_TRACE("new-house_mode:", cj_test);
                    }

                    char *updated_scene_str = cJSON_PrintBuffered(__FUNCTION__, cj_scene, 4096, false);
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
                l_scenes_list_v2_t *scene_node = EZPI_core_scenes_get_by_id_v2(scene_id);
                if (scene_node && scene_node->house_modes)
                {
                    EZPI_scenes_delete_house_modes(scene_node->house_modes);
                    if (NULL != (scene_node->house_modes = EZPI_scenes_populate_house_modes(cJSON_Duplicate(__FUNCTION__, cj_house_mode_arr, true))))
                    {
                        TRACE_S("Updating ... House_modes ; Success");
                    }
                }
            }
        }
    }
}

void scenes_action_block_test(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str); // For NULL Broadcast
    if (cj_result)
    {
        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON *cj_block = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_block_str);
            if (cj_block)
            {
                // 1. populate --> 'test_then_block' ;
                l_action_block_v2_t *test_then_block = (l_action_block_v2_t *)ezlopi_malloc(__FUNCTION__, sizeof(l_action_block_v2_t));
                if (test_then_block)
                {
                    memset(test_then_block, 0, sizeof(l_action_block_v2_t));

                    cJSON *dupli = cJSON_Duplicate(__FUNCTION__, cj_block, true);
                    if (dupli) // duplicate the 'cj_block' to avoid crashes
                    {
                        EZPI_scenes_populate_assign_action_block(test_then_block, dupli, SCENE_BLOCK_TYPE_THEN);
                        // CJSON_TRACE("test_then:", dupli);
                        cJSON_Delete(__FUNCTION__, dupli);
                    }

                    // 2. Now to fill the 's_ezlopi_core_http_mbedtls_t' variable and execute 'send_http_request'.
                    s_ezlopi_core_http_mbedtls_t *tmp_http_data = (s_ezlopi_core_http_mbedtls_t *)ezlopi_malloc(__FUNCTION__, sizeof(s_ezlopi_core_http_mbedtls_t));
                    if (tmp_http_data)
                    {
                        memset(tmp_http_data, 0, sizeof(s_ezlopi_core_http_mbedtls_t));
                        l_fields_v2_t *curr_field = test_then_block->fields;

                        const s_sendhttp_method_t __sendhttp_method[] = {
                            {.field_name = "request", .field_func = EZPI_parse_http_request_type},
                            {.field_name = "url", .field_func = EZPI_parse_http_url},
                            {.field_name = "credential", .field_func = EZPI_parse_http_creds},
                            {.field_name = "contentType", .field_func = EZPI_parse_http_content_type},
                            {.field_name = "content", .field_func = EZPI_parse_http_content},
                            {.field_name = "headers", .field_func = EZPI_parse_http_headers},
                            {.field_name = "skipSecurity", .field_func = EZPI_parse_http_skipsecurity},
                            {.field_name = NULL, .field_func = NULL},
                        };

                        while (NULL != curr_field) // fields
                        {
                            for (uint8_t i = 0; i < ((sizeof(__sendhttp_method) / sizeof(__sendhttp_method[i]))); i++)
                            {
                                if (EZPI_STRNCMP_IF_EQUAL(__sendhttp_method[i].field_name, curr_field->name, strlen(__sendhttp_method[i].field_name) + 1, strlen(curr_field->name) + 1))
                                {
                                    (__sendhttp_method[i].field_func)(tmp_http_data, curr_field);
                                    break;
                                }
                            }
                            curr_field = curr_field->next;
                        }
                        // now to trigger http_request and extract the response.
                        tmp_http_data->response = NULL;
                        tmp_http_data->response_maxlen = 0;
                        ezlopi_core_http_mbedtls_req(tmp_http_data); // Returns:- [response_buffer = &Memory_block]

                        if (tmp_http_data->response)
                        {
                            int code = 400;
                            char detail[100] = { 0 };
                            if (sscanf(tmp_http_data->response, "HTTP/1.1 %d %99s[^\n]", &code, detail) == 2)
                            {
                                cJSON_AddNumberToObject(__FUNCTION__, cj_result, ezlopi_httpAnswerCode_str, code);
                            }
                            else
                            {
                                cJSON_AddNumberToObject(__FUNCTION__, cj_result, ezlopi_httpAnswerCode_str, code);
                            }
                        }
                        else
                        {
                            cJSON_AddNumberToObject(__FUNCTION__, cj_result, ezlopi_httpAnswerCode_str, 404);
                        }

                        EZPI_free_http_mbedtls_struct(tmp_http_data);
                        ezlopi_free(__FUNCTION__, tmp_http_data);
                    }

                    EZPI_scenes_delete_action_blocks(test_then_block);
                }
            }
        }
    }
}

void scenes_block_enabled_set(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON *cj_scene_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_sceneId_str);
        cJSON *cj_block_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_blockId_str);
        bool block_en = false;
        CJSON_GET_VALUE_BOOL(cj_params, ezlopi_enabled_str, block_en);
        if (cj_scene_id && (NULL != cj_scene_id->valuestring) && cj_block_id && (NULL != cj_block_id->valuestring))
        {
            EZPI_core_scenes_block_enable_set_reset(cj_scene_id->valuestring, cj_block_id->valuestring, block_en);
        }
    }
}

void scenes_block_status_reset(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str); // For NULL broadcast
    if (cj_result)
    {
        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON *cj_scene_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_sceneId_str);
            if (cj_scene_id && (NULL != cj_scene_id->valuestring))
            {
                cJSON *cj_block_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_blockId_str);
                if (cj_block_id && (NULL != cj_block_id->valuestring))
                {
                    EZPI_core_scenes_reset_when_block(cj_scene_id->valuestring, cj_block_id->valuestring);
                }
                else
                {
                    EZPI_core_scenes_reset_when_block(cj_scene_id->valuestring, NULL);
                }
            }
        }
    }
}

void scenes_meta_set(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON *cj_meta = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_meta_str);
        if (cj_meta)
        {
            cJSON *cj_scene_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_sceneId_str);
            if (cj_scene_id && cj_scene_id->valuestring)
            {
                EZPI_core_scenes_set_meta_by_id(cj_scene_id->valuestring, NULL, cj_meta);
            }
        }
    }
}

void scenes_blockmeta_set(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON *cj_meta = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_meta_str);
        if (cj_meta)
        {
            cJSON *cj_scene_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_sceneId_str);
            cJSON *cj_block_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_blockId_str);
            if ((cj_scene_id && cj_scene_id->valuestring) && (cj_block_id && cj_block_id->valuestring))
            {
                #warning "The 'block_id' facility is only for 'when-blocks' [ 'Action-blocks' is not added in UI ]";
                EZPI_core_scenes_set_meta_by_id(cj_scene_id->valuestring, cj_block_id->valuestring, cj_meta);
            }
        }
    }
}

void scenes_stop(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str); // For NULL Broadcast
    if (cj_result)
    {
        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            TRACE_D("Stop executed");
            cJSON *cj_scene_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_sceneId_str);
            if (cj_scene_id && cj_scene_id->valuestring)
            {
                #warning "add support for thenGroup or elseGroups";
                uint32_t u32_scene_id = strtoul(cj_scene_id->valuestring, NULL, 16);
                ezlopi_meshbot_service_stop_for_scene_id(u32_scene_id);
            }
        }
    }
}

void scenes_clone(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str); // For NULL Broadcast
    if (cj_result)
    {
        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON *cj_scene_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi__id_str);
            if (cj_scene_id && cj_scene_id->valuestring)
            {
                char *scene_str = ezlopi_nvs_read_str(cj_scene_id->valuestring);
                if (scene_str)
                {
                    cJSON *cj_org_scene = cJSON_Parse(__FUNCTION__, scene_str);
                    if (cj_org_scene)
                    {
                        cJSON *cj_dup_scene = cJSON_Duplicate(__FUNCTION__, cj_org_scene, 1);
                        if (cj_dup_scene)
                        {
                            char name_buf[32] = { 0 };
                            cJSON *cj_custom_name = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_name_str);
                            if (cj_custom_name && cj_custom_name->valuestring && (0 < cj_custom_name->str_value_len))
                            {
                                snprintf(name_buf, sizeof(name_buf), "%s", cj_custom_name->valuestring);
                            }
                            else
                            {
                                cJSON *cj_get_name = cJSON_GetObjectItem(__FUNCTION__, cj_dup_scene, ezlopi_name_str);
                                if (cj_get_name && cj_get_name->valuestring && (0 < cj_get_name->str_value_len))
                                {
                                    uint32_t idx = 0;
                                    bool dupli_flag = false;
                                    l_scenes_list_v2_t *scene_node = NULL;

                                    do
                                    { // generate "XX_cloned(N)" and add to the duplicate scene.
                                        dupli_flag = false;
                                        snprintf(name_buf, sizeof(name_buf), "%s_cloned(%d)", cj_get_name->valuestring, ++idx);

                                        scene_node = EZPI_core_scenes_get_scene_head_v2();
                                        while (scene_node)
                                        { // check if the new-generated 'name' is redundant?
                                            if (EZPI_STRNCMP_IF_EQUAL(scene_node->name, name_buf, sizeof(scene_node->name), sizeof(name_buf)))
                                            {
                                                dupli_flag = true;
                                                break;
                                            }
                                            scene_node = scene_node->next;
                                        }
                                    } while (dupli_flag);
                                }
                                else
                                {
                                    snprintf(name_buf, sizeof(name_buf), "%s_cloned(N)", cj_scene_id->valuestring);
                                }
                            }

                            cJSON_DeleteItemFromObject(__FUNCTION__, cj_dup_scene, ezlopi__id_str);
                            cJSON_DeleteItemFromObject(__FUNCTION__, cj_dup_scene, ezlopi_name_str);
                            cJSON_AddStringToObject(__FUNCTION__, cj_dup_scene, ezlopi_name_str, name_buf);
                            CJSON_TRACE("__duplicated :", cj_dup_scene);

                            // store the 'new_scene' in nvs
                            uint32_t new_scene_id = EZPI_core_scenes_store_new_scene_v2(cj_dup_scene);
                            if (new_scene_id)
                            {
                                TRACE_D("new-scene-id: %08x", new_scene_id);
                                char tmp_buff[32];
                                snprintf(tmp_buff, sizeof(tmp_buff), "%08x", new_scene_id);
                                cJSON_AddStringToObject(__FUNCTION__, cj_request, ezlopi__id_str, tmp_buff); // this is for (reply_broadcast)
                                EZPI_core_scenes_new_scene_populate(cj_dup_scene, new_scene_id);
                                // Trigger new-scene to 'start'
                                // ezlopi_meshbot_service_start_scene(EZPI_core_scenes_get_by_id_v2(new_scene_id));
                            }

                            cJSON_Delete(__FUNCTION__, cj_dup_scene);
                        }

                        cJSON_Delete(__FUNCTION__, cj_org_scene);
                    }

                    ezlopi_free(__FUNCTION__, scene_str);
                }
            }
        }
    }
}

////// updater for scene
////// useful for 'hub.scenes.enabled.set'
void scene_changed(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_sender_str);
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_error_str);

    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_scene_changed_str);

    cJSON *cj_method = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_method_str);
    cJSON_AddItemToObject(__FUNCTION__, cj_response, ezlopi_changed_by_str, cJSON_Duplicate(__FUNCTION__, cj_method, cJSON_True));

    cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON *cj_scene_id = NULL;
        if (NULL != (cj_scene_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi__id_str)) ||
            NULL != (cj_scene_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_sceneId_str))) //  sometimes 'cj_request' contain the key: [either 'sceneId' or '_id']
        {
            if (cj_scene_id && cj_scene_id->valuestring)
            {
                char *scene_str = ezlopi_nvs_read_str(cj_scene_id->valuestring);
                if (scene_str)
                {
                    cJSON_AddRawToObject(__FUNCTION__, cj_response, ezlopi_result_str, scene_str);
                    ezlopi_free(__FUNCTION__, scene_str);
                }
            }
        }
    }

    // Finally, Make sure 'result : {}' is present in broadcast
    if (NULL == cJSON_GetObjectItem(__FUNCTION__, cj_response, ezlopi_result_str))
    {
        cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    }
}

void scene_added(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_sender_str);
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_error_str);

    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_scene_added_str);

    cJSON *new_scene_id = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi__id_str);
    if (new_scene_id && new_scene_id->valuestring)
    {
        char *new_scene = ezlopi_nvs_read_str(new_scene_id->valuestring);
        if (new_scene)
        {
            cJSON_AddRawToObject(__FUNCTION__, cj_response, ezlopi_result_str, new_scene);
            ezlopi_free(__FUNCTION__, new_scene);
        }
    }
    else /* sometime request contain the key: 'sceneId' instead of '_id' */
    {
        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON *new_scene_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_sceneId_str);
            if (new_scene_id && new_scene_id->valuestring)
            {
                char *new_scene = ezlopi_nvs_read_str(new_scene_id->valuestring);
                if (new_scene)
                {
                    cJSON_AddRawToObject(__FUNCTION__, cj_response, ezlopi_result_str, new_scene);
                    ezlopi_free(__FUNCTION__, new_scene);
                }
            }
        }
    }

    // Finally, Make sure 'result : {}' is present in broadcast
    if (NULL == cJSON_GetObjectItem(__FUNCTION__, cj_response, ezlopi_result_str))
    {
        cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    }
}

void scene_deleted(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_sender_str);
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_error_str);

    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_scene_deleted_str);

    cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON *cj_scene_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi__id_str);
        if (cj_scene_id && cj_scene_id->valuestring)
        {
            cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
            if (cj_result)
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi__id_str, cj_scene_id->valuestring);
                cJSON_AddBoolToObject(__FUNCTION__, cj_result, ezlopi_syncNotification_str, true);
            }
        }
    }

    // Finally, Make sure 'result : {}' is present in broadcast
    if (NULL == cJSON_GetObjectItem(__FUNCTION__, cj_response, ezlopi_result_str))
    {
        cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    }
}
#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS