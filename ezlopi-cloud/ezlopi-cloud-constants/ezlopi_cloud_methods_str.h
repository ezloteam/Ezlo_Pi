/* ===========================================================================
** Copyright (C) 2022 Ezlo Innovation Inc
**
** Under EZLO AVAILABLE SOURCE LICENSE (EASL) AGREEMENT
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/

/**
 * @file    ezlopi_cloud_methods_str.h
 * @brief
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 *          Lomas Subedi
 *          Riken Maharjan
 *          Nabin Dangi
 * @version 1.0
 * @date    October 13th, 2022 7:24 PM
 */
#ifndef __EZLOPI_METHODS_STR_H__
#define __EZLOPI_METHODS_STR_H__

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

/*******************************************************************************
 *                          C++ Declaration Wrapper
 *******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

    /*******************************************************************************
     *                          Type & Macro Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*****************************************************************************************
     *                                     Methods                                           *
     *****************************************************************************************/
    extern const char *method_hub_item_updated;
    extern const char *method_add_device;
    extern const char *method_add_all_devices;
    extern const char *method_cloud_firmware_info_get;
    // extern const  char *method_cloud_access_keys_controller_sync;
    // extern const  char *method_cloud_timezones_get
    // extern const  char *method_cloud_controller_abstracts_set;
    // extern const  char *method_cloud_controller_abstracts_list;
    // extern const  char *method_cloud_storage_controller_token;
    // extern const  char *method_extensions_plugin_info_installed;
    // extern const  char *method_extensions_plugin_install_execute;
    // extern const  char *method_extensions_plugin_install_request;
    // extern const  char *method_extensions_plugin_run ;
    // extern const  char *method_extensions_plugin_uninstall_execute ;
    // extern const  char *method_extensions_plugin_uninstall_request ;
    // extern const  char *method_hub_offline_credentials_updated;
    // extern const  char *method_hub_bridge_controller_connection_list ;
    // extern const  char *method_hub_bridge_controller_connection_set ;
    // extern const  char *method_hub_coordinates_get ;
    // extern const  char *method_hub_coordinates_set;
    // extern const  char *method_hub_description_set;
    // extern const  char *method_hub_device_force_remove ;
    // extern const  char *method_hub_device_firmware_update ;
    extern const char *method_hub_devices_list;
    extern const char *method_hub_device_armed_set;
    // extern const  char *method_hub_device_name_set;
    extern const char *method_hub_device_room_set;
    // extern const  char *method_hub_device_setting_dictionary_value_set ;
    // extern const  char *method_hub_device_setting_value_set ;
    // extern const  char *method_hub_gateway_setting_value_set ;
    // extern const  char *method_hub_detection_device_add;
    // extern const  char *method_hub_detection_devices_list ;
    // extern const  char *method_hub_detection_device_remove ;
    // extern const  char *method_hub_detection_device_set ;
    extern const char *method_hub_setting_updated;
    // extern const  char *method_hub_device_setting_reset ;
    // extern const  char *method_hub_device_settings_list;
    // extern const  char *method_hub_setting_value_set_request;
    // extern const  char *method_hub_setting_value_reset_request ;
    extern const char *method_hub_device_setting_updated;
    // extern const  char *method_hub_extensions_plugin_info_installed ;
    // extern const  char *method_hub_extensions_plugin_install_execute ;
    // extern const  char *method_hub_software_update_execute ;
    // extern const  char *method_hub_extensions_plugin_install_request ;
    // extern const  char *method_hub_extensions_plugin_run ;
    // extern const  char *method_hub_extensions_plugin_uninstall_execute;
    // extern const  char *method_hub_extensions_plugin_uninstall_request;
    extern const char *method_hub_favorite_list;
    extern const char *method_hub_favorite_set;
    extern const char *method_hub_firmware_update;
    extern const char *method_hub_info_get;
    extern const char *method_hub_data_value_list_request;
    extern const char *method_hub_settings_list_request;
    extern const char *method_hub_features_list;
    extern const char *method_hub_feature_status_set;
    extern const char *method_hub_gateways_list;
    extern const char *method_hub_geteway_settings_list;
    extern const char *method_hub_item_dictionary_clear;
    extern const char *method_hub_item_dictionary_value_add;
    extern const char *method_hub_item_dictionary_value_remove;
    extern const char *method_hub_items_list;
    extern const char *method_hub_item_dictionary_value_set;
    extern const char *method_hub_item_value_set;
    extern const char *method_hub_item_value_percent_apply;
    extern const char *method_hub_modes_get;
    // extern const  char *method_hub_modes_current_get;
    // extern const  char *method_hub_modes_switch ;
    // extern const  char *method_hub_modes_switch_to_delay_set;
    // extern const  char *method_hub_modes_alarm_delay_set ;
    extern const char *method_hub_modes_alarms_off_add;
    extern const char *method_hub_modes_alarms_off_remove;
    // extern const  char *method_hub_modes_switch_cancel ;
    // extern const  char *method_hub_modes_notification_add;
    // extern const  char *method_hub_modes_notificationnotifyallset ;
    // extern const  char *method_hub_modes_notifications_set;
    // extern const  char *method_hub_modes_notification_remove;
    // extern const  char *method_hub_modes_disarmed_default_set;
    // extern const  char *method_hub_modes_disarmed_devices_add;
    // extern const  char *method_hub_modes_disarmed_devices_remove;
    // extern const  char *method_hub_network_reset ;
    // extern const  char *method_hub_network_wifi_scan_start ;
    // extern const  char *method_hub_network_wifi_scan_stop ;
    extern const char *method_hub_network_wifi_scan_progress;
    extern const char *method_hub_reset;
    // extern const  char *method_hub_log_remote_set;
    // extern const  char *method_hub_log_local_set;
    extern const char *method_hub_network_get;
    extern const char *method_hub_version_get;
    extern const char *method_hub_provision_info_set;
    extern const char *method_hub_room_all_delete;
    extern const char *method_hub_room_create;
    extern const char *method_hub_room_delete;
    extern const char *method_hub_room_image_set;
    extern const char *method_hub_room_get;
    extern const char *method_hub_room_list;
    extern const char *method_hub_room_name_set;
    // extern const  char *method_hub_room_order_set;
    // extern const  char *method_hub_room_view_set;
    // extern const  char *method_hub_server_request;
    // extern const  char *method_hub_time_location_get;
    // extern const  char *method_hub_time_location_set;
    extern const char *method_hub_time_location_list;
    // extern const  char *method_hub_software_info_get;
    extern const char *method_register;
    extern const char *method_registered;
    // extern const  char *method_hub_devices_service_notifications_set ;
    extern const char *method_hub_reboot;
    // extern const  char *method_hub_offline_login_hub;
    // extern const  char *method_hub_offline_login_ui ;
    // extern const  char *method_hub_network_enabled_set ;
    // extern const  char *method_hub_detection_devices_scan_start;
    // extern const  char *method_hub_detection_devices_scans_top;
    extern const char *method_hub_scene_run_progress;
    extern const char *ezlopi_rpc_method_notfound_str;

    extern const char *ezlopi_hub_expression_added_str;
    extern const char *ezlopi_hub_expression_changed_str;
    extern const char *ezlopi_hub_expression_deleted_str;

    extern const char *ezlopi_hub_scene_changed_str;
    extern const char *ezlopi_hub_scene_added_str;
    extern const char *ezlopi_hub_scene_deleted_str;

    extern const char *ezlopi_hub_room_created_str;
    extern const char *ezlopi_hub_room_edited_str;
    extern const char *ezlopi_hub_room_deleted_str;
    extern const char *ezlopi_hub_room_reordered_str;

    extern const char *ezlopi_hub_modes_changed_str;
    extern const char *ezlopi_hub_modes_alarmed_str;

    extern const char *ezlopi_hub_device_group_created;
    extern const char *ezlopi_hub_device_group_deleted;
    extern const char *ezlopi_hub_device_group_updated;

    extern const char *ezlopi_hub_item_group_created;
    extern const char *ezlopi_hub_item_group_deleted;
    extern const char *ezlopi_hub_item_group_updated;

    extern const char *ezlopi_hub_device_updated;
    extern const char *ezlopi_hub_modes_disarmed_devices_added;
    extern const char *ezlopi_hub_modes_disarmed_devices_removed;
    extern const char *ezlopi_hub_modes_alarms_off_added;
    extern const char *ezlopi_hub_modes_alarms_off_removed;
    extern const char *ezlopi_hub_modes_cameras_off_added;
    extern const char *ezlopi_hub_modes_cameras_off_removed;
    extern const char *ezlopi_hub_modes_bypass_devices_added;
    extern const char *ezlopi_hub_modes_bypass_devices_removed;
    extern const char *ezlopi_hub_modes_protect_button_added;
    extern const char *ezlopi_hub_modes_protect_button_updated;
    extern const char *ezlopi_hub_modes_protect_button_removed;
    extern const char *ezlopi_hub_modes_protect_devices_added;
    extern const char *ezlopi_hub_modes_protect_devices_removed;
    extern const char *ezlopi_hub_modes_entry_delay_changed;

#ifdef __cplusplus
}
#endif
#endif // __EZLOPI_METHODS_STR_H__

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
