#ifndef _EZLOPI_CORE_SCENES_METHODS_H_
#define _EZLOPI_CORE_SCENES_METHODS_H_

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include <string.h>

typedef enum e_scene_method_type
{
#define EZLOPI_SCENE(method, name, func) EZLOPI_SCENE_##method,
#include "ezlopi_core_scenes_method_types.h"
#undef EZLOPI_SCENE
} e_scene_method_type_t;

e_scene_method_type_t ezlopi_scenes_method_get_type_enum(char* method_name);
const char* ezlopi_scene_get_scene_method_name(e_scene_method_type_t method_type);

/* then methods */

#endif  // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#endif // _EZLOPI_CORE_SCENES_METHODS_H_
