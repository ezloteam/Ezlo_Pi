#ifndef _EZLOPI_CORE_SCENES_POPULATE_H_
#define _EZLOPI_CORE_SCENES_POPULATE_H_

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include "cjext.h"
#include <stdint.h>

#include "ezlopi_core_scenes_v2.h"

l_fields_v2_t* ezlopi_scenes_populate_fields(cJSON* cj_fields);
void ezlopi_scenes_populate_assign_field(l_fields_v2_t* new_field, cJSON* cj_field);

void ezlopi_scenes_populate_assign_action_delay(s_action_delay_v2_t* action_delay, cJSON* cj_delay);
void ezlopi_scenes_populate_assign_block_options(s_block_options_v2_t* p_block_options, cJSON* cj_block_options);
void ezlopi_scenes_populate_assign_method(s_method_v2_t* p_method, cJSON* cj_method);

l_when_block_v2_t* ezlopi_scenes_populate_when_blocks(cJSON* cj_when_blocks);
void ezlopi_scenes_populate_assign_when_block(l_when_block_v2_t* new_when_block, cJSON* cj_when_block);

l_action_block_v2_t* ezlopi_scenes_populate_action_blocks(cJSON* cj_then_blocks, e_scenes_block_type_v2_t block_type);
void ezlopi_scenes_populate_assign_action_block(l_action_block_v2_t* new_action_block, cJSON* cj_action_block, e_scenes_block_type_v2_t block_type);

l_house_modes_v2_t* ezlopi_scenes_populate_house_modes(cJSON* cj_house_modes);
void ezlopi_scenes_populate_assign_house_mode(l_house_modes_v2_t* new_house_mode, cJSON* cj_house_mode);

l_user_notification_v2_t* ezlopi_scenes_populate_user_notifications(cJSON* cj_user_notifications);
void ezlopi_scenes_populate_assign_user_notification(l_user_notification_v2_t* new_user_notification, cJSON* cj_user_notification);

void ezlopi_scenes_populate_scene(l_scenes_list_v2_t* new_scene, cJSON* cj_scene, uint32_t scene_id);

#endif  // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#endif // _EZLOPI_CORE_SCENES_POPULATE_H_
