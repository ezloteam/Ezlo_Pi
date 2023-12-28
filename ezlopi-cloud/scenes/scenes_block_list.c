#include <string.h>
#include <stdint.h>

#include "trace.h"
#include "cJSON.h"
#include "scenes.h"

#include "ezlopi_nvs.h"
#include "ezlopi_devices.h"
#include "ezlopi_scenes_v2.h"
#include "ezlopi_cjson_macros.h"
#include "ezlopi_meshbot_service.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_scenes_operators.h"

static cJSON *__get_device_scene_block_list(e_scenes_block_type_v2_t block_type, cJSON *cj_device_id);
static e_scenes_block_type_v2_t __get_block_type_and_create_block_array(cJSON *cj_result, cJSON **cj_block_array, cJSON *cj_block_type);
static void __add_scenes_blocks_by_device_ids(e_scenes_block_type_v2_t block_type, cJSON *cj_block_array, cJSON *cj_devices_array);

void scenes_blocks_list(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi__id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));

    cJSON *cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_paramas = cJSON_GetObjectItem(cj_request, ezlopi_params_str);
        if (cj_paramas)
        {
            cJSON *cj_block_type = cJSON_GetObjectItem(cj_paramas, ezlopi_blockType_str);
            if (cj_block_type && cj_block_type->valuestring)
            {
                cJSON *cj_block_array = NULL;
                e_scenes_block_type_v2_t block_type = __get_block_type_and_create_block_array(cj_result, &cj_block_array, cj_block_type);

                if (cj_block_array && block_type)
                {
                    cJSON *cj_devices_array = cJSON_GetObjectItem(cj_paramas, "devices");
                    if (cj_devices_array && (cJSON_Array == cj_devices_array->type))
                    {
                        __add_scenes_blocks_by_device_ids(block_type, cj_block_array, cj_devices_array);
                    }
                }
            }
        }
    }
}

static cJSON *__get_device_scene_block_list_by_item_id(e_scenes_block_type_v2_t block_type, uint32_t item_id)
{
    cJSON *cj_block = NULL;
    l_scenes_list_v2_t *scene_node = ezlopi_scenes_get_scenes_head_v2();
    while (scene_node)
    {
        TRACE_D("Here");
        switch (block_type)
        {
        case SCENE_BLOCK_TYPE_WHEN:
        {
            if (scene_node->when_block)
            {
                TRACE_D("Here");

                l_fields_v2_t *field_node = scene_node->when_block->fields;
                while (field_node)
                {
                    TRACE_D("field->name: %s", field_node->name);
                    TRACE_D("field->value_type: %d", field_node->value_type);

                    if (0 == strncmp("item", field_node->name, 5))
                    {
                        TRACE_D("item: %d", field_node->value_type);

                        if (field_node->value.value_double == item_id)
                        {
                        }
                        break;
                    }
                    field_node = field_node->next;
                }
            }
            else
            {
                TRACE_D("Here");
            }

            break;
        }
        case SCENE_BLOCK_TYPE_THEN:
        {
            TRACE_D("Here");
            break;
        }
        case SCENE_BLOCK_TYPE_ELSE:
        {
            TRACE_D("Here");
            break;
        }
        default:
        {

            TRACE_D("Here");
            break;
        }
        }

        scene_node = scene_node->next;
    }

    return cj_block;
}

static void __add_scenes_blocks_by_item_ids(e_scenes_block_type_v2_t block_type, cJSON *cj_block_array, l_ezlopi_item_t *item_list)
{
    while (item_list)
    {
        cJSON *cj_block = __get_device_scene_block_list_by_item_id(block_type, item_list->cloud_properties.item_id);
        if (cj_block)
        {
            TRACE_D("Here");
            if (!cJSON_AddItemToArray(cj_block_array, cj_block))
            {
                cJSON_Delete(cj_block);
            }
        }
        else
        {
            TRACE_D("Here");
        }

        item_list = item_list->next;
    }
}

static void __add_scenes_blocks_by_device_ids(e_scenes_block_type_v2_t block_type, cJSON *cj_block_array, cJSON *cj_devices_array)
{
    int device_id_idx = 0;
    cJSON *cj_device_id = NULL;

    CJSON_TRACE("device array", cj_devices_array);

    while (NULL != (cj_device_id = cJSON_GetArrayItem(cj_devices_array, device_id_idx++)))
    {
        TRACE_D("device-id: %s", cj_device_id->valuestring ? cj_device_id->valuestring : ezlopi__str);
        if (cj_device_id->valuestring)
        {
            uint32_t device_id = strtoul(cj_device_id->valuestring, NULL, 16);
            l_ezlopi_device_t *device_node = ezlopi_device_get_by_id(device_id);
            if (device_node && device_node->items)
            {
                TRACE_D("Here");
                __add_scenes_blocks_by_item_ids(block_type, cj_block_array, device_node->items);
            }
            else
            {
                TRACE_E("Device-id not found in list");
            }
        }
    }
}

static e_scenes_block_type_v2_t __get_block_type_and_create_block_array(cJSON *cj_result, cJSON **cj_block_array, cJSON *cj_block_type)
{
    e_scenes_block_type_v2_t block_type = SCENE_BLOCK_TYPE_NONE;

    if (0 == strncmp(ezlopi_when_str, cj_block_type->valuestring, 5))
    {
        *cj_block_array = cJSON_AddArrayToObject(cj_result, ezlopi_when_str);
        block_type = SCENE_BLOCK_TYPE_WHEN;
    }
    else if (0 == strncmp(ezlopi_then_str, cj_block_type->valuestring, 5))
    {
        *cj_block_array = cJSON_AddArrayToObject(cj_result, ezlopi_then_str);
        block_type = SCENE_BLOCK_TYPE_THEN;
    }
    else if (0 == strncmp(ezlopi_else_str, cj_block_type->valuestring, 5))
    {
        *cj_block_array = cJSON_AddArrayToObject(cj_result, ezlopi_else_str);
        block_type = SCENE_BLOCK_TYPE_ELSE;
    }

    return block_type;
}
