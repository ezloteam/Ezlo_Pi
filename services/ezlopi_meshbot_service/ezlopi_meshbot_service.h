#ifndef __EZLOPI_SCENES_SERVICE__
#define __EZLOPI_SCENES_SERVICE__

#include "stdint.h"
#include "ezlopi_scenes_v2.h"

void ezlopi_scenes_meshbot_init(void);
uint32_t ezlopi_scenes_service_run_by_id(uint32_t _id);
uint32_t ezlopi_meshbot_service_start_scene(l_scenes_list_v2_t *scene_node);

uint32_t ezlopi_meshbot_service_stop_for_scene_id(uint32_t _id);
uint32_t ezlopi_meshobot_service_stop_scene(l_scenes_list_v2_t *scene_node);

#endif // __EZLOPI_SCENES_SERVICE__
