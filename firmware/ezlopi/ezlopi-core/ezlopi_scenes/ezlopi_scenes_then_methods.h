#ifndef __EZLOPI_SCENES_THEN_METHODS_H__
#define __EZLOPI_SCENES_THEN_METHODS_H__

#include "ezlopi_scenes_v2.h"

int ezlopi_scene_then_set_item_value(l_scenes_list_v2_t *scene_node, void *arg);
int ezlopi_scene_then_set_device_armed(l_scenes_list_v2_t *scene_node, void *arg);
int ezlopi_scene_then_send_cloud_abstract_command(l_scenes_list_v2_t *scene_node, void *arg);
int ezlopi_scene_then_switch_house_mode(l_scenes_list_v2_t *scene_node, void *arg);
int ezlopi_scene_then_send_http_request(l_scenes_list_v2_t *scene_node, void *arg);
int ezlopi_scene_then_run_custom_script(l_scenes_list_v2_t *scene_node, void *arg);
int ezlopi_scene_then_run_plugin_script(l_scenes_list_v2_t *scene_node, void *arg);
int ezlopi_scene_then_run_scene(l_scenes_list_v2_t *scene_node, void *arg);
int ezlopi_scene_then_set_scene_state(l_scenes_list_v2_t *scene_node, void *arg);
int ezlopi_scene_then_reset_latch(l_scenes_list_v2_t *scene_node, void *arg);
int ezlopi_scene_then_reset_scene_latches(l_scenes_list_v2_t *scene_node, void *arg);
int ezlopi_scene_then_reboot_hub(l_scenes_list_v2_t *scene_node, void *arg);
int ezlopi_scene_then_reset_hub(l_scenes_list_v2_t *scene_node, void *arg);
int ezlopi_scene_then_cloud_api(l_scenes_list_v2_t *scene_node, void *arg);
int ezlopi_scene_then_set_expression(l_scenes_list_v2_t *scene_node, void *arg);
int ezlopi_scene_then_set_variable(l_scenes_list_v2_t *scene_node, void *arg);
int ezlopi_scene_then_toggle_value(l_scenes_list_v2_t *scene_node, void *arg);

#endif // __EZLOPI_SCENES_THEN_METHODS_H__
