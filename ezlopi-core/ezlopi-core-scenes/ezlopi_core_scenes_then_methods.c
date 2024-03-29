#include "ezlopi_util_trace.h"

#include "ezlopi_core_devices.h"
#include "ezlopi_core_scenes_scripts.h"
#include "ezlopi_core_scenes_then_methods.h"
#include "ezlopi_service_meshbot.h"

#include "ezlopi_cloud_constants.h"

int ezlopi_scene_then_set_item_value(l_scenes_list_v2_t* curr_scene, void* arg)
{
    int ret = 0;
    uint32_t item_id = 0;
    cJSON* cj_params = cJSON_CreateObject();

    if (cj_params)
    {
        l_action_block_v2_t* curr_then = (l_action_block_v2_t*)arg;
        if (curr_then)
        {
            l_fields_v2_t* curr_field = curr_then->fields;
            while (curr_field)
            {
                if (0 == strncmp(curr_field->name, "item", 4))
                {
                    cJSON_AddStringToObject(cj_params, ezlopi__id_str, curr_field->field_value.u_value.value_string);
                    item_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
                    TRACE_D("item_id: %s", curr_field->field_value.u_value.value_string);

                    // cJSON_AddStringToObject(cj_params, ezlopi__id_str, curr_field->field_value.u_value.value_string);
                    // item_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
                    // TRACE_D("item_id: %s", curr_field->field_value.u_value.value_string);
                }
                else if (0 == strncmp(curr_field->name, ezlopi_value_str, 5))
                {
                    if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
                    {
                        cJSON_AddNumberToObject(cj_params, ezlopi_value_str, curr_field->field_value.u_value.value_double);
                        TRACE_D("value: %f", curr_field->field_value.u_value.value_double);
                    }
                    else if (EZLOPI_VALUE_TYPE_BOOL == curr_field->value_type)
                    {
                        cJSON_AddBoolToObject(cj_params, ezlopi_value_str, curr_field->field_value.u_value.value_bool);
                        TRACE_D("value: %s", curr_field->field_value.u_value.value_bool ? ezlopi_true_str : ezlopi_false_str);
                    }
                    else if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type)
                    {
                        cJSON_AddStringToObject(cj_params, ezlopi_value_str, curr_field->field_value.u_value.value_string);
                        TRACE_D("value: %s", curr_field->field_value.u_value.value_string);
                    }
                }

                curr_field = curr_field->next;
            }

            l_ezlopi_device_t* curr_device = ezlopi_device_get_head();
            uint32_t found_item = 0;
            while (curr_device)
            {
                l_ezlopi_item_t* curr_item = curr_device->items;
                while (curr_item)
                {
                    if (item_id == curr_item->cloud_properties.item_id)
                    {
                        curr_item->func(EZLOPI_ACTION_SET_VALUE, curr_item, cj_params, curr_item->user_arg);
                        found_item = 1;
                        break;
                    }
                    curr_item = curr_item->next;
                }
                if (found_item)
                {
                    TRACE_D("\r\n\r\n FOUND DEVICE \r\n\r\n");
                    break;
                }
                curr_device = curr_device->next;
            }
        }

        cJSON_Delete(cj_params);
    }

    return ret;
}
int ezlopi_scene_then_set_device_armed(l_scenes_list_v2_t* curr_scene, void* arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_send_cloud_abstract_command(l_scenes_list_v2_t* curr_scene, void* arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_switch_house_mode(l_scenes_list_v2_t* curr_scene, void* arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_send_http_request(l_scenes_list_v2_t* curr_scene, void* arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_run_custom_script(l_scenes_list_v2_t* curr_scene, void* arg)
{
    int ret = 0;

    uint32_t script_id = 0;
    l_action_block_v2_t* curr_then = (l_action_block_v2_t*)arg;
    if (curr_then)
    {
        l_fields_v2_t* curr_field = curr_then->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, "script", 7))
            {
                script_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
                TRACE_S("script_id: %s", curr_field->field_value.u_value.value_string);
            }
            curr_field = curr_field->next;
        }
    }

    if (script_id)
    {
        ret = 1;
        ezlopi_scenes_scripts_run_by_id(script_id);
    }

    return ret;
}
int ezlopi_scene_then_run_plugin_script(l_scenes_list_v2_t* curr_scene, void* arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_run_scene(l_scenes_list_v2_t* curr_scene, void* arg)
{
    int ret = 0;
    uint32_t sceneID = 0;
    bool execute_else_condition = false;
    l_action_block_v2_t* curr_then = (l_action_block_v2_t*)arg;
    if (curr_then)
    {
        l_fields_v2_t* curr_field = curr_then->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, "sceneId", 7))
            {
                if (curr_field->field_value.e_type == VALUE_TYPE_STRING)
                {
                    sceneID = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
                }
                else
                {
                    ret = 1;
                }
            }
            else if (0 == strncmp(curr_field->name, "block", 5))
            {
                if (curr_field->field_value.e_type == VALUE_TYPE_STRING)
                {
                    if (0 == strncmp(curr_field->field_value.u_value.value_string, "else", 4))
                    {
                        execute_else_condition = true;
                    }
                    else if (0 == strncmp(curr_field->field_value.u_value.value_string, "thenGroups", 10))
                    {
                        TRACE_D("Running scene group, yet to be implemented.");
                    }
                    else
                    {
                        ret = 1;
                    }
                }
                else
                {
                    ret = 1;
                }
            }
            else if ((0 == strncmp(curr_field->name, "group", 5)) && (curr_field->value_type == EZLOPI_VALUE_TYPE_STRING))
            {
                TRACE_D("Running scene group, yet to be implemented.");
            }
            curr_field = curr_field->next;
        }
        if (execute_else_condition)
        {
            TRACE_D("Executing else condition");
            ezlopi_meshbot_execute_scene_else_action_group(sceneID);
        }
        else
        {
            TRACE_D("Executing scene, id: %d", sceneID);
            ezlopi_scenes_service_run_by_id(sceneID);
        }
    }
    else
    {
        ret = 1;
    }
    return ret;
}
int ezlopi_scene_then_set_scene_state(l_scenes_list_v2_t* curr_scene, void* arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_reset_latch(l_scenes_list_v2_t* curr_scene, void* arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_reset_scene_latches(l_scenes_list_v2_t* curr_scene, void* arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_reboot_hub(l_scenes_list_v2_t* curr_scene, void* arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_reset_hub(l_scenes_list_v2_t* curr_scene, void* arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_cloud_api(l_scenes_list_v2_t* curr_scene, void* arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_set_expression(l_scenes_list_v2_t* curr_scene, void* arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_set_variable(l_scenes_list_v2_t* curr_scene, void* arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_toggle_value(l_scenes_list_v2_t* curr_scene, void* arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}