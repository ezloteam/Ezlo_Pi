#include <string.h>

#include "ezlopi_scenes_v2.h"

void ezlopi_scenes_delete_user_notifications(l_user_notification_v2_t *user_notifications)
{
    if (user_notifications)
    {

        ezlopi_scenes_delete_user_notifications(user_notifications->next);
        user_notifications->next = NULL;
        free(user_notifications);
    }
}

void ezlopi_scenes_delete_house_modes(l_house_modes_v2_t *house_modes)
{
    if (house_modes)
    {
        ezlopi_scenes_delete_house_modes(house_modes->next);
        house_modes->next = NULL;
        free(house_modes);
    }
}

void ezlopi_scenes_delete_fields(l_fields_v2_t *fields)
{
    if (fields)
    {
        ezlopi_scenes_delete_fields(fields->next);
        fields->next = NULL;
        free(fields);
    }
}

void ezlopi_scenes_delete_then_blocks(l_then_block_v2_t *then_blocks)
{
    if (then_blocks)
    {
        ezlopi_scenes_delete_fields(then_blocks->fields);
        ezlopi_scenes_delete_then_blocks(then_blocks->next);
        then_blocks->next = NULL;
        free(then_blocks);
    }
}

void ezlopi_scenes_delete_when_blocks(l_when_block_v2_t *when_blocks)
{
    if (when_blocks)
    {
        ezlopi_scenes_delete_fields(when_blocks->fields);
        ezlopi_scenes_delete_when_blocks(when_blocks->next);
        when_blocks->next = NULL;
        free(when_blocks);
    }
}

void ezlopi_scenes_delete(l_scenes_list_v2_t *scenes_list)
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

// void ezlopi_scenes_delete_by_id(uint32_t _id)
// {
//     ezlopi_scenes_delete(ezlopi_scenes_pop_by_id(_id));
// }