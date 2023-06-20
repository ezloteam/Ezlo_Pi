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

static e_arg_type_t ezlopi_scenes_parse_arg_type(char *method_name)
{
    e_arg_type_t arg_type = EZLOPI_SCENE_ARG_TYPE_NONE;
    if (method_name)
    {
        if (0 == strncmp(method_name, "setItemValue", 12))
        {
            arg_type = EZLOPI_SCENE_ARG_TYPE_DEVICE;
        }
        else if (0 == strncmp(method_name, "sendHttpRequest", 15))
        {
            arg_type = EZLOPI_SCENE_ARG_TYPE_HTTP_REQUEST;
        }
        else if (0 == strncmp(method_name, "switchHouseMode", 15))
        {
            arg_type = EZLOPI_SCENE_ARG_TYPE_HOUSE_MODE;
        }
        // else if (0 == strncmp(method_name, "lua", 12)) // remains
        // {
        //     arg_type = EZLOPI_SCENE_ARG_TYPE_LUA_SCRIPT;
        // }
    }

    return arg_type;
}

static void ezlopi_scenes_cjson_add_arg_device(s_arg_device_t *device_arg, cJSON *cj_args)
{
    ezlopi_scenes_cjson_add_string(cj_args, "item", device_arg->item);
    ezlopi_scenes_cjson_add_string(cj_args, "value", device_arg->value);
}

static void ezlopi_scenes_cjson_add_arg_http_request(s_arg_http_request_t *http_request_arg, cJSON *cj_args)
{
    ezlopi_scenes_cjson_add_string(cj_args, "content", http_request_arg->content);
    ezlopi_scenes_cjson_add_string(cj_args, "contentType", http_request_arg->content_type);
    ezlopi_scenes_cjson_add_string(cj_args, "credential", http_request_arg->credential);
    ezlopi_scenes_cjson_add_string(cj_args, "headers", http_request_arg->headers);
    ezlopi_scenes_cjson_add_string(cj_args, "skipSecurity", http_request_arg->skip_security);
    ezlopi_scenes_cjson_add_string(cj_args, "url", http_request_arg->url);
}

static void ezlopi_scenes_cjson_add_arg_house_modes(s_arg_house_mode_t *house_mode_arg, cJSON *cj_args)
{
}

static void ezlopi_scenes_cjson_add_arg_lua_script(s_arg_lua_script_t *lua_script_arg, cJSON *cj_args)
{
}

static void ezlopi_scenes_cjson_add_block_options(cJSON *cj_block_array, s_block_options_t *block_option)
{
    if (cj_block_array && block_option)
    {
        cJSON *cj_block_options = cJSON_AddObjectToObject(cj_block_array, "blockOptions");
        if (cj_block_options)
        {
            cJSON *cj_method = cJSON_AddObjectToObject(cj_block_options, "method");
            if (cj_method)
            {
                ezlopi_scenes_cjson_add_string(cj_method, "name", block_option->method.name);
                block_option->method.arg_type = ezlopi_scenes_parse_arg_type(block_option->method.name);

                cJSON *cj_args = cJSON_AddObjectToObject(cj_method, "args");
                if (cj_args)
                {
                    switch (block_option->method.arg_type)
                    {
                    case EZLOPI_SCENE_ARG_TYPE_DEVICE:
                    {
                        ezlopi_scenes_cjson_add_arg_device(&block_option->method.u_arg.device, cj_args);
                        break;
                    }
                    case EZLOPI_SCENE_ARG_TYPE_HTTP_REQUEST:
                    {
                        ezlopi_scenes_cjson_add_arg_http_request(&block_option->method.u_arg.http_request, cj_args);
                        break;
                    }
                    case EZLOPI_SCENE_ARG_TYPE_HOUSE_MODE:
                    {
                        ezlopi_scenes_cjson_add_arg_house_modes(&block_option->method.u_arg.house_mode, cj_args);
                        break;
                    }
                    case EZLOPI_SCENE_ARG_TYPE_LUA_SCRIPT:
                    {
                        ezlopi_scenes_cjson_add_arg_lua_script(&block_option->method.u_arg.lua, cj_args);
                        break;
                    }
                    default:
                    {
                        break;
                    }
                    }
                }
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

cJSON *ezlopi_scenes_cjson_create_then_block(l_then_block_t *then_block)
{
    cJSON *cj_then_block = NULL;
    if (then_block)
    {
        cj_then_block = cJSON_CreateObject();
        if (cj_then_block)
        {
            ezlopi_scenes_cjson_add_block_options(cj_then_block, &then_block->block_options);
            ezlopi_scenes_cjson_add_string(cj_then_block, "blockType", "then");
            ezlopi_scenes_cjson_add_fields(cj_then_block, then_block->fields);
        }
    }

    return cj_then_block;
}

void ezlopi_scenes_cjson_add_then_blocks(cJSON *root, l_then_block_t *then_blocks)
{
    if (root && then_blocks)
    {
        cJSON *cj_then_block_array = cJSON_AddArrayToObject(root, "then");
        if (cj_then_block_array)
        {
            while (then_blocks)
            {
                cJSON *cj_then_block = ezlopi_scenes_cjson_create_then_block(then_blocks);
                if (cj_then_block)
                {
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

cJSON *ezlopi_scenes_cjson_create_when_block(l_when_block_t *when_block)
{
    cJSON *cj_when_block = NULL;
    if (when_block)
    {
        cj_when_block = cJSON_CreateObject();
        if (cj_when_block)
        {
            ezlopi_scenes_cjson_add_block_options(cj_when_block, &when_block->block_options);
            ezlopi_scenes_cjson_add_string(cj_when_block, "blockType", "when");
            ezlopi_scenes_cjson_add_fields(cj_when_block, when_block->fields);
        }
    }

    return cj_when_block;
}

void ezlopi_scenes_cjson_add_when_blocks(cJSON *root, l_then_block_t *when_blocks)
{
    if (root && when_blocks)
    {
        cJSON *cj_when_block_array = cJSON_AddArrayToObject(root, "when");
        if (cj_when_block_array)
        {
            while (when_blocks)
            {
                cJSON *cj_when_block = ezlopi_scenes_cjson_create_when_block(when_blocks);
                if (cj_when_block)
                {
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

cJSON *ezlopi_ezlopi_scenes_create_cjson_scene(l_scenes_list_t *scene)
{
    cJSON *cj_scene = NULL;
    if (scene)
    {
        cj_scene = cJSON_CreateObject();
        if (cj_scene)
        {
            char tmp_str[16] = {0};
            snprintf(tmp_str, sizeof(tmp_str), "%08x", scene->_id);
            // cJSON_AddNumberToObject(cj_scene, "_id", scene->_id);
            cJSON_AddStringToObject(cj_scene, "_id", tmp_str);
            cJSON_AddBoolToObject(cj_scene, "enabled", scene->enabled);
            ezlopi_scenes_cjson_add_string(cj_scene, "group_id", scene->group_id);
            cJSON_AddBoolToObject(cj_scene, "is_group", scene->is_group);
            ezlopi_scenes_cjson_add_string(cj_scene, "name", scene->name);
            ezlopi_scenes_cjson_add_string(cj_scene, "parent_id", scene->parent_id);
            ezlopi_scenes_cjson_add_user_notifications(cj_scene, scene->user_notifications);
            ezlopi_scenes_cjson_add_house_modes(cj_scene, scene->house_modes);
            ezlopi_scenes_cjson_add_then_blocks(cj_scene, scene->then);
            ezlopi_scenes_cjson_add_when_blocks(cj_scene, scene->when);
        }
    }

    return cj_scene;
}

cJSON *ezlopi_scenes_create_cjson_scene_list(l_scenes_list_t *scenes_list)
{
    cJSON *cj_scenes_array = cJSON_CreateArray();
    if (cj_scenes_array)
    {
        while (scenes_list)
        {
            cJSON *cj_scene = ezlopi_ezlopi_scenes_create_cjson_scene(scenes_list);
            if (cj_scene)
            {
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

    cJSON *cj_scenes_array = ezlopi_scenes_create_cjson_scene_list(scenes_list);
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