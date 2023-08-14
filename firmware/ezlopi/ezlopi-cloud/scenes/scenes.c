#include <string.h>
#include <stdint.h>

#include "scenes.h"
#include "trace.h"
#include "frozen.h"
#include "cJSON.h"
#include "ezlopi_nvs.h"
#include "ezlopi_scenes.h"
#include "ezlopi_devices.h"
#include "ezlopi_cloud_constants.h"

void scenes_list(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON *cjson_result = cJSON_AddObjectToObject(cj_response, ezlopi_result);
    if (cjson_result)
    {
        cJSON *cjson_scenes_array = ezlopi_scenes_create_cjson_scene_list(ezlopi_scenes_get_scenes_list());
        if (cjson_scenes_array)
        {
            if (!cJSON_AddItemToObject(cjson_result, "scenes", cjson_scenes_array))
            {
                cJSON_Delete(cjson_scenes_array);
            }
        }
    }
}

void scenes_create(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    // cJSON *cjson_result = cJSON_AddObjectToObject(cj_response, ezlopi_result);

    cJSON *cj_params = cJSON_GetObjectItem(cj_request, "params");
    if (cj_params)
    {
        ezlopi_scene_add_new_scene_cjson(cj_params);
        esp_restart();
    }
}

void scenes_get(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));

    uint32_t tmp_added_result = 0;

    cJSON *cj_params = cJSON_GetObjectItem(cj_request, "params");
    if (cj_params)
    {
        cJSON *cj_ids = cJSON_GetObjectItem(cj_params, "_id");
        if (cj_ids && cj_ids->valuestring)
        {
            uint32_t incoming_scene_id = strtoul(cj_ids->valuestring, NULL, 16);
            l_scenes_list_t *scenes_list = ezlopi_scenes_get_scenes_list();

            while (scenes_list)
            {
                if (incoming_scene_id == scenes_list->_id)
                {
                    cJSON *cj_scene = ezlopi_scenes_create_cjson_scene(scenes_list);
                    if (cj_scene)
                    {
                        if (cJSON_AddItemToObject(cj_response, ezlopi_result, cj_scene))
                        {
                            tmp_added_result = 1;
                        }
                        else
                        {
                            cJSON_Delete(cj_scene);
                        }
                    }
                }
                scenes_list = scenes_list->next;
            }
        }
    }

    if (0 == tmp_added_result)
    {
        cJSON_AddObjectToObject(cj_response, ezlopi_result);
    }
}

void scenes_edit(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));

    cJSON *cj_params = cJSON_GetObjectItem(cj_request, "params");
    if (cj_params)
    {
        cJSON *cj_id = cJSON_GetObjectItem(cj_params, "_id");
        if (cj_id && cj_id->valuestring)
        {
            uint32_t u_id = strtoul(cj_id->valuestring, NULL, 16);
            cJSON *cj_eo = cJSON_GetObjectItem(cj_params, "eo");
            if (cj_eo)
            {
                ezlopi_scenes_update_by_id(u_id, cj_eo);
            }
        }
    }
}

void scenes_delete(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON_AddObjectToObject(cj_response, "result");

    cJSON *cj_params = cJSON_GetObjectItem(cj_request, "params");
    if (cj_params)
    {
        cJSON *cj_id = cJSON_GetObjectItem(cj_params, "_id");
        if (cj_id && cj_id->valuestring)
        {
            uint32_t u_id = strtoul(cj_id->valuestring, NULL, 16);
            ezlopi_scenes_delete_by_id(u_id);
        }
    }
}

void scenes_status_get(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON_AddObjectToObject(cj_response, "result");

    TRACE_E("Not implemented!");
#warning "Incomplete"

    cJSON *cj_params = cJSON_GetObjectItem(cj_request, "params");
    if (cj_params)
    {
        cJSON *cj_scene_id = cJSON_GetObjectItem(cj_params, "sceneId");
        if (cj_scene_id && cj_scene_id->valuestring)
        {
            uint32_t u_id = strtoul(cj_scene_id->valuestring, NULL, 16);
        }
    }
}

void scenes_blocks_list(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi__id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));

    cJSON *cj_result = cJSON_AddObjectToObject(cj_response, "result");
    if (cj_result)
    {
        cJSON *cj_paramas = cJSON_GetObjectItem(cj_request, "params");
        if (cj_paramas)
        {
            cJSON *cj_block_type = cJSON_GetObjectItem(cj_paramas, "blockType");
            if (cj_block_type && cj_block_type->valuestring)
            {
                cJSON *cj_block_array = NULL;
                e_scenes_block_type_t block_type = SCENE_BLOCK_TYPE_NONE;
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
                            l_scenes_list_t *req_scene = ezlopi_scenes_get_by_id(device_id);

                            if (req_scene)
                            {
                                switch (block_type)
                                {
                                case SCENE_BLOCK_TYPE_WHEN:
                                {
                                    l_when_block_t *curr_when_block = req_scene->when;
                                    while (curr_when_block)
                                    {
                                        cJSON *cj_when_block = ezlopi_scenes_cjson_create_when_block(req_scene->when);
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
                                case SCENE_BLOCK_TYPE_THEN:
                                {
                                    l_then_block_t *curr_then_block = req_scene->then;
                                    while (curr_then_block)
                                    {
                                        cJSON *cj_then_block = ezlopi_scenes_cjson_create_then_block(req_scene->then);
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

typedef struct s_block_data_list_collector
{
    char *key_string;
    void (*func)(char *list_name, cJSON *result);
} s_block_data_list_collector_t;

static void __value_types_list(char *list_name, cJSON *cj_result);
static void __scalable_value_types_list(char *list_name, cJSON *cj_result);
static void __value_scales_list(char *list_name, cJSON *cj_result);
static void __scenes_value_types_list(char *list_name, cJSON *cj_result);
static void __value_types_families_list(char *list_name, cJSON *cj_result);
static void __comparison_operators_list(char *list_name, cJSON *cj_result);
static void __comparison_methods_list(char *list_name, cJSON *cj_result);
static void __advanced_scenes_version_list(char *list_name, cJSON *cj_result);

static s_block_data_list_collector_t block_data_list_collector[] = {
    {.key_string = "valueTypes", .func = __value_types_list},
    {.key_string = "scalableValueTypes", .func = __scalable_value_types_list},
    {.key_string = "valueScales", .func = __value_scales_list},
    {.key_string = "scenesValueTypes", .func = __scenes_value_types_list},
    {.key_string = "valueTypeFamilies", .func = __value_types_families_list},
    {.key_string = "comparisonOperators", .func = __comparison_operators_list},
    {.key_string = "comparisonMethods", .func = __comparison_methods_list},
    {.key_string = "advancedScenesVersion", .func = __advanced_scenes_version_list},
    {.key_string = NULL, .func = NULL},
};

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

typedef struct s_scenes_value_types
{
    char *value_type;
} s_scenes_value_types_t;
static const s_scenes_value_types_t scenes_value_type_list[] = {
    {.value_type = "24_hours_interval"},
    {.value_type = "interval"},
    {.value_type = "hms_interval"},
    {.value_type = NULL},
};
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
                uint32_t idx = 0;
                while (scenes_value_type_list[idx].value_type)
                {
                    cJSON *cj_string_val = cJSON_CreateString(scenes_value_type_list[idx].value_type);
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
            cJSON_AddRawToObject(cj_value_type_famiies, "numeric", numeric);
            cJSON_AddRawToObject(cj_value_type_famiies, "strings", strings);
        }
    }
}

#include "ezlopi_scenes_operators.h"

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
                cJSON *cj_family = cJSON_CreateObject();
                if (cj_family)
                {
                    cJSON_AddStringToObject(cj_family, "family", "numeric");
                    cJSON *cj_methods_array = cJSON_AddArrayToObject(cj_family, "methods");
                    if (cj_methods_array)
                    {
                        e_scene_cmp_operators_t op_idx = SCENES_OPERATORS_LESS;
                        TRACE_B("op_idx: %u", op_idx);
                        TRACE_B("op func: %p", ezlopi_scenes_operators_get_op);
                        while (ezlopi_scenes_operators_get_op(op_idx))
                        {
                            cJSON *cj_method = cJSON_CreateObject();
                            if (cj_method)
                            {
                                const char *op = ezlopi_scenes_operators_get_op(op_idx);
                                const char *name = ezlopi_scenes_operators_get_name(op_idx);
                                const char *method = ezlopi_scenes_operators_get_method(op_idx);
                                TRACE_B("[idx: %u] :=> op: %s, name: %s, method: %s",
                                        op_idx, op ? op : "null",
                                        name ? name : "null",
                                        method ? method : "null");

                                cJSON_AddStringToObject(cj_method, "op", ezlopi_scenes_operators_get_op(op_idx));
                                cJSON_AddStringToObject(cj_method, "name", ezlopi_scenes_operators_get_name(op_idx));
                                cJSON_AddStringToObject(cj_method, "method", ezlopi_scenes_operators_get_method(op_idx));

                                if (!cJSON_AddItemToArray(cj_methods_array, cj_method))
                                {
                                    cJSON_Delete(cj_method);
                                }
                            }

                            op_idx++;
                        }
                    }
                }

                if (!cJSON_AddItemToArray(cj_families_array, cj_family))
                {
                    cJSON_Delete(cj_family);
                }
            }
        }
    }
}
static void __comparison_methods_list(char *list_name, cJSON *cj_result)
{
    if (cj_result)
    {
        cJSON *cj_comparision_methods = cJSON_AddObjectToObject(cj_result, list_name);
        if (cj_comparision_methods)
        {
            const static char *info = "{\"version\":\"1.0.0\"}";
            cJSON_AddRawToObject(cj_comparision_methods, "info", info);
            cJSON *cj_compare_numbers = cJSON_AddObjectToObject(cj_comparision_methods, "compareNumbers");
            if (cj_compare_numbers)
            {
                const static char *comparator = "{\"family\":\"numeric\",\"field\":\"comparator\",\"options\":[\"<\",\">\",\"<=\",\">=\",\"==\",\"!=\"\"],\"type\":\"enum\"}";
                const static char *data_source = "[{\"index\":0,\"types\":[\"item\",\"expression\"]},{\"index\":1,\"types\":[\"constant\",\"expression\"]}]";
                cJSON_AddStringToObject(cj_compare_numbers, "comparator", comparator);
                cJSON_AddStringToObject(cj_compare_numbers, "dataSource", data_source);
            }
            cJSON_AddRawToObject(cj_comparision_methods, "info", info);

            cJSON *cj_compare_strings = cJSON_AddObjectToObject(cj_comparision_methods, "compareStrings");
            if (cj_compare_strings)
            {
                const static char *comparator = "{\"family\":\"strings\",\"field\":\"comparator\",\"options\":[\"<\",\">\",\"<=\",\">=\",\"==\",\"!=\"\"],\"type\":\"enum\"}";
                const static char *data_source = "[{\"index\":0,\"types\":[\"item\",\"expression\"]},{\"index\":1,\"types\":[\"constant\",\"expression\"]}]";
                cJSON_AddStringToObject(cj_compare_strings, "comparator", comparator);
                cJSON_AddStringToObject(cj_compare_strings, "dataSource", data_source);
            }
        }
    }
}
static void __advanced_scenes_version_list(char *list_name, cJSON *cj_result)
{
    if (cj_result)
    {
    }
}

void scenes_block_data_list(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
        cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));

        cJSON *cj_params = cJSON_GetObjectItem(cj_request, "params");
        if (cj_params)
        {
            cJSON *cj_result = cJSON_AddObjectToObject(cj_response, "result");
            if (cj_result)
            {
                cJSON *temp = cj_params->child;

                while (temp != NULL)
                {
                    uint32_t idx = 0;
                    while (block_data_list_collector[idx].func)
                    {
                        if (0 == strcmp(block_data_list_collector[idx].key_string, temp->string))
                        {
                            block_data_list_collector[idx].func(block_data_list_collector[idx].key_string, cj_result);
                        }
                        idx++;
                    }

                    temp = temp->next;
                }
            }
        }
    }
}

void scenes_scripts_list(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
        cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));

        cJSON *cj_result = cJSON_AddObjectToObject(cj_response, "result");
        if (cj_result)
        {
        }
    }
}

void scenes_run(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON *cjson_result = cJSON_AddObjectToObject(cj_response, ezlopi_result);

    cJSON *cj_params = cJSON_GetObjectItem(cj_request, "params");
    if (cj_params)
    {
        cJSON *cj_scene_id = cJSON_GetObjectItem(cj_params, "sceneId");
        if (cj_scene_id && cj_scene_id->valuestring)
        {
            uint32_t u32_scene_id = strtoul(cj_scene_id->valuestring, NULL, 16);
            l_scenes_list_t *scene_object = ezlopi_scenes_get_by_id(u32_scene_id);
            if (scene_object)
            {
#warning "Need to call Secene logic execute here."
            }
        }
    }
}
