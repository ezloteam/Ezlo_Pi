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
 * @file    ezlopi_core_scenes_operators.h
 * @brief   These functions perform comparision operation for scenes
 * @author  ezlopi_team_np
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

#if 0 /* Currently not in use --> check 'in_array' when-condition before removing */
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

    /**
     * @brief This function returns of operator symbols in string format.
     *
     * @param operator enum of corresponding operator symbol
     * @return const char *
     */
    const char *EZPI_scenes_numeric_comparator_operators_get_op(e_scene_num_cmp_operators_t operator);
    /**
     * @brief This function returns 'operator_name' of operator symbol
     *
     * @param operator enum of corresponding operator symbol
     * @return const char *
     */
    const char *EZPI_scenes_numeric_comparator_operators_get_name(e_scene_num_cmp_operators_t operator);
    /**
     * @brief This function returns 'method_name' of operator symbol
     *
     * @param operator enum of corresponding operator symbol
     * @return const char *
     */
    const char *EZPI_scenes_numeric_comparator_operators_get_method(e_scene_num_cmp_operators_t operator);
    /**
     * @brief This function compare 'number_value' from input args
     *
     * @param item_exp_field block with item_exp-field info
     * @param value_field block with value-field info
     * @param comparator_field block with comparator-field info
     * @param devgrp_field block with device-group info
     * @param itemgrp_field block with item-group info
     * @return int
     */
    int EZPI_scenes_operators_value_number_operations(l_fields_v2_t *item_exp_field, l_fields_v2_t *value_field, l_fields_v2_t *comparator_field, l_fields_v2_t *devgrp_field, l_fields_v2_t *itemgrp_field);

    /**
     * @brief This function compare 'number_range_value' using given args
     *
     * @param start_value_field block with 'start_value_field' info
     * @param end_value_field block with 'end_value_field' info
     * @param item_exp_field block with 'item_exp_field' info
     * @param comparator_choice compare-type : [0='between'; 1='Not_between'] .
     * @return int
     */
    int EZPI_scenes_operators_value_number_range_operations(l_fields_v2_t *item_exp_field, l_fields_v2_t *start_value_field, l_fields_v2_t *end_value_field, bool comparator_choice);
    /**
     * @brief This function compare 'number_range_values' using given args
     *
     * @param devgrp_field block with device-group info
     * @param itemgrp_field block with item-group info
     * @param start_value_field block with 'start_value_field' info
     * @param end_value_field block with 'start_value_field' info
     * @param comparator_choice compare-type : [0='between'; 1='Not_between'] .
     * @return int
     */
    int EZPI_scenes_operators_value_number_range_operations_with_group(l_fields_v2_t *devgrp_field, l_fields_v2_t *itemgrp_field, l_fields_v2_t *start_value_field, l_fields_v2_t *end_value_field, bool comparator_choice);

    /**
     * @brief This function returns of operator symbols in string format.
     *
     * @param operator enum of corresponding operator symbol
     * @return const char*
     */
    const char *EZPI_scenes_strings_comparator_operators_get_op(e_scene_str_cmp_operators_t operator);
    /**
     * @brief This function returns 'operator_name' of operator symbol
     *
     * @param operator enum of corresponding operator symbol
     * @return const char*
     */
    const char *EZPI_scenes_strings_comparator_operators_get_name(e_scene_str_cmp_operators_t operator);
    /**
     * @brief This function returns 'method_name' of operator symbol
     *
     * @param operator enum of corresponding operator symbol
     * @return const char*
     */
    const char *EZPI_scenes_strings_comparator_operators_get_method(e_scene_str_cmp_operators_t operator);
    /**
     * @brief  This function compare 'string_values' using given args
     *
     * @param devgrp_field block with 'device_group_field' info
     * @param itemgrp_field block with 'item_group_field' info
     * @param item_exp_field block with 'item_exp_field' info
     * @param value_field block with 'value_field' info
     * @param comparator_field block with 'comparator_field' info
     * @return int
     */
    int EZPI_scenes_operators_value_strings_operations(l_fields_v2_t *devgrp_field, l_fields_v2_t *itemgrp_field, l_fields_v2_t *item_exp_field, l_fields_v2_t *value_field, l_fields_v2_t *comparator_field);

    /**
     * @brief This function returns of 'operator' symbols in string format.
     *
     * @param operator enum of corresponding operator symbol
     * @return const char*
     */
    const char *EZPI_scenes_strops_comparator_operators_get_op(e_scene_strops_cmp_operators_t operator);
    /**
     * @brief This function returns of 'string_operation' symbols .
     *
     * @param operator enum of corresponding operator symbol
     * @return const char*
     */
    const char *EZPI_scenes_strops_comparator_operators_get_name(e_scene_strops_cmp_operators_t operator);
    /**
     * @brief This function returns 'method_name' of operator symbols .
     *
     * @param operator enum of corresponding operator symbol
     * @return const char*
     */
    const char *EZPI_scenes_strops_comparator_operators_get_method(e_scene_strops_cmp_operators_t operator);
    /**
     * @brief  This function compare 'string_operation' symbols using given args
     *
     * @param item_exp_field block with 'item_exp_field' info
     * @param value_field block with 'value_field' info
     * @param comparator_field block with 'comparator_field' info
     * @return int
     */
    int EZPI_scenes_operators_value_strops_operations(l_fields_v2_t *item_exp_field, l_fields_v2_t *value_field, l_fields_v2_t *comparator_field);
    /**
     * @brief This function compare 'string_operators' with device/item-groups using given args
     *
     * @param value_field block with 'value_field' info
     * @param operation_field block with 'operation_field' info
     * @param devgrp_field block with 'devgrp_field' info
     * @param itemgrp_field block with 'itemgrp_field' info
     * @return int
     */
    int EZPI_scenes_operators_value_strops_operations_with_group(l_fields_v2_t *value_field, l_fields_v2_t *operation_field, l_fields_v2_t *devgrp_field, l_fields_v2_t *itemgrp_field);

    /**
     * @brief This function perfroms 'in-array' operations using given args
     *
     * @param item_exp_field block with 'item_exp_field' info
     * @param value_field block with 'value_field' info
     * @param comparator_field block with 'comparator_field' info
     * @return int
     */
    int EZPI_scenes_operators_value_inarr_operations(l_fields_v2_t *item_exp_field, l_fields_v2_t *value_field, l_fields_v2_t *comparator_field);
    /**
     * @brief This function perfroms 'in-array' operations with device/item-groups using given args
     *
     * @param value_field block with 'value_field' info
     * @param operation_field block with 'operation_field' info
     * @param devgrp_field block with 'devgrp_field' info
     * @param itemgrp_field block with 'itemgrp_field' info
     * @return int
     */
    int EZPI_scenes_operators_value_inarr_operations_with_group(l_fields_v2_t *value_field, l_fields_v2_t *operation_field, l_fields_v2_t *devgrp_field, l_fields_v2_t *itemgrp_field);

    /**
     * @brief This function returns of 'operator' symbols in string format.
     *
     * @param operator  enum of corresponding operator symbol
     * @return const char*
     */
    const char *EZPI_scenes_value_with_less_comparator_operators_get_op(e_scene_value_with_less_cmp_operators_t operator);
    /**
     * @brief This function returns of symbols 'compare_oprs_with_less-than'  .
     *
     * @param operator  enum of corresponding operator symbol
     * @return const char*
     */
    const char *EZPI_scenes_value_with_less_comparator_operators_get_name(e_scene_value_with_less_cmp_operators_t operator);
    /**
     * @brief This function returns 'method_name' of 'compare_oprs_with_less-than' symbols .
     *
     * @param operator  enum of corresponding operator symbol
     * @return const char*
     */
    const char *EZPI_scenes_value_with_less_comparator_operators_get_method(e_scene_value_with_less_cmp_operators_t operator);

    /**
     * @brief This function returns of 'operator' symbols in string format.
     *
     * @param operator  enum of corresponding operator symbol
     * @return const char*
     */
    const char *EZPI_scenes_value_without_less_comparator_operators_get_op(e_scene_value_without_less_cmp_operators_t operator);
    /**
     * @brief This function returns of symbols 'compare_oprs_without_less-than'  .
     *
     * @param operator  enum of corresponding operator symbol
     * @return const char*
     */
    const char *EZPI_scenes_value_without_less_comparator_operators_get_name(e_scene_value_without_less_cmp_operators_t operator);
    /**
     * @brief This function returns 'method_name' of 'compare_oprs_without_less-than' symbols .
     *
     * @param operator  enum of corresponding operator symbol
     * @return const char*
     */
    const char *EZPI_scenes_value_without_less_comparator_operators_get_method(e_scene_value_without_less_cmp_operators_t operator);

    /**
     * @brief This function compare item_values using comparision operators with 'less-than' sign
     *
     * @param item_exp_field  block with 'item_exp_field' info
     * @param value_field block with 'value_field' info
     * @param value_type_field block with 'value_type_field' info
     * @param comparator_field block with 'comparator_field' info
     * @return int
     */
    int EZPI_scenes_operators_value_comparevalues_with_less_operations(l_fields_v2_t *item_exp_field, l_fields_v2_t *value_field, l_fields_v2_t *value_type_field, l_fields_v2_t *comparator_field);
    /**
     * @brief  This function compare item_values using comparision operators without 'less-than' sign
     *
     * @param value_field block with 'value_field' info
     * @param value_type_field block with 'value_type_field' info
     * @param comparator_field block with 'comparator_field' info
     * @param devgrp_field block with 'devgrp_field' info
     * @param itemgrp_field block with 'itemgrp_field' info
     * @return int
     */
    int EZPI_scenes_operators_value_comparevalues_with_less_operations_with_group(l_fields_v2_t *value_field, l_fields_v2_t *value_type_field, l_fields_v2_t *comparator_field, l_fields_v2_t *devgrp_field, l_fields_v2_t *itemgrp_field);

    /**
     * @brief This function determines if there is atleast one desired 'dictionary-type' value in perticular 'item_id'.
     *
     * @param item_id Compare with value from item with '_id'
     * @param value_field block with 'value_field' info
     * @return int
     */
    int EZPI_scenes_operators_has_atleastone_dictionary_value_operations(uint32_t item_id, l_fields_v2_t *value_field);
    /**
     * @brief This funciton compare previous and new dictionary values of perticular 'item_id'
     *
     * @param scene_node Pointer to scene_node
     * @param item_id  Id of target item
     * @param key_field Block containing 'Key' information
     * @param operation_field Block containing type of operation we need to perform
     * @return int
     */
    int EZPI_scenes_operators_is_dictionary_changed_operations(l_scenes_list_v2_t *scene_node, uint32_t item_id, l_fields_v2_t *key_field, l_fields_v2_t *operation_field);

#ifdef __cplusplus
}
#endif

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#endif // _EZLOPI_CORE_SCENES_OPERATORS_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
