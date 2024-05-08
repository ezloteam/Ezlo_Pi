#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_scenes_value.h"
#include "ezlopi_core_scenes_populate.h"

#include "ezlopi_cloud_constants.h"

void ezlopi_scenes_populate_scene(l_scenes_list_v2_t* new_scene, cJSON* cj_scene, uint32_t scene_id)
{
    if (new_scene && cj_scene)
    {
        new_scene->_id = scene_id;
        new_scene->task_handle = NULL;
        new_scene->status = EZLOPI_SCENE_STATUS_STOPPED;

        CJSON_GET_VALUE_BOOL(cj_scene, ezlopi_enabled_str, new_scene->enabled);
        CJSON_GET_VALUE_DOUBLE(cj_scene, ezlopi_is_group_str, new_scene->is_group);

        CJSON_GET_VALUE_STRING_BY_COPY(cj_scene, ezlopi_group_id_str, new_scene->group_id);
        CJSON_GET_VALUE_STRING_BY_COPY(cj_scene, ezlopi_name_str, new_scene->name);
        CJSON_GET_VALUE_STRING_BY_COPY(cj_scene, ezlopi_parent_id_str, new_scene->parent_id);

        {
            cJSON* cj_user_notifications = cJSON_GetObjectItem(cj_scene, ezlopi_user_notifications_str);
            if (cj_user_notifications && (cJSON_Array == cj_user_notifications->type))
            {
                new_scene->user_notifications = ezlopi_scenes_populate_user_notifications(cj_user_notifications);
            }
        }

        {
            cJSON* cj_house_modes = cJSON_GetObjectItem(cj_scene, ezlopi_house_modes_str);
            if (cj_house_modes && (cJSON_Array == cj_house_modes->type))
            {
                new_scene->house_modes = ezlopi_scenes_populate_house_modes(cj_house_modes);
            }
        }

        {
            cJSON* cj_then_blocks = cJSON_GetObjectItem(cj_scene, ezlopi_then_str);
            if (cj_then_blocks && (cJSON_Array == cj_then_blocks->type))
            {
                new_scene->then_block = ezlopi_scenes_populate_action_blocks(cj_then_blocks, SCENE_BLOCK_TYPE_THEN);
            }
        }

        {
            cJSON* cj_when_blocks = cJSON_GetObjectItem(cj_scene, ezlopi_when_str);
            if (cj_when_blocks && (cJSON_Array == cj_when_blocks->type))
            {
                new_scene->when_block = ezlopi_scenes_populate_when_blocks(cj_when_blocks);
            }
        }

        {
            cJSON* cj_else_blocks = cJSON_GetObjectItem(cj_scene, ezlopi_else_str);
            if (cj_else_blocks && (cJSON_Array == cj_else_blocks->type))
            {
                new_scene->else_block = ezlopi_scenes_populate_action_blocks(cj_else_blocks, SCENE_BLOCK_TYPE_ELSE);
            }
        }
    }
}

l_user_notification_v2_t* ezlopi_scenes_populate_user_notifications(cJSON* cj_user_notifications)
{
    l_user_notification_v2_t* tmp_user_notifications_head = NULL;

    if (cj_user_notifications && (cJSON_Array == cj_user_notifications->type))
    {
        int user_notifications_idx = 0;
        cJSON* cj_user_notification = NULL;
        l_user_notification_v2_t* curr_user_notification = NULL;

        while (NULL != (cj_user_notification = cJSON_GetArrayItem(cj_user_notifications, user_notifications_idx++)))
        {
            if (tmp_user_notifications_head)
            {
                curr_user_notification->next = (l_user_notification_v2_t*)malloc(sizeof(l_user_notification_v2_t));
                if (curr_user_notification->next)
                {
                    memset(curr_user_notification->next, 0, sizeof(l_user_notification_v2_t));
                    ezlopi_scenes_populate_assign_user_notification(curr_user_notification->next, cj_user_notification);
                    curr_user_notification = curr_user_notification->next;
                }
            }
            else
            {
                tmp_user_notifications_head = (l_user_notification_v2_t*)malloc(sizeof(l_user_notification_v2_t));
                if (tmp_user_notifications_head)
                {
                    memset(tmp_user_notifications_head, 0, sizeof(l_user_notification_v2_t));
                    ezlopi_scenes_populate_assign_user_notification(tmp_user_notifications_head, cj_user_notification);
                    curr_user_notification = tmp_user_notifications_head;
                }
            }
        }
    }

    return tmp_user_notifications_head;
}

void ezlopi_scenes_populate_assign_user_notification(l_user_notification_v2_t* new_user_notification, cJSON* cj_user_notification)
{
    if (new_user_notification && cj_user_notification && cj_user_notification->valuestring)
    {
        snprintf(new_user_notification->user_id, sizeof(new_user_notification->user_id), "%s", cj_user_notification->valuestring);
        new_user_notification->next = NULL;
    }
}

l_house_modes_v2_t* ezlopi_scenes_populate_house_modes(cJSON* cj_house_modes)
{
    l_house_modes_v2_t* tmp_house_mode_head = NULL;
    if (cj_house_modes)
    {
        int house_mode_idx = 0;
        cJSON* cj_house_mode = NULL;
        l_house_modes_v2_t* tmp_house_mode = NULL;

        while (NULL != (cj_house_mode = cJSON_GetArrayItem(cj_house_modes, house_mode_idx++)))
        {
            if (tmp_house_mode_head)
            {

                tmp_house_mode->next = (l_house_modes_v2_t*)malloc(sizeof(l_house_modes_v2_t));
                if (tmp_house_mode->next)
                {
                    memset(tmp_house_mode->next, 0, sizeof(l_house_modes_v2_t));
                    ezlopi_scenes_populate_assign_house_mode(tmp_house_mode->next, cj_house_mode);
                    tmp_house_mode = tmp_house_mode->next;
                }
            }
            else
            {
                tmp_house_mode_head = (l_house_modes_v2_t*)malloc(sizeof(l_house_modes_v2_t));
                if (tmp_house_mode_head)
                {
                    memset(tmp_house_mode_head, 0, sizeof(l_house_modes_v2_t));
                    ezlopi_scenes_populate_assign_house_mode(tmp_house_mode_head, cj_house_mode);
                    tmp_house_mode = tmp_house_mode_head;
                }
            }
        }
    }

    return tmp_house_mode_head;
}

void ezlopi_scenes_populate_assign_house_mode(l_house_modes_v2_t* new_house_mode, cJSON* cj_house_mode)
{
    if (new_house_mode && cj_house_mode && cj_house_mode->valuestring)
    {
        strncpy(new_house_mode->house_mode, cj_house_mode->valuestring, sizeof(new_house_mode->house_mode));
        new_house_mode->next = NULL;
    }
}

l_action_block_v2_t* ezlopi_scenes_populate_action_blocks(cJSON* cj_action_blocks, e_scenes_block_type_v2_t block_type)
{
    l_action_block_v2_t* tmp_action_block_head = NULL;
    if (cj_action_blocks)
    {
        int then_block_idx = 0;
        cJSON* cj_action_block = NULL;
        l_action_block_v2_t* tmp_then_block = NULL;

        while (NULL != (cj_action_block = cJSON_GetArrayItem(cj_action_blocks, then_block_idx++)))
        {
            if (tmp_action_block_head)
            {
                tmp_then_block->next = (l_action_block_v2_t*)malloc(sizeof(l_action_block_v2_t));
                if (tmp_then_block->next)
                {
                    memset(tmp_then_block->next, 0, sizeof(l_action_block_v2_t));
                    ezlopi_scenes_populate_assign_action_block(tmp_then_block->next, cj_action_block, block_type);
                    tmp_then_block = tmp_then_block->next;
                }
            }
            else
            {
                tmp_action_block_head = (l_action_block_v2_t*)malloc(sizeof(l_action_block_v2_t));
                if (tmp_action_block_head)
                {
                    memset(tmp_action_block_head, 0, sizeof(l_action_block_v2_t));
                    ezlopi_scenes_populate_assign_action_block(tmp_action_block_head, cj_action_block, block_type);
                    tmp_then_block = tmp_action_block_head;
                }
            }
        }
    }

    return tmp_action_block_head;
}

void ezlopi_scenes_populate_assign_action_block(l_action_block_v2_t* new_action_block, cJSON* cj_action_block, e_scenes_block_type_v2_t block_type)
{
    if (new_action_block && cj_action_block)
    {
        cJSON* cj_block_options = cJSON_GetObjectItem(cj_action_block, ezlopi_blockOptions_str);
        if (cj_block_options)
        {
            ezlopi_scenes_populate_assign_block_options(&new_action_block->block_options, cj_block_options);
        }

        new_action_block->block_type = block_type;
        cJSON* cj_delay = cJSON_GetObjectItem(cj_action_block, ezlopi_delay_str);
        if (cj_delay)
        {
            ezlopi_scenes_populate_assign_action_delay(&new_action_block->delay, cj_delay);
        }

        cJSON* cj_fields = cJSON_GetObjectItem(cj_action_block, ezlopi_fields_str);
        if (cj_fields)
        {
            new_action_block->fields = ezlopi_scenes_populate_fields(cj_fields);
        }

        cJSON* cj__temp_id = cJSON_GetObjectItem(cj_action_block, "_tempId");
        if (cj__temp_id && cj__temp_id->valuestring)
        {
            snprintf(new_action_block->_tempId, sizeof(new_action_block->_tempId), "%s", cj__temp_id->valuestring);
        }
    }
}

l_when_block_v2_t* ezlopi_scenes_populate_when_blocks(cJSON* cj_when_blocks)
{
    l_when_block_v2_t* tmp_when_block_head = NULL;
    if (cj_when_blocks)
    {
        int when_block_idx = 0;
        cJSON* cj_when_block = NULL;
        l_when_block_v2_t* tmp_when_block = NULL;

        while (NULL != (cj_when_block = cJSON_GetArrayItem(cj_when_blocks, when_block_idx++)))
        {
            if (tmp_when_block_head)
            {
                tmp_when_block->next = (l_when_block_v2_t*)malloc(sizeof(l_when_block_v2_t));
                if (tmp_when_block->next)
                {
                    memset(tmp_when_block->next, 0, sizeof(l_when_block_v2_t));
                    ezlopi_scenes_populate_assign_when_block(tmp_when_block->next, cj_when_block);
                    tmp_when_block = tmp_when_block->next;
                }
            }
            else
            {
                tmp_when_block_head = (l_when_block_v2_t*)malloc(sizeof(l_when_block_v2_t));
                if (tmp_when_block_head)
                {
                    memset(tmp_when_block_head, 0, sizeof(l_when_block_v2_t));
                    ezlopi_scenes_populate_assign_when_block(tmp_when_block_head, cj_when_block);
                    tmp_when_block = tmp_when_block_head;
                }
            }
        }
    }

    return tmp_when_block_head;
}

void ezlopi_scenes_populate_assign_when_block(l_when_block_v2_t* new_when_block, cJSON* cj_when_block)
{
    if (new_when_block)
    {
        cJSON* cj_block_options = cJSON_GetObjectItem(cj_when_block, ezlopi_blockOptions_str);
        if (cj_block_options)
        {
            ezlopi_scenes_populate_assign_block_options(&new_when_block->block_options, cj_block_options);
        }

        new_when_block->block_type = SCENE_BLOCK_TYPE_WHEN;

        cJSON* cj_fields = cJSON_GetObjectItem(cj_when_block, ezlopi_fields_str);
        if (cj_fields)
        {
            new_when_block->fields = ezlopi_scenes_populate_fields(cj_fields);
        }
    }
}

void ezlopi_scenes_populate_assign_block_options(s_block_options_v2_t* p_block_options, cJSON* cj_block_options)
{
    cJSON* cj_method = cJSON_GetObjectItem(cj_block_options, ezlopi_method_str);
    if (cj_method)
    {
        ezlopi_scenes_populate_assign_method(&p_block_options->method, cj_method);
    }
}

void ezlopi_scenes_populate_assign_method(s_method_v2_t* p_method, cJSON* cj_method)
{
    CJSON_GET_VALUE_STRING_BY_COPY(cj_method, ezlopi_name_str, p_method->name);
    p_method->type = ezlopi_scenes_method_get_type_enum(p_method->name);
}

void ezlopi_scenes_populate_assign_action_delay(s_action_delay_v2_t* action_delay, cJSON* cj_delay)
{
    if (action_delay && cj_delay)
    {
        CJSON_GET_VALUE_DOUBLE(cj_delay, "days", action_delay->days);
        CJSON_GET_VALUE_DOUBLE(cj_delay, "hours", action_delay->hours);
        CJSON_GET_VALUE_DOUBLE(cj_delay, "minutes", action_delay->minutes);
        CJSON_GET_VALUE_DOUBLE(cj_delay, "seconds", action_delay->seconds);
    }
}

l_fields_v2_t* ezlopi_scenes_populate_fields(cJSON* cj_fields)
{
    l_fields_v2_t* tmp_fields_head = NULL;
    if (cj_fields)
    {
        int fields_idx = 0;
        cJSON* cj_field = NULL;
        l_fields_v2_t* tmp_field = NULL;

        while (NULL != (cj_field = cJSON_GetArrayItem(cj_fields, fields_idx++)))
        {
            if (tmp_fields_head)
            {
                tmp_field->next = (l_fields_v2_t*)malloc(sizeof(l_fields_v2_t));
                if (tmp_field->next)
                {
                    memset(tmp_field->next, 0, sizeof(l_fields_v2_t));
                    ezlopi_scenes_populate_assign_field(tmp_field->next, cj_field);
                    tmp_field = tmp_field->next;
                }
            }
            else
            {
                tmp_fields_head = (l_fields_v2_t*)malloc(sizeof(l_fields_v2_t));
                if (tmp_fields_head)
                {
                    memset(tmp_fields_head, 0, sizeof(l_fields_v2_t));
                    ezlopi_scenes_populate_assign_field(tmp_fields_head, cj_field);
                    tmp_field = tmp_fields_head;
                }
            }
        }
    }

    return tmp_fields_head;
}

void ezlopi_scenes_populate_fields_get_value(l_fields_v2_t* field, cJSON* cj_value)
{
    if (field && cj_value)
    {
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
                CJSON_TRACE("single_obj_value", cj_value);

                field->field_value.u_value.when_block = (l_when_block_v2_t*)malloc(sizeof(l_when_block_v2_t));
                if (field->field_value.u_value.when_block)
                {
                    memset(field->field_value.u_value.when_block, 0, sizeof(l_when_block_v2_t));
                    ezlopi_scenes_populate_assign_when_block(field->field_value.u_value.when_block, cj_value);
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
            field->field_value.e_type = VALUE_TYPE_BLOCK;
            CJSON_TRACE("value", cj_value);
            l_when_block_v2_t* curr_when_block = NULL;

            while (NULL != (cj_block = cJSON_GetArrayItem(cj_value, block_idx++)))
            {
                if (field->field_value.u_value.when_block)
                {
                    curr_when_block->next = (l_when_block_v2_t*)malloc(sizeof(l_when_block_v2_t));
                    if (curr_when_block->next)
                    {
                        memset(curr_when_block->next, 0, sizeof(l_when_block_v2_t));
                        ezlopi_scenes_populate_assign_when_block(curr_when_block->next, cj_block);
                        curr_when_block = curr_when_block->next;
                    }
                }
                else
                {
                    field->field_value.u_value.when_block = (l_when_block_v2_t*)malloc(sizeof(l_when_block_v2_t));
                    if (field->field_value.u_value.when_block)
                    {
                        memset(field->field_value.u_value.when_block, 0, sizeof(l_when_block_v2_t));
                        ezlopi_scenes_populate_assign_when_block(field->field_value.u_value.when_block, cj_block);
                        curr_when_block = field->field_value.u_value.when_block;
                    }
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

void ezlopi_scenes_populate_assign_field(l_fields_v2_t* new_field, cJSON* cj_field)
{
    if (cj_field && new_field)
    {
        CJSON_GET_VALUE_STRING_BY_COPY(cj_field, ezlopi_name_str, new_field->name);
        new_field->value_type = ezlopi_core_scenes_value_get_type(cj_field, ezlopi_type_str);
        ezlopi_scenes_populate_fields_get_value(new_field, cJSON_GetObjectItem(cj_field, ezlopi_value_str));
    }
}
