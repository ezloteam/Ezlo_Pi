#include <string.h>

#include "ezlopi_core_scenes_v2.h"

void ezlopi_scenes_delete_user_notifications(l_user_notification_v2_t* user_notifications)
{
    if (user_notifications)
    {
        ezlopi_scenes_delete_user_notifications(user_notifications->next);
        user_notifications->next = NULL;
        free(__FUNCTION__, user_notifications);
    }
}

void ezlopi_scenes_delete_house_modes(l_house_modes_v2_t* house_modes)
{
    if (house_modes)
    {
        ezlopi_scenes_delete_house_modes(house_modes->next);
        house_modes->next = NULL;
        free(__FUNCTION__, house_modes);
    }
}

void ezlopi_scenes_delete_fields(l_fields_v2_t* fields)
{
    if (fields)
    {

        ezlopi_scenes_delete_fields(fields->next);
        fields->next = NULL;
        ezlopi_scenes_delete_field_value(fields);
        free(__FUNCTION__, fields);
    }
}

void ezlopi_scenes_delete_action_blocks(l_action_block_v2_t* action_blocks)
{
    if (action_blocks)
    {
        ezlopi_scenes_delete_fields(action_blocks->fields);
        ezlopi_scenes_delete_action_blocks(action_blocks->next);
        action_blocks->next = NULL;
        free(__FUNCTION__, action_blocks);
    }
}

void ezlopi_scenes_delete_when_blocks(l_when_block_v2_t* when_blocks)
{
    if (when_blocks)
    {
        ezlopi_scenes_delete_fields(when_blocks->fields);
        ezlopi_scenes_delete_when_blocks(when_blocks->next);
        when_blocks->next = NULL;
        free(__FUNCTION__, when_blocks);
    }
}

void ezlopi_scenes_delete(l_scenes_list_v2_t* scenes_list)
{
    if (scenes_list)
    {
        ezlopi_scenes_delete_user_notifications(scenes_list->user_notifications);
        ezlopi_scenes_delete_house_modes(scenes_list->house_modes);
        ezlopi_scenes_delete_action_blocks(scenes_list->then_block);
        ezlopi_scenes_delete_action_blocks(scenes_list->else_block);
        ezlopi_scenes_delete_when_blocks(scenes_list->when_block);

        ezlopi_scenes_delete(scenes_list->next);
        scenes_list->next = NULL;
        free(__FUNCTION__, scenes_list);
    }
}

// void ezlopi_scenes_delete_by_id(uint32_t _id)
// {
//     ezlopi_scenes_delete(ezlopi_scenes_pop_by_id_v2(_id));
// }

void ezlopi_scenes_delete_field_value(l_fields_v2_t* field)
{
    switch (field->field_value.e_type)
        // switch (field->value.type)
    {
    case VALUE_TYPE_NUMBER:
    {
        field->field_value.u_value.value_double = 0;
        // field->field_value.u_value.value_double = 0;
        break;
    }
    case VALUE_TYPE_STRING:
    {
        if (field->field_value.u_value.value_string)
            // if (field->field_value.u_value.value_string)
        {
            free(__FUNCTION__, field->field_value.u_value.value_string);
            field->field_value.u_value.value_string = NULL;

            // free(__FUNCTION__, field->field_value.u_value.value_string);
            // field->field_value.u_value.value_string = NULL;
        }
        break;
    }
    case VALUE_TYPE_BOOL:
    {
        field->field_value.u_value.value_bool = false;
        // field->field_value.u_value.value_bool = false;
        break;
    }
    case VALUE_TYPE_CJSON:
    {
        if (field->field_value.u_value.cj_value)
            // if (field->field_value.u_value.cj_value)
        {
            cJSON_Delete(__FUNCTION__, field->field_value.u_value.cj_value);
            field->field_value.u_value.cj_value = NULL;

            // cJSON_Delete(__FUNCTION__, field->field_value.u_value.cj_value);
            // field->field_value.u_value.cj_value = NULL;
        }
        break;
    }
    case VALUE_TYPE_BLOCK:
    {
        if (field->field_value.u_value.when_block)
            // if (field->value.when_block)
        {
            ezlopi_scenes_delete_when_blocks(field->field_value.u_value.when_block);
            field->field_value.u_value.when_block = NULL;

            // ezlopi_scenes_delete_when_blocks(field->value.when_block);
            // field->value.when_block = NULL;
        }
        break;
    }
    default:
    {
        break;
    }
    }
}