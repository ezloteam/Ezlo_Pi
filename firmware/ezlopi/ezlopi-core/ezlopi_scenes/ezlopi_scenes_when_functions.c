#include "string.h"

#include "trace.h"
#include "ezlopi_scenes.h"
#include "ezlopi_devices.h"

int ezlopi_scene_when_is_item_state(l_scenes_list_t *curr_scene, void *arg)
{
    int ret = 0;
    TRACE_W("Warning: when-method not implemented!");
#if 0
    l_when_block_t *when_block = a_when_block;
    if (when_block)
    {
        uint32_t item_id = 0;

        l_fields_t *curr_field = when_block->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, "item", 4))
            {
            }
            else if (1)
            {
            }
            curr_field = curr_field->next;
        }
    }
#endif

    return ret;
}

int ezlopi_scene_when_is_interval(l_scenes_list_t *curr_scene, void *arg)
{
    int ret = 0;

    if (curr_scene)
    {
        char *end_prt = NULL;
        uint32_t interval = strtoul(curr_scene->when->fields->value.value_string, &end_prt, 10);
        if (end_prt)
        {
            if (0 == strncmp(end_prt, "m", 1))
            {
                interval *= 60;
            }
            else if (0 == strncmp(end_prt, "h", 1))
            {
                interval *= (60 * 60);
            }
            else if (0 == strncmp(end_prt, "d", 1))
            {
                interval *= (60 * 60 * 24);
            }
        }

        uint32_t last_update = 0;
        if (curr_scene->when->fields->user_arg)
        {
            last_update = (uint32_t)curr_scene->when->fields->user_arg;

            if (((xTaskGetTickCount() - last_update) / 1000) > interval)
            {
                curr_scene->when->fields->user_arg = xTaskGetTickCount();
                ret = 1;
            }
        }
        else
        {
            ret = 1;
            curr_scene->when->fields->user_arg = xTaskGetTickCount();
        }
    }

    return ret;
}

int ezlopi_scene_when_is_item_state_changed(l_scenes_list_t *curr_scene, void *arg)
{
    TRACE_W("Warning: when-method not implemented!");
    return 0;
}

int ezlopi_scene_when_is_button_state(l_scenes_list_t *curr_scene, void *arg)
{
    TRACE_W("Warning: when-method not implemented!");
    return 0;
}

int ezlopi_scene_when_is_sun_state(l_scenes_list_t *curr_scene, void *arg)
{
    TRACE_W("Warning: when-method not implemented!");
    return 0;
}

int ezlopi_scene_when_is_date(l_scenes_list_t *curr_scene, void *arg)
{
    TRACE_W("Warning: when-method not implemented!");
    return 0;
}

int ezlopi_scene_when_is_once(l_scenes_list_t *curr_scene, void *arg)
{
    TRACE_W("Warning: when-method not implemented!");
    return 0;
}

int ezlopi_scene_when_is_date_range(l_scenes_list_t *curr_scene, void *arg)
{
    TRACE_W("Warning: when-method not implemented!");
    return 0;
}

int ezlopi_scene_when_is_user_lock_operation(l_scenes_list_t *curr_scene, void *arg)
{
    TRACE_W("Warning: when-method not implemented!");
    return 0;
}

int ezlopi_scene_when_is_house_mode_changed_to(l_scenes_list_t *curr_scene, void *arg)
{
    TRACE_W("Warning: when-method not implemented!");
    return 0;
}

int ezlopi_scene_when_is_house_mode_changed_from(l_scenes_list_t *curr_scene, void *arg)
{
    TRACE_W("Warning: when-method not implemented!");
    return 0;
}

int ezlopi_scene_when_is_device_state(l_scenes_list_t *curr_scene, void *arg)
{
    TRACE_W("Warning: when-method not implemented!");
    return 0;
}

int ezlopi_scene_when_is_network_state(l_scenes_list_t *curr_scene, void *arg)
{
    TRACE_W("Warning: when-method not implemented!");
    return 0;
}

int ezlopi_scene_when_is_scene_state(l_scenes_list_t *curr_scene, void *arg)
{
    TRACE_W("Warning: when-method not implemented!");
    return 0;
}

int ezlopi_scene_when_is_group_state(l_scenes_list_t *curr_scene, void *arg)
{
    TRACE_W("Warning: when-method not implemented!");
    return 0;
}

int ezlopi_scene_when_is_cloud_state(l_scenes_list_t *curr_scene, void *arg)
{
    TRACE_W("Warning: when-method not implemented!");
    return 0;
}

int ezlopi_scene_when_is_battery_state(l_scenes_list_t *curr_scene, void *arg)
{
    TRACE_W("Warning: when-method not implemented!");
    return 0;
}

int ezlopi_scene_when_is_battery_level(l_scenes_list_t *curr_scene, void *arg)
{
    TRACE_W("Warning: when-method not implemented!");
    return 0;
}

int ezlopi_scene_when_compare_numbers(l_scenes_list_t *curr_scene, void *arg)
{
    TRACE_W("Warning: when-method not implemented!");
    return 0;
}

int ezlopi_scene_when_compare_number_range(l_scenes_list_t *curr_scene, void *arg)
{
    TRACE_W("Warning: when-method not implemented!");
    return 0;
}

int ezlopi_scene_when_compare_strings(l_scenes_list_t *curr_scene, void *arg)
{
    TRACE_W("Warning: when-method not implemented!");
    return 0;
}

int ezlopi_scene_when_string_operation(l_scenes_list_t *curr_scene, void *arg)
{
    TRACE_W("Warning: when-method not implemented!");
    return 0;
}

int ezlopi_scene_when_in_array(l_scenes_list_t *curr_scene, void *arg)
{
    TRACE_W("Warning: when-method not implemented!");
    return 0;
}

int ezlopi_scene_when_compare_values(l_scenes_list_t *curr_scene, void *arg)
{
    TRACE_W("Warning: when-method not implemented!");
    return 0;
}

int ezlopi_scene_when_has_atleast_one_dictionary_value(l_scenes_list_t *curr_scene, void *arg)
{
    TRACE_W("Warning: when-method not implemented!");
    return 0;
}

int ezlopi_scene_when_is_firmware_update_state(l_scenes_list_t *curr_scene, void *arg)
{
    TRACE_W("Warning: when-method not implemented!");
    return 0;
}

int ezlopi_scene_when_is_dictionary_changed(l_scenes_list_t *curr_scene, void *arg)
{
    TRACE_W("Warning: when-method not implemented!");
    return 0;
}

int ezlopi_scene_when_is_detected_in_hot_zone(l_scenes_list_t *curr_scene, void *arg)
{
    TRACE_W("Warning: when-method not implemented!");
    return 0;
}

int ezlopi_scene_when_and(l_scenes_list_t *curr_scene, void *arg)
{
    TRACE_W("Warning: when-method not implemented!");
    return 0;
}

int ezlopi_scene_when_not(l_scenes_list_t *curr_scene, void *arg)
{
    TRACE_W("Warning: when-method not implemented!");
    return 0;
}

int ezlopi_scene_when_or(l_scenes_list_t *curr_scene, void *arg)
{
    TRACE_W("Warning: when-method not implemented!");
    return 0;
}

int ezlopi_scene_when_xor(l_scenes_list_t *curr_scene, void *arg)
{
    TRACE_W("Warning: when-method not implemented!");
    return 0;
}

int ezlopi_scene_when_function(l_scenes_list_t *curr_scene, void *arg)
{
    TRACE_W("Warning: when-method not implemented!");
    return 0;
}
