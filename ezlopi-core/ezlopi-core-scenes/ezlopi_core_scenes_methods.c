#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include "ezlopi_util_trace.h"

#include "ezlopi_core_scenes_v2.h"
#include "ezlopi_core_scenes_methods.h"

static const char *ezlopi_scenes_methods_name[] = {
#define EZLOPI_SCENE(method_type, name, func, category) name,
#include "ezlopi_core_scenes_method_types.h"
#undef EZLOPI_SCENE
};

e_scenes_method_category_t ezlopi_scenes_method_category_enum[] = {
#define EZLOPI_SCENE(method_type, name, func, category) category,
#include "ezlopi_core_scenes_method_types.h"
#undef EZLOPI_SCENE
};


const char *ezlopi_scene_get_scene_method_name(e_scene_method_type_t method_type)
{
    const char *ret = NULL;
    if ((method_type > EZLOPI_SCENE_METHOD_TYPE_NONE) && (method_type < EZLOPI_SCENE_METHOD_TYPE_MAX))
    {
        ret = ezlopi_scenes_methods_name[method_type];
    }
    return ret;
}

e_scene_method_type_t ezlopi_scenes_method_get_type_enum(char *method_name)
{
    e_scene_method_type_t method_type = EZLOPI_SCENE_METHOD_TYPE_NONE;
    if (method_name)
    {
        size_t method_len = strlen(method_name);
        for (e_scene_method_type_t i = EZLOPI_SCENE_WHEN_METHOD_IS_ITEM_STATE; i < EZLOPI_SCENE_METHOD_TYPE_MAX; i++)
        {
            size_t max_len = (method_len > strlen(ezlopi_scenes_methods_name[i])) ? method_len : strlen(ezlopi_scenes_methods_name[i]);
            if (0 == strncmp(method_name, ezlopi_scenes_methods_name[i], max_len))
            {
                method_type = i;
                break;
            }
        }
    }

    return method_type;
}

e_scenes_method_category_t ezlopi_scene_get_scene_method_category_enum(char *method_name)
{
    e_scenes_method_category_t ret = METHOD_CATEGORY_NAN;
    if (method_name)
    {
        e_scene_method_type_t method_type = ezlopi_scenes_method_get_type_enum(method_name);

        if ((method_type > EZLOPI_SCENE_METHOD_TYPE_NONE) && (method_type < EZLOPI_SCENE_METHOD_TYPE_MAX))
        {
            ret = ezlopi_scenes_method_category_enum[method_type];
        }
    }
    return ret;
}

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS