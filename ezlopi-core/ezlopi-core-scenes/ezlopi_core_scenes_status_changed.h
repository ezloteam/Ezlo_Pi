#ifndef _EZLOPI_CORE_SCENES_STATUS_CHANGED_H_
#define _EZLOPI_CORE_SCENES_STATUS_CHANGED_H_

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include "ezlopi_core_scenes_v2.h"

const char* ezlopi_scenes_status_to_string(e_scene_status_v2_t scene_status);
int ezlopi_scenes_status_change_broadcast(l_scenes_list_v2_t* scene_node, const char* status_str);

#endif  // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#endif // _EZLOPI_CORE_SCENES_STATUS_CHANGED_H_
