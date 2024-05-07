#ifndef _EZLOPI_CORE_SCENES_WHEN_METHODS_H_
#define _EZLOPI_CORE_SCENES_WHEN_METHODS_H_

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include "ezlopi_core_scenes_v2.h"

/* When methods */
int ezlopi_scene_when_is_item_state(l_scenes_list_v2_t* scene_node, void* arg);
int ezlopi_scene_when_is_interval(l_scenes_list_v2_t* scene_node, void* arg);
int ezlopi_scene_when_is_item_state_changed(l_scenes_list_v2_t* scene_node, void* arg);
int ezlopi_scene_when_is_button_state(l_scenes_list_v2_t* scene_node, void* arg);
int ezlopi_scene_when_is_sun_state(l_scenes_list_v2_t* scene_node, void* arg);
int ezlopi_scene_when_is_date(l_scenes_list_v2_t* scene_node, void* arg);
int ezlopi_scene_when_is_once(l_scenes_list_v2_t* scene_node, void* arg);
int ezlopi_scene_when_is_date_range(l_scenes_list_v2_t* scene_node, void* arg);
int ezlopi_scene_when_is_user_lock_operation(l_scenes_list_v2_t* scene_node, void* arg);
int ezlopi_scene_when_is_house_mode_changed_to(l_scenes_list_v2_t* scene_node, void* arg);
int ezlopi_scene_when_is_house_mode_changed_from(l_scenes_list_v2_t* scene_node, void* arg);
int ezlopi_scene_when_is_House_Mode_Switch_to_Range(l_scenes_list_v2_t* scene_node, void* arg);
int ezlopi_scene_when_is_House_Mode_Alarm_Phase_Range(l_scenes_list_v2_t* scene_node, void* arg);
int ezlopi_scene_when_is_device_state(l_scenes_list_v2_t* scene_node, void* arg);
int ezlopi_scene_when_is_network_state(l_scenes_list_v2_t* scene_node, void* arg);
int ezlopi_scene_when_is_scene_state(l_scenes_list_v2_t* scene_node, void* arg);
int ezlopi_scene_when_is_group_state(l_scenes_list_v2_t* scene_node, void* arg);
int ezlopi_scene_when_is_cloud_state(l_scenes_list_v2_t* scene_node, void* arg);
int ezlopi_scene_when_is_battery_state(l_scenes_list_v2_t* scene_node, void* arg);
int ezlopi_scene_when_is_battery_level(l_scenes_list_v2_t* scene_node, void* arg);
int ezlopi_scene_when_compare_numbers(l_scenes_list_v2_t* scene_node, void* arg);
int ezlopi_scene_when_compare_number_range(l_scenes_list_v2_t* scene_node, void* arg);
int ezlopi_scene_when_compare_strings(l_scenes_list_v2_t* scene_node, void* arg);
int ezlopi_scene_when_string_operation(l_scenes_list_v2_t* scene_node, void* arg);
int ezlopi_scene_when_in_array(l_scenes_list_v2_t* scene_node, void* arg);
int ezlopi_scene_when_compare_values(l_scenes_list_v2_t* scene_node, void* arg);
int ezlopi_scene_when_has_atleast_one_dictionary_value(l_scenes_list_v2_t* scene_node, void* arg);
int ezlopi_scene_when_is_firmware_update_state(l_scenes_list_v2_t* scene_node, void* arg);
int ezlopi_scene_when_is_dictionary_changed(l_scenes_list_v2_t* scene_node, void* arg);
int ezlopi_scene_when_is_detected_in_hot_zone(l_scenes_list_v2_t* scene_node, void* arg);
int ezlopi_scene_when_and(l_scenes_list_v2_t* scene_node, void* arg);
int ezlopi_scene_when_not(l_scenes_list_v2_t* scene_node, void* arg);
int ezlopi_scene_when_or(l_scenes_list_v2_t* scene_node, void* arg);
int ezlopi_scene_when_xor(l_scenes_list_v2_t* scene_node, void* arg);
int ezlopi_scene_when_function(l_scenes_list_v2_t* scene_node, void* arg);

#endif  // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#endif // _EZLOPI_CORE_SCENES_WHEN_METHODS_H_
