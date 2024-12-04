#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include "ezlopi_util_trace.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_sntp.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_scenes_v2.h"
#include "ezlopi_core_scenes_edit.h"
#include "ezlopi_core_scenes_cjson.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_scenes_value.h"
#include "ezlopi_core_scenes_print.h"
#include "ezlopi_core_scenes_methods.h"
#include "ezlopi_core_scenes_when_methods.h"
#include "ezlopi_core_scenes_then_methods.h"
#include "ezlopi_core_scenes_status_changed.h"
#include "ezlopi_core_errors.h"

#include "ezlopi_cloud_constants.h"
#include "ezlopi_service_meshbot.h"
#include "EZLOPI_USER_CONFIG.h"

static l_scenes_list_v2_t *scenes_list_head_v2 = NULL;

static const f_scene_method_v2_t ezlopi_core_scenes_methods[] = {
#define EZLOPI_SCENE(method_type, name, func, category) func,
#include "ezlopi_core_scenes_method_types.h"
#undef EZLOPI_SCENE
};

static l_fields_v2_t *______new_field_populate(cJSON *cj_field);
static l_fields_v2_t *_____fields_populate(cJSON *cj_fields);

static void _____new_action_delay(s_action_delay_v2_t *action_delay, cJSON *cj_delay);
static void _____new_block_options_populate(s_block_options_v2_t *p_block_options, cJSON *cj_block_options);
static void __new_method_populate(s_method_v2_t *p_method, cJSON *cj_method);

static l_when_block_v2_t *____new_when_block_populate(cJSON *cj_when_block);
static l_when_block_v2_t *___when_blocks_populate(cJSON *cj_when_blocks);

static l_action_block_v2_t *____new_action_block_populate(cJSON *cj_then_block, e_scenes_block_type_v2_t block_type);
static l_action_block_v2_t *___action_blocks_populate(cJSON *cj_then_blocks, e_scenes_block_type_v2_t block_type);

static l_house_modes_v2_t *____new_house_mode_populate(cJSON *cj_house_mode);
static l_house_modes_v2_t *___house_modes_populate(cJSON *cj_house_modes);

static l_user_notification_v2_t *____new_user_notification_populate(cJSON *cj_user_notification);
static l_user_notification_v2_t *___user_notifications_populate(cJSON *cj_user_notifications);

static l_scenes_list_v2_t *__new_scene_populate(cJSON *cj_scene, uint32_t scene_id);
static l_scenes_list_v2_t *_scenes_populate(cJSON *cj_scene, uint32_t scene_id);

//------------------------------------------------------------------------------------

int ezlopi_scene_edit_by_id(uint32_t scene_id, cJSON *cj_scene)
{
    int ret = 0;

    if (EZPI_SUCCESS == ezlopi_core_scene_edit_store_updated_to_nvs(cj_scene)) // first store in nvs // add the new-block-id
    {
        if (EZPI_SUCCESS == ezlopi_core_scene_edit_update_id(scene_id, cj_scene)) // then populate to nvs
        {
            ret = 1;
        }
    }

    return ret;
}

void ezlopi_scene_add_users_in_notifications(l_scenes_list_v2_t *scene_node, cJSON *cj_user)
{
    if (scene_node && cj_user && cj_user->valuestring)
    {
        if (scene_node->user_notifications)
        {
            l_user_notification_v2_t *user_node = scene_node->user_notifications;
            while (user_node->next)
            {
                user_node = user_node->next;
            }
            user_node->next = ____new_user_notification_populate(cj_user);
        }
        else
        {
            scene_node->user_notifications = ____new_user_notification_populate(cj_user);
        }
    }
}

uint32_t ezlopi_store_new_scene_v2(cJSON *cj_new_scene)
{
    uint32_t new_scene_id = 0;
    if (cj_new_scene)
    {
        new_scene_id = ezlopi_cloud_generate_scene_id();
        char new_scene_id_str[32];
        snprintf(new_scene_id_str, sizeof(new_scene_id_str), "%08x", new_scene_id);
        cJSON_AddStringToObject(__FUNCTION__, cj_new_scene, ezlopi__id_str, new_scene_id_str);

        if (ezlopi_core_scene_add_when_blockId_if_reqd(cj_new_scene))
        {
            TRACE_S("==> Added new_blockIds : SUCCESS");
        }

        if (ezlopi_core_scene_add_group_id_if_reqd(cj_new_scene))
        {
            TRACE_S("==> Added new_group_id : SUCCESS");
        }

        char *new_scene_str = cJSON_PrintBuffered(__FUNCTION__, cj_new_scene, 4096, false);
        TRACE_D("length of 'new_scene_str': %d", strlen(new_scene_str));

        if (new_scene_str)
        {
            if (EZPI_SUCCESS == ezlopi_nvs_write_str(new_scene_str, strlen(new_scene_str) + 1, new_scene_id_str))
            {
                bool free_scene_list_str = 1;
                char *scenes_list_str = ezlopi_nvs_scene_get_v2();
                if (NULL == scenes_list_str)
                {
                    scenes_list_str = "[]";
                    free_scene_list_str = 0;
                }

                cJSON *cj_scenes_list = cJSON_Parse(__FUNCTION__, scenes_list_str);
                if (cj_scenes_list)
                {
                    cJSON *cj_new_scene_id = cJSON_CreateNumber(__FUNCTION__, new_scene_id);
                    if (cj_new_scene_id)
                    {
                        if (!cJSON_AddItemToArray(cj_scenes_list, cj_new_scene_id))
                        {
                            cJSON_Delete(__FUNCTION__, cj_new_scene_id);
                            ezlopi_nvs_delete_stored_data_by_id(new_scene_id);
                            new_scene_id = 0;
                        }
                        else
                        {
                            char *updated_scenes_list = cJSON_PrintBuffered(__FUNCTION__, cj_scenes_list, 1024, false);
                            TRACE_D("length of 'updated_scenes_list': %d", strlen(updated_scenes_list));

                            if (updated_scenes_list)
                            {
                                TRACE_D("updated_scenes_list: %s", updated_scenes_list);
                                if (EZPI_SUCCESS == ezlopi_nvs_scene_set_v2(updated_scenes_list))
                                {
                                    TRACE_D("Scenes list updated.");
                                }
                                else
                                {
                                    TRACE_E("Scenes list update failed!");
                                }

                                ezlopi_free(__FUNCTION__, updated_scenes_list);
                            }
                        }
                    }

                    cJSON_Delete(__FUNCTION__, cj_scenes_list);
                }

                if (free_scene_list_str)
                {
                    ezlopi_free(__FUNCTION__, scenes_list_str);
                }
            }
            else
            {
                new_scene_id = 0;
            }

            ezlopi_free(__FUNCTION__, new_scene_str);
        }
        else
        {
            new_scene_id = 0;
        }
    }

    return new_scene_id;
}

uint32_t ezlopi_scenes_get_list_v2(cJSON *cj_scenes_array)
{
    int ret = 0;
    char *scenes_ids = ezlopi_nvs_scene_get_v2();
    if (scenes_ids)
    {
        cJSON *cj_scenes_ids = cJSON_Parse(__FUNCTION__, scenes_ids);
        if (cj_scenes_ids)
        {
            int array_size = cJSON_GetArraySize(cj_scenes_ids);
            for (int i = 0; i < array_size; i++)
            {
                cJSON *cj_scene_id = cJSON_GetArrayItem(cj_scenes_ids, i);
                if (cj_scene_id && cj_scene_id->valuedouble)
                {
                    char scene_id_str[32];
                    uint32_t scene_id = (uint32_t)cj_scene_id->valuedouble;
                    snprintf(scene_id_str, sizeof(scene_id_str), "%08x", scene_id);
                    char *scene_str = ezlopi_nvs_read_str(scene_id_str);
                    if (scene_str)
                    {
                        cJSON *cj_scene = cJSON_Parse(__FUNCTION__, scene_str);
                        if (cj_scene)
                        {
                            cJSON_AddStringToObject(__FUNCTION__, cj_scene, ezlopi__id_str, scene_id_str); // NVS already might have '_id'
                            if (!cJSON_AddItemToArray(cj_scenes_array, cj_scene))
                            {
                                cJSON_Delete(__FUNCTION__, cj_scene);
                            }
                            else
                            {
                                ret += 1;
                            }
                        }

                        ezlopi_free(__FUNCTION__, scene_str);
                    }
                }
            }

            cJSON_Delete(__FUNCTION__, cj_scenes_ids);
        }

        ezlopi_free(__FUNCTION__, scenes_ids);
    }

    return ret;
}

l_scenes_list_v2_t *ezlopi_scenes_get_by_id_v2(uint32_t _id)
{
    l_scenes_list_v2_t *ret_scene = NULL;
    l_scenes_list_v2_t *scenes_list = scenes_list_head_v2;
    while (scenes_list)
    {
        if (_id == scenes_list->_id)
        {
            ret_scene = scenes_list;
            break;
        }
        scenes_list = scenes_list->next;
    }

    return ret_scene;
}

l_scenes_list_v2_t *ezlopi_scenes_pop_by_id_v2(uint32_t _id)
{
    l_scenes_list_v2_t *ret_scene = NULL;

    if (_id == scenes_list_head_v2->_id)
    {
        ret_scene = scenes_list_head_v2;
        scenes_list_head_v2 = scenes_list_head_v2->next;
        ret_scene->next = NULL;
    }
    else
    {
        l_scenes_list_v2_t *scenes_list = scenes_list_head_v2;
        while (scenes_list->next)
        {
            if (_id == scenes_list->next->_id)
            {
                ret_scene = scenes_list->next;
                scenes_list->next = scenes_list->next->next;
                ret_scene->next = NULL;
                break;
            }
            scenes_list = scenes_list->next;
        }
    }

    return ret_scene;
}

l_scenes_list_v2_t *ezlopi_scenes_new_scene_populate(cJSON *cj_new_scene, uint32_t scene_id)
{
    return _scenes_populate(cj_new_scene, scene_id);
}

void ezlopi_scenes_depopulate_by_id_v2(uint32_t _id)
{
    if (EZPI_SUCCESS == ezlopi_meshbot_stop_without_broadcast(ezlopi_scenes_get_by_id_v2(_id)))
    {
        ezlopi_scenes_delete(ezlopi_scenes_pop_by_id_v2(_id));
    }
}

ezlopi_error_t ezlopi_scenes_enable_disable_scene_by_id_v2(uint32_t _id, bool enabled_flag)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (_id)
    {
        char tmp_buffer[32]; // store the scene name here
        snprintf(tmp_buffer, sizeof(tmp_buffer), "%08x", _id);

        char *scene_str = ezlopi_nvs_read_str(tmp_buffer);
        if (scene_str)
        {
            // converting string to cJSON format
            cJSON *cj_scene = cJSON_Parse(__FUNCTION__, scene_str);
            if (cj_scene)
            {
                cJSON *enable_item = cJSON_GetObjectItem(__FUNCTION__, cj_scene, ezlopi_enabled_str);
                TRACE_S("prev_enable => [%s]", (enable_item->type == cJSON_True) ? "True" : "False");
                if ((enable_item && cJSON_IsBool(enable_item)) && (enable_item->type != ((enabled_flag) ? cJSON_True : cJSON_False)))
                {
                    enable_item->type = (enabled_flag) ? cJSON_True : cJSON_False;

                    l_scenes_list_v2_t *curr_scene = ezlopi_scenes_get_by_id_v2(_id);
                    if (curr_scene)
                    {
                        curr_scene->enabled = enabled_flag;
                    }

                    CJSON_TRACE("cj_scene----> 2. updated", cj_scene);

                    /*  DONOT use : 'ezlopi_core_scene_edit_store_updated_to_nvs' .. Here */
                    char *update_scene_str = cJSON_PrintBuffered(__FUNCTION__, cj_scene, 4096, false);
                    TRACE_D("length of 'update_scene_str': %d", strlen(update_scene_str));

                    if (update_scene_str)
                    {
                        cJSON *cj_scene_id = cJSON_GetObjectItem(__FUNCTION__, cj_scene, ezlopi__id_str);
                        if (cj_scene_id && cj_scene_id->valuestring)
                        {
                            ezlopi_nvs_delete_stored_data_by_name(cj_scene_id->valuestring);
                            ret = ezlopi_nvs_write_str(update_scene_str, strlen(update_scene_str), cj_scene_id->valuestring);

                            if (EZPI_SUCCESS == ret)
                            {
                                TRACE_W("nvs updated successfull");
                            }
                            else
                            {
                                TRACE_E("Error!! failed");
                            }
                        }

                        ezlopi_free(__FUNCTION__, update_scene_str);
                    }
                }
                else
                {
                    TRACE_W("Scene-Already [%s] or, invalid option ", (true == enabled_flag) ? "enabled" : "disabled");
                }
                cJSON_Delete(__FUNCTION__, cj_scene);
            }
            // free the scene_name
            ezlopi_free(__FUNCTION__, scene_str);
        }
    }
    return ret;
}

void ezlopi_scenes_remove_id_from_list_v2(uint32_t _id)
{
    char *scenes_id_list_str = ezlopi_nvs_scene_get_v2();
    if (scenes_id_list_str)
    {
        cJSON *cj_scene_id_list = cJSON_Parse(__FUNCTION__, scenes_id_list_str);
        if (cj_scene_id_list)
        {
            uint32_t list_len = cJSON_GetArraySize(cj_scene_id_list);

            for (int idx = 0; idx < list_len; idx++)
            {
                cJSON *cj_id = cJSON_GetArrayItem(cj_scene_id_list, idx);
                if (cj_id)
                {
                    if (cj_id->valuedouble == _id)
                    {
                        cJSON_DeleteItemFromArray(__FUNCTION__, cj_scene_id_list, idx);

                        char *updated_id_list_str = cJSON_PrintBuffered(__FUNCTION__, cj_scene_id_list, 1024, false);
                        TRACE_D("length of 'updated_id_list_str': %d", strlen(updated_id_list_str));

                        if (updated_id_list_str)
                        {
                            ezlopi_nvs_scene_set_v2(updated_id_list_str);
                            ezlopi_free(__FUNCTION__, updated_id_list_str);
                        }
                    }
                }
            }

            cJSON_Delete(__FUNCTION__, cj_scene_id_list);
        }
        ezlopi_free(__FUNCTION__, scenes_id_list_str);
    }
}

l_scenes_list_v2_t *ezlopi_scenes_get_scenes_head_v2(void)
{
    return scenes_list_head_v2;
}

f_scene_method_v2_t ezlopi_scene_get_method_v2(e_scene_method_type_t scene_method_type)
{
    f_scene_method_v2_t method_ptr = NULL;
    if ((scene_method_type > EZLOPI_SCENE_METHOD_TYPE_NONE) && (scene_method_type < EZLOPI_SCENE_METHOD_TYPE_MAX))
    {
        method_ptr = ezlopi_core_scenes_methods[scene_method_type];
    }
    return method_ptr;
}

//-------------------------------------------------------------------------------------------
#if 0 // may be used in future
static void __remove_residue_scenes_ids_from_list(void)
{
    TRACE_D("---------- # Removing [Scenes] residue-Ids # ----------");
    // check --> nvs_devgrp_list for unncessary "residue-IDs" & update the list
    uint32_t residue_nvs_scenes_id = 0;
    bool scenes_list_has_residue = false; // this indicates absence of residue-IDs // those IDs which are still in the "nvs-list" but doesnot not exists in "nvs-body"
    char *list_ptr = NULL;

    do
    {
        if (scenes_list_has_residue)
        {
            if (0 != residue_nvs_scenes_id)
            {
                ezlopi_scenes_remove_id_from_list_v2(residue_nvs_scenes_id);
            }
            scenes_list_has_residue = false;
        }

        list_ptr = ezlopi_nvs_scene_get_v2();
        if (list_ptr)
        {
            cJSON *cj_id_list = cJSON_Parse(__FUNCTION__, list_ptr);
            if (cj_id_list)
            {
                int array_size = cJSON_GetArraySize(cj_id_list);
                for (int i = 0; i < array_size; i++)
                {
                    cJSON *cj_id = cJSON_GetArrayItem(cj_id_list, i);
                    if (cj_id && cj_id->valuedouble)
                    {
                        char tmp_buffer[32];
                        snprintf(tmp_buffer, sizeof(tmp_buffer), "%08x", (uint32_t)cj_id->valuedouble);

                        if (NULL == ezlopi_nvs_read_str(tmp_buffer))
                        {
                            residue_nvs_scenes_id = (uint32_t)cj_id->valuedouble; // A residue_id is found..
                            scenes_list_has_residue = true;                       // this will trigger a removal of "invalid_nvs_devgrp_id" .
                            break;                                                // get out of for
                        }
                    }
                }
            }
        }
    } while (scenes_list_has_residue);
    TRACE_D("---------- # --------------------------------- # ----------");
}
#endif
//-------------------------------------------------------------------------------------------
/**
 * @brief main functions to initiated scenes-nodes
 *
 */
ezlopi_error_t ezlopi_scenes_init_v2(void)
{
    ezlopi_error_t error = EZPI_ERR_JSON_PARSE_FAILED;
    char *scenes_id_list_str = ezlopi_nvs_scene_get_v2();

    if (scenes_id_list_str)
    {
        TRACE_D("scenes_id_list_str : %s", scenes_id_list_str);
        cJSON *cj_scenes_ids = cJSON_Parse(__FUNCTION__, scenes_id_list_str);
        if (cj_scenes_ids)
        {
            int array_size = cJSON_GetArraySize(cj_scenes_ids);
            for (int i = 0; i < array_size; i++)
            {
                cJSON *cj_scene_id = cJSON_GetArrayItem(cj_scenes_ids, i);
                if (cj_scene_id && cj_scene_id->valuedouble)
                {
                    char tmp_buffer[32];
                    uint32_t scene_id = (uint32_t)cj_scene_id->valuedouble;

                    snprintf(tmp_buffer, sizeof(tmp_buffer), "%08x", scene_id);
                    ezlopi_cloud_update_scene_id((uint32_t)scene_id);
                    char *scene_str = ezlopi_nvs_read_str(tmp_buffer);
                    if (scene_str)
                    {
                        cJSON *cj_scene = cJSON_Parse(__FUNCTION__, scene_str);
                        if (cj_scene)
                        {
                            l_scenes_list_v2_t *new_scene = _scenes_populate(cj_scene, scene_id);
                            cJSON_Delete(__FUNCTION__, cj_scene);

                            if (new_scene)
                            {
                                cJSON *cj_new_scene = ezlopi_scenes_create_cjson_scene(new_scene);
                                // CJSON_TRACE("new_scene", cj_new_scene);
                                cJSON_Delete(__FUNCTION__, cj_new_scene);
                            }
                        }

                        ezlopi_free(__FUNCTION__, scene_str);
                    }
                }
            }
            error = EZPI_SUCCESS;
            cJSON_Delete(__FUNCTION__, cj_scenes_ids);
        }

        ezlopi_free(__FUNCTION__, scenes_id_list_str);
    }
#if ENABLE_SCENES_PRINT
    ezlopi_scenes_print(scenes_list_head_v2);
#endif
    return error;
}

static l_scenes_list_v2_t *_scenes_populate(cJSON *cj_scene, uint32_t scene_id)
{
    l_scenes_list_v2_t *new_scene_node = NULL;
    if (scenes_list_head_v2)
    {
        l_scenes_list_v2_t *curr_scene = scenes_list_head_v2;
        while (curr_scene->next)
        {
            curr_scene = curr_scene->next;
        }

        curr_scene->next = __new_scene_populate(cj_scene, scene_id);
        new_scene_node = curr_scene->next;
    }
    else
    {
        // CJSON_TRACE("new-scene", cj_scene);
        scenes_list_head_v2 = __new_scene_populate(cj_scene, scene_id);
        new_scene_node = scenes_list_head_v2;
    }

    return new_scene_node;
}

static l_scenes_list_v2_t *__new_scene_populate(cJSON *cj_scene, uint32_t scene_id)
{
    l_scenes_list_v2_t *new_scene = NULL;
    if (cj_scene)
    {
        new_scene = ezlopi_malloc(__FUNCTION__, sizeof(l_scenes_list_v2_t));
        if (new_scene)
        {
            uint32_t tmp_success_creating_scene = 1;

            memset(new_scene, 0, sizeof(l_scenes_list_v2_t));

            new_scene->_id = scene_id;
            new_scene->task_handle = NULL;
            new_scene->status = EZLOPI_SCENE_STATUS_STOPPED;

            CJSON_GET_VALUE_BOOL(cj_scene, ezlopi_enabled_str, new_scene->enabled);
            CJSON_GET_VALUE_BOOL(cj_scene, ezlopi_is_group_str, new_scene->is_group);

            {
                char tmp_grp_id[32] = {0};
                CJSON_GET_VALUE_STRING_BY_COPY(cj_scene, ezlopi_group_id_str, tmp_grp_id);
                if (0 < strlen(tmp_grp_id))
                {
                    new_scene->group_id = (uint32_t)strtoul(tmp_grp_id, NULL, 16);
                    TRACE_S("group_id: %08x", new_scene->group_id);
                    ezlopi_cloud_update_group_id(new_scene->group_id);
                }
            }

            CJSON_GET_VALUE_STRING_BY_COPY(cj_scene, ezlopi_name_str, new_scene->name);
            CJSON_GET_VALUE_STRING_BY_COPY(cj_scene, ezlopi_parent_id_str, new_scene->parent_id);

            {
                cJSON *cj_meta = cJSON_GetObjectItem(__FUNCTION__, cj_scene, ezlopi_meta_str);
                if (cj_meta && (cJSON_Object == cj_meta->type))
                {
                    new_scene->meta = cJSON_Duplicate(__FUNCTION__, cj_meta, 1);
                }
            }

            {
                cJSON *cj_user_notifications = cJSON_GetObjectItem(__FUNCTION__, cj_scene, ezlopi_user_notifications_str);
                if (cj_user_notifications && (cJSON_Array == cj_user_notifications->type))
                {
                    new_scene->user_notifications = ___user_notifications_populate(cj_user_notifications);
                }
            }

            {
                cJSON *cj_house_modes = cJSON_GetObjectItem(__FUNCTION__, cj_scene, ezlopi_house_modes_str);
                if (cj_house_modes && (cJSON_Array == cj_house_modes->type))
                {
                    new_scene->house_modes = ___house_modes_populate(cj_house_modes);
                }
            }

            {
                cJSON *cj_then_blocks = cJSON_GetObjectItem(__FUNCTION__, cj_scene, ezlopi_then_str);
                if (cj_then_blocks && (cJSON_Array == cj_then_blocks->type))
                {
                    new_scene->then_block = ___action_blocks_populate(cj_then_blocks, SCENE_BLOCK_TYPE_THEN);
                }
            }

            {
                cJSON *cj_when_blocks = cJSON_GetObjectItem(__FUNCTION__, cj_scene, ezlopi_when_str);
                if (cj_when_blocks && (cJSON_Array == cj_when_blocks->type))
                {
                    new_scene->when_block = ___when_blocks_populate(cj_when_blocks);
                }
            }

            {
                cJSON *cj_else_blocks = cJSON_GetObjectItem(__FUNCTION__, cj_scene, ezlopi_else_str);
                if (cj_else_blocks && (cJSON_Array == cj_else_blocks->type))
                {
                    new_scene->else_block = ___action_blocks_populate(cj_else_blocks, SCENE_BLOCK_TYPE_ELSE);
                }
            }

            if (0 == tmp_success_creating_scene)
            {
                // delete new_scene
            }
        }
    }

    return new_scene;
}

static l_user_notification_v2_t *___user_notifications_populate(cJSON *cj_user_notifications)
{
    l_user_notification_v2_t *tmp_user_notifications_head = NULL;
    if (cj_user_notifications && (cJSON_Array == cj_user_notifications->type))
    {
        cJSON *cj_user_notification = NULL;
        int user_notifications_idx = 0;

        while (NULL != (cj_user_notification = cJSON_GetArrayItem(cj_user_notifications, user_notifications_idx++)))
        {
            if (tmp_user_notifications_head)
            {
                l_user_notification_v2_t *curr_user_notification = tmp_user_notifications_head;
                while (curr_user_notification->next)
                {
                    curr_user_notification = curr_user_notification->next;
                }
                curr_user_notification->next = ____new_user_notification_populate(cj_user_notification);
            }
            else
            {
                tmp_user_notifications_head = ____new_user_notification_populate(cj_user_notification);
            }
        }
    }

    return tmp_user_notifications_head;
}

static l_user_notification_v2_t *____new_user_notification_populate(cJSON *cj_user_notification)
{
    l_user_notification_v2_t *new_user_notification = NULL;

    if (cj_user_notification && cj_user_notification->valuestring)
    {
        new_user_notification = ezlopi_malloc(__FUNCTION__, sizeof(l_user_notification_v2_t));
        if (new_user_notification)
        {
            memset(new_user_notification, 0, sizeof(l_user_notification_v2_t));
            snprintf(new_user_notification->user_id, sizeof(new_user_notification->user_id), "%s", cj_user_notification->valuestring);
            new_user_notification->next = NULL;
        }
    }

    return new_user_notification;
}

static l_house_modes_v2_t *___house_modes_populate(cJSON *cj_house_modes)
{
    l_house_modes_v2_t *tmp_house_mode_head = NULL;
    if (cj_house_modes)
    {
        int house_mode_idx = 0;
        cJSON *cj_house_mode = NULL;

        while (NULL != (cj_house_mode = cJSON_GetArrayItem(cj_house_modes, house_mode_idx++)))
        {
            if (tmp_house_mode_head)
            {
                l_house_modes_v2_t *tmp_house_mode = tmp_house_mode_head;
                while (tmp_house_mode->next)
                {
                    tmp_house_mode = tmp_house_mode->next;
                }

                tmp_house_mode->next = ____new_house_mode_populate(cj_house_mode);
            }
            else
            {
                tmp_house_mode_head = ____new_house_mode_populate(cj_house_mode);
            }
        }
    }

    return tmp_house_mode_head;
}

static l_house_modes_v2_t *____new_house_mode_populate(cJSON *cj_house_mode)
{
    l_house_modes_v2_t *new_house_mode = NULL;

    if (cj_house_mode && cj_house_mode->valuestring)
    {
        new_house_mode = ezlopi_malloc(__FUNCTION__, sizeof(l_house_modes_v2_t));
        if (new_house_mode)
        {
            memset(new_house_mode, 0, sizeof(l_house_modes_v2_t));
            strncpy(new_house_mode->house_mode, cj_house_mode->valuestring, sizeof(new_house_mode->house_mode));
            new_house_mode->next = NULL;
        }
    }

    return new_house_mode;
}

static l_action_block_v2_t *___action_blocks_populate(cJSON *cj_action_blocks, e_scenes_block_type_v2_t block_type)
{
    l_action_block_v2_t *tmp_action_block_head = NULL;
    if (cj_action_blocks)
    {
        int then_block_idx = 0;
        cJSON *cj_action_block = NULL;

        while (NULL != (cj_action_block = cJSON_GetArrayItem(cj_action_blocks, then_block_idx++)))
        {
            if (tmp_action_block_head)
            {
                l_action_block_v2_t *tmp_then_block = tmp_action_block_head;
                while (tmp_then_block->next)
                {
                    tmp_then_block = tmp_then_block->next;
                }

                tmp_then_block->next = ____new_action_block_populate(cj_action_block, block_type);
            }
            else
            {
                tmp_action_block_head = ____new_action_block_populate(cj_action_block, block_type);
            }
        }
    }

    return tmp_action_block_head;
}

static l_action_block_v2_t *____new_action_block_populate(cJSON *cj_action_block, e_scenes_block_type_v2_t block_type)
{
    l_action_block_v2_t *new_then_block = ezlopi_malloc(__FUNCTION__, sizeof(l_action_block_v2_t));
    if (new_then_block)
    {
        memset(new_then_block, 0, sizeof(l_action_block_v2_t));
        cJSON *cj_block_options = cJSON_GetObjectItem(__FUNCTION__, cj_action_block, ezlopi_blockOptions_str);
        if (cj_block_options)
        {
            _____new_block_options_populate(&new_then_block->block_options, cj_block_options);
        }

        new_then_block->block_type = block_type;
        cJSON *cj_delay = cJSON_GetObjectItem(__FUNCTION__, cj_action_block, ezlopi_delay_str);
        if (cj_delay)
        {
            _____new_action_delay(&new_then_block->delay, cj_delay);
        }

        cJSON *cj_fields = cJSON_GetObjectItem(__FUNCTION__, cj_action_block, ezlopi_fields_str);
        if (cj_fields)
        {
            new_then_block->fields = _____fields_populate(cj_fields);
        }

        cJSON *cj__temp_id = cJSON_GetObjectItem(__FUNCTION__, cj_action_block, "_tempId");
        if (cj__temp_id && cj__temp_id->valuestring)
        {
            snprintf(new_then_block->_tempId, sizeof(new_then_block->_tempId), "%s", cj__temp_id->valuestring);
        }
    }

    return new_then_block;
}

static l_when_block_v2_t *___when_blocks_populate(cJSON *cj_when_blocks)
{
    l_when_block_v2_t *tmp_when_block_head = NULL;
    if (cj_when_blocks)
    {
        int when_block_idx = 0;
        cJSON *cj_when_block = NULL;

        while (NULL != (cj_when_block = cJSON_GetArrayItem(cj_when_blocks, when_block_idx++)))
        {
            if (tmp_when_block_head)
            {
                l_when_block_v2_t *tmp_when_block = tmp_when_block_head;
                while (tmp_when_block->next)
                {
                    tmp_when_block = tmp_when_block->next;
                }

                tmp_when_block->next = ____new_when_block_populate(cj_when_block);
            }
            else
            {
                tmp_when_block_head = ____new_when_block_populate(cj_when_block);
            }
        }
    }

    return tmp_when_block_head;
}
static l_when_block_v2_t *____new_when_block_populate(cJSON *cj_when_block)
{
    l_when_block_v2_t *new_when_block = ezlopi_malloc(__FUNCTION__, sizeof(l_when_block_v2_t));
    if (new_when_block)
    {
        memset(new_when_block, 0, sizeof(l_when_block_v2_t));

        cJSON *cj_blockEnable = cJSON_GetObjectItem(__FUNCTION__, cj_when_block, ezlopi_block_enable_str);
        if ((cj_blockEnable) && cJSON_IsBool(cj_blockEnable))
        {
            new_when_block->block_enable = (cJSON_True == cj_blockEnable->type) ? true : false;
            TRACE_D("blockEnable: %d", new_when_block->block_enable);
        }
        {
            if (NULL == new_when_block->when_grp)
            {
                new_when_block->when_grp = ezlopi_malloc(__FUNCTION__, sizeof(l_group_block_type_t));
            }
            if (new_when_block->when_grp)
            {
                memset(new_when_block->when_grp, 0, sizeof(l_group_block_type_t));
                CJSON_GET_VALUE_STRING_BY_COPY(cj_when_block, ezlopi_blockName_str, new_when_block->when_grp->grp_blockName);
                if (0 < strlen(new_when_block->when_grp->grp_blockName))
                {
                    TRACE_D("group_blockName : %s ", new_when_block->when_grp->grp_blockName);
                }

                char grp_id_str[32] = {0};
                CJSON_GET_VALUE_STRING_BY_COPY(cj_when_block, ezlopi_group_id_str, grp_id_str);
                if (0 < strlen(grp_id_str))
                {
                    new_when_block->when_grp->grp_id = (uint32_t)strtoul(grp_id_str, NULL, 16);
                    TRACE_D("group_blockId : %08x ", new_when_block->when_grp->grp_id);
                }
            }
        }

        {
            char tmp_block_id[32] = {0};
            CJSON_GET_VALUE_STRING_BY_COPY(cj_when_block, ezlopi_blockId_str, tmp_block_id);
            if (0 < strlen(tmp_block_id))
            {
                new_when_block->blockId = (uint32_t)strtoul(tmp_block_id, NULL, 16);
                TRACE_D("blockId (edit): %08x", new_when_block->blockId);
                ezlopi_cloud_update_when_blockId(new_when_block->blockId);
            }
        }

        cJSON *cj_block_options = cJSON_GetObjectItem(__FUNCTION__, cj_when_block, ezlopi_blockOptions_str);
        if (cj_block_options)
        {
            _____new_block_options_populate(&new_when_block->block_options, cj_block_options);
        }

        new_when_block->block_type = SCENE_BLOCK_TYPE_WHEN;

        cJSON *cj_new_blockmeta = cJSON_GetObjectItem(__FUNCTION__, cj_when_block, "blockMeta");
        if (cj_new_blockmeta && (cJSON_Object == cj_new_blockmeta->type))
        {
            new_when_block->cj_block_meta = cJSON_Duplicate(__FUNCTION__, cj_new_blockmeta, 1);
        }

        cJSON *cj_fields = cJSON_GetObjectItem(__FUNCTION__, cj_when_block, ezlopi_fields_str);
        if (cj_fields)
        {
            new_when_block->fields = _____fields_populate(cj_fields);
        }
    }

    return new_when_block;
}

static void _____new_block_options_populate(s_block_options_v2_t *p_block_options, cJSON *cj_block_options)
{
    cJSON *cj_method = cJSON_GetObjectItem(__FUNCTION__, cj_block_options, ezlopi_method_str);
    if (cj_method)
    {
        __new_method_populate(&p_block_options->method, cj_method);
    }

    if (0 == strncmp(ezlopi_function_str, p_block_options->method.name, 9))
    {
        cJSON *cj_func = cJSON_GetObjectItem(__FUNCTION__, cj_block_options, ezlopi_function_str);
        if (cj_func)
        {
            p_block_options->cj_function = cJSON_Duplicate(__FUNCTION__, cj_func, cJSON_True);
        }
    }
}

static void __new_method_populate(s_method_v2_t *p_method, cJSON *cj_method)
{
    CJSON_GET_VALUE_STRING_BY_COPY(cj_method, ezlopi_name_str, p_method->name);
    p_method->type = ezlopi_scenes_method_get_type_enum(p_method->name);
}

static void _____new_action_delay(s_action_delay_v2_t *action_delay, cJSON *cj_delay)
{
    if (action_delay && cj_delay)
    {
        CJSON_GET_VALUE_DOUBLE(cj_delay, "days", action_delay->days);
        CJSON_GET_VALUE_DOUBLE(cj_delay, "hours", action_delay->hours);
        CJSON_GET_VALUE_DOUBLE(cj_delay, "minutes", action_delay->minutes);
        CJSON_GET_VALUE_DOUBLE(cj_delay, "seconds", action_delay->seconds);
    }
}

static l_fields_v2_t *_____fields_populate(cJSON *cj_fields)
{
    l_fields_v2_t *tmp_fields_head = NULL;
    if (cj_fields)
    {
        int fields_idx = 0;
        cJSON *cj_field = NULL;
        while (NULL != (cj_field = cJSON_GetArrayItem(cj_fields, fields_idx++)))
        {
            if (tmp_fields_head)
            {
                l_fields_v2_t *tmp_flield = tmp_fields_head;
                while (tmp_flield->next)
                {
                    tmp_flield = tmp_flield->next;
                }
                tmp_flield->next = ______new_field_populate(cj_field);
            }
            else
            {
                tmp_fields_head = ______new_field_populate(cj_field);
            }
        }
    }

    return tmp_fields_head;
}

static void _______fields_get_value(l_fields_v2_t *field, cJSON *cj_value)
{
    if (field && cj_value)
    {
        // CJSON_TRACE("cj_value", cj_value);
        TRACE_I("type: %s", ezlopi_scene_get_scene_value_type_name(field->value_type));
        switch (cj_value->type)
        {
        case cJSON_Number:
        {
            field->field_value.e_type = VALUE_TYPE_NUMBER;
            field->field_value.u_value.value_double = cj_value->valuedouble;
            TRACE_I("value: %f", field->field_value.u_value.value_double);
            break;
        }
        case cJSON_String:
        {
            field->field_value.e_type = VALUE_TYPE_STRING;
            uint32_t value_len = strlen(cj_value->valuestring) + 1;
            field->field_value.u_value.value_string = ezlopi_malloc(__FUNCTION__, value_len);
            if (field->field_value.u_value.value_string)
            {
                snprintf(field->field_value.u_value.value_string, value_len, "%s", cj_value->valuestring);
                TRACE_I("value: %s", field->field_value.u_value.value_string);
            }
            else
            {
                TRACE_E("Malloc failed!");
            }
            break;
        }
        case cJSON_True:
        {
            field->field_value.e_type = VALUE_TYPE_BOOL;
            field->field_value.u_value.value_bool = true;
            TRACE_I("value: true");
            break;
        }
        case cJSON_False:
        {
            field->field_value.e_type = VALUE_TYPE_BOOL;
            field->field_value.u_value.value_bool = false;
            TRACE_I("value: false");
            break;
        }
        case cJSON_Object:
        {
            if (EZLOPI_VALUE_TYPE_BLOCK == field->value_type) // when there is only one-block [ 'cJSON_Object' ]
            {
                field->field_value.e_type = VALUE_TYPE_BLOCK;
                CJSON_TRACE("cj_single_block", cj_value);
                if (field->field_value.u_value.when_block)
                {
                    l_when_block_v2_t *curr_when_block = field->field_value.u_value.when_block;
                    while (curr_when_block->next)
                    {
                        curr_when_block = curr_when_block->next;
                    }
                    curr_when_block->next = ____new_when_block_populate(cj_value);
                }
                else
                {
                    field->field_value.u_value.when_block = ____new_when_block_populate(cj_value);
                }
            }
            else
            {
                field->field_value.e_type = VALUE_TYPE_CJSON;
                field->field_value.u_value.cj_value = cJSON_Duplicate(__FUNCTION__, cj_value, cJSON_True);
                CJSON_TRACE("value", field->field_value.u_value.cj_value);
            }
            break;
        }
        case cJSON_Array:
        {
            int block_idx = 0;
            cJSON *cj_block = NULL;

            switch (field->value_type)
            {
            case EZLOPI_VALUE_TYPE_ARRAY:
            case EZLOPI_VALUE_TYPE_24_HOURS_TIME:
            case EZLOPI_VALUE_TYPE_24_HOURS_TIME_ARRAY:
            case EZLOPI_VALUE_TYPE_INT_ARRAY:
            case EZLOPI_VALUE_TYPE_HMS_INTERVAL:
            case EZLOPI_VALUE_TYPE_HOUSE_MODE_ID_ARRAY:
            {
                field->field_value.e_type = VALUE_TYPE_CJSON;
                field->field_value.u_value.cj_value = cJSON_Duplicate(__FUNCTION__, cj_value, cJSON_True);
                break;
            }
            case EZLOPI_VALUE_TYPE_BLOCKS: // there are more than one-blocks [since 'cJSON_Array' ]
            {
                field->field_value.e_type = VALUE_TYPE_BLOCK;
                while (NULL != (cj_block = cJSON_GetArrayItem(cj_value, block_idx++)))
                {
                    // CJSON_TRACE("cj_block", cj_block);

                    if (field->field_value.u_value.when_block)
                    {
                        l_when_block_v2_t *curr_when_block = field->field_value.u_value.when_block;
                        while (curr_when_block->next)
                        {
                            curr_when_block = curr_when_block->next;
                        }
                        curr_when_block->next = ____new_when_block_populate(cj_block);
                    }
                    else
                    {
                        field->field_value.u_value.when_block = ____new_when_block_populate(cj_block);
                    }
                }
                break;
            }
            default:
            {
                TRACE_W("Value type not implemented");
            }
            }
            break;
        }
        default:
        {
            field->field_value.e_type = VALUE_TYPE_UNDEFINED;
            TRACE_E("cj_value type: %d", cj_value->type);
            break;
        }
        }
    }
}

static l_fields_v2_t *______new_field_populate(cJSON *cj_field)
{
    l_fields_v2_t *field = NULL;
    if (cj_field)
    {
        field = ezlopi_malloc(__FUNCTION__, sizeof(l_fields_v2_t));
        if (field)
        {
            memset(field, 0, sizeof(l_fields_v2_t));

            // field->field_value.u_value.when_block

            CJSON_GET_VALUE_STRING_BY_COPY(cj_field, ezlopi_name_str, field->name);
            field->value_type = ezlopi_core_scenes_value_get_type(cj_field, ezlopi_type_str);
            _______fields_get_value(field, cJSON_GetObjectItem(__FUNCTION__, cj_field, ezlopi_value_str));
        }
    }

    return field;
}

//---------------------- For group-id   (only for complete_new setup)-------------------------------------------------------------
static void ______add_groupID_and_flag(cJSON *cj_target)
{
    uint32_t group_id = ezlopi_cloud_generate_scene_group_id();
    char group_id_str[32];
    snprintf(group_id_str, sizeof(group_id_str), "%08x", group_id);
    TRACE_S("new_Group_ID:  %s", group_id_str);

    /* 1. Adding is_group_flag */
    cJSON *cj_is_group = cJSON_GetObjectItem(__FUNCTION__, cj_target, ezlopi_is_group_str);
    if (cj_is_group && (cJSON_False == cj_is_group->type))
    {
        cj_is_group->type = cJSON_True;
    }
    else
    {
        cJSON_AddBoolToObject(__FUNCTION__, cj_target, ezlopi_is_group_str, cJSON_True);
    }

    /* 2. Adding groupID */
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_target, ezlopi_group_id_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_target, ezlopi_group_id_str, group_id_str); // nvs
}
static bool ____check_and_append_group_id(cJSON *cj_when_block)
{
    bool add_groupId_flag = false; // this flag triggers new group_addition
    int fields_block_idx = 0;
    int value_block_idx = 0;

    // <1> check for multiple/nested scene
    cJSON *cj_fields_blocks = cJSON_GetObjectItem(__FUNCTION__, cj_when_block, ezlopi_fields_str);
    if (cj_fields_blocks && (cJSON_Array == cj_fields_blocks->type))
    {
        cJSON *cj_fields_block = NULL;
        while (NULL != (cj_fields_block = cJSON_GetArrayItem(cj_fields_blocks, fields_block_idx++)))
        {
            cJSON *name = cJSON_GetObjectItem(__FUNCTION__, cj_fields_block, ezlopi_name_str);
            cJSON *type = cJSON_GetObjectItem(__FUNCTION__, cj_fields_block, ezlopi_type_str);
            if (name && type)
            {
                if ((EZPI_STRNCMP_IF_EQUAL(name->valuestring, "blocks", strlen(name->valuestring), 7) && EZPI_STRNCMP_IF_EQUAL(type->valuestring, "blocks", strlen(type->valuestring), 7)) ||
                    (EZPI_STRNCMP_IF_EQUAL(name->valuestring, "block", strlen(name->valuestring), 6) && EZPI_STRNCMP_IF_EQUAL(type->valuestring, "block", strlen(type->valuestring), 6)))
                {
                    cJSON *cj_value_blocks = cJSON_GetObjectItem(__FUNCTION__, cj_fields_block, ezlopi_value_str);
                    if (cj_value_blocks && (cJSON_Array == cj_value_blocks->type))
                    {
                        cJSON *cj_value_block = NULL;
                        while (NULL != (cj_value_block = cJSON_GetArrayItem(cj_value_blocks, value_block_idx++)))
                        {
                            add_groupId_flag |= ____check_and_append_group_id(cj_value_block); // if any one of the 'when' condition has group_id ; then "add_groupId_flag = true"
                        }
                    }
                    else if (cj_value_blocks && (cJSON_Object == cj_value_blocks->type))
                    {
                        add_groupId_flag |= ____check_and_append_group_id(cj_value_blocks); // if any one of the 'when' condition has group_id ; then "add_groupId_flag = true"
                    }
                }
                else
                {
                    // TRACE_D("No further ----> groups!!");
                    break;
                }
            }
        }
    }

    // <2> Add group_id if 'block_name' exists
    cJSON *cj_group_blockName = cJSON_GetObjectItem(__FUNCTION__, cj_when_block, ezlopi_blockName_str);
    if (cj_group_blockName && (NULL != cj_group_blockName->valuestring))
    {
        add_groupId_flag = true;
        ______add_groupID_and_flag(cj_when_block);
    }

    return add_groupId_flag;
}
int ezlopi_core_scene_add_group_id_if_reqd(cJSON *cj_new_scene)
{
    int ret = 0;
    /* [ In each-element form 'when-array' ] --> you can check for block_name and add group-id here*/
    cJSON *cj_when_blocks = cJSON_GetObjectItem(__FUNCTION__, cj_new_scene, ezlopi_when_str);
    if (cj_when_blocks && (cJSON_Array == cj_when_blocks->type))
    {
        bool add_when_blockId_flag = false; // this flag triggers new group_addition
        int when_block_idx = 0;
        cJSON *cj_when_block = NULL;
        while (NULL != (cj_when_block = cJSON_GetArrayItem(cj_when_blocks, when_block_idx++)))
        {
            add_when_blockId_flag |= ____check_and_append_group_id(cj_when_block);
        }

        // checks for 'group_blockName' in main-case.
        if (add_when_blockId_flag)
        {
            ret = 1;
            /* Adding group-ID when --> block_name is present.*/
            ______add_groupID_and_flag(cj_new_scene);
        }
    }
    return ret;
}

//---------------------- For blockId    (only for complete_new setup)---------------------------------------------------------------
static int _____check_and_add_when_blockId(cJSON *cj_new_scene_when_block)
{
    int ret = 0;
    bool add_when_blockId_flag = false; // this flag triggers new-blockId addition
    int fields_block_idx = 0;
    int value_block_idx = 0;

    // <1> For single when-case
    cJSON *cj_blockOptions = cJSON_GetObjectItem(__FUNCTION__, cj_new_scene_when_block, ezlopi_blockOptions_str);
    if (cj_blockOptions)
    {
        cJSON *cj_method = cJSON_GetObjectItem(__FUNCTION__, cj_blockOptions, ezlopi_method_str);
        if (cj_method)
        {
            cJSON *cj_args = cJSON_GetObjectItem(__FUNCTION__, cj_method, "args");
            if (cj_args)
            {
                if ((NULL == cJSON_GetObjectItem(__FUNCTION__, cj_args, "blocks")) &&
                    (NULL == cJSON_GetObjectItem(__FUNCTION__, cj_args, "block")))
                {
                    // TRACE_D("adding new-block-ID :");
                    add_when_blockId_flag = true;
                }
            }
        }
    }

    // <2> For multiple nested when-case
    cJSON *cj_fields_blocks = cJSON_GetObjectItem(__FUNCTION__, cj_new_scene_when_block, ezlopi_fields_str);
    if (cj_fields_blocks && (cJSON_Array == cj_fields_blocks->type))
    {
        cJSON *cj_fields_block = NULL;
        while (NULL != (cj_fields_block = cJSON_GetArrayItem(cj_fields_blocks, fields_block_idx++)))
        {
            cJSON *name = cJSON_GetObjectItem(__FUNCTION__, cj_fields_block, ezlopi_name_str);
            cJSON *type = cJSON_GetObjectItem(__FUNCTION__, cj_fields_block, ezlopi_type_str);
            if (name && type)
            {
                if ((EZPI_STRNCMP_IF_EQUAL(name->valuestring, "blocks", strlen(name->valuestring), 7) && EZPI_STRNCMP_IF_EQUAL(type->valuestring, "blocks", strlen(type->valuestring), 7)) ||
                    (EZPI_STRNCMP_IF_EQUAL(name->valuestring, "block", strlen(name->valuestring), 6) && EZPI_STRNCMP_IF_EQUAL(type->valuestring, "block", strlen(type->valuestring), 6)))
                {
                    cJSON *cj_value_blocks = cJSON_GetObjectItem(__FUNCTION__, cj_fields_block, ezlopi_value_str);
                    if (cj_value_blocks && (cJSON_Array == cj_value_blocks->type))
                    {
                        cJSON *cj_value_block = NULL;
                        while (NULL != (cj_value_block = cJSON_GetArrayItem(cj_value_blocks, value_block_idx++)))
                        {
                            _____check_and_add_when_blockId(cj_value_block); /* RECURSIVE call*/
                        }
                    }
                    else if (cj_value_blocks && (cJSON_Object == cj_value_blocks->type))
                    {
                        _____check_and_add_when_blockId(cj_value_blocks); /* RECURSIVE call*/
                    }
                }
                else
                {
                    // TRACE_D("No further ----> Blocks!!");
                    break;
                }
            }
        }
    }

    if (add_when_blockId_flag)
    {
        uint32_t new_blockId = ezlopi_cloud_generate_scene_when_blockId();
        char new_blockId_str[32];
        snprintf(new_blockId_str, sizeof(new_blockId_str), "%08x", new_blockId);
        TRACE_S("---> new_when_blockId:  %s", new_blockId_str);

        /* 1. Adding block_en flag */
        cJSON *cj_block_en = cJSON_GetObjectItem(__FUNCTION__, cj_new_scene_when_block, ezlopi_block_enable_str);
        if (cj_block_en && (cJSON_False == cj_block_en->type))
        {
            cj_block_en->type = cJSON_True;
        }
        else
        {
            cJSON_AddBoolToObject(__FUNCTION__, cj_new_scene_when_block, ezlopi_block_enable_str, cJSON_True);
        }

        /* 2. Adding block_id */
        cJSON_DeleteItemFromObject(__FUNCTION__, cj_new_scene_when_block, ezlopi_blockId_str);
        cJSON_AddStringToObject(__FUNCTION__, cj_new_scene_when_block, ezlopi_blockId_str, new_blockId_str); // nvs
        ret = 1;
    }
    return ret;
}
int ezlopi_core_scene_add_when_blockId_if_reqd(cJSON *cj_new_scene)
{
    int ret = 0;
    cJSON *cj_when_blocks = cJSON_GetObjectItem(__FUNCTION__, cj_new_scene, ezlopi_when_str);
    if (cj_when_blocks && (cJSON_Array == cj_when_blocks->type))
    {
        cJSON *cj_when_block = NULL;
        int when_block_idx = 0;
        while (NULL != (cj_when_block = cJSON_GetArrayItem(cj_when_blocks, when_block_idx++)))
        { // [ In each-element form 'when-array' ] --> you can check for block_name and add group-id here
            ret = _____check_and_add_when_blockId(cj_when_block);
        }
    }
    return ret;
}

//---------------------- For Function-block ---------------------------------------------------------------
#if 0 /* ENABLE/DEIABLE Flag of FUNCTION -->> [for future use] */
//--------------------------------------------------------------------------------------------------
//                  Functions for : scene latch-Enable-flag change
//--------------------------------------------------------------------------------------------------
static void ____modify_function_in_blockmeta(cJSON *cj_when_block, bool enable_status)
{
    cJSON *cj_blockMeta = cJSON_GetObjectItem(__FUNCTION__, cj_when_block, "blockMeta");
    if (cj_blockMeta)
    {
        cJSON *cj_ruleTrigger = cJSON_GetObjectItem(__FUNCTION__, cj_blockMeta, "ruleTrigger");
        if (cj_ruleTrigger)
        {
            cJSON *cj_function = cJSON_GetObjectItem(__FUNCTION__, cj_ruleTrigger, "function");
            if (cj_function)
            {
                cJSON *cj_latch = cJSON_GetObjectItem(__FUNCTION__, cj_function, "latch");
                if (cj_latch)
                {
                    cJSON *cj_enabled = cJSON_GetObjectItem(__FUNCTION__, cj_latch, "enabled");
                    if (cJSON_IsBool(cj_enabled) && cj_enabled)
                    {
                        // may need to include 'naming' filter
                        cj_enabled->type = (enable_status ? cJSON_True : cJSON_False); /* change latch-status in nvs*/
                        CJSON_TRACE("blockMeta", cj_blockMeta);
                    }
                }
            }
            else
            {
                TRACE_E("error !! no function in BlockMeta");
            }
        }
    }
}

static bool ____change_latch_status_in_blockOptions(cJSON *cj_when_block, bool enable_status)
{
    bool ret = false;
    cJSON *cj_blockOptions = cJSON_GetObjectItem(__FUNCTION__, cj_when_block, "blockOptions");
    if (cj_blockOptions)
    {
        cJSON *cj_function = cJSON_GetObjectItem(__FUNCTION__, cj_blockOptions, "function");
        if (cj_function)
        {
            cJSON *cj_latch = cJSON_GetObjectItem(__FUNCTION__, cj_function, "latch");
            if (cj_latch)
            {
                cJSON *cj_enabled = cJSON_GetObjectItem(__FUNCTION__, cj_latch, "enabled");
                if (cJSON_IsBool(cj_enabled) && cj_enabled)
                {
                    // may need to include 'naming' filter
                    ret = true;
                    cj_enabled->type = (enable_status ? cJSON_True : cJSON_False); /* change latch-status in nvs*/
                    CJSON_TRACE("blockOptions", cj_blockOptions);
                }
            }
        }
        else
        {
            TRACE_E("error !! no function in BLOCK-Options");
        }
    }
    return ret;
}
static bool ___enable_disable_latch_with_blockId(cJSON *cj_when_block, uint32_t blockId, bool enable_status)
{
    bool latch_cleared = false;
    int fields_block_idx = 0;
    int value_block_idx = 0;

    /* <1> single scene function */
    cJSON *cj_blockId = cJSON_GetObjectItem(__FUNCTION__, cj_when_block, "blockId");
    if (cj_blockId && cj_blockId->valuestring)
    {
        uint32_t tmp_blockId = (uint32_t)strtoul(cj_blockId->valuestring, NULL, 16);
        if (1 == ((0 < blockId) ? (tmp_blockId == blockId ? 1 : 0) : 1)) // blockId is uint32_t
        {
            ____modify_function_in_blockmeta(cj_when_block, enable_status);
            latch_cleared = ____change_latch_status_in_blockOptions(cj_when_block, enable_status);
        }
    }
    else
    {
        /* <2> nested scene with function combined by 'And/OR' */
        cJSON *cj_fields_blocks = cJSON_GetObjectItem(__FUNCTION__, cj_when_block, "fields");
        if (cj_fields_blocks && (cJSON_Array == cj_fields_blocks->type))
        {
            cJSON *cj_fields_block = NULL;
            while (NULL != (cj_fields_block = cJSON_GetArrayItem(cj_fields_blocks, fields_block_idx++)))
            {

                cJSON *name = cJSON_GetObjectItem(__FUNCTION__, cj_fields_block, "name");
                cJSON *type = cJSON_GetObjectItem(__FUNCTION__, cj_fields_block, "type");
                if (name && type)
                {
                    if ((EZPI_STRNCMP_IF_EQUAL(name->valuestring, "blocks", strlen(name->valuestring), 7) && EZPI_STRNCMP_IF_EQUAL(type->valuestring, "blocks", strlen(type->valuestring), 7)) ||
                        (EZPI_STRNCMP_IF_EQUAL(name->valuestring, "block", strlen(name->valuestring), 6) && EZPI_STRNCMP_IF_EQUAL(type->valuestring, "block", strlen(type->valuestring), 6)))
                    {
                        /* now scanning the value-section within 'fields-block'*/
                        cJSON *cj_value_blocks = cJSON_GetObjectItem(__FUNCTION__, cj_fields_block, "value");
                        if (cj_value_blocks && (cJSON_Array == cj_value_blocks->type))
                        {
                            cJSON *cj_value_block = NULL;
                            while (NULL != (cj_value_block = cJSON_GetArrayItem(cj_value_blocks, value_block_idx++)))
                            {
                                latch_cleared = ___enable_disable_latch_with_blockId(cj_value_block, blockId, enable_status);
                            }
                        }
                        else if (cj_value_blocks && (cJSON_Object == cj_value_blocks->type)) // single value-block
                        {
                            latch_cleared = ___enable_disable_latch_with_blockId(cj_value_blocks, blockId, enable_status);
                        }
                    }
                    else
                    {
                        TRACE_D("No further ----> Blocks!!");
                        break;
                    }
                }
            }
        }

        if (0 < blockId) /* This case arrives when -->> [1. no-block-id is given]  ;  [2. 'Main-when-block' has function.]  */
        {
            latch_cleared = ____change_latch_status_in_blockOptions(cj_when_block, enable_status);
        }
    }
    return latch_cleared;
}

int ezlopi_core_scene_set_reset_latch_enable(const char *sceneId_str, const char *blockId_str, bool enable_status)
{
    int ret = 0;
    if (sceneId_str && blockId_str)
    {
        uint32_t sceneId = (uint32_t)strtoul(sceneId_str, NULL, 16);
        uint32_t blockId = (uint32_t)strtoul(blockId_str, NULL, 16);
        l_scenes_list_v2_t *curr_scene = ezlopi_scenes_get_by_id_v2(sceneId);
        if (curr_scene)
        {
            /*first disable in scene-linked-list*/
            s_when_function_t *function_state = (s_when_function_t *)curr_scene->when_block->fields->user_arg;
            if (function_state)
            {
                function_state->transtion_instant = 0;
                function_state->current_state = false;
            }

            /* secondly change the flag in nvs*/
            char *scene_str = ezlopi_nvs_read_str(sceneId_str);
            if (scene_str)
            {
                // converting string to cJSON format
                cJSON *cj_scene = cJSON_Parse(__FUNCTION__, scene_str); /* "params" : {...}*/
                if (cj_scene)
                {
                    bool latch_cleared = false;
                    int when_block_idx = 0;
                    cJSON *cj_when_block = NULL;
                    cJSON *cj_when_blocks = cJSON_GetObjectItem(__FUNCTION__, cj_scene, "when");
                    while (NULL != (cj_when_block = cJSON_GetArrayItem(cj_when_blocks, when_block_idx++)))
                    {
                        latch_cleared = ___enable_disable_latch_with_blockId(cj_when_block, blockId, enable_status);
                    }

                    if (latch_cleared)
                    {
                        /*  DONOT use : 'ezlopi_core_scene_edit_store_updated_to_nvs' .. Here */
                        char *update_scene_str = cJSON_PrintBuffered(__FUNCTION__, cj_scene, 4096, false);
                        TRACE_D("length of 'update_scene_str': %d", strlen(update_scene_str));

                        if (update_scene_str)
                        {
                            cJSON *cj_scene_id = cJSON_GetObjectItem(__FUNCTION__, cj_scene, ezlopi__id_str);
                            if (cj_scene_id && cj_scene_id->valuestring)
                            {
                                ezlopi_nvs_delete_stored_data_by_name(cj_scene_id->valuestring);

                                ret = (EZPI_SUCCESS == ezlopi_nvs_write_str(update_scene_str, strlen(update_scene_str), cj_scene_id->valuestring));
                                if (ret)
                                {
                                    TRACE_W("nvs updated successfull");
                                }
                                else
                                {
                                    TRACE_E("Error!! failed");
                                }
                            }

                            ezlopi_free(__FUNCTION__, update_scene_str);
                        }
                    }
                    cJSON_Delete(__FUNCTION__, cj_scene);
                }
                ezlopi_free(__FUNCTION__, scene_str);
            }
        }
    }
    return ret;
}
#endif

//--------------------------------------------------------------------------------------------------
//                  Functions for : Latch of when-block
//--------------------------------------------------------------------------------------------------
int ezlopi_core_scene_reset_latch_state(const char *sceneId_str, const char *blockId_str)
{
    int ret = 0;

    uint32_t sceneId = strtoul(sceneId_str, NULL, 16);
    l_scenes_list_v2_t *curr_scene = ezlopi_scenes_get_by_id_v2(sceneId);
    if (curr_scene)
    {
        l_when_block_v2_t *curr_when_block = curr_scene->when_block;
        while (curr_when_block)
        {
#warning "incomplete : need to iterate throught nested-when";
            s_when_function_t *function_state = (s_when_function_t *)curr_scene->when_block->fields->user_arg;
            if (function_state)
            {
                ret = 1;

                /* if block-id is given */
                if (NULL != blockId_str)
                {
                    uint32_t tmp_blockId = (uint32_t)(strtoul(blockId_str, NULL, 16));
                    if (curr_when_block->blockId == tmp_blockId)
                    {
                        /* Now reset the curr_function_state of this latch */
                        function_state->transtion_instant = 0;
                        function_state->current_state = false;
                        break;
                    }
                }
                else
                {
                    function_state->transtion_instant = 0;
                    function_state->current_state = false;
                }

                // reset this latch block once
                if (!curr_when_block->block_status_reset_once)
                {
                    curr_when_block->block_status_reset_once = true;
                }
            }

            curr_when_block = curr_when_block->next;
        }
    }
    return ret;
}

//--------------------------------------------------------------------------------------------------
//                  Functions for : when-block ( ALL when ; with/without latches)
//--------------------------------------------------------------------------------------------------
int ezlopi_core_scene_reset_when_block(const char *sceneId_str, const char *blockId_str)
{
    int ret = 0;

    /* 1. first turn-ON 'reset-flag' for sceneId */
    uint32_t sceneId = strtoul(sceneId_str, NULL, 16);

    l_scenes_list_v2_t *curr_scene = ezlopi_scenes_get_by_id_v2(sceneId);
    if (curr_scene)
    {
        ret = 1;
        l_when_block_v2_t *curr_when_block = curr_scene->when_block;
        while (curr_when_block)
        {
            if (NULL != blockId_str)
            {
                uint32_t tmp_block_id = (uint32_t)strtoul(blockId_str, NULL, 16);
                if (curr_when_block->blockId == tmp_block_id)
                {
                    curr_when_block->block_status_reset_once = true;
                    break;
                }
            }
            else
            {
                curr_when_block->block_status_reset_once = true;
            }
            curr_when_block = curr_when_block->next;
        }
    }

    /* 2. reset latch with ->> sceneId & blockId*/
    ret = ezlopi_core_scene_reset_latch_state(sceneId_str, blockId_str);

    return ret;
}

//--------------------------------------------------------------------------------------------------
//                  Functions for : scene block-en-changes only
//--------------------------------------------------------------------------------------------------
static bool _____change_block_en_status(cJSON *cj_when_block, bool enable_status)
{
    bool ret = false;
    cJSON *cj_block_en = cJSON_GetObjectItem(__FUNCTION__, cj_when_block, "block_enable");
    if (cJSON_IsBool(cj_block_en) && cj_block_en)
    {
        ret = true;
        // TRACE_S(" block_en => %d", enable_status);
        cj_block_en->type = (enable_status ? cJSON_True : cJSON_False); /* change latch-status in nvs*/
    }

    return ret;
}
static bool ___enable_disable_block_en_with_blockId(cJSON *cj_when_block, uint32_t blockId, bool enable_status)
{
    bool block_en_changed = false;
    int fields_block_idx = 0;
    int value_block_idx = 0;

    /* <1> single scene function */
    // CJSON_TRACE("cj_when_block : ", cj_when_block);
    cJSON *cj_blockId = cJSON_GetObjectItem(__FUNCTION__, cj_when_block, "blockId");
    if ((cj_blockId && cj_blockId->valuestring))
    {
        uint32_t curr_blockId = (uint32_t)strtoul(cj_blockId->valuestring, NULL, 16);
        if (curr_blockId == blockId)
        {
            block_en_changed = _____change_block_en_status(cj_when_block, enable_status);
        }
    }
    else
    { /* <2> nested scene with function combined by 'And/OR' */
        cJSON *cj_fields_blocks = cJSON_GetObjectItem(__FUNCTION__, cj_when_block, "fields");
        if (cj_fields_blocks && (cJSON_Array == cj_fields_blocks->type))
        {
            cJSON *cj_fields_block = NULL;
            while (NULL != (cj_fields_block = cJSON_GetArrayItem(cj_fields_blocks, fields_block_idx++)))
            {
                cJSON *name = cJSON_GetObjectItem(__FUNCTION__, cj_fields_block, "name");
                cJSON *type = cJSON_GetObjectItem(__FUNCTION__, cj_fields_block, "type");
                if (name && type)
                {
                    if ((EZPI_STRNCMP_IF_EQUAL(name->valuestring, "blocks", strlen(name->valuestring), 7) && EZPI_STRNCMP_IF_EQUAL(type->valuestring, "blocks", strlen(type->valuestring), 7)) ||
                        (EZPI_STRNCMP_IF_EQUAL(name->valuestring, "block", strlen(name->valuestring), 6) && EZPI_STRNCMP_IF_EQUAL(type->valuestring, "block", strlen(type->valuestring), 6)))
                    {
                        /* now scanning the value-section within 'fields-block'*/
                        cJSON *cj_value_blocks = cJSON_GetObjectItem(__FUNCTION__, cj_fields_block, "value");
                        if (cj_value_blocks && (cJSON_Array == cj_value_blocks->type))
                        {
                            cJSON *cj_value_block = NULL;
                            while (NULL != (cj_value_block = cJSON_GetArrayItem(cj_value_blocks, value_block_idx++)))
                            {
                                if (true == (block_en_changed = ___enable_disable_block_en_with_blockId(cj_value_block, blockId, enable_status)))
                                {
                                    break; // changed only targeted blockID.
                                }
                            }
                        }
                        else if (cj_value_blocks && (cJSON_Object == cj_value_blocks->type)) // single value-block
                        {
                            block_en_changed = ___enable_disable_block_en_with_blockId(cj_value_blocks, blockId, enable_status);
                        }
                    }
                    else
                    {
                        // TRACE_D("No further ----> Blocks!!");
                        break;
                    }
                }
            }
        }
    }

    return block_en_changed;
}
ezlopi_error_t ezlopi_core_scene_block_enable_set_reset(const char *sceneId_str, const char *blockId_str, bool enable_status)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (sceneId_str && blockId_str)
    {
        uint32_t sceneId = (uint32_t)strtoul(sceneId_str, NULL, 16);
        uint32_t blockId = (uint32_t)strtoul(sceneId_str, NULL, 16);
        l_scenes_list_v2_t *curr_scene = ezlopi_scenes_get_by_id_v2(sceneId);
        if (curr_scene)
        {
            /* First change the flag in nvs*/
            char *scene_str = ezlopi_nvs_read_str(sceneId_str);
            if (scene_str)
            {
                // converting string to cJSON format
                cJSON *cj_scene = cJSON_Parse(__FUNCTION__, scene_str); /* "params" : {...}*/
                if (cj_scene)
                {
                    bool block_enabled_changed = false;
                    int when_block_idx = 0;
                    cJSON *cj_when_block = NULL;
                    cJSON *cj_when_blocks = cJSON_GetObjectItem(__FUNCTION__, cj_scene, "when");
                    while (NULL != (cj_when_block = cJSON_GetArrayItem(cj_when_blocks, when_block_idx++)))
                    {
                        block_enabled_changed |= ___enable_disable_block_en_with_blockId(cj_when_block, blockId, enable_status);
                    }

                    if (block_enabled_changed)
                    {
                        /*  DONOT use : 'ezlopi_core_scene_edit_store_updated_to_nvs' .. Here */
                        {
                            char *update_scene_str = cJSON_PrintBuffered(__FUNCTION__, cj_scene, 4096, false);
                            TRACE_D("length of 'update_scene_str': %d", strlen(update_scene_str));

                            if (update_scene_str)
                            {
                                cJSON *cj_scene_id = cJSON_GetObjectItem(__FUNCTION__, cj_scene, ezlopi__id_str);
                                if (cj_scene_id && cj_scene_id->valuestring)
                                {
                                    ezlopi_nvs_delete_stored_data_by_name(cj_scene_id->valuestring);
                                    ret = ezlopi_nvs_write_str(update_scene_str, strlen(update_scene_str), cj_scene_id->valuestring);

                                    if (EZPI_SUCCESS == ret)
                                    {
                                        TRACE_W("nvs updated successfull");
                                        /*secondly Change in ll */
                                        ezlopi_core_scene_edit_update_id(sceneId, cj_scene);
                                    }
                                    else
                                    {
                                        TRACE_E("Error!! failed");
                                    }
                                }

                                ezlopi_free(__FUNCTION__, update_scene_str);
                            }
                        }
                    }
                    cJSON_Delete(__FUNCTION__, cj_scene);
                }
                ezlopi_free(__FUNCTION__, scene_str);
            }
        }
    }
    return ret;
}

//--------------------------------------------------------------------------------------------------
//                  Functions for : Adding scene-Meta & metaBlock
//--------------------------------------------------------------------------------------------------
static bool _____put_new_block_meta(cJSON *cj_when_block, cJSON *cj_new_blockmeta)
{
    bool ret = false;
    if (cj_when_block && cj_new_blockmeta)
    {
        cJSON_DeleteItemFromObject(__FUNCTION__, cj_when_block, "blockMeta");
        ret = cJSON_AddItemToObject(__FUNCTION__, cj_when_block, "blockMeta", cJSON_Duplicate(__FUNCTION__, cj_new_blockmeta, 1));
    }
    return ret;
}
static bool ___add_new_blockmeta_by_id(cJSON *cj_when_block, uint32_t blockId, cJSON *cj_blockmeta)
{
    bool block_meta_changed = false;
    int fields_block_idx = 0;
    int value_block_idx = 0;

    /* <1> single scene function */
    // CJSON_TRACE("cj_when_block : ", cj_when_block);
    cJSON *cj_blockId = cJSON_GetObjectItem(__FUNCTION__, cj_when_block, "blockId");
    if (cj_blockId && cj_blockId->valuestring)
    {
        uint32_t tmp_blockId = (uint32_t)strtoul(cj_blockId->valuestring, NULL, 16);
        if (tmp_blockId == blockId)
        {
            block_meta_changed = _____put_new_block_meta(cj_when_block, cj_blockmeta);
        }
    }
    else
    { /* <2> nested scene with function combined by 'And/OR/NOT' */
        cJSON *cj_fields_blocks = cJSON_GetObjectItem(__FUNCTION__, cj_when_block, "fields");
        if (cj_fields_blocks && (cJSON_Array == cj_fields_blocks->type))
        {
            cJSON *cj_fields_block = NULL;
            while (NULL != (cj_fields_block = cJSON_GetArrayItem(cj_fields_blocks, fields_block_idx++)))
            {
                cJSON *name = cJSON_GetObjectItem(__FUNCTION__, cj_fields_block, "name");
                cJSON *type = cJSON_GetObjectItem(__FUNCTION__, cj_fields_block, "type");
                if (name && type)
                {
                    if ((EZPI_STRNCMP_IF_EQUAL(name->valuestring, "blocks", strlen(name->valuestring), 7) && EZPI_STRNCMP_IF_EQUAL(type->valuestring, "blocks", strlen(type->valuestring), 7)) ||
                        (EZPI_STRNCMP_IF_EQUAL(name->valuestring, "block", strlen(name->valuestring), 6) && EZPI_STRNCMP_IF_EQUAL(type->valuestring, "block", strlen(type->valuestring), 6)))
                    {
                        /* now scanning the value-section within 'fields-block'*/
                        cJSON *cj_value_blocks = cJSON_GetObjectItem(__FUNCTION__, cj_fields_block, "value");
                        if (cj_value_blocks && (cJSON_Array == cj_value_blocks->type)) // nested value-block
                        {
                            cJSON *cj_value_block = NULL;
                            while (NULL != (cj_value_block = cJSON_GetArrayItem(cj_value_blocks, value_block_idx++)))
                            {
                                if (true == (block_meta_changed = ___add_new_blockmeta_by_id(cj_value_block, blockId, cj_blockmeta)))
                                {
                                    break; // changed only targeted blockID.
                                }
                            }
                        }
                        else if (cj_value_blocks && (cJSON_Object == cj_value_blocks->type)) // single value-block
                        {
                            block_meta_changed = ___add_new_blockmeta_by_id(cj_value_blocks, blockId, cj_blockmeta);
                        }
                    }
                    else
                    {
                        // TRACE_D("No further ----> Blocks!!");
                        break;
                    }
                }
            }
        }
    }

    return block_meta_changed;
}

ezlopi_error_t ezlopi_core_scene_meta_by_id(const char *sceneId_str, const char *blockId_str, cJSON *cj_new_meta)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (sceneId_str && blockId_str)
    {
        uint32_t sceneId = (uint32_t)strtoul(sceneId_str, NULL, 16);
        uint32_t blockId = (uint32_t)strtoul(blockId_str, NULL, 16);
        l_scenes_list_v2_t *curr_scene = ezlopi_scenes_get_by_id_v2(sceneId);
        if (curr_scene && cj_new_meta)
        {
            char *scene_str = ezlopi_nvs_read_str(sceneId_str);
            if (scene_str)
            {
                cJSON *cj_scene = cJSON_Parse(__FUNCTION__, scene_str); /* "params" : {...}*/
                if (cj_scene)
                {
                    bool meta_data_added = false;
                    if (NULL != blockId_str)
                    {
                        int when_block_idx = 0;
                        cJSON *cj_when_block = NULL;
                        cJSON *cj_when_blocks = cJSON_GetObjectItem(__FUNCTION__, cj_scene, "when");
                        while (NULL != (cj_when_block = cJSON_GetArrayItem(cj_when_blocks, when_block_idx++)))
                        {
                            meta_data_added |= ___add_new_blockmeta_by_id(cj_when_block, blockId, cj_new_meta);
                        }
                    }
                    else
                    {
                        if (cJSON_GetObjectItem(__FUNCTION__, cj_scene, ezlopi_meta_str))
                        {
                            cJSON_DeleteItemFromObject(__FUNCTION__, cj_scene, ezlopi_meta_str);
                        }
                        meta_data_added = (bool)cJSON_AddItemToObject(__FUNCTION__, cj_scene, ezlopi_meta_str, cJSON_Duplicate(__FUNCTION__, cj_new_meta, 1));
                    }

                    if (meta_data_added)
                    {
                        // CJSON_TRACE("new_cj_scene", cj_scene);
                        /*  DONOT use : 'ezlopi_core_scene_edit_store_updated_to_nvs' .. Here */
                        char *update_scene_str = cJSON_PrintBuffered(__FUNCTION__, cj_scene, 4096, false);
                        TRACE_D("length of 'update_scene_str': %d", strlen(update_scene_str));

                        if (update_scene_str)
                        {
                            cJSON *cj_scene_id = cJSON_GetObjectItem(__FUNCTION__, cj_scene, ezlopi__id_str);
                            if (cj_scene_id && cj_scene_id->valuestring)
                            {
                                ezlopi_nvs_delete_stored_data_by_name(cj_scene_id->valuestring);
                                ret = ezlopi_nvs_write_str(update_scene_str, strlen(update_scene_str), cj_scene_id->valuestring);

                                if (EZPI_SUCCESS == ret)
                                {
                                    TRACE_W("nvs updated successfully");
                                    /*secondly Change in ll */
                                    ezlopi_core_scene_edit_update_id(sceneId, cj_scene);
                                }
                                else
                                {
                                    TRACE_E("Error!! failed");
                                }
                            }

                            ezlopi_free(__FUNCTION__, update_scene_str);
                        }
                    }
                    cJSON_Delete(__FUNCTION__, cj_scene);
                }
                ezlopi_free(__FUNCTION__, scene_str);
            }
        }
    }
    return ret;
}
//--------------------------------------------------------------------------------------------------
//                  Functions for : listing Time-related when-blocks
//--------------------------------------------------------------------------------------------------
static char *___get_time_list_from_when_block_fields(cJSON *cj_scenes_array, l_scenes_list_v2_t *curr_scene, l_fields_v2_t *curr_field_block);
static char *__get_time_category_method_name(cJSON *cj_scenes_array, l_scenes_list_v2_t *curr_scene, l_when_block_v2_t *curr_when_block);

static void _____add_the_scene_time_method_to_arr(cJSON *cj_scenes_array, l_scenes_list_v2_t *curr_scene, char *method_name)
{
    if (cj_scenes_array && curr_scene && method_name)
    {
        cJSON *cj_new_add = cJSON_CreateObject(__FUNCTION__);
        if (cj_new_add)
        {
            char scene_id_str[32];
            snprintf(scene_id_str, sizeof(scene_id_str), "%08x", curr_scene->_id);

            cJSON_AddStringToObject(__FUNCTION__, cj_new_add, ezlopi_sceneId_str, scene_id_str);
            cJSON_AddStringToObject(__FUNCTION__, cj_new_add, "sceneName", curr_scene->name);
            cJSON_AddStringToObject(__FUNCTION__, cj_new_add, "methodName", method_name);

            char timestamp_str[64] = {0};
            EZPI_CORE_sntp_epoch_to_iso8601(timestamp_str, sizeof(timestamp_str), (time_t)(curr_scene->executed_date));
            cJSON_AddStringToObject(__FUNCTION__, cj_new_add, "executionDate", timestamp_str);

            if (!cJSON_AddItemToArray(cj_scenes_array, cj_new_add))
            {
                cJSON_Delete(__FUNCTION__, cj_new_add);
            }
            else
            {
                TRACE_D("ADDING [curr_scene->name : %s | method_name: %s]", curr_scene->name, method_name);
            }
        }
    }
}
static char *___get_time_list_from_when_block_fields(cJSON *cj_scenes_array, l_scenes_list_v2_t *curr_scene, l_fields_v2_t *curr_field_block)
{
    char *ret_str = NULL;

    if (curr_field_block)
    {
        if (curr_field_block->next)
        {
            if (NULL != (ret_str = ___get_time_list_from_when_block_fields(cj_scenes_array, curr_scene, curr_field_block)))
            {
                TRACE_D("\t\t---| Found : Field_method_name: %s", ret_str);
            }
        }

        // iterate throught value array if field_name_type  is 'block'
        if ((EZPI_STRNCMP_IF_EQUAL(curr_field_block->name, "blocks", sizeof(curr_field_block->name), 7) && (VALUE_TYPE_BLOCK == curr_field_block->field_value.e_type)) ||
            (EZPI_STRNCMP_IF_EQUAL(curr_field_block->name, "block", sizeof(curr_field_block->name), 6) && (VALUE_TYPE_BLOCK == curr_field_block->field_value.e_type)))
        {
            // this function iterates through the when-block-field-values.
            if (VALUE_TYPE_BLOCK == curr_field_block->field_value.e_type)
            {
                ret_str = __get_time_category_method_name(cj_scenes_array, curr_scene, curr_field_block->field_value.u_value.when_block);
            }
        }
    }
    return ret_str;
}
static char *__get_time_category_method_name(cJSON *cj_scenes_array, l_scenes_list_v2_t *curr_scene, l_when_block_v2_t *curr_when_block)
{
    char *ret_str = NULL;

    if (curr_when_block)
    {
        if (curr_when_block->next)
        {
            if (NULL != (ret_str = __get_time_category_method_name(cj_scenes_array, curr_scene, curr_when_block->next)))
            {
                TRACE_D("\t---| Found : %s", ret_str);
            }
        }

        // now examine if block-name is of 'logical-category'
        const char *curr_when_category_name = ezlopi_scene_get_scene_method_category_name(curr_when_block->block_options.method.name); // give corresponding 'category_name' for respective 'method_name'
        if (curr_when_category_name)
        {
            if (EZPI_STRNCMP_IF_EQUAL(curr_when_category_name, "when_category_time", strlen(curr_when_category_name), 19))
            {
                _____add_the_scene_time_method_to_arr(cj_scenes_array, curr_scene, curr_when_block->block_options.method.name);
            }
            else if ((EZPI_STRNCMP_IF_EQUAL(curr_when_category_name, "when_category_logic", strlen(curr_when_category_name), 20))        // and,or,xor
                     || (EZPI_STRNCMP_IF_EQUAL(curr_when_category_name, "when_category_function", strlen(curr_when_category_name), 23))) // function -> for/repeat/follow....
            {
                ret_str = ___get_time_list_from_when_block_fields(cj_scenes_array, curr_scene, curr_when_block->fields);
            }
        }
    }
    return ret_str;
}

int ezlopi_core_scenes_get_time_list(cJSON *cj_scenes_array)
{
    int ret = 0;
    if (cj_scenes_array)
    {
        // get the corresponding scene for ll
        l_scenes_list_v2_t *curr_scene = scenes_list_head_v2;
        while (curr_scene)
        {
            // extract the method-name and enum from ll
            __get_time_category_method_name(cj_scenes_array, curr_scene, curr_scene->when_block); // return first found time-related method_name for now [need additions]
            curr_scene = curr_scene->next;
        }
    }

    return ret;
}

//--------------------------------------------------------------------------------------------------------------------
static l_when_block_v2_t *___get_group_when_blocks(l_when_block_v2_t *curr_when_block, uint32_t group_id);

static l_when_block_v2_t *__iterate_through_fields(l_fields_v2_t *fields, uint32_t group_id)
{
    l_when_block_v2_t *ret = NULL;
    if (fields)
    {
        if ((EZPI_STRNCMP_IF_EQUAL(fields->name, "blocks", strlen(fields->name), 7) && (EZLOPI_VALUE_TYPE_BLOCKS == fields->value_type)) ||
            (EZPI_STRNCMP_IF_EQUAL(fields->name, "block", strlen(fields->name), 7) && (EZLOPI_VALUE_TYPE_BLOCK == fields->value_type)))
        {
            l_when_block_v2_t *curr_field_when_blocks = fields->field_value.u_value.when_block; // value : {when_block , ...}
            while (curr_field_when_blocks)
            {
                if (NULL != (ret = ___get_group_when_blocks(curr_field_when_blocks, group_id)))
                {
                    break;
                }
                curr_field_when_blocks = curr_field_when_blocks->next;
            }
        }
    }

    return ret;
}
static l_when_block_v2_t *___get_group_when_blocks(l_when_block_v2_t *curr_when_block, uint32_t group_id)
{
    l_when_block_v2_t *ret = NULL;
    if (curr_when_block)
    {
        // now examine if block-name is of 'logical-category'
        const char *curr_when_category_name = ezlopi_scene_get_scene_method_category_name(curr_when_block->block_options.method.name); // give corresponding 'category_name' for respective 'method_name'
        if (curr_when_category_name)
        {
            if ((EZPI_STRNCMP_IF_EQUAL(curr_when_category_name, "when_category_logic", strlen(curr_when_category_name), 20))        // and/or/not
                || (EZPI_STRNCMP_IF_EQUAL(curr_when_category_name, "when_category_function", strlen(curr_when_category_name), 23))) // function -> for/repeat/follow....
            {
                // check for --> the 'when-block' containing the 'group-id'
                if ((NULL != curr_when_block->when_grp) && (0 < strlen(curr_when_block->when_grp->grp_blockName)) && (0 < curr_when_block->when_grp->grp_id))
                {
                    TRACE_D("group_id : %08x vs [%08x]", curr_when_block->when_grp->grp_id, group_id);
                    if (curr_when_block->when_grp->grp_id == group_id)
                    {
                        ret = curr_when_block; // if this 'block-group-id' matches with 'group_id' ; Exit the loop.
                    }
                }

                if (NULL == ret) // examine further
                {
                    l_fields_v2_t *curr_field = curr_when_block->fields;
                    while (curr_field) // check for nested fields
                    {
                        if (NULL != (ret = __iterate_through_fields(curr_field, group_id)))
                        {
                            break;
                        }
                        curr_field = curr_field->next;
                    }
                }
            }
            else
            {
                // TRACE_E(" Invalid !! , Empty  AND/OR/NOT operation.");
            }
        }
    }

    return ret;
}
l_when_block_v2_t *ezlopi_core_scene_get_group_block(uint32_t scene_id, uint32_t group_id)
{
    l_when_block_v2_t *ret = NULL;

    l_scenes_list_v2_t *scene_node = ezlopi_scenes_get_by_id_v2(scene_id);
    if (scene_node)
    {
        l_when_block_v2_t *curr_when_block = scene_node->when_block;
        while (curr_when_block)
        {
            l_when_block_v2_t *found_matched_block = ___get_group_when_blocks(curr_when_block, group_id);
            if (found_matched_block) // found the when-block linked with 'group_id'
            {
                // execute the matched 'when-block'.
                f_scene_method_v2_t scene_method = ezlopi_scene_get_method_v2(found_matched_block->block_options.method.type);
                if (scene_method)
                {
                    found_matched_block->when_grp->grp_state = (bool)(scene_method(scene_node, (void *)found_matched_block));
                    ret = found_matched_block;
                }
                break;
            }
            curr_when_block = curr_when_block->next;
        }
    }
    return ret;
}
//--------------------------------------------------------------------------------------------------
/* Add for Group-Id in future*/
//--------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS