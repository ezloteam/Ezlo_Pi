#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_scenes_populate.h"

l_scenes_list_v2_t *ezlopi_scenes_populate_new_scene(cJSON *cj_scene, uint32_t scene_id)
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

            CJSON_GET_VALUE_INT(cj_scene, ezlopi_enabled_str, new_scene->enabled);
            CJSON_GET_VALUE_INT(cj_scene, ezlopi_is_group_str, new_scene->is_group);

            {
                CJSON_GET_VALUE_STRING_BY_COPY(cj_scene, ezlopi_group_id_str, new_scene->group_id);
                TRACE_D("group_id: %s", new_scene->group_id);
            }

            CJSON_GET_VALUE_STRING_BY_COPY(cj_scene, ezlopi_name_str, new_scene->name);
            CJSON_GET_VALUE_STRING_BY_COPY(cj_scene, ezlopi_parent_id_str, new_scene->parent_id);

            {
                cJSON *cj_user_notifications = cJSON_GetObjectItem(cj_scene, ezlopi_user_notifications_str);
                if (cj_user_notifications && (cJSON_Array == cj_user_notifications->type))
                {
                    new_scene->user_notifications = ezlopi_scenes_populate_user_notifications(cj_user_notifications);
                }
            }

            {
                cJSON *cj_house_modes = cJSON_GetObjectItem(cj_scene, ezlopi_house_modes_str);
                if (cj_house_modes && (cJSON_Array == cj_house_modes->type))
                {
                    new_scene->house_modes = ezlopi_scenes_populate_house_modes(cj_house_modes);
                }
            }

            {
                cJSON *cj_then_blocks = cJSON_GetObjectItem(cj_scene, ezlopi_then_str);
                if (cj_then_blocks && (cJSON_Array == cj_then_blocks->type))
                {
                    new_scene->then_block = ezlopi_scenes_populate_action_blocks(cj_then_blocks, SCENE_BLOCK_TYPE_THEN);
                }
            }

            {
                cJSON *cj_then_blocks = cJSON_GetObjectItem(cj_scene, ezlopi_when_str);
                if (cj_then_blocks && (cJSON_Array == cj_then_blocks->type))
                {
                    new_scene->when_block = ezlopi_scenes_populate_when_blocks(cj_then_blocks);
                }
            }

            {
                cJSON *cj_else_blocks = cJSON_GetObjectItem(cj_scene, ezlopi_else_str);
                if (cj_else_blocks && (cJSON_Array == cj_else_blocks->type))
                {
                    new_scene->else_block = ezlopi_scenes_populate_action_blocks(cj_else_blocks, SCENE_BLOCK_TYPE_ELSE);
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

l_user_notification_v2_t *ezlopi_scenes_populate_user_notifications(cJSON *cj_user_notifications)
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
                curr_user_notification->next = ezlopi_scenes_populate_new_user_notification(cj_user_notification);
            }
            else
            {
                tmp_user_notifications_head = ezlopi_scenes_populate_new_user_notification(cj_user_notification);
            }
        }
    }

    return tmp_user_notifications_head;
}

l_user_notification_v2_t *ezlopi_scenes_populate_new_user_notification(cJSON *cj_user_notification)
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

l_house_modes_v2_t *ezlopi_scenes_populate_house_modes(cJSON *cj_house_modes)
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

                tmp_house_mode->next = ezlopi_scenes_populate_new_house_mode(cj_house_mode);
            }
            else
            {
                tmp_house_mode_head = ezlopi_scenes_populate_new_house_mode(cj_house_mode);
            }
        }
    }

    return tmp_house_mode_head;
}

l_house_modes_v2_t *ezlopi_scenes_populate_new_house_mode(cJSON *cj_house_mode)
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

l_action_block_v2_t *ezlopi_scenes_populate_action_blocks(cJSON *cj_action_blocks, e_scenes_block_type_v2_t block_type)
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

                tmp_then_block->next = ezlopi_scenes_populate_new_action_block(cj_action_block, block_type);
            }
            else
            {
                tmp_action_block_head = ezlopi_scenes_populate_new_action_block(cj_action_block, block_type);
            }
        }
    }

    return tmp_action_block_head;
}

l_action_block_v2_t *ezlopi_scenes_populate_new_action_block(cJSON *cj_action_block, e_scenes_block_type_v2_t block_type)
{
    l_action_block_v2_t *new_then_block = malloc(sizeof(l_action_block_v2_t));
    if (new_then_block)
    {
        memset(new_then_block, 0, sizeof(l_action_block_v2_t));
        cJSON *cj_block_options = cJSON_GetObjectItem(cj_action_block, ezlopi_blockOptions_str);
        if (cj_block_options)
        {
            ezlopi_scenes_populate_new_block_options(&new_then_block->block_options, cj_block_options);
        }

        new_then_block->block_type = block_type;
        cJSON *cj_delay = cJSON_GetObjectItem(cj_action_block, ezlopi_delay_str);
        if (cj_delay)
        {
            ezlopi_scenes_populate_new_action_delay(&new_then_block->delay, cj_delay);
        }

        cJSON *cj_fields = cJSON_GetObjectItem(cj_action_block, ezlopi_fields_str);
        if (cj_fields)
        {
            new_then_block->fields = ezlopi_scenes_populate_fields(cj_fields);
        }

        cJSON *cj__temp_id = cJSON_GetObjectItem(cj_action_block, "_tempId");
        if (cj__temp_id && cj__temp_id->valuestring)
        {
            snprintf(new_then_block->_tempId, sizeof(new_then_block->_tempId), "%s", cj__temp_id->valuestring);
        }
    }

    return new_then_block;
}

l_when_block_v2_t *ezlopi_scenes_populate_when_blocks(cJSON *cj_when_blocks)
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

                tmp_when_block->next = ezlopi_scenes_populate_new_when_block(cj_when_block);
            }
            else
            {
                tmp_when_block_head = ezlopi_scenes_populate_new_when_block(cj_when_block);
            }
        }
    }

    return tmp_when_block_head;
}

l_when_block_v2_t *ezlopi_scenes_populate_new_when_block(cJSON *cj_when_block)
{
    l_when_block_v2_t *new_when_block = malloc(sizeof(l_when_block_v2_t));
    if (new_when_block)
    {
        memset(new_when_block, 0, sizeof(l_when_block_v2_t));
        cJSON *cj_block_options = cJSON_GetObjectItem(cj_when_block, ezlopi_blockOptions_str);
        if (cj_block_options)
        {
            ezlopi_scenes_populate_new_block_options(&new_when_block->block_options, cj_block_options);
        }

        new_when_block->block_type = SCENE_BLOCK_TYPE_WHEN;

        cJSON *cj_fields = cJSON_GetObjectItem(cj_when_block, ezlopi_fields_str);
        if (cj_fields)
        {
            new_when_block->fields = ezlopi_scenes_populate_fields(cj_fields);
        }
    }

    return new_when_block;
}

void ezlopi_scenes_populate_new_block_options(s_block_options_v2_t *p_block_options, cJSON *cj_block_options)
{
    cJSON *cj_method = cJSON_GetObjectItem(cj_block_options, ezlopi_key_method_str);
    if (cj_method)
    {
        ezlopi_scenes_populate_new_method(&p_block_options->method, cj_method);
    }
}

void ezlopi_scenes_populate_new_method(s_method_v2_t *p_method, cJSON *cj_method)
{
    CJSON_GET_VALUE_STRING_BY_COPY(cj_method, ezlopi_name_str, p_method->name);
    p_method->type = ezlopi_scenes_method_get_type_enum(p_method->name);
}

void ezlopi_scenes_populate_new_action_delay(s_action_delay_v2_t *action_delay, cJSON *cj_delay)
{
    if (action_delay && cj_delay)
    {
        CJSON_GET_VALUE_DOUBLE(cj_delay, "days", action_delay->days);
        CJSON_GET_VALUE_DOUBLE(cj_delay, "hours", action_delay->hours);
        CJSON_GET_VALUE_DOUBLE(cj_delay, "minutes", action_delay->minutes);
        CJSON_GET_VALUE_DOUBLE(cj_delay, "seconds", action_delay->seconds);
    }
}

l_fields_v2_t *ezlopi_scenes_populate_fields(cJSON *cj_fields)
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

                tmp_flield->next = ezlopi_scenes_populate_new_field(cj_field);
            }
            else
            {
                tmp_fields_head = ezlopi_scenes_populate_new_field(cj_field);
            }
        }
    }

    return tmp_fields_head;
}

void ezlopi_scenes_populate_fields_get_value(l_fields_v2_t *field, cJSON *cj_value)
{
    if (field && cj_value)
    {
        trace("type: %s", ezlopi_scene_get_scene_value_type_name_v2(field->value_type));
        switch (cj_value->type)
        {
        case cJSON_Number:
        {
            field->value.type = VALUE_TYPE_NUMBER;
            field->value.value_double = cj_value->valuedouble;
            TRACE_B("value: %f", field->value.value_double);
            break;
        }
        case cJSON_String:
        {
            field->value.type = VALUE_TYPE_STRING;
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
            field->value.type = VALUE_TYPE_BOOL;
            field->value.value_bool = true;
            TRACE_B("value: true");
            break;
        }
        case cJSON_False:
        {
            field->value.type = VALUE_TYPE_BOOL;
            field->value.value_bool = false;
            TRACE_B("value: false");
            break;
        }
        case cJSON_Object:
        {
            field->value.type = VALUE_TYPE_CJSON;
            field->value.cj_value = cJSON_Duplicate(cj_value, cJSON_True);
            CJSON_TRACE("value", field->value.cj_value);
            break;
        }
        case cJSON_Array:
        {
            int block_idx = 0;
            cJSON *cj_block = NULL;
            field->value.type = VALUE_TYPE_BLOCK;
            CJSON_TRACE("value", cj_value);

            while (NULL != (cj_block = cJSON_GetArrayItem(cj_value, block_idx++)))
            {
                if (field->value.when_block)
                {
                    l_when_block_v2_t *curr_when_block = field->value.when_block;
                    while (curr_when_block->next)
                    {
                        curr_when_block = curr_when_block->next;
                    }
                    curr_when_block->next = ezlopi_scenes_populate_new_when_block(cj_block);
                }
                else
                {
                    field->value.when_block = ezlopi_scenes_populate_new_when_block(cj_block);
                }
            }
            break;
        }
        default:
        {
            field->value.type = VALUE_TYPE_UNDEFINED;
            TRACE_E("cj_value type: %d", cj_value->type);
            break;
        }
        }
    }
}

l_fields_v2_t *ezlopi_scenes_populate_new_field_populate(cJSON *cj_field)
{
    l_fields_v2_t *field = NULL;
    if (cj_field)
    {
        field = malloc(sizeof(l_fields_v2_t));
        if (field)
        {
            memset(field, 0, sizeof(l_fields_v2_t));
            CJSON_GET_VALUE_STRING_BY_COPY(cj_field, ezlopi_name_str, field->name);

            field->value_type = ezlopi_scenes_get_expressions_value_type(cJSON_GetObjectItem(cj_field, ezlopi_type_str));
            ezlopi_scenes_populate_fields_get_value(field, cJSON_GetObjectItem(cj_field, ezlopi_value_str));
        }
    }

    return field;
}
