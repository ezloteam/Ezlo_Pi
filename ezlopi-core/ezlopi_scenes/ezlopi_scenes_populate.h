#ifndef __EZLOPI_SCENES_POPULATE_H__
#define __EZLOPI_SCENES_POPULATE_H__

#include <cJSON.h>
#include <stdint.h>

#include "ezlopi_scenes_v2.h"

l_when_block_v2_t *ezlopi_scenes_populate_new_when_block(cJSON *cj_when_block);

l_fields_v2_t *ezlopi_scenes_populate_new_field(cJSON *cj_field);
l_fields_v2_t *ezlopi_scenes_populate_fields(cJSON *cj_fields);

void ezlopi_scenes_populate_new_action_delay(s_action_delay_v2_t *action_delay, cJSON *cj_delay);
void ezlopi_scenes_populate_new_block_options(s_block_options_v2_t *p_block_options, cJSON *cj_block_options);
void ezlopi_scenes_populate_new_method(s_method_v2_t *p_method, cJSON *cj_method);

l_when_block_v2_t *ezlopi_scenes_populate_new_when_block(cJSON *cj_when_block);
l_when_block_v2_t *ezlopi_scenes_populate_when_blocks(cJSON *cj_when_blocks);

l_action_block_v2_t *ezlopi_scenes_populate_new_action_block(cJSON *cj_then_block, e_scenes_block_type_v2_t block_type);
l_action_block_v2_t *ezlopi_scenes_populate_action_blocks(cJSON *cj_then_blocks, e_scenes_block_type_v2_t block_type);

l_house_modes_v2_t *ezlopi_scenes_populate_new_house_mode(cJSON *cj_house_mode);
l_house_modes_v2_t *ezlopi_scenes_populate_house_modes(cJSON *cj_house_modes);

l_user_notification_v2_t *ezlopi_scenes_populate_new_user_notification(cJSON *cj_user_notification);
l_user_notification_v2_t *ezlopi_scenes_populate_user_notifications(cJSON *cj_user_notifications);

l_scenes_list_v2_t *ezlopi_scenes_populate_new_scene(cJSON *cj_scene, uint32_t scene_id);
l_scenes_list_v2_t *ezlopi_scenes_populate_scenes(cJSON *cj_scene, uint32_t scene_id);

#endif // __EZLOPI_SCENES_POPULATE_H__
