#include "string.h"
#include "cJSON.h"

#include "trace.h"
#include "ezlopi_scenes.h"

static const char *ezlopi_scenes_methods_name[] = {
#define EZLOPI_SCENE(method_type, name, func) name,
#include "ezlopi_scenes_method_types.h"
#undef EZLOPI_SCENE
};

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

static void ezlopi_scenes_cjson_add_when_block_options(cJSON *cj_block_array, l_when_block_t *when_block)
{
    if (cj_block_array && when_block)
    {
        cJSON *cj_block_options = cJSON_AddObjectToObject(cj_block_array, "blockOptions");
        if (cj_block_options)
        {
            cJSON *cj_method = cJSON_AddObjectToObject(cj_block_options, "method");
            if (cj_method)
            {
                if ((when_block->block_options.method.type > EZLOPI_SCENE_METHOD_TYPE_NONE) &&
                    (when_block->block_options.method.type < EZLOPI_SCENE_METHOD_TYPE_MAX))
                {
                    TRACE_E("Method: %s", ezlopi_scenes_methods_name[when_block->block_options.method.type]);
                    ezlopi_scenes_cjson_add_string(cj_method, "name", ezlopi_scenes_methods_name[when_block->block_options.method.type]);
                }
                else
                {
                    TRACE_E("Method type error");
                }

                cJSON *cj_args = cJSON_AddObjectToObject(cj_method, "args");
                if (cj_args)
                {
                    l_fields_t *curr_field = when_block->fields;
                    while (curr_field)
                    {
                        cJSON_AddStringToObject(cj_args, curr_field->name, curr_field->name);
                        curr_field = curr_field->next;
                    }
                }
            }
        }
    }
}

static void ezlopi_scenes_cjson_add_then_block_options(cJSON *cj_block_array, l_then_block_t *then_block)
{
    if (cj_block_array && then_block)
    {

        cJSON *cj_block_options = cJSON_AddObjectToObject(cj_block_array, "blockOptions");
        if (cj_block_options)
        {

            cJSON *cj_method = cJSON_AddObjectToObject(cj_block_options, "method");
            if (cj_method)
            {
                if ((then_block->block_options.method.type > EZLOPI_SCENE_METHOD_TYPE_NONE) &&
                    (then_block->block_options.method.type < EZLOPI_SCENE_METHOD_TYPE_MAX))
                {
                    TRACE_E("Method: %s", ezlopi_scenes_methods_name[then_block->block_options.method.type]);
                    ezlopi_scenes_cjson_add_string(cj_method, "name", ezlopi_scenes_methods_name[then_block->block_options.method.type]);
                }
                else
                {
                    TRACE_E("Method type error");
                }

#if 0
                switch (then_block->block_options.method.type)
                {
                case EZLOPI_SCENE_THEN_METHOD_SET_ITEM_VALUE:
                {
                    ezlopi_scenes_cjson_add_string(cj_method, "name", "setItemValue");
                    break;
                }
                case EZLOPI_SCENE_THEN_METHOD_SET_DEVICE_ARMED:
                {
                    ezlopi_scenes_cjson_add_string(cj_method, "name", "setDeviceArmed");
                    break;
                }
                case EZLOPI_SCENE_THEN_METHOD_SEND_CLOUD_ABSTRACT_COMMAND:
                {
                    ezlopi_scenes_cjson_add_string(cj_method, "name", "sendCloudAbstractCommand");
                    break;
                }
                case EZLOPI_SCENE_THEN_METHOD_SWITCH_HOUSE_MODE:
                {
                    ezlopi_scenes_cjson_add_string(cj_method, "name", "switchHouseMode");
                    break;
                }
                case EZLOPI_SCENE_THEN_METHOD_SEND_HTTP_REQUEST:
                {
                    ezlopi_scenes_cjson_add_string(cj_method, "name", "sendHttpRequest");
                    break;
                }
                case EZLOPI_SCENE_THEN_METHOD_RUN_CUSTOM_SCRIPT:
                {
                    ezlopi_scenes_cjson_add_string(cj_method, "name", "runCustomScript");
                    break;
                }
                case EZLOPI_SCENE_THEN_METHOD_RUN_PLUGIN_SCRIPT:
                {
                    ezlopi_scenes_cjson_add_string(cj_method, "name", "runPluginScript");
                    break;
                }
                case EZLOPI_SCENE_THEN_METHOD_RUN_SCENE:
                {
                    ezlopi_scenes_cjson_add_string(cj_method, "name", "runScene");
                    break;
                }
                case EZLOPI_SCENE_THEN_METHOD_SET_SCENE_STATE:
                {
                    ezlopi_scenes_cjson_add_string(cj_method, "name", "setSceneState");
                    break;
                }
                case EZLOPI_SCENE_THEN_RESET_LATCH:
                {
                    ezlopi_scenes_cjson_add_string(cj_method, "name", "resetLatch");
                    break;
                }
                case EZLOPI_SCENE_THEN_RESET_SCENE_LATCHES:
                {
                    ezlopi_scenes_cjson_add_string(cj_method, "name", "resetSceneLatches");
                    break;
                }
                case EZLOPI_SCENE_THEN_REBOOT_HUB:
                {
                    ezlopi_scenes_cjson_add_string(cj_method, "name", "rebootHub");
                    break;
                }
                case EZLOPI_SCENE_THEN_RESET_HUB:
                {
                    ezlopi_scenes_cjson_add_string(cj_method, "name", "resetHub");
                    break;
                }
                case EZLOPI_SCENE_THEN_CLOUD_API:
                {
                    ezlopi_scenes_cjson_add_string(cj_method, "name", "cloudAPI");
                    break;
                }
                case EZLOPI_SCENE_THEN_SET_EXPRESSION:
                {
                    ezlopi_scenes_cjson_add_string(cj_method, "name", "setExpression");
                    break;
                }
                case EZLOPI_SCENE_THEN_SET_VARIABLE:
                {
                    ezlopi_scenes_cjson_add_string(cj_method, "name", "setVariable");
                    break;
                }
                case EZLOPI_SCENE_THEN_TOGGLE_VALUE:
                {
                    ezlopi_scenes_cjson_add_string(cj_method, "name", "toggleValue");
                    break;
                }
                default:
                {
                    break;
                }
                }
#endif
                cJSON *cj_args = cJSON_AddObjectToObject(cj_method, "args");
                if (cj_args)
                {
                    l_fields_t *curr_field = then_block->fields;
                    while (curr_field)
                    {
                        cJSON_AddStringToObject(cj_args, curr_field->name, curr_field->name);
                        curr_field = curr_field->next;
                    }
                }
            }
        }
    }
}

static void ezlopi_scenes_cjson_add_action_delay(cJSON *cj_then_block, s_action_delay_t *action_delay)
{
    if (cj_then_block && action_delay)
    {
        if (action_delay->days || action_delay->hours || action_delay->minutes || action_delay->seconds)
        {
            cJSON *cj_action_delay = cJSON_AddObjectToObject(cj_then_block, "delay");
            if (cj_action_delay)
            {
                cJSON_AddNumberToObject(cj_action_delay, "days", action_delay->days);
                cJSON_AddNumberToObject(cj_action_delay, "hours", action_delay->hours);
                cJSON_AddNumberToObject(cj_action_delay, "minutes", action_delay->minutes);
                cJSON_AddNumberToObject(cj_action_delay, "seconds", action_delay->seconds);
            }
        }
    }
}

static void ezlopi_scenes_cjson_add_fields(cJSON *cj_block, l_fields_t *fields)
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
                    // ezlopi_scenes_cjson_add_string(cj_field, "type", curr_field->type);
                    switch (curr_field->value_type)
                    {
                    case SCENE_VALUE_TYPE_BOOL:
                    {
                        ezlopi_scenes_cjson_add_string(cj_field, "type", "bool");
                        if (0 == curr_field->value.value_double)
                        {
                            cJSON_AddFalseToObject(cj_field, "value");
                        }
                        else
                        {
                            cJSON_AddTrueToObject(cj_field, "value");
                        }
                        break;
                    }
                    case SCENE_VALUE_TYPE_INT:
                    {
                        ezlopi_scenes_cjson_add_string(cj_field, "type", "int");
                        cJSON_AddNumberToObject(cj_field, "value", curr_field->value.value_double);
                        break;
                    }
                    case SCENE_VALUE_TYPE_ITEM:
                    {
                        ezlopi_scenes_cjson_add_string(cj_field, "type", "item");
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
            ezlopi_scenes_cjson_add_then_block_options(cj_then_block, then_block);
            ezlopi_scenes_cjson_add_string(cj_then_block, "blockType", "then");
            ezlopi_scenes_cjson_add_action_delay(cj_then_block, &then_block->delay);
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
            ezlopi_scenes_cjson_add_when_block_options(cj_when_block, when_block);
            ezlopi_scenes_cjson_add_string(cj_when_block, "blockType", "when");
            ezlopi_scenes_cjson_add_fields(cj_when_block, when_block->fields);
        }
    }

    return cj_when_block;
}

void ezlopi_scenes_cjson_add_when_blocks(cJSON *root, l_when_block_t *when_blocks)
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

cJSON *ezlopi_scenes_create_cjson_scene(l_scenes_list_t *scene)
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

            cJSON *cj_scene = ezlopi_scenes_create_cjson_scene(scenes_list);
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