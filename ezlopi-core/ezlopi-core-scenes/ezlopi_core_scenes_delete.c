#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS


#include <string.h>

#include "ezlopi_core_scenes_v2.h"

void ezlopi_scenes_delete_user_notifications(l_user_notification_v2_t* user_notifications)
{
    if (user_notifications)
    {
        ezlopi_scenes_delete_user_notifications(user_notifications->next);
        user_notifications->next = NULL;
        free(user_notifications);
    }
}

void ezlopi_scenes_delete_house_modes(l_house_modes_v2_t* house_modes)
{
    if (house_modes)
    {
        ezlopi_scenes_delete_house_modes(house_modes->next);
        house_modes->next = NULL;
        free(house_modes);
    }
}

void ezlopi_scenes_delete_fields(l_fields_v2_t* fields)
{
    if (fields)
    {
        ezlopi_scenes_delete_fields(fields->next);
        if (fields->user_arg)
        {
            free(fields->user_arg);
        }
        fields->next = NULL;
        ezlopi_scenes_delete_field_value(fields);
        free(fields);
    }
}

void ezlopi_scenes_delete_action_blocks(l_action_block_v2_t* action_blocks)
{
    if (action_blocks)
    {
        if (NULL != action_blocks->block_options.cj_function)
        {
            cJSON_Delete(action_blocks->block_options.cj_function);
        }
        ezlopi_scenes_delete_fields(action_blocks->fields);
        ezlopi_scenes_delete_action_blocks(action_blocks->next);
        action_blocks->next = NULL;
        free(action_blocks);
    }
}

void ezlopi_scenes_delete_when_blocks(l_when_block_v2_t* when_blocks)
{
    if (when_blocks)
    {
        if (NULL != when_blocks->block_options.cj_function)
        {
            cJSON_Delete(when_blocks->block_options.cj_function);
        }
        ezlopi_scenes_delete_fields(when_blocks->fields);
        ezlopi_scenes_delete_when_blocks(when_blocks->next);
        when_blocks->next = NULL;
        free(when_blocks);
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

        #warning "void* thread_ctx" need to be freed here;
        ezlopi_scenes_delete(scenes_list->next);
        scenes_list->next = NULL;
        free(scenes_list);
    }
}

// void ezlopi_scenes_delete_by_id(uint32_t _id)
// {
//     ezlopi_scenes_delete(ezlopi_scenes_pop_by_id_v2(_id));
// }

void ezlopi_scenes_delete_field_value(l_fields_v2_t* field)
{
    switch (field->field_value.e_type)
    {
    case VALUE_TYPE_NUMBER:
    {
        field->field_value.u_value.value_double = 0;
        break;
    }
    case VALUE_TYPE_STRING:
    {
        if (field->field_value.u_value.value_string)
        {
            free(field->field_value.u_value.value_string);
            field->field_value.u_value.value_string = NULL;
        }
        break;
    }
    case VALUE_TYPE_BOOL:
    {
        field->field_value.u_value.value_bool = false;
        break;
    }
    case VALUE_TYPE_CJSON:
    {
        if (field->field_value.u_value.cj_value)
        {
            cJSON_Delete(field->field_value.u_value.cj_value);
            field->field_value.u_value.cj_value = NULL;
        }
        break;
    }
    case VALUE_TYPE_BLOCK:
    {
        if (field->field_value.u_value.when_block)
        {
            ezlopi_scenes_delete_when_blocks(field->field_value.u_value.when_block);
            field->field_value.u_value.when_block = NULL;
        }
        break;
    }
    default:
    {
        break;
    }
    }
}
#endif  // CONFIG_EZPI_SERV_ENABLE_MESHBOTS