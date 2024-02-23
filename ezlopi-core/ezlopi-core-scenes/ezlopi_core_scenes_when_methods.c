
#include "ezlopi_util_trace.h"
#include "ezlopi_cloud_constants.h"

#include "ezlopi_core_modes.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_scenes_operators.h"
#include "ezlopi_core_scenes_when_methods.h"

int ezlopi_scene_when_is_item_state(l_scenes_list_v2_t* scene_node, void* arg)
{
    int ret = 0;
    l_when_block_v2_t* when_block = (l_when_block_v2_t*)arg;
    if (when_block)
    {
        uint32_t item_id = 0;
        l_fields_v2_t* value_field = NULL;
        #warning "Warning: armed check remains"

            l_fields_v2_t* curr_field = when_block->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, "item", 4))
            {
                item_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
            }
            else if (0 == strncmp(curr_field->name, ezlopi_value_str, 4))
            {
                value_field = curr_field;
            }
            curr_field = curr_field->next;
        }

        if (item_id && value_field)
        {
            l_ezlopi_device_t* curr_device = ezlopi_device_get_head();
            while (curr_device)
            {
                l_ezlopi_item_t* curr_item = curr_device->items;
                while (curr_item)
                {
                    if (item_id == curr_item->cloud_properties.item_id)
                    {
                        cJSON* cj_tmp_value = cJSON_CreateObject();
                        if (cj_tmp_value)
                        {
                            curr_item->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, curr_item, (void*)cj_tmp_value, NULL);
                            cJSON* cj_value = cJSON_GetObjectItem(cj_tmp_value, ezlopi_value_str);
                            if (cj_value)
                            {
                                switch (cj_value->type)
                                {
                                case cJSON_True:
                                {
                                    if (true == value_field->field_value.u_value.value_bool)
                                    {
                                        ret = 1;
                                    }
                                    break;
                                }
                                case cJSON_False:
                                {
                                    if (false == value_field->field_value.u_value.value_bool)
                                    {
                                        ret = 1;
                                    }
                                    break;
                                }
                                case cJSON_Number:
                                {
                                    if (cj_value->valuedouble == value_field->field_value.u_value.value_double)
                                    {
                                        ret = 1;
                                    }
                                    break;
                                }
                                case cJSON_String:
                                {
                                    uint32_t cmp_size = (strlen(cj_value->valuestring) > strlen(value_field->field_value.u_value.value_string)) ? strlen(cj_value->valuestring) : strlen(value_field->field_value.u_value.value_string);
                                    if (0 == strncmp(cj_value->valuestring, value_field->field_value.u_value.value_string, cmp_size))
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

    return ret;
}

int ezlopi_scene_when_is_interval(l_scenes_list_v2_t* scene_node, void* arg)
{
    int ret = 0;

    if (scene_node)
    {
        char* end_prt = NULL;
        uint32_t interval = strtoul(scene_node->when_block->fields->field_value.u_value.value_string, &end_prt, 10);
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
        if (scene_node->when_block->fields->user_arg)
        {
            last_update = (uint32_t)scene_node->when_block->fields->user_arg;

            if (((xTaskGetTickCount() - last_update) / 1000) > interval)
            {
                scene_node->when_block->fields->user_arg = (void*)xTaskGetTickCount();
                ret = 1;
            }
        }
        else
        {
            ret = 1;
            scene_node->when_block->fields->user_arg = (void*)xTaskGetTickCount();
        }
    }

    return ret;
}

int ezlopi_scene_when_is_item_state_changed(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W("Warning: when-method 'is_item_state_changed' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_button_state(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W("Warning: when-method 'is_button_state' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_sun_state(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W("Warning: when-method 'is_sun_state' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_date(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W("Warning: when-method 'is_date' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_once(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W("Warning: when-method 'is_once' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_date_range(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W("Warning: when-method 'is_date_range' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_user_lock_operation(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W("Warning: when-method 'is_user_lock_operation' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_house_mode_changed_to(l_scenes_list_v2_t* scene_node, void* arg)
{
    int ret = 0;
    l_when_block_v2_t* when_block = (l_when_block_v2_t*)arg;

    if (when_block)
    {
        l_fields_v2_t* house_mode_id_array = NULL;
        l_fields_v2_t* curr_field = when_block->fields;

        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, ezlopi_houseMode_str, strlen(ezlopi_houseMode_str)))
            {
                house_mode_id_array = curr_field;
            }

            curr_field = curr_field->next;
        }

        uint32_t idx = 0;
        cJSON* cj_house_mdoe_id = NULL;

        while (NULL == (cj_house_mdoe_id = cJSON_GetArrayItem(house_mode_id_array->field_value.u_value.cj_value, idx++)))
        {
            if (cj_house_mdoe_id->valuestring)
            {
                uint32_t house_mode_id = strtoul(cj_house_mdoe_id->valuestring, NULL, 16);
                s_ezlopi_modes_t* modes = ezlopi_core_modes_get_custom_modes();
                if ((modes->current_mode_id == house_mode_id) && ((uint32_t)house_mode_id_array->user_arg != modes->current_mode_id))
                {
                    ret = 1;
                    house_mode_id_array->user_arg = (void*)house_mode_id;
                    TRACE_E("house-mode-changed-to: %d", house_mode_id);
                }
            }
        }
    }

    return ret;
}

int ezlopi_scene_when_is_house_mode_changed_from(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W("Warning: when-method 'is_house_mode_changed_from' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_device_state(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W("Warning: when-method 'is_device_state' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_network_state(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W("Warning: when-method 'is_network_state' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_scene_state(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W("Warning: when-method 'is_scene_state' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_group_state(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W("Warning: when-method 'is_group_state' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_cloud_state(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W("Warning: when-method 'is_cloud_state' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_battery_state(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W("Warning: when-method 'is_battery_state' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_battery_level(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W("Warning: when-method 'is_battery_level' not implemented!");
    return 0;
}

int ezlopi_scene_when_compare_numbers(l_scenes_list_v2_t* scene_node, void* arg)
{
    int ret = 0;
    l_when_block_v2_t* when_block = (l_when_block_v2_t*)arg;

    if (when_block && scene_node)
    {
        uint32_t item_id = 0;
        l_fields_v2_t* value_field = NULL;
        l_fields_v2_t* comparator_field = NULL;

        l_fields_v2_t* curr_field = when_block->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, "item", 4))
            {
                item_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
            }
            else if (0 == strncmp(curr_field->name, ezlopi_value_str, 4))
            {
                value_field = curr_field;
            }
            else if (0 == strncmp(curr_field->name, "comparator", 10))
            {
                comparator_field = curr_field;
            }
            curr_field = curr_field->next;
        }

        if (item_id && value_field && comparator_field)
        {
            ret = ezlopi_scenes_operators_value_number_operations(item_id, value_field, comparator_field);
        }
    }

    return ret;
}

int ezlopi_scene_when_compare_number_range(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W("Warning: when-method 'number_range' not implemented!");
    return 0;
}

int ezlopi_scene_when_compare_strings(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W("Warning: when-method 'compare_strings' not implemented!");
    return 0;
}

int ezlopi_scene_when_string_operation(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W("Warning: when-method 'string_operation' not implemented!");
    return 0;
}

int ezlopi_scene_when_in_array(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W("Warning: when-method 'in_array' not implemented!");
    return 0;
}

int ezlopi_scene_when_compare_values(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W("Warning: when-method 'compare_values' not implemented!");
    return 0;
}

int ezlopi_scene_when_has_atleast_one_dictionary_value(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W("Warning: when-method 'has_atleast_one_dictionary_value' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_firmware_update_state(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W("Warning: when-method 'is_firmware_update_state' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_dictionary_changed(l_scenes_list_v2_t* scene_node, void* arg)
{
   int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (scene_node && when_block)
    {
        uint32_t item_id = 0;
        l_fields_v2_t *key_field = NULL;
        l_fields_v2_t *operation_field = NULL;

        l_fields_v2_t *curr_field = when_block->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, "item", 5))
            {
                item_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
            }
            else if (0 == strncmp(curr_field->name, "key", 4))
            {
                key_field = curr_field; // this contains "options [array]" & 'value': to be checked
            }
            else if (0 == strncmp(curr_field->name, "operation", 10))
            {
                operation_field = curr_field; // this contains "options [array]" & 'value': to be checked
            }
            curr_field = curr_field->next;
        }

        if (item_id && key_field && operation_field)
        {
            ret = ezlopi_scenes_operators_is_dictionary_changed_operations(scene_node, item_id, key_field, operation_field);
        }
    }
    return ret;
}

int ezlopi_scene_when_is_detected_in_hot_zone(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W("Warning: when-method 'is_detected_in_hot_zone' not implemented!");
    return 0;
}

int ezlopi_scene_when_and(l_scenes_list_v2_t* scene_node, void* arg)
{
    int ret = 0;
    l_when_block_v2_t* when_block = (l_when_block_v2_t*)arg;
    if (when_block)
    {
        l_when_block_v2_t* value_when_block = when_block->fields->field_value.u_value.when_block;
        while (value_when_block)
        {
            f_scene_method_v2_t scene_method = ezlopi_scene_get_method_v2(value_when_block->block_options.method.type);
            if (scene_method)
            {
                ret &= scene_method(scene_node, (void*)value_when_block);
                if (!ret)
                {
                    break;
                }
            }

            value_when_block = value_when_block->next;
        }
    }

    return ret;
}

int ezlopi_scene_when_not(l_scenes_list_v2_t* scene_node, void* arg)
{
    int ret = 0;
    l_when_block_v2_t* when_block = (l_when_block_v2_t*)arg;
    if (when_block)
    {
        ret = 1; // required for the first case
        l_when_block_v2_t* value_when_block = when_block->fields->field_value.u_value.when_block;
        while (value_when_block)
        {
            f_scene_method_v2_t scene_method = ezlopi_scene_get_method_v2(value_when_block->block_options.method.type);
            if (scene_method)
            {
                // iterate through all '_when_blocks_'
                ret = !(scene_method(scene_node, (void*)value_when_block)); // if all the block-calls are false, then return 1;
            }

            value_when_block = value_when_block->next;
        }
    }
    return ret;
}

int ezlopi_scene_when_or(l_scenes_list_v2_t* scene_node, void* arg)
{
    int ret = 0;
    l_when_block_v2_t* when_block = (l_when_block_v2_t*)arg;
    if (when_block)
    {
        l_when_block_v2_t* value_when_block = when_block->fields->field_value.u_value.when_block;
        while (value_when_block)
        {
            f_scene_method_v2_t scene_method = ezlopi_scene_get_method_v2(value_when_block->block_options.method.type);
            if (scene_method)
            {
                ret |= scene_method(scene_node, (void*)value_when_block);
                if (ret)
                {
                    break;
                }
            }

            value_when_block = value_when_block->next;
        }
    }

    return ret;
}

int ezlopi_scene_when_xor(l_scenes_list_v2_t* scene_node, void* arg)
{
    int ret = 0;
    l_when_block_v2_t* when_block = (l_when_block_v2_t*)arg;
    if (when_block)
    {
        l_when_block_v2_t* value_when_block = when_block->fields->field_value.u_value.when_block;
        while (value_when_block)
        {
            f_scene_method_v2_t scene_method = ezlopi_scene_get_method_v2(value_when_block->block_options.method.type);
            if (scene_method)
            {
                // iterate through all the '_when_blocks_'
                ret ^= scene_method(scene_node, (void*)value_when_block);
                // return 1 ; if odd no of '_when_block_' conditions are true
            }

            value_when_block = value_when_block->next;
        }
    }

    return ret;
}

int ezlopi_scene_when_function(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W("Warning: when-method not implemented!");
    return 0;
}
