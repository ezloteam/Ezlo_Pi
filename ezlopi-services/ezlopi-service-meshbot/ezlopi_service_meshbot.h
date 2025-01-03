#ifndef _EZLOPI_SERVICE_MESHBOT_H_
#define _EZLOPI_SERVICE_MESHBOT_H_

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include <stdint.h>

#include "ezlopi_core_scenes_v2.h"

void ezlopi_scenes_meshbot_init(void);
uint32_t ezlopi_scenes_service_run_by_id(uint32_t _id);
uint32_t ezlopi_meshbot_service_start_scene(l_scenes_list_v2_t *scene_node);

ezlopi_error_t ezlopi_meshbot_stop_without_broadcast(l_scenes_list_v2_t *scene_node);
uint32_t ezlopi_meshbot_service_stop_for_scene_id(uint32_t _id);
uint32_t ezlopi_meshobot_service_stop_scene(l_scenes_list_v2_t *scene_node);

uint32_t ezlopi_meshbot_execute_scene_else_action_group(uint32_t scene_id);

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#endif // _EZLOPI_SERVICE_MESHBOT_H_
