#include <cJSON.h>

#include "trace.h"

#include "ezlopi_nvs.h"
#include "ezlopi_cloud.h"
#include "ezlopi_devices.h"
#include "ezlopi_scenes_v2.h"
#include "ezlopi_cjson_macros.h"
#include "ezlopi_factory_info.h"
#include "ezlopi_scenes_methods.h"
#include "ezlopi_meshbot_service.h"
#include "ezlopi_scenes_when_methods.h"
#include "ezlopi_scenes_then_methods.h"
#include "ezlopi_scenes_status_changed.h"

static l_scenes_list_v2_t *scenes_list_head_v2 = NULL;

static const char *scenes_value_type_name[] = {
#define EZLOPI_VALUE_TYPE(type, name) name,
#include "ezlopi_scenes_value_types.h"
#undef EZLOPI_VALUE_TYPE
};

static const f_scene_method_v2_t ezlopi_scenes_methods[] = {
#define EZLOPI_SCENE(method_type, name, func) func,
#include "ezlopi_scenes_method_types.h"
#undef EZLOPI_SCENE
};

static l_when_block_v2_t *__new_when_block_populate(cJSON *cj_when_block);

static l_fields_v2_t *__new_field_populate(cJSON *cj_field);
static l_fields_v2_t *__fields_populate(cJSON *cj_fields);

static void __new_action_delay(s_action_delay_v2_t *action_delay, cJSON *cj_delay);
static void __new_block_options_populate(s_block_options_v2_t *p_block_options, cJSON *cj_block_options);
static void __new_method_populate(s_method_v2_t *p_method, cJSON *cj_method);

static l_when_block_v2_t *__new_when_block_populate(cJSON *cj_when_block);
static l_when_block_v2_t *__when_blocks_populate(cJSON *cj_when_blocks);

static l_action_block_v2_t *__new_action_block_populate(cJSON *cj_then_block, e_scenes_block_type_v2_t block_type);
static l_action_block_v2_t *__action_blocks_populate(cJSON *cj_then_blocks, e_scenes_block_type_v2_t block_type);

static l_house_modes_v2_t *__new_house_mode_populate(cJSON *cj_house_mode);
static l_house_modes_v2_t *__house_modes_populate(cJSON *cj_house_modes);

static l_user_notification_v2_t *__new_user_notification_populate(cJSON *cj_user_notification);
static l_user_notification_v2_t *__user_notifications_populate(cJSON *cj_user_notifications);

static l_scenes_list_v2_t *__new_scene_populate(cJSON *cj_scene, uint32_t scene_id);
static l_scenes_list_v2_t *__scenes_populate(cJSON *cj_scene, uint32_t scene_id);

uint32_t ezlopi_store_new_scene_v2(cJSON *cj_new_scene)
{
    uint32_t new_scene_id = 0;
    if (cj_new_scene)
    {
        new_scene_id = ezlopi_cloud_generate_scene_id();
        char tmp_buffer[32];
        snprintf(tmp_buffer, sizeof(tmp_buffer), "%08x", new_scene_id);
        cJSON_AddStringToObject(cj_new_scene, "_id", tmp_buffer);
        char *new_scnee_str = cJSON_Print(cj_new_scene);
        if (new_scnee_str)
        {
            if (ezlopi_nvs_write_str(new_scnee_str, strlen(new_scnee_str) + 1, tmp_buffer))
            {
                bool free_scene_list_str = 1;
                char *scenes_list_str = ezlopi_nvs_scene_get_v2();
                if (NULL == scenes_list_str)
                {
                    scenes_list_str = "[]";
                    free_scene_list_str = 0;
                }

                cJSON *cj_scenes_list = cJSON_Parse(scenes_list_str);
                if (cj_scenes_list)
                {
                    cJSON *cj_new_scene_id = cJSON_CreateNumber(new_scene_id);
                    if (!cJSON_AddItemToArray(cj_scenes_list, cj_new_scene_id))
                    {
                        cJSON_Delete(cj_new_scene_id);
                        ezlopi_nvs_delete_stored_script(new_scene_id);
                        new_scene_id = 0;
                    }
                    else
                    {
                        char *updated_scenes_list = cJSON_Print(cj_scenes_list);
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

uint32_t ezlopi_scenes_get_list_v2(cJSON *cj_scenes_array)
{
    int ret = 0;
    char *scenes_ids = ezlopi_nvs_scene_get_v2();
    if (scenes_ids)
    {
        cJSON *cj_scenes_ids = cJSON_Parse(scenes_ids);
        if (cj_scenes_ids)
        {
            int array_size = cJSON_GetArraySize(cj_scenes_ids);
            for (int i = 0; i < array_size; i++)
            {
                cJSON *cj_scene_id = cJSON_GetArrayItem(cj_scenes_ids, i);
                if (cj_scene_id && cj_scene_id->valuedouble)
                {
                    char tmp_buffer[32];
                    uint32_t scene_id = (uint32_t)cj_scene_id->valueint;
                    snprintf(tmp_buffer, sizeof(tmp_buffer), "%08x", scene_id);
                    char *scene_str = ezlopi_nvs_read_str(tmp_buffer);
                    if (scene_str)
                    {
                        cJSON *cj_scene = cJSON_Parse(scene_str);
                        if (cj_scene)
                        {
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
    return __scenes_populate(cj_new_scene, scene_id);
}

void ezlopi_scenes_depopulate_by_id_v2(uint32_t _id)
{
    if (1 == ezlopi_meshbot_service_stop_for_scene_id(_id))
    {
        ezlopi_scenes_delete(ezlopi_scenes_pop_by_id_v2(_id));
    }
}

void ezlopi_scenes_remove_id_from_list_v2(uint32_t _id)
{
    char *scenes_id_list_str = ezlopi_nvs_scene_get_v2();
    if (scenes_id_list_str)
    {
        cJSON *cj_scene_id_list = cJSON_Parse(scenes_id_list_str);
        if (cj_scene_id_list)
        {
            uint32_t list_len = cJSON_GetArraySize(cj_scene_id_list);

            for (int idx = list_len; idx < list_len; idx++)
            {
                cJSON *cj_id = cJSON_GetArrayItem(cj_scene_id_list, idx);
                if (cj_id)
                {
                    if (cj_id->valuedouble == _id)
                    {
                        cJSON_DeleteItemFromArray(cj_scene_id_list, idx);

                        char *updated_id_list_str = cJSON_Print(cj_scene_id_list);
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

l_scenes_list_v2_t *ezlopi_scenes_get_scenes_head_v2(void)
{
    return scenes_list_head_v2;
}

f_scene_method_v2_t ezlopi_scene_get_method_v2(e_scene_method_type_t scene_method_type)
{
    f_scene_method_v2_t method_ptr = NULL;
    if ((scene_method_type > EZLOPI_SCENE_METHOD_TYPE_NONE) && (scene_method_type < EZLOPI_SCENE_METHOD_TYPE_MAX))
    {
        method_ptr = ezlopi_scenes_methods[scene_method_type];
    }
    return method_ptr;
}

const char *ezlopi_scene_get_scene_value_type_name_v2(e_scene_value_type_v2_t value_type)
{
    const char *ret = NULL;
    if ((value_type > EZLOPI_VALUE_TYPE_NONE) && (value_type < EZLOPI_VALUE_TYPE_MAX))
    {
        ret = scenes_value_type_name[value_type];
    }
    return ret;
}

/**
 * @brief main functions to initiated scenes-nodes
 *
 */
void ezlopi_scenes_init_v2(void)
{
    char *scenes_id_list_str = ezlopi_nvs_scene_get_v2();

    if (scenes_id_list_str)
    {
        cJSON *cj_scenes_ids = cJSON_Parse(scenes_id_list_str);
        if (cj_scenes_ids)
        {
            int array_size = cJSON_GetArraySize(cj_scenes_ids);
            for (int i = 0; i < array_size; i++)
            {
                cJSON *cj_scene_id = cJSON_GetArrayItem(cj_scenes_ids, i);
                if (cj_scene_id && cj_scene_id->valuedouble)
                {
                    char tmp_buffer[32];
                    uint32_t scene_id = (uint32_t)cj_scene_id->valueint;

                    snprintf(tmp_buffer, sizeof(tmp_buffer), "%08x", scene_id);
                    ezlopi_cloud_update_scene_id((uint32_t)scene_id);
                    char *scene_str = ezlopi_nvs_read_str(tmp_buffer);
                    if (scene_str)
                    {
                        cJSON *cj_scene = cJSON_Parse(scene_str);
                        if (cj_scene)
                        {
                            __scenes_populate(cj_scene, scene_id);
                            cJSON_Delete(cj_scene);
                        }

                        free(scene_str);
                    }
                }
            }

            cJSON_Delete(cj_scenes_ids);
        }

        free(scenes_id_list_str);
    }
    else
    {
        TRACE_W("Scenes not available!");
    }

    ezlopi_scenes_print(scenes_list_head_v2);
}

static l_scenes_list_v2_t *__scenes_populate(cJSON *cj_scene, uint32_t scene_id)
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
        new_scene = malloc(sizeof(l_scenes_list_v2_t));
        if (new_scene)
        {
            uint32_t tmp_success_creating_scene = 1;

            memset(new_scene, 0, sizeof(l_scenes_list_v2_t));

            new_scene->_id = scene_id;
            new_scene->task_handle = NULL;
            new_scene->status = EZLOPI_SCENE_STATUS_STOPPED;

            CJSON_GET_VALUE_INT(cj_scene, "enabled", new_scene->enabled);
            CJSON_GET_VALUE_INT(cj_scene, "is_group", new_scene->is_group);

            {
                CJSON_GET_VALUE_STRING_BY_COPY(cj_scene, "group_id", new_scene->group_id);
                TRACE_D("group_id: %s", new_scene->group_id);
            }

            CJSON_GET_VALUE_STRING_BY_COPY(cj_scene, "name", new_scene->name);
            CJSON_GET_VALUE_STRING_BY_COPY(cj_scene, "parent_id", new_scene->parent_id);

            {
                cJSON *cj_user_notifications = cJSON_GetObjectItem(cj_scene, "user_notifications");
                if (cj_user_notifications && (cJSON_Array == cj_user_notifications->type))
                {
                    new_scene->user_notifications = __user_notifications_populate(cj_user_notifications);
                }
            }

            {
                cJSON *cj_house_modes = cJSON_GetObjectItem(cj_scene, "house_modes");
                if (cj_house_modes && (cJSON_Array == cj_house_modes->type))
                {
                    new_scene->house_modes = __house_modes_populate(cj_house_modes);
                }
            }

            {
                cJSON *cj_then_blocks = cJSON_GetObjectItem(cj_scene, "then");
                if (cj_then_blocks && (cJSON_Array == cj_then_blocks->type))
                {
                    new_scene->then_block = __action_blocks_populate(cj_then_blocks, SCENE_BLOCK_TYPE_THEN);
                }
            }

            {
                cJSON *cj_then_blocks = cJSON_GetObjectItem(cj_scene, "when");
                if (cj_then_blocks && (cJSON_Array == cj_then_blocks->type))
                {
                    new_scene->when_block = __when_blocks_populate(cj_then_blocks);
                }
            }

            {
                cJSON *cj_else_blocks = cJSON_GetObjectItem(cj_scene, "else");
                if (cj_else_blocks && (cJSON_Array == cj_else_blocks->type))
                {
                    new_scene->else_block = __action_blocks_populate(cj_else_blocks, SCENE_BLOCK_TYPE_ELSE);
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

static l_user_notification_v2_t *__user_notifications_populate(cJSON *cj_user_notifications)
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
                curr_user_notification->next = __new_user_notification_populate(cj_user_notification);
            }
            else
            {
                tmp_user_notifications_head = __new_user_notification_populate(cj_user_notification);
            }
        }
    }

    return tmp_user_notifications_head;
}

static l_user_notification_v2_t *__new_user_notification_populate(cJSON *cj_user_notification)
{
    l_user_notification_v2_t *new_user_notification = NULL;

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

static l_house_modes_v2_t *__house_modes_populate(cJSON *cj_house_modes)
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

                tmp_house_mode->next = __new_house_mode_populate(cj_house_mode);
            }
            else
            {
                tmp_house_mode_head = __new_house_mode_populate(cj_house_mode);
            }
        }
    }

    return tmp_house_mode_head;
}

static l_house_modes_v2_t *__new_house_mode_populate(cJSON *cj_house_mode)
{
    l_house_modes_v2_t *new_house_mode = NULL;

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

static l_action_block_v2_t *__action_blocks_populate(cJSON *cj_action_blocks, e_scenes_block_type_v2_t block_type)
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

                tmp_then_block->next = __new_action_block_populate(cj_action_block, block_type);
            }
            else
            {
                tmp_action_block_head = __new_action_block_populate(cj_action_block, block_type);
            }
        }
    }

    return tmp_action_block_head;
}

static l_action_block_v2_t *__new_action_block_populate(cJSON *cj_action_block, e_scenes_block_type_v2_t block_type)
{
    l_action_block_v2_t *new_then_block = malloc(sizeof(l_action_block_v2_t));
    if (new_then_block)
    {
        memset(new_then_block, 0, sizeof(l_action_block_v2_t));
        cJSON *cj_block_options = cJSON_GetObjectItem(cj_action_block, "blockOptions");
        if (cj_block_options)
        {
            __new_block_options_populate(&new_then_block->block_options, cj_block_options);
        }

        new_then_block->block_type = block_type;
        cJSON *cj_delay = cJSON_GetObjectItem(cj_action_block, "delay");
        if (cj_delay)
        {
            __new_action_delay(&new_then_block->delay, cj_delay);
        }

        cJSON *cj_fields = cJSON_GetObjectItem(cj_action_block, "fields");
        if (cj_fields)
        {
            new_then_block->fields = __fields_populate(cj_fields);
        }

        cJSON *cj__temp_id = cJSON_GetObjectItem(cj_action_block, "_tempId");
        if (cj__temp_id && cj__temp_id->valuestring)
        {
            snprintf(new_then_block->_tempId, sizeof(new_then_block->_tempId), "%s", cj__temp_id->valuestring);
        }
    }

    return new_then_block;
}

static l_when_block_v2_t *__when_blocks_populate(cJSON *cj_when_blocks)
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

                tmp_when_block->next = __new_when_block_populate(cj_when_block);
            }
            else
            {
                tmp_when_block_head = __new_when_block_populate(cj_when_block);
            }
        }
    }

    return tmp_when_block_head;
}

static l_when_block_v2_t *__new_when_block_populate(cJSON *cj_when_block)
{
    l_when_block_v2_t *new_when_block = malloc(sizeof(l_when_block_v2_t));
    if (new_when_block)
    {
        memset(new_when_block, 0, sizeof(l_when_block_v2_t));
        cJSON *cj_block_options = cJSON_GetObjectItem(cj_when_block, "blockOptions");
        if (cj_block_options)
        {
            __new_block_options_populate(&new_when_block->block_options, cj_block_options);
        }

        new_when_block->block_type = SCENE_BLOCK_TYPE_WHEN;

        cJSON *cj_fields = cJSON_GetObjectItem(cj_when_block, "fields");
        if (cj_fields)
        {
            new_when_block->fields = __fields_populate(cj_fields);
        }
    }

    return new_when_block;
}

static void __new_block_options_populate(s_block_options_v2_t *p_block_options, cJSON *cj_block_options)
{
    cJSON *cj_method = cJSON_GetObjectItem(cj_block_options, "method");
    if (cj_method)
    {
        __new_method_populate(&p_block_options->method, cj_method);
    }
}

static void __new_method_populate(s_method_v2_t *p_method, cJSON *cj_method)
{
    CJSON_GET_VALUE_STRING_BY_COPY(cj_method, "name", p_method->name);
    p_method->type = ezlopi_scenes_method_get_type_enum(p_method->name);
}

static void __new_action_delay(s_action_delay_v2_t *action_delay, cJSON *cj_delay)
{
    if (action_delay && cj_delay)
    {
        CJSON_GET_VALUE_DOUBLE(cj_delay, "days", action_delay->days);
        CJSON_GET_VALUE_DOUBLE(cj_delay, "hours", action_delay->hours);
        CJSON_GET_VALUE_DOUBLE(cj_delay, "minutes", action_delay->minutes);
        CJSON_GET_VALUE_DOUBLE(cj_delay, "seconds", action_delay->seconds);
    }
}

static l_fields_v2_t *__fields_populate(cJSON *cj_fields)
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

                tmp_flield->next = __new_field_populate(cj_field);
            }
            else
            {
                tmp_fields_head = __new_field_populate(cj_field);
            }
        }
    }

    return tmp_fields_head;
}

static void __fields_get_value(l_fields_v2_t *field, cJSON *cj_value)
{
    if (field && cj_value)
    {
        switch (cj_value->type)
        {
        case cJSON_Number:
        {
            field->value.value_double = cj_value->valuedouble;
            TRACE_B("value: %f", field->value.value_double);
            break;
        }
        case cJSON_String:
        {
            uint32_t value_len = strlen(cj_value->valuestring) + 1;
            field->value.value_string = malloc(value_len);
            if (field->value.value_string)
            {
                snprintf(field->value.value_string, value_len, "%s", cj_value->valuestring);
                TRACE_B("value: %s", field->value.value_string);
            }
            else
            {
                TRACE_E("Malloc failed!");
            }
            break;
        }
        case cJSON_True:
        {
            field->value.value_bool = true;
            TRACE_B("value: true");
            break;
        }
        case cJSON_False:
        {
            field->value.value_bool = false;
            TRACE_B("value: false");
            break;
        }
        case cJSON_Array:
        {
            cJSON *cj_block = NULL;
            int block_idx = 0;

            while (NULL != (cj_block = cJSON_GetArrayItem(cj_value, block_idx++)))
            {
                if (field->value.when_block)
                {
                    l_when_block_v2_t *curr_when_block = field->value.when_block;
                    while (curr_when_block->next)
                    {
                        curr_when_block = curr_when_block->next;
                    }
                    curr_when_block->next = __new_when_block_populate(cj_block);
                }
                else
                {
                    field->value.when_block = __new_when_block_populate(cj_block);
                }
            }
            break;
        }
        case cJSON_Object:
        {
            if (NULL != field->value.value_json)
            {
                char *c = cJSON_Print(field->value.value_json);
                if (c)
                {
                    TRACE_E("Prev value: %s", c);
                    cJSON_free(c);
                }
                cJSON_Delete(field->value.value_json);
                field->value.value_json = NULL;
            }

            field->value.value_json = cJSON_Duplicate(cj_value, 1);
            if (field->value.value_json)
            {
                TRACE_I("Duplicating and copyig into :- CJSON object '.value_json'");
                //----------------------------------------------------------------

                // cJSON *header = cj_value->child;
                // while (header)
                // {
                //     cJSON_AddStringToObject(field->value.value_json, header->string, header->valuestring);
                //     header = header->next;
                // }

                //----------------------------------------------------------------

                char *c = cJSON_Print(field->value.value_json);
                if (c)
                {
                    TRACE_W("NEW header_values: \n%s", c);
                    cJSON_free(c);
                }
            }
            break;
        }
        default:
        {
            TRACE_E("cj_value type: %d", cj_value->type);
            break;
        }
        }
    }
}

static l_fields_v2_t *__new_field_populate(cJSON *cj_field)
{
    l_fields_v2_t *field = NULL;
    if (cj_field)
    {
        field = malloc(sizeof(l_fields_v2_t));
        if (field)
        {
            memset(field, 0, sizeof(l_fields_v2_t));
            CJSON_GET_VALUE_STRING_BY_COPY(cj_field, "name", field->name);

            field->value_type = ezlopi_scenes_get_expressions_value_type(cJSON_GetObjectItem(cj_field, "type"));
            // cJSON *cj_value = cJSON_GetObjectItem(cj_field, "value");

            __fields_get_value(field, cJSON_GetObjectItem(cj_field, "value"));

#if 0
            if (cj_value)
            {
                switch (cj_value->type)
                {
                case cJSON_Number:
                {
                    field->value.value_double = cj_value->valuedouble;
                    TRACE_B("value: %f", field->value.value_double);
                    break;
                }
                case cJSON_String:
                {
                    uint32_t value_len = strlen(cj_value->valuestring) + 1;
                    field->value.value_string = malloc(value_len);
                    if (field->value.value_string)
                    {
                        snprintf(field->value.value_string, value_len, "%s", cj_value->valuestring);
                        TRACE_B("value: %s", field->value.value_string);
                    }
                    else
                    {
                        TRACE_E("Malloc failed!");
                    }
                    break;
                }
                case cJSON_True:
                {
                    field->value.value_bool = true;
                    TRACE_B("value: true");
                    break;
                }
                case cJSON_False:
                {
                    field->value.value_bool = false;
                    TRACE_B("value: false");
                    break;
                }
                case cJSON_Array:
                {
                    cJSON *cj_block = NULL;
                    int block_idx = 0;

                    while (NULL != (cj_block = cJSON_GetArrayItem(cj_value, block_idx++)))
                    {
                        if (field->value.when_block)
                        {
                            l_when_block_v2_t *curr_when_block = field->value.when_block;
                            while (curr_when_block->next)
                            {
                                curr_when_block = curr_when_block->next;
                            }
                            curr_when_block->next = __new_when_block_populate(cj_block);
                        }
                        else
                        {
                            field->value.when_block = __new_when_block_populate(cj_block);
                        }
                    }
                    break;
                }
                default:
                {
                    TRACE_E("cj_value type: %d", cj_value->type);
                    break;
                }
                }
            }
#endif
        }
    }

    return field;
}

e_scene_value_type_v2_t ezlopi_scenes_get_value_type(cJSON *cj_field)
{
    e_scene_value_type_v2_t ret = EZLOPI_VALUE_TYPE_NONE;
    if (cj_field)
    {
        char *type_str = NULL;
        CJSON_GET_VALUE_STRING(cj_field, "type", type_str);
        if (type_str)
        {
            uint32_t type_str_len = strlen(type_str);
            for (int i = EZLOPI_VALUE_TYPE_NONE; i < EZLOPI_VALUE_TYPE_MAX; i++)
            {
                uint32_t check_str_len = strlen(scenes_value_type_name[i]);
                uint32_t check_len = (check_str_len < type_str_len) ? type_str_len : check_str_len;
                if (0 == strncmp(scenes_value_type_name[i], type_str, check_len))
                {
                    ret = i;
                    break;
                }
            }
        }
    }
    return ret;
}

e_scene_value_type_v2_t ezlopi_scenes_get_expressions_value_type(cJSON *cj_value_type)
{
    e_scene_value_type_v2_t ret = EZLOPI_VALUE_TYPE_NONE;
    if (cj_value_type && cj_value_type->valuestring)
    {
        uint32_t type_str_len = strlen(cj_value_type->valuestring);
        for (int i = EZLOPI_VALUE_TYPE_NONE; i < EZLOPI_VALUE_TYPE_MAX; i++)
        {
            uint32_t check_str_len = strlen(scenes_value_type_name[i]);
            uint32_t check_len = (check_str_len < type_str_len) ? type_str_len : check_str_len;
            if (0 == strncmp(scenes_value_type_name[i], cj_value_type->valuestring, check_len))
            {
                ret = i;
                break;
            }
        }
    }
    return ret;
}