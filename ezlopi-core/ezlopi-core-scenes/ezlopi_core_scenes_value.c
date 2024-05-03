#include "ezlopi_cloud_constants.h"

#include "ezlopi_core_scenes_v2.h"
#include "ezlopi_core_scenes_value.h"

static const char *sg_scenes_value_type_name[] = {
#define EZLOPI_VALUE_TYPE(type, name) name,
#include "ezlopi_core_scenes_value_types.h"
#undef EZLOPI_VALUE_TYPE
};

const char *ezlopi_scene_get_scene_value_type_name(e_scene_value_type_v2_t value_type)
{
    const char *ret = ezlopi__str;
    if ((value_type >= EZLOPI_VALUE_TYPE_NONE) && (value_type < EZLOPI_VALUE_TYPE_MAX))
    {
        ret = sg_scenes_value_type_name[value_type];
    }

    return ret;
}

e_scene_value_type_v2_t ezlopi_core_scenes_value_get_type(cJSON *cj_root, const char *type_key_str)
{
    char *value_type_str = NULL;
    e_scene_value_type_v2_t ret = EZLOPI_VALUE_TYPE_NONE;

    if (cj_root && (NULL == cj_root->valuestring))
    {
        cJSON *cj_value_type = cJSON_GetObjectItem(__FUNCTION__, cj_root, type_key_str);
        if (cj_value_type && cj_value_type->valuestring)
        {
            value_type_str = cj_value_type->valuestring;
        }
    }
    else if ((cj_root->type == cJSON_String) && (cj_root->valuestring))
    {
        value_type_str = cj_root->valuestring;
    }

    if (value_type_str)
    {
        for (int i = EZLOPI_VALUE_TYPE_NONE; i < EZLOPI_VALUE_TYPE_MAX; i++)
        {
            if (0 == strcmp(sg_scenes_value_type_name[i], value_type_str))
            {
                ret = i;
                break;
            }
        }
    }

    return ret;
}

// void ezlopi_core_scenes_value_parse()