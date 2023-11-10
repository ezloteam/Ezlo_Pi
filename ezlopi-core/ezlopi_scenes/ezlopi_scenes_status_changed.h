#ifndef __EZLOPI_SCENES_STATUS_CHNAGED_H__
#define __EZLOPI_SCENES_STATUS_CHNAGED_H__

#include "ezlopi_scenes_v2.h"

const static char *scene_status_started_str = "started";
const static char *scene_status_finished_str = "finished";
const static char *scene_status_partially_finished_str = "partially_finished";
const static char *scene_status_failed_str = "failed";
const static char *scene_status_stopped_str = "stopped";

const char *ezlopi_scenes_status_to_string(e_scene_status_v2_t scene_status);
int ezlopi_scenes_status_change_broadcast(l_scenes_list_v2_t *scene_node, const char *status_str);

#endif // __EZLOPI_SCENES_STATUS_CHNAGED_H__
