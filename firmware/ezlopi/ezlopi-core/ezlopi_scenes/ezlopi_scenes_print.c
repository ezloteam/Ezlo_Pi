#include "ezlopi_scenes.h"

void ezlopi_print_args(s_args_t *args)
{
    TRACE_D("|-------------- item: %s", args->item);
    TRACE_D("|-------------- value: %s", args->value);
}

void ezlopi_print_methods(s_method_t *methods)
{
    TRACE_D("|----------- args:");
    ezlopi_print_args(&methods->args);
    TRACE_D("|----------- name: %s", methods->name);
}

void ezlopi_print_block_options(s_block_options_t *block_options)
{
    TRACE_D("|----- blockOptions:");
    TRACE_D("|-------- method");
    ezlopi_print_methods(&block_options->method);
}

void ezlopi_print_fields(l_fields_t *fields)
{
    TRACE_D("|----- fields: ");
    while (fields)
    {
        TRACE_D("---------------------------------------------------------------");
        TRACE_D("|-------- name: %s", fields->name);
        TRACE_D("|-------- type: %s", fields->name);
        if (fields->value_type)
        {
            TRACE_D("|-------- value: %d", fields->value.value_double);
        }
        else
        {
            TRACE_D("|-------- value: %s", fields->value.value_string);
        }
        TRACE_D("---------------------------------------------------------------");
        fields = fields->next;
    }
}

void ezlopi_print_then_blocks(l_then_block_t *then_blocks)
{
    TRACE_D("|-- then: ");
    while (then_blocks)
    {
        TRACE_D("|---------------------------------------------");
        ezlopi_print_block_options(&then_blocks->block_options);
        TRACE_D("|----- blockType: %s", then_blocks->block_type);
        ezlopi_print_fields(then_blocks->fields);
        TRACE_D("|---------------------------------------------");

        then_blocks = then_blocks->next;
    }
}

void ezlopi_print_house_modes(l_house_modes_t *house_modes)
{
    TRACE_D("|-- house_modes: ");
    while (house_modes)
    {
        TRACE_D("|----- %s", house_modes->house_mode);
        house_modes = house_modes->next;
    }
}

void ezlopi_print_user_notifications(l_user_notification_t *user_notification)
{
    TRACE_D("|-- user_notifications: ");
    while (user_notification)
    {
        TRACE_D("|----- %s", user_notification->user_id);
        user_notification = user_notification->next;
    }
}

void ezlopi_print_when_blocks(l_when_block_t *when_blocks)
{
    TRACE_D("|-- when: ");
    while (when_blocks)
    {
        TRACE_D("|---------------------------------------------");
        ezlopi_print_block_options(&when_blocks->block_options);
        TRACE_D("|----- blockType: %s", when_blocks->block_type);
        ezlopi_print_fields(when_blocks->fields);
        TRACE_D("|---------------------------------------------");
        when_blocks = when_blocks->next;
    }
}

void ezlopi_scenes_print(l_scenes_list_t *scene_link_list)
{
    while (scene_link_list)
    {
        TRACE_D("---------------------------------------------------------------");
        TRACE_D("|-- enabled: %d", scene_link_list->enabled);
        TRACE_D("|-- group_id: %s", scene_link_list->group_id);
        TRACE_D("|-- is_group: %d", scene_link_list->is_group);
        TRACE_D("|-- name: %s", scene_link_list->name);
        TRACE_D("|-- parent_id: %s", scene_link_list->parent_id);
        ezlopi_print_user_notifications(scene_link_list->user_notifications);
        ezlopi_print_house_modes(scene_link_list->house_modes);
        ezlopi_print_then_blocks(scene_link_list->then);
        ezlopi_print_when_blocks(scene_link_list->when);

        TRACE_D("---------------------------------------------------------------");
        scene_link_list = scene_link_list->next;
    }
}
