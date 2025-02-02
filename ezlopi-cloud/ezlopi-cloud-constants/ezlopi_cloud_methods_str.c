/* ===========================================================================
** Copyright (C) 2025 Ezlo Innovation Inc
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
 * @file    ezlopi_cloud_methods_str.c
 * @brief   Definitions for cloud method strings
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 * @version 1.0
 * @date    November 22nd, 2023 3:27 PM
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/
const char *method_hub_item_updated = "hub.item.updated";
const char *method_add_device = "addDevice";
const char *method_add_all_devices = "addAllDevices";
const char *method_cloud_firmware_info_get = "cloud.firmware.info.get";
// const  char *method_cloud_access_keys_controller_sync = "cloud.access_keys_controller_sync";
// const  char *method_cloud_timezones_get = "nma.timezones.info.get";
// const  char *method_cloud_controller_abstracts_set = "cloud.controller_abstracts_set";
// const  char *method_cloud_controller_abstracts_list = "cloud.controller_abstracts_list";
// const  char *method_cloud_storage_controller_token = "cloud.storage_controller_token";
// const  char *method_extensions_plugin_info_installed = "extensions.plugin.info.installed";
// const  char *method_extensions_plugin_install_execute = "extensions.plugin.install.execute";
// const  char *method_extensions_plugin_install_request = "extensions.plugin.install.request";
// const  char *method_extensions_plugin_run = "extensions.plugin.run";
// const  char *method_extensions_plugin_uninstall_execute = "extensions.plugin.uninstall.execute";
// const  char *method_extensions_plugin_uninstall_request = "extensions.plugin.uninstall.request";
// const  char *method_hub_offline_credentials_updated = "hub.offline.credentials.updated";
// const  char *method_hub_bridge_controller_connection_list = "hub.bridge.connections.list";
// const  char *method_hub_bridge_controller_connection_set = "hub.bridge.controller.connection.set";
// const  char *method_EZPI_hub_coordinates_get = "hub.coordinates.get";
// const  char *method_EZPI_hub_coordinates_set = "hub.coordinates.set";
// const  char *method_hub_description_set = "hub.description.set";
// const  char *method_hub_device_force_remove = "hub.device.force_remove";
// const  char *method_hub_device_firmware_update = "hub.device.firmware.update";
const char *method_hub_devices_list = "hub.devices.list";
const char *method_hub_device_armed_set = "hub.device.armed.set";
// const  char *method_hub_device_name_set = "hub.device.name.set";
const char *method_hub_device_room_set = "hub.device.room.set";
// const  char *method_hub_device_setting_dictionary_value_set = "hub.device.setting.dictionary.value.set";
// const  char *method_hub_device_setting_value_set = "hub.device.setting.value.set";
// const  char *method_hub_gateway_setting_value_set = "hub.gateway.setting.value.set";
// const  char *method_hub_detection_device_add = "hub.detection.device.add";
// const  char *method_hub_detection_devices_list = "hub.detection.devices.list";
// const  char *method_hub_detection_device_remove = "hub.detection.device.remove";
// const  char *method_hub_detection_device_set = "hub.detection.device.set";
const char *method_hub_setting_updated = "hub.setting.updated";
// const  char *method_hub_device_setting_reset = "hub.device.setting.reset";
// const  char *method_hub_device_settings_list = "hub.device.settings.list";
// const  char *method_hub_setting_value_set_request = "hub.setting.value.set";
// const  char *method_hub_setting_value_reset_request = "hub.setting.value.reset";
const char *method_hub_device_setting_updated = "hub.device.setting.updated";
// const  char *method_hub_extensions_plugin_info_installed = "hub.extensions.plugin.info.installed";
// const  char *method_hub_extensions_plugin_install_execute = "hub.extensions.plugin.install.execute";
// const  char *method_hub_software_update_execute = "hub.software.update.execute";
// const  char *method_hub_extensions_plugin_install_request = "hub.extensions.plugin.install.request";
// const  char *method_hub_extensions_plugin_run = "hub.extensions.plugin.run";
// const  char *method_hub_extensions_plugin_uninstall_execute = "hub.extensions.plugin.uninstall.execute";
// const  char *method_hub_extensions_plugin_uninstall_request = "hub.extensions.plugin.uninstall.request";
const char *method_hub_favorite_list = "hub.favorite.list";
const char *method_hub_favorite_set = "hub.favorite.set";
const char *method_hub_firmware_update = "hub.firmware.update";
const char *method_hub_info_get = "hub.info.get";
const char *method_hub_data_value_list_request = "hub.data.list";
const char *method_hub_settings_list_request = "hub.settings.list";
const char *method_hub_features_list = "hub.features.list";
const char *method_hub_feature_status_set = "hub.feature.status.set";
const char *method_hub_gateways_list = "hub.gateways.list";
const char *method_hub_geteway_settings_list = "hub.gateway.settings.list";
const char *method_hub_item_dictionary_clear = "hub.item.dictionary.clear";
const char *method_hub_item_dictionary_value_add = "hub.item.dictionary.value.add";
const char *method_hub_item_dictionary_value_remove = "hub.item.dictionary.value.remove";
const char *method_hub_items_list = "hub.items.list";
const char *method_hub_item_dictionary_value_set = "hub.item.dictionary.value.set";
const char *method_hub_item_value_set = "hub.item.value.set";
const char *method_hub_item_value_percent_apply = "hub.item.value.percent.apply";
const char *method_hub_modes_get = "hub.modes.get";
// const  char *method_hub_modes_current_get = "hub.modes.current.get";
// const  char *method_hub_modes_switch = "hub.modes.switch";
// const  char *method_hub_modes_switch_to_delay_set = "hub.modes.switch_to_delay.set";
// const  char *method_hub_modes_alarm_delay_set = "hub.modes.alarm_delay.set";
const char *method_hub_modes_alarms_off_add = "hub.modes.alarms_off.add";
const char *method_hub_modes_alarms_off_remove = "hub.modes.alarms_off.remove";
// const  char *method_hub_modes_switch_cancel = "hub.modes.cancel_switch";
// const  char *method_hub_modes_notification_add = "hub.modes.notifications.add";
// const  char *method_hub_modes_notificationnotifyallset = "hub.modes.notifications.notify_all.set";
// const  char *method_hub_modes_notifications_set = "hub.modes.notifications.set";
// const  char *method_hub_modes_notification_remove = "hub.modes.notifications.remove";
// const  char *method_hub_modes_disarmed_default_set = "hub.modes.disarmed_default.set";
// const  char *method_hub_modes_disarmed_devices_add = "hub.modes.disarmed_devices.add";
// const  char *method_hub_modes_disarmed_devices_remove = "hub.modes.disarmed_devices.remove";
// const  char *method_hub_network_reset = "hub.network.reset";
// const  char *method_hub_network_wifi_scan_start = "hub.network.wifi.scan.start";
// const  char *method_hub_network_wifi_scan_stop = "hub.network.wifi.scan.stop";
const char *method_hub_network_wifi_scan_progress = "hub.network.wifi.scan.progress";
const char *method_hub_reset = "hub.reset";
// const  char *method_hub_log_remote_set = "hub.log.set";
// const  char *method_hub_log_local_set = "hub.log.local.set";
const char *method_hub_network_get = "hub.network.get";
const char *method_hub_version_get = "hub.version.get";
const char *method_hub_provision_info_set = "hub.provision_info.set";
const char *method_hub_room_all_delete = "hub.room.all.delete";
const char *method_hub_room_create = "hub.room.create";
const char *method_hub_room_delete = "hub.room.delete";
const char *method_hub_room_image_set = "hub.room.image.set";
const char *method_hub_room_get = "hub.room.get";
const char *method_hub_room_list = "hub.room.list";
const char *method_hub_room_name_set = "hub.room.name.set";
// const  char *method_hub_room_order_set = "hub.room.order.set";
// const  char *method_hub_room_view_set = "hub.room.view.set";
// const  char *method_hub_server_request = "hub.server.request";
// const  char *method_hub_time_location_get = "hub.time.location.get";
// const  char *method_hub_time_location_set = "hub.time.location.set";
const char *method_hub_time_location_list = "hub.time.location.list";
// const  char *method_hub_software_info_get = "hub.software.info.get";
const char *method_register = "register";
const char *method_registered = "registered";
// const  char *method_hub_devices_service_notifications_set = "hub.devices.service.notifications.set";
const char *method_hub_reboot = "hub.reboot";
// const  char *method_hub_offline_login_hub = "hub.offline.login.hub";
// const  char *method_hub_offline_login_ui = "hub.offline.login.ui";
// const  char *method_hub_network_enabled_set = "hub.network.enabled.set";
// const  char *method_hub_detection_devices_scan_start = "hub.detection.devices.scan.start";
// const  char *method_hub_detection_devices_scans_top = "hub.detection.devices.scan.stop";
const char *method_hub_scene_run_progress = "hub.scene.run.progress";
const char *ezlopi_rpc_method_notfound_str = "rpc.method.notfound";

const char *ezlopi_hub_expression_added_str = "hub.expression.added";
const char *ezlopi_hub_expression_changed_str = "hub.expression.changed";
const char *ezlopi_hub_expression_deleted_str = "hub.expression.deleted";

const char *ezlopi_hub_scene_changed_str = "hub.scene.changed";
const char *ezlopi_hub_scene_added_str = "hub.scene.added";
const char *ezlopi_hub_scene_deleted_str = "hub.scene.deleted";

const char *ezlopi_hub_room_created_str = "hub.room.created";
const char *ezlopi_hub_room_edited_str = "hub.room.edited";
const char *ezlopi_hub_room_deleted_str = "hub.room.deleted";
const char *ezlopi_hub_room_reordered_str = "hub.room.reordered";

const char *ezlopi_hub_modes_changed_str = "hub.modes.changed";
const char *ezlopi_hub_modes_alarmed_str = "hub.modes.alarmed";

const char *ezlopi_hub_device_group_created = "hub.device.group.created";
const char *ezlopi_hub_device_group_deleted = "hub.device.group.deleted";
const char *ezlopi_hub_device_group_updated = "hub.device.group.updated";

const char *ezlopi_hub_item_group_created = "hub.item.group.created";
const char *ezlopi_hub_item_group_deleted = "hub.item.group.deleted";
const char *ezlopi_hub_item_group_updated = "hub.item.group.updated";

const char *ezlopi_hub_device_updated = "hub.device.updated";
const char *ezlopi_hub_modes_disarmed_devices_added = "hub.modes.disarmed_devices.added";
const char *ezlopi_hub_modes_disarmed_devices_removed = "hub.modes.disarmed_devices.removed";
const char *ezlopi_hub_modes_alarms_off_added = "hub.modes.alarms_off.added";
const char *ezlopi_hub_modes_alarms_off_removed = "hub.modes.alarms_off.removed";
const char *ezlopi_hub_modes_cameras_off_added = "hub.modes.cameras_off.added";
const char *ezlopi_hub_modes_cameras_off_removed = "hub.modes.cameras_off.removed";
const char *ezlopi_hub_modes_bypass_devices_added = "hub.modes.bypass_devices.added";
const char *ezlopi_hub_modes_bypass_devices_removed = "hub.modes.bypass_devices.removed";
const char *ezlopi_hub_modes_protect_button_added = "hub.modes.protect.button.added";
const char *ezlopi_hub_modes_protect_button_updated = "hub.modes.protect.button.updated";
const char *ezlopi_hub_modes_protect_button_removed = "hub.modes.protect.button.removed";
const char *ezlopi_hub_modes_protect_devices_added = "hub.modes.protect.devices.added";
const char *ezlopi_hub_modes_protect_devices_removed = "hub.modes.protect.devices.removed";
const char *ezlopi_hub_modes_entry_delay_changed = "hub.modes.entry_delay.changed";
const char *ezlopi_hub_modes_swinger_shutdown_reset_done = "hub.modes.swinger.shutdown.reset.done";
const char *ezlopi_hub_modes_swinger_shutdown_disable_added = "hub.modes.swinger.shutdown.disable.added";
const char *ezlopi_hub_modes_swinger_shutdown_disable_removed = "hub.modes.swinger.shutdown.disable.remved";
const char *ezlopi_hub_modes_swinger_shutdown_limit_changed = "hub.modes.swinger.shutdown.limits.changed";
const char *ezlopi_hub_modes_local_alarmed_turned_off = "hub.modes.local.alarmed.turned.off";

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Static Function Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
