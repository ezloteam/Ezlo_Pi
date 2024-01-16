#ifndef __EZLOPI_SCENES_STATUS_CHNAGED_H__
#define __EZLOPI_SCENES_STATUS_CHNAGED_H__

#include "ezlopi_scenes_v2.h"

const char *ezlopi_scenes_status_to_string(e_scene_status_v2_t scene_status);
int ezlopi_scenes_status_change_broadcast(l_scenes_list_v2_t *scene_node, const char *status_str);

#endif // __EZLOPI_SCENES_STATUS_CHNAGED_H__
