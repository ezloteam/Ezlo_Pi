#include <string.h>
#include <stdint.h>

#include "scenes.h"
#include "trace.h"
#include "cJSON.h"
#include "ezlopi_nvs.h"
#include "ezlopi_scenes_v2.h"
#include "ezlopi_devices.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_scenes_operators.h"
#include "ezlopi_meshbot_service.h"

typedef struct s_data_source_n_target_object
{
    char *types;
    char *field;
} s_data_source_n_target_object_t;

static void __value_types_list(char *list_name, cJSON *cj_result);
static void __scalable_value_types_list(char *list_name, cJSON *cj_result);
static void __value_scales_list(char *list_name, cJSON *cj_result);
static void __scenes_value_types_list(char *list_name, cJSON *cj_result);
static void __value_types_families_list(char *list_name, cJSON *cj_result);
static void __comparison_operators_list(char *list_name, cJSON *cj_result);
static void __comparison_methods_list(char *list_name, cJSON *cj_result);
static void __action_methods_list(char *list_name, cJSON *cj_result);
static void __advanced_scenes_version_list(char *list_name, cJSON *cj_result);

static void __add_data_src_dest_array_to_object(cJSON *cj_method, char *array_name, s_data_source_n_target_object_t *data_list);

void scenes_list(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON *cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_scene_array = cJSON_AddArrayToObject(cj_result, "scenes");
        ezlopi_scenes_get_list_v2(cj_scene_array);
    }
}

void scenes_create(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));

    cJSON *cj_params = cJSON_GetObjectItem(cj_request, ezlopi_params_str);
    if (cj_params)
    {
        uint32_t new_scene_id = ezlopi_store_new_scene_v2(cj_params);
        if (new_scene_id)
        {
            ezlopi_scenes_new_scene_populate(cj_params, new_scene_id);
        }
    }
}

void scenes_get(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));

    cJSON *cj_params = cJSON_GetObjectItem(cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON *cj_ids = cJSON_GetObjectItem(cj_params, "_id");
        if (cj_ids && cj_ids->valuestring)
        {
            char *scene_str = ezlopi_nvs_read_str(cj_ids->valuestring);

            if (scene_str)
            {
                cJSON *cj_scene = cJSON_Parse(scene_str);
                if (cj_scene)
                {
                    if (!cJSON_AddItemToObject(cj_response, "result", cj_scene))
                    {
                        cJSON_Delete(cj_scene);
                    }
                }

                free(scene_str);
            }
        }
    }
}

void scenes_edit(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));

    cJSON *cj_params = cJSON_GetObjectItem(cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON *cj_id = cJSON_GetObjectItem(cj_params, "_id");
        if (cj_id && cj_id->valuestring)
        {
            uint32_t u_id = strtoul(cj_id->valuestring, NULL, 16);
            cJSON *cj_eo = cJSON_GetObjectItem(cj_params, "eo");
            if (cj_eo)
            {
                // ezlopi_scenes_update_by_id(u_id, cj_eo);
            }
        }
    }
}

// void scenes_edit(cJSON *cj_request, cJSON *cj_response)
// {
//     cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
//     cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
//
//     cJSON *cj_params = cJSON_GetObjectItem(cj_request, ezlopi_params_str);
//     if (cj_params)
//     {
//         cJSON *cj_id = cJSON_GetObjectItem(cj_params, "_id");
//         if (cj_id && cj_id->valuestring)
//         {
//             uint32_t u_id = strtoul(cj_id->valuestring, NULL, 16);
//             cJSON *cj_eo = cJSON_GetObjectItem(cj_params, "eo");
//             if (cj_eo)
//             {
//                 ezlopi_scenes_update_by_id(u_id, cj_eo);
//             }
//         }
//     }
// }

void scenes_delete(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON_AddObjectToObject(cj_response, ezlopi_result_str);

    cJSON *cj_params = cJSON_GetObjectItem(cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON *cj_id = cJSON_GetObjectItem(cj_params, "_id");
        if (cj_id && cj_id->valuestring)
        {
            uint32_t u_id = strtoul(cj_id->valuestring, NULL, 16);
            ezlopi_nvs_delete_stored_script(u_id);
            ezlopi_scenes_depopulate_by_id_v2(u_id);
            ezlopi_scenes_remove_id_from_list_v2(u_id);
        }
    }
}

void scenes_status_get(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON_AddObjectToObject(cj_response, ezlopi_result_str);

    cJSON *cj_params = cJSON_GetObjectItem(cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON *cj_scene_id = cJSON_GetObjectItem(cj_params, "sceneId");
        if (cj_scene_id && cj_scene_id->valuestring)
        {
            uint32_t u_id = strtoul(cj_scene_id->valuestring, NULL, 16);
            l_scenes_list_v2_t *scene_node = ezlopi_scenes_get_by_id_v2(u_id);
            if (scene_node)
            {
                // scene_node->status;
            }
        }
    }
}

void scenes_run(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON *cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);

    cJSON *cj_params = cJSON_GetObjectItem(cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON *cj_scene_id = cJSON_GetObjectItem(cj_params, "sceneId");
        if (cj_scene_id && cj_scene_id->valuestring)
        {
            uint32_t u32_scene_id = strtoul(cj_scene_id->valuestring, NULL, 16);
            ezlopi_scenes_service_run_by_id(u32_scene_id);
        }
    }
}

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
            cJSON *cj_block_type = cJSON_GetObjectItem(cj_paramas, "blockType");
            if (cj_block_type && cj_block_type->valuestring)
            {
                cJSON *cj_block_array = NULL;
                e_scenes_block_type_v2_t block_type = SCENE_BLOCK_TYPE_NONE;
                if (0 == strncmp("when", cj_block_type->valuestring, 4))
                {
                    cj_block_array = cJSON_AddArrayToObject(cj_result, "when");
                    block_type = SCENE_BLOCK_TYPE_WHEN;
                }
                else if (0 == strncmp("then", cj_block_type->valuestring, 4))
                {
                    cj_block_array = cJSON_AddArrayToObject(cj_result, "then");
                    block_type = SCENE_BLOCK_TYPE_THEN;
                }

                if (cj_block_array && block_type)
                {
                    cJSON *cj_devices_array = cJSON_GetObjectItem(cj_paramas, "devices");
                    if (cj_devices_array && (cJSON_Array == cj_devices_array->type))
                    {
                        int device_id_idx = 0;
                        cJSON *cj_device_id = NULL;
                        while (NULL != (cj_device_id = cJSON_GetArrayItem(cj_devices_array, device_id_idx++)))
                        {
                            uint32_t device_id = strtoul(cj_device_id->valuestring, NULL, 16);
                            l_scenes_list_v2_t *req_scene = ezlopi_scenes_get_by_id_v2(device_id);

                            if (req_scene)
                            {
                                switch (block_type)
                                {
                                case SCENE_BLOCK_TYPE_WHEN:
                                {
                                    l_when_block_v2_t *curr_when_block = req_scene->when_block;
                                    while (curr_when_block)
                                    {
                                        cJSON *cj_when_block = NULL;
                                        // ezlopi_scenes_cjson_create_when_block(req_scene->when_block);
                                        if (cj_when_block)
                                        {
                                            if (!cJSON_AddItemToArray(cj_block_array, cj_when_block))
                                            {
                                                cJSON_Delete(cj_when_block);
                                            }
                                        }
                                        curr_when_block = curr_when_block->next;
                                    }

                                    break;
                                }
#warning "Check if else block is required or not"
                                case SCENE_BLOCK_TYPE_THEN:
                                {
                                    l_action_block_v2_t *curr_then_block = req_scene->then_block;
                                    while (curr_then_block)
                                    {
                                        cJSON *cj_then_block = NULL;
                                        // ezlopi_scenes_cjson_create_then_block(req_scene->then_block);
                                        if (cj_then_block)
                                        {
                                            if (!cJSON_AddItemToArray(cj_block_array, cj_then_block))
                                            {
                                                cJSON_Delete(cj_then_block);
                                            }
                                        }
                                        curr_then_block = curr_then_block->next;
                                    }
                                    break;
                                }
                                default:
                                {
                                    break;
                                }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void scenes_block_data_list(cJSON *cj_request, cJSON *cj_response)
{
    typedef struct s_block_data_list_collector
    {
        char *key_string;
        void (*func)(char *list_name, cJSON *result);
    } s_block_data_list_collector_t;

    static const s_block_data_list_collector_t block_data_list_collector[] = {
        {.key_string = "valueTypes", .func = __value_types_list},
        {.key_string = "scalableValueTypes", .func = __scalable_value_types_list},
        {.key_string = "valueScales", .func = __value_scales_list},
        {.key_string = "scenesValueTypes", .func = __scenes_value_types_list},
        {.key_string = "valueTypeFamilies", .func = __value_types_families_list},
        {.key_string = "comparisonOperators", .func = __comparison_operators_list},
        {.key_string = "comparisonMethods", .func = __comparison_methods_list},
        {.key_string = "actionMethods", .func = __action_methods_list},
        {.key_string = "advancedScenesVersion", .func = __advanced_scenes_version_list},
        {.key_string = NULL, .func = NULL},
    };

    if (cj_request && cj_response)
    {
        cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
        cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));

        cJSON *cj_params = cJSON_GetObjectItem(cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON *cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
            if (cj_result)
            {
                uint32_t idx = 0;
                while (block_data_list_collector[idx].func)
                {
                    cJSON *cj_value = cJSON_GetObjectItem(cj_params, block_data_list_collector[idx].key_string);
                    if (cj_value && (cj_value->type == cJSON_True))
                    {
                        block_data_list_collector[idx].func(block_data_list_collector[idx].key_string, cj_result);
                    }

                    idx++;
                }
            }
        }
    }
}

static void __value_types_list(char *list_name, cJSON *cj_result)
{
    if (cj_result && list_name)
    {
        cJSON *cj_value_types = cJSON_AddObjectToObject(cj_result, list_name);
        if (cj_value_types)
        {
            cJSON *cj_value_array = cJSON_AddArrayToObject(cj_value_types, "list");
            if (cj_value_array)
            {
                l_ezlopi_device_t *devices = ezlopi_device_get_head();
                while (devices)
                {
                    l_ezlopi_item_t *items = devices->items;
                    while (items)
                    {
                        if (NULL == items->cloud_properties.scale)
                        {
                            cJSON *cj_item_value_type = cJSON_CreateString(items->cloud_properties.value_type);
                            if (cj_item_value_type)
                            {
                                if (!cJSON_AddItemToArray(cj_value_array, cj_item_value_type))
                                {
                                    cJSON_Delete(cj_item_value_type);
                                }
                            }
                        }
                        items = items->next;
                    }
                    devices = devices->next;
                }
            }
        }
    }
}

static void __scalable_value_types_list(char *list_name, cJSON *cj_result)
{
    if (cj_result && list_name)
    {
        cJSON *cj_value_types = cJSON_AddObjectToObject(cj_result, list_name);
        if (cj_value_types)
        {
            cJSON *cj_value_array = cJSON_AddArrayToObject(cj_value_types, "list");
            if (cj_value_array)
            {
                l_ezlopi_device_t *devices = ezlopi_device_get_head();
                while (devices)
                {
                    l_ezlopi_item_t *items = devices->items;
                    while (items)
                    {
                        if (items->cloud_properties.scale)
                        {
                            cJSON *cj_item_value_type = cJSON_CreateString(items->cloud_properties.value_type);
                            if (cj_item_value_type)
                            {
                                if (!cJSON_AddItemToArray(cj_value_array, cj_item_value_type))
                                {
                                    cJSON_Delete(cj_item_value_type);
                                }
                            }
                        }
                        items = items->next;
                    }
                    devices = devices->next;
                }
            }
        }
    }
}

static void __value_scales_list(char *list_name, cJSON *cj_result)
{
    if (cj_result && list_name)
    {
        cJSON *cj_value_scales = cJSON_AddObjectToObject(cj_result, list_name);
        if (cj_value_scales)
        {
            l_ezlopi_device_t *devices = ezlopi_device_get_head();
            while (devices)
            {
                l_ezlopi_item_t *items = devices->items;
                while (items)
                {
                    if (items->cloud_properties.scale)
                    {
                        cJSON *cj_scale_array = NULL;
                        cJSON *cj_value_type = cJSON_GetObjectItem(cj_value_scales, items->cloud_properties.value_type);
                        if (NULL == cj_value_type)
                        {
                            cj_value_type = cJSON_AddObjectToObject(cj_value_scales, items->cloud_properties.value_type);
                            if (cj_value_type)
                            {
                                cj_scale_array = cJSON_AddArrayToObject(cj_value_type, "scales");
                                cJSON_AddFalseToObject(cj_value_type, "converter");
                            }
                        }
                        else
                        {
                            cj_scale_array = cJSON_GetObjectItem(cj_value_type, "scales");
                        }

                        if (cj_scale_array)
                        {
                            cJSON *cj_scale = cJSON_CreateString(items->cloud_properties.scale);
                            if (cj_scale)
                            {
                                if (!cJSON_AddItemToArray(cj_scale_array, cj_scale))
                                {
                                    cJSON_Delete(cj_scale);
                                }
                            }
                        }
                    }
                    items = items->next;
                }
                devices = devices->next;
            }
        }
    }
}

static void __scenes_value_types_list(char *list_name, cJSON *cj_result)
{
    if (cj_result && list_name)
    {
        cJSON *cj_scenes_value_types = cJSON_AddObjectToObject(cj_result, list_name);
        if (cj_scenes_value_types)
        {
            cJSON *cj_value_array = cJSON_AddArrayToObject(cj_scenes_value_types, "list");
            if (cj_value_array)
            {
                uint32_t idx = EZLOPI_VALUE_TYPE_NONE + 1;
                while (ezlopi_scene_get_scene_value_type_name_v2(idx))
                {
                    cJSON *cj_string_val = cJSON_CreateString(ezlopi_scene_get_scene_value_type_name_v2(idx));
                    if (cj_string_val)
                    {
                        if (!cJSON_AddItemToArray(cj_value_array, cj_string_val))
                        {
                            cJSON_Delete(cj_string_val);
                        }
                    }
                    idx++;
                }
            }
        }
    }
}

static void __value_types_families_list(char *list_name, cJSON *cj_result)
{
    if (cj_result && list_name)
    {
        cJSON *cj_value_type_famiies = cJSON_AddObjectToObject(cj_result, list_name);
        if (cj_value_type_famiies)
        {
            const static char *numeric = "[\"int\",\"float\",\"scalableValueTypes\"]";
            const static char *strings = "[\"string\",\"token\"]";
            const static char *value_with_less = "[\"int\",\"float\",\"scalableValueTypes\",\"string\"]";
            const static char *value_without_less = "[]"; // remained to fill

            cJSON_AddRawToObject(cj_value_type_famiies, "numeric", numeric);
            cJSON_AddRawToObject(cj_value_type_famiies, "strings", strings);
            cJSON_AddRawToObject(cj_value_type_famiies, "valuesWithLess", value_with_less);
            cJSON_AddRawToObject(cj_value_type_famiies, "valuesWithoutLess", value_without_less);
        }
    }
}

static cJSON *__comparision_operators_numeric(void)
{
    cJSON *cj_family = cJSON_CreateObject();
    if (cj_family)
    {
        cJSON_AddStringToObject(cj_family, "family", "numeric");
        cJSON *cj_methods_array = cJSON_AddArrayToObject(cj_family, "methods");
        if (cj_methods_array)
        {
            e_scene_num_cmp_operators_t op_idx = SCENES_NUM_COMP_OPERATORS_NONE + 1;
            while (ezlopi_scenes_numeric_comparator_operators_get_op(op_idx))
            {
                cJSON *cj_method = cJSON_CreateObject();
                if (cj_method)
                {
                    cJSON_AddStringToObject(cj_method, "op", ezlopi_scenes_numeric_comparator_operators_get_op(op_idx));
                    cJSON_AddStringToObject(cj_method, "name", ezlopi_scenes_numeric_comparator_operators_get_name(op_idx));
                    cJSON_AddStringToObject(cj_method, "method", ezlopi_scenes_numeric_comparator_operators_get_method(op_idx));

                    if (!cJSON_AddItemToArray(cj_methods_array, cj_method))
                    {
                        cJSON_Delete(cj_method);
                        cj_method = NULL;
                    }
                }

                op_idx++;
            }
        }
    }

    return cj_family;
}

static cJSON *__comparision_operators_strings(void)
{
    cJSON *cj_family = cJSON_CreateObject();
    if (cj_family)
    {
        cJSON_AddStringToObject(cj_family, "family", "strings");
        cJSON *cj_methods_array = cJSON_AddArrayToObject(cj_family, "methods");
        if (cj_methods_array)
        {
            e_scene_str_cmp_operators_t op_idx = SCENES_STRINGS_OPERATORS_NONE + 1;
            while (ezlopi_scenes_strings_comparator_operators_get_op(op_idx))
            {
                cJSON *cj_method = cJSON_CreateObject();
                if (cj_method)
                {
                    cJSON_AddStringToObject(cj_method, "op", ezlopi_scenes_strings_comparator_operators_get_op(op_idx));
                    cJSON_AddStringToObject(cj_method, "name", ezlopi_scenes_strings_comparator_operators_get_name(op_idx));
                    cJSON_AddStringToObject(cj_method, "method", ezlopi_scenes_strings_comparator_operators_get_method(op_idx));

                    if (!cJSON_AddItemToArray(cj_methods_array, cj_method))
                    {
                        cJSON_Delete(cj_method);
                        cj_method = NULL;
                    }
                }

                op_idx++;
            }
        }
    }

    return cj_family;
}

static cJSON *__comparision_operators_values_with_less(void)
{
    cJSON *cj_family = cJSON_CreateObject();
    if (cj_family)
    {
        cJSON_AddStringToObject(cj_family, "family", "valuesWithLess");
        cJSON *cj_methods_array = cJSON_AddArrayToObject(cj_family, "methods");
        if (cj_methods_array)
        {
            e_scene_value_with_less_cmp_operators_t op_idx = SCENES_VALUES_WITH_LESS_OPERATORS_NONE + 1;
            while (ezlopi_scenes_value_with_less_comparator_operators_get_op(op_idx))
            {
                cJSON *cj_method = cJSON_CreateObject();
                if (cj_method)
                {
                    cJSON_AddStringToObject(cj_method, "op", ezlopi_scenes_value_with_less_comparator_operators_get_op(op_idx));
                    cJSON_AddStringToObject(cj_method, "name", ezlopi_scenes_value_with_less_comparator_operators_get_name(op_idx));
                    cJSON_AddStringToObject(cj_method, "method", ezlopi_scenes_value_with_less_comparator_operators_get_method(op_idx));

                    if (!cJSON_AddItemToArray(cj_methods_array, cj_method))
                    {
                        cJSON_Delete(cj_method);
                        cj_method = NULL;
                    }
                }

                op_idx++;
            }
        }
    }

    return cj_family;
}

static cJSON *__comparision_operators_values_without_less(void)
{
    cJSON *cj_family = cJSON_CreateObject();
    if (cj_family)
    {
        cJSON_AddStringToObject(cj_family, "family", "valuesWithoutLess");
        cJSON *cj_methods_array = cJSON_AddArrayToObject(cj_family, "methods");
        if (cj_methods_array)
        {
            e_scene_value_without_less_cmp_operators_t op_idx = SCENES_VALUES_WITHOUT_LESS_OPERATORS_NONE + 1;
            while (ezlopi_scenes_value_without_less_comparator_operators_get_op(op_idx))
            {
                cJSON *cj_method = cJSON_CreateObject();
                if (cj_method)
                {
                    cJSON_AddStringToObject(cj_method, "op", ezlopi_scenes_value_without_less_comparator_operators_get_op(op_idx));
                    cJSON_AddStringToObject(cj_method, "name", ezlopi_scenes_value_without_less_comparator_operators_get_name(op_idx));
                    cJSON_AddStringToObject(cj_method, "method", ezlopi_scenes_value_without_less_comparator_operators_get_method(op_idx));

                    if (!cJSON_AddItemToArray(cj_methods_array, cj_method))
                    {
                        cJSON_Delete(cj_method);
                        cj_method = NULL;
                    }
                }

                op_idx++;
            }
        }
    }

    return cj_family;
}

static void __comparison_operators_list(char *list_name, cJSON *cj_result)
{
    if (cj_result)
    {
        cJSON *cj_value_types = cJSON_AddObjectToObject(cj_result, list_name);
        if (cj_value_types)
        {
            cJSON *cj_families_array = cJSON_AddArrayToObject(cj_value_types, "families");
            if (cj_families_array)
            {
                static cJSON *(*com_operators_funcs[])(void) = {
                    __comparision_operators_numeric,
                    __comparision_operators_strings,
                    __comparision_operators_values_with_less,
                    __comparision_operators_values_without_less,
                    NULL,
                };

                uint32_t family_idx = 0;
                while (com_operators_funcs[family_idx])
                {
                    cJSON *cj_family = com_operators_funcs[family_idx]();
                    if (cj_family)
                    {
                        if (!cJSON_AddItemToArray(cj_families_array, cj_family))
                        {
                            cJSON_Delete(cj_family);
                        }
                    }

                    family_idx++;
                }
            }
        }
    }
}

static cJSON *__comparision_method_info(void)
{
    cJSON *cj_info = cJSON_CreateObject();
    if (cj_info)
    {
        cJSON_AddStringToObject(cj_info, "version", "1.0.0");
    }

    return cj_info;
}

static cJSON *__comparision_method_compare_number_range(void)
{
    cJSON *cj_compare_number_range = cJSON_CreateObject();
    if (cj_compare_number_range)
    {
        cJSON *cj_comparator = cJSON_AddObjectToObject(cj_compare_number_range, "comparator");
        if (cj_comparator)
        {
            cJSON_AddStringToObject(cj_comparator, "family", "numeric");
            cJSON_AddStringToObject(cj_comparator, "field", "comparator");
            cJSON_AddStringToObject(cj_comparator, "type", "enum");

            const char *options_str = "[\"between\", \"not_between\"]";
            cJSON_AddRawToObject(cj_comparator, "options", options_str);
        }

        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"item\",\"expression\",\"device_group\",\"item_group\"]", .field = NULL},
            {.types = "[\"constant\"]", .field = "startValue"},
            {.types = "[\"constant\"]", .field = "endValue"},
            {.types = NULL, .field = NULL},
        };

        __add_data_src_dest_array_to_object(cj_compare_number_range, "dataSource", data_src_obj);
    }

    return cj_compare_number_range;
}

static cJSON *__comparision_method_compare_numbers(void)
{
    cJSON *cj_compare_numbers = cJSON_CreateObject();
    if (cj_compare_numbers)
    {

        cJSON *cj_comparator = cJSON_AddObjectToObject(cj_compare_numbers, "comparator");
        if (cj_comparator)
        {
            cJSON_AddStringToObject(cj_comparator, "family", "numeric");
            cJSON_AddStringToObject(cj_comparator, "field", "comparator");
            cJSON_AddStringToObject(cj_comparator, "type", "enum");

            const static char *options_str = "[\"<\",\">\",\"<=\",\">=\",\"==\",\"!=\"]";
            cJSON_AddRawToObject(cj_comparator, "options", options_str);
        }

        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"item\",\"expression\"]", .field = NULL},
            {.types = "[\"constant\",\"expression\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_compare_numbers, "dataSource", data_src_obj);
    }

    return cj_compare_numbers;
}

static cJSON *__comparision_method_compare_strings(void)
{
    cJSON *cj_compare_strings = cJSON_CreateObject();
    if (cj_compare_strings)
    {
        cJSON *cj_comparator = cJSON_AddObjectToObject(cj_compare_strings, "comparator");
        if (cj_comparator)
        {
            cJSON_AddStringToObject(cj_comparator, "family", "strings");
            cJSON_AddStringToObject(cj_comparator, "field", "comparator");
            cJSON_AddStringToObject(cj_comparator, "type", "enum");

            const static char *options_str = "[\"<\",\">\",\"<=\",\">=\",\"==\",\"!=\"]";
            cJSON_AddRawToObject(cj_comparator, "options", options_str);
        }

        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"item\",\"expression\",\"device_group\",\"item_group\"]", .field = NULL},
            {.types = "[\"constant\",\"expression\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_compare_strings, "dataSource", data_src_obj);
    }

    return cj_compare_strings;
}

static cJSON *__comparision_method_compare_values(void)
{
    cJSON *cj_compare_values = cJSON_CreateObject();
    if (cj_compare_values)
    {
        cJSON *cj_comparator = cJSON_AddObjectToObject(cj_compare_values, "comparator");
        if (cj_comparator)
        {
            cJSON_AddStringToObject(cj_comparator, "family", "valuesWithoutLess");
            cJSON_AddStringToObject(cj_comparator, "field", "comparator");
            cJSON_AddStringToObject(cj_comparator, "type", "enum");

            const static char *options_str = "[\"==\",\"!=\"]";
            cJSON_AddRawToObject(cj_comparator, "options", options_str);
        }

        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"item\",\"expression\",\"device_group\",\"item_group\"]", .field = NULL},
            {.types = "[\"constant\",\"expression\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_compare_values, "dataSource", data_src_obj);
    }

    return cj_compare_values;
}

static cJSON *__comparision_method_in_array(void)
{
    cJSON *cj_in_array = cJSON_CreateObject();
    if (cj_in_array)
    {
        cJSON *cj_comparator = cJSON_AddObjectToObject(cj_in_array, "comparator");
        if (cj_comparator)
        {
            cJSON_AddStringToObject(cj_comparator, "family", "array");
            cJSON_AddStringToObject(cj_comparator, "field", "comparator");
            cJSON_AddStringToObject(cj_comparator, "type", "enum");

            const static char *options_str = "[\"in\",\"not_in\"]";
            cJSON_AddRawToObject(cj_comparator, "options", options_str);
        }

        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"item\",\"expression\"]", .field = NULL},
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_in_array, "dataSource", data_src_obj);
    }

    return cj_in_array;
}

static cJSON *__comparision_method_is_device_item_group(void)
{
    cJSON *cj_is_device_item_grp = cJSON_CreateObject();
    if (cj_is_device_item_grp)
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"device_group\"]", .field = NULL},
            {.types = "[\"item_group\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };

        __add_data_src_dest_array_to_object(cj_is_device_item_grp, "dataSource", data_src_obj);
    }

    return cj_is_device_item_grp;
}

static cJSON *__comparision_method_is_device_state(void)
{
    cJSON *cj_is_device_state = cJSON_CreateObject();
    if (cj_is_device_state)
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"device\",\"device_group\"]", .field = NULL},
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };

        __add_data_src_dest_array_to_object(cj_is_device_state, "dataSource", data_src_obj);
    }

    return cj_is_device_state;
}

static cJSON *__comparision_method_is_item_state(void)
{
    cJSON *cj_is_item_state = cJSON_CreateObject();
    if (cj_is_item_state)
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"device_item\",\"item\",\"item_group\",\"device_group\"]", .field = NULL},
            {.types = "[\"constant\",\"expression\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };

        __add_data_src_dest_array_to_object(cj_is_item_state, "dataSource", data_src_obj);
    }

    return cj_is_item_state;
}

static cJSON *__comparision_method_is_item_state_changed(void)
{
    cJSON *cj_is_item_state_changed = cJSON_CreateObject();
    if (cj_is_item_state_changed)
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"item\",\"expression\"]", .field = NULL},
            {.types = "[\"constant\",\"expression\"]", .field = "start"},
            {.types = "[\"constant\",\"expression\"]", .field = "finish"},
            {.types = NULL, .field = NULL},
        };

        __add_data_src_dest_array_to_object(cj_is_item_state_changed, "dataSource", data_src_obj);
    }

    return cj_is_item_state_changed;
}

static cJSON *__comparision_method_string_operation(void)
{
    cJSON *cj_string_operation = cJSON_CreateObject();
    if (cj_string_operation)
    {
        cJSON *cj_comparator = cJSON_AddObjectToObject(cj_string_operation, "comparator");
        if (cj_comparator)
        {
            cJSON_AddStringToObject(cj_comparator, "family", "strings");
            cJSON_AddStringToObject(cj_comparator, "field", "comparator");
            cJSON_AddStringToObject(cj_comparator, "type", "enum");

            const static char *options_str = "[\"begin\",\"end\",\"contain\",\"length\",\"not_begin\",\"not_end\",\"not_contain\",\"not_length\"]";
            cJSON_AddRawToObject(cj_comparator, "options", options_str);
        }

        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"item\",\"expression\",\"device_group\",\"item_group\"]", .field = NULL},
            {.types = "[\"constant\",\"expression\"]", .field = "start"},
            {.types = NULL, .field = NULL},
        };

        __add_data_src_dest_array_to_object(cj_string_operation, "dataSource", data_src_obj);
    }

    return cj_string_operation;
}

static void __comparison_methods_list(char *list_name, cJSON *cj_result)
{
    if (cj_result)
    {
        cJSON *cj_comparision_methods = cJSON_AddObjectToObject(cj_result, list_name);
        if (cj_comparision_methods)
        {
            cJSON_AddItemToObject(cj_comparision_methods, "info", __comparision_method_info());
            cJSON_AddItemToObject(cj_comparision_methods, "compareNumberRange", __comparision_method_compare_number_range());
            cJSON_AddItemToObject(cj_comparision_methods, "compareNumbers", __comparision_method_compare_numbers());
            cJSON_AddItemToObject(cj_comparision_methods, "compareStrings", __comparision_method_compare_strings());
            cJSON_AddItemToObject(cj_comparision_methods, "compareValues", __comparision_method_compare_values());
            cJSON_AddItemToObject(cj_comparision_methods, "inArray", __comparision_method_in_array());
            cJSON_AddItemToObject(cj_comparision_methods, "isDeviceItemGroup", __comparision_method_is_device_item_group());
            cJSON_AddItemToObject(cj_comparision_methods, "isDeviceState", __comparision_method_is_device_state());
            cJSON_AddItemToObject(cj_comparision_methods, "isItemState", __comparision_method_is_item_state());
            cJSON_AddItemToObject(cj_comparision_methods, "isItemStateChanged", __comparision_method_is_item_state_changed());
            cJSON_AddItemToObject(cj_comparision_methods, "stringOperation", __comparision_method_string_operation());
        }
    }
}

static void __add_data_src_dest_array_to_object(cJSON *cj_method, char *array_name, s_data_source_n_target_object_t *data_list)
{
    cJSON *cj_data_source_list = cJSON_AddArrayToObject(cj_method, array_name);
    if (cj_data_source_list)
    {
        uint32_t idx = 0;
        while (data_list[idx].types || data_list[idx].field)
        {
            cJSON_AddNumberToObject(cj_data_source_list, "index", idx);
            if (data_list[idx].types)
            {
                cJSON_AddRawToObject(cj_data_source_list, "types", data_list[idx].types);
            }
            if (data_list[idx].field)
            {
                cJSON_AddRawToObject(cj_data_source_list, "field", data_list[idx].field);
            }
            idx++;
        }
    }
}

static cJSON *__action_methods_info(void)
{
    cJSON *cj_method = cJSON_CreateObject();
    if (cj_method) // Info
    {
        cJSON_AddStringToObject(cj_method, "schema_version", "0.0.1");
    }
    return cj_method;
}
static cJSON *__action_methods_set_item_value()
{
    cJSON *cj_method = cJSON_CreateObject();
    if (cj_method) // setItemValue
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_method, "dataSource", data_src_obj);
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"item\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"async\"]";
        cJSON_AddRawToObject(cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------

        cJSON_AddStringToObject(cj_method, "typeSystem", "itemValueTypes");
        //-------------------------------------------------------------------------

        static const char *scope_raw_str = "[\"local\",\"global\"]";
        cJSON_AddRawToObject(cj_method, "scope", execution_raw_str);
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *__action_methods_set_device_armed()
{
    cJSON *cj_method = cJSON_CreateObject();
    if (cj_method) // setDeviceArmed
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_method, "dataSource", data_src_obj);
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"device\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"async\"]";
        cJSON_AddRawToObject(cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------

        static const char *scope_raw_str = "[\"local\",\"global\"]";
        cJSON_AddRawToObject(cj_method, "scope", execution_raw_str);
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *__action_methods_send_cloud_abstract_command()
{
    cJSON *cj_method = cJSON_CreateObject();
    if (cj_method) // sendCloudAbstractCommand
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"object\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_method, "dataSource", data_src_obj);
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"cloud\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"sync\"]";
        cJSON_AddRawToObject(cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *__action_methods_switch_house_mode()
{
    cJSON *cj_method = cJSON_CreateObject();
    if (cj_method) // switchHouseMode
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_method, "dataSource", data_src_obj);
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"houseMode\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"async\"]";
        cJSON_AddRawToObject(cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *__action_methods_send_http_request()
{
    cJSON *cj_method = cJSON_CreateObject();
    if (cj_method) // sendHttpRequest
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_method, "dataSource", data_src_obj);
        //-------------------------------------------------------------------------

        cJSON_AddNullToObject(cj_method, "dataTarget");
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"async\"]";
        cJSON_AddRawToObject(cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t dataTarget_sideEffects_obj[] = {
            {.types = "[\"expression\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        cJSON *cj_side_effects = cJSON_AddArrayToObject(cj_method, "sideEffects");
        if (cj_side_effects)
        {
            cJSON *cj_side_eff_elem = cJSON_CreateObject();
            if (cj_side_eff_elem)
            {
                cJSON_AddStringToObject(cj_side_eff_elem, "action", "saveResult");
                __add_data_src_dest_array_to_object(cj_side_eff_elem, "dataTarget", dataTarget_sideEffects_obj);

                if (!cJSON_AddItemToArray(cj_side_effects, cj_side_eff_elem))
                {
                    cJSON_Delete(cj_side_eff_elem);
                }
            }
        }
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *__action_methods_run_custom_script()
{
    cJSON *cj_method = cJSON_CreateObject();
    if (cj_method) // runCustomScript
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_method, "dataSource", data_src_obj);
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"async\"]";
        cJSON_AddRawToObject(cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *__action_methods_run_plugin_script()
{
    cJSON *cj_method = cJSON_CreateObject();
    if (cj_method) // runPluginScript
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_method, "dataSource", data_src_obj);
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"script\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"sync\"]";
        cJSON_AddRawToObject(cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *__action_methods_run_scene()
{
    cJSON *cj_method = cJSON_CreateObject();
    if (cj_method) // runScene
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_method, "dataSource", data_src_obj);
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"scene\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"async\"]";
        cJSON_AddRawToObject(cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *__action_methods_stop_scene()
{
    cJSON *cj_method = cJSON_CreateObject();
    if (cj_method) // stopScene
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_method, "dataSource", data_src_obj);
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"scene\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"async\"]";
        cJSON_AddRawToObject(cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *__action_methods_set_scene_state()
{
    cJSON *cj_method = cJSON_CreateObject();
    if (cj_method) // setSceneState
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_method, "dataSource", data_src_obj);
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"scene\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"async\"]";
        cJSON_AddRawToObject(cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *__action_methods_reboot_hub()
{
    cJSON *cj_method = cJSON_CreateObject();
    if (cj_method) // rebootHub
    {
        cJSON_AddNullToObject(cj_method, "dataSource");
        //-------------------------------------------------------------------------
        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------
        static const char *execution_raw_str = "[\"async\"]";
        cJSON_AddRawToObject(cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *__action_methods_cloud_api()
{
    cJSON *cj_method = cJSON_CreateObject();
    if (cj_method) // cloudAPI
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_method, "dataSource", data_src_obj);
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"async\"]";
        cJSON_AddRawToObject(cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t dataTarget_sideEffects_obj[] = {
            {.types = "[\"expression\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        cJSON *cj_side_effects = cJSON_AddArrayToObject(cj_method, "sideEffects");
        if (cj_side_effects)
        {
            cJSON *cj_side_eff_elem = cJSON_CreateObject();
            if (cj_side_eff_elem)
            {
                cJSON_AddStringToObject(cj_side_eff_elem, "action", "saveResult");
                __add_data_src_dest_array_to_object(cj_side_eff_elem, "dataTarget", dataTarget_sideEffects_obj);

                if (!cJSON_AddItemToArray(cj_side_effects, cj_side_eff_elem))
                {
                    cJSON_Delete(cj_side_eff_elem);
                }
            }
        }
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *__action_methods_reset_hub()
{
    cJSON *cj_method = cJSON_CreateObject();
    if (cj_method) // resetHub
    {
        cJSON_AddNullToObject(cj_method, "dataSource");
        //-------------------------------------------------------------------------
        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------
        static const char *execution_raw_str = "[\"async\"]";
        cJSON_AddRawToObject(cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *__action_methods_reset_latch()
{
    cJSON *cj_method = cJSON_CreateObject();
    if (cj_method) // resetLatch
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_method, "dataSource", data_src_obj);
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"async\"]";
        cJSON_AddRawToObject(cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *__action_methods_set_variable()
{
    cJSON *cj_method = cJSON_CreateObject();
    if (cj_method) // setVariable
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_method, "dataSource", data_src_obj);
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"expression\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"sync\"]";
        cJSON_AddRawToObject(cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------

        static const char *type_system_raw_str = "[\"itemValueTypes\",\"free\"]";
        cJSON_AddRawToObject(cj_method, "typeSystem", type_system_raw_str);
        //-------------------------------------------------------------------------

        cJSON *cj_side_effects = cJSON_AddArrayToObject(cj_method, "sideEffects");
        if (cj_side_effects)
        {
            cJSON *cj_side_eff_elem = cJSON_CreateObject();
            if (cj_side_eff_elem)
            {
                cJSON_AddStringToObject(cj_side_eff_elem, "action", "saveResult");
                __add_data_src_dest_array_to_object(cj_side_eff_elem, "dataTarget", data_target_obj);

                if (!cJSON_AddItemToArray(cj_side_effects, cj_side_eff_elem))
                {
                    cJSON_Delete(cj_side_eff_elem);
                }
            }
        }
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *__action_methods_reset_scene_latches()
{
    cJSON *cj_method = cJSON_CreateObject();
    if (cj_method) // resetSceneLatches
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_method, "dataSource", data_src_obj);
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"async\"]";
        cJSON_AddRawToObject(cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *__action_methods_set_expression()
{
    cJSON *cj_method = cJSON_CreateObject();
    if (cj_method) // setExpression
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_method, "dataSource", data_src_obj);
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"expression\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"sync\"]";
        cJSON_AddRawToObject(cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------

        static const char *type_system_raw_str = "[\"itemValueTypes\",\"free\"]";
        cJSON_AddRawToObject(cj_method, "typeSystem", type_system_raw_str);
        //-------------------------------------------------------------------------

        cJSON *cj_side_effects = cJSON_AddArrayToObject(cj_method, "sideEffects");
        if (cj_side_effects)
        {
            cJSON *cj_side_eff_elem = cJSON_CreateObject();
            if (cj_side_eff_elem)
            {
                cJSON_AddStringToObject(cj_side_eff_elem, "action", "saveResult");
                __add_data_src_dest_array_to_object(cj_side_eff_elem, "dataTarget", data_target_obj);

                if (!cJSON_AddItemToArray(cj_side_effects, cj_side_eff_elem))
                {
                    cJSON_Delete(cj_side_eff_elem);
                }
            }
        }
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *__action_methods_toggle_value()
{
    cJSON *cj_method = cJSON_CreateObject();
    if (cj_method) // toggleValue
    {
        cJSON_AddNullToObject(cj_method, "dataSource");
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"item\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"async\"]";
        cJSON_AddRawToObject(cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------

        cJSON_AddStringToObject(cj_method, "typeSystem", "itemValueTypes");
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *__action_methods_group_set_item_value()
{
    cJSON *cj_method = cJSON_CreateObject();
    if (cj_method) // groupSetItemValue
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"constant\",\"expression\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_method, "dataSource", data_src_obj);
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"device_group\"]", .field = NULL},
            {.types = "[\"item_group\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"async\"]";
        cJSON_AddRawToObject(cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------

        cJSON_AddStringToObject(cj_method, "typeSystem", "itemValueTypes");
        //-------------------------------------------------------------------------

        static const char *scope_raw_str = "[\"local\"]";
        cJSON_AddRawToObject(cj_method, "scope", execution_raw_str);
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *__action_methods_group_toggle_value()
{
    cJSON *cj_method = cJSON_CreateObject();
    if (cj_method) // groupToggleValue
    {
        cJSON_AddNullToObject(cj_method, "dataSource");
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"device_group\"]", .field = NULL},
            {.types = "[\"item_group\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"async\"]";
        cJSON_AddRawToObject(cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------

        cJSON_AddStringToObject(cj_method, "typeSystem", "itemValueTypes");
        //-------------------------------------------------------------------------

        static const char *scope_raw_str = "[\"local\"]";
        cJSON_AddRawToObject(cj_method, "scope", execution_raw_str);
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *__action_methods_group_set_device_armed()
{
    cJSON *cj_method = cJSON_CreateObject();
    if (cj_method) // groupSetDeviceArmed
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_method, "dataSource", data_src_obj);
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"device_group\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        __add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"async\"]";
        cJSON_AddRawToObject(cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------

        cJSON_AddStringToObject(cj_method, "typeSystem", "itemValueTypes");
        //-------------------------------------------------------------------------

        static const char *scope_raw_str = "[\"local\"]";
        cJSON_AddRawToObject(cj_method, "scope", execution_raw_str);
        //-------------------------------------------------------------------------
    }
    return cj_method;
}

static void __action_methods_list(char *list_name, cJSON *cj_result)
{
    if (cj_result)
    {
        cJSON *cj_action_methods = cJSON_AddObjectToObject(cj_result, list_name);
        if (cj_action_methods)
        {
            cJSON_AddItemToObject(cj_action_methods, "Info", __action_methods_info());
            cJSON_AddItemToObject(cj_action_methods, "setItemValue", __action_methods_set_item_value());
            cJSON_AddItemToObject(cj_action_methods, "setDeviceArmed", __action_methods_set_device_armed());
            cJSON_AddItemToObject(cj_action_methods, "sendCloudAbstractCommand", __action_methods_send_cloud_abstract_command());
            cJSON_AddItemToObject(cj_action_methods, "switchHouseMode", __action_methods_switch_house_mode());
            cJSON_AddItemToObject(cj_action_methods, "sendHttpRequest", __action_methods_send_http_request());
            cJSON_AddItemToObject(cj_action_methods, "runCustomScript", __action_methods_run_custom_script());
            cJSON_AddItemToObject(cj_action_methods, "runPluginScript", __action_methods_run_plugin_script());
            cJSON_AddItemToObject(cj_action_methods, "runScene", __action_methods_run_scene());
            cJSON_AddItemToObject(cj_action_methods, "stopScene", __action_methods_stop_scene());
            cJSON_AddItemToObject(cj_action_methods, "setSceneState", __action_methods_set_scene_state());
            cJSON_AddItemToObject(cj_action_methods, "rebootHub", __action_methods_reboot_hub());
            cJSON_AddItemToObject(cj_action_methods, "cloudAPI", __action_methods_cloud_api());
            cJSON_AddItemToObject(cj_action_methods, "resetHub", __action_methods_reset_hub());
            cJSON_AddItemToObject(cj_action_methods, "resetLatch", __action_methods_reset_latch());
            cJSON_AddItemToObject(cj_action_methods, "setVariable", __action_methods_set_variable());
            cJSON_AddItemToObject(cj_action_methods, "resetSceneLatches", __action_methods_reset_scene_latches());
            cJSON_AddItemToObject(cj_action_methods, "setExpression", __action_methods_set_expression());
            cJSON_AddItemToObject(cj_action_methods, "toggleValue", __action_methods_toggle_value());
            cJSON_AddItemToObject(cj_action_methods, "groupSetItemValue", __action_methods_group_set_item_value());
            cJSON_AddItemToObject(cj_action_methods, "groupToggleValue", __action_methods_group_toggle_value());
            cJSON_AddItemToObject(cj_action_methods, "groupSetDeviceArmed", __action_methods_group_set_device_armed());
        }
    }
}

static void __advanced_scenes_version_list(char *list_name, cJSON *cj_result)
{
    if (cj_result)
    {
        cJSON *cj_avance_scenes_version = cJSON_AddObjectToObject(cj_result, list_name);
        if (cj_avance_scenes_version)
        {
            cJSON_AddStringToObject(cj_avance_scenes_version, "version", "Major.Minor");
        }
    }
}

// helper functions

static void __add_data_src_dest_array_to_object(cJSON *cj_method, char *array_name, s_data_source_n_target_object_t *data_list)
{
    cJSON *cj_data_source_n_target_list = cJSON_AddArrayToObject(cj_method, array_name);
    if (cj_data_source_n_target_list)
    {
        uint32_t idx = 0;
        while (data_list[idx].types || data_list[idx].field)
        {
            cJSON *cj_arr_object = cJSON_CreateObject();
            if (cj_arr_object)
            {
                cJSON_AddNumberToObject(cj_arr_object, "index", idx);
                if (data_list[idx].types)
                {
                    cJSON_AddRawToObject(cj_arr_object, "types", data_list[idx].types);
                }

                if (data_list[idx].field)
                {
                    cJSON_AddStringToObject(cj_arr_object, "field", data_list[idx].field);
                }

                if (!cJSON_AddItemToArray(cj_data_source_n_target_list, cj_arr_object))
                {
                    cJSON_Delete(cj_arr_object);
                }
            }

            idx++;
        }
    }
}