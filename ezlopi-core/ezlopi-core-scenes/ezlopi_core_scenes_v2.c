
#include "ezlopi_util_trace.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_scenes_v2.h"
#include "ezlopi_core_scenes_edit.h"
#include "ezlopi_core_scenes_cjson.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_scenes_value.h"
#include "ezlopi_core_scenes_print.h"
#include "ezlopi_core_scenes_when_methods.h"
#include "ezlopi_core_scenes_then_methods.h"
#include "ezlopi_core_scenes_status_changed.h"

#include "ezlopi_cloud_constants.h"
#include "ezlopi_service_meshbot.h"

static l_scenes_list_v2_t* scenes_list_head_v2 = NULL;

static const f_scene_method_v2_t ezlopi_core_scenes_methods[] = {
#define EZLOPI_SCENE(method_type, name, func) func,
#include "ezlopi_core_scenes_method_types.h"
#undef EZLOPI_SCENE
};

static l_fields_v2_t* ______new_field_populate(cJSON* cj_field);
static l_fields_v2_t* _____fields_populate(cJSON* cj_fields);

static void _____new_action_delay(s_action_delay_v2_t* action_delay, cJSON* cj_delay);
static void _____new_block_options_populate(s_block_options_v2_t* p_block_options, cJSON* cj_block_options);
static void __new_method_populate(s_method_v2_t* p_method, cJSON* cj_method);

static l_when_block_v2_t* ____new_when_block_populate(cJSON* cj_when_block);
static l_when_block_v2_t* ___when_blocks_populate(cJSON* cj_when_blocks);

static l_action_block_v2_t* ____new_action_block_populate(cJSON* cj_then_block, e_scenes_block_type_v2_t block_type);
static l_action_block_v2_t* ___action_blocks_populate(cJSON* cj_then_blocks, e_scenes_block_type_v2_t block_type);

static l_house_modes_v2_t* ____new_house_mode_populate(cJSON* cj_house_mode);
static l_house_modes_v2_t* ___house_modes_populate(cJSON* cj_house_modes);

static l_user_notification_v2_t* ____new_user_notification_populate(cJSON* cj_user_notification);
static l_user_notification_v2_t* ___user_notifications_populate(cJSON* cj_user_notifications);

static l_scenes_list_v2_t* __new_scene_populate(cJSON* cj_scene, uint32_t scene_id);
static l_scenes_list_v2_t* _scenes_populate(cJSON* cj_scene, uint32_t scene_id);

int ezlopi_scene_edit_by_id(uint32_t scene_id, cJSON* cj_scene)
{
    int ret = 0;

    if (1 == ezlopi_core_scene_edit_update_id(scene_id, cj_scene))
    {
        if (1 == ezlopi_core_scene_edit_store_updated_to_nvs(cj_scene))
        {
            ret = 1;
        }
    }

    return ret;
}

void ezlopi_scene_add_users_in_notifications(l_scenes_list_v2_t* scene_node, cJSON* cj_user)
{
    if (scene_node && cj_user && cj_user->valuestring)
    {
        if (scene_node->user_notifications)
        {
            l_user_notification_v2_t* user_node = scene_node->user_notifications;
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

uint32_t ezlopi_store_new_scene_v2(cJSON* cj_new_scene)
{
    uint32_t new_scene_id = 0;
    if (cj_new_scene)
    {
        new_scene_id = ezlopi_cloud_generate_scene_id();
        char tmp_buffer[32];
        snprintf(tmp_buffer, sizeof(tmp_buffer), "%08x", new_scene_id);
        cJSON_AddStringToObject(cj_new_scene, ezlopi__id_str, tmp_buffer);

        char* new_scnee_str = cJSON_PrintBuffered(cj_new_scene, 4096, false);
        TRACE_D("length of 'new_scnee_str': %d", strlen(new_scnee_str));

        if (new_scnee_str)
        {
            if (ezlopi_nvs_write_str(new_scnee_str, strlen(new_scnee_str) + 1, tmp_buffer))
            {
                bool free_scene_list_str = 1;
                char* scenes_list_str = ezlopi_nvs_scene_get_v2();
                if (NULL == scenes_list_str)
                {
                    scenes_list_str = "[]";
                    free_scene_list_str = 0;
                }

                cJSON* cj_scenes_list = cJSON_Parse(scenes_list_str);
                if (cj_scenes_list)
                {
                    cJSON* cj_new_scene_id = cJSON_CreateNumber(new_scene_id);
                    if (!cJSON_AddItemToArray(cj_scenes_list, cj_new_scene_id))
                    {
                        cJSON_Delete(cj_new_scene_id);
                        ezlopi_nvs_delete_stored_data_by_id(new_scene_id);
                        new_scene_id = 0;
                    }
                    else
                    {
                        char* updated_scenes_list = cJSON_PrintBuffered(cj_scenes_list, 1024, false);
                        TRACE_D("length of 'updated_scenes_list': %d", strlen(updated_scenes_list));

                        if (updated_scenes_list)
                        {
                            TRACE_D("updated_scenes_list: %s", updated_scenes_list);
                            if (ezlopi_nvs_scene_set_v2(updated_scenes_list))
                            {
                                TRACE_D("Scenes list updated.");
                            }
                            else
                            {
                                TRACE_E("Scenes list update failed!");
                            }

                            free(updated_scenes_list);
                        }
                    }

                    cJSON_Delete(cj_scenes_list);
                }

                if (free_scene_list_str)
                {
                    free(scenes_list_str);
                }
            }
            else
            {
                new_scene_id = 0;
            }

            free(new_scnee_str);
        }
        else
        {
            new_scene_id = 0;
        }
    }

    return new_scene_id;
}

uint32_t ezlopi_scenes_get_list_v2(cJSON* cj_scenes_array)
{
    int ret = 0;
    char* scenes_ids = ezlopi_nvs_scene_get_v2();
    if (scenes_ids)
    {
        cJSON* cj_scenes_ids = cJSON_Parse(scenes_ids);
        if (cj_scenes_ids)
        {
            int array_size = cJSON_GetArraySize(cj_scenes_ids);
            for (int i = 0; i < array_size; i++)
            {
                cJSON* cj_scene_id = cJSON_GetArrayItem(cj_scenes_ids, i);
                if (cj_scene_id && cj_scene_id->valuedouble)
                {
                    char tmp_buffer[32];
                    uint32_t scene_id = (uint32_t)cj_scene_id->valuedouble;
                    snprintf(tmp_buffer, sizeof(tmp_buffer), "%08x", scene_id);
                    char* scene_str = ezlopi_nvs_read_str(tmp_buffer);
                    if (scene_str)
                    {
                        cJSON* cj_scene = cJSON_Parse(scene_str);
                        if (cj_scene)
                        {
                            char scene_id_str[32];
                            snprintf(scene_id_str, sizeof(scene_id_str), "%08x", (uint32_t)cj_scene_id->valuedouble);
                            cJSON_AddStringToObject(cj_scene, ezlopi__id_str, scene_id_str);
                            if (!cJSON_AddItemToArray(cj_scenes_array, cj_scene))
                            {
                                cJSON_Delete(cj_scene);
                            }
                            else
                            {
                                ret += 1;
                            }
                        }

                        free(scene_str);
                    }
                }
            }

            cJSON_Delete(cj_scenes_ids);
        }

        free(scenes_ids);
    }

    return ret;
}

l_scenes_list_v2_t* ezlopi_scenes_get_by_id_v2(uint32_t _id)
{
    l_scenes_list_v2_t* ret_scene = NULL;
    l_scenes_list_v2_t* scenes_list = scenes_list_head_v2;
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

l_scenes_list_v2_t* ezlopi_scenes_pop_by_id_v2(uint32_t _id)
{
    l_scenes_list_v2_t* ret_scene = NULL;

    if (_id == scenes_list_head_v2->_id)
    {
        ret_scene = scenes_list_head_v2;
        scenes_list_head_v2 = scenes_list_head_v2->next;
        ret_scene->next = NULL;
    }
    else
    {
        l_scenes_list_v2_t* scenes_list = scenes_list_head_v2;
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

l_scenes_list_v2_t* ezlopi_scenes_new_scene_populate(cJSON* cj_new_scene, uint32_t scene_id)
{
    return _scenes_populate(cj_new_scene, scene_id);
}

void ezlopi_scenes_depopulate_by_id_v2(uint32_t _id)
{
    if (1 == ezlopi_meshbot_service_stop_for_scene_id(_id))
    {
        ezlopi_scenes_delete(ezlopi_scenes_pop_by_id_v2(_id));
    }
}

int ezlopi_scenes_enable_disable_id_from_list_v2(uint32_t _id, bool enabled_flag)
{
    int ret = 0;
    char* scenes_id_list_str = ezlopi_nvs_scene_get_v2();
    if (scenes_id_list_str)
    {
        cJSON* cj_scene_id_list = cJSON_Parse(scenes_id_list_str);
        if (cj_scene_id_list)
        {
            uint32_t list_len = cJSON_GetArraySize(cj_scene_id_list);

            for (int idx = 0; idx < list_len; idx++)
            {
                cJSON* cj_scene_id = cJSON_GetArrayItem(cj_scene_id_list, idx);
                if (cj_scene_id && (cj_scene_id->type == cJSON_Number))
                {
                    if (cj_scene_id->valuedouble == _id)
                    {
                        char tmp_buffer[32]; // store the scene name here
                        uint32_t scene_id = (uint32_t)(cj_scene_id->valuedouble);
                        snprintf(tmp_buffer, sizeof(tmp_buffer), "%08x", scene_id);

                        char* scene_str = ezlopi_nvs_read_str(tmp_buffer);
                        if (scene_str)
                        {
                            // converting string to cJSON format
                            cJSON* cj_scene = cJSON_Parse(scene_str);
                            if (cj_scene)
                            {
                                cJSON* enable_item = cJSON_GetObjectItem(cj_scene, ezlopi_enabled_str);
                                TRACE_S("prev_enable => [%s]", (enable_item->type == cJSON_True) ? "True" : "False");
                                if ((enable_item && cJSON_IsBool(enable_item)) && (enable_item->type != ((enabled_flag) ? cJSON_True : cJSON_False)))
                                {
                                    enable_item->type = (enabled_flag) ? cJSON_True : cJSON_False;

                                    l_scenes_list_v2_t* curr_scene_head = scenes_list_head_v2;
                                    while (curr_scene_head)
                                    {
                                        if (curr_scene_head->_id == _id)
                                        {
                                            curr_scene_head->enabled = enabled_flag;
                                            break;
                                        }

                                        curr_scene_head = curr_scene_head->next;
                                    }

                                    CJSON_TRACE("cj_scene----> 2. updated", cj_scene);

                                    if (1 == ezlopi_core_scene_edit_store_updated_to_nvs(cj_scene))
                                    {
                                        ret = 1;
                                        TRACE_W("nvs enabled successfull");
                                    }
                                    else
                                    {
                                        TRACE_E("Error!! failed");
                                    }
                                }
                                else
                                {
                                    TRACE_W("Scene-Already [%s] or, invalid option ", (true == enabled_flag) ? "enabled" : "disabled");
                                }
                                cJSON_Delete(cj_scene);
                            }
                            // free the scene_name
                            free(scene_str);
                        }
                        break;
                    }
                }
            }
            cJSON_Delete(cj_scene_id_list);
        }
        free(scenes_id_list_str);
    }
    return ret;
}

void ezlopi_scenes_remove_id_from_list_v2(uint32_t _id)
{
    char* scenes_id_list_str = ezlopi_nvs_scene_get_v2();
    if (scenes_id_list_str)
    {
        cJSON* cj_scene_id_list = cJSON_Parse(scenes_id_list_str);
        if (cj_scene_id_list)
        {
            uint32_t list_len = cJSON_GetArraySize(cj_scene_id_list);

            for (int idx = list_len; idx < list_len; idx++)
            {
                cJSON* cj_id = cJSON_GetArrayItem(cj_scene_id_list, idx);
                if (cj_id)
                {
                    if (cj_id->valuedouble == _id)
                    {
                        cJSON_DeleteItemFromArray(cj_scene_id_list, idx);

                        char* updated_id_list_str = cJSON_PrintBuffered(cj_scene_id_list, 1024, false);
                        TRACE_D("length of 'updated_id_list_str': %d", strlen(updated_id_list_str));

                        if (updated_id_list_str)
                        {
                            ezlopi_nvs_scene_set_v2(updated_id_list_str);
                            free(updated_id_list_str);
                        }
                    }
                }
            }

            cJSON_Delete(cj_scene_id_list);
        }
        free(scenes_id_list_str);
    }
}

l_scenes_list_v2_t* ezlopi_scenes_get_scenes_head_v2(void)
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

/**
 * @brief main functions to initiated scenes-nodes
 *
 */
void ezlopi_scenes_init_v2(void)
{
    char* scenes_id_list_str = ezlopi_nvs_scene_get_v2();

    if (scenes_id_list_str)
    {
        cJSON* cj_scenes_ids = cJSON_Parse(scenes_id_list_str);
        if (cj_scenes_ids)
        {
            int array_size = cJSON_GetArraySize(cj_scenes_ids);
            for (int i = 0; i < array_size; i++)
            {
                cJSON* cj_scene_id = cJSON_GetArrayItem(cj_scenes_ids, i);
                if (cj_scene_id && cj_scene_id->valuedouble)
                {
                    char tmp_buffer[32];
                    uint32_t scene_id = (uint32_t)cj_scene_id->valuedouble;

                    snprintf(tmp_buffer, sizeof(tmp_buffer), "%08x", scene_id);
                    ezlopi_cloud_update_scene_id((uint32_t)scene_id);
                    char* scene_str = ezlopi_nvs_read_str(tmp_buffer);
                    if (scene_str)
                    {
                        cJSON* cj_scene = cJSON_Parse(scene_str);
                        if (cj_scene)
                        {
                            l_scenes_list_v2_t* new_scene = _scenes_populate(cj_scene, scene_id);
                            cJSON_Delete(cj_scene);

                            if (new_scene)
                            {
                                cJSON* cj_new_scene = ezlopi_scenes_create_cjson_scene(new_scene);
                                // CJSON_TRACE("new_scene", cj_new_scene);
                                cJSON_Delete(cj_new_scene);
                            }
                        }

                        free(scene_str);
                    }
                }

            }

            cJSON_Delete(cj_scenes_ids);
        }

        free(scenes_id_list_str);
    }

    ezlopi_scenes_print(scenes_list_head_v2);
}

static l_scenes_list_v2_t* _scenes_populate(cJSON* cj_scene, uint32_t scene_id)
{
    l_scenes_list_v2_t* new_scene_node = NULL;
    if (scenes_list_head_v2)
    {
        l_scenes_list_v2_t* curr_scene = scenes_list_head_v2;
        while (curr_scene->next)
        {
            curr_scene = curr_scene->next;
        }

        curr_scene->next = __new_scene_populate(cj_scene, scene_id);
        new_scene_node = curr_scene->next;
    }
    else
    {
        CJSON_TRACE("new-scene", cj_scene);
        scenes_list_head_v2 = __new_scene_populate(cj_scene, scene_id);
        new_scene_node = scenes_list_head_v2;
    }

    return new_scene_node;
}

static l_scenes_list_v2_t* __new_scene_populate(cJSON* cj_scene, uint32_t scene_id)
{
    l_scenes_list_v2_t* new_scene = NULL;
    if (cj_scene)
    {
        new_scene = malloc(sizeof(l_scenes_list_v2_t));
        if (new_scene)
        {
            uint32_t tmp_success_creating_scene = 1;

            memset(new_scene, 0, sizeof(l_scenes_list_v2_t));

            new_scene->_id = scene_id;
            new_scene->task_handle = NULL;
            new_scene->status = EZLOPI_SCENE_STATUS_STOPPED;

            CJSON_GET_VALUE_DOUBLE(cj_scene, ezlopi_enabled_str, new_scene->enabled);
            CJSON_GET_VALUE_DOUBLE(cj_scene, ezlopi_is_group_str, new_scene->is_group);

            {
                CJSON_GET_VALUE_STRING_BY_COPY(cj_scene, ezlopi_group_id_str, new_scene->group_id);
                TRACE_D("group_id: %s", new_scene->group_id);
            }

            CJSON_GET_VALUE_STRING_BY_COPY(cj_scene, ezlopi_name_str, new_scene->name);
            CJSON_GET_VALUE_STRING_BY_COPY(cj_scene, ezlopi_parent_id_str, new_scene->parent_id);

            {
                cJSON* cj_user_notifications = cJSON_GetObjectItem(cj_scene, ezlopi_user_notifications_str);
                if (cj_user_notifications && (cJSON_Array == cj_user_notifications->type))
                {
                    new_scene->user_notifications = ___user_notifications_populate(cj_user_notifications);
                }
            }

            {
                cJSON* cj_house_modes = cJSON_GetObjectItem(cj_scene, ezlopi_house_modes_str);
                if (cj_house_modes && (cJSON_Array == cj_house_modes->type))
                {
                    new_scene->house_modes = ___house_modes_populate(cj_house_modes);
                }
            }

            {
                cJSON* cj_then_blocks = cJSON_GetObjectItem(cj_scene, ezlopi_then_str);
                if (cj_then_blocks && (cJSON_Array == cj_then_blocks->type))
                {
                    new_scene->then_block = ___action_blocks_populate(cj_then_blocks, SCENE_BLOCK_TYPE_THEN);
                }
            }

            {
                cJSON* cj_then_blocks = cJSON_GetObjectItem(cj_scene, ezlopi_when_str);
                if (cj_then_blocks && (cJSON_Array == cj_then_blocks->type))
                {
                    new_scene->when_block = ___when_blocks_populate(cj_then_blocks);
                }
            }

            {
                cJSON* cj_else_blocks = cJSON_GetObjectItem(cj_scene, ezlopi_else_str);
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

static l_user_notification_v2_t* ___user_notifications_populate(cJSON* cj_user_notifications)
{
    l_user_notification_v2_t* tmp_user_notifications_head = NULL;
    if (cj_user_notifications && (cJSON_Array == cj_user_notifications->type))
    {
        cJSON* cj_user_notification = NULL;
        int user_notifications_idx = 0;

        while (NULL != (cj_user_notification = cJSON_GetArrayItem(cj_user_notifications, user_notifications_idx++)))
        {
            if (tmp_user_notifications_head)
            {
                l_user_notification_v2_t* curr_user_notification = tmp_user_notifications_head;
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

static l_user_notification_v2_t* ____new_user_notification_populate(cJSON* cj_user_notification)
{
    l_user_notification_v2_t* new_user_notification = NULL;

    if (cj_user_notification && cj_user_notification->valuestring)
    {
        new_user_notification = malloc(sizeof(l_user_notification_v2_t));
        if (new_user_notification)
        {
            memset(new_user_notification, 0, sizeof(l_user_notification_v2_t));
            snprintf(new_user_notification->user_id, sizeof(new_user_notification->user_id), "%s", cj_user_notification->valuestring);
            new_user_notification->next = NULL;
        }
    }

    return new_user_notification;
}

static l_house_modes_v2_t* ___house_modes_populate(cJSON* cj_house_modes)
{
    l_house_modes_v2_t* tmp_house_mode_head = NULL;
    if (cj_house_modes)
    {
        int house_mode_idx = 0;
        cJSON* cj_house_mode = NULL;

        while (NULL != (cj_house_mode = cJSON_GetArrayItem(cj_house_modes, house_mode_idx++)))
        {
            if (tmp_house_mode_head)
            {
                l_house_modes_v2_t* tmp_house_mode = tmp_house_mode_head;
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

static l_house_modes_v2_t* ____new_house_mode_populate(cJSON* cj_house_mode)
{
    l_house_modes_v2_t* new_house_mode = NULL;

    if (cj_house_mode && cj_house_mode->valuestring)
    {
        new_house_mode = malloc(sizeof(l_house_modes_v2_t));
        if (new_house_mode)
        {
            memset(new_house_mode, 0, sizeof(l_house_modes_v2_t));
            strncpy(new_house_mode->house_mode, cj_house_mode->valuestring, sizeof(new_house_mode->house_mode));
            new_house_mode->next = NULL;
        }
    }

    return new_house_mode;
}

static l_action_block_v2_t* ___action_blocks_populate(cJSON* cj_action_blocks, e_scenes_block_type_v2_t block_type)
{
    l_action_block_v2_t* tmp_action_block_head = NULL;
    if (cj_action_blocks)
    {
        int then_block_idx = 0;
        cJSON* cj_action_block = NULL;

        while (NULL != (cj_action_block = cJSON_GetArrayItem(cj_action_blocks, then_block_idx++)))
        {
            if (tmp_action_block_head)
            {
                l_action_block_v2_t* tmp_then_block = tmp_action_block_head;
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

static l_action_block_v2_t* ____new_action_block_populate(cJSON* cj_action_block, e_scenes_block_type_v2_t block_type)
{
    l_action_block_v2_t* new_then_block = malloc(sizeof(l_action_block_v2_t));
    if (new_then_block)
    {
        memset(new_then_block, 0, sizeof(l_action_block_v2_t));
        cJSON* cj_block_options = cJSON_GetObjectItem(cj_action_block, ezlopi_blockOptions_str);
        if (cj_block_options)
        {
            _____new_block_options_populate(&new_then_block->block_options, cj_block_options);
        }

        new_then_block->block_type = block_type;
        cJSON* cj_delay = cJSON_GetObjectItem(cj_action_block, ezlopi_delay_str);
        if (cj_delay)
        {
            _____new_action_delay(&new_then_block->delay, cj_delay);
        }

        cJSON* cj_fields = cJSON_GetObjectItem(cj_action_block, ezlopi_fields_str);
        if (cj_fields)
        {
            new_then_block->fields = _____fields_populate(cj_fields);
        }

        cJSON* cj__temp_id = cJSON_GetObjectItem(cj_action_block, "_tempId");
        if (cj__temp_id && cj__temp_id->valuestring)
        {
            snprintf(new_then_block->_tempId, sizeof(new_then_block->_tempId), "%s", cj__temp_id->valuestring);
        }
    }

    return new_then_block;
}

static l_when_block_v2_t* ___when_blocks_populate(cJSON* cj_when_blocks)
{
    l_when_block_v2_t* tmp_when_block_head = NULL;
    if (cj_when_blocks)
    {
        int when_block_idx = 0;
        cJSON* cj_when_block = NULL;

        while (NULL != (cj_when_block = cJSON_GetArrayItem(cj_when_blocks, when_block_idx++)))
        {
            if (tmp_when_block_head)
            {
                l_when_block_v2_t* tmp_when_block = tmp_when_block_head;
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

static l_when_block_v2_t* ____new_when_block_populate(cJSON* cj_when_block)
{
    l_when_block_v2_t* new_when_block = malloc(sizeof(l_when_block_v2_t));
    if (new_when_block)
    {
        memset(new_when_block, 0, sizeof(l_when_block_v2_t));
        cJSON* cj_block_options = cJSON_GetObjectItem(cj_when_block, ezlopi_blockOptions_str);
        if (cj_block_options)
        {
            _____new_block_options_populate(&new_when_block->block_options, cj_block_options);
        }

        new_when_block->block_type = SCENE_BLOCK_TYPE_WHEN;

        cJSON* cj_fields = cJSON_GetObjectItem(cj_when_block, ezlopi_fields_str);
        if (cj_fields)
        {
            new_when_block->fields = _____fields_populate(cj_fields);
        }
    }

    return new_when_block;
}

static void _____new_block_options_populate(s_block_options_v2_t* p_block_options, cJSON* cj_block_options)
{
    cJSON* cj_method = cJSON_GetObjectItem(cj_block_options, ezlopi_method_str);
    if (cj_method)
    {
        __new_method_populate(&p_block_options->method, cj_method);
    }
}

static void __new_method_populate(s_method_v2_t* p_method, cJSON* cj_method)
{
    CJSON_GET_VALUE_STRING_BY_COPY(cj_method, ezlopi_name_str, p_method->name);
    p_method->type = ezlopi_scenes_method_get_type_enum(p_method->name);
}

static void _____new_action_delay(s_action_delay_v2_t* action_delay, cJSON* cj_delay)
{
    if (action_delay && cj_delay)
    {
        CJSON_GET_VALUE_DOUBLE(cj_delay, "days", action_delay->days);
        CJSON_GET_VALUE_DOUBLE(cj_delay, "hours", action_delay->hours);
        CJSON_GET_VALUE_DOUBLE(cj_delay, "minutes", action_delay->minutes);
        CJSON_GET_VALUE_DOUBLE(cj_delay, "seconds", action_delay->seconds);
    }
}

static l_fields_v2_t* _____fields_populate(cJSON* cj_fields)
{
    l_fields_v2_t* tmp_fields_head = NULL;
    if (cj_fields)
    {
        int fields_idx = 0;
        cJSON* cj_field = NULL;
        while (NULL != (cj_field = cJSON_GetArrayItem(cj_fields, fields_idx++)))
        {
            if (tmp_fields_head)
            {
                l_fields_v2_t* tmp_flield = tmp_fields_head;
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

static void _______fields_get_value(l_fields_v2_t* field, cJSON* cj_value)
{
    if (field && cj_value)
    {
        CJSON_TRACE("cj_value", cj_value);
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
            if (EZLOPI_VALUE_TYPE_HOUSE_MODE_ID == field->value_type)
            {
                field->field_value.e_type = VALUE_TYPE_NUMBER;
                field->field_value.u_value.value_double = strtoul(cj_value->valuestring, NULL, 16);
            }
            else
            {
                field->field_value.e_type = VALUE_TYPE_STRING;
                uint32_t value_len = strlen(cj_value->valuestring) + 1;
                field->field_value.u_value.value_string = malloc(value_len);
                if (field->field_value.u_value.value_string)
                {
                    snprintf(field->field_value.u_value.value_string, value_len, "%s", cj_value->valuestring);
                    TRACE_I("value: %s", field->field_value.u_value.value_string);
                }
                else
                {
                    TRACE_E("Malloc failed!");
                }
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
            if (EZLOPI_VALUE_TYPE_BLOCK == field->value_type)
            {
                field->field_value.e_type = VALUE_TYPE_BLOCK;
                CJSON_TRACE("cj_single_block", cj_value);
                if (field->field_value.u_value.when_block)
                {
                    l_when_block_v2_t* curr_when_block = field->field_value.u_value.when_block;
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
                field->field_value.u_value.cj_value = cJSON_Duplicate(cj_value, cJSON_True);
                CJSON_TRACE("value", field->field_value.u_value.cj_value);
            }
            break;
        }
        case cJSON_Array:
        {
            int block_idx = 0;
            cJSON* cj_block = NULL;
            CJSON_TRACE("value", cj_value);

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
                field->field_value.u_value.cj_value = cJSON_Duplicate(cj_value, cJSON_True);
                break;
            }
            case EZLOPI_VALUE_TYPE_BLOCKS:
            {
                field->field_value.e_type = VALUE_TYPE_BLOCK;
                while (NULL != (cj_block = cJSON_GetArrayItem(cj_value, block_idx++)))
                {
                    CJSON_TRACE("cj_block", cj_block);

                    if (field->field_value.u_value.when_block)
                    {
                        l_when_block_v2_t* curr_when_block = field->field_value.u_value.when_block;
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

static l_fields_v2_t* ______new_field_populate(cJSON* cj_field)
{
    l_fields_v2_t* field = NULL;
    if (cj_field)
    {
        field = malloc(sizeof(l_fields_v2_t));
        if (field)
        {
            memset(field, 0, sizeof(l_fields_v2_t));

            // field->field_value.u_value.when_block

            CJSON_GET_VALUE_STRING_BY_COPY(cj_field, ezlopi_name_str, field->name);
            field->value_type = ezlopi_core_scenes_value_get_type(cj_field, ezlopi_type_str);
            _______fields_get_value(field, cJSON_GetObjectItem(cj_field, ezlopi_value_str));
        }
    }

    return field;
}
