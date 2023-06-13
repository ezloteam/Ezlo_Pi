#include "string.h"
#include "cJSON.h"
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
                    if (cJSON_False == cJSON_AddItemToArray(cj_user_notifications_array, array_item))
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
                    if (cJSON_False == cJSON_AddItemToArray(cj_user_notifications_array, array_item))
                    {
                        cJSON_Delete(array_item);
                    }
                }
                house_modes = house_modes->next;
            }
        }
    }
}

static void ezlopi_scenes_cjson_add_then_blocks(cJSON *root, l_then_block_t *then_blocks)
{
    if (root && then_blocks)
    {
        cJSON *cj_then_block = cJSON_AddArrayToObject(root, "then");
        if (cj_then_block)
        {
            cJSON *cj_block_options = cJSON_AddObjectToObject(cj_then_block, "blockOptions");
            if (cj_block_options)
            {
                cJSON *cj_method = cJSON_AddObjectToObject(cj_block_options, "method");
                if (cj_method)
                {
                    cJSON *cj_args = cJSON_AddObjectToObject(cj_block_options, "method");
                    if (cj_args)
                    {
                        ezlopi_scenes_cjson_add_string(cj_args, "item", then_blocks->block_options.method.args.item);
                        ezlopi_scenes_cjson_add_string(cj_args, "value", then_blocks->block_options.method.args.value);
                    }
                    ezlopi_scenes_cjson_add_string(cj_method, "name", then_blocks->block_options.method.name);
                }
            }

            ezlopi_scenes_cjson_add_string(cj_then_block, "blockType", then_blocks->block_type);

            cJSON *cj_fields_array = cJSON_AddArrayToObject(cj_then_block, "fields");
            if (cj_fields_array)
            {
                l_fields_t *curr_field = then_blocks->fields;
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
                    }

                    curr_field = curr_field->next;
                }
            }
        }
    }
}

char *ezlopi_scenes_create_cjson(l_scenes_list_t *scenes_list)
{
    char *cj_scenes = NULL;
    if (scenes_list)
    {
        cJSON *root = cJSON_CreateObject();
        if (root)
        {
            cJSON_AddBoolToObject(root, "enabled", scenes_list->enabled);
            ezlopi_scenes_cjson_add_string(root, "group_id", scenes_list->group_id);
            cJSON_AddBoolToObject(root, "is_group", scenes_list->is_group);
            ezlopi_scenes_cjson_add_string(root, "name", scenes_list->name);
            ezlopi_scenes_cjson_add_string(root, "parent_id", scenes_list->parent_id);
            ezlopi_scenes_cjson_add_user_notifications(root, scenes_list->user_notifications);
            ezlopi_scenes_cjson_add_house_modes(root, scenes_list->house_modes);
            ezlopi_scenes_cjson_add_then_blocks(root, scenes_list->then);

            // cJSON_AddStringToObject
        }
    }
    return cj_scenes;
}