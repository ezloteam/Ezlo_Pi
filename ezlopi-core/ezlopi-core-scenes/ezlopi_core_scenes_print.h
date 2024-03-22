#ifndef __EZLOPI_CORE_SCENES_PRINT_H__
#define __EZLOPI_CORE_SCENES_PRINT_H__

#include "ezlopi_core_scenes_v2.h"

void ezlopi_print_block_options(s_block_options_v2_t* block_options, l_fields_v2_t* fields);
void ezlopi_print_fields(l_fields_v2_t* fields);
void ezlopi_print_house_modes(l_house_modes_v2_t* house_modes);
void ezlopi_print_user_notifications(l_user_notification_v2_t* user_notification);
void ezlopi_print_when_blocks(l_when_block_v2_t* when_blocks);
void ezlopi_print_action_blocks(l_action_block_v2_t* action_block);
void ezlopi_scenes_print(l_scenes_list_v2_t* scene_link_list);

#endif // __EZLOPI_CORE_SCENES_PRINT_H__