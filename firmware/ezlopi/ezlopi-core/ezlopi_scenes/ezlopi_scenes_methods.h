#ifndef __EZLOPI_SCENES_METHODS_H__
#define __EZLOPI_SCENES_METHODS_H__

#include "ezlopi_scenes.h"

/* When functions */
int ezlopi_scene_when_is_item_state(void *a_when_block);
int ezlopi_scene_when_is_interval(l_scenes_list_t *curr_scene);
int ezlopi_scene_when_is_item_state_changed(l_scenes_list_t *curr_scene);
int ezlopi_scene_when_is_button_state(l_scenes_list_t *curr_scene);
int ezlopi_scene_when_is_sun_state(l_scenes_list_t *curr_scene);
int ezlopi_scene_when_is_date(l_scenes_list_t *curr_scene);
int ezlopi_scene_when_is_once(l_scenes_list_t *curr_scene);
int ezlopi_scene_when_is_date_range(l_scenes_list_t *curr_scene);
int ezlopi_scene_when_is_user_lock_operation(l_scenes_list_t *curr_scene);
int ezlopi_scene_when_is_house_mode_changed_to(l_scenes_list_t *curr_scene);
int ezlopi_scene_when_is_house_mode_changed_from(l_scenes_list_t *curr_scene);
int ezlopi_scene_when_is_device_state(l_scenes_list_t *curr_scene);
int ezlopi_scene_when_is_network_state(l_scenes_list_t *curr_scene);
int ezlopi_scene_when_is_scene_state(l_scenes_list_t *curr_scene);
int ezlopi_scene_when_is_group_state(l_scenes_list_t *curr_scene);
int ezlopi_scene_when_is_cloud_state(l_scenes_list_t *curr_scene);
int ezlopi_scene_when_is_battery_state(l_scenes_list_t *curr_scene);
int ezlopi_scene_when_is_battery_level(l_scenes_list_t *curr_scene);
int ezlopi_scene_when_compare_numbers(l_scenes_list_t *curr_scene);
int ezlopi_scene_when_compare_number_range(l_scenes_list_t *curr_scene);
int ezlopi_scene_when_compare_strings(l_scenes_list_t *curr_scene);
int ezlopi_scene_when_string_operation(l_scenes_list_t *curr_scene);
int ezlopi_scene_when_in_array(l_scenes_list_t *curr_scene);
int ezlopi_scene_when_compare_values(l_scenes_list_t *curr_scene);
int ezlopi_scene_when_has_atleast_one_dictionary_value(l_scenes_list_t *curr_scene);
int ezlopi_scene_when_is_firmware_update_state(l_scenes_list_t *curr_scene);
int ezlopi_scene_when_is_dictionary_changed(l_scenes_list_t *curr_scene);
int ezlopi_scene_when_is_detected_in_hot_zone(l_scenes_list_t *curr_scene);
int ezlopi_scene_when_and(l_scenes_list_t *curr_scene);
int ezlopi_scene_when_not(l_scenes_list_t *curr_scene);
int ezlopi_scene_when_or(l_scenes_list_t *curr_scene);
int ezlopi_scene_when_xor(l_scenes_list_t *curr_scene);
int ezlopi_scene_when_function(l_scenes_list_t *curr_scene);

/* then functions */
int ezlopi_scene_then_set_item_value(l_scenes_list_t *curr_scene);
int ezlopi_scene_then_set_device_armed(l_scenes_list_t *curr_scene);
int ezlopi_scene_then_send_cloud_abstract_command(l_scenes_list_t *curr_scene);
int ezlopi_scene_then_switch_house_mode(l_scenes_list_t *curr_scene);
int ezlopi_scene_then_send_http_request(l_scenes_list_t *curr_scene);
int ezlopi_scene_then_run_custom_script(l_scenes_list_t *curr_scene);
int ezlopi_scene_then_run_plugin_script(l_scenes_list_t *curr_scene);
int ezlopi_scene_then_run_scene(l_scenes_list_t *curr_scene);
int ezlopi_scene_then_set_scene_state(l_scenes_list_t *curr_scene);
int ezlopi_scene_then_reset_latch(l_scenes_list_t *curr_scene);
int ezlopi_scene_then_reset_scene_latches(l_scenes_list_t *curr_scene);
int ezlopi_scene_then_reboot_hub(l_scenes_list_t *curr_scene);
int ezlopi_scene_then_reset_hub(l_scenes_list_t *curr_scene);
int ezlopi_scene_then_cloud_api(l_scenes_list_t *curr_scene);
int ezlopi_scene_then_set_expression(l_scenes_list_t *curr_scene);
int ezlopi_scene_then_set_variable(l_scenes_list_t *curr_scene);
int ezlopi_scene_then_toggle_value(l_scenes_list_t *curr_scene);

#endif // __EZLOPI_SCENES_METHODS_H__
