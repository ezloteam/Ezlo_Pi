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
    // ezlopi_scenes_cjson_add_string(cj_args, "headers", http_request_arg->headers);
    ezlopi_scenes_cjson_add_string(cj_args, "skipSecurity", http_request_arg->skip_security);
    ezlopi_scenes_cjson_add_string(cj_args, "url", http_request_arg->url);
}

static void ezlopi_scenes_cjson_add_arg_house_modes(s_arg_house_mode_t *house_mode_arg, cJSON *cj_args)
{
}

static void ezlopi_scenes_cjson_add_arg_lua_script(s_arg_lua_script_t *lua_script_arg, cJSON *cj_args)
{
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
                cJSON *cj_args = cJSON_AddObjectToObject(cj_method, "args");
                if (cj_args)
                {
                    switch (when_block->block_options.method.type)
                    {
                    case EZLOPI_SCENE_WHEN_METHOD_IS_ITEM_STATE:
                    {
                        ezlopi_scenes_cjson_add_string(cj_method, "name", "isItemState");
                        break;
                    }
                    case EZLOPI_SCENE_WHEN_METHOD_IS_ITEM_STATE_CHANGED:
                    {
                        ezlopi_scenes_cjson_add_string(cj_method, "name", "isItemStateChanged");
                        break;
                    }
                    case EZLOPI_SCENE_WHEN_METHOD_IS_BUTTON_STATE:
                    {
                        ezlopi_scenes_cjson_add_string(cj_method, "name", "isButtonState");
                        break;
                    }
                    case EZLOPI_SCENE_WHEN_METHOD_IS_SUN_STATE:
                    {
                        ezlopi_scenes_cjson_add_string(cj_method, "name", "isSunState");
                        break;
                    }
                    case EZLOPI_SCENE_WHEN_METHOD_IS_DATE:
                    {
                        ezlopi_scenes_cjson_add_string(cj_method, "name", "isDate");
                        break;
                    }
                    case EZLOPI_SCENE_WHEN_METHOD_IS_ONCE:
                    {
                        ezlopi_scenes_cjson_add_string(cj_method, "name", "isOnce");
                        break;
                    }
                    case EZLOPI_SCENE_WHEN_METHOD_IS_INTERVAL:
                    {
                        ezlopi_scenes_cjson_add_string(cj_method, "name", "isInterval");
                        break;
                    }
                    case EZLOPI_SCENE_WHEN_METHOD_IS_DATE_RANGE:
                    {
                        ezlopi_scenes_cjson_add_string(cj_method, "name", "isDateRange");
                        break;
                    }
                    case EZLOPI_SCENE_WHEN_METHOD_IS_USER_LOCK_OPERATION:
                    {
                        ezlopi_scenes_cjson_add_string(cj_method, "name", "isUserLockOperation");
                        break;
                    }
                    case EZLOPI_SCENE_WHEN_METHOD_IS_HOUSE_MODE_CHANGED_TO:
                    {
                        ezlopi_scenes_cjson_add_string(cj_method, "name", "isHouseModeChangedTo");
                        break;
                    }
                    case EZLOPI_SCENE_WHEN_METHOD_IS_HOUSE_MODE_CHANGED_FROM:
                    {
                        ezlopi_scenes_cjson_add_string(cj_method, "name", "isHouseModeChangedFrom");
                        break;
                    }
                    case EZLOPI_SCENE_WHEN_METHOD_IS_DEVICE_STATE:
                    {
                        ezlopi_scenes_cjson_add_string(cj_method, "name", "isDeviceState");
                        break;
                    }
                    case EZLOPI_SCENE_WHEN_METHOD_IS_NETWORK_STATE:
                    {
                        ezlopi_scenes_cjson_add_string(cj_method, "name", "isNetworkState");
                        break;
                    }
                    case EZLOPI_SCENE_WHEN_METHOD_IS_SCENE_STATE:
                    {
                        ezlopi_scenes_cjson_add_string(cj_method, "name", "isSceneState");
                        break;
                    }
                    case EZLOPI_SCENE_WHEN_METHOD_IS_GROUP_STATE:
                    {
                        ezlopi_scenes_cjson_add_string(cj_method, "name", "isGroupState");
                        break;
                    }
                    case EZLOPI_SCENE_WHEN_METHOD_IS_CLOUD_STATE:
                    {
                        ezlopi_scenes_cjson_add_string(cj_method, "name", "isCloudState");
                        break;
                    }
                    case EZLOPI_SCENE_WHEN_METHOD_IS_BATTERY_STATE:
                    {
                        ezlopi_scenes_cjson_add_string(cj_method, "name", "isBatteryState");
                        break;
                    }
                    case EZLOPI_SCENE_WHEN_METHOD_IS_BATTERY_LEVEL:
                    {
                        ezlopi_scenes_cjson_add_string(cj_method, "name", "isBatteryLevel");
                        break;
                    }
                    case EZLOPI_SCENE_WHEN_METHOD_COMPARE_NUMBERS:
                    {
                        ezlopi_scenes_cjson_add_string(cj_method, "name", "compareNumbers");
                        break;
                    }
                    case EZLOPI_SCENE_WHEN_METHOD_COMPARE_NUMBER_RANGE:
                    {
                        ezlopi_scenes_cjson_add_string(cj_method, "name", "compareNumberRange");
                        break;
                    }
                    case EZLOPI_SCENE_WHEN_METHOD_COMPARE_STRINGS:
                    {
                        ezlopi_scenes_cjson_add_string(cj_method, "name", "compareStrings");
                        break;
                    }
                    case EZLOPI_SCENE_WHEN_METHOD_STRING_OPERATION:
                    {
                        ezlopi_scenes_cjson_add_string(cj_method, "name", "stringOperation");
                        break;
                    }
                    case EZLOPI_SCENE_WHEN_METHOD_IN_ARRAY:
                    {
                        ezlopi_scenes_cjson_add_string(cj_method, "name", "inArray");
                        break;
                    }
                    case EZLOPI_SCENE_WHEN_METHOD_COMPARE_VALUES:
                    {
                        ezlopi_scenes_cjson_add_string(cj_method, "name", "compareValues");
                        break;
                    }
                    case EZLOPI_SCENE_WHEN_METHOD_HAS_ATLEAST_ONE_DICTIONARY_VALUE:
                    {
                        ezlopi_scenes_cjson_add_string(cj_method, "name", "hasAtLeastOneDictionaryValue");
                        break;
                    }
                    case EZLOPI_SCENE_WHEN_METHOD_IS_FIRMWARE_UPDATE_STATE:
                    {
                        ezlopi_scenes_cjson_add_string(cj_method, "name", "isFirmwareUpdateState");
                        break;
                    }
                    case EZLOPI_SCENE_WHEN_METHOD_IS_DICTIONARY_CHANGED:
                    {
                        ezlopi_scenes_cjson_add_string(cj_method, "name", "isDictionaryChanged");
                        break;
                    }
                    case EZLOPI_SCENE_WHEN_METHOD_IS_DETECTED_IN_HOTZONE:
                    {
                        ezlopi_scenes_cjson_add_string(cj_method, "name", "isDetectedInHotzone");
                        break;
                    }
                    case EZLOPI_SCENE_WHEN_METHOD_AND:
                    {
                        ezlopi_scenes_cjson_add_string(cj_method, "name", "and");
                        break;
                    }
                    case EZLOPI_SCENE_WHEN_METHOD_NOT:
                    {
                        ezlopi_scenes_cjson_add_string(cj_method, "name", "not");
                        break;
                    }
                    case EZLOPI_SCENE_WHEN_METHOD_OR:
                    {
                        ezlopi_scenes_cjson_add_string(cj_method, "name", "or ");
                        break;
                    }
                    case EZLOPI_SCENE_WHEN_METHOD_XOR:
                    {
                        ezlopi_scenes_cjson_add_string(cj_method, "name", "xor");
                        break;
                    }
                    case EZLOPI_SCENE_WHEN_METHOD_FUNCTION:
                    {
                        ezlopi_scenes_cjson_add_string(cj_method, "name", "function");
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
                cJSON *cj_args = cJSON_AddObjectToObject(cj_method, "args");
                if (cj_args)
                {
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
            ezlopi_scenes_cjson_add_then_block_options(cj_then_block, &then_block);
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