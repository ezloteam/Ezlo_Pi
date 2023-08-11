#ifndef __EZLOPI_SCENES_OPERATORS_H__
#define __EZLOPI_SCENES_OPERATORS_H__

#include "string.h"

typedef enum e_scene_cmp_operators
{
#define SCENES_OPERATORS(OPERATOR, op, name, method) SCENES_OPERATORS_##OPERATOR,
#include "ezlopi_scenes_operators_macros.h"
#undef SCENES_OPERATORS
} e_scene_cmp_operators_t;

// Operators
const char *ezlopi_scenes_operators_get_op(e_scene_cmp_operators_t operator);
const char *ezlopi_scenes_operators_get_name(e_scene_cmp_operators_t operator);
const char *ezlopi_scenes_operators_get_method(e_scene_cmp_operators_t operator);

#endif // __EZLOPI_SCENES_OPERATORS_H__
