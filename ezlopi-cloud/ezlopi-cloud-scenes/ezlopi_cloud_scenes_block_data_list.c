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

/**
 * @file    ezlopi_cloud_scenes_block_data_list.c
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
#include "cjext.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_cloud_scenes.h"
#include "ezlopi_cloud_constants.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_scenes_v2.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_scenes_value.h"
#include "ezlopi_core_scenes_operators.h"

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
/**
 * @brief Structure that wraps data source and target object
 *
 */
typedef struct s_data_source_n_target_object
{
    char *types; /**< Data type */
    char *field; /**< Data source */
} s_data_source_n_target_object_t;

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
static void ezpi_value_types_list(char *list_name, cJSON *cj_result);
static void ezpi_scalable_value_types_list(char *list_name, cJSON *cj_result);
static void ezpi_value_scales_list(char *list_name, cJSON *cj_result);
static void ezpi_scenes_value_types_list(char *list_name, cJSON *cj_result);
static void ezpi_value_types_families_list(char *list_name, cJSON *cj_result);
static cJSON *ezpi_comparision_operators_numeric(void);
static cJSON *ezpi_comparision_operators_strings(void);
static cJSON *ezpi_comparision_operators_values_with_less(void);
static cJSON *ezpi_comparision_operators_values_without_less(void);
static void ezpi_comparison_operators_list(char *list_name, cJSON *cj_result);
static cJSON *ezpi_comparision_method_info(void);
static cJSON *ezpi_comparision_method_compare_number_range(void);
static cJSON *ezpi_comparision_method_compare_numbers(void);
static cJSON *ezpi_comparision_method_compare_strings(void);
static cJSON *ezpi_comparision_method_compare_values(void);
static cJSON *ezpi_comparision_method_in_array(void);
static cJSON *ezpi_comparision_method_is_device_item_group(void);
static cJSON *ezpi_comparision_method_is_device_state(void);
static cJSON *ezpi_comparision_method_is_item_state(void);
static cJSON *ezpi_comparision_method_is_item_state_changed(void);
static cJSON *ezpi_comparision_method_string_operation(void);
static void ezpi_comparison_methods_list(char *list_name, cJSON *cj_result);
static cJSON *ezpi_action_methods_info(void);
static cJSON *ezpi_action_methods_set_item_value();
static cJSON *ezpi_action_methods_set_device_armed();
static cJSON *ezpi_action_methods_send_cloud_abstract_command();
static cJSON *ezpi_action_methods_switch_house_mode();
static cJSON *ezpi_action_methods_send_http_request();
static cJSON *ezpi_action_methods_run_custom_script();
static cJSON *ezpi_action_methods_run_plugin_script();
static cJSON *ezpi_action_methods_run_scene();
static cJSON *ezpi_action_methods_stop_scene();
static cJSON *ezpi_action_methods_set_scene_state();
static cJSON *ezpi_action_methods_reboot_hub();
static cJSON *ezpi_action_methods_cloud_api();
static cJSON *ezpi_action_methods_reset_hub();
static cJSON *ezpi_action_methods_reset_latch();
static cJSON *ezpi_action_methods_set_variable();
static cJSON *ezpi_action_methods_reset_scene_latches();
static cJSON *ezpi_action_methods_set_expression();
static cJSON *ezpi_action_methods_toggle_value();
static cJSON *ezpi_action_methods_group_set_item_value();
static cJSON *ezpi_action_methods_group_toggle_value();
static cJSON *ezpi_action_methods_group_set_device_armed();
static void ezpi_action_methods_list(char *list_name, cJSON *cj_result);
static void ezpi_advanced_scenes_version_list(char *list_name, cJSON *cj_result);
static void ezpi_add_data_src_dest_array_to_object(cJSON *cj_method, char *array_name, const s_data_source_n_target_object_t *data_list);

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
void EZPI_scenes_block_data_list(cJSON *cj_request, cJSON *cj_response)
{
    typedef struct s_block_data_list_collector
    {
        char *key_string;
        void (*func)(char *list_name, cJSON *result);
    } s_block_data_list_collector_t;

    static const s_block_data_list_collector_t block_data_list_collector[] = {
        {.key_string = "valueTypes", .func = ezpi_value_types_list},
        {.key_string = "scalableValueTypes", .func = ezpi_scalable_value_types_list},
        {.key_string = "valueScales", .func = ezpi_value_scales_list},
        {.key_string = "scenesValueTypes", .func = ezpi_scenes_value_types_list},
        {.key_string = "valueTypeFamilies", .func = ezpi_value_types_families_list},
        {.key_string = "comparisonOperators", .func = ezpi_comparison_operators_list},
        {.key_string = "comparisonMethods", .func = ezpi_comparison_methods_list},
        {.key_string = "actionMethods", .func = ezpi_action_methods_list},
        {.key_string = "advancedScenesVersion", .func = ezpi_advanced_scenes_version_list},
        {.key_string = NULL, .func = NULL},
    };

    if (cj_request && cj_response)
    {
        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
            if (cj_result)
            {
                uint32_t idx = 0;
                while (block_data_list_collector[idx].func)
                {
                    cJSON *cj_value = cJSON_GetObjectItem(__FUNCTION__, cj_params, block_data_list_collector[idx].key_string);
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
/*******************************************************************************
 *                          Static Function Definitions
 *******************************************************************************/
static void ezpi_value_types_list(char *list_name, cJSON *cj_result)
{
    if (cj_result && list_name)
    {
        cJSON *cj_value_types = cJSON_AddObjectToObject(__FUNCTION__, cj_result, list_name);
        if (cj_value_types)
        {
            cJSON *cj_value_array = cJSON_AddArrayToObject(__FUNCTION__, cj_value_types, "list");
            if (cj_value_array)
            {
                l_ezlopi_device_t *devices = EZPI_core_device_get_head();
                while (devices)
                {
                    l_ezlopi_item_t *items = devices->items;
                    while (items)
                    {
                        if (NULL == items->cloud_properties.scale)
                        {
                            cJSON *cj_item_value_type = cJSON_CreateString(__FUNCTION__, items->cloud_properties.value_type);
                            if (cj_item_value_type)
                            {
                                if (!cJSON_AddItemToArray(cj_value_array, cj_item_value_type))
                                {
                                    cJSON_Delete(__FUNCTION__, cj_item_value_type);
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

static void ezpi_scalable_value_types_list(char *list_name, cJSON *cj_result)
{
    if (cj_result && list_name)
    {
        cJSON *cj_value_types = cJSON_AddObjectToObject(__FUNCTION__, cj_result, list_name);
        if (cj_value_types)
        {
            cJSON *cj_value_array = cJSON_AddArrayToObject(__FUNCTION__, cj_value_types, "list");
            if (cj_value_array)
            {
                l_ezlopi_device_t *devices = EZPI_core_device_get_head();
                while (devices)
                {
                    l_ezlopi_item_t *items = devices->items;
                    while (items)
                    {
                        if (items->cloud_properties.scale)
                        {
                            cJSON *cj_item_value_type = cJSON_CreateString(__FUNCTION__, items->cloud_properties.value_type);
                            if (cj_item_value_type)
                            {
                                if (!cJSON_AddItemToArray(cj_value_array, cj_item_value_type))
                                {
                                    cJSON_Delete(__FUNCTION__, cj_item_value_type);
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

static void ezpi_value_scales_list(char *list_name, cJSON *cj_result)
{
    if (cj_result && list_name)
    {
        cJSON *cj_value_scales = cJSON_AddObjectToObject(__FUNCTION__, cj_result, list_name);
        if (cj_value_scales)
        {
            l_ezlopi_device_t *devices = EZPI_core_device_get_head();
            while (devices)
            {
                l_ezlopi_item_t *items = devices->items;
                while (items)
                {
                    if (items->cloud_properties.scale)
                    {
                        cJSON *cj_scale_array = NULL;
                        cJSON *cj_value_type = cJSON_GetObjectItem(__FUNCTION__, cj_value_scales, items->cloud_properties.value_type);
                        if (NULL == cj_value_type)
                        {
                            cj_value_type = cJSON_AddObjectToObject(__FUNCTION__, cj_value_scales, items->cloud_properties.value_type);
                            if (cj_value_type)
                            {
                                cj_scale_array = cJSON_AddArrayToObject(__FUNCTION__, cj_value_type, "scales");
                                cJSON_AddFalseToObject(__FUNCTION__, cj_value_type, "converter");
                            }
                        }
                        else
                        {
                            cj_scale_array = cJSON_GetObjectItem(__FUNCTION__, cj_value_type, "scales");
                        }

                        if (cj_scale_array)
                        {
                            cJSON *cj_scale = cJSON_CreateString(__FUNCTION__, items->cloud_properties.scale);
                            if (cj_scale)
                            {
                                if (!cJSON_AddItemToArray(cj_scale_array, cj_scale))
                                {
                                    cJSON_Delete(__FUNCTION__, cj_scale);
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

static void ezpi_scenes_value_types_list(char *list_name, cJSON *cj_result)
{
    if (cj_result && list_name)
    {
        cJSON *cj_scenes_value_types = cJSON_AddObjectToObject(__FUNCTION__, cj_result, list_name);
        if (cj_scenes_value_types)
        {
            cJSON *cj_value_array = cJSON_AddArrayToObject(__FUNCTION__, cj_scenes_value_types, "list");
            if (cj_value_array)
            {
                uint32_t idx = EZLOPI_VALUE_TYPE_NONE + 1;
                while (idx < EZLOPI_VALUE_TYPE_MAX)
                {
                    const char *type_name_str = EZPI_core_scenes_get_scene_value_type_name(idx);
                    if (type_name_str)
                    {
                        cJSON *cj_string_val = cJSON_CreateString(__FUNCTION__, type_name_str);
                        if (cj_string_val)
                        {
                            if (!cJSON_AddItemToArray(cj_value_array, cj_string_val))
                            {
                                cJSON_Delete(__FUNCTION__, cj_string_val);
                            }
                        }
                    }

                    idx++;
                }
            }
        }
    }
}

static void ezpi_value_types_families_list(char *list_name, cJSON *cj_result)
{
    if (cj_result && list_name)
    {
        cJSON *cj_value_type_famiies = cJSON_AddObjectToObject(__FUNCTION__, cj_result, list_name);
        if (cj_value_type_famiies)
        {
            const static char *numeric = "[\"int\",\"float\",\"scalableValueTypes\"]";
            const static char *strings = "[\"string\",\"token\"]";
            const static char *value_with_less = "[\"int\",\"float\",\"scalableValueTypes\",\"string\"]";
            const static char *value_without_less = "[]"; // remained to fill

            cJSON_AddRawToObject(__FUNCTION__, cj_value_type_famiies, "numeric", numeric);
            cJSON_AddRawToObject(__FUNCTION__, cj_value_type_famiies, "strings", strings);
            cJSON_AddRawToObject(__FUNCTION__, cj_value_type_famiies, "valuesWithLess", value_with_less);
            cJSON_AddRawToObject(__FUNCTION__, cj_value_type_famiies, "valuesWithoutLess", value_without_less);
        }
    }
}

static cJSON *ezpi_comparision_operators_numeric(void)
{
    cJSON *cj_family = cJSON_CreateObject(__FUNCTION__);
    if (cj_family)
    {
        cJSON_AddStringToObject(__FUNCTION__, cj_family, "family", "numeric");
        cJSON *cj_methods_array = cJSON_AddArrayToObject(__FUNCTION__, cj_family, "methods");
        if (cj_methods_array)
        {
            e_scene_num_cmp_operators_t op_idx = SCENES_NUM_COMP_OPERATORS_NONE + 1;
            while (EZPI_scenes_numeric_comparator_operators_get_op(op_idx))
            {
                cJSON *cj_method = cJSON_CreateObject(__FUNCTION__);
                if (cj_method)
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_method, "op", EZPI_scenes_numeric_comparator_operators_get_op(op_idx));
                    cJSON_AddStringToObject(__FUNCTION__, cj_method, ezlopi_name_str, EZPI_scenes_numeric_comparator_operators_get_name(op_idx));
                    cJSON_AddStringToObject(__FUNCTION__, cj_method, ezlopi_method_str, EZPI_scenes_numeric_comparator_operators_get_method(op_idx));

                    if (!cJSON_AddItemToArray(cj_methods_array, cj_method))
                    {
                        cJSON_Delete(__FUNCTION__, cj_method);
                        cj_method = NULL;
                    }
                }

                op_idx++;
            }
        }
    }

    return cj_family;
}

static cJSON *ezpi_comparision_operators_strings(void)
{
    cJSON *cj_family = cJSON_CreateObject(__FUNCTION__);
    if (cj_family)
    {
        cJSON_AddStringToObject(__FUNCTION__, cj_family, "family", "strings");
        cJSON *cj_methods_array = cJSON_AddArrayToObject(__FUNCTION__, cj_family, "methods");
        if (cj_methods_array)
        {
            /* <1> Adding for 'Strings' within 'family : strings' */
            e_scene_str_cmp_operators_t op_idx = SCENES_STRINGS_OPERATORS_NONE + 1;
            while (EZPI_scenes_strings_comparator_operators_get_op(op_idx))
            {
                cJSON *cj_method = cJSON_CreateObject(__FUNCTION__);
                if (cj_method)
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_method, "op", EZPI_scenes_strings_comparator_operators_get_op(op_idx));
                    cJSON_AddStringToObject(__FUNCTION__, cj_method, ezlopi_name_str, EZPI_scenes_strings_comparator_operators_get_name(op_idx));
                    cJSON_AddStringToObject(__FUNCTION__, cj_method, ezlopi_method_str, EZPI_scenes_strings_comparator_operators_get_method(op_idx));

                    if (!cJSON_AddItemToArray(cj_methods_array, cj_method))
                    {
                        cJSON_Delete(__FUNCTION__, cj_method);
                        cj_method = NULL;
                    }
                }
                op_idx++;
            }

            /* <2> Adding for 'StringOperators' within 'family : strings' */
            op_idx = SCENES_STROPS_COMP_OPERATORES_NONE + 1;
            while (EZPI_scenes_strops_comparator_operators_get_op(op_idx))
            {
                cJSON *cj_method = cJSON_CreateObject(__FUNCTION__);
                if (cj_method)
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_method, "op", EZPI_scenes_strops_comparator_operators_get_op(op_idx));
                    cJSON_AddStringToObject(__FUNCTION__, cj_method, ezlopi_name_str, EZPI_scenes_strops_comparator_operators_get_name(op_idx));
                    cJSON_AddStringToObject(__FUNCTION__, cj_method, ezlopi_method_str, EZPI_scenes_strops_comparator_operators_get_method(op_idx));

                    if (!cJSON_AddItemToArray(cj_methods_array, cj_method))
                    {
                        cJSON_Delete(__FUNCTION__, cj_method);
                        cj_method = NULL;
                    }
                }
                op_idx++;
            }
        }
    }

    return cj_family;
}

static cJSON *ezpi_comparision_operators_values_with_less(void)
{
    cJSON *cj_family = cJSON_CreateObject(__FUNCTION__);
    if (cj_family)
    {
        cJSON_AddStringToObject(__FUNCTION__, cj_family, "family", "valuesWithLess");
        cJSON *cj_methods_array = cJSON_AddArrayToObject(__FUNCTION__, cj_family, "methods");
        if (cj_methods_array)
        {
            e_scene_value_with_less_cmp_operators_t op_idx = SCENES_VALUES_WITH_LESS_OPERATORS_NONE + 1;
            while (EZPI_scenes_value_with_less_comparator_operators_get_op(op_idx))
            {
                cJSON *cj_method = cJSON_CreateObject(__FUNCTION__);
                if (cj_method)
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_method, "op", EZPI_scenes_value_with_less_comparator_operators_get_op(op_idx));
                    cJSON_AddStringToObject(__FUNCTION__, cj_method, ezlopi_name_str, EZPI_scenes_value_with_less_comparator_operators_get_name(op_idx));
                    cJSON_AddStringToObject(__FUNCTION__, cj_method, ezlopi_method_str, EZPI_scenes_value_with_less_comparator_operators_get_method(op_idx));

                    if (!cJSON_AddItemToArray(cj_methods_array, cj_method))
                    {
                        cJSON_Delete(__FUNCTION__, cj_method);
                        cj_method = NULL;
                    }
                }

                op_idx++;
            }
        }
    }

    return cj_family;
}

static cJSON *ezpi_comparision_operators_values_without_less(void)
{
    cJSON *cj_family = cJSON_CreateObject(__FUNCTION__);
    if (cj_family)
    {
        cJSON_AddStringToObject(__FUNCTION__, cj_family, "family", "valuesWithoutLess");
        cJSON *cj_methods_array = cJSON_AddArrayToObject(__FUNCTION__, cj_family, "methods");
        if (cj_methods_array)
        {
            e_scene_value_without_less_cmp_operators_t op_idx = SCENES_VALUES_WITHOUT_LESS_OPERATORS_NONE + 1;
            while (EZPI_scenes_value_without_less_comparator_operators_get_op(op_idx))
            {
                cJSON *cj_method = cJSON_CreateObject(__FUNCTION__);
                if (cj_method)
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_method, "op", EZPI_scenes_value_without_less_comparator_operators_get_op(op_idx));
                    cJSON_AddStringToObject(__FUNCTION__, cj_method, ezlopi_name_str, EZPI_scenes_value_without_less_comparator_operators_get_name(op_idx));
                    cJSON_AddStringToObject(__FUNCTION__, cj_method, ezlopi_method_str, EZPI_scenes_value_without_less_comparator_operators_get_method(op_idx));

                    if (!cJSON_AddItemToArray(cj_methods_array, cj_method))
                    {
                        cJSON_Delete(__FUNCTION__, cj_method);
                        cj_method = NULL;
                    }
                }

                op_idx++;
            }
        }
    }

    return cj_family;
}

static void ezpi_comparison_operators_list(char *list_name, cJSON *cj_result)
{
    if (cj_result)
    {
        cJSON *cj_value_types = cJSON_AddObjectToObject(__FUNCTION__, cj_result, list_name);
        if (cj_value_types)
        {
            cJSON *cj_families_array = cJSON_AddArrayToObject(__FUNCTION__, cj_value_types, "families");
            if (cj_families_array)
            {
                static cJSON *(*com_operators_funcs[])(void) = {
                    ezpi_comparision_operators_numeric,
                    ezpi_comparision_operators_strings,
                    ezpi_comparision_operators_values_with_less,
                    ezpi_comparision_operators_values_without_less,
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
                            cJSON_Delete(__FUNCTION__, cj_family);
                        }
                    }

                    family_idx++;
                }
            }
        }
    }
}

static cJSON *ezpi_comparision_method_info(void)
{
    cJSON *cj_info = cJSON_CreateObject(__FUNCTION__);
    if (cj_info)
    {
        cJSON_AddStringToObject(__FUNCTION__, cj_info, ezlopi_version_str, "1.0.0");
    }

    return cj_info;
}

static cJSON *ezpi_comparision_method_compare_number_range(void)
{
    cJSON *cj_compare_number_range = cJSON_CreateObject(__FUNCTION__);
    if (cj_compare_number_range)
    {
        cJSON *cj_comparator = cJSON_AddObjectToObject(__FUNCTION__, cj_compare_number_range, "comparator");
        if (cj_comparator)
        {
            cJSON_AddStringToObject(__FUNCTION__, cj_comparator, "family", "numeric");
            cJSON_AddStringToObject(__FUNCTION__, cj_comparator, "field", "comparator");
            cJSON_AddStringToObject(__FUNCTION__, cj_comparator, ezlopi_type_str, ezlopi_enum_str);

            const char *options_str = "[\"between\", \"not_between\"]";
            cJSON_AddRawToObject(__FUNCTION__, cj_comparator, "options", options_str);
        }

        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"item\",\"expression\",\"device_group\",\"item_group\"]", .field = NULL},
            {.types = "[\"constant\"]", .field = "startValue"},
            {.types = "[\"constant\"]", .field = "endValue"},
            {.types = NULL, .field = NULL},
        };

        ezpi_add_data_src_dest_array_to_object(cj_compare_number_range, "dataSource", data_src_obj);
    }

    return cj_compare_number_range;
}

static cJSON *ezpi_comparision_method_compare_numbers(void)
{
    cJSON *cj_compare_numbers = cJSON_CreateObject(__FUNCTION__);
    if (cj_compare_numbers)
    {

        cJSON *cj_comparator = cJSON_AddObjectToObject(__FUNCTION__, cj_compare_numbers, "comparator");
        if (cj_comparator)
        {
            cJSON_AddStringToObject(__FUNCTION__, cj_comparator, "family", "numeric");
            cJSON_AddStringToObject(__FUNCTION__, cj_comparator, "field", "comparator");
            cJSON_AddStringToObject(__FUNCTION__, cj_comparator, ezlopi_type_str, ezlopi_enum_str);

            const static char *options_str = "[\"<\",\">\",\"<=\",\">=\",\"==\",\"!=\"]";
            cJSON_AddRawToObject(__FUNCTION__, cj_comparator, "options", options_str);
        }

        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"item\",\"expression\",\"device_group\",\"item_group\"]", .field = NULL},
            {.types = "[\"constant\",\"expression\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_compare_numbers, "dataSource", data_src_obj);
    }

    return cj_compare_numbers;
}

static cJSON *ezpi_comparision_method_compare_strings(void)
{
    cJSON *cj_compare_strings = cJSON_CreateObject(__FUNCTION__);
    if (cj_compare_strings)
    {
        cJSON *cj_comparator = cJSON_AddObjectToObject(__FUNCTION__, cj_compare_strings, "comparator");
        if (cj_comparator)
        {
            cJSON_AddStringToObject(__FUNCTION__, cj_comparator, "family", "strings");
            cJSON_AddStringToObject(__FUNCTION__, cj_comparator, "field", "comparator");
            cJSON_AddStringToObject(__FUNCTION__, cj_comparator, ezlopi_type_str, ezlopi_enum_str);

            const static char *options_str = "[\"<\",\">\",\"<=\",\">=\",\"==\",\"!=\"]";
            cJSON_AddRawToObject(__FUNCTION__, cj_comparator, "options", options_str);
        }

        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"item\",\"expression\",\"device_group\",\"item_group\"]", .field = NULL},
            {.types = "[\"constant\",\"expression\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_compare_strings, "dataSource", data_src_obj);
    }

    return cj_compare_strings;
}

static cJSON *ezpi_comparision_method_compare_values(void)
{
    cJSON *cj_compare_values = cJSON_CreateObject(__FUNCTION__);
    if (cj_compare_values)
    {
        cJSON *cj_comparator = cJSON_AddObjectToObject(__FUNCTION__, cj_compare_values, "comparator");
        if (cj_comparator)
        {
            cJSON_AddStringToObject(__FUNCTION__, cj_comparator, "family", "valuesWithoutLess");
            cJSON_AddStringToObject(__FUNCTION__, cj_comparator, "field", "comparator");
            cJSON_AddStringToObject(__FUNCTION__, cj_comparator, ezlopi_type_str, ezlopi_enum_str);

            const static char *options_str = "[\"==\",\"!=\"]";
            cJSON_AddRawToObject(__FUNCTION__, cj_comparator, "options", options_str);
        }

        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"item\",\"expression\",\"device_group\",\"item_group\"]", .field = NULL},
            {.types = "[\"constant\",\"expression\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_compare_values, "dataSource", data_src_obj);
    }

    return cj_compare_values;
}

static cJSON *ezpi_comparision_method_in_array(void)
{
    cJSON *cj_in_array = cJSON_CreateObject(__FUNCTION__);
    if (cj_in_array)
    {
        cJSON *cj_comparator = cJSON_AddObjectToObject(__FUNCTION__, cj_in_array, "comparator");
        if (cj_comparator)
        {
            cJSON_AddStringToObject(__FUNCTION__, cj_comparator, "family", "array");
            cJSON_AddStringToObject(__FUNCTION__, cj_comparator, "field", "comparator");
            cJSON_AddStringToObject(__FUNCTION__, cj_comparator, ezlopi_type_str, ezlopi_enum_str);

            const static char *options_str = "[\"in\",\"not_in\"]";
            cJSON_AddRawToObject(__FUNCTION__, cj_comparator, "options", options_str);
        }

        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"item\",\"expression\",\"device_group\",\"item_group\"]", .field = NULL},
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_in_array, "dataSource", data_src_obj);
    }

    return cj_in_array;
}

static cJSON *ezpi_comparision_method_is_device_item_group(void)
{
    cJSON *cj_is_device_item_grp = cJSON_CreateObject(__FUNCTION__);
    if (cj_is_device_item_grp)
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"device_group\"]", .field = NULL},
            {.types = "[\"item_group\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };

        ezpi_add_data_src_dest_array_to_object(cj_is_device_item_grp, "dataSource", data_src_obj);
    }

    return cj_is_device_item_grp;
}

static cJSON *ezpi_comparision_method_is_device_state(void)
{
    cJSON *cj_is_device_state = cJSON_CreateObject(__FUNCTION__);
    if (cj_is_device_state)
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"device\",\"device_group\"]", .field = NULL},
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };

        ezpi_add_data_src_dest_array_to_object(cj_is_device_state, "dataSource", data_src_obj);
    }

    return cj_is_device_state;
}

static cJSON *ezpi_comparision_method_is_item_state(void)
{
    cJSON *cj_is_item_state = cJSON_CreateObject(__FUNCTION__);
    if (cj_is_item_state)
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"device_item\",\"item\",\"item_group\",\"device_group\"]", .field = NULL},
            {.types = "[\"constant\",\"expression\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };

        ezpi_add_data_src_dest_array_to_object(cj_is_item_state, "dataSource", data_src_obj);
    }

    return cj_is_item_state;
}

static cJSON *ezpi_comparision_method_is_item_state_changed(void)
{
    cJSON *cj_is_item_state_changed = cJSON_CreateObject(__FUNCTION__);
    if (cj_is_item_state_changed)
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"item\",\"expression\"]", .field = NULL},
            {.types = "[\"constant\",\"expression\"]", .field = "start"},
            {.types = "[\"constant\",\"expression\"]", .field = "finish"},
            {.types = NULL, .field = NULL},
        };

        ezpi_add_data_src_dest_array_to_object(cj_is_item_state_changed, "dataSource", data_src_obj);
    }

    return cj_is_item_state_changed;
}

static cJSON *ezpi_comparision_method_string_operation(void)
{
    cJSON *cj_string_operation = cJSON_CreateObject(__FUNCTION__);
    if (cj_string_operation)
    {
        cJSON *cj_comparator = cJSON_AddObjectToObject(__FUNCTION__, cj_string_operation, "comparator");
        if (cj_comparator)
        {
            cJSON_AddStringToObject(__FUNCTION__, cj_comparator, "family", "strings");
            cJSON_AddStringToObject(__FUNCTION__, cj_comparator, "field", "comparator");
            cJSON_AddStringToObject(__FUNCTION__, cj_comparator, ezlopi_type_str, ezlopi_enum_str);

            const static char *options_str = "[\"begin\",\"end\",\"contain\",\"length\",\"not_begin\",\"not_end\",\"not_contain\",\"not_length\"]";
            cJSON_AddRawToObject(__FUNCTION__, cj_comparator, "options", options_str);
        }

        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"item\",\"expression\",\"device_group\",\"item_group\"]", .field = NULL},
            {.types = "[\"constant\",\"expression\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };

        ezpi_add_data_src_dest_array_to_object(cj_string_operation, "dataSource", data_src_obj);
    }

    return cj_string_operation;
}

static void ezpi_comparison_methods_list(char *list_name, cJSON *cj_result)
{
    if (cj_result)
    {
        cJSON *cj_comparision_methods = cJSON_AddObjectToObject(__FUNCTION__, cj_result, list_name);
        if (cj_comparision_methods)
        {
            cJSON_AddItemToObject(__FUNCTION__, cj_comparision_methods, ezlopi_info_str, ezpi_comparision_method_info());
            cJSON_AddItemToObject(__FUNCTION__, cj_comparision_methods, "compareNumberRange", ezpi_comparision_method_compare_number_range());
            cJSON_AddItemToObject(__FUNCTION__, cj_comparision_methods, "compareNumbers", ezpi_comparision_method_compare_numbers());
            cJSON_AddItemToObject(__FUNCTION__, cj_comparision_methods, "compareStrings", ezpi_comparision_method_compare_strings());
            cJSON_AddItemToObject(__FUNCTION__, cj_comparision_methods, "compareValues", ezpi_comparision_method_compare_values());
            cJSON_AddItemToObject(__FUNCTION__, cj_comparision_methods, "inArray", ezpi_comparision_method_in_array());
            cJSON_AddItemToObject(__FUNCTION__, cj_comparision_methods, "isDeviceItemGroup", ezpi_comparision_method_is_device_item_group());
            cJSON_AddItemToObject(__FUNCTION__, cj_comparision_methods, "isDeviceState", ezpi_comparision_method_is_device_state());
            cJSON_AddItemToObject(__FUNCTION__, cj_comparision_methods, "isItemState", ezpi_comparision_method_is_item_state());
            cJSON_AddItemToObject(__FUNCTION__, cj_comparision_methods, "isItemStateChanged", ezpi_comparision_method_is_item_state_changed());
            cJSON_AddItemToObject(__FUNCTION__, cj_comparision_methods, "stringOperation", ezpi_comparision_method_string_operation());
        }
    }
}

static cJSON *ezpi_action_methods_info(void)
{
    cJSON *cj_method = cJSON_CreateObject(__FUNCTION__);
    if (cj_method) // Info
    {
        cJSON_AddStringToObject(__FUNCTION__, cj_method, "schema_version", "0.0.1");
    }
    return cj_method;
}
static cJSON *ezpi_action_methods_set_item_value()
{
    cJSON *cj_method = cJSON_CreateObject(__FUNCTION__);
    if (cj_method) // setItemValue
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_method, "dataSource", data_src_obj);
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"item\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"async\"]";
        cJSON_AddRawToObject(__FUNCTION__, cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------

        cJSON_AddStringToObject(__FUNCTION__, cj_method, "typeSystem", "itemValueTypes");
        //-------------------------------------------------------------------------

        static const char *scope_raw_str = "[\"local\",\"global\"]";
        cJSON_AddRawToObject(__FUNCTION__, cj_method, "scope", scope_raw_str);
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *ezpi_action_methods_set_device_armed()
{
    cJSON *cj_method = cJSON_CreateObject(__FUNCTION__);
    if (cj_method) // setDeviceArmed
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_method, "dataSource", data_src_obj);
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"device\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"async\"]";
        cJSON_AddRawToObject(__FUNCTION__, cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------

        static const char *scope_raw_str = "[\"local\",\"global\"]";
        cJSON_AddRawToObject(__FUNCTION__, cj_method, "scope", scope_raw_str);
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *ezpi_action_methods_send_cloud_abstract_command()
{
    cJSON *cj_method = cJSON_CreateObject(__FUNCTION__);
    if (cj_method) // sendCloudAbstractCommand
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"object\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_method, "dataSource", data_src_obj);
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"cloud\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"sync\"]";
        cJSON_AddRawToObject(__FUNCTION__, cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *ezpi_action_methods_switch_house_mode()
{
    cJSON *cj_method = cJSON_CreateObject(__FUNCTION__);
    if (cj_method) // switchHouseMode
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_method, "dataSource", data_src_obj);
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"houseMode\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"async\"]";
        cJSON_AddRawToObject(__FUNCTION__, cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *ezpi_action_methods_send_http_request()
{
    cJSON *cj_method = cJSON_CreateObject(__FUNCTION__);
    if (cj_method) // sendHttpRequest
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_method, "dataSource", data_src_obj);
        //-------------------------------------------------------------------------

        cJSON_AddNullToObject(__FUNCTION__, cj_method, "dataTarget");
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"async\"]";
        cJSON_AddRawToObject(__FUNCTION__, cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t dataTarget_sideEffects_obj[] = {
            {.types = "[\"expression\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        cJSON *cj_side_effects = cJSON_AddArrayToObject(__FUNCTION__, cj_method, "sideEffects");
        if (cj_side_effects)
        {
            cJSON *cj_side_eff_elem = cJSON_CreateObject(__FUNCTION__);
            if (cj_side_eff_elem)
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_side_eff_elem, "action", "saveResult");
                ezpi_add_data_src_dest_array_to_object(cj_side_eff_elem, "dataTarget", dataTarget_sideEffects_obj);

                if (!cJSON_AddItemToArray(cj_side_effects, cj_side_eff_elem))
                {
                    cJSON_Delete(__FUNCTION__, cj_side_eff_elem);
                }
            }
        }
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *ezpi_action_methods_run_custom_script()
{
    cJSON *cj_method = cJSON_CreateObject(__FUNCTION__);
    if (cj_method) // runCustomScript
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_method, "dataSource", data_src_obj);
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"async\"]";
        cJSON_AddRawToObject(__FUNCTION__, cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *ezpi_action_methods_run_plugin_script()
{
    cJSON *cj_method = cJSON_CreateObject(__FUNCTION__);
    if (cj_method) // runPluginScript
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_method, "dataSource", data_src_obj);
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"script\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"sync\"]";
        cJSON_AddRawToObject(__FUNCTION__, cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *ezpi_action_methods_run_scene()
{
    cJSON *cj_method = cJSON_CreateObject(__FUNCTION__);
    if (cj_method) // runScene
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_method, "dataSource", data_src_obj);
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"scene\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"async\"]";
        cJSON_AddRawToObject(__FUNCTION__, cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *ezpi_action_methods_stop_scene()
{
    cJSON *cj_method = cJSON_CreateObject(__FUNCTION__);
    if (cj_method) // stopScene
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_method, "dataSource", data_src_obj);
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"scene\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"async\"]";
        cJSON_AddRawToObject(__FUNCTION__, cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *ezpi_action_methods_set_scene_state()
{
    cJSON *cj_method = cJSON_CreateObject(__FUNCTION__);
    if (cj_method) // setSceneState
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_method, "dataSource", data_src_obj);
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"scene\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"async\"]";
        cJSON_AddRawToObject(__FUNCTION__, cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *ezpi_action_methods_reboot_hub()
{
    cJSON *cj_method = cJSON_CreateObject(__FUNCTION__);
    if (cj_method) // rebootHub
    {
        cJSON_AddNullToObject(__FUNCTION__, cj_method, "dataSource");
        //-------------------------------------------------------------------------
        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------
        static const char *execution_raw_str = "[\"async\"]";
        cJSON_AddRawToObject(__FUNCTION__, cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *ezpi_action_methods_cloud_api()
{
    cJSON *cj_method = cJSON_CreateObject(__FUNCTION__);
    if (cj_method) // cloudAPI
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_method, "dataSource", data_src_obj);
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"async\"]";
        cJSON_AddRawToObject(__FUNCTION__, cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t dataTarget_sideEffects_obj[] = {
            {.types = "[\"expression\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        cJSON *cj_side_effects = cJSON_AddArrayToObject(__FUNCTION__, cj_method, "sideEffects");
        if (cj_side_effects)
        {
            cJSON *cj_side_eff_elem = cJSON_CreateObject(__FUNCTION__);
            if (cj_side_eff_elem)
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_side_eff_elem, "action", "saveResult");
                ezpi_add_data_src_dest_array_to_object(cj_side_eff_elem, "dataTarget", dataTarget_sideEffects_obj);

                if (!cJSON_AddItemToArray(cj_side_effects, cj_side_eff_elem))
                {
                    cJSON_Delete(__FUNCTION__, cj_side_eff_elem);
                }
            }
        }
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *ezpi_action_methods_reset_hub()
{
    cJSON *cj_method = cJSON_CreateObject(__FUNCTION__);
    if (cj_method) // resetHub
    {
        cJSON_AddNullToObject(__FUNCTION__, cj_method, "dataSource");
        //-------------------------------------------------------------------------
        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------
        static const char *execution_raw_str = "[\"async\"]";
        cJSON_AddRawToObject(__FUNCTION__, cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *ezpi_action_methods_reset_latch()
{
    cJSON *cj_method = cJSON_CreateObject(__FUNCTION__);
    if (cj_method) // resetLatch
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_method, "dataSource", data_src_obj);
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"async\"]";
        cJSON_AddRawToObject(__FUNCTION__, cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *ezpi_action_methods_set_variable()
{
    cJSON *cj_method = cJSON_CreateObject(__FUNCTION__);
    if (cj_method) // setVariable
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_method, "dataSource", data_src_obj);
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"expression\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"sync\"]";
        cJSON_AddRawToObject(__FUNCTION__, cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------

        static const char *type_system_raw_str = "[\"itemValueTypes\",\"free\"]";
        cJSON_AddRawToObject(__FUNCTION__, cj_method, "typeSystem", type_system_raw_str);
        //-------------------------------------------------------------------------

        cJSON *cj_side_effects = cJSON_AddArrayToObject(__FUNCTION__, cj_method, "sideEffects");
        if (cj_side_effects)
        {
            cJSON *cj_side_eff_elem = cJSON_CreateObject(__FUNCTION__);
            if (cj_side_eff_elem)
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_side_eff_elem, "action", "saveResult");
                ezpi_add_data_src_dest_array_to_object(cj_side_eff_elem, "dataTarget", data_target_obj);

                if (!cJSON_AddItemToArray(cj_side_effects, cj_side_eff_elem))
                {
                    cJSON_Delete(__FUNCTION__, cj_side_eff_elem);
                }
            }
        }
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *ezpi_action_methods_reset_scene_latches()
{
    cJSON *cj_method = cJSON_CreateObject(__FUNCTION__);
    if (cj_method) // resetSceneLatches
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_method, "dataSource", data_src_obj);
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"async\"]";
        cJSON_AddRawToObject(__FUNCTION__, cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *ezpi_action_methods_set_expression()
{
    cJSON *cj_method = cJSON_CreateObject(__FUNCTION__);
    if (cj_method) // setExpression
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_method, "dataSource", data_src_obj);
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"expression\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"sync\"]";
        cJSON_AddRawToObject(__FUNCTION__, cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------

        static const char *type_system_raw_str = "[\"itemValueTypes\",\"free\"]";
        cJSON_AddRawToObject(__FUNCTION__, cj_method, "typeSystem", type_system_raw_str);
        //-------------------------------------------------------------------------

        cJSON *cj_side_effects = cJSON_AddArrayToObject(__FUNCTION__, cj_method, "sideEffects");
        if (cj_side_effects)
        {
            cJSON *cj_side_eff_elem = cJSON_CreateObject(__FUNCTION__);
            if (cj_side_eff_elem)
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_side_eff_elem, "action", "saveResult");
                ezpi_add_data_src_dest_array_to_object(cj_side_eff_elem, "dataTarget", data_target_obj);

                if (!cJSON_AddItemToArray(cj_side_effects, cj_side_eff_elem))
                {
                    cJSON_Delete(__FUNCTION__, cj_side_eff_elem);
                }
            }
        }
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *ezpi_action_methods_toggle_value()
{
    cJSON *cj_method = cJSON_CreateObject(__FUNCTION__);
    if (cj_method) // toggleValue
    {
        cJSON_AddNullToObject(__FUNCTION__, cj_method, "dataSource");
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"item\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"async\"]";
        cJSON_AddRawToObject(__FUNCTION__, cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------

        cJSON_AddStringToObject(__FUNCTION__, cj_method, "typeSystem", "itemValueTypes");
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *ezpi_action_methods_group_set_item_value()
{
    cJSON *cj_method = cJSON_CreateObject(__FUNCTION__);
    if (cj_method) // groupSetItemValue
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"constant\",\"expression\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_method, "dataSource", data_src_obj);
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"device_group\"]", .field = NULL},
            {.types = "[\"item_group\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"async\"]";
        cJSON_AddRawToObject(__FUNCTION__, cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------

        cJSON_AddStringToObject(__FUNCTION__, cj_method, "typeSystem", "itemValueTypes");
        //-------------------------------------------------------------------------

        static const char *scope_raw_str = "[\"local\"]";
        cJSON_AddRawToObject(__FUNCTION__, cj_method, "scope", scope_raw_str);
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *ezpi_action_methods_group_toggle_value()
{
    cJSON *cj_method = cJSON_CreateObject(__FUNCTION__);
    if (cj_method) // groupToggleValue
    {
        cJSON_AddNullToObject(__FUNCTION__, cj_method, "dataSource");
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"device_group\"]", .field = NULL},
            {.types = "[\"item_group\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"async\"]";
        cJSON_AddRawToObject(__FUNCTION__, cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------

        cJSON_AddStringToObject(__FUNCTION__, cj_method, "typeSystem", "itemValueTypes");
        //-------------------------------------------------------------------------

        static const char *scope_raw_str = "[\"local\"]";
        cJSON_AddRawToObject(__FUNCTION__, cj_method, "scope", scope_raw_str);
        //-------------------------------------------------------------------------
    }
    return cj_method;
}
static cJSON *ezpi_action_methods_group_set_device_armed()
{
    cJSON *cj_method = cJSON_CreateObject(__FUNCTION__);
    if (cj_method) // groupSetDeviceArmed
    {
        static const s_data_source_n_target_object_t data_src_obj[] = {
            {.types = "[\"constant\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_method, "dataSource", data_src_obj);
        //-------------------------------------------------------------------------

        static const s_data_source_n_target_object_t data_target_obj[] = {
            {.types = "[\"device_group\"]", .field = NULL},
            {.types = NULL, .field = NULL},
        };
        ezpi_add_data_src_dest_array_to_object(cj_method, "dataTarget", data_target_obj);
        //-------------------------------------------------------------------------

        static const char *execution_raw_str = "[\"async\"]";
        cJSON_AddRawToObject(__FUNCTION__, cj_method, "execution", execution_raw_str);
        //-------------------------------------------------------------------------

        cJSON_AddStringToObject(__FUNCTION__, cj_method, "typeSystem", "itemValueTypes");
        //-------------------------------------------------------------------------

        static const char *scope_raw_str = "[\"local\"]";
        cJSON_AddRawToObject(__FUNCTION__, cj_method, "scope", scope_raw_str);
        //-------------------------------------------------------------------------
    }
    return cj_method;
}

static void ezpi_action_methods_list(char *list_name, cJSON *cj_result)
{
    if (cj_result)
    {
        cJSON *cj_action_methods = cJSON_AddObjectToObject(__FUNCTION__, cj_result, list_name);
        if (cj_action_methods)
        {
            cJSON_AddItemToObject(__FUNCTION__, cj_action_methods, "Info", ezpi_action_methods_info());
            cJSON_AddItemToObject(__FUNCTION__, cj_action_methods, "setItemValue", ezpi_action_methods_set_item_value());
            cJSON_AddItemToObject(__FUNCTION__, cj_action_methods, "setDeviceArmed", ezpi_action_methods_set_device_armed());
            cJSON_AddItemToObject(__FUNCTION__, cj_action_methods, "sendCloudAbstractCommand", ezpi_action_methods_send_cloud_abstract_command());
            cJSON_AddItemToObject(__FUNCTION__, cj_action_methods, "switchHouseMode", ezpi_action_methods_switch_house_mode());
            cJSON_AddItemToObject(__FUNCTION__, cj_action_methods, "sendHttpRequest", ezpi_action_methods_send_http_request());
            cJSON_AddItemToObject(__FUNCTION__, cj_action_methods, "runCustomScript", ezpi_action_methods_run_custom_script());
            cJSON_AddItemToObject(__FUNCTION__, cj_action_methods, "runPluginScript", ezpi_action_methods_run_plugin_script());
            cJSON_AddItemToObject(__FUNCTION__, cj_action_methods, "runScene", ezpi_action_methods_run_scene());
            cJSON_AddItemToObject(__FUNCTION__, cj_action_methods, "stopScene", ezpi_action_methods_stop_scene());
            cJSON_AddItemToObject(__FUNCTION__, cj_action_methods, "setSceneState", ezpi_action_methods_set_scene_state());
            cJSON_AddItemToObject(__FUNCTION__, cj_action_methods, "rebootHub", ezpi_action_methods_reboot_hub());
            cJSON_AddItemToObject(__FUNCTION__, cj_action_methods, "cloudAPI", ezpi_action_methods_cloud_api());
            cJSON_AddItemToObject(__FUNCTION__, cj_action_methods, "resetHub", ezpi_action_methods_reset_hub());
            cJSON_AddItemToObject(__FUNCTION__, cj_action_methods, "resetLatch", ezpi_action_methods_reset_latch());
            cJSON_AddItemToObject(__FUNCTION__, cj_action_methods, "setVariable", ezpi_action_methods_set_variable());
            cJSON_AddItemToObject(__FUNCTION__, cj_action_methods, "resetSceneLatches", ezpi_action_methods_reset_scene_latches());
            cJSON_AddItemToObject(__FUNCTION__, cj_action_methods, "setExpression", ezpi_action_methods_set_expression());
            cJSON_AddItemToObject(__FUNCTION__, cj_action_methods, "toggleValue", ezpi_action_methods_toggle_value());
            cJSON_AddItemToObject(__FUNCTION__, cj_action_methods, "groupSetItemValue", ezpi_action_methods_group_set_item_value());
            cJSON_AddItemToObject(__FUNCTION__, cj_action_methods, "groupToggleValue", ezpi_action_methods_group_toggle_value());
            cJSON_AddItemToObject(__FUNCTION__, cj_action_methods, "groupSetDeviceArmed", ezpi_action_methods_group_set_device_armed());
        }
    }
}

static void ezpi_advanced_scenes_version_list(char *list_name, cJSON *cj_result)
{
    if (cj_result)
    {
        cJSON *cj_avance_scenes_version = cJSON_AddObjectToObject(__FUNCTION__, cj_result, list_name);
        if (cj_avance_scenes_version)
        {
            cJSON_AddStringToObject(__FUNCTION__, cj_avance_scenes_version, ezlopi_version_str, "Major.Minor");
        }
    }
}

// helper functions

static void ezpi_add_data_src_dest_array_to_object(cJSON *cj_method, char *array_name, const s_data_source_n_target_object_t *data_list)
{
    cJSON *cj_data_source_n_target_list = cJSON_AddArrayToObject(__FUNCTION__, cj_method, array_name);
    if (cj_data_source_n_target_list)
    {
        uint32_t idx = 0;
        while (data_list[idx].types || data_list[idx].field)
        {
            cJSON *cj_arr_object = cJSON_CreateObject(__FUNCTION__);
            if (cj_arr_object)
            {
                cJSON_AddNumberToObject(__FUNCTION__, cj_arr_object, "index", idx);
                if (data_list[idx].types)
                {
                    cJSON_AddRawToObject(__FUNCTION__, cj_arr_object, "types", data_list[idx].types);
                }

                if (data_list[idx].field)
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_arr_object, "field", data_list[idx].field);
                }

                if (!cJSON_AddItemToArray(cj_data_source_n_target_list, cj_arr_object))
                {
                    cJSON_Delete(__FUNCTION__, cj_arr_object);
                }
            }

            idx++;
        }
    }
}
#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
