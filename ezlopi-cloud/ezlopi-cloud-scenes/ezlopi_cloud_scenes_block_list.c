#include <string.h>
#include <stdint.h>

#include "ezlopi_util_trace.h"
#include "cjext.h"
#include "ezlopi_cloud_scenes.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_scenes_cjson.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_service_meshbot.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_core_scenes_operators.h"

static cJSON* __create_when_block_cjson(l_when_block_v2_t* when_block);

static bool __found_item_in_field(l_fields_v2_t* field_node, uint32_t item_id);
static cJSON* __add_scenes_blocks_by_device_ids(e_scenes_block_type_v2_t block_type, cJSON* cj_devices_array);
static cJSON* __add_scenes_blocks_by_item_ids(e_scenes_block_type_v2_t block_type, l_ezlopi_item_t* item_list);
static e_scenes_block_type_v2_t __get_block_type_and_create_block_array(cJSON* cj_result, char const** block_type_name, cJSON* cj_block_type);

void scenes_blocks_list(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON* cj_paramas = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_paramas)
        {
            cJSON* cj_block_type = cJSON_GetObjectItem(__FUNCTION__, cj_paramas, ezlopi_blockType_str);
            if (cj_block_type && cj_block_type->valuestring)
            {
                char* block_type_name = NULL;
                e_scenes_block_type_v2_t block_type = __get_block_type_and_create_block_array(cj_result, (const char**)&block_type_name, cj_block_type);

                if (block_type)
                {
                    cJSON* cj_devices_array = cJSON_GetObjectItem(__FUNCTION__, cj_paramas, "devices");
                    if (cj_devices_array && (cJSON_Array == cj_devices_array->type))
                    {
                        cJSON* cj_block_array = __add_scenes_blocks_by_device_ids(block_type, cj_devices_array);
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

static void __add_block_options_and_fields_cjson(cJSON* cj_block, s_block_options_v2_t* block_options, l_fields_v2_t* fields_node)
{
    cJSON* cj_block_opt = cJSON_AddObjectToObject(__FUNCTION__, cj_block, ezlopi_blockOptions_str);
    cJSON* cj_fields = cJSON_AddArrayToObject(__FUNCTION__, cj_block, ezlopi_fields_str);
    if (cj_block_opt && cj_fields)
    {
        cJSON* cj_method = cJSON_AddObjectToObject(__FUNCTION__, cj_block_opt, ezlopi_method_str);
        if (cj_method)
        {
            cJSON_AddStringToObject(__FUNCTION__, cj_method, ezlopi_name_str, block_options->method.name);
            cJSON* cj_args = cJSON_AddObjectToObject(__FUNCTION__, cj_method, ezlopi_args_str);
            if (cj_args)
            {
                while (fields_node)
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_args, fields_node->name, fields_node->name);

                    cJSON* cj_field_obj = ezlopi_scene_cjson_get_field(fields_node);
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

static cJSON* __create_when_block_cjson(l_when_block_v2_t* when_block)
{
    cJSON* cj_when_block = NULL;

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

static cJSON* __create_then_block_cjson(l_action_block_v2_t* then_block)
{
    cJSON* cj_then_block = NULL;

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

static cJSON* __create_else_block_cjson(l_action_block_v2_t* else_block)
{
    cJSON* cj_else_block = NULL;

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

static bool __found_item_in_field(l_fields_v2_t* field_node, uint32_t item_id)
{
    bool ret = false;
    while (field_node)
    {
        if (EZLOPI_VALUE_TYPE_ITEM == field_node->value_type)
        {
            uint32_t item_id_check = strtoul(field_node->field_value.u_value.value_string, NULL, 16);
            TRACE_D("item-id: %s | %08x", field_node->field_value.u_value.value_string, item_id);

            // uint32_t item_id_check = strtoul(field_node->field_value.u_value.value_string, NULL, 16);
            // TRACE_D("item-id: %s | %08x", field_node->field_value.u_value.value_string, item_id);

            if (item_id_check == item_id)
            {
                ret = 1;
                break;
            }
        }

        field_node = field_node->next;
    }

    return ret;
}

static cJSON* __add_scenes_blocks_by_item_ids(e_scenes_block_type_v2_t block_type, l_ezlopi_item_t* item_list)
{
    cJSON* cj_block = NULL;
    while (item_list)
    {
        l_scenes_list_v2_t* scene_node = ezlopi_scenes_get_scenes_head_v2();
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

static cJSON* __add_scenes_blocks_by_device_ids(e_scenes_block_type_v2_t block_type, cJSON* cj_devices_array)
{
    int device_id_idx = 0;
    cJSON* cj_device_id = NULL;
    cJSON* cj_block_array = NULL;

    CJSON_TRACE("device array", cj_devices_array);

    while (NULL != (cj_device_id = cJSON_GetArrayItem(cj_devices_array, device_id_idx++)))
    {
        TRACE_D("device-id: %s", cj_device_id->valuestring ? cj_device_id->valuestring : ezlopi__str);
        if (cj_device_id->valuestring)
        {
            uint32_t device_id = strtoul(cj_device_id->valuestring, NULL, 16);
            l_ezlopi_device_t* device_node = ezlopi_device_get_by_id(device_id);
            if (device_node)
            {
                TRACE_D("Here");
                cJSON* cj_block = __add_scenes_blocks_by_item_ids(block_type, device_node->items);
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
                TRACE_E("Device-id not found in list");
            }
        }
    }

    return cj_block_array;
}

static e_scenes_block_type_v2_t __get_block_type_and_create_block_array(cJSON* cj_result, char const** block_type_name, cJSON* cj_block_type)
{
    e_scenes_block_type_v2_t block_type = SCENE_BLOCK_TYPE_NONE;

    if (0 == strncmp(ezlopi_when_str, cj_block_type->valuestring, 5))
    {
        *block_type_name = ezlopi_when_str;
        block_type = SCENE_BLOCK_TYPE_WHEN;
    }
    else if (0 == strncmp(ezlopi_then_str, cj_block_type->valuestring, 5))
    {
        *block_type_name = ezlopi_then_str;
        block_type = SCENE_BLOCK_TYPE_THEN;
    }
    else if (0 == strncmp(ezlopi_else_str, cj_block_type->valuestring, 5))
    {
        *block_type_name = ezlopi_else_str;
        block_type = SCENE_BLOCK_TYPE_ELSE;
    }

    return block_type;
}
