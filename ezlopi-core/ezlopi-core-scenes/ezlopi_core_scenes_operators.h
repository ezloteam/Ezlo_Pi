#ifndef _EZLOPI_CORE_SCENES_OPERATORS_H_
#define _EZLOPI_CORE_SCENES_OPERATORS_H_

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include <string.h>
#include <stdint.h>

#include "ezlopi_core_scenes_v2.h"

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


typedef enum e_scene_inarr_cmp_operators
{
#define SCENES_IN_ARRAY_OPERATORS(OPERATOR, op, name, method) SCENES_IN_ARRAY_OPERATORS_##OPERATOR,
#include "__operators_macros/__in_array_comparision_operators_macros.h"
#undef SCENES_IN_ARRAY_OPERATORS
} e_scene_inarr_cmp_operators_t;

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

//-------------------------------------------------------------------
// Numeric Operators
e_scene_num_cmp_operators_t ezlopi_scenes_numeric_comparator_operators_get_enum(char* operator_str);
const char* ezlopi_scenes_numeric_comparator_operators_get_op(e_scene_num_cmp_operators_t operator);
const char* ezlopi_scenes_numeric_comparator_operators_get_name(e_scene_num_cmp_operators_t operator);
const char* ezlopi_scenes_numeric_comparator_operators_get_method(e_scene_num_cmp_operators_t operator);

int ezlopi_scenes_operators_value_number_operations(l_fields_v2_t* item_exp_field, l_fields_v2_t* value_field, l_fields_v2_t* comparator_field);

//-------------------------------------------------------------------
//Number-Range Operators
int ezlopi_scenes_operators_value_number_range_operations(l_fields_v2_t* item_exp_field, l_fields_v2_t* start_value_field, l_fields_v2_t* end_value_field, bool comparator_choice);


//-------------------------------------------------------------------
// Strings Operators
e_scene_str_cmp_operators_t ezlopi_scenes_strings_comparator_operators_get_enum(char* operator_str);
const char* ezlopi_scenes_strings_comparator_operators_get_op(e_scene_str_cmp_operators_t operator);
const char* ezlopi_scenes_strings_comparator_operators_get_name(e_scene_str_cmp_operators_t operator);
const char* ezlopi_scenes_strings_comparator_operators_get_method(e_scene_str_cmp_operators_t operator);

int ezlopi_scenes_operators_value_strings_operations(l_fields_v2_t* item_exp_field, l_fields_v2_t* value_field, l_fields_v2_t* comparator_field);

//-------------------------------------------------------------------
// String-Operation Operators
e_scene_strops_cmp_operators_t ezlopi_scenes_strops_comparator_operators_get_enum(char* operator_str);
const char* ezlopi_scenes_strops_comparator_operators_get_op(e_scene_strops_cmp_operators_t operator);
const char* ezlopi_scenes_strops_comparator_operators_get_name(e_scene_strops_cmp_operators_t operator);
const char* ezlopi_scenes_strops_comparator_operators_get_method(e_scene_strops_cmp_operators_t operator);

int ezlopi_scenes_operators_value_strops_operations(l_fields_v2_t* item_exp_field, l_fields_v2_t* value_field, l_fields_v2_t* comparator_field);

//-------------------------------------------------------------------
// InArray Operators
#if 0
// e_scene_inarr_cmp_operators_t ezlopi_scenes_inarr_comparator_operators_get_enum(char* operator_inarr);
// const char* ezlopi_scenes_inarr_comparator_operators_get_op(e_scene_inarr_cmp_operators_t operator);
// const char* ezlopi_scenes_inarr_comparator_operators_get_name(e_scene_inarr_cmp_operators_t operator);
// const char* ezlopi_scenes_inarr_comparator_operators_get_method(e_scene_inarr_cmp_operators_t operator);
#endif

int ezlopi_scenes_operators_value_inarr_operations(l_fields_v2_t* item_exp_field, l_fields_v2_t* value_field, l_fields_v2_t* comparator_field);

// Value with less Operators
e_scene_value_with_less_cmp_operators_t ezlopi_scenes_value_with_less_comparator_operators_get_enum(char* operator_str);
const char* ezlopi_scenes_value_with_less_comparator_operators_get_op(e_scene_value_with_less_cmp_operators_t operator);
const char* ezlopi_scenes_value_with_less_comparator_operators_get_name(e_scene_value_with_less_cmp_operators_t operator);
const char* ezlopi_scenes_value_with_less_comparator_operators_get_method(e_scene_value_with_less_cmp_operators_t operator);

// int ezlopi_scenes_operators_value_with_less_operations(uint32_t item_id, l_fields_v2_t* value_field, l_fields_v2_t* comparator_field);

// Value without less Operators
e_scene_value_without_less_cmp_operators_t ezlopi_scenes_value_without_less_comparator_operators_get_enum(char* operator_str);
const char* ezlopi_scenes_value_without_less_comparator_operators_get_op(e_scene_value_without_less_cmp_operators_t operator);
const char* ezlopi_scenes_value_without_less_comparator_operators_get_name(e_scene_value_without_less_cmp_operators_t operator);
const char* ezlopi_scenes_value_without_less_comparator_operators_get_method(e_scene_value_without_less_cmp_operators_t operator);

// int ezlopi_scenes_operators_value_without_less_operations(uint32_t item_id, l_fields_v2_t* value_field, l_fields_v2_t* comparator_field);

// CompareValues Operators 'without-less' (default)
// int ezlopi_scenes_operators_value_comparevalues_without_less_operations(uint32_t item_id, l_fields_v2_t* value_field, l_fields_v2_t* value_type_field, l_fields_v2_t* comparator_field);
int ezlopi_scenes_operators_value_comparevalues_with_less_operations(l_fields_v2_t* item_exp_field, l_fields_v2_t* value_field, l_fields_v2_t* value_type_field, l_fields_v2_t* comparator_field);

// CompareNumberRange Operators
int ezlopi_scenes_operators_value_number_range_operations(l_fields_v2_t * item_exp_field, l_fields_v2_t * start_value_field, l_fields_v2_t * end_value_field, bool comparator_choice);

// HasAtleastOneDictionaryValue
int ezlopi_scenes_operators_has_atleastone_dictionary_value_operations(uint32_t item_id, l_fields_v2_t* value_field);
// IsDictornaryChanged
int ezlopi_scenes_operators_is_dictionary_changed_operations(l_scenes_list_v2_t* scene_node, uint32_t item_id, l_fields_v2_t* key_field, l_fields_v2_t* operation_field);

#endif  // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#endif // _EZLOPI_CORE_SCENES_OPERATORS_H_