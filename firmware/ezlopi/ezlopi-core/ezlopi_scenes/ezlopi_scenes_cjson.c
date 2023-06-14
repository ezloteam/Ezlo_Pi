#include "string.h"
#include "cJSON.h"

#include "trace.h"

#include "ezlopi_scenes.h"

static void ezlopi_scenes_cjson_add_string(cJSON *root, char *key, char *value)
{
    if (root && key && value)
    {
        if (isprint(value[0]))
        {
            cJSON_AddStringToObject(root, key, value);
        }
        else
        {
            cJSON_AddNullToObject(root, key);
        }
    }
}

static void ezlopi_scenes_cjson_add_user_notifications(cJSON *root, l_user_notification_t *user_notifications)
{
    if (root && user_notifications)
    {
        cJSON *cj_user_notifications_array = cJSON_AddArrayToObject(root, "user_notifications");
        if (cj_user_notifications_array)
        {
            while (user_notifications)
            {
                cJSON *array_item = cJSON_CreateString(user_notifications->user_id);
                if (array_item)
                {
                    if (!cJSON_AddItemToArray(cj_user_notifications_array, array_item))
                    {
                        cJSON_Delete(array_item);
                    }
                }
                user_notifications = user_notifications->next;
            }
        }
    }
}

static void ezlopi_scenes_cjson_add_house_modes(cJSON *root, l_house_modes_t *house_modes)
{
    if (root && house_modes)
    {
        cJSON *cj_user_notifications_array = cJSON_AddArrayToObject(root, "house_modes");
        if (cj_user_notifications_array)
        {
            while (house_modes)
            {
                cJSON *array_item = cJSON_CreateString(house_modes->house_mode);
                if (array_item)
                {
                    if (!cJSON_AddItemToArray(cj_user_notifications_array, array_item))
                    {
                        cJSON_Delete(array_item);
                    }
                }
                house_modes = house_modes->next;
            }
        }
    }
}

static void ezlopi_scenes_cjson_add_block_options(cJSON *cj_then_block_array, s_block_options_t *block_option)
{
    if (cj_then_block_array && block_option)
    {
        cJSON *cj_block_options = cJSON_AddObjectToObject(cj_then_block_array, "blockOptions");
        if (cj_block_options)
        {
            cJSON *cj_method = cJSON_AddObjectToObject(cj_block_options, "method");
            if (cj_method)
            {
                cJSON *cj_args = cJSON_AddObjectToObject(cj_method, "args");
                if (cj_args)
                {
                    ezlopi_scenes_cjson_add_string(cj_args, "item", block_option->method.args.item);
                    ezlopi_scenes_cjson_add_string(cj_args, "value", block_option->method.args.value);
                }
                ezlopi_scenes_cjson_add_string(cj_method, "name", block_option->method.name);
            }
        }
    }
}

static void ezlopi_scenes_cjson_add_fields(cJSON *cj_block, l_scenes_list_t *fields)
{
    if (cj_block)
    {
        cJSON *cj_fields_array = cJSON_AddArrayToObject(cj_block, "fields");
        if (cj_fields_array)
        {
            l_fields_t *curr_field = fields;
            while (curr_field)
            {
                cJSON *cj_field = cJSON_CreateObject();
                if (cj_field)
                {
                    ezlopi_scenes_cjson_add_string(cj_field, "name", curr_field->name);
                    ezlopi_scenes_cjson_add_string(cj_field, "type", curr_field->type);
                    switch (curr_field->value_type)
                    {
                    case SCENE_VALUE_TYPE_FALSE:
                    {
                        cJSON_AddFalseToObject(cj_field, "value");
                        break;
                    }
                    case SCENE_VALUE_TYPE_TRUE:
                    {
                        cJSON_AddTrueToObject(cj_field, "value");
                        break;
                    }
                    case SCENE_VALUE_TYPE_NUMBER:
                    {
                        cJSON_AddNumberToObject(cj_field, "value", curr_field->value.value_double);
                        break;
                    }
                    case SCENE_VALUE_TYPE_STRING:
                    {
                        ezlopi_scenes_cjson_add_string(cj_field, "value", curr_field->value.value_string);
                        break;
                    }
                    default:
                    {
                        break;
                    }
                    }

                    if (!cJSON_AddItemToArray(cj_fields_array, cj_field))
                    {
                        cJSON_Delete(cj_field);
                    }
                }

                curr_field = curr_field->next;
            }
        }
    }
}

static void ezlopi_scenes_cjson_add_then_blocks(cJSON *root, l_then_block_t *then_blocks)
{
    if (root && then_blocks)
    {
        cJSON *cj_then_block_array = cJSON_AddArrayToObject(root, "then");
        if (cj_then_block_array)
        {
            while (then_blocks)
            {
                cJSON *cj_then_block = cJSON_CreateObject();
                if (cj_then_block)
                {
                    ezlopi_scenes_cjson_add_block_options(cj_then_block, &then_blocks->block_options);
                    ezlopi_scenes_cjson_add_string(cj_then_block, "blockType", then_blocks->block_type);
                    ezlopi_scenes_cjson_add_fields(cj_then_block, then_blocks->fields);
                    if (!cJSON_AddItemToArray(cj_then_block_array, cj_then_block))
                    {
                        cJSON_Delete(cj_then_block);
                    }
                }

                then_blocks = then_blocks->next;
            }
        }
    }
}

static void ezlopi_scenes_cjson_add_when_blocks(cJSON *root, l_then_block_t *when_blocks)
{
    if (root && when_blocks)
    {
        cJSON *cj_when_block_array = cJSON_AddArrayToObject(root, "when");
        if (cj_when_block_array)
        {
            while (when_blocks)
            {
                cJSON *cj_when_block = cJSON_CreateObject();
                if (cj_when_block)
                {
                    ezlopi_scenes_cjson_add_block_options(cj_when_block, &when_blocks->block_options);
                    ezlopi_scenes_cjson_add_string(cj_when_block, "blockType", when_blocks->block_type);
                    ezlopi_scenes_cjson_add_fields(cj_when_block, when_blocks->fields);

                    if (!cJSON_AddItemToArray(cj_when_block_array, cj_when_block))
                    {
                        cJSON_Delete(cj_when_block);
                    }
                }

                when_blocks = when_blocks->next;
            }
        }
    }
}

cJSON *ezlopi_scenes_create_cjson(l_scenes_list_t *scenes_list)
{
    cJSON *cj_scenes_array = cJSON_CreateArray();
    if (cj_scenes_array)
    {
        while (scenes_list)
        {
            cJSON *cj_scene = cJSON_CreateObject();
            if (cj_scene)
            {
                // ezlopi_scenes_cjson_add_string(cj_scene, "_id", scenes_list->_id);
                cJSON_AddNumberToObject(cj_scene, "_id", scenes_list->_id);
                cJSON_AddBoolToObject(cj_scene, "enabled", scenes_list->enabled);
                ezlopi_scenes_cjson_add_string(cj_scene, "group_id", scenes_list->group_id);
                cJSON_AddBoolToObject(cj_scene, "is_group", scenes_list->is_group);
                ezlopi_scenes_cjson_add_string(cj_scene, "name", scenes_list->name);
                ezlopi_scenes_cjson_add_string(cj_scene, "parent_id", scenes_list->parent_id);
                ezlopi_scenes_cjson_add_user_notifications(cj_scene, scenes_list->user_notifications);
                ezlopi_scenes_cjson_add_house_modes(cj_scene, scenes_list->house_modes);
                ezlopi_scenes_cjson_add_then_blocks(cj_scene, scenes_list->then);
                ezlopi_scenes_cjson_add_when_blocks(cj_scene, scenes_list->when);

                if (!cJSON_AddItemToArray(cj_scenes_array, cj_scene))
                {
                    cJSON_Delete(cj_scene);
                }
            }

            scenes_list = scenes_list->next;
        }
    }

    return cj_scenes_array;
}

char *ezlopi_scenes_create_json_string(l_scenes_list_t *scenes_list)
{
    char *scenes_list_str = NULL;

    cJSON *cj_scenes_array = ezlopi_scenes_create_cjson(scenes_list);
    if (cj_scenes_array)
    {
        char *scenes_list_str = cJSON_Print(cj_scenes_array);
        if (scenes_list_str)
        {
            TRACE_D("scenes_list_str: \r\n%s", scenes_list_str);
            cJSON_Minify(scenes_list);
        }

        cJSON_Delete(cj_scenes_array);
    }
    return scenes_list_str;
}