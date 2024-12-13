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
 * @file    main.h
 * @brief   These functions perform comparision operation for scenes
 * @author  xx
 * @version 0.1
 * @date    12th DEC 2024
*/
#ifndef _EZLOPI_CORE_SCENES_OPERATORS_H_
#define _EZLOPI_CORE_SCENES_OPERATORS_H_

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

/*******************************************************************************
*                          Include Files
*******************************************************************************/
#include <string.h>
#include <stdint.h>

#include "ezlopi_core_scenes_v2.h"

/*******************************************************************************
*                          C++ Declaration Wrapper
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

    /*******************************************************************************
    *                          Type & Macro Declarations
    *******************************************************************************/
    typedef enum e_scene_num_cmp_operators
    {
#define SCENES_NUM_COMP_OPERATORS(OPERATOR, op, name, method) SCENES_NUM_COMP_OPERATORS_##OPERATOR,
#include "__operators_macros/__numeric_comparision_operators_macros.h"
#undef SCENES_NUM_COMP_OPERATORS
    } e_scene_num_cmp_operators_t;

    typedef enum e_scene_str_cmp_operators
    {
#define SCENES_STRINGS_OPERATORS(OPERATOR, op, name, method) SCENES_STRINGS_OPERATORS_##OPERATOR,
#include "__operators_macros/__strings_comparision_operators_macros.h"
#undef SCENES_STRINGS_OPERATORS
    } e_scene_str_cmp_operators_t;

    typedef enum e_scene_strops_cmp_operators
    {
#define SCENES_STROPS_COMP_OPERATORES(OPERATOR, op, name, method) SCENES_STROPS_COMP_OPERATORES_##OPERATOR,
#include "__operators_macros/__string_op_comparision_operators_macros.h"
#undef SCENES_STROPS_COMP_OPERATORES
    } e_scene_strops_cmp_operators_t;

#if 0  /* Currently not in use --> check 'in_array' when-condition before removing */
    typedef enum e_scene_inarr_cmp_operators
    {
#define SCENES_IN_ARRAY_OPERATORS(OPERATOR, op, name, method) SCENES_IN_ARRAY_OPERATORS_##OPERATOR,
#include "__operators_macros/__in_array_comparision_operators_macros.h"
#undef SCENES_IN_ARRAY_OPERATORS
    } e_scene_inarr_cmp_operators_t;
#endif
    typedef enum e_scene_value_with_less_cmp_operators
    {
#define SCENES_VALUES_WITH_LESS_OPERATORS(OPERATOR, op, name, method) SCENES_VALUES_WITH_LESS_OPERATORS_##OPERATOR,
#include "__operators_macros/__value_with_less_comparision_operators_macros.h"
#undef SCENES_VALUES_WITH_LESS_OPERATORS
    } e_scene_value_with_less_cmp_operators_t;

    typedef enum e_scene_value_without_less_cmp_operators
    {
#define SCENES_VALUES_WITHOUT_LESS_OPERATORS(OPERATOR, op, name, method) SCENES_VALUES_WITHOUT_LESS_OPERATORS_##OPERATOR,
#include "__operators_macros/__value_without_less_comparision_operators_macros.h"
#undef SCENES_VALUES_WITHOUT_LESS_OPERATORS
    } e_scene_value_without_less_cmp_operators_t;

    /*******************************************************************************
    *                          Extern Data Declarations
    *******************************************************************************/

    /*******************************************************************************
    *                          Extern Function Prototypes
    *******************************************************************************/
    // Numeric Operators
    /**
    * @brief This function returns of operator symbols in string format.
    *
    * @param operator enum of corresponding operator symbol
    * @return const char *
    */
    const char *ezlopi_scenes_numeric_comparator_operators_get_op(e_scene_num_cmp_operators_t operator);
    /**
    * @brief This function returns 'operator_name' of operator symbol
    *
    * @param operator enum of corresponding operator symbol
    * @return const char *
    */
    const char *ezlopi_scenes_numeric_comparator_operators_get_name(e_scene_num_cmp_operators_t operator);
    /**
    * @brief This function returns 'method_name' of operator symbol
    *
    * @param operator enum of corresponding operator symbol
    * @return const char *
    */
    const char *ezlopi_scenes_numeric_comparator_operators_get_method(e_scene_num_cmp_operators_t operator);
    /**
    * @brief This function compare 'number_value' from input args
    *
    * @param item_exp_field item_exp-field block info
    * @param value_field value-field block info
    * @param comparator_field comparator-field block info
    * @param devgrp_field device-group block info
    * @param itemgrp_field item-group block info
    *
    * @return int
    */
    int ezlopi_scenes_operators_value_number_operations(l_fields_v2_t *item_exp_field, l_fields_v2_t *value_field, l_fields_v2_t *comparator_field, l_fields_v2_t *devgrp_field, l_fields_v2_t *itemgrp_field);
    // e_scene_num_cmp_operators_t ezlopi_scenes_numeric_comparator_operators_get_enum(char *operator_str);

    //Number-Range Operators
    /**
    * @brief This function compare 'number_value' from input args
    *
    * @param start_value_field value-field block info
    * @param end_value_field comparator-field block info
    * @param item_exp_field item_exp-field block info
    * @param comparator_choice device-group block info
    *
    * @return int
    */
    int ezlopi_scenes_operators_value_number_range_operations(l_fields_v2_t *item_exp_field, l_fields_v2_t *start_value_field, l_fields_v2_t *end_value_field, bool comparator_choice);
    int ezlopi_scenes_operators_value_number_range_operations_with_group(l_fields_v2_t *start_value_field, l_fields_v2_t *end_value_field, bool comparator_choice, l_fields_v2_t *devgrp_field, l_fields_v2_t *itemgrp_field);

    // Strings Operators
    const char *ezlopi_scenes_strings_comparator_operators_get_op(e_scene_str_cmp_operators_t operator);
    const char *ezlopi_scenes_strings_comparator_operators_get_name(e_scene_str_cmp_operators_t operator);
    const char *ezlopi_scenes_strings_comparator_operators_get_method(e_scene_str_cmp_operators_t operator);
    int ezlopi_scenes_operators_value_strings_operations(l_fields_v2_t *item_exp_field, l_fields_v2_t *value_field, l_fields_v2_t *comparator_field, l_fields_v2_t *devgrp_field, l_fields_v2_t *itemgrp_field);
    // e_scene_str_cmp_operators_t ezlopi_scenes_strings_comparator_operators_get_enum(char *operator_str);

    // String-Operation Operators
    const char *ezlopi_scenes_strops_comparator_operators_get_op(e_scene_strops_cmp_operators_t operator);
    const char *ezlopi_scenes_strops_comparator_operators_get_name(e_scene_strops_cmp_operators_t operator);
    const char *ezlopi_scenes_strops_comparator_operators_get_method(e_scene_strops_cmp_operators_t operator);
    int ezlopi_scenes_operators_value_strops_operations(l_fields_v2_t *item_exp_field, l_fields_v2_t *value_field, l_fields_v2_t *comparator_field);
    int ezlopi_scenes_operators_value_strops_operations_with_group(l_fields_v2_t *value_field, l_fields_v2_t *operation_field, l_fields_v2_t *devgrp_field, l_fields_v2_t *itemgrp_field);
    // e_scene_strops_cmp_operators_t ezlopi_scenes_strops_comparator_operators_get_enum(char *operator_str);

    // InArray Operators
#if 0
// e_scene_inarr_cmp_operators_t ezlopi_scenes_inarr_comparator_operators_get_enum(char* operator_inarr);
// const char* ezlopi_scenes_inarr_comparator_operators_get_op(e_scene_inarr_cmp_operators_t operator);
// const char* ezlopi_scenes_inarr_comparator_operators_get_name(e_scene_inarr_cmp_operators_t operator);
// const char* ezlopi_scenes_inarr_comparator_operators_get_method(e_scene_inarr_cmp_operators_t operator);
#endif
    int ezlopi_scenes_operators_value_inarr_operations(l_fields_v2_t *item_exp_field, l_fields_v2_t *value_field, l_fields_v2_t *comparator_field);
    int ezlopi_scenes_operators_value_inarr_operations_with_group(l_fields_v2_t *value_field, l_fields_v2_t *operation_field, l_fields_v2_t *devgrp_field, l_fields_v2_t *itemgrp_field);

    // Value with less Operators
    const char *ezlopi_scenes_value_with_less_comparator_operators_get_op(e_scene_value_with_less_cmp_operators_t operator);
    const char *ezlopi_scenes_value_with_less_comparator_operators_get_name(e_scene_value_with_less_cmp_operators_t operator);
    const char *ezlopi_scenes_value_with_less_comparator_operators_get_method(e_scene_value_with_less_cmp_operators_t operator);
    // e_scene_value_with_less_cmp_operators_t ezlopi_scenes_value_with_less_comparator_operators_get_enum(char *operator_str);
    // int ezlopi_scenes_operators_value_with_less_operations(uint32_t item_id, l_fields_v2_t* value_field, l_fields_v2_t* comparator_field);

    // Value without less Operators
    const char *ezlopi_scenes_value_without_less_comparator_operators_get_op(e_scene_value_without_less_cmp_operators_t operator);
    const char *ezlopi_scenes_value_without_less_comparator_operators_get_name(e_scene_value_without_less_cmp_operators_t operator);
    const char *ezlopi_scenes_value_without_less_comparator_operators_get_method(e_scene_value_without_less_cmp_operators_t operator);
    // e_scene_value_without_less_cmp_operators_t ezlopi_scenes_value_without_less_comparator_operators_get_enum(char *operator_str);
    // int ezlopi_scenes_operators_value_without_less_operations(uint32_t item_id, l_fields_v2_t* value_field, l_fields_v2_t* comparator_field);

    // CompareValues Operators 'without-less' (default)
    int ezlopi_scenes_operators_value_comparevalues_with_less_operations(l_fields_v2_t *item_exp_field, l_fields_v2_t *value_field, l_fields_v2_t *value_type_field, l_fields_v2_t *comparator_field);
    int ezlopi_scenes_operators_value_comparevalues_with_less_operations_with_group(l_fields_v2_t *value_field, l_fields_v2_t *value_type_field, l_fields_v2_t *comparator_field, l_fields_v2_t *devgrp_field, l_fields_v2_t *itemgrp_field);
    // int ezlopi_scenes_operators_value_comparevalues_without_less_operations(uint32_t item_id, l_fields_v2_t* value_field, l_fields_v2_t* value_type_field, l_fields_v2_t* comparator_field);

    // HasAtleastOneDictionaryValue
    int ezlopi_scenes_operators_has_atleastone_dictionary_value_operations(uint32_t item_id, l_fields_v2_t *value_field);

    // IsDictornaryChanged
    int ezlopi_scenes_operators_is_dictionary_changed_operations(l_scenes_list_v2_t *scene_node, uint32_t item_id, l_fields_v2_t *key_field, l_fields_v2_t *operation_field);


#ifdef __cplusplus
}
#endif

#endif  // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#endif // _EZLOPI_CORE_SCENES_OPERATORS_H_

/*******************************************************************************
*                          End of File
*******************************************************************************/


