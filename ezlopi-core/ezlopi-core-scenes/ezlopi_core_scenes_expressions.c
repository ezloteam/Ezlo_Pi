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
 * @file    ezlopi_core_scenes_expressions.c
 * @brief   These function performs operation on scene-expression data
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 * @version 0.1
 * @date    12th DEC 2024
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include "cjext.h"
// #include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"
#include "lua_helper_functions.h"
#include "core_getters_api.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_scenes_value.h"
#include "ezlopi_core_scenes_expressions.h"
#include "ezlopi_core_errors.h"

#include "ezlopi_cloud_constants.h"
#include "EZLOPI_USER_CONFIG.h"
/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/
void EZPI_scenes_expressions_populate_items(s_ezlopi_expressions_t *exp_node, cJSON *cj_items);
void EZPI_scenes_expressions_populate_devitem_names(s_ezlopi_expressions_t *exp_node, cJSON *cj_device_item_names);
/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
static s_exp_device_item_names_t *__expressions_device_item_names_create(cJSON *cj_device_item_name);
static s_ezlopi_expressions_t *__expressions_create_node(uint32_t exp_id, cJSON *cj_expression);
static e_scene_value_type_v2_t *__parse_expression_type_filter(cJSON *cj_params);
static s_exp_items_t *__expressions_items_create(cJSON *cj_item);
static uint32_t __expression_store_to_nvs(uint32_t exp_id, cJSON *cj_expression);
static bool __check_expression_type_filter(s_ezlopi_expressions_t *exp_node, e_scene_value_type_v2_t *type_filter_arr);
static void __get_expressions_value(s_ezlopi_expressions_t *exp_node, cJSON *cj_value);
static void __add_expression_value(s_ezlopi_expressions_t *exp_node, cJSON *cj_expr);
static void __add_expression_items(s_ezlopi_expressions_t *exp_node, cJSON *cj_params);
static void __add_expression_device_item_names(s_ezlopi_expressions_t *exp_node, cJSON *cj_params);
static void ____get_devitem_expn_result(cJSON *cj_expr_val, lua_State *lua_state, const char *member_str);
static void ___create_lua_subtable(lua_State *lua_state, l_ezlopi_item_t *item_prop, const char *sub_table_name);
static int ____get_simple_expn_result(cJSON *cj_des, lua_State *lua_state, const char *name, int status);
static int __remove_exp_id_from_nvs_exp_list(uint32_t target_id);
static int __evaluate_expression(cJSON *cj_des, cJSON *lua_prop_params, const char *exp_name, const char *exp_code);
static int __edit_expression_ll(s_ezlopi_expressions_t *expn_node, cJSON *cj_new_expression);
/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/
static s_ezlopi_expressions_t *l_expressions_head = NULL;

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
s_ezlopi_expressions_t *EZPI_scenes_expressions_get_node_by_name(char *expression_name)
{
    s_ezlopi_expressions_t *curr_expr = l_expressions_head;
    if (expression_name && curr_expr)
    {
        size_t len = strlen(expression_name) + 1;
        while (curr_expr)
        {
            if (EZPI_STRNCMP_IF_EQUAL(curr_expr->name, expression_name, strlen(curr_expr->name) + 1, len))
            {
                break;
            }
            curr_expr = curr_expr->next;
        }
    }
    return curr_expr;
}

cJSON *EZPI_scenes_expressions_get_cjson(s_ezlopi_expressions_t *exp_node)
{
    cJSON *ret_cj_exp = NULL;
    if (exp_node)
    {
        ret_cj_exp = cJSON_CreateObject(__FUNCTION__);
        if (ret_cj_exp)
        {
            cJSON_AddStringToObject(__FUNCTION__, ret_cj_exp, ezlopi_name_str, exp_node->name);
            cJSON_AddStringToObject(__FUNCTION__, ret_cj_exp, ezlopi_code_str, exp_node->code);
            __add_expression_value(exp_node, ret_cj_exp);
            cJSON_AddBoolToObject(__FUNCTION__, ret_cj_exp, ezlopi_variable_str, exp_node->variable);

            cJSON *cj_params = cJSON_AddObjectToObject(__FUNCTION__, ret_cj_exp, ezlopi_params_str);
            if (cj_params)
            {
                __add_expression_items(exp_node, cj_params);
                __add_expression_device_item_names(exp_node, cj_params);
            }

            if (exp_node->meta_data)
            {
                cJSON_AddItemToObject(__FUNCTION__, ret_cj_exp, ezlopi_metadata_str, cJSON_Duplicate(__FUNCTION__, exp_node->meta_data, 1));
            }

#ifdef CONFIG_EZPI_UTIL_TRACE_EN
            // TRACE_S("New_modified ; %s[%#x]", exp_node->name, exp_node->exp_id);
            // CJSON_TRACE("cj_exp_after_changes_in_ll", ret_cj_exp);
#endif
        }
    }
    return ret_cj_exp;
}

ezlopi_error_t EZPI_scenes_expressions_update_nvs(char *nvs_exp_id_key, cJSON *cj_updated_exp)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (cj_updated_exp)
    {
        char *update_exp_str = cJSON_PrintBuffered(__FUNCTION__, cj_updated_exp, 1024, false);
        TRACE_D("length of 'update_exp_str': %d", strlen(update_exp_str));

        if (update_exp_str)
        {
            EZPI_core_nvs_delete_stored_data_by_name(nvs_exp_id_key);
            ret = EZPI_core_nvs_write_str(update_exp_str, strlen(update_exp_str), nvs_exp_id_key);
            if (EZPI_SUCCESS == ret)
            {
                TRACE_S("successfully saved/modified expression in nvs");
            }
            ezlopi_free(__FUNCTION__, update_exp_str);
        }
    }
    return ret;
}

s_ezlopi_expressions_t *EZPI_scenes_expressions_node_pop_by_id(uint32_t _id)
{
    s_ezlopi_expressions_t *popped_node = NULL;

    if (_id == l_expressions_head->exp_id)
    {
        popped_node = l_expressions_head;
        l_expressions_head = l_expressions_head->next;
        popped_node->next = NULL;
    }
    else
    {
        s_ezlopi_expressions_t *curr_node = l_expressions_head;
        while (curr_node->next)
        {
            if (_id == curr_node->next->exp_id)
            {
                popped_node = curr_node->next;
                curr_node->next = curr_node->next->next;
                popped_node->next = NULL;
                break;
            }
            curr_node = curr_node->next;
        }
    }

    return popped_node;
}

int EZPI_scenes_expressions_delete_by_name(char *expression_name)
{
    int ret = 0;
    if (expression_name)
    {
        s_ezlopi_expressions_t *curr_expr = l_expressions_head;
        // s_ezlopi_expressions_t*  prev_expr = NULL;
        size_t len = strlen(expression_name) + 1;
        while (curr_expr)
        {
            if (EZPI_STRNCMP_IF_EQUAL(curr_expr->name, expression_name, strlen(curr_expr->name) + 1, len))
            {
                /*  Depopulating the 'exp_id' from 'expression_ll'*/
                ret = EZPI_scenes_expressions_delete_node(EZPI_scenes_expressions_node_pop_by_id(curr_expr->exp_id));
                break;
            }

            // prev_expr = curr_expr;
            curr_expr = curr_expr->next;
        }
    }

    return ret;
}

void EZPI_scenes_expressions_print(s_ezlopi_expressions_t *exp_node)
{
#if (ENABLE_TRACE)
    if (exp_node)
    {
        TRACE_D("----------------- Expression --------------------");
        TRACE_D("Name: %s", exp_node->name);
        TRACE_D("Exp_id: %08x", exp_node->exp_id);
        TRACE_D("code: %s", exp_node->code ? exp_node->code : ezlopi__str);

        TRACE_D("-- Items:");
        int count = 0;
        s_exp_items_t *items = exp_node->items;
        while (items)
        {
            TRACE_D("\t-------------- item-%d ------------", ++count);
            TRACE_D("\tname: %s", items->name);
            TRACE_D("\t_id: %08x", items->_id);
            items = items->next;
        }
        TRACE_D("\t-----------------------------------");

        TRACE_D("-- Device Item Names:");
        count = 0;
        s_exp_device_item_names_t *device_item_names = exp_node->device_item_names;
        while (device_item_names)
        {
            TRACE_D("\t----------device-item-names-%d-------", ++count);
            TRACE_D("\tname: %s", device_item_names->name);
            TRACE_D("\titem-name: %s", device_item_names->item_name);
            TRACE_D("\tdevice-name: %s", device_item_names->device_name);
            device_item_names = device_item_names->next;
        }
        TRACE_D("\t-----------------------------------");

        CJSON_TRACE("meta-data", exp_node->meta_data);
        TRACE_D("Is variable: %s", exp_node->variable ? ezlopi_true_str : ezlopi_false_str);
        TRACE_D("value-type: %s", EZPI_core_scenes_get_scene_value_type_name(exp_node->value_type));

        if (exp_node->exp_value.type > EXPRESSION_VALUE_TYPE_UNDEFINED && exp_node->exp_value.type < EXPRESSION_VALUE_TYPE_MAX)
        {
            switch (exp_node->exp_value.type)
            {
            case EXPRESSION_VALUE_TYPE_STRING:
            {
                TRACE_D("value: %s", exp_node->exp_value.u_value.str_value ? exp_node->exp_value.u_value.str_value : ezlopi__str);
                break;
            }
            case EXPRESSION_VALUE_TYPE_CJ:
            {
                CJSON_TRACE(ezlopi_value_str, exp_node->exp_value.u_value.cj_value);
                break;
            }
            case EXPRESSION_VALUE_TYPE_BOOL:
            {
                TRACE_D("value: %s", exp_node->exp_value.u_value.boolean_value ? ezlopi_true_str : ezlopi_false_str);
                break;
            }
            case EXPRESSION_VALUE_TYPE_NUMBER:
            {
                TRACE_D("value: %lf", exp_node->exp_value.u_value.number_value);
                break;
            }

            default:
            {
                TRACE_W("exp_node->exp_value.type: value type is undefined!");
                break;
            }
            }
        }

        TRACE_D("----------------- ---------- --------------------");
    }
#endif
}

s_ezlopi_expressions_t *EZPI_scenes_expression_get_by_name(char *target_exp_name)
{
    s_ezlopi_expressions_t *curr_node = NULL;
    if (l_expressions_head && target_exp_name)
    {
        size_t len = strlen(target_exp_name) + 1;
        curr_node = l_expressions_head;
        while (curr_node)
        {
            if (EZPI_STRNCMP_IF_EQUAL(curr_node->name, target_exp_name, strlen(curr_node->name) + 1, len))
            {
                break;
            }
            curr_node = curr_node->next;
        }
    }

    return curr_node;
}

ezlopi_error_t EZPI_scenes_expressions_update_expr(s_ezlopi_expressions_t *expression_node, cJSON *cj_new_expression)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (expression_node && cj_new_expression)
    {
        if (1 == __edit_expression_ll(expression_node, cj_new_expression)) // if successfully updated in ll
        {
            EZPI_scenes_expressions_print(expression_node);
            char id_str[32];
            snprintf(id_str, sizeof(id_str), "%08x", expression_node->exp_id);

#ifdef CONFIG_EZPI_UTIL_TRACE_EN
            CJSON_TRACE("NEW_EXPN_stored", cj_new_expression);
#endif
            ret = EZPI_scenes_expressions_update_nvs(id_str, cj_new_expression); // update in nvs
        }
    }
    return ret;
}

uint32_t EZPI_scenes_expressions_add_to_head(uint32_t exp_id, cJSON *cj_expression)
{
    uint32_t new_exp_id = 0;

    if (l_expressions_head)
    {
        s_ezlopi_expressions_t *current_exp = l_expressions_head;
        while (current_exp->next)
        {
            current_exp = current_exp->next;
        }

        current_exp->next = __expressions_create_node(exp_id, cj_expression);
        if (current_exp->next)
        {
            // EZPI_scenes_expressions_print(current_exp->next);
            new_exp_id = current_exp->next->exp_id;
        }
    }
    else
    {
        l_expressions_head = __expressions_create_node(exp_id, cj_expression);
        if (l_expressions_head)
        {
            // EZPI_scenes_expressions_print(l_expressions_head);
            new_exp_id = l_expressions_head->exp_id;
        }
    }

    return new_exp_id;
}

void EZPI_scenes_expressions_delete_items(s_exp_items_t *exp_items)
{
    if (exp_items)
    {
        EZPI_scenes_expressions_delete_items(exp_items->next);
        ezlopi_free(__FUNCTION__, exp_items);
    }
}

void EZPI_scenes_expressions_delete_devitem_names(s_exp_device_item_names_t *exp_device_item_names)
{
    if (exp_device_item_names)
    {
        EZPI_scenes_expressions_delete_devitem_names(exp_device_item_names->next);
        ezlopi_free(__FUNCTION__, exp_device_item_names);
    }
}

int EZPI_scenes_expressions_delete_node(s_ezlopi_expressions_t *exp_node)
{
    int ret = 0;
    if (exp_node)
    {
        ret = EZPI_scenes_expressions_delete_node(exp_node->next);

        // 1. clear in the node in 'expression_ll'
        {
            if (exp_node->code)
            {
                ezlopi_free(__FUNCTION__, exp_node->code);
            }

            switch (exp_node->exp_value.type)
            {
            case EXPRESSION_VALUE_TYPE_STRING:
            {
                if (exp_node->exp_value.u_value.str_value)
                {
                    ezlopi_free(__FUNCTION__, exp_node->exp_value.u_value.str_value);
                    exp_node->exp_value.u_value.str_value = NULL;
                }
                break;
            }
            case EXPRESSION_VALUE_TYPE_CJ:
            {
                if (exp_node->exp_value.u_value.cj_value)
                {
                    cJSON_Delete(__FUNCTION__, exp_node->exp_value.u_value.cj_value);
                    exp_node->exp_value.u_value.cj_value = NULL;
                }
                break;
            }
            default:
            {
                break;
            }
            }

            if (exp_node->meta_data)
            {
                ezlopi_free(__FUNCTION__, exp_node->meta_data);
            }

            if (exp_node->items)
            {
                EZPI_scenes_expressions_delete_items(exp_node->items);
                exp_node->items->next = NULL;
                exp_node->items = NULL;
            }
            if (exp_node->device_item_names)
            {
                EZPI_scenes_expressions_delete_devitem_names(exp_node->device_item_names);
                exp_node->device_item_names->next = NULL;
                exp_node->device_item_names = NULL;
            }
        }

        // 2. clearing from NVS
        {
            EZPI_core_nvs_delete_stored_data_by_id(exp_node->exp_id); // remove 'target_exp_id' from nvs
            /*  Now to update 'expression_nvs_list' after removing 'exp_id' from nvs*/
            ret = __remove_exp_id_from_nvs_exp_list(exp_node->exp_id);
        }
    }

    return ret;
}

ezlopi_error_t EZPI_scenes_expressions_init(void)
{
    // __remove_residue_expn_ids_from_list(); // for furture
    ezlopi_error_t error = EZPI_ERR_JSON_PARSE_FAILED;
    char *exp_id_list_str = EZPI_core_nvs_read_scenes_expressions();
    if (exp_id_list_str)
    {
        TRACE_D("exp_id_list_str: %s", exp_id_list_str);

        cJSON *cj_exp_id_list = cJSON_Parse(__FUNCTION__, exp_id_list_str);
        if (cj_exp_id_list)
        {
            cJSON *cj_exp_id = NULL;
            cJSON_ArrayForEach(cj_exp_id, cj_exp_id_list)
            {
                if (cj_exp_id->valuestring)
                {
                    uint32_t exp_id = strtoul(cj_exp_id->valuestring, NULL, 16);
                    if (exp_id)
                    {
                        char *exp_str = EZPI_core_nvs_read_str(cj_exp_id->valuestring);
                        if (exp_str)
                        {
                            cJSON *cj_exp = cJSON_Parse(__FUNCTION__, exp_str);
                            ezlopi_free(__FUNCTION__, exp_str);

                            if (cj_exp)
                            {
                                EZPI_scenes_expressions_add_to_head(exp_id, cj_exp);
                                cJSON_Delete(__FUNCTION__, cj_exp);
                            }
                        }
                    }
                }
            }
            error = EZPI_SUCCESS;
        }

        ezlopi_free(__FUNCTION__, exp_id_list_str);
    }
    return error;
}

void EZPI_scenes_expressions_populate_items(s_ezlopi_expressions_t *exp_node, cJSON *cj_items)
{
    if (cj_items)
    {
        cJSON *cj_item = NULL;
        s_exp_items_t *new_item_head = NULL;
        s_exp_items_t *curr_item_node = NULL;

        cJSON_ArrayForEach(cj_item, cj_items)
        {
            if (new_item_head)
            {
                curr_item_node->next = __expressions_items_create(cj_item);
                curr_item_node = curr_item_node->next;
            }
            else
            {
                new_item_head = __expressions_items_create(cj_item);
                curr_item_node = new_item_head;
            }
        }

        if (exp_node->items)
        {
            s_exp_items_t *exp_item_node = exp_node->items;
            while (exp_item_node->next)
            {
                exp_item_node = exp_item_node->next;
            }

            exp_item_node->next = new_item_head;
        }
        else
        {
            exp_node->items = new_item_head;
        }
    }
}

void EZPI_scenes_expressions_populate_devitem_names(s_ezlopi_expressions_t *exp_node, cJSON *cj_device_item_names)
{
    if (cj_device_item_names)
    {
        cJSON *cj_dev_item_name = NULL;
        s_exp_device_item_names_t *new_device_item_names_head = NULL;
        s_exp_device_item_names_t *cur_device_item_names_head = NULL;

        cJSON_ArrayForEach(cj_dev_item_name, cj_device_item_names)
        {
            if (new_device_item_names_head)
            {
                cur_device_item_names_head->next = __expressions_device_item_names_create(cj_dev_item_name);
                cur_device_item_names_head = cur_device_item_names_head->next;
            }
            else
            {
                new_device_item_names_head = __expressions_device_item_names_create(cj_dev_item_name);
                cur_device_item_names_head = new_device_item_names_head;
            }
        }

        if (exp_node->device_item_names)
        {
            s_exp_device_item_names_t *exp_device_item_names_node = exp_node->device_item_names;
            while (exp_device_item_names_node->next)
            {
                exp_device_item_names_node = exp_device_item_names_node->next;
            }

            exp_device_item_names_node->next = new_device_item_names_head;
        }
        else
        {
            exp_node->device_item_names = new_device_item_names_head;
        }
    }
}

void EZPI_scenes_expressions_list_cjson(cJSON *cj_expresson_array, cJSON *cj_params)
{
    if (cj_expresson_array)
    {
        bool show_code = false;
        bool show_value = true;
        e_scene_value_type_v2_t *type_filter_array = NULL;

        if (cj_params)
        {
            CJSON_GET_VALUE_BOOL(cj_params, ezlopi_showCode_str, show_code);
            (NULL != cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_showValue_str)) ? (CJSON_GET_VALUE_BOOL(cj_params, ezlopi_showValue_str, show_value)) : NULL;
            type_filter_array = __parse_expression_type_filter(cj_params);
        }

        s_ezlopi_expressions_t *curr_exp = l_expressions_head;
        while (curr_exp)
        {
            if (__check_expression_type_filter(curr_exp, type_filter_array))
            {
                bool valid_append_flag = true;
                cJSON *cj_expr = cJSON_CreateObject(__FUNCTION__);
                if (cj_expr)
                {
                    char exp_id[32];
                    snprintf(exp_id, sizeof(exp_id), "%08x", curr_exp->exp_id);
                    cJSON_AddStringToObject(__FUNCTION__, cj_expr, ezlopi__id_str, exp_id);
                    cJSON_AddStringToObject(__FUNCTION__, cj_expr, ezlopi_name_str, curr_exp->name);

                    if (show_code && curr_exp->code)
                    {
                        cJSON_AddStringToObject(__FUNCTION__, cj_expr, ezlopi_code_str, curr_exp->code);
                    }

                    if (curr_exp->meta_data)
                    {
                        cJSON_AddItemReferenceToObject(__FUNCTION__, cj_expr, ezlopi_metadata_str, curr_exp->meta_data);
                    }

                    cJSON *lua_prop_params = NULL;
                    if ((NULL != curr_exp->items) || (NULL != curr_exp->device_item_names))
                    {
                        cJSON *cj_val_params = cJSON_AddObjectToObject(__FUNCTION__, cj_expr, ezlopi_params_str);
                        if (cj_val_params)
                        {
                            __add_expression_items(curr_exp, cj_val_params);
                            __add_expression_device_item_names(curr_exp, cj_val_params);
                            lua_prop_params = cj_val_params;
                        }
                    }

                    if (show_value)
                    {
                        if (curr_exp->variable)
                        {
                            __add_expression_value(curr_exp, cj_expr); // adds valueType + value
                        }
                        else
                        {
                            if ((EZLOPI_VALUE_TYPE_NONE < curr_exp->value_type) && (EZLOPI_VALUE_TYPE_MAX > curr_exp->value_type))
                            {
                                cJSON_AddStringToObject(__FUNCTION__, cj_expr, ezlopi_valueType_str, EZPI_core_scenes_get_scene_value_type_name(curr_exp->value_type));
                            }
                            if (curr_exp->code)
                            {
                                __evaluate_expression(cj_expr, lua_prop_params, curr_exp->name, curr_exp->code);
                            }
                        }
                    }

                    cJSON_AddBoolToObject(__FUNCTION__, cj_expr, ezlopi_variable_str, curr_exp->variable);

                    //--------------------------- check for 'variable' condition --------------------------------
                    if (valid_append_flag)
                    {
                        cJSON *cj_check_variable = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_variable_str);
                        if (cj_check_variable)
                        {
                            bool req_val = (cJSON_True == cj_check_variable->type) ? true : false;
                            if (curr_exp->variable != req_val)
                            {
                                valid_append_flag = false;
                            }
                        }
                    }

                    //--------------------------- check for 'NAMES' condition --------------------------------
                    if (valid_append_flag)
                    {
                        cJSON *cj_check_name = NULL;
                        cJSON *cj_names_list = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_names_str);
                        if (cj_names_list)
                        {
                            cJSON_ArrayForEach(cj_check_name, cj_names_list)
                            {
                                if (EZPI_STRNCMP_IF_EQUAL(cj_check_name->valuestring, curr_exp->name, cj_check_name->str_value_len, strlen(curr_exp->name) + 1))
                                {
                                    valid_append_flag = true;
                                    break;
                                }
                                else
                                {
                                    valid_append_flag = false;
                                }
                            }
                        }
                    }

                    if (valid_append_flag)
                    { // append the node , if all conditions are satisfied
                        if (!cJSON_AddItemToArray(cj_expresson_array, cj_expr))
                        {
                            cJSON_Delete(__FUNCTION__, cj_expr);
                        }
                    }
                    else
                    {
                        cJSON_Delete(__FUNCTION__, cj_expr);
                    }
                }
            }

            curr_exp = curr_exp->next;
        }
    }
}

int EZPI_scenes_expressions_eval_simple(cJSON *cj_des, const char *exp_name, const char *exp_code)
{
    return __evaluate_expression(cj_des, NULL, exp_name, exp_code); // this is only for simple
}

#if 0 // may be used in future
static void __remove_residue_expn_ids_from_list(void)
{
    TRACE_D("---------- # Removing [Expression] residue-Ids # ----------");
    // check --> nvs_devgrp_list for unncessary "residue-IDs" & update the list
    uint32_t residue_nvs_expn_id = 0;
    bool expn_list_has_residue = false; // this indicates absence of residue-IDs // those IDs which are still in the "nvs-list" but doesnot not exists in "nvs-body"
    char *list_ptr = NULL;

    do
    {
        if (expn_list_has_residue)
        {
            if (0 != residue_nvs_expn_id)
            {
                __remove_exp_id_from_nvs_exp_list(residue_nvs_expn_id);
            }
            expn_list_has_residue = false;
        }

        list_ptr = EZPI_core_nvs_read_scenes_expressions();
        if (list_ptr)
        {
            cJSON *cj_id_list = cJSON_Parse(__FUNCTION__, list_ptr);
            if (cj_id_list)
            {
                int array_size = cJSON_GetArraySize(cj_id_list);
                for (int i = 0; i < array_size; i++)
                {
                    cJSON *cj_id = cJSON_GetArrayItem(cj_id_list, i);
                    if (cj_id && cj_id->valuestring)
                    {
                        if (NULL == EZPI_core_nvs_read_str(cj_id->valuestring))
                        {
                            residue_nvs_expn_id = (uint32_t)strtoul(cj_id->valuestring, NULL, 16); // A residue_id is found..
                            expn_list_has_residue = true;                                          // this will trigger a removal of "invalid_nvs_devgrp_id" .
                            break;                                                                 // get out of for
                        }
                    }
                }
            }
        }
    } while (expn_list_has_residue);
    TRACE_D("---------- # --------------------------------- # ----------");
}
#endif

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/
static int __edit_expression_ll(s_ezlopi_expressions_t *expn_node, cJSON *cj_new_expression)
{
    int ret = 0;
    // now update in ll
    if (expn_node && cj_new_expression)
    {
        ret = 1;
        // 1. code
        {
            if (expn_node->code)
            {
                ezlopi_free(__FUNCTION__, expn_node->code);
                expn_node->code = NULL;
            }

            cJSON *cj_code = cJSON_GetObjectItem(__FUNCTION__, cj_new_expression, ezlopi_code_str);
            if (cj_code && cj_code->valuestring && cj_code->str_value_len)
            {
                expn_node->code = (char *)ezlopi_malloc(__FUNCTION__, cj_code->str_value_len + 1);
                if (expn_node->code)
                {
                    snprintf(expn_node->code, cj_code->str_value_len + 1, "%.*s", cj_code->str_value_len, cj_code->valuestring);
                }
            }
        }

        // 2. params
        {
            if (expn_node->items)
            {
                EZPI_scenes_expressions_delete_items(expn_node->items);
                expn_node->items->next = NULL;
                expn_node->items = NULL;
            }
            if (expn_node->device_item_names)
            {
                EZPI_scenes_expressions_delete_devitem_names(expn_node->device_item_names);
                expn_node->device_item_names->next = NULL;
                expn_node->device_item_names = NULL;
            }

            cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_new_expression, ezlopi_params_str);
            if (cj_params)
            {
                // 1. items
                cJSON *cj_items = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_items_str);
                if (cj_items)
                {
                    EZPI_scenes_expressions_populate_items(expn_node, cj_items);
                }

                // 2. device_items
                cJSON *cj_device_item_names = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_device_item_names_str);
                if (cj_device_item_names)
                {
                    EZPI_scenes_expressions_populate_devitem_names(expn_node, cj_device_item_names);
                }
            }
        }

        // 3 .  is_variable
        (NULL != cJSON_GetObjectItem(__FUNCTION__, cj_new_expression, ezlopi_variable_str)) ? (CJSON_GET_VALUE_BOOL(cj_new_expression, ezlopi_variable_str, expn_node->variable)) : NULL;

        // 4. Metadata
        {
            if (expn_node->meta_data)
            {
                cJSON_Delete(__FUNCTION__, expn_node->meta_data);
                expn_node->meta_data = NULL;
            }
            cJSON *cj_metaData = cJSON_GetObjectItem(__FUNCTION__, cj_new_expression, ezlopi_metadata_str);
            if (cj_metaData)
            {
                expn_node->meta_data = cJSON_Duplicate(__FUNCTION__, cj_metaData, cJSON_True);
            }
        }

        // 5. valueType
        {
            cJSON *cj_valueType = cJSON_GetObjectItem(__FUNCTION__, cj_new_expression, ezlopi_valueType_str);
            if (cj_valueType && cj_valueType->valuestring && cj_valueType->str_value_len)
            {
                expn_node->value_type = EZPI_core_scenes_value_get_type(cj_new_expression, ezlopi_valueType_str);
            }
        }

        // 6. Value  [exp_value]
        {
            // First delete value if :- string or object
            switch (expn_node->exp_value.type)
            {
            case EXPRESSION_VALUE_TYPE_STRING:
            {
                if (expn_node->exp_value.u_value.str_value)
                {
                    ezlopi_free(__FUNCTION__, expn_node->exp_value.u_value.str_value);
                    expn_node->exp_value.u_value.str_value = NULL;
                }
                break;
            }
            case EXPRESSION_VALUE_TYPE_CJ:
            {
                if (expn_node->exp_value.u_value.cj_value)
                {
                    cJSON_Delete(__FUNCTION__, expn_node->exp_value.u_value.cj_value);
                    expn_node->exp_value.u_value.cj_value = NULL;
                }
                break;
            }
            default:
                // TRACE_D("expn_valueType [%d]", expn_node->exp_value.type);
                break;
            }

            if (true == expn_node->variable) // only if variable--> store the 'constant' variable-value.
            {
                cJSON *cj_value = cJSON_GetObjectItem(__FUNCTION__, cj_new_expression, ezlopi_value_str);
                if (cj_value)
                {
                    __get_expressions_value(expn_node, cj_value);
                }
            }
        }
    }

    return ret;
}

static int ____get_simple_expn_result(cJSON *cj_des, lua_State *lua_state, const char *name, int status)
{
    int ret = 1;
    if (LUA_OK == status)
    {
        if (lua_isnumber(lua_state, -1))
        {
            float val_num = (float)lua_tonumber(lua_state, -1);
            // TRACE_D(" [num]: %.2lf", val_num);
            cJSON_AddNumberToObject(__FUNCTION__, cj_des, ezlopi_value_str, (float)((int)(val_num * 1000) / 1000.0f));
        }
        else if (lua_isstring(lua_state, -1))
        {
            const char *val_str = lua_tostring(lua_state, -1);
            // TRACE_D(" [str]: %s", val_str);
            cJSON_AddStringToObject(__FUNCTION__, cj_des, ezlopi_value_str, val_str);
        }
        else if (lua_isboolean(lua_state, -1))
        {
            bool value_bool = lua_toboolean(lua_state, -1);
            // TRACE_D(" [bool]: %d", value_bool);
            cJSON_AddBoolToObject(__FUNCTION__, cj_des, ezlopi_value_str, value_bool);
        }
    }
    else if (LUA_ERRERR == status)
    {
        ret = 0;
        const char *err = lua_tostring(lua_state, -1);
        TRACE_D(" [error]: %s", err);
        cJSON_AddStringToObject(__FUNCTION__, cj_des, ezlopi_error_str, err);
    }

    return ret;
}

static void ____get_devitem_expn_result(cJSON *cj_expr_val, lua_State *lua_state, const char *member_str)
{
    if (cj_expr_val && member_str)
    {
        lua_pushstring(lua_state, member_str);
        lua_gettable(lua_state, -2);
        if (!lua_isnil(lua_state, -1))
        {
            if (lua_isnumber(lua_state, -1))
            {
                float res = (float)lua_tonumber(lua_state, -1);
                // TRACE_D(" %s: %.2lf", member_str, res);
                cJSON_AddNumberToObject(__FUNCTION__, cj_expr_val, ezlopi_value_str, (float)((int)(res * 1000) / 1000.0f));
            }
            else if (lua_isstring(lua_state, -1))
            {
                const char *res = lua_tostring(lua_state, -1);
                // TRACE_D(" %s: %s", member_str, res);
                cJSON_AddStringToObject(__FUNCTION__, cj_expr_val, member_str, res);
            }
            else if (lua_isboolean(lua_state, -1))
            {
                bool res = lua_toboolean(lua_state, -1);
                // TRACE_D(" %s: %d", member_str, res);
                cJSON_AddBoolToObject(__FUNCTION__, cj_expr_val, member_str, res);
            }
            else
            {
                TRACE_E(" %s --> type:%d (need : LUA_TSTRING[4] / LUA_TNUMBER[3] / LUA_TBOOLEAN[1])", member_str, lua_type(lua_state, -1));
            }
            lua_pop(lua_state, 1);
        }
        else
        {
            TRACE_D("ERROR ! cannot find  {result.%s} in  result_table", member_str);
        }
    }
}

static void ___create_lua_subtable(lua_State *lua_state, l_ezlopi_item_t *item_prop, const char *sub_table_name)
{
    if (item_prop && sub_table_name)
    {
        lua_newtable(lua_state);
        cJSON *cj_result = cJSON_CreateObject(__FUNCTION__);
        if (cj_result)
        {
            item_prop->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, item_prop, cj_result, item_prop->user_arg);
            cJSON *cj_value = cJSON_GetObjectItem(__FUNCTION__, cj_result, ezlopi_value_str);
            if (cj_value)
            {
                switch (cj_value->type)
                {
                case cJSON_True:
                case cJSON_False:
                {
                    lua_create_table_bool_key_value(ezlopi_value_str, cj_value->valuedouble ? true : false);
                    break;
                }
                case cJSON_Number:
                {
                    lua_create_table_number_key_value(ezlopi_value_str, cj_value->valuedouble);
                    break;
                }
                case cJSON_String:
                {
                    if (cj_value->valuestring)
                    {
                        lua_create_table_string_key_value(ezlopi_value_str, cj_value->valuestring);
                    }
                    break;
                }
                default:
                    break;
                }
            }
            cJSON_Delete(__FUNCTION__, cj_result);
        }
        lua_create_table_string_key_value(ezlopi_scale_str, item_prop->cloud_properties.scale);
        lua_setfield(lua_state, -2, sub_table_name);
    }
}

static void __get_expressions_value(s_ezlopi_expressions_t *exp_node, cJSON *cj_value)
{
    if (exp_node && cj_value)
    {
        switch (cj_value->type)
        {
        case cJSON_Number:
        {
            exp_node->exp_value.u_value.number_value = cj_value->valuedouble;
            exp_node->exp_value.type = EXPRESSION_VALUE_TYPE_NUMBER;
            TRACE_I("value: %lf", exp_node->exp_value.u_value.number_value);
            break;
        }
        case cJSON_String:
        {
            uint32_t value_len = strlen(cj_value->valuestring) + 1;
            exp_node->exp_value.type = EXPRESSION_VALUE_TYPE_STRING;
            exp_node->exp_value.u_value.str_value = ezlopi_malloc(__FUNCTION__, value_len);
            if (exp_node->exp_value.u_value.str_value)
            {
                snprintf(exp_node->exp_value.u_value.str_value, value_len, "%s", cj_value->valuestring);
                TRACE_I("value: %s", exp_node->exp_value.u_value.str_value);
            }
            else
            {
                TRACE_E("Malloc failed!");
            }
            break;
        }
        case cJSON_True:
        {
            exp_node->exp_value.type = EXPRESSION_VALUE_TYPE_BOOL;
            exp_node->exp_value.u_value.boolean_value = true;
            TRACE_I("value: true");
            break;
        }
        case cJSON_False:
        {
            exp_node->exp_value.type = EXPRESSION_VALUE_TYPE_BOOL;
            exp_node->exp_value.u_value.boolean_value = false;
            TRACE_I("value: false");
            break;
        }
        case cJSON_Array:
        case cJSON_Object:
        {
            TRACE_E("cj_value type: %d", cj_value->type);
            exp_node->exp_value.type = EXPRESSION_VALUE_TYPE_CJ;
            exp_node->exp_value.u_value.cj_value = cJSON_Duplicate(__FUNCTION__, cj_value, true);
            break;
        }
        default:
        {
            exp_node->exp_value.u_value.number_value = 0;
            exp_node->exp_value.type = EXPRESSION_VALUE_TYPE_UNDEFINED;
            TRACE_E("Error: Undefine value type: %d", cj_value->type);
            break;
        }
        }
    }
}

static int __evaluate_expression(cJSON *cj_des, cJSON *lua_prop_params, const char *exp_name, const char *exp_code)
{
    int ret = 0;
    lua_State *lua_state = luaL_newstate();
    if (lua_state && exp_name && exp_code)
    {
        luaL_openlibs(lua_state);
        {
            if (lua_prop_params && (NULL != cJSON_GetObjectItem(__FUNCTION__, lua_prop_params, ezlopi_device_item_names_str) || NULL != cJSON_GetObjectItem(__FUNCTION__, lua_prop_params, ezlopi_items_str))) // if , params exits
            {
                // uint8_t total_key_count = 0;
                // create 'params' tables.
                lua_newtable(lua_state); // params

                // if 'device_item_names' exists
                cJSON *cj_device_item_names = NULL;
                cJSON *cj_items = NULL;

                cj_device_item_names = cJSON_GetObjectItem(__FUNCTION__, lua_prop_params, ezlopi_device_item_names_str);
                cj_items = cJSON_GetObjectItem(__FUNCTION__, lua_prop_params, ezlopi_items_str);

                // 1. If 'device_item_names' is given
                if (cj_device_item_names && cJSON_IsArray(cj_device_item_names))
                {
                    cJSON *cj_device_item = NULL;
                    cJSON_ArrayForEach(cj_device_item, cj_device_item_names)
                    {
                        // get the item id
                        cJSON *cj_target_name = cJSON_GetObjectItem(__FUNCTION__, cj_device_item, ezlopi_name_str);
                        cJSON *cj_dev_name = cJSON_GetObjectItem(__FUNCTION__, cj_device_item, ezlopi_deviceName_str);
                        cJSON *cj_item_name = cJSON_GetObjectItem(__FUNCTION__, cj_device_item, ezlopi_itemName_str);
                        if ((cj_target_name && cj_target_name->valuestring) && (cj_dev_name && cj_dev_name->valuestring) && (cj_item_name && cj_item_name->valuestring))
                        {
                            l_ezlopi_device_t *curr_dev_node = EZPI_core_device_get_head();
                            while (curr_dev_node)
                            {
                                if (EZPI_STRNCMP_IF_EQUAL(cj_dev_name->valuestring, curr_dev_node->cloud_properties.device_name, cj_dev_name->str_value_len, strlen(curr_dev_node->cloud_properties.device_name) + 1)) // "deviceName" == "siren"
                                {
                                    l_ezlopi_item_t *item_prop = curr_dev_node->items;
                                    while (item_prop)
                                    {
                                        if (EZPI_STRNCMP_IF_EQUAL(cj_item_name->valuestring, item_prop->cloud_properties.item_name, cj_item_name->str_value_len, strlen(item_prop->cloud_properties.item_name) + 1)) // "itemName" == "sound_level"
                                        {                                                                                                                                                                            // create the sub-table
                                            ___create_lua_subtable(lua_state, item_prop, cj_target_name->valuestring);
                                            // total_key_count++;
                                            // TRACE_D(" [items_%d] : adding '%s'.", total_key_count, cj_target_name->valuestring);
                                            break;
                                        }

                                        item_prop = item_prop->next;
                                    }
                                    break;
                                }
                                curr_dev_node = curr_dev_node->next;
                            }
                        }
                    }
                }
                // 2. If 'items' is given
                if (cj_items && cJSON_IsArray(cj_items))
                {
                    cJSON *cj_item = NULL;
                    cJSON_ArrayForEach(cj_item, cj_items)
                    {
                        cJSON *cj_item_name = cJSON_GetObjectItem(__FUNCTION__, cj_item, ezlopi_name_str);
                        cJSON *cj_item_id = cJSON_GetObjectItem(__FUNCTION__, cj_item, ezlopi__id_str);

                        if ((cj_item_id && cj_item_id->valuestring) && (cj_item_name && cj_item_name->valuestring))
                        {
                            uint32_t _id = strtoul(cj_item_id->valuestring, NULL, 16);
                            l_ezlopi_item_t *item_prop = EZPI_core_device_get_item_by_id(_id);

                            if (item_prop)
                            {
                                ___create_lua_subtable(lua_state, item_prop, cj_item_name->valuestring);
                                // total_key_count++;
                                // TRACE_D(" [items_%d] : adding '%s'.", total_key_count, cj_item_name->valuestring);
                            }
                        }
                    }
                }
                // Set the 'params' table as a global variable
                lua_setglobal(lua_state, "params");
            }

            // perform the lua-script executions
            int tmp_ret = luaL_loadstring(lua_state, exp_code);
            if (LUA_OK == tmp_ret)
            {
                tmp_ret = lua_pcall(lua_state, 0, 1, 0);
                if (LUA_OK == tmp_ret)
                {
                    if (lua_istable(lua_state, -1)) // checks if 'params' is a table
                    {
                        cJSON *cj_expr_val = cJSON_AddObjectToObject(__FUNCTION__, cj_des, ezlopi_value_str);
                        if (cj_expr_val)
                        {
                            ____get_devitem_expn_result(cj_expr_val, lua_state, ezlopi_value_str);
                            ____get_devitem_expn_result(cj_expr_val, lua_state, ezlopi_scale_str);
                            ret = 1;
                        }
                        lua_pop(lua_state, 1); // Remove result table (params)
                    }
                    else
                    {
                        ret = ____get_simple_expn_result(cj_des, lua_state, exp_name, tmp_ret);
                    }
                }
                else
                {
                    const char *error_msg = lua_tostring(lua_state, -1);
                    TRACE_D("LUA Runtime error [%d]: %s", tmp_ret, error_msg);
                    cJSON_AddStringToObject(__FUNCTION__, cj_des, ezlopi_error_str, error_msg);
                    lua_pop(lua_state, -1); // Remove the error message from the stack
                }
            }
            else
            {
                const char *error_msg = lua_tostring(lua_state, -1);
                TRACE_D("LUA Compile error [%d]: %s", tmp_ret, error_msg);
                cJSON_AddStringToObject(__FUNCTION__, cj_des, ezlopi_error_str, error_msg);
                lua_pop(lua_state, -1); // Remove the error message from the stack
            }
        }
        lua_close(lua_state);
    }
    else
    {
        TRACE_E("Couldn't create lua state for -> %s", exp_name);
    }
    return ret;
}

static bool __check_expression_type_filter(s_ezlopi_expressions_t *exp_node, e_scene_value_type_v2_t *type_filter_arr)
{
    bool ret = true;
    if (type_filter_arr)
    {
        ret = false;
        uint32_t idx = 0;
        while (type_filter_arr[idx])
        {
            if (type_filter_arr[idx] == exp_node->value_type)
            {
                ret = true;
                break;
            }
            idx++;
        }
    }

    return ret;
}

static s_exp_items_t *__expressions_items_create(cJSON *cj_item)
{
    s_exp_items_t *new_item_node = NULL;

    if (cj_item)
    {
        new_item_node = ezlopi_malloc(__FUNCTION__, sizeof(s_exp_items_t));
        if (new_item_node)
        {
            memset(new_item_node, 0, sizeof(s_exp_items_t));
            CJSON_GET_VALUE_STRING_BY_COPY(cj_item, ezlopi_name_str, new_item_node->name, sizeof(new_item_node->name));

            new_item_node->_id = EZPI_core_cjson_get_id(cj_item, ezlopi__id_str);
            // CJSON_GET_ID(new_item_node->_id, cJSON_GetObjectItem(__FUNCTION__, cj_item, ezlopi__id_str));
        }
        else
        {
            TRACE_E("Error: malloc failed!");
        }
    }

    return new_item_node;
}

static s_exp_device_item_names_t *__expressions_device_item_names_create(cJSON *cj_device_item_name)
{
    s_exp_device_item_names_t *new_device_item_name = NULL;

    if (cj_device_item_name)
    {
        new_device_item_name = ezlopi_malloc(__FUNCTION__, sizeof(s_exp_device_item_names_t));
        if (new_device_item_name)
        {
            memset(new_device_item_name, 0, sizeof(s_exp_device_item_names_t));
            CJSON_GET_VALUE_STRING_BY_COPY(cj_device_item_name, ezlopi_name_str, new_device_item_name->name, sizeof(new_device_item_name->name));
            CJSON_GET_VALUE_STRING_BY_COPY(cj_device_item_name, ezlopi_deviceName_str, new_device_item_name->device_name, sizeof(new_device_item_name->device_name));
            CJSON_GET_VALUE_STRING_BY_COPY(cj_device_item_name, ezlopi_itemName_str, new_device_item_name->item_name, sizeof(new_device_item_name->item_name));
        }
    }

    return new_device_item_name;
}

static s_ezlopi_expressions_t *__expressions_create_node(uint32_t exp_id, cJSON *cj_expression)
{
    s_ezlopi_expressions_t *new_exp_node = ezlopi_malloc(__FUNCTION__, sizeof(s_ezlopi_expressions_t));

    if (new_exp_node)
    {
        memset(new_exp_node, 0, sizeof(s_ezlopi_expressions_t));

        CJSON_GET_VALUE_STRING_BY_COPY(cj_expression, ezlopi_name_str, new_exp_node->name, sizeof(new_exp_node->name));

        cJSON *cj_code = cJSON_GetObjectItem(__FUNCTION__, cj_expression, ezlopi_code_str);
        if (cj_code && cj_code->valuestring && cj_code->str_value_len)
        {
            new_exp_node->code = ezlopi_malloc(__FUNCTION__, cj_code->str_value_len + 1);
            if (new_exp_node->code)
            {
                snprintf(new_exp_node->code, cj_code->str_value_len + 1, "%.*s", cj_code->str_value_len, cj_code->valuestring);
            }
        }

        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_expression, ezlopi_params_str);

        if (cj_params)
        {
            cJSON *cj_items = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_items_str);
            EZPI_scenes_expressions_populate_items(new_exp_node, cj_items);

            cJSON *cj_device_item_names = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_device_item_names_str);
            EZPI_scenes_expressions_populate_devitem_names(new_exp_node, cj_device_item_names);
        }

        CJSON_GET_VALUE_BOOL(cj_expression, ezlopi_variable_str, new_exp_node->variable);

        cJSON *cj_metaData = cJSON_GetObjectItem(__FUNCTION__, cj_expression, ezlopi_metadata_str);
        if (cj_metaData)
        {
            new_exp_node->meta_data = cJSON_Duplicate(__FUNCTION__, cj_metaData, true);
        }

        new_exp_node->value_type = EZPI_core_scenes_value_get_type(cj_expression, ezlopi_valueType_str);

        if (new_exp_node->variable) // if true ; incoming 'Value' should be saved into nvs
        {
            __get_expressions_value(new_exp_node, cJSON_GetObjectItem(__FUNCTION__, cj_expression, ezlopi_value_str));
        }

        new_exp_node->exp_id = __expression_store_to_nvs(exp_id, cj_expression);
        // EZPI_scenes_expressions_print(new_exp_node);
    }

    return new_exp_node;
}

static uint32_t __expression_store_to_nvs(uint32_t exp_id, cJSON *cj_expression)
{
    if (0 == exp_id)
    {
        char *exp_string = cJSON_PrintBuffered(__FUNCTION__, cj_expression, 1024, false);
        TRACE_D("length of 'exp_string': %d", strlen(exp_string));

        if (exp_string)
        {
            exp_id = EZPI_core_cloud_generate_expression_id();
            if (exp_id)
            {
                char exp_id_str[32];
                snprintf(exp_id_str, sizeof(exp_id_str), "%08x", exp_id);

                if (EZPI_SUCCESS == EZPI_core_nvs_write_str(exp_string, strlen(exp_string), exp_id_str))
                {
                    bool free_exp_id_list_str = 1;
                    char *exp_id_list_str = EZPI_core_nvs_read_scenes_expressions();
                    if (NULL == exp_id_list_str)
                    {
                        exp_id_list_str = "[]";
                        free_exp_id_list_str = 0;
                        TRACE_W("Expressions ids-list not found in NVS");
                    }

                    TRACE_D("Expressions-IDs: %s", exp_id_list_str);
                    cJSON *cj_exp_id_list = cJSON_Parse(__FUNCTION__, exp_id_list_str);

                    if (free_exp_id_list_str)
                    {
                        ezlopi_free(__FUNCTION__, exp_id_list_str);
                        exp_id_list_str = NULL;
                    }

#ifdef CONFIG_EZPI_UTIL_TRACE_EN
                    CJSON_TRACE("cj_esp-ids", cj_exp_id_list);
#endif

                    if (cj_exp_id_list)
                    {
                        // TRACE_D("Here");
                        cJSON *cj_exp_id = cJSON_CreateString(__FUNCTION__, exp_id_str);
                        if (cj_exp_id)
                        {
                            // TRACE_D("Here");
                            if (cJSON_AddItemToArray(cj_exp_id_list, cj_exp_id))
                            {
                                exp_id_list_str = cJSON_PrintBuffered(__FUNCTION__, cj_exp_id_list, 1024, false);
                                TRACE_D("length of 'exp_id_list_str': %d", strlen(exp_id_list_str));

                                if (exp_id_list_str)
                                {
                                    EZPI_core_nvs_write_scenes_expressions(exp_id_list_str);
                                    ezlopi_free(__FUNCTION__, exp_id_list_str);
                                }
                            }
                            else
                            {
                                cJSON_Delete(__FUNCTION__, cj_exp_id);
                            }
                        }

                        cJSON_Delete(__FUNCTION__, cj_exp_id_list);
                    }
                }
                else
                {
                    TRACE_E("failed to store new_exprn [%s]", exp_id_str);
                }
            }

            ezlopi_free(__FUNCTION__, exp_string);
        }
    }
    else
    {
        EZPI_core_cloud_update_expression_id(exp_id);
    }

    return exp_id;
}

static e_scene_value_type_v2_t *__parse_expression_type_filter(cJSON *cj_params)
{
    e_scene_value_type_v2_t *type_filter_array = NULL;
    cJSON *cj_types_filter_array = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_filterTypes_str);
    if (cj_types_filter_array)
    {
        type_filter_array = ezlopi_calloc(__FUNCTION__, sizeof(e_scene_value_type_v2_t), cJSON_GetArraySize(cj_types_filter_array) + 1);
        if (type_filter_array)
        {
            uint32_t idx = 0;
            cJSON *cj_type = NULL;
            cJSON_ArrayForEach(cj_type, cj_types_filter_array)
            {
                type_filter_array[idx] = EZPI_core_scenes_value_get_type(cj_type, NULL);
                idx++;
            }
            type_filter_array[idx] = 0;
        }
    }

    return type_filter_array;
}

static void __add_expression_value(s_ezlopi_expressions_t *exp_node, cJSON *cj_expr)
{
    if (EZLOPI_VALUE_TYPE_NONE < exp_node->value_type && EZLOPI_VALUE_TYPE_MAX > exp_node->value_type)
    {
        cJSON_AddStringToObject(__FUNCTION__, cj_expr, ezlopi_valueType_str, EZPI_core_scenes_get_scene_value_type_name(exp_node->value_type));
        switch (exp_node->exp_value.type)
        {
        case EXPRESSION_VALUE_TYPE_STRING:
        {
            cJSON_AddStringToObject(__FUNCTION__, cj_expr, ezlopi_value_str, exp_node->exp_value.u_value.str_value);
            break;
        }
        case EXPRESSION_VALUE_TYPE_BOOL:
        {
            cJSON_AddBoolToObject(__FUNCTION__, cj_expr, ezlopi_value_str, exp_node->exp_value.u_value.boolean_value);
            break;
        }
        case EXPRESSION_VALUE_TYPE_NUMBER:
        {
            cJSON_AddNumberToObject(__FUNCTION__, cj_expr, ezlopi_value_str, exp_node->exp_value.u_value.number_value);
            break;
        }
        case EXPRESSION_VALUE_TYPE_CJ:
        {
            cJSON_AddItemToObject(__FUNCTION__, cj_expr, ezlopi_value_str, cJSON_Duplicate(__FUNCTION__, exp_node->exp_value.u_value.cj_value, 1));
            break;
        }

        default:
            break;
        }
    }
}

static void __add_expression_items(s_ezlopi_expressions_t *exp_node, cJSON *cj_params)
{
    if (exp_node->items)
    {
        cJSON *cj_items = cJSON_AddArrayToObject(__FUNCTION__, cj_params, ezlopi_items_str);
        if (cj_items)
        {
            s_exp_items_t *curr_item = exp_node->items;
            while (curr_item)
            {
                cJSON *cj_item = cJSON_CreateObject(__FUNCTION__);
                if (cj_item)
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_item, ezlopi_name_str, curr_item->name);

                    char id_str[32];
                    snprintf(id_str, sizeof(id_str), "%08x", curr_item->_id);
                    cJSON_AddStringToObject(__FUNCTION__, cj_item, ezlopi__id_str, id_str);
                    if (!cJSON_AddItemToArray(cj_items, cj_item))
                    {
                        cJSON_Delete(__FUNCTION__, cj_item);
                    }
                }
                curr_item = curr_item->next;
            }
        }
    }
}

static void __add_expression_device_item_names(s_ezlopi_expressions_t *exp_node, cJSON *cj_params)
{
    if (exp_node->device_item_names)
    {
        cJSON *cj_device_item_names = cJSON_AddArrayToObject(__FUNCTION__, cj_params, ezlopi_device_item_names_str);
        if (cj_device_item_names)
        {
            s_exp_device_item_names_t *curr_device_item_names = exp_node->device_item_names;
            while (curr_device_item_names)
            {
                cJSON *cj_device_item_name = cJSON_CreateObject(__FUNCTION__);
                if (cj_device_item_name)
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_device_item_name, ezlopi_name_str, curr_device_item_names->name);
                    cJSON_AddStringToObject(__FUNCTION__, cj_device_item_name, ezlopi_deviceName_str, curr_device_item_names->device_name);
                    cJSON_AddStringToObject(__FUNCTION__, cj_device_item_name, ezlopi_itemName_str, curr_device_item_names->item_name);

                    if (!cJSON_AddItemToArray(cj_device_item_names, cj_device_item_name))
                    {
                        cJSON_Delete(__FUNCTION__, cj_device_item_name);
                    }
                }
                curr_device_item_names = curr_device_item_names->next;
            }
        }
    }
}

static int __remove_exp_id_from_nvs_exp_list(uint32_t target_id)
{
    int ret = 0;
    /*  Now to update_list_in_nvs*/
    char *exp_ids = EZPI_core_nvs_read_scenes_expressions();
    if (exp_ids)
    {
        cJSON *cj_exp_ids = cJSON_Parse(__FUNCTION__, exp_ids);
        ezlopi_free(__FUNCTION__, exp_ids);

        if (cj_exp_ids)
        {
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
            CJSON_TRACE("expression-ids", cj_exp_ids);
#endif

            uint32_t idx = 0;
            cJSON *cj_exp_id = NULL;
            cJSON_ArrayForEach(cj_exp_id, cj_exp_ids)
            {
                uint32_t _id = strtoul(cj_exp_id->valuestring, NULL, 16);
                if (_id == target_id)
                {
                    cJSON_DeleteItemFromArray(__FUNCTION__, cj_exp_ids, idx);
                }

                // iterate upto last elements (incase -->  for doubles)
                idx++;
            }

            char *updated_ids_str = cJSON_PrintBuffered(__FUNCTION__, cj_exp_ids, 1024, false);
            TRACE_D("length of 'updated_ids_str': %d", strlen(updated_ids_str));

            cJSON_Delete(__FUNCTION__, cj_exp_ids);

            if (updated_ids_str)
            {
                TRACE_D("updated-expression-ids: %s", updated_ids_str);
                EZPI_core_nvs_write_scenes_expressions(updated_ids_str);
                ezlopi_free(__FUNCTION__, updated_ids_str);
                ret = 1;
            }
        }
    }
    return ret;
}

/*******************************************************************************
 *                          End of File
 *******************************************************************************/

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS
