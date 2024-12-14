/* ===========================================================================
** Copyright (C) 2024 Ezlo Innovation Inc
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
* @file    ezlopi_core_scenes_when_methods.c
* @brief   Functions that operates on scene-when-methods
* @author  xx
* @version 0.1
* @date    12th DEC 2024
*/

/*******************************************************************************
*                          Include Files
*******************************************************************************/
#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

// #include <time.h>
// #include <string.h>
#include "ezlopi_util_trace.h"

// #include "ezlopi_core_devices.h"
#include "ezlopi_core_modes.h"
#include "ezlopi_core_sntp.h"
#include "ezlopi_core_ota.h"
#include "ezlopi_core_http.h"
#include "ezlopi_core_device_group.h"
#include "ezlopi_core_event_group.h"
#include "ezlopi_core_scenes_operators.h"
#include "ezlopi_core_scenes_expressions.h"
#include "ezlopi_core_websocket_client.h"
#include "ezlopi_core_scenes_when_methods.h"
#include "ezlopi_core_scenes_status_changed.h"
#include "ezlopi_core_scenes_when_methods_helper_functions.h"
#include "ezlopi_core_errors.h"

#include "ezlopi_service_meshbot.h"
#include "ezlopi_cloud_constants.h"

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

int EZPI_core_scenes_when_is_item_state(l_scenes_list_v2_t *scene_node, void *arg)
{
    // TRACE_W(" is_item_state ");
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block && scene_node)
    {
        if (false == when_block->block_enable)
        {
            TRACE_D("Block-disabled [%s]", when_block->block_options.method.name);
            return 0;
        }

        if (true == when_block->block_status_reset_once)
        {
            when_block->block_status_reset_once = false;
            return 0;
        }

        uint32_t item_id = 0;
        uint32_t device_group_id = 0;
        uint32_t item_group_id = 0;
        bool armed_check = false;
        bool value_armed = false;

        l_fields_v2_t *value_field = NULL;
        l_fields_v2_t *curr_field = when_block->fields;
        size_t name_len = 0;
        while (curr_field)
        {
            name_len = (curr_field->name ? strlen(curr_field->name) + 1 : 0);
            if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_item_str, name_len, 5) && (NULL != curr_field->field_value.u_value.value_string))
            {
                item_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_value_str, name_len, 6) && (NULL != curr_field->field_value.u_value.value_string))
            {
                value_field = curr_field;
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_deviceGroup_str, name_len, 12) && (NULL != curr_field->field_value.u_value.value_string))
            {
                device_group_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_itemGroup_str, name_len, 10) && (NULL != curr_field->field_value.u_value.value_string))
            {
                item_group_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_armed_str, name_len, 6))
            {
                if (EZLOPI_VALUE_TYPE_BOOL == curr_field->value_type)
                {
                    armed_check = true;
                    value_armed = curr_field->field_value.u_value.value_bool;
                }
            }

            curr_field = curr_field->next;
        }
        // 1. check for item_value
        if (item_id && value_field)
        {
            ret = ISITEM_state_single_condition(item_id, value_field);
        }
        else if (device_group_id && item_group_id && value_field) // since device_and_item group both need to exist
        {
            ret = ISITEM_state_with_grp_condition(device_group_id, item_group_id, value_field);
        }
        // 2. check for armed condition
        if (ret && armed_check)
        {
            armed_check = false;
            l_ezlopi_device_t *device_node = ezlopi_device_get_head();
            while (device_node)
            {
                l_ezlopi_item_t *item_node = device_node->items;
                while (item_node)
                {
                    if (item_id == item_node->cloud_properties.item_id)
                    {
                        ret = ((value_armed == device_node->cloud_properties.armed) ? 1 : 0);
                        // s_ezlopi_cloud_controller_t *controller_info = ezlopi_device_get_controller_information();
                        // if (controller_info)
                        // {
                        // #warning "we need to change from 'controller' to 'device_id' specific";
                        //     ret = ((value_armed == controller_info->armed) ? 1 : 0);
                        // }
                        armed_check = true;
                        break;
                    }
                    item_node = item_node->next;
                }
                if (armed_check)
                {
                    break;
                }
                device_node = device_node->next;
            }
        }
    }

    return ret;
}

int EZPI_core_scenes_when_is_interval(l_scenes_list_v2_t *scene_node, void *arg)
{
    // TRACE_W(" is_interval ");
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block && scene_node)
    {
        if (false == when_block->block_enable)
        {
            TRACE_D("Block-disabled [%s]", when_block->block_options.method.name);
            return 0;
        }

        if (true == when_block->block_status_reset_once)
        {
            when_block->block_status_reset_once = false;
            return 0;
        }

        char *end_prt = NULL;
        uint32_t interval = strtoul(scene_node->when_block->fields->field_value.u_value.value_string, &end_prt, 10);
        if (end_prt)
        {
            if (EZPI_STRNCMP_IF_EQUAL(end_prt, "m", strlen(end_prt) + 1, 2))
            {
                interval *= 60;
            }
            else if (EZPI_STRNCMP_IF_EQUAL(end_prt, "h", strlen(end_prt) + 1, 2))
            {
                interval *= (60 * 60);
            }
            else if (EZPI_STRNCMP_IF_EQUAL(end_prt, "d", strlen(end_prt) + 1, 2))
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
                scene_node->when_block->fields->user_arg = (void *)xTaskGetTickCount();
                ret = 1;
            }
        }
        else
        {
            ret = 1;
            scene_node->when_block->fields->user_arg = (void *)xTaskGetTickCount();
        }
    }
    return ret;
}

int EZPI_core_scenes_when_is_item_state_changed(l_scenes_list_v2_t *scene_node, void *arg)
{
    int ret = 0;
    // TRACE_W("Warning: when-method 'is_item_state_changed' not implemented!");
    // TRACE_D(" is_item_state_changed ");
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block && scene_node)
    {
        if (false == when_block->block_enable)
        {
            TRACE_D("Block-disabled [%s]", when_block->block_options.method.name);
            return 0;
        }

        if (true == when_block->block_status_reset_once)
        {
            when_block->block_status_reset_once = false;
            return 0;
        }

        l_fields_v2_t *item_exp_field = NULL;
        l_fields_v2_t *start_field = NULL;
        l_fields_v2_t *finish_field = NULL;

        l_fields_v2_t *curr_field = when_block->fields;
        size_t name_len = 0;
        while (curr_field)
        {
            name_len = (curr_field->name ? strlen(curr_field->name) + 1 : 0);
            if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_item_str, name_len, 5))
            {
                if (EZLOPI_VALUE_TYPE_ITEM == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    item_exp_field = curr_field;
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_expression_str, name_len, 11))
            {
                if (EZLOPI_VALUE_TYPE_EXPRESSION == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    item_exp_field = curr_field;
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_start_str, name_len, 6)) // this indicates the item/expression must have the "prev-val == start_field"
            {
                if (EZLOPI_VALUE_TYPE_NONE < curr_field->value_type && curr_field->value_type < EZLOPI_VALUE_TYPE_MAX)
                {
                    start_field = curr_field;
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_finish_str, name_len, 7)) // this indicates the item/expression must have the "new-val == finish_field"
            {
                if (EZLOPI_VALUE_TYPE_NONE < curr_field->value_type && curr_field->value_type < EZLOPI_VALUE_TYPE_MAX)
                {
                    finish_field = curr_field;
                }
            }
            curr_field = curr_field->next;
        }

        if (item_exp_field)
        {
            // A. Extraction and storing the new data.
            s_item_exp_data_t *new_extract_data = ezlopi_malloc(__FUNCTION__, sizeof(s_item_exp_data_t));
            if (new_extract_data)
            {
                memset(new_extract_data, 0, sizeof(s_item_exp_data_t));

                if (EZLOPI_VALUE_TYPE_EXPRESSION == item_exp_field->value_type) // EXPRESSION
                {
                    new_extract_data->value_type = EZLOPI_VALUE_TYPE_EXPRESSION;
                    s_ezlopi_expressions_t *curr_expr_left = EZPI_scenes_expressions_get_node_by_name(item_exp_field->field_value.u_value.value_string);
                    if (curr_expr_left)
                    {
                        if (curr_expr_left->variable) // 1. experssion is 'variable-type'
                        {
                            switch (curr_expr_left->value_type) // the main value type to consider when comparing
                            {
                            case EXPRESSION_VALUE_TYPE_STRING:
                            {
                                new_extract_data->sample_data.e_type = VALUE_TYPE_STRING;
                                size_t value_len = strlen(curr_expr_left->exp_value.u_value.str_value) + 1;

                                new_extract_data->sample_data.u_value.value_string = ezlopi_malloc(__FUNCTION__, value_len);
                                if (new_extract_data->sample_data.u_value.value_string)
                                {
                                    snprintf(new_extract_data->sample_data.u_value.value_string, value_len, "%s", curr_expr_left->exp_value.u_value.str_value);
                                    TRACE_D("copied : %s", new_extract_data->sample_data.u_value.value_string);
                                }
                                break;
                            }
                            case EXPRESSION_VALUE_TYPE_BOOL:
                            {
                                new_extract_data->sample_data.e_type = VALUE_TYPE_BOOL;
                                new_extract_data->sample_data.u_value.value_bool = curr_expr_left->exp_value.u_value.boolean_value;
                                break;
                            }
                            case EXPRESSION_VALUE_TYPE_NUMBER:
                            {
                                new_extract_data->sample_data.e_type = VALUE_TYPE_NUMBER;
                                new_extract_data->sample_data.u_value.value_double = curr_expr_left->exp_value.u_value.number_value;
                                break;
                            }
                            default:
                                break;
                            }
                        }
                        else // 2. expression is 'expression-type'
                        {
                            cJSON *cj_expr_des = cJSON_CreateObject(__FUNCTION__);
                            if (cj_expr_des && (0 < EZPI_scenes_expressions_eval_simple(cj_expr_des, curr_expr_left->name, curr_expr_left->code)))
                            {
                                cJSON *cj_value = cJSON_GetObjectItem(__FUNCTION__, cj_expr_des, ezlopi_value_str);
                                if (cj_value)
                                {
                                    switch (cj_value->type)
                                    {
                                    case cJSON_True:
                                    case cJSON_False:
                                    {
                                        new_extract_data->sample_data.e_type = VALUE_TYPE_BOOL;
                                        new_extract_data->sample_data.u_value.value_bool = (cJSON_True == cj_value->type ? 1 : 0);
                                        break;
                                    }
                                    case cJSON_Number:
                                    {
                                        new_extract_data->sample_data.e_type = VALUE_TYPE_NUMBER;
                                        new_extract_data->sample_data.u_value.value_double = cJSON_GetNumberValue(cj_value);
                                        break;
                                    }
                                    case cJSON_String:
                                    {
                                        new_extract_data->sample_data.e_type = VALUE_TYPE_STRING;
                                        size_t value_len = strlen(cj_value->valuestring) + 1;

                                        new_extract_data->sample_data.u_value.value_string = ezlopi_malloc(__FUNCTION__, value_len);
                                        if (new_extract_data->sample_data.u_value.value_string)
                                        {
                                            snprintf(new_extract_data->sample_data.u_value.value_string, value_len, "%s", cj_value->valuestring);
                                            TRACE_D("copied : %s", new_extract_data->sample_data.u_value.value_string);
                                        }
                                        break;
                                    }
                                    default:
                                        break;
                                    }
                                }
                                cJSON_Delete(__FUNCTION__, cj_expr_des);
                            }
                        }
                    }
                }
                else // ITEM
                {
                    new_extract_data->value_type = EZLOPI_VALUE_TYPE_ITEM;
                    uint32_t item_id = strtoul(item_exp_field->field_value.u_value.value_string, NULL, 16);
                    l_ezlopi_item_t *curr_item = ezlopi_device_get_item_by_id(item_id);
                    if (curr_item)
                    {
                        cJSON *cj_item_value = cJSON_CreateObject(__FUNCTION__);
                        if (cj_item_value)
                        {
                            curr_item->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, curr_item, (void *)cj_item_value, NULL);
                            cJSON *cj_value = cJSON_GetObjectItem(__FUNCTION__, cj_item_value, ezlopi_value_str);
                            if (cj_value)
                            {
                                switch (cj_value->type)
                                {
                                case cJSON_True:
                                case cJSON_False:
                                {
                                    new_extract_data->sample_data.e_type = VALUE_TYPE_BOOL;
                                    new_extract_data->sample_data.u_value.value_bool = (cJSON_True == cj_value->type) ? 1 : 0;
                                    break;
                                }
                                case cJSON_Number:
                                {
                                    new_extract_data->sample_data.e_type = VALUE_TYPE_NUMBER;
                                    new_extract_data->sample_data.u_value.value_double = cj_value->valuedouble;
                                    break;
                                }
                                case cJSON_String:
                                {
                                    new_extract_data->sample_data.e_type = VALUE_TYPE_STRING;
                                    size_t value_len = strlen(cj_value->valuestring) + 1;

                                    new_extract_data->sample_data.u_value.value_string = ezlopi_malloc(__FUNCTION__, value_len);
                                    if (new_extract_data->sample_data.u_value.value_string)
                                    {
                                        snprintf(new_extract_data->sample_data.u_value.value_string, value_len, "%s", cj_value->valuestring);
                                        TRACE_D("copied : %s", new_extract_data->sample_data.u_value.value_string);
                                    }
                                    break;
                                }
                                default:
                                    break;
                                }
                            }

                            cJSON_Delete(__FUNCTION__, cj_item_value);
                        }
                    }
                }

                // Perform Operation
                if (1 == (ret = IS_itemstate_changed(new_extract_data, start_field, finish_field, scene_node)))
                {
                    TRACE_S("Activating THEN-METHOD");
                }

                // remove malloc for 'extracted_data'
                if (VALUE_TYPE_STRING == new_extract_data->sample_data.e_type)
                {
                    if (new_extract_data->sample_data.u_value.value_string)
                    {
                        ezlopi_free(__FUNCTION__, new_extract_data->sample_data.u_value.value_string);
                        new_extract_data->sample_data.u_value.value_string = NULL;
                    }
                }
                ezlopi_free(__FUNCTION__, new_extract_data);
            }
        }
    }
    return ret;
}

int EZPI_core_scenes_when_is_button_state(l_scenes_list_v2_t *scene_node, void *arg)
{
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block && scene_node)
    {
        if (false == when_block->block_enable)
        {
            TRACE_D("Block-disabled [%s]", when_block->block_options.method.name);
            return 0;
        }

        if (true == when_block->block_status_reset_once)
        {
            when_block->block_status_reset_once = false;
            return 0;
        }

        uint32_t item_id = 0;
        l_fields_v2_t *value_field = NULL;
        l_fields_v2_t *curr_field = when_block->fields;
        size_t name_len = 0;
        while (curr_field)
        {
            name_len = (curr_field->name ? strlen(curr_field->name) + 1 : 0);
            if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_item_str, name_len, 5) && (NULL != curr_field->field_value.u_value.value_string))
            {
                item_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_value_str, name_len, 6) && (NULL != curr_field->field_value.u_value.value_string))
            {
                if (EZLOPI_VALUE_TYPE_TOKEN == curr_field->value_type)
                {
                    value_field = curr_field;
                }
            }

            curr_field = curr_field->next;
        }
        // 1. check for item_value
        if (item_id && value_field)
        {
            ret = ISITEM_state_single_condition(item_id, value_field);
        }

    }

    return ret;
}

int EZPI_core_scenes_when_is_sun_state(l_scenes_list_v2_t *scene_node, void *arg)
{
    // TRACE_W(" is_SunState ");
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block && scene_node)
    {
        if (false == when_block->block_enable)
        {
            TRACE_D("Block-disabled [%s]", when_block->block_options.method.name);
            return 0;
        }

        if (true == when_block->block_status_reset_once)
        {
            when_block->block_status_reset_once = false;
            return 0;
        }

        // if (0 < ezlopi_event_group_wait_for_event(EZLOPI_EVENT_NMA_REG, 100, false))
        // {
        //     TRACE_W("module not online");
        //     return 0;
        // }

        time_t rawtime = 0;
        time(&rawtime);
        struct tm *info;
        info = localtime(&rawtime);

        // list of function for extracting field parameter
        const s_issunstate_method_t __issunstate_field[] = {
            {.field_name = ezlopi_sunrise_str, .field_func = ISSUNSTATE_get_suntime},
            {.field_name = ezlopi_sunset_str, .field_func = ISSUNSTATE_get_suntime},
            {.field_name = ezlopi_time_str, .field_func = ISSUNSTATE_get_offs_tmval},
            {.field_name = ezlopi_weekdays_str, .field_func = ISSUNSTATE_eval_weekdays},
            {.field_name = ezlopi_days_str, .field_func = ISSUNSTATE_eval_days},
            {.field_name = ezlopi_range_str, .field_func = ISSUNSTATE_eval_range},
            {.field_name = NULL, .field_func = NULL},
        };

        // Condition checker
        uint8_t flag_check = 0;
        l_fields_v2_t *curr_field = when_block->fields;
        while (NULL != curr_field)
        {
            for (uint8_t i = 0; i < ((sizeof(__issunstate_field) / sizeof(__issunstate_field[i]))); i++)
            {
                if (EZPI_STRNCMP_IF_EQUAL(__issunstate_field[i].field_name, curr_field->name, strlen(__issunstate_field[i].field_name) + 1, strlen(curr_field->name) + 1))
                {
                    flag_check |= (__issunstate_field[i].field_func)(scene_node, curr_field, info, ((0 == i) ? 1 : (1 == i) ? 2 : 0));
                    break;
                }
            }
            curr_field = curr_field->next;
        }
        // Now check the flag results
        ret = ISSUNSTATE_check_flag_result(scene_node, info, flag_check);
    }
    return ret;
}

int EZPI_core_scenes_when_is_date(l_scenes_list_v2_t *scene_node, void *arg)
{
    // TRACE_W(" isDate ");
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block && scene_node)
    {
        if (false == when_block->block_enable)
        {
            TRACE_D("Block-disabled [%s]", when_block->block_options.method.name);
            return 0;
        }

        if (true == when_block->block_status_reset_once)
        {
            when_block->block_status_reset_once = false;
            return 0;
        }

        time_t rawtime = 0;
        time(&rawtime);
        struct tm *info;
        info = localtime(&rawtime);
        if (2 == info->tm_sec) // nth sec mark
        {
            // list of field function to extract the respective parameters
            const s_isdate_method_t __isdate_func[] = {
                {.field_name = ezlopi_type_str, .field_func = ISDATE_type_check},
                {.field_name = ezlopi_time_str, .field_func = ISDATE_tm_check},
                {.field_name = ezlopi_weekdays_str, .field_func = ISDATE_weekdays_check},
                {.field_name = ezlopi_days_str, .field_func = ISDATE_mdays_check},
                {.field_name = ezlopi_weeks_str, .field_func = ISDATE_year_weeks_check},
                {.field_name = NULL, .field_func = NULL},
            };
            uint8_t flag_check = 0;
            e_isdate_modes_t mode_type = ISDATE_UNDEFINED_MODE;
            l_fields_v2_t *curr_field = when_block->fields;
            while (NULL != curr_field)
            {
                for (uint8_t i = 0; i < ((sizeof(__isdate_func) / sizeof(__isdate_func[i]))); i++)
                {
                    if (EZPI_STRNCMP_IF_EQUAL(__isdate_func[i].field_name, curr_field->name, strlen(__isdate_func[i].field_name) + 1, strlen(curr_field->name) + 1))
                    {
                        flag_check |= (__isdate_func[i].field_func)(&mode_type, info, curr_field); // bit0 - bit3
                        break;
                    }
                }
                curr_field = curr_field->next;
            }
            ret = ISDATE_check_flag_result(mode_type, flag_check);
            // Output Filter based on date+time of activation
            // TRACE_S("mode[%d], isDate:- FLAG_STATUS: %#x", mode_type, flag_check);
        }
    }
    return ret;
}

int EZPI_core_scenes_when_is_once(l_scenes_list_v2_t *scene_node, void *arg)
{
    // TRACE_W(" isOnce ");
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block && scene_node)
    {
        if (false == when_block->block_enable)
        {
            TRACE_D("Block-disabled [%s]", when_block->block_options.method.name);
            return 0;
        }

        if (true == when_block->block_status_reset_once)
        {
            when_block->block_status_reset_once = false;
            return 0;
        }

        time_t rawtime = 0;
        time(&rawtime);
        struct tm *info;
        info = localtime(&rawtime);

        if (4 == info->tm_sec) // nth sec mark
        {
            // list of funciton to check validity of each field values
            const s_isonce_method_t __isonce_method[] = {
                {.field_name = ezlopi_time_str, .field_func = ISONCE_tm_check},
                {.field_name = ezlopi_day_str, .field_func = ISONCE_day_check},
                {.field_name = ezlopi_month_str, .field_func = ISONCE_month_check},
                {.field_name = ezlopi_year_str, .field_func = ISONCE_year_check},
                {.field_name = NULL, .field_func = NULL},
            };
            uint8_t flag_check = 0;
            l_fields_v2_t *curr_field = when_block->fields;
            while (curr_field)
            {
                for (uint8_t i = 0; i < ((sizeof(__isonce_method) / sizeof(__isonce_method[i]))); i++)
                {
                    if (EZPI_STRNCMP_IF_EQUAL(__isonce_method[i].field_name, curr_field->name, strlen(__isonce_method[i].field_name) + 1, strlen(curr_field->name) + 1))
                    {
                        flag_check |= (__isonce_method[i].field_func)(curr_field, info);
                        break;
                    }
                }
                curr_field = curr_field->next;
            }

            // Output Filter based on date & time
            ret = ISONCE_check_flag_result(scene_node, flag_check);
            // TRACE_S("isOnce :- FLAG_STATUS: 0x0%x", flag_check);
        }
    }
    return ret;
}

int EZPI_core_scenes_when_is_date_range(l_scenes_list_v2_t *scene_node, void *arg)
{
    // TRACE_W(" isDate_range ");
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block && scene_node)
    {
        if (false == when_block->block_enable)
        {
            TRACE_D("Block-disabled [%s]", when_block->block_options.method.name);
            return 0;
        }

        if (true == when_block->block_status_reset_once)
        {
            when_block->block_status_reset_once = false;
            return 0;
        }

        time_t rawtime = 0;
        time(&rawtime);
        struct tm *info;
        info = localtime(&rawtime);

        if (6 == info->tm_sec) // nth sec mark
        {
            // Default values : start and end times.
            struct tm start = {
                .tm_hour = 0,
                .tm_min = 0,
            };
            struct tm end = {
                .tm_hour = 23,
                .tm_min = 59,
            };

            // field function pointers
            const s_isdate_range_method_t _isdate_range_fields[] = {
                {.field_name = "startTime", .field_func = ISDATE_range_get_tm},
                {.field_name = "startDay", .field_func = ISDATE_range_get_startday},
                {.field_name = "startMonth", .field_func = ISDATE_range_get_startmonth},
                {.field_name = "startYear", .field_func = ISDATE_range_get_startyear},
                {.field_name = "endTime", .field_func = ISDATE_range_get_tm},
                {.field_name = "endDay", .field_func = ISDATE_range_get_endday},
                {.field_name = "endMonth", .field_func = ISDATE_range_get_endmonth},
                {.field_name = "endYear", .field_func = ISDATE_range_get_endyear},
                {.field_name = NULL, .field_func = NULL},
            };
            l_fields_v2_t *curr_field = when_block->fields;
            while (curr_field)
            {
                for (int i = 0; i < ((sizeof(_isdate_range_fields) / sizeof(_isdate_range_fields[i]))); i++)
                {
                    if (EZPI_STRNCMP_IF_EQUAL(_isdate_range_fields[i].field_name, curr_field->name, strlen(_isdate_range_fields[i].field_name) + 1, strlen(curr_field->name) + 1))
                    {
                        (_isdate_range_fields[i].field_func)(curr_field, ((i < 4) ? &start : &end));
                        break;
                    }
                }
                curr_field = curr_field->next;
            }

            // Check for time,day,month and year validity
            uint8_t(*ISDATE_range_check_flags[])(struct tm *start, struct tm *end, struct tm *info) = {
                ISDATE_range_check_tm,
                ISDATE_range_check_day,
                ISDATE_range_check_month,
                ISDATE_range_check_year,
            };
            uint8_t flag_check = 0;
            for (uint8_t i = 0; i < ISDATE_RANGE_MAX; i++)
            {
                flag_check |= ISDATE_range_check_flags[i](&start, &end, info);
            }

            ret = ISDATE_range_check_flag_result(flag_check);
            TRACE_S("isdate_range flag_check [0x0%x]", flag_check);
        }
    }
    return ret;
}

int EZPI_core_scenes_when_is_user_lock_operation(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_W("Warning: when-method 'is_user_lock_operation' not implemented!");
    return 0;
}

#if defined(CONFIG_EZPI_SERV_ENABLE_MODES)
int EZPI_core_scenes_when_is_house_mode_changed_to(l_scenes_list_v2_t *scene_node, void *arg)
{
    // TRACE_W(" isHouse_modechanged_to");
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;

    if (when_block)
    {
        if (false == when_block->block_enable)
        {
            TRACE_D("Block-disabled [%s]", when_block->block_options.method.name);
            return 0;
        }

        if (true == when_block->block_status_reset_once)
        {
            when_block->block_status_reset_once = false;
            return 0;
        }

        l_fields_v2_t *house_mode_id_array = NULL;
        l_fields_v2_t *curr_field = when_block->fields;

        while (curr_field)
        {
            if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_houseMode_str, strlen(curr_field->name) + 1, 10))
            {
                if (EZLOPI_VALUE_TYPE_HOUSE_MODE_ID_ARRAY == curr_field->value_type)
                {
                    house_mode_id_array = curr_field;
                }
            }

            curr_field = curr_field->next;
        }

        cJSON *cj_house_mdoe_id = NULL;

        cJSON_ArrayForEach(cj_house_mdoe_id, house_mode_id_array->field_value.u_value.cj_value)
        {
            if (cj_house_mdoe_id->valuestring)
            {
                uint32_t house_mode_id = strtoul(cj_house_mdoe_id->valuestring, NULL, 16);
                s_ezlopi_modes_t *modes = EZPI_core_modes_get_custom_modes();
                if ((uint32_t)house_mode_id_array->user_arg != modes->current_mode_id) /* first check if there is transition */
                {
                    if (modes->current_mode_id == house_mode_id) /* if : new_state == desired */
                    {
                        ret = 1;
                    }
                    house_mode_id_array->user_arg = (void *)house_mode_id; // updates every evaluation
                    // TRACE_E("house-mode-changed-to: %d", house_mode_id);
                }
            }
        }
    }

    return ret;
}

int EZPI_core_scenes_when_is_house_mode_changed_from(l_scenes_list_v2_t *scene_node, void *arg)
{
    // TRACE_W(" isHouse_mode_changed_from");
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;

    if (when_block)
    {
        if (false == when_block->block_enable)
        {
            TRACE_D("Block-disabled [%s]", when_block->block_options.method.name);
            return 0;
        }

        if (true == when_block->block_status_reset_once)
        {
            when_block->block_status_reset_once = false;
            return 0;
        }

        l_fields_v2_t *house_mode_id_array = NULL;
        l_fields_v2_t *curr_field = when_block->fields;

        while (curr_field)
        {
            if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_houseMode_str, strlen(curr_field->name) + 1, 10))
            {
                if (EZLOPI_VALUE_TYPE_HOUSE_MODE_ID_ARRAY == curr_field->value_type)
                {
                    house_mode_id_array = curr_field;
                }
            }
            curr_field = curr_field->next;
        }

        cJSON *cj_house_mode_id = NULL;
        cJSON_ArrayForEach(cj_house_mode_id, house_mode_id_array->field_value.u_value.cj_value)
        {
            if (cj_house_mode_id->valuestring)
            {
                uint32_t house_mode_id = strtoul(cj_house_mode_id->valuestring, NULL, 16); // "value": [ "1", "2"]
                s_ezlopi_modes_t *modes = EZPI_core_modes_get_custom_modes();
                if ((uint32_t)house_mode_id_array->user_arg != modes->current_mode_id) /* first check if there is transition */
                {
                    if ((uint32_t)house_mode_id_array->user_arg == house_mode_id) /* if : old_state == desired */
                    {
                        ret = 1;
                        house_mode_id_array->user_arg = (void *)house_mode_id; // updates every evaluation
                        break;
                    }
                    else
                    {
                        house_mode_id_array->user_arg = (void *)house_mode_id; // updates every evaluation
                    }
                    // TRACE_E("house-mode-changed-to: %d", house_mode_id);
                }
            }
        }
    }
    return ret;
}

int EZPI_core_scenes_when_is_house_mode_alarm_phase_range(l_scenes_list_v2_t *scene_node, void *arg)
{
    // TRACE_W(" is_House_Mode_Alarm_Phase_Range ");
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;

    if (when_block && scene_node)
    {
        if (false == when_block->block_enable)
        {
            TRACE_D("Block-disabled [%s]", when_block->block_options.method.name);
            return 0;
        }

        if (true == when_block->block_status_reset_once)
        {
            when_block->block_status_reset_once = false;
            return 0;
        }

        char *phase_name = NULL;
        l_fields_v2_t *curr_field = when_block->fields;

        while (curr_field)
        {
            if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_phase_str, strlen(curr_field->name) + 1, 10))
            {
                if ((EZLOPI_VALUE_TYPE_STRING == curr_field->value_type) && (NULL != curr_field->field_value.u_value.value_string))
                {
                    s_ezlopi_modes_t *curr_mode = EZPI_core_modes_get_custom_modes();

                    phase_name = ((EZLOPI_MODES_ALARM_PHASE_IDLE == curr_mode->alarmed.phase) ? (char *)ezlopi_idle_str
                        : (EZLOPI_MODES_ALARM_PHASE_BYPASS == curr_mode->alarmed.phase) ? (char *)ezlopi_bypass_str
                        : (EZLOPI_MODES_ALARM_PHASE_ENTRYDELAY == curr_mode->alarmed.phase) ? (char *)ezlopi_entryDelay_str
                        : (EZLOPI_MODES_ALARM_PHASE_MAIN == curr_mode->alarmed.phase) ? (char *)ezlopi_main_str
                        : ezlopi_null_str);

                    // TRACE_D(" req_mode : %s vs mode : %s ", curr_field->field_value.u_value.value_string, phase_name);
                    if (EZPI_STRNCMP_IF_EQUAL(curr_field->field_value.u_value.value_string, phase_name, strlen(curr_field->field_value.u_value.value_string) + 1, strlen(phase_name) + 1))
                    {
                        ret = 1;
                    }
                }
            }
            curr_field = curr_field->next;
        }
    }
    return ret;
}

int EZPI_core_scenes_when_is_house_mode_switch_to_range(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_W(" is_House_Mode_Switch_to_Range ");
    int ret = 0;

    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;

    if (when_block && scene_node)
    {
        if (false == when_block->block_enable)
        {
            TRACE_D("Block-disabled [%s]", when_block->block_options.method.name);
            return 0;
        }

        if (true == when_block->block_status_reset_once)
        {
            when_block->block_status_reset_once = false;
            return 0;
        }

        s_ezlopi_modes_t *curr_mode = EZPI_core_modes_get_custom_modes();
        if ((curr_mode->time_is_left_to_switch_sec > 0) && (EZLOPI_MODES_ALARM_PHASE_NONE < curr_mode->alarmed.phase && curr_mode->alarmed.phase < EZLOPI_MODES_ALARM_PHASE_MAX))
        {
            TRACE_S(" Current HouseMode_phase has [%d]sec time before switch.(Duration confirmed) ", curr_mode->time_is_left_to_switch_sec);
            ret = 1;
        }
        else
        {
            TRACE_E(" Current HouseMode_phase does not have duration ");
        }
    }

    return ret;
}
#endif // CONFIG_EZPI_SERV_ENABLE_MODES

int EZPI_core_scenes_when_is_device_state(l_scenes_list_v2_t *scene_node, void *arg)
{
    // TRACE_W(" isDevice_state. ");
    int ret = 0;

    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block && scene_node)
    {
        if (false == when_block->block_enable)
        {
            TRACE_D("Block-disabled [%s]", when_block->block_options.method.name);
            return 0;
        }

        if (true == when_block->block_status_reset_once)
        {
            when_block->block_status_reset_once = false;
            return 0;
        }

        uint32_t device_id = 0;
        uint32_t device_group_id = 0;

        bool value_armed = false;
        bool value_reachable = false;

        l_fields_v2_t *curr_field = when_block->fields;
        size_t name_len = 0;
        while (curr_field)
        {
            name_len = (curr_field->name ? strlen(curr_field->name) + 1 : 0);
            if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, dev_type_device, name_len, 7))
            {
                if (EZLOPI_VALUE_TYPE_DEVICE == curr_field->value_type && (NULL != curr_field->field_value.u_value.value_string))
                {
                    device_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_armed_str, name_len, 6))
            {
                if (EZLOPI_VALUE_TYPE_BOOL == curr_field->value_type)
                {
                    value_armed = curr_field->field_value.u_value.value_bool;
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_reachable_str, name_len, 10))
            {
                if (EZLOPI_VALUE_TYPE_BOOL == curr_field->value_type)
                {
                    value_reachable = curr_field->field_value.u_value.value_bool;
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_deviceGroup_str, name_len, 12) && (NULL != curr_field->field_value.u_value.value_string))
            {
                device_group_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
            }
            curr_field = curr_field->next;
        }

        if (device_id)
        {
            l_ezlopi_device_t *curr_device = ezlopi_device_get_by_id(device_id);
            if (curr_device)
            {
                // ret = ((value_armed == curr_device->cloud_properties.armed) ? 1 : 0);
                s_ezlopi_cloud_controller_t *controller_info = ezlopi_device_get_controller_information();
                if (controller_info)
                {
                    #warning "we need to change from 'controller' to 'device_id' specific";
                    ret = ((value_armed == controller_info->armed) ? 1 : 0) && ((value_reachable == controller_info->service_notification) ? 1 : 0);
                }
            }
        }
        else if (device_group_id)
        {
            l_ezlopi_device_grp_t *curr_devgrp = ezlopi_core_device_group_get_by_id(device_group_id);
            if (curr_devgrp)
            {
                cJSON *cj_device = NULL;
                cJSON_ArrayForEach(cj_device, curr_devgrp->devices)
                {
                    uint32_t curr_devce_id = strtoul(cj_device->valuestring, NULL, 16);
                    l_ezlopi_device_t *curr_device = ezlopi_device_get_by_id(curr_devce_id); // immediately goto "102ec000" ...
                    if (curr_device)
                    {
                        //  ret = ((value_armed == curr_device->cloud_properties.armed) ? 1 : 0);
                        s_ezlopi_cloud_controller_t *controller_info = ezlopi_device_get_controller_information();
                        if (controller_info)
                        {
                            #warning "we need to change from 'controller' to 'device_id' specific";
                            ret = ((value_armed == controller_info->armed) ? 1 : 0) && ((value_reachable == controller_info->service_notification) ? 1 : 0);
                        }
                    }

                    if ((0 < device_id) && (device_id != curr_devce_id))
                    {
                        ret = 0;
                    }
                }
            }
        }
    }

    return ret;
}

int EZPI_core_scenes_when_is_network_state(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_W("Warning: when-method 'is_network_state' not implemented!");
    return 0;
}

int EZPI_core_scenes_when_is_scene_state(l_scenes_list_v2_t *scene_node, void *arg)
{
    // TRACE_W(" isScene_state ");
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block && scene_node)
    {
        if (false == when_block->block_enable)
        {
            TRACE_D("Block-disabled [%s]", when_block->block_options.method.name);
            return 0;
        }

        if (true == when_block->block_status_reset_once)
        {
            when_block->block_status_reset_once = false;
            return 0;
        }

        uint32_t scene_id = 0;
        l_fields_v2_t *value_field = NULL;

        l_fields_v2_t *curr_field = when_block->fields;
        while (curr_field)
        {
            size_t name_len = (curr_field->name ? strlen(curr_field->name) + 1 : 0);
            if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_scene_str, name_len, 6))
            {
                if (EZLOPI_VALUE_TYPE_SCENEID == curr_field->value_type && (NULL != curr_field->field_value.u_value.value_string))
                {
                    scene_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
                    // TRACE_E("scene_id : %d", scene_id);
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_state_str, name_len, 6))
            {
                if (EZLOPI_VALUE_TYPE_TOKEN == curr_field->value_type && (NULL != curr_field->field_value.u_value.value_string))
                {
                    value_field = curr_field;
                }
            }
            curr_field = curr_field->next;
        }

        if (scene_id && value_field)
        {
            const char *tmp_str = value_field->field_value.u_value.value_string;
            size_t tmp_str_len = (tmp_str ? strlen(tmp_str) + 1 : 0);
            l_scenes_list_v2_t *curr_scene = EZPI_core_scenes_get_scene_head_v2();
            while (curr_scene)
            {
                if (curr_scene->_id == scene_id)
                {
                    if (EZPI_STRNCMP_IF_EQUAL(ezlopi_any_result_str, tmp_str, 11, tmp_str_len))
                    {
                        ret = 1;
                    }
                    else if (EZPI_STRNCMP_IF_EQUAL(ezlopi_scene_enabled_str, tmp_str, 14, tmp_str_len))
                    {
                        ret = (true == curr_scene->enabled) ? 1 : 0;
                    }
                    else if (EZPI_STRNCMP_IF_EQUAL(ezlopi_scene_disabled_str, tmp_str, 15, tmp_str_len))
                    {
                        ret = (false == curr_scene->enabled) ? 1 : 0;
                    }
                    else if (EZPI_STRNCMP_IF_EQUAL(scene_status_finished_str, tmp_str, 9, tmp_str_len))
                    {
                        ret = (EZLOPI_SCENE_STATUS_STOP == curr_scene->status) ? 1 : 0;
                    }
                    else if (EZPI_STRNCMP_IF_EQUAL(scene_status_partially_finished_str, tmp_str, 19, tmp_str_len))
                    {
                        ret = (EZLOPI_SCENE_STATUS_RUNNING == curr_scene->status) ? 1 : 0;
                    }
                    else if (EZPI_STRNCMP_IF_EQUAL(scene_status_stopped_str, tmp_str, 8, tmp_str_len))
                    {
                        ret = (EZLOPI_SCENE_STATUS_STOPPED == curr_scene->status) ? 1 : 0;
                    }
                    #warning "need to add 'FAILED' status for scene";
                    // else if (EZPI_STRNCMP_IF_EQUAL("failed", tmp_str, 7,tmp_str_len))
                    // {
                    //     ret = (false == curr_scene->enabled)? 1:0;
                    // }
                    // TRACE_E("scene_state : %s , ret = %d", EZPI_core_scenes_status_to_string(curr_scene->status), ret);
                    break;
                }
                curr_scene = curr_scene->next;
            }
        }
    }

    return ret;
}

int EZPI_core_scenes_when_is_group_state(l_scenes_list_v2_t *scene_node, void *arg)
{
    // TRACE_W("Warning: when-method 'is_group_state' not implemented!");
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block && scene_node)
    {
        if (false == when_block->block_enable)
        {
            TRACE_D("Block-disabled [%s]", when_block->block_options.method.name);
            return 0;
        }

        if (true == when_block->block_status_reset_once)
        {
            when_block->block_status_reset_once = false;
            return 0;
        }

        uint32_t scene_id = 0;
        uint32_t group_id = 0;
        char *state_str = NULL;

        l_fields_v2_t *curr_field = when_block->fields;
        size_t name_len = 0;
        while (curr_field)
        {
            name_len = (curr_field->name ? strlen(curr_field->name) + 1 : 0);
            if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_scene_str, name_len, 6))
            {
                if (EZLOPI_VALUE_TYPE_SCENEID == curr_field->value_type && (NULL != curr_field->field_value.u_value.value_string))
                {
                    scene_id = (uint32_t)strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
                    // TRACE_D("scene_id : %08x", scene_id);
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_group_str, name_len, 6))
            {
                if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && (NULL != curr_field->field_value.u_value.value_string))
                {
                    group_id = (uint32_t)strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
                    // TRACE_D("group_id : %08x", group_id);
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_state_str, name_len, 6))
            {
                if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && (NULL != curr_field->field_value.u_value.value_string))
                {
                    state_str = curr_field->field_value.u_value.value_string;
                    // TRACE_D("state_str : %s", state_str);
                }
            }
            curr_field = curr_field->next;
        }

        if ((0 < scene_id) && (0 < group_id) && (NULL != state_str))
        {
            // 1. find the 'when-grp-block'
            size_t len = (state_str ? strlen(state_str) + 1 : 0);
            l_when_block_v2_t *curr_grp_block = EZPI_core_scenes_get_group_block(scene_id, group_id);
            if (curr_grp_block)
            {
                if (EZPI_STRNCMP_IF_EQUAL(state_str, ezlopi_true_str, len, 5))
                {
                    ret = (curr_grp_block->when_grp->grp_state == true);
                }
                else if (EZPI_STRNCMP_IF_EQUAL(state_str, ezlopi_false_str, len, 6))
                {
                    ret = (curr_grp_block->when_grp->grp_state == false);
                }
                else if (EZPI_STRNCMP_IF_EQUAL(state_str, ezlopi_changed_str, len, 8))
                {
                    if (NULL != scene_node->when_block->fields->user_arg)
                    {
                        ret = ((uint32_t)scene_node->when_block->fields->user_arg == (uint32_t)curr_grp_block->when_grp->grp_state);
                    }
                    scene_node->when_block->fields->user_arg = (void *)curr_grp_block->when_grp->grp_state; // new state
                }
                // TRACE_S("isgroupState__ret :%d", ret);
            }
        }
    }

    return ret;
}

int EZPI_core_scenes_when_is_cloud_state(l_scenes_list_v2_t *scene_node, void *arg)
{
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block && scene_node)
    {
        if (false == when_block->block_enable)
        {
            TRACE_D("Block-disabled [%s]", when_block->block_options.method.name);
            return 0;
        }

        if (true == when_block->block_status_reset_once)
        {
            when_block->block_status_reset_once = false;
            return 0;
        }

        char *field_str_val = NULL;
        l_fields_v2_t *curr_field = when_block->fields;
        while (curr_field)
        {
            if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_state_str, strlen(curr_field->name) + 1, 6))
            {
                if (EZLOPI_VALUE_TYPE_TOKEN == curr_field->value_type && (NULL != curr_field->field_value.u_value.value_string))
                {
                    field_str_val = curr_field->field_value.u_value.value_string;
                }
            }
            curr_field = curr_field->next;
        }

        if (field_str_val)
        {
            if (EZPI_SUCCESS == ezlopi_event_group_wait_for_event(EZLOPI_EVENT_NMA_REG, 100, false))
            {
                ret = (EZPI_STRNCMP_IF_EQUAL(field_str_val, ezlopi_connected_str, strlen(field_str_val) + 1, 10));
            }
            else
            {
                ret = (EZPI_STRNCMP_IF_EQUAL(field_str_val, ezlopi_disconnected_str, strlen(field_str_val) + 1, 13));
            }
        }
    }
    return ret;
}

int EZPI_core_scenes_when_is_battery_state(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_W("Warning: when-method 'is_battery_state' not implemented!");
    return 0;
}

int EZPI_core_scenes_when_is_battery_level(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_W("Warning: when-method 'is_battery_level' not implemented!");
    return 0;
}

int EZPI_core_scene_when_compare_numbers(l_scenes_list_v2_t *scene_node, void *arg)
{
    // TRACE_W(" Compare_num ");
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block && scene_node)
    {
        if (false == when_block->block_enable)
        {
            TRACE_D("Block-disabled [%s]", when_block->block_options.method.name);
            return 0;
        }

        if (true == when_block->block_status_reset_once)
        {
            when_block->block_status_reset_once = false;
            return 0;
        }

        l_fields_v2_t *item_exp_field = NULL;
        l_fields_v2_t *value_field = NULL;
        l_fields_v2_t *comparator_field = NULL;
        l_fields_v2_t *devgrp_field = NULL;
        l_fields_v2_t *itemgrp_field = NULL;

        l_fields_v2_t *curr_field = when_block->fields;
        size_t name_len = 0;
        while (curr_field)
        {
            name_len = (curr_field->name ? strlen(curr_field->name) + 1 : 0);
            if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_item_str, name_len, 5))
            {
                if (EZLOPI_VALUE_TYPE_ITEM == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    item_exp_field = curr_field;
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_expression_str, name_len, 11))
            {
                if (EZLOPI_VALUE_TYPE_EXPRESSION == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    item_exp_field = curr_field; // expression_name
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_comparator_str, name_len, 11))
            {
                comparator_field = curr_field;
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_value_str, name_len, 6))
            {
                if (EZLOPI_VALUE_TYPE_ITEM == curr_field->value_type && (NULL != curr_field->field_value.u_value.value_string))
                {
                    value_field = curr_field;
                }
                else if (EZLOPI_VALUE_TYPE_EXPRESSION == curr_field->value_type && (NULL != curr_field->field_value.u_value.value_string))
                {
                    value_field = curr_field; // this field has expression_name
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_deviceGroup_str, name_len, 12) && (NULL != curr_field->field_value.u_value.value_string))
            {
                devgrp_field = curr_field;
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_itemGroup_str, name_len, 10) && (NULL != curr_field->field_value.u_value.value_string))
            {
                itemgrp_field = curr_field;
            }
            curr_field = curr_field->next;
        }

        ret = EZPI_scenes_operators_value_number_operations(item_exp_field, value_field, comparator_field, devgrp_field, itemgrp_field);
    }

    return ret;
}

int EZPI_core_scene_when_compare_number_range(l_scenes_list_v2_t *scene_node, void *arg)
{
    // TRACE_W(" Compare_num_range");
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block && scene_node)
    {
        if (false == when_block->block_enable)
        {
            TRACE_D("Block-disabled [%s]", when_block->block_options.method.name);
            return 0;
        }

        if (true == when_block->block_status_reset_once)
        {
            when_block->block_status_reset_once = false;
            return 0;
        }

        l_fields_v2_t *item_exp_field = NULL;
        l_fields_v2_t *end_value_field = NULL;
        l_fields_v2_t *start_value_field = NULL;
        l_fields_v2_t *devgrp_field = NULL;
        l_fields_v2_t *itemgrp_field = NULL;
        bool comparator_choice = false; /* 0->'between' | 1->'not_between'*/

        l_fields_v2_t *curr_field = when_block->fields;
        size_t name_len = 0;
        while (curr_field)
        {
            name_len = (curr_field->name ? strlen(curr_field->name) + 1 : 0);
            if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_item_str, name_len, 5))
            {
                if (EZLOPI_VALUE_TYPE_ITEM == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    item_exp_field = curr_field;
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_expression_str, name_len, 11))
            {
                if (EZLOPI_VALUE_TYPE_EXPRESSION == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    item_exp_field = curr_field;
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_comparator_str, name_len, 11))
            {
                if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    comparator_choice = (EZPI_STRNCMP_IF_EQUAL(curr_field->field_value.u_value.value_string, ezlopi_notbetween_str, strlen(curr_field->field_value.u_value.value_string) + 1, 11)) ? true : false; // 0->between (default) ; 1->not_between
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_startValue_str, name_len, 11))
            {
                start_value_field = curr_field;
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_endValue_str, name_len, 9))
            {
                end_value_field = curr_field;
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_deviceGroup_str, name_len, 12) && (NULL != curr_field->field_value.u_value.value_string))
            {
                devgrp_field = curr_field;
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_itemGroup_str, name_len, 10) && (NULL != curr_field->field_value.u_value.value_string))
            {
                itemgrp_field = curr_field;
            }
            curr_field = curr_field->next;
        }

        if (devgrp_field && itemgrp_field && start_value_field && end_value_field)
        {
            ret = EZPI_scenes_operators_value_number_range_operations_with_group(devgrp_field, itemgrp_field, start_value_field, end_value_field, comparator_choice);
        }
        else if (item_exp_field && start_value_field && end_value_field)
        {
            ret = EZPI_scenes_operators_value_number_range_operations(item_exp_field, start_value_field, end_value_field, comparator_choice);
        }
        //-----------------------------------------------------------------------------------------------------------------
    }

    return ret;
}

int EZPI_core_scene_when_compare_strings(l_scenes_list_v2_t *scene_node, void *arg)
{
    // TRACE_W(" Compare_strings ");
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block && scene_node)
    {
        if (false == when_block->block_enable)
        {
            TRACE_D("Block-disabled [%s]", when_block->block_options.method.name);
            return 0;
        }

        if (true == when_block->block_status_reset_once)
        {
            when_block->block_status_reset_once = false;
            return 0;
        }

        l_fields_v2_t *item_exp_field = NULL;
        l_fields_v2_t *value_field = NULL;
        l_fields_v2_t *comparator_field = NULL;
        l_fields_v2_t *devgrp_field = NULL;
        l_fields_v2_t *itemgrp_field = NULL;

        l_fields_v2_t *curr_field = when_block->fields;
        size_t name_len = 0;
        while (curr_field)
        {
            name_len = (curr_field->name ? strlen(curr_field->name) + 1 : 0);
            if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_item_str, name_len, 5))
            {
                if (EZLOPI_VALUE_TYPE_ITEM == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    item_exp_field = curr_field;
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_expression_str, name_len, 11))
            {
                if (EZLOPI_VALUE_TYPE_EXPRESSION == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    item_exp_field = curr_field;
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_comparator_str, name_len, 11))
            {
                if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    comparator_field = curr_field;
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_value_str, name_len, 6))
            {
                if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && (NULL != curr_field->field_value.u_value.value_string))
                {
                    value_field = curr_field; // this field has string
                }
                else if (EZLOPI_VALUE_TYPE_EXPRESSION == curr_field->value_type && (NULL != curr_field->field_value.u_value.value_string))
                {
                    value_field = curr_field; // this field has expression_name
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_deviceGroup_str, name_len, 12) && (NULL != curr_field->field_value.u_value.value_string))
            {
                devgrp_field = curr_field;
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_itemGroup_str, name_len, 10) && (NULL != curr_field->field_value.u_value.value_string))
            {
                itemgrp_field = curr_field;
            }
            curr_field = curr_field->next;
        }

        if (devgrp_field && itemgrp_field && item_exp_field && value_field && comparator_field)
        {
            ret = EZPI_scenes_operators_value_strings_operations(devgrp_field, itemgrp_field, item_exp_field, value_field, comparator_field);
        }
    }

    return ret;
}

int EZPI_core_scene_when_string_operation(l_scenes_list_v2_t *scene_node, void *arg)
{
    // TRACE_W(" Compare_string_op ");
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block && scene_node)
    {
        if (false == when_block->block_enable)
        {
            TRACE_D("Block-disabled [%s]", when_block->block_options.method.name);
            return 0;
        }

        if (true == when_block->block_status_reset_once)
        {
            when_block->block_status_reset_once = false;
            return 0;
        }

        l_fields_v2_t *item_exp_field = NULL;
        l_fields_v2_t *value_field = NULL;
        l_fields_v2_t *operation_field = NULL;
        l_fields_v2_t *devgrp_field = NULL;
        l_fields_v2_t *itemgrp_field = NULL;

        l_fields_v2_t *curr_field = when_block->fields;
        size_t name_len = 0;
        while (curr_field)
        {
            name_len = (curr_field->name ? strlen(curr_field->name) + 1 : 0);
            if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_item_str, name_len, 5))
            {
                if (EZLOPI_VALUE_TYPE_ITEM == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    item_exp_field = curr_field;
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_expression_str, name_len, 11))
            {
                if (EZLOPI_VALUE_TYPE_EXPRESSION == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    item_exp_field = curr_field;
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_operation_str, name_len, 10))
            {
                if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    operation_field = curr_field;
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_value_str, name_len, 6))
            {
                if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && NULL != curr_field->field_value.u_value.value_string)
                {
                    value_field = curr_field; // this field has string
                }
                else if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
                {
                    value_field = curr_field; // this field has double/int value
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_deviceGroup_str, name_len, 12) && (NULL != curr_field->field_value.u_value.value_string))
            {
                devgrp_field = curr_field;
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_itemGroup_str, name_len, 10) && (NULL != curr_field->field_value.u_value.value_string))
            {
                itemgrp_field = curr_field;
            }
            curr_field = curr_field->next;
        }

        if (devgrp_field && itemgrp_field && value_field && operation_field) // only for item_value 'string comparisions'
        {
            ret = EZPI_scenes_operators_value_strops_operations_with_group(value_field, operation_field, devgrp_field, itemgrp_field);
        }
        else
        {
            ret = EZPI_scenes_operators_value_strops_operations(item_exp_field, value_field, operation_field);
        }
    }
    return ret;
}

int EZPI_core_scene_when_in_array(l_scenes_list_v2_t *scene_node, void *arg)
{
    // TRACE_W(" Compare_inArray ");
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block && scene_node)
    {
        if (false == when_block->block_enable)
        {
            TRACE_D("Block-disabled [%s]", when_block->block_options.method.name);
            return 0;
        }

        if (true == when_block->block_status_reset_once)
        {
            when_block->block_status_reset_once = false;
            return 0;
        }

        l_fields_v2_t *item_exp_field = NULL;
        l_fields_v2_t *value_field = NULL;
        l_fields_v2_t *operation_field = NULL;
        l_fields_v2_t *devgrp_field = NULL;
        l_fields_v2_t *itemgrp_field = NULL;

        l_fields_v2_t *curr_field = when_block->fields;
        size_t name_len = 0;
        while (curr_field)
        {
            name_len = (curr_field->name ? strlen(curr_field->name) + 1 : 0);
            if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_item_str, name_len, 5))
            {
                if (EZLOPI_VALUE_TYPE_ITEM == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    item_exp_field = curr_field;
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_expression_str, name_len, 11))
            {
                if (EZLOPI_VALUE_TYPE_EXPRESSION == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    item_exp_field = curr_field;
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_value_str, name_len, 6))
            {
                if (EZLOPI_VALUE_TYPE_ARRAY == curr_field->value_type && (cJSON_IsArray(curr_field->field_value.u_value.cj_value)))
                {
                    value_field = curr_field;
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_operation_str, name_len, 10))
            {
                if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    operation_field = curr_field;
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_deviceGroup_str, name_len, 12) && (NULL != curr_field->field_value.u_value.value_string))
            {
                devgrp_field = curr_field;
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_itemGroup_str, name_len, 10) && (NULL != curr_field->field_value.u_value.value_string))
            {
                itemgrp_field = curr_field;
            }
            curr_field = curr_field->next;
        }

        if (devgrp_field && itemgrp_field && value_field && operation_field) // only for item_value 'string comparisions'
        {
            ret = EZPI_scenes_operators_value_inarr_operations_with_group(value_field, operation_field, devgrp_field, itemgrp_field);
        }
        else if (item_exp_field && value_field && operation_field)
        {
            ret = EZPI_scenes_operators_value_inarr_operations(item_exp_field, value_field, operation_field);
        }
    }

    return ret;
}

int EZPI_core_scene_when_compare_values(l_scenes_list_v2_t *scene_node, void *arg)
{
    // TRACE_W(" Compare_values ");
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block && scene_node)
    {
        if (false == when_block->block_enable)
        {
            TRACE_D("Block-disabled [%s]", when_block->block_options.method.name);
            return 0;
        }

        if (true == when_block->block_status_reset_once)
        {
            when_block->block_status_reset_once = false;
            return 0;
        }

        l_fields_v2_t *value_type_field = NULL;
        l_fields_v2_t *value_field = NULL;
        l_fields_v2_t *comparator_field = NULL;
        l_fields_v2_t *item_exp_field = NULL;
        l_fields_v2_t *devgrp_field = NULL;
        l_fields_v2_t *itemgrp_field = NULL;

        l_fields_v2_t *curr_field = when_block->fields;
        size_t name_len = 0;
        while (curr_field)
        {
            name_len = (curr_field->name ? strlen(curr_field->name) + 1 : 0);
            if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_item_str, name_len, 5))
            {
                if (EZLOPI_VALUE_TYPE_ITEM == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    item_exp_field = curr_field;
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_expression_str, name_len, 11))
            {
                if (EZLOPI_VALUE_TYPE_EXPRESSION == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    item_exp_field = curr_field;
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_value_type_str, name_len, 11))
            {
                if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    value_type_field = curr_field;
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_comparator_str, name_len, 11))
            {
                if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    comparator_field = curr_field;
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_value_str, name_len, 6))
            {
                if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    value_field = curr_field;
                }
                else if (EZLOPI_VALUE_TYPE_EXPRESSION == curr_field->value_type && (NULL != curr_field->field_value.u_value.value_string))
                {
                    value_field = curr_field; // this field has expression_name
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_deviceGroup_str, name_len, 12) && (NULL != curr_field->field_value.u_value.value_string))
            {
                devgrp_field = curr_field;
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_itemGroup_str, name_len, 10) && (NULL != curr_field->field_value.u_value.value_string))
            {
                itemgrp_field = curr_field;
            }
            curr_field = curr_field->next;
        }

        if (devgrp_field && itemgrp_field && value_field && value_type_field && comparator_field) // only for item_value 'string comparisions'
        {
            ret = EZPI_scenes_operators_value_comparevalues_with_less_operations_with_group(value_field, value_type_field, comparator_field, devgrp_field, itemgrp_field);
        }
        else
        {
            ret = EZPI_scenes_operators_value_comparevalues_with_less_operations(item_exp_field, value_field, value_type_field, comparator_field);
        }
    }
    return ret;
}

int EZPI_core_scene_when_has_atleast_one_dictionary_value(l_scenes_list_v2_t *scene_node, void *arg)
{
    // TRACE_W(" atleast_one_diction_val ");
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (scene_node && when_block)
    {
        if (false == when_block->block_enable)
        {
            TRACE_D("Block-disabled [%s]", when_block->block_options.method.name);
            return 0;
        }

        if (true == when_block->block_status_reset_once)
        {
            when_block->block_status_reset_once = false;
            return 0;
        }

        uint32_t item_id = 0;
        l_fields_v2_t *value_field = NULL;

        l_fields_v2_t *curr_field = when_block->fields;
        size_t name_len = 0;
        while (curr_field)
        {
            name_len = (curr_field->name ? strlen(curr_field->name) + 1 : 0);
            if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_item_str, name_len, 5))
            {
                if (EZLOPI_VALUE_TYPE_ITEM == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    item_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_value_str, name_len, 6))
            {
                if (EZLOPI_VALUE_TYPE_TOKEN == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    value_field = curr_field; // this contains "options [array]" & 'value': to be checked
                }
            }
            curr_field = curr_field->next;
        }

        // now to extract the
        if (item_id && value_field)
        {
            ret = EZPI_scenes_operators_has_atleastone_dictionary_value_operations(item_id, value_field);
        }
    }
    return ret;
}

int EZPI_core_scenes_when_is_firmware_update_state(l_scenes_list_v2_t *scene_node, void *arg)
{
    // TRACE_W(" firmware_update ");
    int ret = 0;
#ifdef CONFIG_EZPI_ENABLE_OTA
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (scene_node && when_block)
    {
        if (false == when_block->block_enable)
        {
            TRACE_D("Block-disabled [%s]", when_block->block_options.method.name);
            return 0;
        }

        if (true == when_block->block_status_reset_once)
        {
            when_block->block_status_reset_once = false;
            return 0;
        }

        uint32_t item_id = 0;
        char *state_value = NULL;

        l_fields_v2_t *curr_field = when_block->fields;
        while (curr_field)
        {
            if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_state_str, strlen(curr_field->name) + 1, 6))
            {
                if (EZLOPI_VALUE_TYPE_TOKEN == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    state_value = curr_field->field_value.u_value.value_string; // started / updating / done
                }
            }
            curr_field = curr_field->next;
        }

        // now to extract the
        if (item_id && (NULL != state_value))
        {
            size_t len = (state_value ? strlen(state_value) + 1 : 0);
            if (EZPI_STRNCMP_IF_EQUAL(ezlopi_done_str, state_value, 5, len) && (0 == __get_ota_state()))
            {
                ret = 1;
            }
            else if (EZPI_STRNCMP_IF_EQUAL(scene_status_started_str, state_value, 8, len) && (1 == __get_ota_state()))
            {
                ret = 1;
            }
            else if (EZPI_STRNCMP_IF_EQUAL("updating", state_value, 9, len) && (2 == __get_ota_state()))
            {
                ret = 1;
            }
        }
    }
#endif // CONFIG_EZPI_ENABLE_OTA
    return ret;
}

int EZPI_core_scenes_when_is_dictionary_changed(l_scenes_list_v2_t *scene_node, void *arg)
{
    // TRACE_W(" isDictionary_changed ");
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (scene_node && when_block)
    {
        if (false == when_block->block_enable)
        {
            TRACE_D("Block-disabled [%s]", when_block->block_options.method.name);
            return 0;
        }

        if (true == when_block->block_status_reset_once)
        {
            when_block->block_status_reset_once = false;
            return 0;
        }

        uint32_t item_id = 0;
        l_fields_v2_t *key_field = NULL;
        l_fields_v2_t *operation_field = NULL;

        l_fields_v2_t *curr_field = when_block->fields;
        size_t name_len = 0;
        while (curr_field)
        {
            name_len = (curr_field->name ? strlen(curr_field->name) + 1 : 0);
            if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_item_str, name_len, 5))
            {
                if (EZLOPI_VALUE_TYPE_ITEM == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    item_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_key_str, name_len, 4))
            {
                if (EZLOPI_VALUE_TYPE_TOKEN == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    key_field = curr_field; // this contains "options [array]" & 'value': to be checked
                }
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_operation_str, name_len, 10))
            {
                if (EZLOPI_VALUE_TYPE_TOKEN == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    operation_field = curr_field; // this contains "options [array]" & 'value': to be checked
                }
            }
            curr_field = curr_field->next;
        }

        if (item_id && key_field && operation_field)
        {
            ret = EZPI_scenes_operators_is_dictionary_changed_operations(scene_node, item_id, key_field, operation_field);
        }
    }
    return ret;
}

int EZPI_core_scenes_when_is_detected_in_hot_zone(l_scenes_list_v2_t *scene_node, void *arg)
{
    TRACE_W("Warning: when-method 'is_detected_in_hot_zone' not implemented!");
    return 0;
}

int EZPI_core_scene_when_and(l_scenes_list_v2_t *scene_node, void *arg)
{
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block)
    {
        ret = 1; // required for the first case
        l_when_block_v2_t *value_when_block = when_block->fields->field_value.u_value.when_block;
        while (value_when_block)
        {
            f_scene_method_v2_t scene_method = EZPI_core_scenes_get_method_v2(value_when_block->block_options.method.type);
            if (scene_method)
            {
                ret &= scene_method(scene_node, (void *)value_when_block);
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

int EZPI_core_scene_when_not(l_scenes_list_v2_t *scene_node, void *arg)
{
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block)
    {
        l_when_block_v2_t *value_when_block = when_block->fields->field_value.u_value.when_block;
        while (value_when_block)
        {
            f_scene_method_v2_t scene_method = EZPI_core_scenes_get_method_v2(value_when_block->block_options.method.type);
            if (scene_method)
            {
                ret = !(scene_method(scene_node, (void *)value_when_block)); // if all the block-calls are false, then return 1;
            }
            value_when_block = value_when_block->next;
        }
    }
    return ret;
}

int EZPI_core_scene_when_or(l_scenes_list_v2_t *scene_node, void *arg)
{
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block)
    {
        l_when_block_v2_t *value_when_block = when_block->fields->field_value.u_value.when_block;
        while (value_when_block)
        {
            f_scene_method_v2_t scene_method = EZPI_core_scenes_get_method_v2(value_when_block->block_options.method.type);
            if (scene_method)
            {
                ret |= scene_method(scene_node, (void *)value_when_block);
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

int EZPI_core_scene_when_xor(l_scenes_list_v2_t *scene_node, void *arg)
{
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block)
    {
        l_when_block_v2_t *value_when_block = when_block->fields->field_value.u_value.when_block;
        while (value_when_block)
        {
            f_scene_method_v2_t scene_method = EZPI_core_scenes_get_method_v2(value_when_block->block_options.method.type);
            if (scene_method)
            {
                // iterate through all the '_when_blocks_'
                ret ^= scene_method(scene_node, (void *)value_when_block);
                // return 1 ; if odd no of '_when_block_' conditions are true
            }
            value_when_block = value_when_block->next;
        }
    }

    return ret;
}

int EZPI_core_scene_when_function(l_scenes_list_v2_t *scene_node, void *arg)
{
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (scene_node && when_block)
    {
        if (true == when_block->block_status_reset_once)
        {
            when_block->block_status_reset_once = false;
            return 0;
        }

        cJSON *function_obj = scene_node->when_block->block_options.cj_function;
        if (function_obj)
        {
            cJSON *cj_latch = cJSON_GetObjectItem(__FUNCTION__, function_obj, ezlopi_latch_str);
            if (cj_latch)
            {
                cJSON *cj_enabled_latch = cJSON_GetObjectItem(__FUNCTION__, cj_latch, ezlopi_enabled_str);
                if (cj_enabled_latch && cJSON_IsTrue(cj_enabled_latch))
                {

                    /*fill the 'fields->user-arg' with 'function_state_info'*/
                    if (NULL == scene_node->when_block->fields->user_arg)
                    {
                        s_when_function_t *function_state_info = (s_when_function_t *)malloc(sizeof(s_when_function_t));
                        if (function_state_info)
                        {
                            memset(function_state_info, 0, sizeof(s_when_function_t));
                            scene_node->when_block->fields->user_arg = (void *)function_state_info;
                        }
                    }

                    const s_function_opr_t __when_funtion_opr[] = {
                        {.opr_name = ezlopi_for_str, .opr_method = WHEN_function_for_opr},
                        {.opr_name = ezlopi_repeat_str, .opr_method = WHEN_function_for_repeat},
                        {.opr_name = ezlopi_follow_str, .opr_method = WHEN_function_for_follow},
                        {.opr_name = ezlopi_pulse_str, .opr_method = WHEN_function_for_pulse},
                        {.opr_name = ezlopi_latch_str, .opr_method = WHEN_function_for_latch},
                        {.opr_name = NULL, .opr_method = NULL},
                    };

                    cJSON *cj_func_opr = NULL;
                    for (uint8_t i = 0; i < ((sizeof(__when_funtion_opr) / sizeof(__when_funtion_opr[i]))); i++)
                    {
                        if (NULL != (cj_func_opr = cJSON_GetObjectItem(__FUNCTION__, function_obj, __when_funtion_opr[i].opr_name)))
                        {
                            // TRACE_S("when_func_here->[%d]", i);
                            ret = (__when_funtion_opr[i].opr_method)(scene_node, when_block, cj_func_opr);
                            break;
                        }
                    }
                }
                else
                {
                    TRACE_E("when-Function ['%s'] --> Disabled", cJSON_GetStringValue(cJSON_GetObjectItem(__FUNCTION__, cj_latch, ezlopi_name_str)));
                }
            }
        }
    }
    return ret;
}

int EZPI_core_scenes_when_is_device_item_group(l_scenes_list_v2_t *scene_node, void *arg)
{
    // TRACE_W(" is_item_state ");
    int ret = 0;
    l_when_block_v2_t *when_block = (l_when_block_v2_t *)arg;
    if (when_block && scene_node)
    {
        if (false == when_block->block_enable)
        {
            TRACE_D("Block-disabled [%s]", when_block->block_options.method.name);
            return 0;
        }

        if (true == when_block->block_status_reset_once)
        {
            when_block->block_status_reset_once = false;
            return 0;
        }

        uint32_t device_group_id = 0;
        uint32_t item_group_id = 0;

        l_fields_v2_t *curr_field = when_block->fields;
        size_t name_len = 0;
        while (curr_field)
        {
            name_len = (curr_field->name ? strlen(curr_field->name) + 1 : 0);
            if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_deviceGroup_str, name_len, 12) && (NULL != curr_field->field_value.u_value.value_string))
            {
                device_group_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
            }
            else if (EZPI_STRNCMP_IF_EQUAL(curr_field->name, ezlopi_itemGroup_str, name_len, 10) && (NULL != curr_field->field_value.u_value.value_string))
            {
                item_group_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
            }
            curr_field = curr_field->next;
        }

        if (device_group_id && item_group_id)
        {
            ret = IS_deviceitem_group_value_check(scene_node, device_group_id, item_group_id);
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
