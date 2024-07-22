#ifndef __EZLOPI_CORE_SCENES_VALUE_H__
#define __EZLOPI_CORE_SCENES_VALUE_H__

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include "cjext.h"
#include "ezlopi_core_scenes_v2.h"

const char* ezlopi_scene_get_scene_value_type_name(e_scene_value_type_v2_t e_value_type);
e_scene_value_type_v2_t ezlopi_scene_get_scene_value_type_enum(const char * value_type_str);
e_scene_value_type_v2_t ezlopi_core_scenes_value_get_type(cJSON* cj_field, const char* type_key_str);

#endif  // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#endif // __EZLOPI_CORE_SCENES_VALUE_H__
