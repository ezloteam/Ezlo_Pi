#include "string.h"

#include "trace.h"
#include "ezlopi_scenes.h"
#include "ezlopi_devices.h"

int ezlopi_scene_when_is_item_state(l_scenes_list_t *curr_scene, void *arg)
{
    int ret = 0;
    // TRACE_W("Warning: when-method not implemented!");
#if 1
    l_when_block_t *when_block = (l_when_block_t *)arg;
    if (when_block)
    {
        uint32_t item_id = 0;
        l_fields_t *value_field = NULL;
#warning "Warning: armed check remains"

        l_fields_t *curr_field = when_block->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, "item", 4))
            {
                item_id = strtoul(curr_field->value.value_string, NULL, 16);
            }
            else if (0 == strncmp(curr_field->name, "value", 4))
            {
                value_field = curr_field;
            }
            curr_field = curr_field->next;
        }

        if (item_id && value_field)
        {
            l_ezlopi_device_t *curr_device = ezlopi_device_get_head();
            while (curr_device)
            {
                l_ezlopi_item_t *curr_item = curr_device->items;
                while (curr_item)
                {
                    TRACE_W("item-id: %08x | %08x", item_id, curr_item->cloud_properties.item_id);
                    if (item_id == curr_item->cloud_properties.item_id)
                    {
                        cJSON *cj_tmp_value = cJSON_CreateObject();
                        if (cj_tmp_value)
                        {
                            TRACE_W("function ptr: %p", curr_item->func);
                            curr_item->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, curr_item, (void *)cj_tmp_value, NULL);

                            cJSON *cj_value = cJSON_GetObjectItem(cj_tmp_value, "value");
                            if (cj_value)
                            {
                                switch (cj_value->type)
                                {
                                case cJSON_True:
                                {
                                    if (true == curr_field->value.value_bool)
                                    {
                                        ret = 1;
                                    }
                                    break;
                                }
                                case cJSON_False:
                                {
                                    if (false == curr_field->value.value_bool)
                                    {
                                        ret = 1;
                                    }
                                    break;
                                }
                                case cJSON_Number:
                                {
                                    if (cj_value->valuedouble == curr_field->value.value_double)
                                    {
                                        ret = 1;
                                    }
                                    break;
                                }
                                case cJSON_String:
                                {
                                    uint32_t cmp_size = (strlen(cj_value->valuestring) > strlen(curr_field->value.value_string)) ? strlen(cj_value->valuestring) : strlen(curr_field->value.value_string);
                                    if (0 == strncmp(cj_value->valuestring, curr_field->value.value_string, cmp_size))
                                    {
                                        ret = 1;
                                    }
                                    break;
                                }
                                default:
                                {
                                    TRACE_E("Value type mis-matched!");
                                }
                                }
                            }

#if 0
                            if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
                            {
                            }
                            else if (EZLOPI_VALUE_TYPE_BOOL == curr_field->value_type)
                            {
                            }
                            else if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type)
                            {
                            }
                            else
                            {
                                TRACE_E("Error: Unknwon value type!");
                            }
#endif

                            cJSON_Delete(cj_tmp_value);
                        }
                        break;
                    }
                    curr_item = curr_item->next;
                }

                curr_device = curr_device->next;
            }
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
