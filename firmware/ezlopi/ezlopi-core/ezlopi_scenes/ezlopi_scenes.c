#include <string.h>
#include "cJSON.h"

#include "trace.h"

#include "ezlopi_nvs.h"
#include "ezlopi_scenes.h"
#include "ezlopi_devices.h"

static const char *test_scene_create_str = "[{\"enabled\":true,\"group_id\":null,\"is_group\":false,\"name\":\"testRule\",\"parent_id\":\"5c6ec961cc01eb07f86f9dd9\",\"user_notifications\":[\"324234234\",\"456456453\",\"678678678\"],\"house_modes\":[\"1\",\"2\",\"4\"],\"then\":[{\"blockOptions\":{\"method\":{\"args\":{\"item\":\"item\",\"value\":\"value\"},\"name\":\"setItemValue\"}},\"blockType\":\"then\",\"fields\":[{\"name\":\"item\",\"type\":\"item\",\"value\":\"897607_32771_1\"},{\"name\":\"value\",\"type\":\"int\",\"value\":10}]}],\"when\":[{\"blockOptions\":{\"method\":{\"args\":{\"item\":\"item\",\"value\":\"value\"},\"name\":\"isItemState\"}},\"blockType\":\"when\",\"fields\":[{\"name\":\"item\",\"type\":\"item\",\"value\":\"5c7fea6b7f00000ab55f2e55\"},{\"name\":\"value\",\"type\":\"bool\",\"value\":true}]}]}]";

static l_scenes_list_t *scenes_list_head = NULL;

static void __new_block_options_create(s_block_options_t *p_block_options, cJSON *cj_block_options);

static l_then_block_t *__new_then_block_create(cJSON *cj_then_block);
static l_then_block_t *__then_blocks_add(cJSON *cj_then_blocks);

static l_when_block_t *__new_when_block_create(cJSON *cj_when_block);
static l_when_block_t *__when_blocks_add(cJSON *cj_when_blocks);

static l_house_modes_t *__new_house_mode_create(cJSON *cj_house_mode);
static l_house_modes_t *__house_modes_add(cJSON *cj_house_modes);

static l_user_notification_t *__new_user_notification_create(cJSON *cj_user_notification);
static l_user_notification_t *__user_notifications_add(cJSON *cj_user_notifications);

static l_scenes_list_t *__new_scene_create(cJSON *cj_scene);

l_scenes_list_t *ezlopi_scenes_pop_by_id(uint32_t _id)
{
    l_scenes_list_t *ret_scene = NULL;

    if (_id == scenes_list_head->_id)
    {
        ret_scene = scenes_list_head;
        scenes_list_head = scenes_list_head->next;
        ret_scene->next = NULL;
    }
    else
    {
        l_scenes_list_t *scenes_list = scenes_list_head;
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

l_scenes_list_t *ezlopi_scenes_get_by_id(uint32_t id)
{
    l_scenes_list_t *tmp_scenes_list = scenes_list_head;
    while (tmp_scenes_list)
    {
        if (tmp_scenes_list->_id == id)
        {
            break;
        }
        tmp_scenes_list = tmp_scenes_list->next;
    }

    return tmp_scenes_list;
}

void ezlopi_scenes_update_by_id(uint32_t _id, cJSON *cj_scene)
{
    if (_id && cj_scene && scenes_list_head)
    {
        if (_id == scenes_list_head->_id)
        {
            l_scenes_list_t *updated_scene = __new_scene_create(cj_scene);
            if (updated_scene)
            {
                l_scenes_list_t *tmp_scene = scenes_list_head;
                updated_scene->next = scenes_list_head->next;
                scenes_list_head = updated_scene;
                tmp_scene->next = NULL;
                ezlopi_scenes_delete(tmp_scene);
            }
        }
        else
        {
            l_scenes_list_t *curr_scene = scenes_list_head;
            while (curr_scene->next)
            {
                if (_id == curr_scene->next->_id)
                {
                    l_scenes_list_t *updated_scene = __new_scene_create(cj_scene);
                    if (updated_scene)
                    {
                        l_scenes_list_t *tmp_scene = curr_scene->next;
                        updated_scene->next = curr_scene->next->next;
                        curr_scene->next = updated_scene;
                        tmp_scene->next = NULL;
                        ezlopi_scenes_delete(tmp_scene);
                    }

                    break;
                }

                curr_scene = curr_scene->next;
            }
        }
    }
}

void ezlopi_scene_update_nvs(void)
{
    char *scene_json_str = ezlopi_scenes_create_json_string(scenes_list_head);
    if (scene_json_str)
    {
        ezlopi_nvs_scene_set(scene_json_str);
        free(scene_json_str);
    }
}

l_scenes_list_t *ezlopi_scenes_get_scenes_list(void)
{
    return scenes_list_head;
}

void ezlopi_scene_add(cJSON *cj_scene)
{
    if (scenes_list_head)
    {
        l_scenes_list_t *curr_scene = scenes_list_head;
        while (curr_scene->next)
        {
            curr_scene = curr_scene->next;
        }

        curr_scene->next = __new_scene_create(cj_scene);
    }
    else
    {
        scenes_list_head = __new_scene_create(cj_scene);
    }
}

void ezlopi_scene_init(void)
{
    // const char *scenes_list = test_scene_create_str;
    const char *scenes_list = ezlopi_nvs_scene_get();
    if (scenes_list)
    {
        TRACE_D("Scene read from NVS:\r\n%s", scenes_list);
        cJSON *cj_scenes_list = cJSON_Parse(scenes_list);

        if (cj_scenes_list)
        {
            if (cJSON_Array == cj_scenes_list->type)
            {
                int scenes_size = cJSON_GetArraySize(cj_scenes_list);
                int scenes_idx = 0;
                cJSON *cj_scene = NULL;
                while (NULL != (cj_scene = cJSON_GetArrayItem(cj_scenes_list, scenes_idx++)))
                {
                    ezlopi_scene_add(cj_scene);
                }
            }
            else if (cJSON_Object == cj_scenes_list->type)
            {
                ezlopi_scene_add(cj_scenes_list);
            }
        }

        ezlopi_scenes_print(scenes_list_head);
    }
    else
    {
        TRACE_E("Scene read from NVS is NULL!");
    }

    char *scenes_json_str = ezlopi_scenes_create_json_string(scenes_list_head);
    if (scenes_json_str)
    {
        free(scenes_json_str);
    }
}

static l_house_modes_t *__new_house_mode_create(cJSON *cj_house_mode)
{
    l_house_modes_t *new_house_mode = NULL;

    if (cj_house_mode && cj_house_mode->valuestring)
    {
        new_house_mode = malloc(sizeof(l_house_modes_t));
        if (new_house_mode)
        {
            memset(new_house_mode, 0, sizeof(l_house_modes_t));
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
        int house_mode_idx = 0;
        cJSON *cj_house_mode = NULL;

        while (NULL != (cj_house_mode = cJSON_GetArrayItem(cj_house_modes, house_mode_idx++)))
        {
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
            memset(new_user_notification, 0, sizeof(l_user_notification_t));
            snprintf(new_user_notification->user_id, sizeof(new_user_notification->user_id), "%s", cj_user_notification->valuestring);
            new_user_notification->next = NULL;
        }
    }

    return new_user_notification;
}

static l_user_notification_t *__user_notifications_add(cJSON *cj_user_notifications)
{
    l_user_notification_t *tmp_user_notifications_head = NULL;
    if (cj_user_notifications && (cJSON_Array == cj_user_notifications->type))
    {
        cJSON *cj_user_notification = NULL;
        int user_notifications_idx = 0;

        while (NULL != (cj_user_notification = cJSON_GetArrayItem(cj_user_notifications, user_notifications_idx++)))
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

static e_arg_type_t __parse_arg_type(char *method_name)
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
        else if (0 == strncmp(method_name, "lua", 12)) // remains
        {
            TRACE_E("Not Imeplemented!");
            arg_type = EZLOPI_SCENE_ARG_TYPE_LUA_SCRIPT;
        }
        else
        {
            TRACE_E("Not Imeplemented!, arg_type: %d", arg_type);
        }
    }

    return arg_type;
}

static void __new_args_create_device(s_arg_device_t *arg_device, cJSON *cj_args)
{
    if (arg_device && cj_args)
    {
        CJSON_GET_VALUE_STRING_BY_COPY(cj_args, "item", arg_device->item);
        CJSON_GET_VALUE_STRING_BY_COPY(cj_args, "value", arg_device->value);
    }
}

// static void __nwe_args_create_http_request_header(s_args)

static void __new_args_create_http_request(s_arg_http_request_t *arg_http_request, cJSON *cj_args)
{
    if (arg_http_request && cj_args)
    {
        CJSON_GET_VALUE_STRING_BY_COPY(cj_args, "content", arg_http_request->content);
        CJSON_GET_VALUE_STRING_BY_COPY(cj_args, "contentType", arg_http_request->content_type);
        CJSON_GET_VALUE_STRING_BY_COPY(cj_args, "credential", arg_http_request->credential);
        // CJSON_GET_VALUE_STRING_BY_COPY(cj_args, "headers", arg_http_request->headers);
        CJSON_GET_VALUE_STRING_BY_COPY(cj_args, "skipSecurity", arg_http_request->skip_security);
        CJSON_GET_VALUE_STRING_BY_COPY(cj_args, "url", arg_http_request->url);
    }
}

static void __new_args_create_house_mode(s_arg_house_mode_t *arg_house_mode, cJSON *cj_args)
{
    if (arg_house_mode && cj_args)
    {
        TRACE_E("Not Implemented!");
    }
}

static void __new_args_create_lua_script(s_arg_lua_script_t *arg_lua_script, cJSON *cj_args)
{
    if (arg_lua_script && cj_args)
    {
        TRACE_E("Not Implemented!");
    }
}

static void __new_method_create(s_method_t *p_method, cJSON *cj_method)
{
    CJSON_GET_VALUE_STRING_BY_COPY(cj_method, "name", p_method->name);
    cJSON *cj_args = cJSON_GetObjectItem(cj_method, "args");
    if (cj_args)
    {
        p_method->arg_type = __parse_arg_type(p_method->name);
        switch (p_method->arg_type)
        {
        case EZLOPI_SCENE_ARG_TYPE_DEVICE:
        {
            __new_args_create_device(&p_method->u_arg.device, cj_args);
            break;
        }
        case EZLOPI_SCENE_ARG_TYPE_HTTP_REQUEST:
        {
            __new_args_create_http_request(&p_method->u_arg.http_request, cj_args);
            break;
        }
        case EZLOPI_SCENE_ARG_TYPE_HOUSE_MODE:
        {
            __new_args_create_house_mode(&p_method->u_arg.house_mode, cj_args);
            break;
        }
        case EZLOPI_SCENE_ARG_TYPE_LUA_SCRIPT:
        {
            __new_args_create_lua_script(&p_method->u_arg.lua, cj_args);
            break;
        }

        default:
        {
            break;
        }
        }
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

static void __new_action_delay(s_action_delay_t *action_delay, cJSON *cj_delay)
{
    if (action_delay && cj_delay)
    {
        CJSON_GET_VALUE_DOUBLE(cj_delay, "days", action_delay->days);
        CJSON_GET_VALUE_DOUBLE(cj_delay, "hours", action_delay->hours);
        CJSON_GET_VALUE_DOUBLE(cj_delay, "minutes", action_delay->minutes);
        CJSON_GET_VALUE_DOUBLE(cj_delay, "seconds", action_delay->seconds);
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
            memset(field, 0, sizeof(l_fields_t));
            CJSON_GET_VALUE_STRING_BY_COPY(cj_field, "name", field->name);
            CJSON_GET_VALUE_STRING_BY_COPY(cj_field, "type", field->type);
            cJSON *cj_value = cJSON_GetObjectItem(cj_field, "value");
            if (cj_value)
            {
                switch (cj_value->type)
                {
                case cJSON_Number:
                {
                    field->value_type = SCENE_VALUE_TYPE_NUMBER;
                    field->value.value_double = cj_value->valuedouble;
                    TRACE_B("value: %f", field->value.value_double);
                    break;
                }
                case cJSON_String:
                {
                    field->value_type = SCENE_VALUE_TYPE_STRING;
                    snprintf(field->value.value_string, sizeof(field->value.value_string), "%s", cj_value->valuestring);
                    TRACE_B("value: %s", field->value.value_string);
                    break;
                }
                case cJSON_True:
                {
                    field->value_type = SCENE_VALUE_TYPE_TRUE;
                    field->value.value_double = 1;
                    TRACE_B("value: 1");
                    break;
                }
                case cJSON_False:
                {
                    field->value_type = SCENE_VALUE_TYPE_FALSE;
                    field->value.value_double = 0;
                    TRACE_B("value: 0");
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
    }

    return field;
}

static l_fields_t *__fields_add(cJSON *cj_fields)
{
    l_fields_t *tmp_fields_head = NULL;
    if (cj_fields)
    {
        int fields_idx = 0;
        cJSON *cj_field = NULL;
        while (NULL != (cj_field = cJSON_GetArrayItem(cj_fields, fields_idx++)))
        {
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
        memset(new_then_block, 0, sizeof(l_then_block_t));
        cJSON *cj_block_options = cJSON_GetObjectItem(cj_then_block, "blockOptions");
        if (cj_block_options)
        {
            __new_block_options_create(&new_then_block->block_options, cj_block_options);
        }

        new_then_block->block_type = SCENE_BLOCK_TYPE_THEN;
        cJSON *cj_delay = cJSON_GetObjectItem(cj_then_block, "delay");
        if (cj_delay)
        {
            __new_action_delay(&new_then_block->delay, cj_delay);
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
        int then_block_idx = 0;
        cJSON *cj_then_block = NULL;

        while (NULL != (cj_then_block = cJSON_GetArrayItem(cj_then_blocks, then_block_idx++)))
        {
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
        memset(new_when_block, 0, sizeof(l_when_block_t));
        cJSON *cj_block_options = cJSON_GetObjectItem(cj_when_block, "blockOptions");
        if (cj_block_options)
        {
            __new_block_options_create(&new_when_block->block_options, cj_block_options);
        }

        new_when_block->block_type = SCENE_BLOCK_TYPE_WHEN;

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
        int when_block_idx = 0;
        cJSON *cj_when_block = NULL;

        while (NULL != (cj_when_block = cJSON_GetArrayItem(cj_when_blocks, when_block_idx++)))
        {
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

static l_scenes_list_t *__new_scene_create(cJSON *cj_scene)
{
    l_scenes_list_t *new_scene = NULL;
    if (cj_scene)
    {
        new_scene = malloc(sizeof(l_scenes_list_t));
        if (new_scene)
        {
            memset(new_scene, 0, sizeof(l_scenes_list_t));

            uint32_t tmp_success_creating_scene = 1;

            cJSON *cj_id = cJSON_GetObjectItem(cj_scene, "_id");
            if (cj_id && cj_id->valuestring)
            {
                new_scene->_id = strtoul(cj_id->valuestring, NULL, 16);
            }
            else
            {
                new_scene->_id = ezlopi_cloud_generate_scene_id();
            }
            CJSON_GET_VALUE_INT(cj_scene, "enabled", new_scene->enabled);
            CJSON_GET_VALUE_INT(cj_scene, "is_group", new_scene->is_group);
            // if (new_scene->is_group)
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
                    new_scene->user_notifications = __user_notifications_add(cj_user_notifications);
                }
            }

            {
                cJSON *cj_house_modes = cJSON_GetObjectItem(cj_scene, "house_modes");
                if (cj_house_modes && (cJSON_Array == cj_house_modes->type))
                {
                    new_scene->house_modes = __house_modes_add(cj_house_modes);
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
                    new_scene->when = __when_blocks_add(cj_then_blocks);
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
