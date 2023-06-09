#include <string.h>
#include "cJSON.h"

#include "trace.h"

#include "ezlopi_scenes.h"
#include "ezlopi_devices.h"
#include "ezlopi_nvs.h"

static l_scenes_list_t *scenes_list_head = NULL;

static void __new_args_create(s_args_t *p_args, cJSON *cj_args);
static void __new_block_options_create(s_block_options_t *p_block_options, cJSON *cj_block_options);
static l_then_block_t *__new_then_block_create(cJSON *cj_then_block);
static l_then_block_t *__then_blocks_add(cJSON *cj_then_blocks);
static l_when_block_t *__new_when_block_create(cJSON *cj_when_block);
static l_when_block_t *__when_blocks_add(cJSON *cj_when_blocks);

static l_house_modes_t *__new_house_mode_create(cJSON *cj_house_mode);
static l_house_modes_t *__house_modes_add(cJSON *cj_house_modes);
static l_user_notification_t *__new_user_notification_create(cJSON *cj_user_notification);
static l_user_notification_t *__user_notifications_add(cJSON *cj_user_notifications);

static l_scenes_list_t *__new_scene_create(cJSON *cj_scene)
{
    l_scenes_list_t *new_scene = NULL;
    if (cj_scene)
    {
        new_scene = malloc(sizeof(l_scenes_list_t));
        if (new_scene)
        {
            uint32_t tmp_success_creating_scene = 1;
            memset(new_scene, 0, sizeof(l_scenes_list_t));

            CJSON_GET_VALUE_STRING_BY_COPY(cj_scene, "_id", new_scene->_id);
            CJSON_GET_VALUE_INT(cj_scene, "enabled", new_scene->enabled);
            CJSON_GET_VALUE_STRING_BY_COPY(cj_scene, "group_id", new_scene->_id);
            CJSON_GET_VALUE_INT(cj_scene, "is_group", new_scene->is_group);
            CJSON_GET_VALUE_STRING_BY_COPY(cj_scene, "name", new_scene->_id);
            CJSON_GET_VALUE_STRING_BY_COPY(cj_scene, "parent_id", new_scene->_id);

            {
                cJSON *cj_house_modes = cJSON_GetObjectItem(cj_scene, "house_modes");
                if (cj_house_modes && (cJSON_Array == cj_house_modes->type))
                {
                    new_scene->house_modes = __house_modes_add(cj_house_modes);
                }
            }

            {
                cJSON *cj_user_notifications = cJSON_GetObjectItem(cj_scene, "user_notifications");
                if (cj_user_notifications && (cJSON_Array == cj_user_notifications->type))
                {
                    new_scene->user_notifications = __user_notifications_add(cj_user_notifications);
                }
            }

            {
                cJSON *cj_then_blocks = cJSON_GetObjectItem(cj_scene, "then");
                if (cj_then_blocks && (cJSON_Array == cj_then_blocks->type))
                {
                    new_scene->then = __then_blocks_add(cj_then_blocks);
                }
            }

            {
                cJSON *cj_then_blocks = cJSON_GetObjectItem(cj_scene, "when");
                if (cj_then_blocks && (cJSON_Array == cj_then_blocks->type))
                {
                    new_scene->then = __then_blocks_add(cj_then_blocks);
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

void ezlopi_scene_create(cJSON *scene)
{
    if (scenes_list_head)
    {
        l_scenes_list_t *curr_scene = scenes_list_head;
        while (curr_scene->next)
        {
            curr_scene = curr_scene->next;
        }
    }
}

void ezlopi_scene_init(void)
{
    char *scenes_list = ezlopi_nvs_get_scenes();
    if (scenes_list)
    {
        cJSON *cj_scenes_list = cJSON_Parse(scenes_list);
        if (cj_scenes_list && (cJSON_Array == cj_scenes_list->type))
        {
            int scenes_idx = 0;
            cJSON *cj_scene = NULL;
            while (NULL != (cj_scene = cJSON_GetArrayItem(cj_scenes_list, scenes_idx)))
            {
                TRACE_B("Scene-%d:", scenes_idx);
                ezlopi_scene_create(cj_scene);
            }
        }
    }
}

void ezlopi_scene_delete(void)
{
}

static l_house_modes_t *__new_house_mode_create(cJSON *cj_house_mode)
{
    l_house_modes_t *new_house_mode = NULL;

    if (cj_house_mode && (cJSON_String == cj_house_mode->type))
    {
        new_house_mode = malloc(sizeof(l_house_modes_t));
        if (new_house_mode)
        {
            strncpy(new_house_mode->house_mode, cj_house_mode->valuestring, sizeof(new_house_mode->house_mode));
            new_house_mode->next = NULL;
        }
    }

    return new_house_mode;
}

static l_house_modes_t *__house_modes_add(cJSON *cj_house_modes)
{
    l_house_modes_t *tmp_house_mode_head = NULL;
    if (cj_house_modes)
    {
        int house_mode_idx = cJSON_GetArraySize(cj_house_modes);
        cJSON *cj_house_mode = NULL;

        while (NULL != (cj_house_mode = cJSON_GetArrayItem(cj_house_modes, house_mode_idx)))
        {
            TRACE_B("house_mode-%d:", house_mode_idx);

            if (tmp_house_mode_head)
            {
                l_house_modes_t *tmp_house_mode = tmp_house_mode_head;
                while (tmp_house_mode->next)
                {
                    tmp_house_mode = tmp_house_mode->next;
                }

                tmp_house_mode->next = __new_house_mode_create(cj_house_mode);
            }
            else
            {
                tmp_house_mode_head = __new_house_mode_create(cj_house_mode);
            }
        }
    }

    return tmp_house_mode_head;
}

static l_user_notification_t *__new_user_notification_create(cJSON *cj_user_notification)
{
    l_user_notification_t *new_user_notification = NULL;

    if (cj_user_notification && cj_user_notification->valuestring)
    {
        new_user_notification = malloc(sizeof(l_user_notification_t));
        if (new_user_notification)
        {
            snprintf(new_user_notification->user_id, sizeof(new_user_notification->user_id), "%s", cj_user_notification->valuestring);
            new_user_notification->next = NULL;
        }
    }

    return new_user_notification;
}

static l_user_notification_t *__user_notifications_add(cJSON *cj_user_notifications)
{
    l_user_notification_t *tmp_user_notifications_head = NULL;
    if (cj_user_notifications)
    {
        int user_notifications_idx = cJSON_GetArraySize(cj_user_notifications);
        cJSON *cj_user_notification = NULL;

        while (NULL != (cj_user_notification = cJSON_GetArrayItem(cj_user_notifications, user_notifications_idx)))
        {
            if (tmp_user_notifications_head)
            {
                l_user_notification_t *curr_user_notification = tmp_user_notifications_head;
                while (curr_user_notification->next)
                {
                    curr_user_notification = curr_user_notification->next;
                }
                curr_user_notification->next = __new_user_notification_create(cj_user_notification);
            }
            else
            {
                tmp_user_notifications_head = __new_user_notification_create(cj_user_notification);
            }
        }
    }

    return tmp_user_notifications_head;
}

static void __new_args_create(s_args_t *p_args, cJSON *cj_args)
{
    CJSON_GET_VALUE_STRING_BY_COPY(cj_args, "item", p_args->item);
    CJSON_GET_VALUE_STRING_BY_COPY(cj_args, "value", p_args->value);
}

static void __new_method_create(s_method_t *p_method, cJSON *cj_method)
{
    CJSON_GET_VALUE_STRING_BY_COPY(cj_method, "name", p_method->name);
    cJSON *cj_args = cJSON_GetObjectItem(cj_method, "args");
    if (cj_args)
    {
        __new_args_create(&p_method->args, cj_args);
    }
}

static void __new_block_options_create(s_block_options_t *p_block_options, cJSON *cj_block_options)
{
    cJSON *cj_method = cJSON_GetObjectItem(cj_block_options, "method");
    if (cj_method)
    {
        __new_method_create(&p_block_options->method, cj_method);
    }
}

static l_fields_t *__new_field_create(cJSON *cj_field)
{
    l_fields_t *field = NULL;
    if (cj_field)
    {
        field = malloc(sizeof(l_fields_t));
        if (field)
        {
            field->next = NULL;
            CJSON_GET_VALUE_STRING_BY_COPY(cj_field, "name", field->name);
            CJSON_GET_VALUE_STRING_BY_COPY(cj_field, "name", field->type);
            cJSON *cj_value = cJSON_GetObjectItem(cj_field, "value");
            if (cj_value)
            {
                if (cJSON_Number == cj_value->type)
                {
                    field->value_type = 0;
                    field->value.value_double = cj_value->valuedouble;
                }
                else if (cJSON_String == cj_value->type)
                {
                    field->value_type = 1;
                    snprintf(field->value.value_string, sizeof(field->value.value_string), "%s", cj_value->valuestring);
                }
                else
                {
                    TRACE_E("Value type mismatced!");
                }
            }
        }
    }

    return field;
}

static l_fields_t *__fields_add(cJSON *cj_fields)
{
    l_fields_t *tmp_fields_head = NULL;
    if (cj_fields)
    {
        int fields_idx = cJSON_GetArraySize(cj_fields);
        cJSON *cj_field = NULL;
        while (NULL != (cj_field == cJSON_GetArrayItem(cj_fields, fields_idx)))
        {
            TRACE_B("field-idx: %d", fields_idx);
            if (tmp_fields_head)
            {
                l_fields_t *tmp_flield = tmp_fields_head;
                while (tmp_flield->next)
                {
                    tmp_flield = tmp_flield->next;
                }

                tmp_flield->next = __new_field_create(cj_field);
            }
            else
            {
                tmp_fields_head = __new_field_create(cj_field);
            }
        }
    }

    return tmp_fields_head;
}

static l_then_block_t *__new_then_block_create(cJSON *cj_then_block)
{
    l_then_block_t *new_then_block = malloc(sizeof(l_then_block_t));
    if (new_then_block)
    {
        cJSON *cj_block_options = cJSON_GetObjectItem(cj_then_block, "blockOptions");
        if (cj_block_options)
        {
            __new_block_options_create(&new_then_block->block_options, cj_block_options);
        }

        cJSON *cj_fields = cJSON_GetObjectItem(cj_then_block, "fields");
        if (cj_fields)
        {
            new_then_block->fields = __fields_add(cj_fields);
        }
    }

    return new_then_block;
}

static l_then_block_t *__then_blocks_add(cJSON *cj_then_blocks)
{
    l_then_block_t *tmp_then_block_head = NULL;
    if (cj_then_blocks)
    {
        int then_block_idx = cJSON_GetArraySize(cj_then_blocks);
        cJSON *cj_then_block = NULL;

        while (NULL != (cj_then_block = cJSON_GetArrayItem(cj_then_blocks, then_block_idx)))
        {
            TRACE_B("then_block-%d:", then_block_idx);

            if (tmp_then_block_head)
            {
                l_then_block_t *tmp_then_block = tmp_then_block_head;
                while (tmp_then_block->next)
                {
                    tmp_then_block = tmp_then_block->next;
                }

                tmp_then_block->next = __new_then_block_create(cj_then_block);
            }
            else
            {
                tmp_then_block_head = __new_then_block_create(cj_then_block);
            }
        }
    }

    return tmp_then_block_head;
}

static l_when_block_t *__new_when_block_create(cJSON *cj_when_block)
{
    l_when_block_t *new_when_block = malloc(sizeof(l_when_block_t));
    if (new_when_block)
    {
        cJSON *cj_block_options = cJSON_GetObjectItem(cj_when_block, "blockOptions");
        if (cj_block_options)
        {
            __new_block_options_create(&new_when_block->block_options, cj_block_options);
        }

        cJSON *cj_fields = cJSON_GetObjectItem(cj_when_block, "fields");
        if (cj_fields)
        {
            new_when_block->fields = __fields_add(cj_fields);
        }
    }

    return new_when_block;
}

static l_when_block_t *__when_blocks_add(cJSON *cj_when_blocks)
{
    l_when_block_t *tmp_when_block_head = NULL;
    if (cj_when_blocks)
    {
        int when_block_idx = cJSON_GetArraySize(cj_when_blocks);
        cJSON *cj_when_block = NULL;

        while (NULL != (cj_when_block = cJSON_GetArrayItem(cj_when_blocks, when_block_idx)))
        {
            TRACE_B("when_block-%d:", when_block_idx);

            if (tmp_when_block_head)
            {
                l_when_block_t *tmp_when_block = tmp_when_block_head;
                while (tmp_when_block->next)
                {
                    tmp_when_block = tmp_when_block->next;
                }

                tmp_when_block->next = __new_when_block_create(cj_when_block);
            }
            else
            {
                tmp_when_block_head = __new_when_block_create(cj_when_block);
            }
        }
    }

    return tmp_when_block_head;
}
