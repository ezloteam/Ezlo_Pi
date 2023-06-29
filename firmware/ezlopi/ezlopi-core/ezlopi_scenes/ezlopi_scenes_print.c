#include "ezlopi_scenes.h"
#include "trace.h"

void ezlopi_print_arg_type_device(s_arg_device_t *device_arg)
{
    TRACE_D("\t\t\t\t\t\t|-- item: %s", device_arg->item);
    TRACE_D("\t\t\t\t\t\t|-- value: %s", device_arg->value);
}

void ezlopi_print_arg_type_http_request(s_arg_http_request_t *http_request_arg)
{
    TRACE_D("\t\t\t\t\t\t|-- content: %s", http_request_arg->content);
    TRACE_D("\t\t\t\t\t\t|-- content_type: %s", http_request_arg->content_type);
    TRACE_D("\t\t\t\t\t\t|-- credential: %s", http_request_arg->credential);
    // TRACE_D("\t\t\t\t\t\t|-- headers: %s", http_request_arg->headers);
    TRACE_E("\t\t\t\t\t\t|-- headers: Not Implemented!");
    TRACE_D("\t\t\t\t\t\t|-- skip_security: %s", http_request_arg->skip_security);
    TRACE_D("\t\t\t\t\t\t|-- url: %s", http_request_arg->url);
}

void ezlopi_print_arg_type_house_mode(s_arg_house_mode_t *house_mode_arg)
{
    TRACE_D("\t\t\t\t\t\t|-- house_mode: %s", house_mode_arg->house_mode);
}

void ezlopi_print_arg_type_lua_script(s_arg_lua_script_t *lua_script_arg)
{
    TRACE_E("Not Implemented!");
}

void ezlopi_print_args(s_method_t *method)
{
    if (method)
    {
        switch (method->type)
        {
        case EZLOPI_SCENE_WHEN_METHOD_IS_ITEM_STATE:
        {
            break;
        }
        case EZLOPI_SCENE_WHEN_METHOD_IS_ITEM_STATE_CHANGED:
        {
            break;
        }
        case EZLOPI_SCENE_WHEN_METHOD_IS_BUTTON_STATE:
        {
            break;
        }
        case EZLOPI_SCENE_WHEN_METHOD_IS_SUN_STATE:
        {
            break;
        }
        case EZLOPI_SCENE_WHEN_METHOD_IS_DATE:
        {
            break;
        }
        case EZLOPI_SCENE_WHEN_METHOD_IS_ONCE:
        {
            break;
        }
        case EZLOPI_SCENE_WHEN_METHOD_IS_INTERVAL:
        {
            break;
        }
        case EZLOPI_SCENE_WHEN_METHOD_IS_DATE_RANGE:
        {
            break;
        }
        case EZLOPI_SCENE_WHEN_METHOD_IS_USER_LOCK_OPERATION:
        {
            break;
        }
        case EZLOPI_SCENE_WHEN_METHOD_IS_HOUSE_MODE_CHANGED_TO:
        {
            break;
        }
        case EZLOPI_SCENE_WHEN_METHOD_IS_HOUSE_MODE_CHANGED_FROM:
        {
            break;
        }
        case EZLOPI_SCENE_WHEN_METHOD_IS_DEVICE_STATE:
        {
            break;
        }
        case EZLOPI_SCENE_WHEN_METHOD_IS_NETWORK_STATE:
        {
            break;
        }
        case EZLOPI_SCENE_WHEN_METHOD_IS_SCENE_STATE:
        {
            break;
        }
        case EZLOPI_SCENE_WHEN_METHOD_IS_GROUP_STATE:
        {
            break;
        }
        case EZLOPI_SCENE_WHEN_METHOD_IS_CLOUD_STATE:
        {
            break;
        }
        case EZLOPI_SCENE_WHEN_METHOD_IS_BATTERY_STATE:
        {
            break;
        }
        case EZLOPI_SCENE_WHEN_METHOD_IS_BATTERY_LEVEL:
        {
            break;
        }
        case EZLOPI_SCENE_WHEN_METHOD_COMPARE_NUMBERS:
        {
            break;
        }
        case EZLOPI_SCENE_WHEN_METHOD_COMPARE_NUMBER_RANGE:
        {
            break;
        }
        case EZLOPI_SCENE_WHEN_METHOD_COMPARE_STRINGS:
        {
            break;
        }
        case EZLOPI_SCENE_WHEN_METHOD_STRING_OPERATION:
        {
            break;
        }
        case EZLOPI_SCENE_WHEN_METHOD_IN_ARRAY:
        {
            break;
        }
        case EZLOPI_SCENE_WHEN_METHOD_COMPARE_VALUES:
        {
            break;
        }
        case EZLOPI_SCENE_WHEN_METHOD_HAS_ATLEAST_ONE_DICTIONARY_VALUE:
        {
            break;
        }
        case EZLOPI_SCENE_WHEN_METHOD_IS_FIRMWARE_UPDATE_STATE:
        {
            break;
        }
        case EZLOPI_SCENE_WHEN_METHOD_IS_DICTIONARY_CHANGED:
        {
            break;
        }
        case EZLOPI_SCENE_WHEN_METHOD_IS_DETECTED_IN_HOTZONE:
        {
            break;
        }
        case EZLOPI_SCENE_WHEN_METHOD_AND:
        {
            break;
        }
        case EZLOPI_SCENE_WHEN_METHOD_NOT:
        {
            break;
        }
        case EZLOPI_SCENE_WHEN_METHOD_OR:
        {
            break;
        }
        case EZLOPI_SCENE_WHEN_METHOD_XOR:
        {
            break;
        }
        case EZLOPI_SCENE_WHEN_METHOD_FUNCTION:
        {
            break;
        }

        case EZLOPI_SCENE_THEN_METHOD_SET_ITEM_VALUE:
        {
            break;
        }
        case EZLOPI_SCENE_THEN_METHOD_SET_DEVICE_ARMED:
        {
            break;
        }
        case EZLOPI_SCENE_THEN_METHOD_SEND_CLOUD_ABSTRACT_COMMAND:
        {
            break;
        }
        case EZLOPI_SCENE_THEN_METHOD_SWITCH_HOUSE_MODE:
        {
            break;
        }
        case EZLOPI_SCENE_THEN_METHOD_SEND_HTTP_REQUEST:
        {
            break;
        }
        case EZLOPI_SCENE_THEN_METHOD_RUN_CUSTOM_SCRIPT:
        {
            break;
        }
        case EZLOPI_SCENE_THEN_METHOD_RUN_PLUGIN_SCRIPT:
        {
            break;
        }
        case EZLOPI_SCENE_THEN_METHOD_RUN_SCENE:
        {
            break;
        }
        case EZLOPI_SCENE_THEN_METHOD_SET_SCENE_STATE:
        {
            break;
        }
        case EZLOPI_SCENE_THEN_RESET_LATCH:
        {
            break;
        }
        case EZLOPI_SCENE_THEN_RESET_SCENE_LATCHES:
        {
            break;
        }
        case EZLOPI_SCENE_THEN_REBOOT_HUB:
        {
            break;
        }
        case EZLOPI_SCENE_THEN_RESET_HUB:
        {
            break;
        }
        case EZLOPI_SCENE_THEN_CLOUD_API:
        {
            break;
        }
        case EZLOPI_SCENE_THEN_SET_EXPRESSION:
        {
            break;
        }
        case EZLOPI_SCENE_THEN_SET_VARIABLE:
        {
            break;
        }
        case EZLOPI_SCENE_THEN_TOGGLE_VALUE:
        {
            break;
        }
        default:
        {
            break;
        }
        }
    }
}

void ezlopi_scenes_print_method_name(void)

void ezlopi_print_methods(s_method_t *methods)
{
    TRACE_D("\t\t\t\t|-- name: %s", methods->name);
    TRACE_D("\t\t\t\t|-- args:");
    ezlopi_print_args(methods);
}

void ezlopi_print_block_options(s_block_options_t *block_options)
{
    TRACE_D("\t\t|-- blockOptions:");
    TRACE_D("\t\t\t|-- method");
    ezlopi_print_methods(&block_options->method);
}

void ezlopi_print_fields(l_fields_t *fields)
{
    TRACE_D("\t\t|-- fields: ");
    while (fields)
    {
        TRACE_D("\t\t\t|-- name: %s", fields->name);
        TRACE_D("\t\t\t|-- type: %s", fields->type);
        if (SCENE_VALUE_TYPE_NUMBER == fields->value_type)
        {
            TRACE_D("\t\t\t|-- value: %f", fields->value.value_double);
        }
        else if (SCENE_VALUE_TYPE_STRING == fields->value_type)
        {
            TRACE_D("\t\t\t|-- value: %s", fields->value.value_string);
        }
        else if ((SCENE_VALUE_TYPE_TRUE == fields->value_type) || (SCENE_VALUE_TYPE_FALSE == fields->value_type))
        {
            TRACE_D("\t\t\t|-- value: %f", fields->value.value_double);
        }
        else
        {
            TRACE_E("\t\t\t|- value type: %d", fields->value_type);
        }

        fields = fields->next;
        if (fields)
        {
            TRACE_D("\t\t\t|--");
        }
    }
}

void ezlopi_print_then_blocks(l_then_block_t *then_blocks)
{
    TRACE_D("\t|-- then: ");
    while (then_blocks)
    {
        ezlopi_print_block_options(&then_blocks->block_options);
        TRACE_D("\t\t|-- blockType: then");
        ezlopi_print_fields(then_blocks->fields);

        then_blocks = then_blocks->next;
        if (then_blocks)
        {
            TRACE_D("\t\t|--");
        }
    }
}

void ezlopi_print_house_modes(l_house_modes_t *house_modes)
{
    TRACE_D("\t|-- house_modes: ");
    while (house_modes)
    {
        TRACE_D("\t\t|-- %s", house_modes->house_mode);
        house_modes = house_modes->next;
    }
}

void ezlopi_print_user_notifications(l_user_notification_t *user_notification)
{
    TRACE_D("\t|-- user_notifications: ");
    while (user_notification)
    {
        TRACE_D("\t\t|-- %s", user_notification->user_id);
        user_notification = user_notification->next;
    }
}

void ezlopi_print_when_blocks(l_when_block_t *when_blocks)
{
    TRACE_D("\t|-- when: ");
    while (when_blocks)
    {
        ezlopi_print_block_options(&when_blocks->block_options);
        TRACE_D("\t\t|-- blockType: when");
        ezlopi_print_fields(when_blocks->fields);
        when_blocks = when_blocks->next;
    }
}

void ezlopi_scenes_print(l_scenes_list_t *scene_link_list)
{
    while (scene_link_list)
    {
        TRACE_D("\t---------------------------------------------------------------");
        TRACE_D("\t|-- enabled: %d", scene_link_list->enabled);
        TRACE_D("\t|-- is_group: %d", scene_link_list->is_group);
        TRACE_D("\t|-- group_id: %s", scene_link_list->group_id);
        TRACE_D("\t|-- name: %s", scene_link_list->name);
        TRACE_D("\t|-- parent_id: %s", scene_link_list->parent_id);
        ezlopi_print_user_notifications(scene_link_list->user_notifications);
        ezlopi_print_house_modes(scene_link_list->house_modes);
        ezlopi_print_then_blocks(scene_link_list->then);
        ezlopi_print_when_blocks(scene_link_list->when);

        TRACE_D("\t---------------------------------------------------------------");
        scene_link_list = scene_link_list->next;
    }
}


