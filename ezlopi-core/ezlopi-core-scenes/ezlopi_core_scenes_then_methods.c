/* ===========================================================================
** Copyright (C) 2025 Ezlo Innovation Inc
**
** Under EZLO AVAILABLE SOURCE LICENSE (EASL) AGREEMENT
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/
/**
 * @file    ezlopi_core_scenes_then_methods.c
 * @brief   Funtions that performs scene-Action/Then method operations
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 * @version 0.1
 * @date    12th DEC 2024
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include "ezlopi_util_trace.h"

// #include "ezlopi_core_devices.h"
// #include "ezlopi_core_scenes_v2.h"
#include "ezlopi_core_nvs.h"
#include "ezlopi_core_http.h"
#include "ezlopi_core_sntp.h"
#include "ezlopi_core_modes.h"
#include "ezlopi_core_reset.h"
#include "ezlopi_core_scenes_value.h"
#include "ezlopi_core_device_group.h"
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
/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/

int EZPI_core_scene_then_set_item_value(l_scenes_list_v2_t *curr_scene, void *arg)
{
    // TRACE_W(" Set_item_value ");
    int ret = 0;
    uint32_t item_id = 0;
    cJSON *cj_params = cJSON_CreateObject(__FUNCTION__);
    if (cj_params)
    {
        l_action_block_v2_t *curr_then = (l_action_block_v2_t *)arg;
        if (curr_then)
        {
            l_fields_v2_t *curr_field = curr_then->fields;
            while (curr_field)
            {
                size_t len = (curr_field->name ? strlen(curr_field->name) + 1 : 0);
                if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_item_str, len, 5))
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_params, ezlopi__id_str, curr_field->field_value.u_value.value_string);
                    item_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
                    // TRACE_D("item_id: %s", curr_field->field_value.u_value.value_string);
                }
                else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_value_str, len, 6))
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
                l_ezlopi_item_t *curr_item = EZPI_core_device_get_item_by_id(item_id);
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
int EZPI_core_scene_then_group_set_item_value(l_scenes_list_v2_t *curr_scene, void *arg)
{
    // TRACE_W("Warning: then-method not implemented!");
    int ret = 0;
    uint32_t device_group_id = 0;
    uint32_t item_group_id = 0;
    // char * item_id_str = NULL;

    cJSON *cj_params = cJSON_CreateObject(__FUNCTION__);
    if (cj_params)
    {
        l_action_block_v2_t *curr_then = (l_action_block_v2_t *)arg;
        if (curr_then)
        {
            l_fields_v2_t *curr_field = curr_then->fields;
            while (curr_field)
            {
                size_t len = (curr_field->name ? strlen(curr_field->name) + 1 : 0);
                if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_deviceGroup_str, len, 12))
                {
                    device_group_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
                    // TRACE_D("item_id: %s", curr_field->field_value.u_value.value_string);
                }
                else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_itemGroup_str, len, 10))
                {
                    item_group_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
                    // TRACE_D("item_id: %s", curr_field->field_value.u_value.value_string);
                }
                else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_value_str, len, 6))
                {
#warning "might need to add more item_value_types";
                    switch (curr_field->value_type)
                    {
                    case EZLOPI_VALUE_TYPE_INT:
                    case EZLOPI_VALUE_TYPE_FLOAT:
                    {
                        cJSON_AddNumberToObject(__FUNCTION__, cj_params, ezlopi_value_str, curr_field->field_value.u_value.value_double);
                        TRACE_D("value: %f", curr_field->field_value.u_value.value_double);
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_BOOL:
                    {
                        cJSON_AddBoolToObject(__FUNCTION__, cj_params, ezlopi_value_str, curr_field->field_value.u_value.value_bool);
                        TRACE_D("value: %s", curr_field->field_value.u_value.value_bool ? ezlopi_true_str : ezlopi_false_str);
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_STRING:
                    case EZLOPI_VALUE_TYPE_TOKEN:
                    {
                        cJSON_AddStringToObject(__FUNCTION__, cj_params, ezlopi_value_str, curr_field->field_value.u_value.value_string);
                        TRACE_D("value: %s", curr_field->field_value.u_value.value_string);
                        break;
                    }
                    default:
                        break;
                    }
                }

                curr_field = curr_field->next;
            }

            if (device_group_id && item_group_id)
            {
                l_ezlopi_device_grp_t *curr_devgrp = EZPI_core_device_group_get_by_id(device_group_id);
                if (curr_devgrp)
                {
                    cJSON *cj_get_devarr = NULL;
                    cJSON_ArrayForEach(cj_get_devarr, curr_devgrp->devices)
                    {
                        uint32_t curr_device_id = strtoul(cj_get_devarr->valuestring, NULL, 16);
                        l_ezlopi_device_t *curr_device = EZPI_core_device_get_by_id(curr_device_id); // immediately goto "102ec000" ...
                        if (curr_device)
                        {
                            l_ezlopi_item_t *curr_item_node = curr_device->items; // perform operation on items of above device --> "102ec000"
                            while (curr_item_node)
                            {
                                // compare with items_list stored in item_group_id
                                l_ezlopi_item_grp_t *curr_item_grp = EZPI_core_item_group_get_by_id(item_group_id); // get  "ll_itemgrp_node"
                                if (curr_item_grp)
                                {
                                    int count = 0;
                                    cJSON *cj_item_names = NULL;
                                    while (NULL != (cj_item_names = cJSON_GetArrayItem(curr_item_grp->item_names, count))) // ["202ec000" , "202ec001" ,..]
                                    {
                                        uint32_t req_item_id_from_itemgrp = strtoul(cj_item_names->valuestring, NULL, 16);
                                        if (req_item_id_from_itemgrp == curr_item_node->cloud_properties.item_id)
                                        {
                                            cJSON_AddStringToObject(__FUNCTION__, cj_params, ezlopi__id_str, cj_item_names->valuestring);
                                            curr_item_node->func(EZLOPI_ACTION_SET_VALUE, curr_item_node, cj_params, curr_item_node->user_arg);
                                            ret = 1;
                                        }
                                        count++;
                                    }
                                }
                                curr_item_node = curr_item_node->next;
                            }
                        }
                    }
                }
            }
        }

        cJSON_Delete(__FUNCTION__, cj_params);
    }
    return ret;
}
int EZPI_core_scene_then_set_device_armed(l_scenes_list_v2_t *curr_scene, void *arg)
{
    // TRACE_W("Warning: then-method not implemented!");
    int ret = 0;
    if (curr_scene)
    {
        uint32_t device_id = 0;
        bool device_armed = false;
        l_action_block_v2_t *curr_then = (l_action_block_v2_t *)arg;
        if (curr_then)
        {
            l_fields_v2_t *curr_field = curr_then->fields;
            while (curr_field)
            {
                size_t len = (curr_field->name ? strlen(curr_field->name) + 1 : 0);
                if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, dev_type_device, len, 7))
                {
                    device_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
                }
                else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_deviceFlag_str, len, 11))
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
                l_ezlopi_device_t *curr_device = EZPI_core_device_get_by_id(device_id);
                if (curr_device)
                {
                    curr_device->cloud_properties.armed = (device_armed) ? true : false;
                    s_ezlopi_cloud_controller_t *controller_info = EZPI_core_device_get_controller_information();
                    if (controller_info)
                    {
#warning "we need to change from 'controller' to device-specific";
                        controller_info->armed = (device_armed) ? true : false;
                    }
                }
            }
        }
    }

    return ret;
}
int EZPI_core_scene_then_group_set_device_armed(l_scenes_list_v2_t *curr_scene, void *arg)
{
    // TRACE_W("Warning: then-method not implemented!");
    int ret = 0;
    if (curr_scene)
    {
        uint32_t device_group_id = 0;
        bool device_armed = false;
        l_action_block_v2_t *curr_then = (l_action_block_v2_t *)arg;
        if (curr_then)
        {
            l_fields_v2_t *curr_field = curr_then->fields;
            while (curr_field)
            {

                size_t len = (curr_field->name ? strlen(curr_field->name) + 1 : 0);
                if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_deviceGroup_str, len, 12))
                {
                    device_group_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
                }
                else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_deviceFlag_str, len, 11))
                {
                    if (EZLOPI_VALUE_TYPE_BOOL == curr_field->value_type)
                    {
                        device_armed = curr_field->field_value.u_value.value_bool;
                    }
                }
                curr_field = curr_field->next;
            }

            if (device_group_id)
            {
                l_ezlopi_device_grp_t *curr_devgrp = EZPI_core_device_group_get_by_id(device_group_id);
                if (curr_devgrp)
                {
                    cJSON *cj_get_devarr = NULL;
                    cJSON_ArrayForEach(cj_get_devarr, curr_devgrp->devices)
                    {
                        uint32_t curr_device_id = strtoul(cj_get_devarr->valuestring, NULL, 16);
                        l_ezlopi_device_t *curr_device = EZPI_core_device_get_by_id(curr_device_id); // immediately goto "102ec000" ...
                        if (curr_device)
                        {
                            curr_device->cloud_properties.armed = (device_armed) ? true : false;
                            s_ezlopi_cloud_controller_t *controller_info = EZPI_core_device_get_controller_information();
                            if (controller_info)
                            {
#warning "we need to change from 'controller' to device-specific [krishna]";
                                controller_info->armed = (device_armed) ? true : false;
                            }
                        }
                    }
                }
            }
        }
    }

    return ret;
}
int EZPI_core_scene_then_send_cloud_abstract_command(l_scenes_list_v2_t *curr_scene, void *arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int EZPI_core_scene_then_switch_house_mode(l_scenes_list_v2_t *curr_scene, void *arg)
{
    // TRACE_W(" switch_house_mode ");
    int ret = 0;
    if (curr_scene)
    {
        uint32_t house_mode_id = 0;
        l_action_block_v2_t *curr_then = (l_action_block_v2_t *)arg;
        if (curr_then)
        {
            l_fields_v2_t *curr_field = curr_then->fields;
            while (curr_field)
            {
                if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_houseMode_str, strlen(curr_field->name) + 1, 10))
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
                s_ezlopi_modes_t *curr_house_mode = EZPI_core_modes_get_custom_modes();

                // find and match the 'house_mode' you want to switch with.
                s_house_modes_t *req_mode = EZPI_core_modes_get_house_mode_by_id(house_mode_id);
                TRACE_E("req-house-mode-id [%d] : curr->switch_to_mode_id[%d]", req_mode->_id, curr_house_mode->switch_to_mode_id);
                if ((req_mode->_id != curr_house_mode->switch_to_mode_id))
                {
                    EZPI_core_modes_api_switch_mode(req_mode);
                    ret = 1;
                }
            }
#endif // CONFIG_EZPI_SERV_ENABLE_MODES
        }
    }

    return ret;
}
int EZPI_core_scene_then_send_http_request(l_scenes_list_v2_t *curr_scene, void *arg)
{
    // TRACE_W(" send_http ");
    int ret = 0;
    l_action_block_v2_t *curr_then = (l_action_block_v2_t *)arg;
    if (curr_then)
    {
        s_ezlopi_core_http_mbedtls_t *tmp_http_data = (s_ezlopi_core_http_mbedtls_t *)ezlopi_malloc(__FUNCTION__, sizeof(s_ezlopi_core_http_mbedtls_t));
        if (tmp_http_data)
        {
            memset(tmp_http_data, 0, sizeof(s_ezlopi_core_http_mbedtls_t));
            l_fields_v2_t *curr_field = curr_then->fields;

            const s_sendhttp_method_t __sendhttp_method[] = {
                {.field_name = ezlopi_request_str, .field_func = EZPI_parse_http_request_type},
                {.field_name = ezlopi_url_str, .field_func = EZPI_parse_http_url},
                {.field_name = ezlopi_credential_str, .field_func = EZPI_parse_http_creds},
                {.field_name = ezlopi_contentType_str, .field_func = EZPI_parse_http_content_type},
                {.field_name = ezlopi_content_str, .field_func = EZPI_parse_http_content},
                {.field_name = ezlopi_headers_str, .field_func = EZPI_parse_http_headers},
                {.field_name = ezlopi_skipSecurity_str, .field_func = EZPI_parse_http_skipsecurity},
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
            EZPI_core_http_mbedtls_req(tmp_http_data); // Returns:- [response_buffer = &Memory_block]

            EZPI_free_http_mbedtls_struct(tmp_http_data);
            ezlopi_free(__FUNCTION__, tmp_http_data);
        }
    }

    return ret;
}
int EZPI_core_scene_then_run_custom_script(l_scenes_list_v2_t *curr_scene, void *arg)
{
    // TRACE_W(" run_custom_script ");
    int ret = 0;
    uint32_t script_id = 0;
    l_action_block_v2_t *curr_then = (l_action_block_v2_t *)arg;
    if (curr_then)
    {
        l_fields_v2_t *curr_field = curr_then->fields;
        while (curr_field)
        {
            if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, "script", strlen(curr_field->name) + 1, 7))
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
        EZPI_scenes_scripts_run_by_id(script_id);
    }

    return ret;
}
int EZPI_core_scene_then_run_plugin_script(l_scenes_list_v2_t *curr_scene, void *arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int EZPI_core_scene_then_run_scene(l_scenes_list_v2_t *curr_scene, void *arg)
{
    // TRACE_W(" run_scene ");
    int ret = 0;
    uint32_t sceneId = 0;
    bool execute_else_condition = false;
    l_action_block_v2_t *curr_then = (l_action_block_v2_t *)arg;
    if (curr_then)
    {
        l_fields_v2_t *curr_field = curr_then->fields;
        while (curr_field)
        {
            size_t name_len = (curr_field->name ? strlen(curr_field->name) + 1 : 0);
            if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_sceneId_str, name_len, 8))
            {
                if (EZLOPI_VALUE_TYPE_SCENEID == curr_field->value_type)
                {
                    sceneId = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_block_str, name_len, 6))
            {
                if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && (NULL != curr_field->field_value.u_value.value_string))
                {
                    char *tmp_str = curr_field->field_value.u_value.value_string;
                    if (EZPI_STRNCMP_IF_EQUAL(tmp_str, ezlopi_else_str, strlen(tmp_str) + 1, 5))
                    {
                        execute_else_condition = true;
                    }
                    else if (EZPI_STRNCMP_IF_EQUAL(tmp_str, ezlopi_thenGroups_str, strlen(tmp_str) + 1, 11))
                    {
                        TRACE_D("Running scene group, yet to be implemented.");
                    }
                }
                else
                {
                    ret = -1;
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_group_str, name_len, 6))
            {
                if (curr_field->value_type == EZLOPI_VALUE_TYPE_STRING)
                {
                    ret = -1;
                    TRACE_D("Running scene group, yet to be implemented.");
                }
            }
            curr_field = curr_field->next;
        }

        if (execute_else_condition)
        {
            TRACE_D("Executing else condition");
            EZPI_meshbot_execute_scene_else_action_group(sceneId);
            ret = 1;
        }
        else
        {
            TRACE_D("Executing scene, id: %d", sceneId);
            EZPI_scenes_service_run_by_id(sceneId);
            ret = 1;
        }
    }
    else
    {
        ret = -1;
    }
    return ret;
}
int EZPI_core_scene_then_set_scene_state(l_scenes_list_v2_t *curr_scene, void *arg)
{
    // TRACE_W(" set_scene_state ");
    int ret = 0;
    uint32_t sceneID = 0;
    bool set_scene_enable = false;
    l_action_block_v2_t *curr_then = (l_action_block_v2_t *)arg;

    if (curr_then)
    {
        l_fields_v2_t *curr_field = curr_then->fields;
        while (curr_field)
        {
            size_t len = (curr_field->name ? strlen(curr_field->name) + 1 : 0);
            if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_sceneId_str, len, 8))
            {
                if (EZLOPI_VALUE_TYPE_SCENEID == curr_field->value_type)
                {
                    sceneID = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_enabled_str, len, 8))
            {
                if (EZLOPI_VALUE_TYPE_BOOL == curr_field->value_type)
                {
                    set_scene_enable = curr_field->field_value.u_value.value_bool;
                }
            }
            curr_field = curr_field->next;
        }
        l_scenes_list_v2_t *scene_node = EZPI_core_scenes_get_by_id_v2(sceneID);
        if (scene_node)
        {
            if (EZPI_SUCCESS == EZPI_core_scenes_enable_disable_scene_by_id_v2(sceneID, set_scene_enable))
            {
                EZPI_core_scenes_status_change_broadcast(scene_node, scene_status_finished_str, EZPI_core_sntp_get_current_time_sec());
                ret = 1;
            }
            else
            {
                // TRACE_E("Failed : 'set_scene_state'");
                EZPI_core_scenes_status_change_broadcast(scene_node, scene_status_failed_str, EZPI_core_sntp_get_current_time_sec());
            }
        }
        TRACE_W("ret = %d", ret);
    }
    return ret;
}
int EZPI_core_scene_then_reset_latch(l_scenes_list_v2_t *curr_scene, void *arg)
{
    int ret = 0;

    l_action_block_v2_t *curr_block = (l_action_block_v2_t *)arg;
    if (curr_block && curr_scene)
    {
        char *sceneId_str = NULL;
        char *blockId_str = NULL;
        l_fields_v2_t *curr_field = curr_block->fields;
        while (curr_field)
        {
            size_t len = (curr_field->name ? strlen(curr_field->name) + 1 : 0);
            if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_sceneId_str, len, 8))
            {
                if ((EZLOPI_VALUE_TYPE_STRING == curr_field->value_type) && (NULL != curr_field->field_value.u_value.value_string))
                {
                    sceneId_str = curr_field->field_value.u_value.value_string;
                    // TRACE_S("sceneId[%s]", sceneId);
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_blockId_str, len, 8))
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
            EZPI_core_scenes_reset_latch_state(sceneId_str, blockId_str);
        }
    }
    return ret;
}
int EZPI_core_scene_then_reset_scene_latches(l_scenes_list_v2_t *curr_scene, void *arg)
{
    int ret = 0;
    char *sceneId_str = 0;
    l_action_block_v2_t *curr_then = (l_action_block_v2_t *)arg;
    if (curr_then && curr_scene)
    {
        l_fields_v2_t *curr_field = curr_then->fields;
        while (curr_field)
        {
            if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_sceneId_str, strlen(curr_field->name) + 1, 8))
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
            EZPI_core_scenes_reset_latch_state(sceneId_str, NULL);
        }
    }
    return ret;
}
int EZPI_core_scene_then_reboot_hub(l_scenes_list_v2_t *curr_scene, void *arg)
{
    TRACE_E("Rebooting ESP......................... ");
    EZPI_core_reset_reboot();
    return 1;
}
int EZPI_core_scene_then_reset_hub(l_scenes_list_v2_t *curr_scene, void *arg)
{
    TRACE_W(" reset_hub ");
    int ret = 0;
    cJSON *cj_params = cJSON_CreateObject(__FUNCTION__);

    if (cj_params)
    {
        l_action_block_v2_t *curr_then = (l_action_block_v2_t *)arg;
        if (curr_then)
        {
            l_fields_v2_t *curr_field = curr_then->fields;
            while (curr_field)
            {
                if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_type_str, strlen(curr_field->name) + 1, 5))
                {
                    if ((EZLOPI_VALUE_TYPE_ENUM == curr_field->value_type) && (NULL != curr_field->field_value.u_value.value_string))
                    {
                        char *tmp_str = curr_field->field_value.u_value.value_string;
                        // TRACE_S("value: %s", tmp_str);
                        if (EZPI_STRNCMP_IF_EQUAL(tmp_str, "factory", strlen(tmp_str) + 1, 8))
                        {
                            TRACE_E("Factory Reseting ESP......................... ");
                            // clear the settings realated to scenes, devices, items, rooms,etc
                            EZPI_scenes_scripts_factory_info_reset();
                            EZPI_core_device_factory_info_reset();
                            EZPI_core_nvs_scenes_factory_info_reset(); // 'nvs' partitions

                            EZPI_core_factory_info_v3_scenes_factory_soft_reset(); // 'ID' partition :- 'wifi' sector
                            TRACE_E("Rebooting ESP......................... ");
                            EZPI_core_reset_reboot();
                        }
                        else if (EZPI_STRNCMP_IF_EQUAL(tmp_str, "soft", strlen(tmp_str), 5))
                        {
                            EZPI_core_nvs_scenes_soft_reset();

                            EZPI_core_factory_info_v3_scenes_factory_soft_reset(); // 'ID' partition :- 'wifi' sector
                            TRACE_E("Rebooting ESP......................... ");
                            EZPI_core_reset_reboot();
                        }
                        else if (EZPI_STRNCMP_IF_EQUAL(tmp_str, "hard", strlen(tmp_str), 5))
                        {
#warning "hard reset not in documention.";
                            EZPI_core_reset_factory_restore();
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
int EZPI_core_scene_then_cloud_api(l_scenes_list_v2_t *curr_scene, void *arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int EZPI_core_scene_then_set_expression(l_scenes_list_v2_t *curr_scene, void *arg)
{
    int ret = 0;
    if (curr_scene)
    {
        char *expression_name = NULL;
        char *code_str = NULL;
        char *exp_value_type = NULL;
        cJSON *cj_metadata = NULL;
        cJSON *cj_params = NULL;

        l_action_block_v2_t *curr_then = (l_action_block_v2_t *)arg;
        if (curr_then)
        {
            l_fields_v2_t *curr_field = curr_then->fields;
            while (curr_field)
            {
                size_t len = (curr_field->name ? strlen(curr_field->name) + 1 : 0);
                if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_name_str, len, 5))
                {
                    if ((EZLOPI_VALUE_TYPE_STRING == curr_field->value_type) && (NULL != curr_field->field_value.u_value.value_string))
                    {
                        expression_name = curr_field->field_value.u_value.value_string;
                    }
                }
                else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_code_str, len, 5))
                {
                    if ((EZLOPI_VALUE_TYPE_STRING == curr_field->value_type) && (NULL != curr_field->field_value.u_value.value_string))
                    {
                        code_str = curr_field->field_value.u_value.value_string;
                    }
                }
                else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_valueType_str, len, 10))
                {
                    if ((EZLOPI_VALUE_TYPE_STRING == curr_field->value_type) && (NULL != curr_field->field_value.u_value.value_string))
                    {
                        exp_value_type = curr_field->field_value.u_value.value_string;
                    }
                }
                else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_metadata_str, len, 9))
                {
                    if ((EZLOPI_VALUE_TYPE_OBJECT == curr_field->value_type) && (NULL != curr_field->field_value.u_value.cj_value))
                    {
                        cj_metadata = curr_field->field_value.u_value.cj_value;
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
                        CJSON_TRACE("params", cj_metadata);
#endif
                    }
                }
                else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_params_str, len, 7))
                {
                    if ((EZLOPI_VALUE_TYPE_EXPRESSION == curr_field->value_type) && (NULL != curr_field->field_value.u_value.cj_value))
                    {
                        cj_params = curr_field->field_value.u_value.cj_value;
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
                        CJSON_TRACE("params", cj_params);
#endif
                    }
                }
                curr_field = curr_field->next;
            }
        }

        if (expression_name)
        {
            ret = (int)(EZPI_SUCCESS == EZPI_core_scenes_then_helper_set_expn_var(expression_name, code_str, exp_value_type, cj_metadata, cj_params, NULL));
        }
    }
    return ret;
}
int EZPI_core_scene_then_set_variable(l_scenes_list_v2_t *curr_scene, void *arg)
{
    int ret = 0;
    if (curr_scene)
    {
        char *expression_name = NULL;
        char *var_value_type = NULL;
        l_fields_v2_t *field_var_value = NULL;
        cJSON *cj_metadata = NULL;

        l_action_block_v2_t *curr_then = (l_action_block_v2_t *)arg;
        if (curr_then)
        {
            l_fields_v2_t *curr_field = curr_then->fields;
            while (curr_field)
            {
                size_t len = (curr_field->name ? strlen(curr_field->name) + 1 : 0);
                if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_name_str, len, 5))
                {
                    if ((EZLOPI_VALUE_TYPE_STRING == curr_field->value_type) && (NULL != curr_field->field_value.u_value.value_string))
                    {
                        expression_name = curr_field->field_value.u_value.value_string;
                    }
                }
                else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_value_str, len, 5))
                {
                    if ((EZLOPI_VALUE_TYPE_NONE < curr_field->value_type && EZLOPI_VALUE_TYPE_MAX > curr_field->value_type)) // humidity
                    {
                        field_var_value = curr_field;
                    }
                }
                else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_valueType_str, len, 10))
                {
                    if ((EZLOPI_VALUE_TYPE_STRING == curr_field->value_type) && (NULL != curr_field->field_value.u_value.value_string))
                    {
                        var_value_type = curr_field->field_value.u_value.value_string;
                    }
                }
                else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_metadata_str, len, 9))
                {
                    if ((EZLOPI_VALUE_TYPE_OBJECT == curr_field->value_type) && (NULL != curr_field->field_value.u_value.cj_value))
                    {
                        cj_metadata = curr_field->field_value.u_value.cj_value;
                    }
                }
                curr_field = curr_field->next;
            }
        }

        if (expression_name)
        {
            ret = (int)(EZPI_SUCCESS == EZPI_core_scenes_then_helper_set_expn_var(expression_name, NULL, var_value_type, cj_metadata, NULL, field_var_value));
        }
    }
    return ret;
}
int EZPI_core_scene_then_toggle_value(l_scenes_list_v2_t *curr_scene, void *arg)
{
    // TRACE_W(" toggle_value ");
    int ret = 0;
    if (curr_scene)
    {
        uint32_t item_id = 0; /* item */
        char *__id_string = NULL;
        char *expression_name = NULL; /* expression */

        l_action_block_v2_t *curr_then = (l_action_block_v2_t *)arg;
        if (curr_then)
        {
            l_fields_v2_t *curr_field = curr_then->fields;
            while (curr_field)
            {
                size_t len = (curr_field->name ? strlen(curr_field->name) + 1 : 0);
                if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_item_str, len, 5))
                {
                    if ((EZLOPI_VALUE_TYPE_ITEM == curr_field->value_type) && (NULL != curr_field->field_value.u_value.value_string))
                    {
                        __id_string = curr_field->field_value.u_value.value_string;
                        item_id = strtoul(__id_string, NULL, 16);
                    }
                }
                else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_expression_str, len, 11)) /*need to add in str*/
                {
                    if ((EZLOPI_VALUE_TYPE_EXPRESSION == curr_field->value_type) && (NULL != curr_field->field_value.u_value.value_string))
                    {
                        expression_name = curr_field->field_value.u_value.value_string;
                        TRACE_W("expn_name : '%s'", expression_name);
                    }
                }
                curr_field = curr_field->next;
            }

            if (item_id && __id_string)
            {
                ret = EZPI_core_scenes_then_helper_toggleValue(item_id, __id_string);
            }
            else if (NULL != expression_name)
            {
                s_ezlopi_expressions_t *curr_exp = EZPI_scenes_expressions_get_node_by_name(expression_name);
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
int EZPI_core_scene_then_group_toggle_value(l_scenes_list_v2_t *curr_scene, void *arg)
{
    int ret = 0;
    if (curr_scene)
    {
        uint32_t device_group_id = 0;
        uint32_t item_group_id = 0;

        l_action_block_v2_t *curr_then = (l_action_block_v2_t *)arg;
        if (curr_then)
        {
            l_fields_v2_t *curr_field = curr_then->fields;
            while (curr_field)
            {
                size_t len = (curr_field->name ? strlen(curr_field->name) + 1 : 0);
                if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_deviceGroup_str, len, 12))
                {
                    if ((EZLOPI_VALUE_TYPE_DEVICE_GROUP == curr_field->value_type) && (NULL != curr_field->field_value.u_value.value_string))
                    {
                        device_group_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
                    }
                }
                else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_itemGroup_str, len, 10))
                {
                    if ((EZLOPI_VALUE_TYPE_ITEM_GROUP == curr_field->value_type) && (NULL != curr_field->field_value.u_value.value_string))
                    {
                        item_group_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
                    }
                }

                curr_field = curr_field->next;
            }

            if (device_group_id && item_group_id)
            {
                l_ezlopi_device_grp_t *curr_devgrp = EZPI_core_device_group_get_by_id(device_group_id);
                if (curr_devgrp)
                {
                    cJSON *cj_get_devarr = NULL;
                    cJSON_ArrayForEach(cj_get_devarr, curr_devgrp->devices)
                    {
                        uint32_t curr_device_id = strtoul(cj_get_devarr->valuestring, NULL, 16);
                        l_ezlopi_device_t *curr_device = EZPI_core_device_get_by_id(curr_device_id); // immediately goto "102ec000" ...
                        if (curr_device)
                        {
                            l_ezlopi_item_t *curr_item_node = curr_device->items; // perform operation on items of above device --> "102ec000"
                            while (curr_item_node)
                            {
                                // compare with items_list stored in item_group_id
                                l_ezlopi_item_grp_t *curr_item_grp = EZPI_core_item_group_get_by_id(item_group_id); // get  "ll_itemgrp_node"
                                if (curr_item_grp)
                                {
                                    cJSON *cj_item_names = NULL;
                                    cJSON_ArrayForEach(cj_item_names, curr_item_grp->item_names)
                                    {
                                        uint32_t req_item_id_from_itemgrp = strtoul(cj_item_names->valuestring, NULL, 16);
                                        // if the item_ids match ; Then compare the "item_values" with that of the "scene's" requirement
                                        if (req_item_id_from_itemgrp == curr_item_node->cloud_properties.item_id)
                                        {
                                            ret = EZPI_core_scenes_then_helper_toggleValue(req_item_id_from_itemgrp, cj_item_names->valuestring);
                                        }
                                    }
                                }
                                curr_item_node = curr_item_node->next;
                            }
                        }
                    }
                }
            }
        }
    }
    return ret;
}

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS
       /*******************************************************************************
        *                          End of File
        *******************************************************************************/