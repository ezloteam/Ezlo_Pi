#include "string.h"

#include "ezlopi_scenes.h"

void ezlopi_scenes_delete_user_notifications(l_user_notification_t *user_notifications)
{
    if (user_notifications)
    {

        ezlopi_scenes_delete_user_notifications(user_notifications->next);
        user_notifications->next = NULL;
        free(user_notifications);
    }
}

void ezlopi_scenes_delete_house_modes(l_house_modes_t *house_modes)
{
    if (house_modes)
    {
        ezlopi_scenes_delete_house_modes(house_modes->next);
        house_modes->next = NULL;
        free(house_modes);
    }
}

void ezlopi_scenes_delete_fields(l_fields_t *fields)
{
    if (fields)
    {
        ezlopi_scenes_delete_fields(fields->next);
        fields->next = NULL;
        free(fields);
    }
}

void ezlopi_scenes_delete_then_blocks(l_then_block_t *then_blocks)
{
    if (then_blocks)
    {
        ezlopi_scenes_delete_fields(then_blocks->fields);
        ezlopi_scenes_delete_then_blocks(then_blocks->next);
        then_blocks->next = NULL;
        free(then_blocks);
    }
}

void ezlopi_scenes_delete_when_blocks(l_when_block_t *when_blocks)
{
    if (when_blocks)
    {
        ezlopi_scenes_delete_fields(when_blocks->fields);
        ezlopi_scenes_delete_when_blocks(when_blocks->next);
        when_blocks->next = NULL;
        free(when_blocks);
    }
}

void ezlopi_scenes_delete(l_scenes_list_t *scenes_list)
{
    if (scenes_list)
    {
        ezlopi_scenes_delete_user_notifications(scenes_list->user_notifications);
        ezlopi_scenes_delete_house_modes(scenes_list->house_modes);
        ezlopi_scenes_delete_then_blocks(scenes_list->then);

        ezlopi_scenes_delete(scenes_list->next);
        scenes_list->next = NULL;
        free(scenes_list);
    }
}