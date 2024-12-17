
/**
 * @file    ezlopi_cloud_scenes_expressions.h
 * @brief
 * @author
 * @version
 * @date
 */
/* ===========================================================================
** Copyright (C) 2022 Ezlo Innovation Inc
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

#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include "ezlopi_util_trace.h"

#include "ezlopi_core_broadcast.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_scenes_expressions.h"
#include "ezlopi_core_errors.h"

#include "ezlopi_cloud_constants.h"
#include "ezlopi_cloud_scenes_expressions.h"

/**
 * @brief Function to broadcast scenes expressions changed
 *
 * @param cj_request Incoming JSON request
 * @param cj_response Outgoing JSON response
 */
static void ezpi_scenes_expressions_changed(cJSON *cj_request, cJSON *cj_response);
/**
 * @brief Function to broadcast scenes expressions added
 *
 * @param cj_request Incoming JSON request
 * @param cj_response Outgoing JSON response
 */
static void ezpi_scenes_expressions_added(cJSON *cj_request, cJSON *cj_response);
/**
 * @brief Function to broadcast scenes expressions added and changed
 *
 * @param cj_request Incoming JSON request
 * @param cj_response Outgoing JSON response
 */
static void ezpi_common_part_of_scenes_expressions_added_and_changed(cJSON *cj_request, cJSON *cj_response);

void EZPI_scenes_expressions_delete(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);

        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON *cj_expression_name = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_name_str);
            if (cj_expression_name && cj_expression_name->valuestring)
            {
                ezlopi_scenes_expressions_delete_by_name(cj_expression_name->valuestring);
            }
        }
    }
}

void EZPI_scenes_expressions_list(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            // CJSON_TRACE("exp-params", cj_params);

            cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
            if (cj_result)
            {
                cJSON *cj_expressions_array = cJSON_AddArrayToObject(__FUNCTION__, cj_result, ezlopi_expressions_str);
                if (cj_expressions_array)
                {
                    ezlopi_scenes_expressions_list_cjson(cj_expressions_array, cj_params);
                }
            }
        }
    }
}

void EZPI_scenes_expressions_set(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);

        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            // CJSON_TRACE("expressions params", cj_params);
            cJSON *cj_name = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_name_str);
            if (cj_name && cj_name->valuestring)
            {
                s_ezlopi_expressions_t *curr_expn_node = ezlopi_scenes_expression_get_by_name(cj_name->valuestring);
                if (NULL != curr_expn_node)
                {
                    ezlopi_scenes_expression_update_expr(curr_expn_node, cj_params);
                }
                else
                {
                    ezlopi_scenes_expressions_add_to_head(0, cj_params);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------------------------------------
//                  EXPRESSION UPDATERS
//-----------------------------------------------------------------------------------------------------------

////// updater for scene.expressions
////// for 'hub.scenes.expressions.set'
void EZPI_scenes_expressions_added_changed(cJSON *cj_request, cJSON *cj_response)
{
    // 1. broadcast 'added'
    cJSON *response1 = cJSON_CreateObject(__FUNCTION__);
    if (response1)
    {
        ezpi_scenes_expressions_added(cj_request, response1);
        if (EZPI_SUCCESS != ezlopi_core_broadcast_add_to_queue(response1))
        {
            cJSON_Delete(__FUNCTION__, response1);
        }
    }

    // 2. for 'changed' - return  'cj_response'
    ezpi_scenes_expressions_changed(cj_request, cj_response);
}

void EZPI_scenes_expressions_deleted(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_sender_str);
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_error_str);

    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_expression_deleted_str);

    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON *cj_expression_name = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_name_str);
            if (cj_expression_name && cj_expression_name->valuestring)
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_name_str, cj_expression_name->valuestring);
            }
        }
    }
}

static void ezpi_scenes_expressions_added(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_sender_str);
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_error_str);

    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_expression_added_str);

    ezpi_common_part_of_scenes_expressions_added_and_changed(cj_request, cj_response);
}

static void ezpi_scenes_expressions_changed(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_sender_str);
    cJSON_DeleteItemFromObject(__FUNCTION__, cj_response, ezlopi_error_str);

    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, ezlopi_hub_expression_changed_str);

    ezpi_common_part_of_scenes_expressions_added_and_changed(cj_request, cj_response);
}

static void ezpi_common_part_of_scenes_expressions_added_and_changed(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON *cj_expression_name = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_name_str);
            if (cj_expression_name && cj_expression_name->valuestring)
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_name_str, cj_expression_name->valuestring);
            }

            cJSON *cj_expression_code = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_code_str);
            if (cj_expression_code && cj_expression_code->valuestring)
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_code_str, cj_expression_code->valuestring);
            }

            cJSON *cj_valueType = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_valueType_str);
            if (cj_valueType && cj_valueType->valuestring)
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_valueType_str, cj_valueType->valuestring);
            }

            cJSON *cj_metadata = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_metadata_str);
            if (cj_metadata)
            {
                cJSON_AddItemToObject(__FUNCTION__, cj_result, ezlopi_metadata_str, cJSON_Duplicate(__FUNCTION__, cj_metadata, cJSON_True));
            }

            cJSON *cj_variable = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_variable_str);
            if (cj_variable && cJSON_IsBool(cj_variable))
            {
                cJSON_AddBoolToObject(__FUNCTION__, cj_result, ezlopi_variable_str, (cJSON_True == cj_variable->type) ? true : false);
            }

            cJSON *cj_value = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_value_str);
            if (cj_value)
            {
                switch (cj_value->type)
                {
                case cJSON_Number:
                {
                    cJSON_AddNumberToObject(__FUNCTION__, cj_result, ezlopi_value_str, cj_value->valuedouble);
                    break;
                }
                case cJSON_String:
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_value_str, cj_value->valuestring);
                    break;
                }
                case cJSON_True:
                {
                    cJSON_AddBoolToObject(__FUNCTION__, cj_result, ezlopi_value_str, cJSON_True);
                    break;
                }
                case cJSON_False:
                {
                    cJSON_AddBoolToObject(__FUNCTION__, cj_result, ezlopi_value_str, cJSON_False);
                    break;
                }
                case cJSON_Array:
                case cJSON_Object:
                {
                    cJSON_AddItemToObject(__FUNCTION__, cj_result, ezlopi_value_str, cJSON_Duplicate(__FUNCTION__, cj_value, cJSON_True));
                    break;
                }
                default:
                {
                    TRACE_E("Error: Undefined value type: %d", cj_value->type);
                    break;
                }
                }
            }
        }
    }
}

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
