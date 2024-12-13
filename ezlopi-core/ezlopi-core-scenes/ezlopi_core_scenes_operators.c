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
* @file    ezlopi_core_scenes_operators.c
* @brief   These functions perform comparision operation for scenes
* @author  xx
* @version 0.1
* @date    12th DEC 2024
*/

/*******************************************************************************
*                          Include Files
*******************************************************************************/
#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include <string.h>
#include "cjext.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_core_devices.h"
#include "ezlopi_core_device_group.h"
#include "ezlopi_core_scenes_v2.h"
#include "ezlopi_core_scenes_value.h"
#include "ezlopi_core_scenes_operators.h"
#include "ezlopi_core_scenes_expressions.h"

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
#define OPERATE_ON_STRINGS(STR1, op, STR2) (strncmp(STR1, STR2, BIGGER_LEN(strlen(STR1), strlen(STR2))) op 0) // logical-arrangement of '__ op 0' returns correct results
#define STR_OP_COMP(STR1, op, STR2) \
    ((NULL == STR1)   ? false       \
     : (NULL == STR2) ? false       \
                      : OPERATE_ON_STRINGS(STR1, op, STR2))

typedef enum e_with_grp
{
    COMPARE_NUM_OR_STR,
    COMPARE_NUM_RANGE,
    COMPARE_VALUES,
    COMPARE_INARRAY,
    COMPARE_STROPS,
    COMPARE_INVALID
} e_with_grp_t;

/*******************************************************************************
*                          Static Function Prototypes
*******************************************************************************/
static int __evaluate_compareNumber_or_compareStrings(l_fields_v2_t *item_exp_field, l_fields_v2_t *value_field, char *comparator_str);
static int __evaluate_compareNumbers_or_compareStrings_with_group(l_fields_v2_t *devgrp_field, l_fields_v2_t *itemgrp_field, l_fields_v2_t *value_field, l_fields_v2_t *comparator_field);
/* Numeric & String */
static int ________compare_val_num(double item_exp_value, double value_to_compare_with, char *comparator_field_str);
static int ________compare_val_str(const char *item_exp_value_str, const char *value_to_compare_with_str, char *comparator_field_str);
static int ____compare_exp_vs_other(s_ezlopi_expressions_t *curr_expr_left, l_fields_v2_t *value_field, char *comparator_field_str);
static int ____compare_exp_vs_exp(s_ezlopi_expressions_t *curr_expr_left, s_ezlopi_expressions_t *curr_expr_right, char *comparator_field_str);
static int ____compare_exp_vs_item(s_ezlopi_expressions_t *curr_expr_left, l_ezlopi_item_t *item_right, char *comparator_field_str);
static int ____compare_item_vs_other(l_ezlopi_item_t *item_left, l_fields_v2_t *value_field, char *comparator_field_str);
static int ____compare_item_vs_exp(l_ezlopi_item_t *item_left, s_ezlopi_expressions_t *curr_expr_right, char *comparator_field_str);
static int ____compare_item_vs_item(l_ezlopi_item_t *item_left, l_ezlopi_item_t *item_right, char *comparator_field_str);
static int __evaluate_numrange(l_fields_v2_t *item_exp_field, l_fields_v2_t *start_value_field, l_fields_v2_t *end_value_field, bool comparator_choice);;
static int __evaluate_compareNumber_or_compareStrings(l_fields_v2_t *item_exp_field, l_fields_v2_t *value_field, char *comparator_str);;
static int __evaluate_compareNumbers_or_compareStrings_with_group(l_fields_v2_t *devgrp_field, l_fields_v2_t *itemgrp_field, l_fields_v2_t *value_field, l_fields_v2_t *comparator_field);;
static int __trigger_grp_functions(e_with_grp_t choice, l_fields_v2_t *devgrp_field, l_fields_v2_t *itemgrp_field, l_fields_v2_t *operation_field, l_fields_v2_t *comparator_field, l_fields_v2_t *value_field, l_fields_v2_t *start_value_field, l_fields_v2_t *end_value_field, l_fields_v2_t *value_type_field, char *comparator_str, bool comparator_choice);
/* Strings */
static char *__get_item_strvalue_by_id(uint32_t item_id);
/* Numeric_range */
static int ________compare_numeric_range_num(double extract_data, double start_value_field_num, double end_value_field_num, bool comparator_choice);
static int ________compare_numeric_range_str(const char *extract_data, const char *start_value_field_str, const char *end_value_field_str, bool comparator_choice);
static int ____compare_range_exp_vs_other(s_ezlopi_expressions_t *curr_expr_left, l_fields_v2_t *start_value_field, l_fields_v2_t *end_value_field, bool comparator_choice);
static int ____compare_range_exp_vs_exp(s_ezlopi_expressions_t *curr_expr_left, s_ezlopi_expressions_t *curr_expr_right_start, s_ezlopi_expressions_t *curr_expr_right_end, bool comparator_choice);
static int ____compare_range_item_vs_other(l_ezlopi_item_t *item, l_fields_v2_t *start_value_field, l_fields_v2_t *end_value_field, bool comparator_choice);
static int ____compare_range_item_vs_exp(l_ezlopi_item_t *item, s_ezlopi_expressions_t *curr_expr_right_start, s_ezlopi_expressions_t *curr_expr_right_end, bool comparator_choice);
static int __evaluate_numrange(l_fields_v2_t *item_exp_field, l_fields_v2_t *start_value_field, l_fields_v2_t *end_value_field, bool comparator_choice);
/* String_operations */
static char *__ezlopi_stropr_laststr_comp(const char *haystack, const char *needle);
/* Values in_array */
static int __compare_inarry_str(char *item_exp_value_str, l_fields_v2_t *value_field, bool operation);
static int __compare_inarry_num(double item_exp_value, l_fields_v2_t *value_field, bool operation);
static int __compare_inarry_cj(cJSON *item_exp_value, l_fields_v2_t *value_field, bool operation);
/* compareValues */
static bool __check_valuetypes(const char *lhs_type_str, const char *rhs_type_str, const char *required_type_str);

/*******************************************************************************
*                          Static Data Definitions
*******************************************************************************/
/* Numeric */
static const char *const ezlopi_scenes_num_cmp_operators_op[] = {
#define SCENES_NUM_COMP_OPERATORS(OPERATOR, op, name, method) op,
#include "__operators_macros/__numeric_comparision_operators_macros.h"
#undef SCENES_NUM_COMP_OPERATORS
};
static const char *const ezlopi_scenes_num_cmp_operators_name[] = {
#define SCENES_NUM_COMP_OPERATORS(OPERATOR, op, name, method) name,
#include "__operators_macros/__numeric_comparision_operators_macros.h"
#undef SCENES_NUM_COMP_OPERATORS
};
static const char *const ezlopi_scenes_num_cmp_operators_method[] = {
#define SCENES_NUM_COMP_OPERATORS(OPERATOR, op, name, method) method,
#include "__operators_macros/__numeric_comparision_operators_macros.h"
#undef SCENES_NUM_COMP_OPERATORS
};
/* Strings */
static const char *const ezlopi_scenes_str_cmp_operators_op[] = {
#define SCENES_STRINGS_OPERATORS(OPERATOR, op, name, method) op,
#include "__operators_macros/__strings_comparision_operators_macros.h"
#undef SCENES_STRINGS_OPERATORS
};
static const char *const ezlopi_scenes_str_cmp_operators_name[] = {
#define SCENES_STRINGS_OPERATORS(OPERATOR, op, name, method) name,
#include "__operators_macros/__strings_comparision_operators_macros.h"
#undef SCENES_STRINGS_OPERATORS
};
static const char *const ezlopi_scenes_str_cmp_operators_method[] = {
#define SCENES_STRINGS_OPERATORS(OPERATOR, op, name, method) method,
#include "__operators_macros/__strings_comparision_operators_macros.h"
#undef SCENES_STRINGS_OPERATORS
};
/* String_operations */
static const char *const ezlopi_scenes_strops_cmp_operators_op[] = {
#define SCENES_STROPS_COMP_OPERATORES(OPERATOR, op, name, method) op,
#include "__operators_macros/__string_op_comparision_operators_macros.h"
#undef SCENES_STROPS_COMP_OPERATORES
};
static const char *const ezlopi_scenes_strops_cmp_operators_name[] = {
#define SCENES_STROPS_COMP_OPERATORES(OPERATOR, op, name, method) name,
#include "__operators_macros/__string_op_comparision_operators_macros.h"
#undef SCENES_STROPS_COMP_OPERATORES
};
static const char *const ezlopi_scenes_strops_cmp_operators_method[] = {
#define SCENES_STROPS_COMP_OPERATORES(OPERATOR, op, name, method) method,
#include "__operators_macros/__string_op_comparision_operators_macros.h"
#undef SCENES_STROPS_COMP_OPERATORES
};
/* Values with Less */
static const char *const ezlopi_scenes_value_with_less_cmp_operators_op[] = {
#define SCENES_VALUES_WITH_LESS_OPERATORS(OPERATOR, op, name, method) op,
#include "__operators_macros/__value_with_less_comparision_operators_macros.h"
#undef SCENES_VALUES_WITH_LESS_OPERATORS
};
static const char *const ezlopi_scenes_value_with_less_cmp_operators_name[] = {
#define SCENES_VALUES_WITH_LESS_OPERATORS(OPERATOR, op, name, method) name,
#include "__operators_macros/__value_with_less_comparision_operators_macros.h"
#undef SCENES_VALUES_WITH_LESS_OPERATORS
};
static const char *const ezlopi_scenes_value_with_less_cmp_operators_method[] = {
#define SCENES_VALUES_WITH_LESS_OPERATORS(OPERATOR, op, name, method) method,
#include "__operators_macros/__value_with_less_comparision_operators_macros.h"
#undef SCENES_VALUES_WITH_LESS_OPERATORS
};
/* Values without less */
static const char *const ezlopi_scenes_value_without_less_cmp_operators_op[] = {
#define SCENES_VALUES_WITHOUT_LESS_OPERATORS(OPERATOR, op, name, method) op,
#include "__operators_macros/__value_without_less_comparision_operators_macros.h"
#undef SCENES_VALUES_WITHOUT_LESS_OPERATORS
};
static const char *const ezlopi_scenes_value_without_less_cmp_operators_name[] = {
#define SCENES_VALUES_WITHOUT_LESS_OPERATORS(OPERATOR, op, name, method) name,
#include "__operators_macros/__value_without_less_comparision_operators_macros.h"
#undef SCENES_VALUES_WITHOUT_LESS_OPERATORS
};
static const char *const ezlopi_scenes_value_without_less_cmp_operators_method[] = {
#define SCENES_VALUES_WITHOUT_LESS_OPERATORS(OPERATOR, op, name, method) method,
#include "__operators_macros/__value_without_less_comparision_operators_macros.h"
#undef SCENES_VALUES_WITHOUT_LESS_OPERATORS
};

/*******************************************************************************
*                          Extern Data Definitions
*******************************************************************************/

/*******************************************************************************
*                          Extern Function Definitions
*******************************************************************************/
/* Numeric */
e_scene_num_cmp_operators_t EZPI_scenes_numeric_comparator_operators_get_enum(char *operator_str)
{
    e_scene_num_cmp_operators_t ret = SCENES_NUM_COMP_OPERATORS_NONE + 1;
    if (operator_str)
    {
        while ((ret <= SCENES_NUM_COMP_OPERATORS_MAX) && ezlopi_scenes_num_cmp_operators_op[ret])
        {
            if (0 == strcmp(ezlopi_scenes_num_cmp_operators_op[ret], operator_str))
            {
                break;
            }
            ret++;
        }
    }
    return ret;
}
const char *EZPI_scenes_numeric_comparator_operators_get_op(e_scene_num_cmp_operators_t operator)
{
    const char *ret = NULL;
    if ((operator> SCENES_NUM_COMP_OPERATORS_NONE) && (operator<SCENES_NUM_COMP_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_num_cmp_operators_op[operator];
    }
    return ret;
}
const char *EZPI_scenes_numeric_comparator_operators_get_name(e_scene_num_cmp_operators_t operator)
{
    const char *ret = NULL;
    if ((operator> SCENES_NUM_COMP_OPERATORS_NONE) && (operator<SCENES_NUM_COMP_OPERATORS_MAX))
    {
        // TRACE_D("Name: %s", ezlopi_scenes_num_cmp_operators_name[operator]);
        ret = ezlopi_scenes_num_cmp_operators_name[operator];
    }
    return ret;
}
const char *EZPI_scenes_numeric_comparator_operators_get_method(e_scene_num_cmp_operators_t operator)
{
    const char *ret = NULL;
    if ((operator> SCENES_NUM_COMP_OPERATORS_NONE) && (operator<SCENES_NUM_COMP_OPERATORS_MAX))
    {
        // TRACE_D("Method: %s", ezlopi_scenes_operators_method[operator]);
        ret = ezlopi_scenes_num_cmp_operators_method[operator];
    }
    return ret;
}
int EZPI_scenes_operators_value_number_operations(l_fields_v2_t *item_exp_field, l_fields_v2_t *value_field, l_fields_v2_t *comparator_field, l_fields_v2_t *devgrp_field, l_fields_v2_t *itemgrp_field)
{
    int ret = 0;
    if (devgrp_field && itemgrp_field && value_field && comparator_field)
    {
        ret = __evaluate_compareNumbers_or_compareStrings_with_group(devgrp_field, itemgrp_field, value_field, comparator_field);
    }
    else
    {
        ret = __evaluate_compareNumber_or_compareStrings(item_exp_field, value_field, comparator_field->field_value.u_value.value_string);
    }
    return ret;
}
/* Numeric_Range */
int EZPI_scenes_operators_value_number_range_operations(l_fields_v2_t *item_exp_field, l_fields_v2_t *start_value_field, l_fields_v2_t *end_value_field, bool comparator_choice)
{
    return __evaluate_numrange(item_exp_field, start_value_field, end_value_field, comparator_choice);
}
int EZPI_scenes_operators_value_number_range_operations_with_group(l_fields_v2_t *devgrp_field, l_fields_v2_t *itemgrp_field, l_fields_v2_t *start_value_field, l_fields_v2_t *end_value_field, bool comparator_choice)
{
    return __trigger_grp_functions(COMPARE_NUM_RANGE, devgrp_field, itemgrp_field, NULL, NULL, NULL, start_value_field, end_value_field, NULL, NULL, comparator_choice);
}
/* Strings */
e_scene_str_cmp_operators_t EZPI_scenes_strings_comparator_operators_get_enum(char *operator_str)
{
    e_scene_str_cmp_operators_t ret = SCENES_STRINGS_OPERATORS_NONE + 1;
    if (operator_str)
    {
        while ((ret <= SCENES_STRINGS_OPERATORS_MAX) && ezlopi_scenes_str_cmp_operators_op[ret])
        {
            if (0 == strcmp(ezlopi_scenes_str_cmp_operators_op[ret], operator_str))
            {
                break;
            }

            ret++;
        }
    }

    return ret;
}
const char *EZPI_scenes_strings_comparator_operators_get_op(e_scene_str_cmp_operators_t operator)
{
    const char *ret = NULL;
    if ((operator>= SCENES_STRINGS_OPERATORS_NONE) && (operator<SCENES_STRINGS_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_str_cmp_operators_op[operator];
    }
    return ret;
}
const char *EZPI_scenes_strings_comparator_operators_get_name(e_scene_str_cmp_operators_t operator)
{
    const char *ret = NULL;
    if ((operator> SCENES_STRINGS_OPERATORS_NONE) && (operator<SCENES_STRINGS_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_str_cmp_operators_name[operator];
    }
    return ret;
}
const char *EZPI_scenes_strings_comparator_operators_get_method(e_scene_str_cmp_operators_t operator)
{
    const char *ret = NULL;
    if ((operator> SCENES_STRINGS_OPERATORS_NONE) && (operator<SCENES_STRINGS_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_str_cmp_operators_method[operator];
    }
    return ret;
}
/* Compare_string */
int EZPI_scenes_operators_value_strings_operations(l_fields_v2_t *devgrp_field, l_fields_v2_t *itemgrp_field, l_fields_v2_t *item_exp_field, l_fields_v2_t *value_field, l_fields_v2_t *comparator_field)
{
    int ret = 0;
    if (devgrp_field && itemgrp_field && value_field && comparator_field)
    {
        ret = __evaluate_compareNumbers_or_compareStrings_with_group(devgrp_field, itemgrp_field, value_field, comparator_field);
    }
    else
    {
        ret = __evaluate_compareNumber_or_compareStrings(item_exp_field, value_field, comparator_field->field_value.u_value.value_string);
    }

    return ret;
}
/* String_operations */
e_scene_strops_cmp_operators_t EZPI_scenes_strops_comparator_operators_get_enum(char *operator_str)
{
    e_scene_strops_cmp_operators_t ret = SCENES_STROPS_COMP_OPERATORES_NONE + 1;
    if (operator_str)
    {
        while ((ret <= SCENES_STROPS_COMP_OPERATORES_MAX) && ezlopi_scenes_strops_cmp_operators_op[ret])
        {
            if (0 == strcmp(ezlopi_scenes_strops_cmp_operators_op[ret], operator_str))
            {
                break;
            }

            ret++;
        }
    }
    return ret;
}
const char *EZPI_scenes_strops_comparator_operators_get_op(e_scene_strops_cmp_operators_t operator)
{
    const char *ret = NULL;
    if ((operator>= SCENES_STROPS_COMP_OPERATORES_NONE) && (operator<SCENES_STROPS_COMP_OPERATORES_MAX))
    {
        ret = ezlopi_scenes_strops_cmp_operators_op[operator];
    }
    return ret;
}
const char *EZPI_scenes_strops_comparator_operators_get_name(e_scene_strops_cmp_operators_t operator)
{
    const char *ret = NULL;
    if ((operator> SCENES_STROPS_COMP_OPERATORES_NONE) && (operator<SCENES_STROPS_COMP_OPERATORES_MAX))
    {
        ret = ezlopi_scenes_strops_cmp_operators_name[operator];
    }
    return ret;
}
const char *EZPI_scenes_strops_comparator_operators_get_method(e_scene_strops_cmp_operators_t operator)
{
    const char *ret = NULL;
    if ((operator> SCENES_STROPS_COMP_OPERATORES_NONE) && (operator<SCENES_STROPS_COMP_OPERATORES_MAX))
    {
        ret = ezlopi_scenes_strops_cmp_operators_method[operator];
    }
    return ret;
}
int EZPI_scenes_operators_value_strops_operations(l_fields_v2_t *item_exp_field, l_fields_v2_t *value_field, l_fields_v2_t *operation_field)
{
    int ret = 0;
    if (item_exp_field && value_field && (operation_field->field_value.u_value.value_string))
    {
        char *item_exp_value_str = NULL;
        char *value_to_compare_with_str = NULL;
        uint32_t value_to_compare_with_num = 0;

        // 1 . LHS => expression (only string values )
        if (EZLOPI_VALUE_TYPE_EXPRESSION == item_exp_field->value_type)
        {
            s_ezlopi_expressions_t *curr_expr_left = EZPI_scenes_expressions_get_node_by_name(item_exp_field->field_value.u_value.value_string);
            if (curr_expr_left)
            {
                if (EZLOPI_VALUE_TYPE_STRING == curr_expr_left->value_type)
                {
                    item_exp_value_str = curr_expr_left->exp_value.u_value.str_value;
                }
                else
                {
                    TRACE_E("Expression doesnot have string_value ; [item_exp_value_str => %s]", item_exp_value_str);
                }
            }
        }
        else // 1. LHS = item_value  (only string values )
        {
            uint32_t item_id = strtoul(item_exp_field->field_value.u_value.value_string, NULL, 16);
            item_exp_value_str = __get_item_strvalue_by_id(item_id);
        }

        //---------------------- RHS (can only be INT_value ; if operation => 'length & not_length' ) -------------------------
        char *tmp_str = operation_field->field_value.u_value.value_string;
        size_t len = (tmp_str) ? strlen(tmp_str) + 1 : 0;
        if (((EZPI_STRNCMP_IF_EQUAL("length", tmp_str, 8, len)) ||
            (EZPI_STRNCMP_IF_EQUAL("not_length", tmp_str, 8, len))) &&
            EZLOPI_VALUE_TYPE_INT == value_field->value_type)
        {
            value_to_compare_with_num = value_field->field_value.u_value.value_double;
        }
        else if (EZLOPI_VALUE_TYPE_STRING == value_field->value_type)
        {
            value_to_compare_with_str = value_field->field_value.u_value.value_string;
        }

        //-------------- ITEM/EXPN must only have string as value ---------------------------
        if (item_exp_value_str && ((NULL != value_to_compare_with_str) || (value_to_compare_with_num > 0)))
        {
            e_scene_strops_cmp_operators_t strops_operator = EZPI_scenes_strops_comparator_operators_get_enum(tmp_str);
            switch (strops_operator)
            {
            case SCENES_STROPS_COMP_OPERATORES_BEGINS_WITH:
            {
                char *str_pos = strstr(item_exp_value_str, value_to_compare_with_str); // finds out the position of first occurance
                if (str_pos)
                {
                    int diff = (str_pos - item_exp_value_str);
                    ret = (0 == diff) ? 1 : 0;
                }
                break;
            }
            case SCENES_STROPS_COMP_OPERATORES_NOT_BEGIN:
            {
                char *str_pos = strstr(item_exp_value_str, value_to_compare_with_str); // finds out the position of first occurance
                if (str_pos)
                {
                    int diff = (str_pos - item_exp_value_str);
                    ret = (0 < diff) ? 1 : 0;
                }
                break;
            }
            case SCENES_STROPS_COMP_OPERATORES_CONTAINS:
            {
                char *str_pos = strstr(item_exp_value_str, value_to_compare_with_str); // finds out the position of first occurance
                if (str_pos)
                {
                    int diff = (str_pos - item_exp_value_str);
                    ret = (diff <= 0) ? 1 : 0;
                }
                break;
            }
            case SCENES_STROPS_COMP_OPERATORES_NOT_CONTAIN:
            {
                char *str_pos = strstr(item_exp_value_str, value_to_compare_with_str); // finds out the position of first occurance
                if (NULL == str_pos)
                {
                    ret = 1;
                }
                break;
            }
            case SCENES_STROPS_COMP_OPERATORES_ENDS_WITH:
            {
                char *last = __ezlopi_stropr_laststr_comp(item_exp_value_str, value_to_compare_with_str); // finds out the position of last occurance
                if (NULL != last)
                {
                    ret = (strlen(value_to_compare_with_str) == strlen(last)) ? 1 : 0;
                }
                break;
            }
            case SCENES_STROPS_COMP_OPERATORES_NOT_END:
            {
                char *last = __ezlopi_stropr_laststr_comp(item_exp_value_str, value_to_compare_with_str); // finds out the position of last occurance
                if (NULL != last)
                {
                    ret = (strlen(value_to_compare_with_str) != strlen(last)) ? 1 : 0;
                }
                break;
            }
            case SCENES_STROPS_COMP_OPERATORES_LENGTH:
            {
                ret = (value_to_compare_with_num == strlen(item_exp_value_str)); // int value comparision
                break;
            }
            case SCENES_STROPS_COMP_OPERATORES_NOT_LENGTH:
            {
                ret = (value_to_compare_with_num != strlen(item_exp_value_str)); // int value comparision
                break;
            }
            default:
            {
                TRACE_E("'SCENES_STROPS_COMP_OPERATORES_* [%d]' out of range!", strops_operator);
                break;
            }
            }
        }
    }

    return ret;
}
int EZPI_scenes_operators_value_strops_operations_with_group(l_fields_v2_t *value_field, l_fields_v2_t *operation_field, l_fields_v2_t *devgrp_field, l_fields_v2_t *itemgrp_field)
{
    return __trigger_grp_functions(COMPARE_STROPS, devgrp_field, itemgrp_field, operation_field, NULL, value_field, NULL, NULL, NULL, NULL, 0);
}
/* Values in_array */
#if 0 /* Currently not in use --> check 'in_array' when-condition before removing */
static const char *const ezlopi_scenes_inarr_cmp_operators_op[] = {
#define SCENES_IN_ARRAY_OPERATORS(OPERATOR, op, name, method) op,
#include "__operators_macros/__in_array_comparision_operators_macros.h"
#undef SCENES_IN_ARRAY_OPERATORS
};
static const char *const ezlopi_scenes_inarr_cmp_operators_name[] = {
#define SCENES_IN_ARRAY_OPERATORS(OPERATOR, op, name, method) name,
#include "__operators_macros/__in_array_comparision_operators_macros.h"
#undef SCENES_IN_ARRAY_OPERATORS
};
static const char *const ezlopi_scenes_inarr_cmp_operators_method[] = {
#define SCENES_IN_ARRAY_OPERATORS(OPERATOR, op, name, method) method,
#include "__operators_macros/__in_array_comparision_operators_macros.h"
#undef SCENES_IN_ARRAY_OPERATORS
};

e_scene_inarr_cmp_operators_t ezlopi_scenes_inarr_comparator_operators_get_enum(char *operator_inarr)
{
    e_scene_inarr_cmp_operators_t ret = SCENES_IN_ARRAY_OPERATORS_NONE + 1;
    if (operator_inarr)
    {
        while ((ret <= SCENES_IN_ARRAY_OPERATORS_MAX) && ezlopi_scenes_inarr_cmp_operators_op[ret])
        {
            if (0 == strcmp(ezlopi_scenes_inarr_cmp_operators_op[ret], operator_inarr))
            {
                break;
            }
            ret++;
        }
    }
    return ret;
}

const char *ezlopi_scenes_inarr_comparator_operators_get_op(e_scene_inarr_cmp_operators_t operator)
{
    const char *ret = NULL;
    if ((operator>= SCENES_IN_ARRAY_OPERATORS_NONE) && (operator<SCENES_IN_ARRAY_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_inarr_cmp_operators_op[operator];
    }
    return ret;
}
const char *ezlopi_scenes_inarr_comparator_operators_get_name(e_scene_inarr_cmp_operators_t operator)
{
    const char *ret = NULL;
    if ((operator>= SCENES_IN_ARRAY_OPERATORS_NONE) && (operator<SCENES_IN_ARRAY_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_inarr_cmp_operators_name[operator];
    }
    return ret;
}
const char *ezlopi_scenes_inarr_comparator_operators_get_method(e_scene_inarr_cmp_operators_t operator)
{
    const char *ret = NULL;
    if ((operator>= SCENES_IN_ARRAY_OPERATORS_NONE) && (operator<SCENES_IN_ARRAY_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_inarr_cmp_operators_method[operator];
    }
    return ret;
}
#endif
int EZPI_scenes_operators_value_inarr_operations(l_fields_v2_t *item_exp_field, l_fields_v2_t *value_field, l_fields_v2_t *operation_field)
{
    int ret = 0;
    if (item_exp_field && value_field && (operation_field->field_value.u_value.value_string))
    {
        char *tmp_str = operation_field->field_value.u_value.value_string;
        size_t len = (tmp_str) ? strlen(tmp_str) + 1 : 0;
        bool operation = (EZPI_STRNCMP_IF_EQUAL(tmp_str, "not_in", len, 7)) ? 1 : 0; // default = 0 [IN]
        //------------------------------------------------
        if (EZLOPI_VALUE_TYPE_EXPRESSION == item_exp_field->value_type)
        {
            s_ezlopi_expressions_t *curr_expr_left = EZPI_scenes_expressions_get_node_by_name(item_exp_field->field_value.u_value.value_string);
            if (curr_expr_left)
            {
                switch (curr_expr_left->exp_value.type)
                {
                case EXPRESSION_VALUE_TYPE_STRING:
                    ret = __compare_inarry_str(curr_expr_left->exp_value.u_value.str_value, value_field, operation);
                    break;
                case EXPRESSION_VALUE_TYPE_CJ:
                    ret = __compare_inarry_cj(curr_expr_left->exp_value.u_value.cj_value, value_field, operation);
                    break;
                case EXPRESSION_VALUE_TYPE_NUMBER:
                    ret = __compare_inarry_num(curr_expr_left->exp_value.u_value.number_value, value_field, operation);
                    break;
                case EXPRESSION_VALUE_TYPE_BOOL:
                    ret = __compare_inarry_num((double)curr_expr_left->exp_value.u_value.boolean_value, value_field, operation);
                    break;
                default:
                    break;
                }
            }
        }
        else
        {
            uint32_t item_id = strtoul(item_exp_field->field_value.u_value.value_string, NULL, 16);
            l_ezlopi_item_t *item = ezlopi_device_get_item_by_id(item_id);
            if (item)
            {
                cJSON *cj_item = cJSON_CreateObject(__FUNCTION__);
                if (cj_item)
                {
                    item->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, item, (void *)cj_item, NULL);
                    cJSON *cj_item_value = cJSON_GetObjectItem(__FUNCTION__, cj_item, ezlopi_value_str);
                    if (cj_item_value)
                    {
                        switch (cj_item_value->type)
                        {
                        case cJSON_String:
                            ret = __compare_inarry_str(cj_item_value->valuestring, value_field, operation);
                            break;
                        case cJSON_Object:
                            ret = __compare_inarry_cj(cj_item_value, value_field, operation);
                            break;
                        case cJSON_Number:
                            ret = __compare_inarry_num(cj_item_value->valuedouble, value_field, operation);
                            break;
                        case cJSON_True:
                        case cJSON_False:
                            ret = __compare_inarry_num((cJSON_True == cj_item_value->type) ? 1 : 0, value_field, operation);
                            break;
                        default:
                            break;
                        }
                    }
                    cJSON_Delete(__FUNCTION__, cj_item);
                }
            }
        }
    }

    return ret;
}
int EZPI_scenes_operators_value_inarr_operations_with_group(l_fields_v2_t *value_field, l_fields_v2_t *operation_field, l_fields_v2_t *devgrp_field, l_fields_v2_t *itemgrp_field)
{
    return __trigger_grp_functions(COMPARE_INARRAY, devgrp_field, itemgrp_field, operation_field, NULL, value_field, NULL, NULL, NULL, NULL, 0);
}
/* Values with Less */
const char *EZPI_scenes_value_with_less_comparator_operators_get_op(e_scene_value_with_less_cmp_operators_t operator)
{
    const char *ret = NULL;
    if ((operator>= SCENES_VALUES_WITH_LESS_OPERATORS_NONE) && (operator<SCENES_VALUES_WITH_LESS_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_value_with_less_cmp_operators_op[operator];
    }
    return ret;
}
const char *EZPI_scenes_value_with_less_comparator_operators_get_name(e_scene_value_with_less_cmp_operators_t operator)
{
    const char *ret = NULL;
    if ((operator> SCENES_VALUES_WITH_LESS_OPERATORS_NONE) && (operator<SCENES_VALUES_WITH_LESS_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_value_with_less_cmp_operators_name[operator];
    }
    return ret;
}
const char *EZPI_scenes_value_with_less_comparator_operators_get_method(e_scene_value_with_less_cmp_operators_t operator)
{
    const char *ret = NULL;
    if ((operator> SCENES_VALUES_WITH_LESS_OPERATORS_NONE) && (operator<SCENES_VALUES_WITH_LESS_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_value_with_less_cmp_operators_method[operator];
    }
    return ret;
}
#if 0
e_scene_value_with_less_cmp_operators_t EZPI_scenes_value_with_less_comparator_operators_get_enum(char *operator_str)
{
    e_scene_value_with_less_cmp_operators_t ret = SCENES_VALUES_WITH_LESS_OPERATORS_NONE + 1;
    if (operator_str)
    {
        while ((ret <= SCENES_VALUES_WITH_LESS_OPERATORS_MAX) && ezlopi_scenes_value_with_less_cmp_operators_op[ret])
        {
            if (0 == strcmp(ezlopi_scenes_value_with_less_cmp_operators_op[ret], operator_str))
            {
                break;
            }

            ret++;
        }
    }

    return ret;
}
int ezlopi_scenes_operators_value_with_less_operations(uint32_t item_id, l_fields_v2_t *value_field, l_fields_v2_t *comparator_field)
{
    int ret = 0;
    if (item_id && value_field && comparator_field)
    {
        double item_value = 0.0;
        cJSON *cj_item_value = cJSON_CreateObject(__FUNCTION__);
        l_ezlopi_device_t *device = ezlopi_device_get_head();
        while (device)
        {
            l_ezlopi_item_t *item = device->items;
            while (item)
            {
                if (item->cloud_properties.item_id == item_id)
                {
                    cJSON *cj_item_value = cJSON_CreateObject(__FUNCTION__);
                    if (cj_item_value)
                    {
                        item->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, item, (void *)cj_item_value, NULL);
                        cJSON *cj_value = cJSON_GetObjectItem(__FUNCTION__, cj_item_value, ezlopi_value_str);
                        if (cj_value)
                        {
                            #warning "Krishna needs to complete this"
                                item_value = cj_value->valuedouble;
                        }
                        cJSON_Delete(cj_item_value);
                    }
                    break;
                }
                item = item->next;
            }
            device = device->next;
        }

        e_scene_value_with_less_cmp_operators_t value_with_less_operator = EZPI_scenes_value_with_less_comparator_operators_get_enum(comparator_field->field_value.u_value.value_string);

        switch (value_with_less_operator)
        {
        case SCENES_VALUES_WITH_LESS_OPERATORS_LESS:
        {
            TRACE_W("'SCENES_VALUES_WITH_LESS_OPERATORS_LESS' not implemented!");
            break;
        }
        case SCENES_VALUES_WITH_LESS_OPERATORS_GREATER:
        {
            TRACE_W("'SCENES_VALUES_WITH_LESS_OPERATORS_GREATER' not implemented!");
            break;
        }
        case SCENES_VALUES_WITH_LESS_OPERATORS_LESS_EQUAL:
        {
            TRACE_W("'SCENES_VALUES_WITH_LESS_OPERATORS_LESS_EQUAL' not implemented!");
            break;
        }
        case SCENES_VALUES_WITH_LESS_OPERATORS_GREATER_EQUAL:
        {
            TRACE_W("'SCENES_VALUES_WITH_LESS_OPERATORS_GREATER_EQUAL' not implemented!");
            break;
        }
        case SCENES_VALUES_WITH_LESS_OPERATORS_EQUAL:
        {
            TRACE_W("'SCENES_VALUES_WITH_LESS_OPERATORS_EQUAL' not implemented!");
            break;
        }
        case SCENES_VALUES_WITH_LESS_OPERATORS_NOT_EQUAL:
        {
            TRACE_W("'SCENES_VALUES_WITH_LESS_OPERATORS_NOT_EQUAL' not implemented!");
            break;
        }
        default:
        {
            TRACE_E("'SCENES_VALUES_WITH_LESS_OPERATORS_* [%d]' out of range!", value_with_less_operator);
            break;
        }
        }

        if (cj_item_value)
        {
            cJSON_Delete(__FUNCTION__, cj_item_value);
        }
    }

    return ret;
}
#endif
/* Values without Less */
const char *EZPI_scenes_value_without_less_comparator_operators_get_op(e_scene_value_without_less_cmp_operators_t operator)
{
    const char *ret = NULL;
    if ((operator>= SCENES_VALUES_WITHOUT_LESS_OPERATORS_NONE) && (operator<SCENES_VALUES_WITHOUT_LESS_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_value_without_less_cmp_operators_op[operator];
    }
    return ret;
}
const char *EZPI_scenes_value_without_less_comparator_operators_get_name(e_scene_value_without_less_cmp_operators_t operator)
{
    const char *ret = NULL;
    if ((operator> SCENES_VALUES_WITHOUT_LESS_OPERATORS_NONE) && (operator<SCENES_VALUES_WITHOUT_LESS_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_value_without_less_cmp_operators_name[operator];
    }
    return ret;
}
const char *EZPI_scenes_value_without_less_comparator_operators_get_method(e_scene_value_without_less_cmp_operators_t operator)
{
    const char *ret = NULL;
    if ((operator> SCENES_VALUES_WITHOUT_LESS_OPERATORS_NONE) && (operator<SCENES_VALUES_WITHOUT_LESS_OPERATORS_MAX))
    {
        ret = ezlopi_scenes_value_without_less_cmp_operators_method[operator];
    }
    return ret;
}
#if 0
e_scene_value_without_less_cmp_operators_t EZPI_scenes_value_without_less_comparator_operators_get_enum(char *operator_str)
{
    e_scene_value_without_less_cmp_operators_t ret = SCENES_VALUES_WITHOUT_LESS_OPERATORS_NONE + 1; // 1st element of enum after *_NONE
    if (operator_str)
    {
        while ((ret <= SCENES_VALUES_WITHOUT_LESS_OPERATORS_MAX) && ezlopi_scenes_value_without_less_cmp_operators_op[ret])
        {
            if (0 == strcmp(ezlopi_scenes_value_without_less_cmp_operators_op[ret], operator_str))
            {
                break;
            }

            ret++;
        }
    }

    return ret;
}

int ezlopi_scenes_operators_value_without_less_operations(uint32_t item_id, l_fields_v2_t *value_field, l_fields_v2_t *comparator_field)
{
    int ret = 0;
    if (item_id && value_field && comparator_field)
    {
        double item_value = 0.0;
        cJSON *cj_item_value = cJSON_CreateObject(__FUNCTION__);
        l_ezlopi_device_t *device = ezlopi_device_get_head();
        while (device)
        {
            l_ezlopi_item_t *item = device->items;
            while (item)
            {
                if (item->cloud_properties.item_id == item_id)
                {
                    cJSON *cj_item_value = cJSON_CreateObject(__FUNCTION__);
                    if (cj_item_value)
                    {
                        item->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, item, (void *)cj_item_value, NULL);
                        cJSON *cj_value = cJSON_GetObjectItem(__FUNCTION__, cj_item_value, ezlopi_value_str);
                        if (cj_value)
                        {
                            #warning "Krishna needs to complete this"
                                item_value = cj_value->valuedouble;
                        }
                        cJSON_Delete(cj_item_value);
                    }
                    break;
                }

                item = item->next;
            }

            device = device->next;
        }

        e_scene_value_without_less_cmp_operators_t value_without_less_operator = EZPI_scenes_value_without_less_comparator_operators_get_enum(comparator_field->field_value.u_value.value_string);

        switch (value_without_less_operator)
        {
        case SCENES_VALUES_WITHOUT_LESS_OPERATORS_EQUAL:
        {
            TRACE_W("'SCENES_VALUES_WITHOUT_LESS_OPERATORS_EQUAL' not implemented!");
            break;
        }
        case SCENES_VALUES_WITHOUT_LESS_OPERATORS_NOT_EQUAL:
        {
            TRACE_W("'SCENES_VALUES_WITHOUT_LESS_OPERATORS_NOT_EQUAL' not implemented!");
            break;
        }
        default:
        {
            TRACE_E("'SCENES_VALUES_WITHOUT_LESS_OPERATORS_* [%d]' out of range!", value_without_less_operator);
            break;
        }
        }

        if (cj_item_value)
        {
            cJSON_Delete(__FUNCTION__, cj_item_value);
        }
    }

    return ret;
}

/* compareValues */

int EZPI_scenes_operators_value_comparevalues_without_less_operations(uint32_t item_id, l_fields_v2_t *value_field, l_fields_v2_t *value_type_field, l_fields_v2_t *comparator_field)
{
    int ret = 0;
    if (item_id && value_field && value_type_field && comparator_field)
    {
        cJSON *item_value = NULL;

        l_ezlopi_device_t *device = ezlopi_device_get_head();
        while (device)
        {
            l_ezlopi_item_t *item = device->items;
            while (item)
            {
                if (item->cloud_properties.item_id == item_id)
                {
                    if (STR_OP_COMP(value_type_field->field_value.u_value.value_string, == , item->cloud_properties.value_type)) // bool == bool?
                    {
                        cJSON *cj_item_value = cJSON_CreateObject(__FUNCTION__);
                        if (cj_item_value)
                        {
                            item->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, item, (void *)cj_item_value, NULL);
                            item_value = cJSON_GetObjectItem(__FUNCTION__, cj_item_value, ezlopi_value_str); // eg. double_type : 5.005  or string_type : "5.005"
                            cJSON_Delete(__FUNCTION__, cj_item_value);
                        }
                    }
                    else
                    {
                        ret = 0; // SCENES_WHEN_TYPE_MISMATCH error
                    }
                    break;
                }
                item = item->next;
            }
            device = device->next;
        }

        if (NULL != item_value)
        { // operating according to 'with-less or without-less' comparator
            char *op_str = (NULL == comparator_field) ? "==" : comparator_field->field_value.u_value.value_string;
            e_scene_value_without_less_cmp_operators_t value_without_less_operator = EZPI_scenes_value_without_less_comparator_operators_get_enum(op_str);

            switch (value_without_less_operator)
            {
            case SCENES_VALUES_WITHOUT_LESS_OPERATORS_EQUAL:
            {

                ret = ((item_value->type == cJSON_True) ? (true == value_field->field_value.u_value.value_bool)
                    : (item_value->type == cJSON_False) ? (false == value_field->field_value.u_value.value_bool)
                    : (item_value->type == cJSON_Number) ? (item_value->valuedouble == value_field->field_value.u_value.value_double)
                    : (item_value->type == cJSON_String) ? STR_OP_COMP(item_value->valuestring, == , value_field->field_value.u_value.value_string)
                    : 0);

                break;
            }
            case SCENES_VALUES_WITHOUT_LESS_OPERATORS_NOT_EQUAL:
            {

                ret = ((item_value->type == cJSON_True) ? (true != value_field->field_value.u_value.value_bool)
                    : (item_value->type == cJSON_False) ? (false != value_field->field_value.u_value.value_bool)
                    : (item_value->type == cJSON_Number) ? (item_value->valuedouble != value_field->field_value.u_value.value_double)
                    : (item_value->type == cJSON_String) ? STR_OP_COMP(item_value->valuestring, != , value_field->field_value.u_value.value_string)
                    : 0);

                break;
            }

            default:
            {
                TRACE_E("'SCENES_VALUES_WITHOUT_LESS_OPERATORS_* [%d]' out of range!", value_without_less_operator);
                break;
            }
            }
        }


    }

    return ret;
}
#endif
int EZPI_scenes_operators_value_comparevalues_with_less_operations(l_fields_v2_t *item_exp_field, l_fields_v2_t *value_field, l_fields_v2_t *value_type_field, l_fields_v2_t *comparator_field)
{
    int ret = 0;
    char *comparator_field_str = (NULL == comparator_field) ? "==" : comparator_field->field_value.u_value.value_string; // default '=='
    if (item_exp_field && value_field && value_type_field && comparator_field_str)
    {
        // 1. LHS = expression
        if (EZLOPI_VALUE_TYPE_EXPRESSION == item_exp_field->value_type)
        {
            s_ezlopi_expressions_t *curr_expr_left = EZPI_scenes_expressions_get_node_by_name(item_exp_field->field_value.u_value.value_string);
            if (curr_expr_left)
            {
                if (EZLOPI_VALUE_TYPE_EXPRESSION == value_field->value_type)
                { // 2. exp vs exp
                    s_ezlopi_expressions_t *curr_expr_right = EZPI_scenes_expressions_get_node_by_name(value_field->field_value.u_value.value_string);
                    if (curr_expr_right)
                    { // check if all have same data-type
                        if (__check_valuetypes(ezlopi_scene_get_scene_value_type_name(curr_expr_left->value_type),
                            ezlopi_scene_get_scene_value_type_name(curr_expr_right->value_type),
                            value_type_field->field_value.u_value.value_string))
                        {
                            ret = ____compare_exp_vs_exp(curr_expr_left, curr_expr_right, comparator_field_str);
                        }
                    }
                }
                else if (EZLOPI_VALUE_TYPE_ITEM == value_field->value_type)
                { // 2. exp vs item
                    uint32_t item_id = strtoul(value_field->field_value.u_value.value_string, NULL, 16);
                    l_ezlopi_item_t *item_right = ezlopi_device_get_item_by_id(item_id);
                    if (item_right)
                    {
                        if (__check_valuetypes(ezlopi_scene_get_scene_value_type_name(curr_expr_left->value_type),
                            item_right->cloud_properties.value_type,
                            value_type_field->field_value.u_value.value_string))
                        {
                            ret = ____compare_exp_vs_item(curr_expr_left, item_right, comparator_field_str);
                        }
                    }
                }
                else
                { // 2. exp vs other
                    if (__check_valuetypes(ezlopi_scene_get_scene_value_type_name(curr_expr_left->value_type),
                        ezlopi_scene_get_scene_value_type_name(value_field->value_type),
                        value_type_field->field_value.u_value.value_string))
                    {
                        ret = ____compare_exp_vs_other(curr_expr_left, value_field, comparator_field_str);
                    }
                }
            }
        }
        else
        { // 1. LHS = item
            uint32_t item_id = strtoul(item_exp_field->field_value.u_value.value_string, NULL, 16);
            l_ezlopi_item_t *item_left = ezlopi_device_get_item_by_id(item_id);
            if (item_left)
            {
                if (EZLOPI_VALUE_TYPE_EXPRESSION == value_field->value_type)
                { // 2. RHS = expression
                    s_ezlopi_expressions_t *curr_expr_right = EZPI_scenes_expressions_get_node_by_name(value_field->field_value.u_value.value_string);
                    if (curr_expr_right)
                    {
                        if (__check_valuetypes(item_left->cloud_properties.value_type,
                            ezlopi_scene_get_scene_value_type_name(curr_expr_right->value_type),
                            value_type_field->field_value.u_value.value_string))
                        {
                            ret = ____compare_item_vs_exp(item_left, curr_expr_right, comparator_field_str);
                        }
                    }
                }
                else if (EZLOPI_VALUE_TYPE_ITEM == value_field->value_type)
                { // 2. RHS = item_right
                    uint32_t item_id = strtoul(value_field->field_value.u_value.value_string, NULL, 16);
                    l_ezlopi_item_t *item_right = ezlopi_device_get_item_by_id(item_id);
                    if (item_right)
                    {
                        if (__check_valuetypes(item_left->cloud_properties.value_type,
                            item_right->cloud_properties.value_type,
                            value_type_field->field_value.u_value.value_string))
                        {
                            ret = ____compare_item_vs_item(item_left, item_right, comparator_field_str);
                        }
                    }
                }
                else
                { // 2. RHS = other
                    if (__check_valuetypes(item_left->cloud_properties.value_type,
                        ezlopi_scene_get_scene_value_type_name(value_field->value_type),
                        value_type_field->field_value.u_value.value_string))
                    {
                        ret = ____compare_item_vs_other(item_left, value_field, comparator_field_str);
                    }
                }
            }
        }
    }
    return ret;
}
int EZPI_scenes_operators_value_comparevalues_with_less_operations_with_group(l_fields_v2_t *value_field, l_fields_v2_t *value_type_field, l_fields_v2_t *comparator_field, l_fields_v2_t *devgrp_field, l_fields_v2_t *itemgrp_field)
{
    return __trigger_grp_functions(COMPARE_VALUES, devgrp_field, itemgrp_field, NULL, comparator_field, value_field, NULL, NULL, value_type_field, NULL, 0);
}
/* Has atleast one dictornary Value */
int EZPI_scenes_operators_has_atleastone_dictionary_value_operations(uint32_t item_id, l_fields_v2_t *value_field)
{
    int ret = 0;

    if (item_id && value_field)
    {
        l_ezlopi_item_t *item = ezlopi_device_get_item_by_id(item_id);
        if (item)
        {
            cJSON *cj_item_value = cJSON_CreateObject(__FUNCTION__);
            if (cj_item_value)
            {
                item->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, item, (void *)cj_item_value, NULL);
                cJSON *cj_valuetype = cJSON_GetObjectItem(__FUNCTION__, cj_item_value, ezlopi_valueType_str); // first check the item_type -> 'valueType'
                if (cj_valuetype)                                                                             // type => dictionary
                {
                    if (STR_OP_COMP("dictionary", == , cJSON_GetStringValue(cj_valuetype))) // 'dictionary' == 'dictionary'?
                    {
                        cJSON *cj_value = cJSON_GetObjectItem(__FUNCTION__, cj_item_value, ezlopi_value_str); // item_value -> dictionary ; [array or object]
                        if (cj_value && cJSON_IsObject(cj_value))
                        {
                            CJSON_TRACE("cj_dictionary :", cj_value);
                            // Check if ["low_battery":"..."] key is present
                            /* need to use array here ; check against vales no keys. */
                            cJSON *dictionaryValue = cJSON_GetObjectItem(__FUNCTION__, cj_value, value_field->field_value.u_value.value_string);
                            if (NULL != dictionaryValue) // if the "eg. low_battery" element exists within the dictionary
                            {
                                ret = 1;
                            }
                            else
                            {
                                TRACE_E(" 'key : value[%s]' : not found", value_field->field_value.u_value.value_string);
                            }
                        }
                    }
                }
                cJSON_Delete(__FUNCTION__, cj_item_value);
            }
        }
    }

    return ret;
}
/* isDictornary Changed */
int EZPI_scenes_operators_is_dictionary_changed_operations(l_scenes_list_v2_t *scene_node, uint32_t item_id, l_fields_v2_t *key_field, l_fields_v2_t *operation_field)
{
    int ret = 0;
    if (item_id && key_field && (operation_field->field_value.u_value.value_string))
    {
        cJSON *item_value = NULL;
        l_ezlopi_item_t *item = ezlopi_device_get_item_by_id(item_id);
        if (item)
        {
            cJSON *cj_item_value = cJSON_CreateObject(__FUNCTION__);
            if (cj_item_value)
            {
                item->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, item, (void *)cj_item_value, NULL);
                cJSON *cj_valuetype = cJSON_GetObjectItem(__FUNCTION__, cj_item_value, ezlopi_valueType_str); // first check the item_type -> 'valueType'
                if (cj_valuetype)                                                                             // type => dictionary
                {
                    if (STR_OP_COMP("dictionary", == , cJSON_GetStringValue(cj_valuetype))) // 'dictionary' == 'dictionary'?
                    {
                        cJSON *cj_value = cJSON_GetObjectItem(__FUNCTION__, cj_item_value, ezlopi_value_str); // item_value -> dictionary ; [array or object]
                        if (cj_value && cJSON_IsObject(cj_value))
                        {
                            item_value = cj_value;
                        }
                    }
                }
                cJSON_Delete(__FUNCTION__, cj_item_value);
            }
        }

        if (NULL != item_value)
        {
            if (cJSON_IsObject(item_value)) // c
            {
                char *tmp_str = operation_field->field_value.u_value.value_string;
                size_t len = (NULL != tmp_str) ? strlen(tmp_str) + 1 : 0;
                if (EZPI_STRNCMP_IF_EQUAL("added", tmp_str, 6, len))
                {
                    // Check if ["key":"key_1"] key is present
                    cJSON *dictionaryValue = cJSON_GetObjectItem(__FUNCTION__, item_value, key_field->field_value.u_value.value_string);
                    if (NULL != dictionaryValue) // if the "eg. key_1" element exists within the dictionary
                    {
                        ret = 1;
                    }
                }
                else if (EZPI_STRNCMP_IF_EQUAL("removed", tmp_str, 8, len))
                {
                    // Check if ["key":"key_1"] key is absent
                    cJSON *dictionaryValue = cJSON_GetObjectItem(__FUNCTION__, item_value, key_field->field_value.u_value.value_string);
                    if (NULL == dictionaryValue) // if the "eg. key_1" element exists within the dictionary
                    {
                        ret = 1;
                    }
                }
                else if (EZPI_STRNCMP_IF_EQUAL("updated", tmp_str, 8, len))
                {
                    // Check if ["key":"key_1"] key is present and its elemnt
                    cJSON *dictionaryValue = cJSON_GetObjectItem(__FUNCTION__, item_value, key_field->field_value.u_value.value_string);
                    if (NULL != dictionaryValue) // if the "eg. key_1" element exists within the dictionary
                    {
                        cJSON *last_dictionary_item = (cJSON *)scene_node->when_block->fields->user_arg;
                        ret = cJSON_Compare(last_dictionary_item, item_value, 0);
                    }
                }

                // if the item exists ; Store current item_value into 'scene_node->when_block->fields->user_arg'
                scene_node->when_block->fields->user_arg = (void *)item_value;
            }
        }
    }
    return ret;
}

/*******************************************************************************
*                         Static Function Definitions
*******************************************************************************/
static int __evaluate_compareNumber_or_compareStrings(l_fields_v2_t *item_exp_field, l_fields_v2_t *value_field, char *comparator_str)
{
    int ret = 0;

    if (item_exp_field && value_field && (NULL != comparator_str))
    {
        // 1. LHS = expression
        if (EZLOPI_VALUE_TYPE_EXPRESSION == item_exp_field->value_type)
        {
            s_ezlopi_expressions_t *curr_expr_left = EZPI_scenes_expressions_get_node_by_name(item_exp_field->field_value.u_value.value_string);
            if (curr_expr_left)
            {
                if (EZLOPI_VALUE_TYPE_EXPRESSION != value_field->value_type)
                {
                    //---------------  expression_valueType  VS  other_valueType  --------------------
                    ret = ____compare_exp_vs_other(curr_expr_left, value_field, comparator_str);
                    //--------------------------------------------------------------------------------
                }
                else if (EZLOPI_VALUE_TYPE_EXPRESSION == value_field->value_type)
                {
                    s_ezlopi_expressions_t *curr_expr_right = EZPI_scenes_expressions_get_node_by_name(value_field->field_value.u_value.value_string);
                    if (curr_expr_right)
                    {
                        //---------------  expression_valueType  VS  expression_valueType  ---------------
                        ret = ____compare_exp_vs_exp(curr_expr_left, curr_expr_right, comparator_str);
                        //--------------------------------------------------------------------------------
                    }
                }
            }
        }
        else
        { // 2. LHS = item
            uint32_t item_id = strtoul(item_exp_field->field_value.u_value.value_string, NULL, 16);

            l_ezlopi_item_t *item = ezlopi_device_get_item_by_id(item_id);
            if (item)
            {
                if (EZLOPI_VALUE_TYPE_EXPRESSION != value_field->value_type)
                {
                    //---------------  item_valueType  VS  other_valueType  --------------------
                    ret = ____compare_item_vs_other(item, value_field, comparator_str);
                    //--------------------------------------------------------------------------
                }
                else if (EZLOPI_VALUE_TYPE_EXPRESSION == value_field->value_type)
                {
                    s_ezlopi_expressions_t *curr_expr_right = EZPI_scenes_expressions_get_node_by_name(value_field->field_value.u_value.value_string);
                    if (curr_expr_right)
                    { //---------------  item_valueType  VS  expression_valueType  --------------------
                        ret = ____compare_item_vs_exp(item, curr_expr_right, comparator_str);
                        //--------------------------------------------------------------------------
                    }
                }
            }
        }
    }
    return ret;
}
static int __evaluate_compareNumbers_or_compareStrings_with_group(l_fields_v2_t *devgrp_field, l_fields_v2_t *itemgrp_field, l_fields_v2_t *value_field, l_fields_v2_t *comparator_field)
{
    return __trigger_grp_functions(COMPARE_NUM_OR_STR, devgrp_field, itemgrp_field, NULL, comparator_field, value_field, NULL, NULL, NULL, NULL, 0);
}
/* Numeric & String */
static int ________compare_val_num(double item_exp_value, double value_to_compare_with, char *comparator_field_str)
{
    int ret = 0;
    e_scene_num_cmp_operators_t numeric_operator = EZPI_scenes_numeric_comparator_operators_get_enum(comparator_field_str);
    switch (numeric_operator)
    {
    case SCENES_NUM_COMP_OPERATORS_LESS:
    {
        ret = (item_exp_value < value_to_compare_with);
        break;
    }
    case SCENES_NUM_COMP_OPERATORS_LESS_EQUAL:
    {
        ret = (item_exp_value <= value_to_compare_with);
        break;
    }
    case SCENES_NUM_COMP_OPERATORS_GREATER:
    {
        ret = (item_exp_value > value_to_compare_with);
        break;
    }
    case SCENES_NUM_COMP_OPERATORS_GREATER_EQUAL:
    {
        ret = (item_exp_value >= value_to_compare_with);
        break;
    }
    case SCENES_NUM_COMP_OPERATORS_EQUAL:
    {
        ret = (item_exp_value == value_to_compare_with);
        break;
    }
    case SCENES_NUM_COMP_OPERATORS_NOT_EQUAL:
    {
        ret = (item_exp_value != value_to_compare_with);
        break;
    }
    default:
    {
        TRACE_E("'SCENES_NUM_COMP_OPERATORS_* [%d]' out of range!", numeric_operator);
        break;
    }
    }

    return ret;
}
static int ________compare_val_str(const char *item_exp_value_str, const char *value_to_compare_with_str, char *comparator_field_str)
{
    int ret = 0;
    e_scene_str_cmp_operators_t string_operator = EZPI_scenes_strings_comparator_operators_get_enum(comparator_field_str);
    switch (string_operator)
    {
    case SCENES_STRINGS_OPERATORS_LESS:
    {
        ret = STR_OP_COMP(item_exp_value_str, < , value_to_compare_with_str);
        break;
    }
    case SCENES_STRINGS_OPERATORS_LESS_EQUAL:
    {
        ret = STR_OP_COMP(item_exp_value_str, <= , value_to_compare_with_str);
        break;
    }
    case SCENES_STRINGS_OPERATORS_GREATER:
    {
        ret = STR_OP_COMP(item_exp_value_str, > , value_to_compare_with_str);
        break;
    }
    case SCENES_STRINGS_OPERATORS_GREATER_EQUAL:
    {
        ret = STR_OP_COMP(item_exp_value_str, >= , value_to_compare_with_str);
        break;
    }
    case SCENES_STRINGS_OPERATORS_EQUAL:
    {
        ret = STR_OP_COMP(item_exp_value_str, == , value_to_compare_with_str);
        break;
    }
    case SCENES_STRINGS_OPERATORS_NOT_EQUAL:
    {
        ret = STR_OP_COMP(item_exp_value_str, != , value_to_compare_with_str);
        break;
    }
    default:
    {
        TRACE_E("'SCENES_STRINGS_OPERATORS_* [%d]' out of range!", string_operator);
        break;
    }
    }

    return ret;
}
static int ____compare_exp_vs_other(s_ezlopi_expressions_t *curr_expr_left, l_fields_v2_t *value_field, char *comparator_field_str)
{
    int ret = 0;

    if (curr_expr_left->value_type == value_field->value_type) // string == string
    {
        switch (curr_expr_left->exp_value.type) // from expression [char ,cj, bool , num]
        {
        case EXPRESSION_VALUE_TYPE_STRING:
        {
            if ((value_field->field_value.e_type == VALUE_TYPE_STRING) &&
                (NULL != curr_expr_left->exp_value.u_value.str_value) &&
                (NULL != value_field->field_value.u_value.value_string))
            {
                ret = ________compare_val_str(
                    curr_expr_left->exp_value.u_value.str_value,
                    value_field->field_value.u_value.value_string,
                    comparator_field_str);
            }
            break;
        }
        case EXPRESSION_VALUE_TYPE_NUMBER:
        {
            if (value_field->field_value.e_type == VALUE_TYPE_NUMBER)
            {
                ret = ________compare_val_num(
                    curr_expr_left->exp_value.u_value.boolean_value,
                    value_field->field_value.u_value.value_double,
                    comparator_field_str);
            }
            break;
        }
        case EXPRESSION_VALUE_TYPE_BOOL: // bool_values can be converted to 1/0s
        {
            if (value_field->field_value.e_type == VALUE_TYPE_BOOL)
            {
                ret = ________compare_val_num(
                    curr_expr_left->exp_value.u_value.boolean_value,
                    value_field->field_value.u_value.value_bool,
                    comparator_field_str);
            }
            break;
        }
        default:
            TRACE_W("Error!! can compare only :- string / bool / number ");
            break;
        }
    }
    else
    {
        TRACE_E(" LHS != RHS [ '%s' vs '%s' ]", ezlopi_scene_get_scene_value_type_name(curr_expr_left->value_type), ezlopi_scene_get_scene_value_type_name(value_field->value_type));
    }

    return ret;
}
static int ____compare_exp_vs_exp(s_ezlopi_expressions_t *curr_expr_left, s_ezlopi_expressions_t *curr_expr_right, char *comparator_field_str)
{
    int ret = 0;

    if (curr_expr_left->value_type == curr_expr_right->value_type) // temperature == temperature
    {
        switch (curr_expr_left->exp_value.type) // from expression [char ,cj, bool , num]
        {
        case EXPRESSION_VALUE_TYPE_STRING:
        {
            if ((curr_expr_right->exp_value.type == EXPRESSION_VALUE_TYPE_STRING) &&
                (NULL != curr_expr_left->exp_value.u_value.str_value) &&
                (NULL != curr_expr_right->exp_value.u_value.str_value))
            {
                ret = ________compare_val_str(
                    curr_expr_left->exp_value.u_value.str_value,
                    curr_expr_right->exp_value.u_value.str_value,
                    comparator_field_str);
            }
            break;
        }
        case EXPRESSION_VALUE_TYPE_NUMBER:
        {
            if (curr_expr_right->exp_value.type == EXPRESSION_VALUE_TYPE_NUMBER)
            {
                ret = ________compare_val_num(
                    curr_expr_left->exp_value.u_value.number_value,
                    curr_expr_right->exp_value.u_value.number_value,
                    comparator_field_str);
            }
            break;
        }
        case EXPRESSION_VALUE_TYPE_BOOL: // bool_values can be converted to 1/0s
        {
            if (curr_expr_right->exp_value.type == EXPRESSION_VALUE_TYPE_BOOL)
            {
                ret = ________compare_val_num(
                    curr_expr_left->exp_value.u_value.boolean_value,
                    curr_expr_right->exp_value.u_value.boolean_value,
                    comparator_field_str);
            }
            break;
        }
        default:
            TRACE_W("Error!! can compare only :- string / bool / number ");
            break;
        }
    }
    else
    {
        TRACE_E(" LHS != RHS [ '%s' vs '%s' ]", ezlopi_scene_get_scene_value_type_name(curr_expr_left->value_type), ezlopi_scene_get_scene_value_type_name(curr_expr_right->value_type));
    }

    return ret;
}
static int ____compare_exp_vs_item(s_ezlopi_expressions_t *curr_expr_left, l_ezlopi_item_t *item_right, char *comparator_field_str)
{
    int ret = 0;

    const char *lhs_str = ezlopi_scene_get_scene_value_type_name(curr_expr_left->value_type);
    const char *rhs_str = item_right->cloud_properties.value_type;
    if (EZPI_STRNCMP_IF_EQUAL(lhs_str, rhs_str, strlen(rhs_str) + 1, strlen(rhs_str) + 1)) // humidity == humidity
    {
        cJSON *cj_right = cJSON_CreateObject(__FUNCTION__);
        if (cj_right)
        {
            item_right->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, item_right, (void *)cj_right, NULL);
            cJSON *cj_item_right = cJSON_GetObjectItem(__FUNCTION__, cj_right, ezlopi_value_str);
            if (cj_item_right)
            {
                switch (curr_expr_left->exp_value.type) // from expression [char ,cj, bool , num]
                {
                case EXPRESSION_VALUE_TYPE_STRING:
                {
                    if (cJSON_IsString(cj_item_right) &&
                        (NULL != curr_expr_left->exp_value.u_value.str_value) &&
                        (NULL != cj_item_right->valuestring))
                    {
                        ret = ________compare_val_str(
                            curr_expr_left->exp_value.u_value.str_value,
                            cj_item_right->valuestring,
                            comparator_field_str);
                    }
                    break;
                }
                case EXPRESSION_VALUE_TYPE_NUMBER:
                {
                    if (cJSON_IsNumber(cj_item_right))
                    {
                        ret = ________compare_val_num(
                            curr_expr_left->exp_value.u_value.boolean_value,
                            cj_item_right->valuedouble,
                            comparator_field_str);
                    }
                    break;
                }
                case EXPRESSION_VALUE_TYPE_BOOL: // bool_values can be converted to 1/0s
                {
                    if (cJSON_IsBool(cj_item_right))
                    {
                        ret = ________compare_val_num(
                            curr_expr_left->exp_value.u_value.boolean_value,
                            (cJSON_True == cj_item_right->type) ? 1 : 0,
                            comparator_field_str);
                    }
                    break;
                }
                default:
                    TRACE_W("Error!! can compare only :- string / bool / number ");
                    break;
                }
            }
            cJSON_Delete(__FUNCTION__, cj_right);
        }
    }
    else
    {
        TRACE_E(" LHS != RHS [ '%s' vs '%s' ]", lhs_str, rhs_str);
    }

    return ret;
}
static int ____compare_item_vs_other(l_ezlopi_item_t *item_left, l_fields_v2_t *value_field, char *comparator_field_str)
{
    int ret = 0;

    const char *lhs_str = item_left->cloud_properties.value_type;
    const char *rhs_str = ezlopi_scene_get_scene_value_type_name(value_field->value_type);
    if (EZPI_STRNCMP_IF_EQUAL(lhs_str, rhs_str, strlen(lhs_str) + 1, strlen(rhs_str) + 1))
    { // making sure :- 'string' == 'string'

        cJSON *cj_item = cJSON_CreateObject(__FUNCTION__);
        if (cj_item)
        {
            item_left->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, item_left, (void *)cj_item, NULL);
            cJSON *cj_item_value = cJSON_GetObjectItem(__FUNCTION__, cj_item, ezlopi_value_str);
            if (cj_item_value)
            {
                switch (cj_item_value->type) // from item_left [char ,cj, bool , num]
                {
                case cJSON_String:
                {
                    if ((value_field->field_value.e_type == VALUE_TYPE_STRING) &&
                        (NULL != cj_item_value->valuestring) &&
                        (NULL != value_field->field_value.u_value.value_string))
                    {
                        ret = ________compare_val_str(
                            cj_item_value->valuestring,
                            value_field->field_value.u_value.value_string,
                            comparator_field_str);
                    }
                    break;
                }
                case cJSON_Number:
                {
                    if (value_field->field_value.e_type == VALUE_TYPE_NUMBER)
                    {
                        ret = ________compare_val_num(
                            cj_item_value->valuedouble,
                            value_field->field_value.u_value.value_double,
                            comparator_field_str);
                    }
                    break;
                }
                case cJSON_False: // bool_values can be converted to 1/0s
                case cJSON_True:  // bool_values can be converted to 1/0s
                {
                    if (value_field->field_value.e_type == VALUE_TYPE_BOOL)
                    {
                        ret = ________compare_val_num(
                            (cJSON_True == cj_item_value->type) ? 1 : 0,
                            value_field->field_value.u_value.value_bool,
                            comparator_field_str);
                    }
                    break;
                }
                default:
                    TRACE_W("Error!! can compare only :- string / bool / number ");
                    break;
                }
            }
            cJSON_Delete(__FUNCTION__, cj_item);
        }
    }
    else
    {
        TRACE_E(" LHS != RHS [ '%s' vs '%s' ]", lhs_str, rhs_str);
    }

    return ret;
}
static int ____compare_item_vs_exp(l_ezlopi_item_t *item_left, s_ezlopi_expressions_t *curr_expr_right, char *comparator_field_str)
{
    int ret = 0;

    const char *lhs_str = item_left->cloud_properties.value_type;
    const char *rhs_str = ezlopi_scene_get_scene_value_type_name(curr_expr_right->value_type);
    if (EZPI_STRNCMP_IF_EQUAL(lhs_str, rhs_str, strlen(lhs_str) + 1, strlen(rhs_str) + 1))
    { // making sure :- 'temperature' == 'temperature'

        cJSON *cj_item = cJSON_CreateObject(__FUNCTION__);
        if (cj_item)
        {
            item_left->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, item_left, (void *)cj_item, NULL);
            cJSON *cj_item_value = cJSON_GetObjectItem(__FUNCTION__, cj_item, ezlopi_value_str);
            if (cj_item_value)
            {
                switch (cj_item_value->type) // from item_left [char ,cj, bool , num]
                {
                case cJSON_String:
                {
                    if ((curr_expr_right->exp_value.type == EXPRESSION_VALUE_TYPE_STRING) &&
                        (NULL != cj_item_value->valuestring) &&
                        (NULL != curr_expr_right->exp_value.u_value.str_value))
                    {
                        ret = ________compare_val_str(
                            cj_item_value->valuestring,
                            curr_expr_right->exp_value.u_value.str_value,
                            comparator_field_str);
                    }
                    break;
                }
                case cJSON_Number:
                {
                    if (curr_expr_right->exp_value.type == EXPRESSION_VALUE_TYPE_NUMBER)
                    {
                        ret = ________compare_val_num(
                            cj_item_value->valuedouble,
                            curr_expr_right->exp_value.u_value.number_value,
                            comparator_field_str);
                    }
                    break;
                }
                case cJSON_True:  // bool_values can be converted to 1/0s
                case cJSON_False: // bool_values can be converted to 1/0s
                {
                    if (curr_expr_right->exp_value.type == EXPRESSION_VALUE_TYPE_BOOL)
                    {
                        ret = ________compare_val_num(
                            (cJSON_True == cj_item_value->type) ? 1 : 0,
                            curr_expr_right->exp_value.u_value.boolean_value,
                            comparator_field_str);
                    }
                    break;
                }
                default:
                    TRACE_W("Error!! can compare only :- string / bool / number ");
                    break;
                }
            }
            cJSON_Delete(__FUNCTION__, cj_item);
        }
    }
    else
    {
        TRACE_E(" LHS != RHS [ '%s' vs '%s' ]", lhs_str, rhs_str);
    }

    return ret;
}
static int ____compare_item_vs_item(l_ezlopi_item_t *item_left, l_ezlopi_item_t *item_right, char *comparator_field_str)
{
    int ret = 0;

    const char *lhs_str = item_left->cloud_properties.value_type;
    const char *rhs_str = item_right->cloud_properties.value_type;
    if (EZPI_STRNCMP_IF_EQUAL(lhs_str, rhs_str, strlen(lhs_str) + 1, strlen(rhs_str) + 1))
    { // making sure :- 'temperature' == 'temperature'
        // 1 . extracting LHS_item_value
        cJSON *cj_left = cJSON_CreateObject(__FUNCTION__);
        if (cj_left)
        {
            item_left->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, item_left, (void *)cj_left, NULL);
            cJSON *cj_item_left = cJSON_GetObjectItem(__FUNCTION__, cj_left, ezlopi_value_str);
            if (cj_item_left)
            {
                // 2. extracting the RHS_item_value
                cJSON *cj_right = cJSON_CreateObject(__FUNCTION__);
                if (cj_right)
                {
                    item_right->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, item_right, (void *)cj_right, NULL);
                    cJSON *cj_item_right = cJSON_GetObjectItem(__FUNCTION__, cj_right, ezlopi_value_str);
                    if (cj_item_right)
                    {
                        switch (cj_item_left->type) // since 'item_left_valueType' === 'item_right_valueType'  ;  [char ,cj, bool , num]
                        {
                        case cJSON_String:
                        {
                            if (cJSON_IsString(cj_item_right) &&
                                (NULL != cj_item_left->valuestring) &&
                                (NULL != cj_item_right->valuestring))
                            {
                                ret = ________compare_val_str(
                                    cj_item_left->valuestring,
                                    cj_item_right->valuestring,
                                    comparator_field_str);
                            }
                            break;
                        }
                        case cJSON_Number:
                        {
                            if (cJSON_IsNumber(cj_item_right))
                            {
                                ret = ________compare_val_num(
                                    cj_item_left->valuedouble,
                                    cj_item_right->valuedouble,
                                    comparator_field_str);
                            }
                            break;
                        }
                        case cJSON_True:  // bool_values can be converted to 1/0s
                        case cJSON_False: // bool_values can be converted to 1/0s
                        {
                            if (cJSON_IsBool(cj_item_right))
                            {
                                ret = ________compare_val_num(
                                    (cJSON_True == cj_item_left->type) ? 1 : 0,
                                    (cJSON_True == cj_item_right->type) ? 1 : 0,
                                    comparator_field_str);
                            }
                            break;
                        }
                        default:
                            TRACE_W("Error!! can compare only :- string / bool / number ");
                            break;
                        }
                    }
                    cJSON_Delete(__FUNCTION__, cj_right);
                }
            }
            cJSON_Delete(__FUNCTION__, cj_left);
        }
    }
    else
    {
        TRACE_E(" LHS != RHS [ '%s' vs '%s' ]", lhs_str, rhs_str);
    }

    return ret;
}
static int __evaluate_numrange(l_fields_v2_t *item_exp_field, l_fields_v2_t *start_value_field, l_fields_v2_t *end_value_field, bool comparator_choice);
static int __evaluate_compareNumber_or_compareStrings(l_fields_v2_t *item_exp_field, l_fields_v2_t *value_field, char *comparator_str);
static int __evaluate_compareNumbers_or_compareStrings_with_group(l_fields_v2_t *devgrp_field, l_fields_v2_t *itemgrp_field, l_fields_v2_t *value_field, l_fields_v2_t *comparator_field);
static int __trigger_grp_functions(e_with_grp_t choice, l_fields_v2_t *devgrp_field, l_fields_v2_t *itemgrp_field, l_fields_v2_t *operation_field, l_fields_v2_t *comparator_field, l_fields_v2_t *value_field, l_fields_v2_t *start_value_field, l_fields_v2_t *end_value_field, l_fields_v2_t *value_type_field, char *comparator_str, bool comparator_choice)
{
    int ret = 0;
    uint32_t device_group_id = strtoul(devgrp_field->field_value.u_value.value_string, NULL, 16);
    uint32_t item_group_id = strtoul(itemgrp_field->field_value.u_value.value_string, NULL, 16);

    l_ezlopi_device_grp_t *curr_devgrp = ezlopi_core_device_group_get_by_id(device_group_id);
    if (curr_devgrp)
    {
        cJSON *cj_get_devarr = NULL;
        cJSON_ArrayForEach(cj_get_devarr, curr_devgrp->devices)
        {
            uint32_t curr_devce_id = strtoul(cj_get_devarr->valuestring, NULL, 16);
            l_ezlopi_device_t *curr_device = ezlopi_device_get_by_id(curr_devce_id); // immediately goto "102ec000" ...
            if (curr_device)
            {
                l_ezlopi_item_t *curr_item_node = curr_device->items; // perform operation on items of above device --> "102ec000"
                while (curr_item_node)
                {
                    // compare with items_list stored in item_group_id
                    l_ezlopi_item_grp_t *curr_item_grp = ezlopi_core_item_group_get_by_id(item_group_id); // get  "ll_itemgrp_node"
                    if (curr_item_grp)
                    {
                        cJSON *cj_item_names = NULL;
                        cJSON_ArrayForEach(cj_item_names, curr_item_grp->item_names)
                        {
                            uint32_t req_item_id_from_itemgrp = strtoul(cj_item_names->valuestring, NULL, 16);
                            // if the item_ids match ; Then compare the "item_values" with that of the "scene's" requirement
                            if (req_item_id_from_itemgrp == curr_item_node->cloud_properties.item_id)
                            {
                                l_fields_v2_t *dummy_item_field = ezlopi_malloc(__FUNCTION__, sizeof(l_fields_v2_t));
                                if (dummy_item_field)
                                {
                                    memset(dummy_item_field, 0, sizeof(l_fields_v2_t));

                                    dummy_item_field->field_value.u_value.value_string = itemgrp_field->field_value.u_value.value_string; // no mallocs // only assign address
                                    dummy_item_field->value_type = EZLOPI_VALUE_TYPE_ITEM;

                                    // trigger according to the choice
                                    switch (choice)
                                    {
                                    case COMPARE_NUM_OR_STR:
                                    {
                                        ret = __evaluate_compareNumber_or_compareStrings(dummy_item_field, value_field, comparator_str);
                                        break;
                                    }
                                    case COMPARE_NUM_RANGE:
                                    {
                                        ret = __evaluate_numrange(dummy_item_field, start_value_field, end_value_field, comparator_choice);
                                        break;
                                    }
                                    case COMPARE_VALUES:
                                    {
                                        ret = EZPI_scenes_operators_value_comparevalues_with_less_operations(dummy_item_field, value_field, value_type_field, comparator_field);
                                        break;
                                    }
                                    case COMPARE_INARRAY:
                                    {
                                        ret = EZPI_scenes_operators_value_inarr_operations(dummy_item_field, value_field, operation_field);
                                        break;
                                    }
                                    case COMPARE_STROPS:
                                    {
                                        ret = EZPI_scenes_operators_value_strops_operations(dummy_item_field, value_field, operation_field);
                                        break;
                                    }

                                    default:
                                        break;
                                    }

                                    dummy_item_field->field_value.u_value.value_string = NULL; // clear the pointer address
                                    ezlopi_free(__FUNCTION__, dummy_item_field);
                                }
                                break;
                            }
                        }
                    }
                    curr_item_node = curr_item_node->next;
                }
            }
        }
    }

    return ret;
}
/* Strings */
static char *__get_item_strvalue_by_id(uint32_t item_id)
{
    char *item_value_str = NULL;
    l_ezlopi_item_t *item = ezlopi_device_get_item_by_id(item_id);
    if (item)
    {
        cJSON *cj_item_value = cJSON_CreateObject(__FUNCTION__);
        if (cj_item_value)
        {
            item->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, item, (void *)cj_item_value, NULL);
            cJSON *cj_value = cJSON_GetObjectItem(__FUNCTION__, cj_item_value, ezlopi_value_str);
            if (cj_value && cJSON_IsString(cj_value))
            {
                item_value_str = cJSON_GetStringValue(cj_value);
            }
            cJSON_Delete(__FUNCTION__, cj_item_value);
        }
    }
    return item_value_str;
}
/* Numeric_Range */
static int ________compare_numeric_range_num(double extract_data, double start_value_field_num, double end_value_field_num, bool comparator_choice)
{
    int ret = 0;
    switch (comparator_choice)
    {
    case false: // between
    {
        ret = ((extract_data >= start_value_field_num) &&
            (extract_data <= end_value_field_num))
            ? 1
            : 0;
        break;
    }
    case true: // not between
    {
        ret = ((extract_data < start_value_field_num) &&
            (extract_data > end_value_field_num))
            ? 1
            : 0;
        break;
    }
    }
    return ret;
}
static int ________compare_numeric_range_str(const char *extract_data, const char *start_value_field_str, const char *end_value_field_str, bool comparator_choice)
{
    int ret = 0;
    switch (comparator_choice)
    {
    case false: // between
    {
        ret = ((STR_OP_COMP(extract_data, >= , start_value_field_str)) &&
            (STR_OP_COMP(extract_data, <= , end_value_field_str)))
            ? 1
            : 0;
        break;
    }
    case true: // not between
    {
        ret = ((STR_OP_COMP(extract_data, < , start_value_field_str)) &&
            (STR_OP_COMP(extract_data, > , end_value_field_str)))
            ? 1
            : 0;

        break;
    }
    }
    return ret;
}
static int ____compare_range_exp_vs_other(s_ezlopi_expressions_t *curr_expr_left, l_fields_v2_t *start_value_field, l_fields_v2_t *end_value_field, bool comparator_choice)
{
    int ret = 0;

    if ((curr_expr_left->value_type == end_value_field->value_type) &&
        (curr_expr_left->value_type == start_value_field->value_type)) // temperature == temperature
    {
        switch (curr_expr_left->exp_value.type) // from expression [char ,cj, bool , num]
        {
        case EXPRESSION_VALUE_TYPE_STRING:
        {
            if (start_value_field->field_value.e_type == VALUE_TYPE_STRING &&
                end_value_field->field_value.e_type == VALUE_TYPE_STRING &&
                (NULL != curr_expr_left->exp_value.u_value.str_value) &&
                (NULL != start_value_field->field_value.u_value.value_string) &&
                (NULL != end_value_field->field_value.u_value.value_string))
            {
                ret = ________compare_numeric_range_str(
                    curr_expr_left->exp_value.u_value.str_value,
                    start_value_field->field_value.u_value.value_string,
                    end_value_field->field_value.u_value.value_string,
                    comparator_choice);
            }
            break;
        }
        case EXPRESSION_VALUE_TYPE_NUMBER:
        {
            if ((start_value_field->field_value.e_type == VALUE_TYPE_NUMBER &&
                end_value_field->field_value.e_type == VALUE_TYPE_NUMBER))
            {
                ret = ________compare_numeric_range_num(
                    (double)curr_expr_left->exp_value.u_value.boolean_value,
                    start_value_field->field_value.u_value.value_double,
                    end_value_field->field_value.u_value.value_double,
                    comparator_choice);
            }
            break;
        }
        case EXPRESSION_VALUE_TYPE_BOOL: // bool_values can be converted to 1/0s
        {
            if ((start_value_field->field_value.e_type == VALUE_TYPE_BOOL &&
                end_value_field->field_value.e_type == VALUE_TYPE_BOOL))
            {
                ret = ________compare_numeric_range_num(
                    (double)curr_expr_left->exp_value.u_value.boolean_value,
                    start_value_field->field_value.u_value.value_bool,
                    end_value_field->field_value.u_value.value_bool,
                    comparator_choice);
            }
            break;
        }
        default:
            TRACE_W("Error!! can compare only :- string / bool / number ");
            break;
        }
    }
    else
    {
        TRACE_E(" LHS != RHS [ '%s' vs '%s' ]", ezlopi_scene_get_scene_value_type_name(start_value_field->value_type), ezlopi_scene_get_scene_value_type_name(end_value_field->value_type));
    }

    return ret;
}
static int ____compare_range_exp_vs_exp(s_ezlopi_expressions_t *curr_expr_left, s_ezlopi_expressions_t *curr_expr_right_start, s_ezlopi_expressions_t *curr_expr_right_end, bool comparator_choice)
{
    int ret = 0;

    if ((curr_expr_left->value_type == curr_expr_right_start->value_type) &&
        (curr_expr_left->value_type == curr_expr_right_end->value_type)) // temperature == temperature
    {
        switch (curr_expr_left->exp_value.type) // from expression [char ,cj, bool , num]
        {
        case EXPRESSION_VALUE_TYPE_STRING:
        {
            if ((curr_expr_right_start->exp_value.type == EXPRESSION_VALUE_TYPE_STRING &&
                curr_expr_right_end->exp_value.type == EXPRESSION_VALUE_TYPE_STRING) &&
                (NULL != curr_expr_left->exp_value.u_value.str_value) &&
                (NULL != curr_expr_right_start->exp_value.u_value.str_value) &&
                (NULL != curr_expr_right_end->exp_value.u_value.str_value))
            {
                ret = ________compare_numeric_range_str(
                    curr_expr_left->exp_value.u_value.str_value,
                    curr_expr_right_start->exp_value.u_value.str_value,
                    curr_expr_right_end->exp_value.u_value.str_value,
                    comparator_choice);
            }
            break;
        }
        case EXPRESSION_VALUE_TYPE_NUMBER:
        {
            if ((curr_expr_right_start->exp_value.type == EXPRESSION_VALUE_TYPE_NUMBER &&
                curr_expr_right_end->exp_value.type == EXPRESSION_VALUE_TYPE_NUMBER))
            {
                ret = ________compare_numeric_range_num(
                    curr_expr_left->exp_value.u_value.number_value,
                    curr_expr_right_start->exp_value.u_value.number_value,
                    curr_expr_right_end->exp_value.u_value.number_value,
                    comparator_choice);
            }
            break;
        }
        case EXPRESSION_VALUE_TYPE_BOOL: // bool_values can be converted to 1/0s
        {
            if ((curr_expr_right_start->exp_value.type == EXPRESSION_VALUE_TYPE_BOOL &&
                curr_expr_right_end->exp_value.type == EXPRESSION_VALUE_TYPE_BOOL))
            {
                ret = ________compare_numeric_range_num(
                    (double)curr_expr_left->exp_value.u_value.boolean_value,
                    curr_expr_right_start->exp_value.u_value.boolean_value,
                    curr_expr_right_end->exp_value.u_value.boolean_value,
                    comparator_choice);
            }
            break;
        }
        default:
            TRACE_W("Error!! can compare only :- string / bool / number ");
            break;
        }
    }
    else
    {
        TRACE_E(" LHS != RHS [ '%s' vs '%s' ]", ezlopi_scene_get_scene_value_type_name(curr_expr_right_start->value_type), ezlopi_scene_get_scene_value_type_name(curr_expr_right_end->value_type));
    }

    return ret;
}
static int ____compare_range_item_vs_other(l_ezlopi_item_t *item, l_fields_v2_t *start_value_field, l_fields_v2_t *end_value_field, bool comparator_choice)
{
    int ret = 0;

    const char *lhs_str = item->cloud_properties.value_type;
    const char *rhs_str = ezlopi_scene_get_scene_value_type_name(start_value_field->value_type);
    if ((EZPI_STRNCMP_IF_EQUAL(lhs_str, rhs_str, strlen(lhs_str) + 1, strlen(rhs_str) + 1) &&
        (start_value_field->value_type == end_value_field->value_type))) // making sure :- 'temperature' == 'temperature'
    {
        cJSON *cj_item = cJSON_CreateObject(__FUNCTION__);
        if (cj_item)
        {
            item->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, item, (void *)cj_item, NULL);
            cJSON *cj_item_value = cJSON_GetObjectItem(__FUNCTION__, cj_item, ezlopi_value_str);
            if (cj_item_value)
            {
                /* Fill the 'extract_data' structure with matching values only */
                switch (cj_item_value->type) // from expression [char ,cj, bool , num]
                {
                case cJSON_String:
                {
                    if ((start_value_field->field_value.e_type == VALUE_TYPE_STRING &&
                        end_value_field->field_value.e_type == VALUE_TYPE_STRING) &&
                        (NULL != cj_item_value->valuestring) &&
                        (NULL != start_value_field->field_value.u_value.value_string) &&
                        (NULL != end_value_field->field_value.u_value.value_string))
                    {
                        ret = ________compare_numeric_range_str(
                            cj_item_value->valuestring,
                            start_value_field->field_value.u_value.value_string,
                            end_value_field->field_value.u_value.value_string,
                            comparator_choice);
                    }
                    break;
                }
                case cJSON_Number:
                {
                    if ((start_value_field->field_value.e_type == VALUE_TYPE_NUMBER &&
                        end_value_field->field_value.e_type == VALUE_TYPE_NUMBER))
                    {
                        ret = ________compare_numeric_range_num(
                            cj_item_value->valuedouble,
                            start_value_field->field_value.u_value.value_double,
                            end_value_field->field_value.u_value.value_double,
                            comparator_choice);
                    }
                    break;
                }
                case cJSON_False: // bool_values can be converted to 1/0s
                case cJSON_True:  // bool_values can be converted to 1/0s
                {
                    if ((start_value_field->field_value.e_type == VALUE_TYPE_BOOL &&
                        end_value_field->field_value.e_type == VALUE_TYPE_BOOL))
                    {
                        ret = ________compare_numeric_range_num(
                            (cJSON_True == cj_item_value->type) ? 1 : 0,
                            start_value_field->field_value.u_value.value_bool,
                            end_value_field->field_value.u_value.value_bool,
                            comparator_choice);
                    }
                    break;
                }
                default:
                    TRACE_W("Error!! can compare only :- string / bool / number ");
                    break;
                }
            }
            cJSON_Delete(__FUNCTION__, cj_item);
        }
    }
    else
    {
        TRACE_E(" LHS != RHS [ '%s' vs '%s' ]", lhs_str, rhs_str);
    }
    return ret;
}
static int ____compare_range_item_vs_exp(l_ezlopi_item_t *item, s_ezlopi_expressions_t *curr_expr_right_start, s_ezlopi_expressions_t *curr_expr_right_end, bool comparator_choice)
{
    int ret = 0;

    const char *lhs_str = item->cloud_properties.value_type;
    const char *rhs_str = ezlopi_scene_get_scene_value_type_name(curr_expr_right_start->value_type);
    if ((EZPI_STRNCMP_IF_EQUAL(lhs_str, rhs_str, strlen(lhs_str) + 1, strlen(rhs_str) + 1)) &&
        (curr_expr_right_start->value_type == curr_expr_right_end->value_type)) // making sure :- 'temperature' == 'temperature'
    {
        cJSON *cj_item = cJSON_CreateObject(__FUNCTION__);
        if (cj_item)
        {
            item->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, item, (void *)cj_item, NULL);
            cJSON *cj_item_value = cJSON_GetObjectItem(__FUNCTION__, cj_item, ezlopi_value_str);
            if (cj_item_value)
            {
                /* Fill the 'extract_data' structure with matching values only */
                switch (cj_item_value->type) // from expression [char ,cj, bool , num]
                {
                case cJSON_String:
                {
                    if ((curr_expr_right_start->exp_value.type == EXPRESSION_VALUE_TYPE_STRING &&
                        curr_expr_right_end->exp_value.type == EXPRESSION_VALUE_TYPE_STRING) &&
                        (NULL != cj_item_value->valuestring) &&
                        (NULL != curr_expr_right_start->exp_value.u_value.str_value) &&
                        (NULL != curr_expr_right_end->exp_value.u_value.str_value))
                    {
                        ret = ________compare_numeric_range_str(
                            cj_item_value->valuestring,
                            curr_expr_right_start->exp_value.u_value.str_value,
                            curr_expr_right_end->exp_value.u_value.str_value,
                            comparator_choice);
                    }
                    break;
                }
                case cJSON_Number:
                {
                    if ((curr_expr_right_start->exp_value.type == EXPRESSION_VALUE_TYPE_NUMBER &&
                        curr_expr_right_end->exp_value.type == EXPRESSION_VALUE_TYPE_NUMBER))
                    {
                        ret = ________compare_numeric_range_num(
                            cj_item_value->valuedouble,
                            curr_expr_right_start->exp_value.u_value.number_value,
                            curr_expr_right_end->exp_value.u_value.number_value,
                            comparator_choice);
                    }
                    break;
                }
                case cJSON_False: // bool_values can be converted to 1/0s
                case cJSON_True:  // bool_values can be converted to 1/0s
                {
                    if ((curr_expr_right_start->exp_value.type == EXPRESSION_VALUE_TYPE_BOOL &&
                        curr_expr_right_end->exp_value.type == EXPRESSION_VALUE_TYPE_BOOL))
                    {
                        ret = ________compare_numeric_range_num(
                            (cJSON_True == cj_item_value->type) ? 1 : 0,
                            curr_expr_right_start->exp_value.u_value.boolean_value,
                            curr_expr_right_end->exp_value.u_value.boolean_value,
                            comparator_choice);
                    }
                    break;
                }
                default:
                    TRACE_W("Error!! can compare only :- string / bool / number ");
                    break;
                }
            }
            cJSON_Delete(__FUNCTION__, cj_item);
        }
    }
    else
    {
        TRACE_E(" LHS != RHS [ '%s' vs '%s' ]", lhs_str, rhs_str);
    }

    return ret;
}
static int __evaluate_numrange(l_fields_v2_t *item_exp_field, l_fields_v2_t *start_value_field, l_fields_v2_t *end_value_field, bool comparator_choice)
{
    int ret = 0;
    // 1. LHS = expression
    if (EZLOPI_VALUE_TYPE_EXPRESSION == item_exp_field->value_type)
    {
        s_ezlopi_expressions_t *curr_expr_left = EZPI_scenes_expressions_get_node_by_name(item_exp_field->field_value.u_value.value_string);
        if (curr_expr_left)
        {
            if (EZLOPI_VALUE_TYPE_EXPRESSION != start_value_field->value_type &&
                EZLOPI_VALUE_TYPE_EXPRESSION != end_value_field->value_type)
            {
                //---------------  expression_valueType  VS  other_valueType  --------------------
                ret = ____compare_range_exp_vs_other(curr_expr_left, start_value_field, end_value_field, comparator_choice);
                //--------------------------------------------------------------------------------
            }
            else if (EZLOPI_VALUE_TYPE_EXPRESSION == start_value_field->value_type &&
                EZLOPI_VALUE_TYPE_EXPRESSION == end_value_field->value_type)
            {
                s_ezlopi_expressions_t *curr_expr_right_start = EZPI_scenes_expressions_get_node_by_name(start_value_field->field_value.u_value.value_string);
                s_ezlopi_expressions_t *curr_expr_right_end = EZPI_scenes_expressions_get_node_by_name(end_value_field->field_value.u_value.value_string);
                if (curr_expr_right_start && curr_expr_right_end)
                { //---------------  expression_valueType  VS  expression_valueType  ---------------
                    ret = ____compare_range_exp_vs_exp(curr_expr_left, curr_expr_right_start, curr_expr_right_end, comparator_choice);
                    //--------------------------------------------------------------------------------
                }
            }
        }
    }
    else
    { // 2. LHS = item
        uint32_t item_id = strtoul(item_exp_field->field_value.u_value.value_string, NULL, 16);

        l_ezlopi_item_t *item = ezlopi_device_get_item_by_id(item_id);
        if (item)
        {
            if (EZLOPI_VALUE_TYPE_EXPRESSION != start_value_field->value_type &&
                EZLOPI_VALUE_TYPE_EXPRESSION != end_value_field->value_type)
            {
                //---------------  item_valueType  VS  other_valueType  --------------------
                ret = ____compare_range_item_vs_other(item, start_value_field, end_value_field, comparator_choice);
                //--------------------------------------------------------------------------
            }
            else if (EZLOPI_VALUE_TYPE_EXPRESSION == start_value_field->value_type &&
                EZLOPI_VALUE_TYPE_EXPRESSION == end_value_field->value_type)
            {
                s_ezlopi_expressions_t *curr_expr_right_start = EZPI_scenes_expressions_get_node_by_name(start_value_field->field_value.u_value.value_string);
                s_ezlopi_expressions_t *curr_expr_right_end = EZPI_scenes_expressions_get_node_by_name(end_value_field->field_value.u_value.value_string);
                if (curr_expr_right_start && curr_expr_right_end)
                { //---------------  item_valueType  VS  expression_valueType  --------------------
                    ret = ____compare_range_item_vs_exp(item, curr_expr_right_start, curr_expr_right_end, comparator_choice);
                    //--------------------------------------------------------------------------
                }
            }
        }
    }
    return ret;
}
/* String_operations */
static char *__ezlopi_stropr_laststr_comp(const char *haystack, const char *needle)
{
    char *loc = NULL;
    char *found = NULL;
    size_t pos = 0;
    while ((found = strstr(haystack + pos, needle)) != 0)
    {
        loc = found;
        pos = (found - haystack) + 1;
    }
    return loc;
}
/* Values in_array */
static int __compare_inarry_str(char *item_exp_value_str, l_fields_v2_t *value_field, bool operation)
{
    int ret = 0;
    if (item_exp_value_str)
    {
        cJSON *iterator = NULL;
        switch (operation)
        {
        case 0: // IN
        {
            cJSON_ArrayForEach(iterator, value_field->field_value.u_value.cj_value)
            {
                if (STR_OP_COMP(item_exp_value_str, == , cJSON_GetStringValue(iterator))) // check for all array elements ; if exists then break
                {
                    ret = 1;
                    break;
                }
            }
            break;
        }
        case 1: // NOT_IN
        {
            cJSON_ArrayForEach(iterator, value_field->field_value.u_value.cj_value)
            {
                ret = 1;
                if (STR_OP_COMP(item_exp_value_str, == , cJSON_GetStringValue(iterator))) // check for all array elements ; if exists then break
                {
                    ret = 0;
                    break;
                }
            }
            break;
        }
        }
    }
    return ret;
}
static int __compare_inarry_num(double item_exp_value, l_fields_v2_t *value_field, bool operation)
{
    int ret = 0;

    cJSON *iterator = NULL;
    switch (operation)
    {
    case 0: // IN
    {
        cJSON_ArrayForEach(iterator, value_field->field_value.u_value.cj_value)
        {
            double rhs_val = (cJSON_IsBool(iterator)) ? ((cJSON_True == iterator->type) ? 1 : 0) : cJSON_GetNumberValue(iterator);
            if (item_exp_value == rhs_val) // check for all array elements ; if exists then 'ret = 1' & break
            {
                ret = 1;
                break;
            }
        }
        break;
    }
    case 1: // NOT_IN
    {
        cJSON_ArrayForEach(iterator, value_field->field_value.u_value.cj_value)
        {
            ret = 1;
            double rhs_val = (cJSON_IsBool(iterator)) ? ((cJSON_True == iterator->type) ? 1 : 0) : cJSON_GetNumberValue(iterator);
            if (item_exp_value == rhs_val) // check for all array elements ; if exists then 'ret = 0' & break
            {
                ret = 0;
                break;
            }
        }
        break;
    }
    }

    return ret;
}
static int __compare_inarry_cj(cJSON *item_exp_value, l_fields_v2_t *value_field, bool operation)
{
    int ret = 0;
    if (item_exp_value)
    {
        cJSON *iterator = NULL;
        switch (operation)
        {
        case 0: // IN
        {
            cJSON_ArrayForEach(iterator, value_field->field_value.u_value.cj_value)
            {
                if (cJSON_Compare(item_exp_value, iterator, cJSON_False)) // check for all array elements ; if exists then 'ret = 1' & break
                {
                    ret = 1;
                    break;
                }
            }
            break;
        }
        case 1: // NOT_IN
        {
            cJSON_ArrayForEach(iterator, value_field->field_value.u_value.cj_value)
            {
                ret = 1;
                if (cJSON_Compare(item_exp_value, iterator, cJSON_False)) // check for all array elements ; if exists then 'ret = 0' & break
                {
                    ret = 0;
                    break;
                }
            }
            break;
        }
        }
    }
    return ret;
}
/* compareValues */
static bool __check_valuetypes(const char *lhs_type_str, const char *rhs_type_str, const char *required_type_str)
{
    bool ret = false;
    if ((EZPI_STRNCMP_IF_EQUAL(lhs_type_str, required_type_str, strlen(lhs_type_str) + 1, strlen(required_type_str)) + 1) &&
        (EZPI_STRNCMP_IF_EQUAL(rhs_type_str, required_type_str, strlen(rhs_type_str) + 1, strlen(required_type_str))) + 1) // humidity == humidity
    {
        ret = true;
    }
    return ret;
}

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS
/*******************************************************************************
*                          End of File
*******************************************************************************/