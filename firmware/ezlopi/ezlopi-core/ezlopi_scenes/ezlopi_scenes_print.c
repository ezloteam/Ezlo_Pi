#include "trace.h"
#include "ezlopi_scenes.h"

void ezlopi_print_block_options(s_block_options_t *block_options, l_fields_t *fields)
{
    TRACE_D("\t\t|-- blockOptions:");
    TRACE_D("\t\t\t|-- method");
    TRACE_D("\t\t\t\t|-- name: %s", block_options->method.name);
    TRACE_D("\t\t\t\t|-- args:");

    while (fields)
    {
        TRACE_D("\t\t\t\t\t|-- %s: %s", fields->name, fields->name);
        fields = fields->next;
    }
}

void ezlopi_print_fields(l_fields_t *fields)
{
    TRACE_D("\t\t|-- fields: ");
    int field_count = 0;
    while (fields)
    {
        TRACE_D("\t\t\t|---------- field_count: %d ----------", ++field_count);
        TRACE_D("\t\t\t|-- name: %s", fields->name);
        if (SCENE_VALUE_TYPE_INT == fields->value_type)
        {
            TRACE_D("\t\t\t|-- type: int");
            TRACE_D("\t\t\t|-- value: %f", fields->value.value_double);
        }
        else if (SCENE_VALUE_TYPE_ITEM == fields->value_type)
        {
            TRACE_D("\t\t\t|-- type: item");
            TRACE_D("\t\t\t|-- value: %s", fields->value.value_string);
        }
        else if (SCENE_VALUE_TYPE_BOOL == fields->value_type)
        {
            TRACE_D("\t\t\t|-- type: bool");
            TRACE_D("\t\t\t|-- value: %s", (0 == fields->value.value_double) ? "false" : "true");
        }
        else
        {
            TRACE_E("\t\t\t|- value type: %d", fields->value_type);
        }

        fields = fields->next;
        if (fields)
        {
            TRACE_D("");
        }
    }
    TRACE_D("\t\t\t|------------------------------------");
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
        ezlopi_print_block_options(&when_blocks->block_options, when_blocks->fields);
        TRACE_D("\t\t|-- blockType: when");
        ezlopi_print_fields(when_blocks->fields);
        when_blocks = when_blocks->next;
    }
}

void ezlopi_print_then_blocks(l_then_block_t *then_blocks)
{
    TRACE_D("\t|-- then: ");
    while (then_blocks)
    {
        ezlopi_print_block_options(&then_blocks->block_options, then_blocks->fields);
        TRACE_D("\t\t|-- blockType: then");
        ezlopi_print_fields(then_blocks->fields);

        then_blocks = then_blocks->next;
        if (then_blocks)
        {
            TRACE_D("\t\t|--");
        }
    }
}

void ezlopi_scenes_print(l_scenes_list_t *scene_link_list)
{
    int scene_count = 0;
    while (scene_link_list)
    {
        TRACE_D("\t----------------------- scene_count: %d ------------------------", ++scene_count);
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
