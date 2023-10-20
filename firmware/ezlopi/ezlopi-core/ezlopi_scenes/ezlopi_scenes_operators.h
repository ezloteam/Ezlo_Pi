#ifndef __EZLOPI_SCENES_OPERATORS_H__
#define __EZLOPI_SCENES_OPERATORS_H__

#include <string.h>
#include "ezlopi_scenes_v2.h"

typedef enum e_scene_cmp_operators
{
#define SCENES_OPERATORS(OPERATOR, op, name, method) SCENES_OPERATORS_##OPERATOR,
#include "ezlopi_scenes_operators_macros.h"
#undef SCENES_OPERATORS
} e_scene_cmp_operators_t;

// Operators
char *ezlopi_scenes_operators_get_op(e_scene_cmp_operators_t operator);
char *ezlopi_scenes_operators_get_name(e_scene_cmp_operators_t operator);
const char *ezlopi_scenes_operators_get_method(e_scene_cmp_operators_t operator);

e_scene_cmp_operators_t ezlopi_scenes_operators_get_enum(char *operator_str);
int ezlopi_scenes_operators_compare_value_number(uint32_t item_id, l_fields_v2_t *value_field, l_fields_v2_t *comparator_field);

#endif // __EZLOPI_SCENES_OPERATORS_H__
