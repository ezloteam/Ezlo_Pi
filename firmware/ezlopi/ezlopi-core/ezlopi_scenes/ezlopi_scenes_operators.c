#include <string.h>
#include "ezlopi_scenes_operators.h"

static const char *const ezlopi_scenes_operators_op[] = {
#define SCENES_OPERATORS(OPERATOR, op, name, method) op,
#include "ezlopi_scenes_operators_macros.h"
#undef SCENES_OPERATORS
};

#if 0
static const char *const ezlopi_scenes_operators_name[] = {
#define SCENES_OPERATORS(OPERATOR, op, name, method) name,
#include "ezlopi_scenes_operators.h"
#undef SCENES_OPERATORS
};

static const char *const ezlopi_scenes_operators_method[] = {
#define SCENES_OPERATORS(OPERATOR, op, name, method) method,
#include "ezlopi_scenes_operators.h"
#undef SCENES_OPERATORS
};
#endif

const char *ezlopi_scenes_operators_get_op(e_scene_cmp_operators_t operator)
{
    const char *ret = NULL;
    if (operator> SCENES_OPERATORS_NONE && operator<SCENES_OPERATORS_MAX)
    {
        ret = ezlopi_scenes_operators_op[operator];
    }
    return ret;
}

#if 0
const char *ezlopi_scenes_operators_get_name(e_scene_cmp_operators_t operator)
{
    const char *ret = NULL;
    if (operator> SCENES_OPERATORS_NONE && operator<SCENES_OPERATORS_MAX)
    {
        ret = ezlopi_scenes_operators_name[operator];
    }
    return ret;
}

const char *ezlopi_scenes_operators_get_method(e_scene_cmp_operators_t operator)
{
    const char *ret = NULL;
    if (operator> SCENES_OPERATORS_NONE && operator<SCENES_OPERATORS_MAX)
    {
        ret = ezlopi_scenes_operators_method[operator];
    }
    return ret;
}
#endif
