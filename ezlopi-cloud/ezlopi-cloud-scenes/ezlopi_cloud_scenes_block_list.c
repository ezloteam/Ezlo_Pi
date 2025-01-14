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
 * @file    ezlopi_cloud_scenes_block_list.c
 * @brief
 * @author
 * @version
 * @date
 */
/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include <string.h>
#include <stdint.h>

#include "ezlopi_util_trace.h"
// #include "cjext.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_scenes_cjson.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_scenes_operators.h"

#include "ezlopi_cloud_scenes.h"
#include "ezlopi_cloud_constants.h"

#include "ezlopi_service_meshbot.h"

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
/**
 * @brief Function to convert when block to JSON
 *
 * @param when_block Pointer to the when block to convert
 * @return cJSON*
 */
static cJSON *__create_when_block_cjson(l_when_block_v2_t *when_block);
/**
 * @brief Function to add block optiosn and fields to the JSON
 *
 * @param cj_block Pointer to the JSON to add to
 * @param block_options Pointer to the block option
 * @param fields_node Pointer to the field node
 */
static void __add_block_options_and_fields_cjson(cJSON *cj_block, s_block_options_v2_t *block_options, l_fields_v2_t *fields_node);
/**
 * @brief Function to trigger device list
 *
 * @param cj_devices_array Device's JSON array
 * @return int
 */
static int __scenes_block_trigger_device_list(cJSON *cj_devices_array);
/**
 * @brief Function to create JSON of a then block
 *
 * @param then_block Pointer to the then block to convert
 * @return cJSON*
 */
static cJSON *__create_then_block_cjson(l_action_block_v2_t *then_block);
/**
 * @brief Function to create JSON of else block
 *
 * @param else_block Pointer to else block to convert
 * @return cJSON*
 */
static cJSON *__create_else_block_cjson(l_action_block_v2_t *else_block);
/**
 * @brief Function that interates through when block to find the required item id
 *
 * @param curr_when_block Pointer to the when block to iterate through
 * @param item_id Item id to find
 * @return true
 * @return false
 */
static bool ____iterate_field_block_to_find_item_id(l_when_block_v2_t *curr_when_block, uint32_t item_id);
/**
 * @brief Function to return if item id is in the field
 *
 * @param field_node Pointer to the field to search at
 * @param item_id Item id to search for
 * @return true
 * @return false
 */
static bool __found_item_in_field(l_fields_v2_t *field_node, uint32_t item_id);
/**
 * @brief Function to add scenes block using device IDs
 *
 * @param block_type Block type to add
 * @param cj_devices_array Pointer to the device JSON array
 * @return cJSON*
 */
static cJSON *__add_scenes_blocks_by_device_ids(e_scenes_block_type_v2_t block_type, cJSON *cj_devices_array);
/**
 * @brief Function to add scenes block using item IDs
 *
 * @param block_type Blcok type to add
 * @param item_list Poninter to the item list
 * @return cJSON*
 */
static cJSON *__add_scenes_blocks_by_item_ids(e_scenes_block_type_v2_t block_type, l_ezlopi_item_t *item_list);
/**
 * @brief Function to get the block type and create block array
 *
 * @param cj_result Pointer to the JSON
 * @param block_type_name Name of the block type
 * @param cj_block_type Pointer to the block type array
 * @return e_scenes_block_type_v2_t
 */
static e_scenes_block_type_v2_t __get_block_type_and_create_block_array(cJSON *cj_result, char const **block_type_name, cJSON *cj_block_type);

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
void EZPI_scenes_trigger_device_list(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str); // For NULL broadcast
    if (cj_result)
    {
        cJSON *cj_devices_array = cJSON_AddArrayToObject(__FUNCTION__, cj_result, ezlopi_devices_str);
        if (cj_devices_array)
        {
            if (0 < __scenes_block_trigger_device_list(cj_devices_array))
            {
                CJSON_TRACE("trigger-device-list", cj_devices_array);
            }
        }
    }
}

void EZPI_scenes_blocks_list(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str); // For NULL broadcast
    if (cj_result)
    {
        cJSON *cj_paramas = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_paramas)
        {
            cJSON *cj_block_type = cJSON_GetObjectItem(__FUNCTION__, cj_paramas, ezlopi_blockType_str);
            if (cj_block_type && cj_block_type->valuestring)
            {
                char *block_type_name = NULL;
                e_scenes_block_type_v2_t block_type = __get_block_type_and_create_block_array(cj_result, (const char **)&block_type_name, cj_block_type);

                if (block_type)
                {
                    cJSON *cj_devices_array = cJSON_GetObjectItem(__FUNCTION__, cj_paramas, ezlopi_devices_str);
                    if (cj_devices_array && (cJSON_Array == cj_devices_array->type))
                    {
                        cJSON *cj_block_array = __add_scenes_blocks_by_device_ids(block_type, cj_devices_array);
                        if (cj_block_array)
                        {
                            if (!cJSON_AddItemToObject(__FUNCTION__, cj_result, block_type_name, cj_block_array))
                            {
                                cJSON_Delete(__FUNCTION__, cj_block_array);
                            }
                        }
                    }
                }
            }
        }
    }
}

/*******************************************************************************
 *                          Static Function Definitions
 *******************************************************************************/
static void __add_block_options_and_fields_cjson(cJSON *cj_block, s_block_options_v2_t *block_options, l_fields_v2_t *fields_node)
{
    cJSON *cj_block_opt = cJSON_AddObjectToObject(__FUNCTION__, cj_block, ezlopi_blockOptions_str);
    cJSON *cj_fields = cJSON_AddArrayToObject(__FUNCTION__, cj_block, ezlopi_fields_str);
    if (cj_block_opt && cj_fields)
    {
        cJSON *cj_method = cJSON_AddObjectToObject(__FUNCTION__, cj_block_opt, ezlopi_method_str);
        if (cj_method)
        {
            cJSON_AddStringToObject(__FUNCTION__, cj_method, ezlopi_name_str, block_options->method.name);
            cJSON *cj_args = cJSON_AddObjectToObject(__FUNCTION__, cj_method, ezlopi_args_str);
            if (cj_args)
            {
                while (fields_node)
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_args, fields_node->name, fields_node->name);

                    cJSON *cj_field_obj = EZPI_scene_cjson_get_field(fields_node);
                    if (!cJSON_AddItemToArray(cj_fields, cj_field_obj))
                    {
                        cJSON_Delete(__FUNCTION__, cj_field_obj);
                    }

                    fields_node = fields_node->next;
                }
            }
        }
    }
}

static cJSON *__create_when_block_cjson(l_when_block_v2_t *when_block)
{
    cJSON *cj_when_block = NULL;

    if (when_block)
    {
        cj_when_block = cJSON_CreateObject(__FUNCTION__);
        if (cj_when_block)
        {
            __add_block_options_and_fields_cjson(cj_when_block, &when_block->block_options, when_block->fields);
            cJSON_AddStringToObject(__FUNCTION__, cj_when_block, ezlopi_blockType_str, ezlopi_when_str);
        }
    }

    return cj_when_block;
}

static cJSON *__create_then_block_cjson(l_action_block_v2_t *then_block)
{
    cJSON *cj_then_block = NULL;

    if (then_block)
    {
        cj_then_block = cJSON_CreateObject(__FUNCTION__);
        if (cj_then_block)
        {
            __add_block_options_and_fields_cjson(cj_then_block, &then_block->block_options, then_block->fields);
            cJSON_AddStringToObject(__FUNCTION__, cj_then_block, ezlopi_blockType_str, ezlopi_then_str);
        }
    }

    return cj_then_block;
}

static cJSON *__create_else_block_cjson(l_action_block_v2_t *else_block)
{
    cJSON *cj_else_block = NULL;

    if (else_block)
    {
        cj_else_block = cJSON_CreateObject(__FUNCTION__);
        if (cj_else_block)
        {
            __add_block_options_and_fields_cjson(cj_else_block, &else_block->block_options, else_block->fields);
            cJSON_AddStringToObject(__FUNCTION__, cj_else_block, ezlopi_blockType_str, ezlopi_else_str);
        }
    }

    return cj_else_block;
}

static bool ____iterate_field_block_to_find_item_id(l_when_block_v2_t *curr_when_block, uint32_t item_id)
{
    bool ret = false;
    if (curr_when_block->next)
    {
        if (____iterate_field_block_to_find_item_id(curr_when_block->next, item_id))
        {
            return true;
        }
    }

    ret = __found_item_in_field(curr_when_block->fields, item_id);

    return ret;
}

static bool __found_item_in_field(l_fields_v2_t *field_node, uint32_t item_id)
{
    bool ret = false;
    uint32_t item_id_check = 0;
    while (field_node)
    {
        switch (field_node->value_type)
        {
        case EZLOPI_VALUE_TYPE_ITEM:
        {
            item_id_check = strtoul(field_node->field_value.u_value.value_string, NULL, 16);
            // TRACE_D("item-id: %s | %08x", field_node->field_value.u_value.value_string, item_id);
            break;
        }
        case EZLOPI_VALUE_TYPE_BLOCK:
        case EZLOPI_VALUE_TYPE_BLOCKS:
        {
            if (VALUE_TYPE_BLOCK == field_node->field_value.e_type)
            {
                if (____iterate_field_block_to_find_item_id(field_node->field_value.u_value.when_block, item_id))
                {
                    item_id_check = item_id;
                }
            }
            break;
        }
        default:
            break;
        }

        if (item_id_check == item_id)
        {
            ret = true;
            break;
        }

        field_node = field_node->next;
    }

    return ret;
}

static cJSON *__add_scenes_blocks_by_item_ids(e_scenes_block_type_v2_t block_type, l_ezlopi_item_t *item_list)
{
    cJSON *cj_block = NULL;
    while (item_list)
    {
        l_scenes_list_v2_t *scene_node = EZPI_core_scenes_get_scene_head_v2();
        while (scene_node)
        {
            // TRACE_D("Here");

            switch (block_type)
            {
            case SCENE_BLOCK_TYPE_WHEN:
            {
                // TRACE_D("Here");
                if (scene_node->when_block)
                {
                    // TRACE_D("Here");
                    if (__found_item_in_field(scene_node->when_block->fields, item_list->cloud_properties.item_id))
                    {
                        TRACE_D("Here");
                        cj_block = __create_when_block_cjson(scene_node->when_block);
                    }
                    else
                    {
                        TRACE_W("item id not found");
                    }
                }
                break;
            }
            case SCENE_BLOCK_TYPE_THEN:
            {
                if (scene_node->then_block)
                {
                    if (__found_item_in_field(scene_node->then_block->fields, item_list->cloud_properties.item_id))
                    {
                        cj_block = __create_then_block_cjson(scene_node->then_block);
                    }
                }
                break;
            }
            case SCENE_BLOCK_TYPE_ELSE:
            {
                if (scene_node->else_block)
                {
                    if (__found_item_in_field(scene_node->else_block->fields, item_list->cloud_properties.item_id))
                    {
                        cj_block = __create_else_block_cjson(scene_node->else_block);
                    }
                }
                break;
            }
            default:
            {
                break;
            }
            }

            scene_node = scene_node->next;
        }

        item_list = item_list->next;
    }

    return cj_block;
}

static cJSON *__add_scenes_blocks_by_device_ids(e_scenes_block_type_v2_t block_type, cJSON *cj_devices_array)
{
    cJSON *cj_device_id = NULL;
    cJSON *cj_block_array = NULL;

#ifdef CONFIG_EZPI_UTIL_TRACE_EN
    CJSON_TRACE("device array", cj_devices_array);
#endif

    cJSON_ArrayForEach(cj_device_id, cj_devices_array)
    {
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
        TRACE_D("device-id: %s", cj_device_id->valuestring ? cj_device_id->valuestring : ezlopi__str);
#endif

        if (cj_device_id->valuestring)
        {
            uint32_t device_id = strtoul(cj_device_id->valuestring, NULL, 16);
            l_ezlopi_device_t *device_node = EZPI_core_device_get_by_id(device_id);
            if (device_node)
            {
                cJSON *cj_block = __add_scenes_blocks_by_item_ids(block_type, device_node->items);
                if (cj_block)
                {
                    if (NULL == cj_block_array)
                    {
                        cj_block_array = cJSON_CreateArray(__FUNCTION__);
                    }

                    if (cj_block_array)
                    {
                        if (!cJSON_AddItemToArray(cj_block_array, cj_block))
                        {
                            cJSON_Delete(__FUNCTION__, cj_block);
                        }
                    }
                }
            }
            else
            {
                TRACE_OTEL(ENUM_EZLOPI_TRACE_SEVERITY_ERROR, "Device-ID: '%s' not found!", cj_device_id->valuestring);
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
                TRACE_E("Device-id not found in list");
#endif
            }
        }
    }

    return cj_block_array;
}

static e_scenes_block_type_v2_t __get_block_type_and_create_block_array(cJSON *cj_result, char const **block_type_name, cJSON *cj_block_type)
{
    e_scenes_block_type_v2_t block_type = SCENE_BLOCK_TYPE_NONE;

    if (EZPI_STRNCMP_IF_EQUAL(ezlopi_when_str, cj_block_type->valuestring, cj_block_type->str_value_len, 5))
    {
        *block_type_name = ezlopi_when_str;
        block_type = SCENE_BLOCK_TYPE_WHEN;
    }
    else if (EZPI_STRNCMP_IF_EQUAL(ezlopi_then_str, cj_block_type->valuestring, cj_block_type->str_value_len, 5))
    {
        *block_type_name = ezlopi_then_str;
        block_type = SCENE_BLOCK_TYPE_THEN;
    }
    else if (EZPI_STRNCMP_IF_EQUAL(ezlopi_else_str, cj_block_type->valuestring, cj_block_type->str_value_len, 5))
    {
        *block_type_name = ezlopi_else_str;
        block_type = SCENE_BLOCK_TYPE_ELSE;
    }

    return block_type;
}

static int __scenes_block_trigger_device_list(cJSON *cj_devices_array)
{
    int ret = 0;
    if (cj_devices_array)
    {
        char device_id_str[32] = {0};
        bool found_item = false;
        l_ezlopi_device_t *device_node = EZPI_core_device_get_head();
        while (device_node)
        {
            found_item = false;
            l_ezlopi_item_t *item_list = device_node->items;
            while (item_list)
            {
                found_item = false;
                l_scenes_list_v2_t *scene_node = EZPI_core_scenes_get_scene_head_v2();
                while (scene_node)
                {
                    if (scene_node->when_block)
                    {
                        if (__found_item_in_field(scene_node->when_block->fields, item_list->cloud_properties.item_id))
                        {
                            TRACE_S("Item found: %08x [ adding device_id : %08x]", item_list->cloud_properties.item_id, item_list->cloud_properties.device_id);
                            found_item = true;
                            break;
                        }
                    }
                    scene_node = scene_node->next;
                }

                if (found_item) // add the corresponding device_id
                {
                    snprintf(device_id_str, sizeof(device_id_str), "%08x", item_list->cloud_properties.device_id); // device_id corresponding to 'item_id'
                    cJSON *cj_device_id_str = cJSON_CreateString(__FUNCTION__, device_id_str);
                    if (cj_device_id_str)
                    {
                        if (!cJSON_AddItemToArray(cj_devices_array, cj_device_id_str))
                        {
                            cJSON_Delete(__FUNCTION__, cj_device_id_str);
                        }
                        else
                        {
                            ret += 1;
                        }
                    }
                    break;
                }

                item_list = item_list->next;
            }

            device_node = device_node->next;
        }
    }
    return ret;
}

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
