#include "ezlopi_scenes.h"
#include "trace.h"

void ezlopi_print_args(s_args_t *args)
{
    TRACE_D("\t\t\t\t\t\t|-- item: %s", args->item);
    TRACE_D("\t\t\t\t\t\t|-- value: %s", args->value);
}

void ezlopi_print_methods(s_method_t *methods)
{
    TRACE_D("\t\t\t\t|-- args:");
    ezlopi_print_args(&methods->args);
    TRACE_D("\t\t\t\t|-- name: %s", methods->name);
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
        TRACE_D("\t\t|-- blockType: %s", then_blocks->block_type);
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
        TRACE_D("\t\t|-- blockType: %s", when_blocks->block_type);
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
