#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include "ezlopi_util_trace.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_http.h"
#include "ezlopi_core_modes.h"
#include "ezlopi_core_reset.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_scenes_v2.h"
#include "ezlopi_core_event_group.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_scenes_edit.h"
#include "ezlopi_core_scenes_scripts.h"
#include "ezlopi_core_scenes_expressions.h"
#include "ezlopi_core_scenes_then_methods.h"
#include "ezlopi_core_scenes_status_changed.h"
#include "ezlopi_core_scenes_then_methods_helper_func.h"
#include "ezlopi_core_scenes_when_methods_helper_functions.h"

#include "ezlopi_service_meshbot.h"
#include "ezlopi_cloud_constants.h"
#include "EZLOPI_USER_CONFIG.h"

int ezlopi_scene_then_set_item_value(l_scenes_list_v2_t* curr_scene, void* arg)
{
    // TRACE_W(" Set_item_value ");
    int ret = 0;
    uint32_t item_id = 0;
    cJSON* cj_params = cJSON_CreateObject(__FUNCTION__);

    if (cj_params)
    {
        l_action_block_v2_t* curr_then = (l_action_block_v2_t*)arg;
        if (curr_then)
        {
            l_fields_v2_t* curr_field = curr_then->fields;
            while (curr_field)
            {
                if (0 == strncmp(curr_field->name, "item", 5))
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_params, ezlopi__id_str, curr_field->field_value.u_value.value_string);
                    item_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
                    // TRACE_D("item_id: %s", curr_field->field_value.u_value.value_string);
                }
                else if (0 == strncmp(curr_field->name, ezlopi_value_str, 5))
                {
                    if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
                    {
                        cJSON_AddNumberToObject(__FUNCTION__, cj_params, ezlopi_value_str, curr_field->field_value.u_value.value_double);
                        TRACE_D("value: %f", curr_field->field_value.u_value.value_double);
                    }
                    else if (EZLOPI_VALUE_TYPE_BOOL == curr_field->value_type)
                    {
                        cJSON_AddBoolToObject(__FUNCTION__, cj_params, ezlopi_value_str, curr_field->field_value.u_value.value_bool);
                        TRACE_D("value: %s", curr_field->field_value.u_value.value_bool ? ezlopi_true_str : ezlopi_false_str);
                    }
                    else if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type)
                    {
                        cJSON_AddStringToObject(__FUNCTION__, cj_params, ezlopi_value_str, curr_field->field_value.u_value.value_string);
                        TRACE_D("value: %s", curr_field->field_value.u_value.value_string);
                    }
                }

                curr_field = curr_field->next;
            }

            if (item_id)
            {
                l_ezlopi_item_t* curr_item = ezlopi_device_get_item_by_id(item_id);
                if (curr_item)
                {
                    curr_item->func(EZLOPI_ACTION_SET_VALUE, curr_item, cj_params, curr_item->user_arg);
                    ret = 1;
                }
            }
        }

        cJSON_Delete(__FUNCTION__, cj_params);
    }
    return ret;
}
int ezlopi_scene_then_set_device_armed(l_scenes_list_v2_t* curr_scene, void* arg)
{
    // TRACE_W("Warning: then-method not implemented!");
    int ret = 0;
    if (curr_scene)
    {
        uint32_t device_id = 0;
        bool device_armed = false;
        l_action_block_v2_t* curr_then = (l_action_block_v2_t*)arg;
        if (curr_then)
        {
            l_fields_v2_t* curr_field = curr_then->fields;
            while (curr_field)
            {
                if (0 == strncmp(curr_field->name, "device", 7))
                {
                    device_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
                }
                else if (0 == strncmp(curr_field->name, "deviceFlag", 11))
                {
                    if (EZLOPI_VALUE_TYPE_BOOL == curr_field->value_type)
                    {
                        device_armed = curr_field->field_value.u_value.value_bool;
                    }
                }
                curr_field = curr_field->next;
            }

            if (device_id)
            {
                l_ezlopi_device_t* curr_device = ezlopi_device_get_by_id(device_id);
                if (curr_device)
                {
                    s_ezlopi_cloud_controller_t* controller_info = ezlopi_device_get_controller_information();
                    if (controller_info)
                    {
                        #warning "we need to change from 'controller' to device-specific [krishna]"
                            controller_info->armed = (device_armed) ? true : false;
                    }
                }
            }
        }

    }

    return ret;
}
int ezlopi_scene_then_send_cloud_abstract_command(l_scenes_list_v2_t* curr_scene, void* arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_switch_house_mode(l_scenes_list_v2_t* curr_scene, void* arg)
{
    // TRACE_W(" switch_house_mode ");
    int ret = 0;
    if (curr_scene)
    {
        uint32_t house_mode_id = 0;
        l_action_block_v2_t* curr_then = (l_action_block_v2_t*)arg;
        if (curr_then)
        {
            l_fields_v2_t* curr_field = curr_then->fields;
            while (curr_field)
            {
                if (0 == strncmp(curr_field->name, "houseMode", 10))
                {
                    if (EZLOPI_VALUE_TYPE_HOUSE_MODE_ID == curr_field->value_type)
                    {
                        if (NULL != curr_field->field_value.u_value.value_string)
                        {
                            house_mode_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
                        }
                        else
                        {
                            house_mode_id = (uint32_t)curr_field->field_value.u_value.value_double;
                        }
                    }
                }

                curr_field = curr_field->next;
            }

#if defined(CONFIG_EZPI_SERV_ENABLE_MODES)
            if (house_mode_id > 0)
            {
                // first get the current 
                s_ezlopi_modes_t* curr_house_mode = ezlopi_core_modes_get_custom_modes();

                // find and match the 'house_mode' you want to switch with.
                s_house_modes_t* req_mode = ezlopi_core_modes_get_house_mode_by_id(house_mode_id);
                TRACE_E("req-house-mode-id [%d] : curr->switch_to_mode_id[%d]",
                    req_mode->_id,
                    curr_house_mode->switch_to_mode_id);
                if ((req_mode->_id != curr_house_mode->switch_to_mode_id))
                {
                    ezlopi_core_modes_api_switch_mode(req_mode);
                    ret = 1;
                }
            }
#endif //CONFIG_EZPI_SERV_ENABLE_MODES
        }

    }

    return ret;
}
int ezlopi_scene_then_send_http_request(l_scenes_list_v2_t* curr_scene, void* arg)
{
    // TRACE_W(" send_http ");
    int ret = 0;
    l_action_block_v2_t* curr_then = (l_action_block_v2_t*)arg;
    if (curr_then)
    {
        s_ezlopi_core_http_mbedtls_t* tmp_http_data = (s_ezlopi_core_http_mbedtls_t*)ezlopi_malloc(__FUNCTION__, sizeof(s_ezlopi_core_http_mbedtls_t));
        if (tmp_http_data)
        {
            memset(tmp_http_data, 0, sizeof(s_ezlopi_core_http_mbedtls_t));
            l_fields_v2_t* curr_field = curr_then->fields;

            const s_sendhttp_method_t __sendhttp_method[] = {
                {.field_name = "request", .field_func = parse_http_request_type},
                {.field_name = "url", .field_func = parse_http_url},
                {.field_name = "credential", .field_func = parse_http_creds},
                {.field_name = "contentType", .field_func = parse_http_content_type},
                {.field_name = "content", .field_func = parse_http_content},
                {.field_name = "headers", .field_func = parse_http_headers},
                {.field_name = "skipSecurity", .field_func = parse_http_skipsecurity},
                {.field_name = NULL, .field_func = NULL},
            };

            while (NULL != curr_field) // fields
            {
                for (uint8_t i = 0; i < ((sizeof(__sendhttp_method) / sizeof(__sendhttp_method[i]))); i++)
                {
                    if (0 == strncmp(__sendhttp_method[i].field_name, curr_field->name, strlen(__sendhttp_method[i].field_name) + 1))
                    {
                        (__sendhttp_method[i].field_func)(tmp_http_data, curr_field);
                        break;
                    }
                }
                curr_field = curr_field->next;
            }
            // now to trigger http_request and extract the response.
            tmp_http_data->response = NULL;
            tmp_http_data->response_maxlen = 0;
            ezlopi_core_http_mbedtls_req(tmp_http_data); // Returns:- [response_buffer = &Memory_block]
            free_http_mbedtls_struct(tmp_http_data);

            ezlopi_free(__FUNCTION__, tmp_http_data);
        }
    }

    return ret;
}
int ezlopi_scene_then_run_custom_script(l_scenes_list_v2_t* curr_scene, void* arg)
{
    // TRACE_W(" run_custom_script ");
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
    // TRACE_W(" run_scene ");
    int ret = 0;
    uint32_t sceneId = 0;
    bool execute_else_condition = false;
    l_action_block_v2_t* curr_then = (l_action_block_v2_t*)arg;
    if (curr_then)
    {
        l_fields_v2_t* curr_field = curr_then->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, ezlopi_sceneId_str, 8))
            {
                if (EZLOPI_VALUE_TYPE_SCENEID == curr_field->value_type)
                {
                    sceneId = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
                }
            }
            else if (0 == strncmp(curr_field->name, "block", 6))
            {
                if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type)
                {
                    if (0 == strncmp(curr_field->field_value.u_value.value_string, "else", 4))
                    {
                        execute_else_condition = true;
                    }
                    else if (0 == strncmp(curr_field->field_value.u_value.value_string, "thenGroups", 10))
                    {
                        TRACE_D("Running scene group, yet to be implemented.");
                    }
                }
                else
                {
                    ret = -1;
                }
            }
            else if ((0 == strncmp(curr_field->name, "group", 6)) && (curr_field->value_type == EZLOPI_VALUE_TYPE_STRING))
            {
                ret = -1;
                TRACE_D("Running scene group, yet to be implemented.");
            }
            curr_field = curr_field->next;
        }

        if (execute_else_condition)
        {
            TRACE_D("Executing else condition");
            ezlopi_meshbot_execute_scene_else_action_group(sceneId);
            ret = 1;
        }
        else
        {
            TRACE_D("Executing scene, id: %d", sceneId);
            ezlopi_scenes_service_run_by_id(sceneId);
            ret = 1;
        }
    }
    else
    {
        ret = -1;
    }
    return ret;
}
int ezlopi_scene_then_set_scene_state(l_scenes_list_v2_t* curr_scene, void* arg)
{
    // TRACE_W(" set_scene_state ");
    int ret = 0;
    uint32_t sceneID = 0;
    bool set_scene_enable = false;
    l_action_block_v2_t* curr_then = (l_action_block_v2_t*)arg;

    if (curr_then)
    {
        l_fields_v2_t* curr_field = curr_then->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, ezlopi_sceneId_str, 8))
            {
                if (EZLOPI_VALUE_TYPE_SCENEID == curr_field->value_type)
                {
                    sceneID = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
                }
            }
            else if (0 == strncmp(curr_field->name, ezlopi_enabled_str, 8))
            {

                TRACE_W("%s", (curr_field->field_value.u_value.value_bool ? "true" : "false"));

                if (EZLOPI_VALUE_TYPE_BOOL == curr_field->value_type)
                {
                    set_scene_enable = curr_field->field_value.u_value.value_bool;
                }
            }
            curr_field = curr_field->next;
        }
        l_scenes_list_v2_t* scene_node = ezlopi_scenes_get_by_id_v2(sceneID);
        if (scene_node)
        {
            if (1 == ezlopi_scenes_enable_disable_id_from_list_v2(sceneID, set_scene_enable))
            {
                ezlopi_scenes_status_change_broadcast(scene_node, scene_status_finished_str);
                ret = 1;
            }
            else
            {
                // TRACE_E("Failed : 'set_scene_state'");
                ezlopi_scenes_status_change_broadcast(scene_node, scene_status_failed_str);
            }
        }
        TRACE_W("ret = %d", ret);
    }
    return ret;
}
int ezlopi_scene_then_reset_latch(l_scenes_list_v2_t* curr_scene, void* arg)
{
    int ret = 0;

    l_action_block_v2_t* curr_block = (l_action_block_v2_t*)arg;
    if (curr_block && curr_scene)
    {
        char * sceneId_str = NULL;
        char * blockId_str = NULL;
        l_fields_v2_t* curr_field = curr_block->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, ezlopi_sceneId_str, 8))
            {
                if ((EZLOPI_VALUE_TYPE_STRING == curr_field->value_type) && (NULL != curr_field->field_value.u_value.value_string))
                {
                    sceneId_str = curr_field->field_value.u_value.value_string;
                    // TRACE_S("sceneId[%s]", sceneId);
                }
            }
            else if (0 == strncmp(curr_field->name, ezlopi_blockId_str, 8))
            {
                if ((EZLOPI_VALUE_TYPE_STRING == curr_field->value_type) && (NULL != curr_field->field_value.u_value.value_string))
                {
                    blockId_str = curr_field->field_value.u_value.value_string;
                    // TRACE_S("blockId[%s]", blockId_str);
                }
            }
            curr_field = curr_field->next;
        }

        if (sceneId_str && blockId_str)
        {
            ezlopi_scene_reset_specific_latch(sceneId_str, blockId_str);
        }
    }
    return ret;
}
int ezlopi_scene_then_reset_scene_latches(l_scenes_list_v2_t* curr_scene, void* arg)
{
    int ret = 0;
    char * sceneId_str = 0;
    l_action_block_v2_t* curr_then = (l_action_block_v2_t*)arg;
    if (curr_then && curr_scene)
    {
        l_fields_v2_t* curr_field = curr_then->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, ezlopi_sceneId_str, 8))
            {
                if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type)
                {
                    sceneId_str = curr_field->field_value.u_value.value_string;
                    // TRACE_S("sceneId[%s]", sceneId);
                }
            }
            curr_field = curr_field->next;
        }

        if (sceneId_str)
        {
            ezlopi_scene_reset_specific_latch(sceneId_str, NULL);
        }
    }
    return ret;
}
int ezlopi_scene_then_reboot_hub(l_scenes_list_v2_t* curr_scene, void* arg)
{
    TRACE_E("Rebooting ESP......................... ");
    EZPI_CORE_reset_reboot();
    return 1;
}
int ezlopi_scene_then_reset_hub(l_scenes_list_v2_t* curr_scene, void* arg)
{
    TRACE_W(" reset_hub ");
    int ret = 0;
    cJSON* cj_params = cJSON_CreateObject(__FUNCTION__);

    if (cj_params)
    {
        l_action_block_v2_t* curr_then = (l_action_block_v2_t*)arg;
        if (curr_then)
        {
            l_fields_v2_t* curr_field = curr_then->fields;
            while (curr_field)
            {
                if (0 == strncmp(curr_field->name, "type", 5))
                {
                    if ((EZLOPI_VALUE_TYPE_ENUM == curr_field->value_type) && (NULL != curr_field->field_value.u_value.value_string))
                    {
                        TRACE_S("value: %s", curr_field->field_value.u_value.value_string);
                        if (0 == strncmp(curr_field->field_value.u_value.value_string, "factory", 8))
                        {
                            TRACE_E("Factory Reseting ESP......................... ");
                            // clear the settings realated to scenes, devices, items, rooms,etc
                            ezlopi_scenes_scripts_factory_info_reset();
                            ezlopi_device_factory_info_reset();
                            ezlopi_nvs_scenes_factory_info_reset(); // 'nvs' partitions

                            ezlopi_factory_info_v3_scenes_factory_soft_reset(); // 'ID' partition :- 'wifi' sector
                            TRACE_E("Rebooting ESP......................... ");
                            EZPI_CORE_reset_reboot();
                        }
                        else if (0 == strncmp(curr_field->field_value.u_value.value_string, "soft", 5))
                        {
                            ezlopi_nvs_scenes_soft_reset();

                            ezlopi_factory_info_v3_scenes_factory_soft_reset(); // 'ID' partition :- 'wifi' sector
                            TRACE_E("Rebooting ESP......................... ");
                            EZPI_CORE_reset_reboot();
                        }
                        else if (0 == strncmp(curr_field->field_value.u_value.value_string, "hard", 5))
                        {
                            #warning "hard reset not in documention.";
                            EZPI_CORE_reset_factory_restore();
                        }
                    }
                }

                curr_field = curr_field->next;
            }
        }

        cJSON_Delete(__FUNCTION__, cj_params);
    }
    return ret;
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
    // TRACE_W(" toggle_value ");
    int ret = 0;
    if (curr_scene)
    {
        uint32_t item_id = 0;       /* item */
        char *  __id_string = NULL;
        char* expression_name = NULL; /* expression */

        l_action_block_v2_t* curr_then = (l_action_block_v2_t*)arg;
        if (curr_then)
        {
            l_fields_v2_t* curr_field = curr_then->fields;
            while (curr_field)
            {
                if (0 == strncmp(curr_field->name, ezlopi_item_str, 5))
                {
                    if ((EZLOPI_VALUE_TYPE_ITEM == curr_field->value_type) && (NULL != curr_field->field_value.u_value.value_string))
                    {
                        __id_string = curr_field->field_value.u_value.value_string;
                        item_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
                    }
                }
                else if (0 == strncmp(curr_field->name, "expression", 11)) /*need to add in str*/
                {
                    if ((EZLOPI_VALUE_TYPE_EXPRESSION == curr_field->value_type) && (NULL != curr_field->field_value.u_value.value_string))
                    {
                        expression_name = curr_field->field_value.u_value.value_string;
                        TRACE_W("expn_name : '%s'", expression_name);
                    }
                }
                curr_field = curr_field->next;
            }

            if (item_id > 0)
            {
                TRACE_W("item_id: %s", __id_string);
                l_ezlopi_item_t* curr_item = ezlopi_device_get_item_by_id(item_id);
                if ((curr_item) && (EZLOPI_DEVICE_INTERFACE_DIGITAL_OUTPUT == curr_item->interface_type))
                {
                    cJSON* cj_tmp_value = cJSON_CreateObject(__FUNCTION__);
                    if (cj_tmp_value)
                    {
                        if (curr_item->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, curr_item, (void*)cj_tmp_value, NULL))
                        {
                            CJSON_TRACE("present_gpio_value", cj_tmp_value);/*value formatted & value only*/

                            cJSON* cj_val = cJSON_GetObjectItem(__FUNCTION__, cj_tmp_value, ezlopi_value_str);
                            if (cj_val)
                            {
                                cJSON* cj_result_value = cJSON_CreateObject(__FUNCTION__);
                                if (cj_result_value)
                                {
                                    cJSON_AddStringToObject(__FUNCTION__, cj_result_value, ezlopi__id_str, __id_string);


                                    if ((0 == strncmp(curr_item->cloud_properties.value_type, value_type_bool, 5)) && cJSON_IsBool(cj_val))
                                    {
                                        // TRACE_S("1. getting 'item_id[%d]' ; bool_value = %s ", item_id, (cj_val->type == cJSON_True) ? "true" : "false"); // "false" or "true"
                                        if (cj_val->type == cJSON_True)
                                        {
                                            cJSON_AddBoolToObject(__FUNCTION__, cj_result_value, ezlopi_value_str, false);
                                        }
                                        else if (cj_val->type == cJSON_False)
                                        {
                                            cJSON_AddBoolToObject(__FUNCTION__, cj_result_value, ezlopi_value_str, true);
                                        }
                                        ret = 1;
                                        curr_item->func(EZLOPI_ACTION_SET_VALUE, curr_item, cj_result_value, curr_item->user_arg);
                                    }
                                    else if ((0 == strncmp(curr_item->cloud_properties.value_type, value_type_int, 4)) && cJSON_IsNumber(cj_val))
                                    {
                                        // TRACE_S("2. getting 'item_id[%d]' ; int_value = %d ", item_id, (int)cj_val->valuedouble);
                                        if (cj_val->valuedouble == 0) // either  '0' or '1'.
                                        {
                                            cJSON_AddNumberToObject(__FUNCTION__, cj_result_value, ezlopi_value_str, 1);
                                        }
                                        else if (cj_val->valuedouble == 1)
                                        {
                                            cJSON_AddNumberToObject(__FUNCTION__, cj_result_value, ezlopi_value_str, 0);
                                        }
                                        ret = 1;
                                        curr_item->func(EZLOPI_ACTION_SET_VALUE, curr_item, cj_result_value, curr_item->user_arg);
                                    }
                                    else
                                    {
                                        ret = 0;
                                        TRACE_E(" 'item_id[%d]' neither 'boolean' nor 'int' ;  Value-type mis-matched!  ", item_id);
                                    }

                                    cJSON_Delete(__FUNCTION__, cj_result_value);
                                }
                            }
                        }

                        cJSON_Delete(__FUNCTION__, cj_tmp_value);
                    }
                }
                else
                {
                    TRACE_E("wrong item_ID");
                }
            }
            else if (NULL != expression_name)
            {
                s_ezlopi_expressions_t* curr_exp = ezlopi_scenes_get_expression_node_by_name(expression_name);
                if (curr_exp)
                {
                    if (EXPRESSION_VALUE_TYPE_NUMBER == curr_exp->exp_value.type)
                    {
                        ret = 1;
                        curr_exp->exp_value.u_value.number_value = (0 == curr_exp->exp_value.u_value.number_value) ? 1 : 0;
                    }
                    else if (EXPRESSION_VALUE_TYPE_BOOL)
                    {
                        ret = 1;
                        curr_exp->exp_value.u_value.boolean_value = (false == curr_exp->exp_value.u_value.boolean_value) ? true : false;
                    }
                }

            }
        }
    }
    return ret;
}
#endif  // CONFIG_EZPI_SERV_ENABLE_MESHBOTS