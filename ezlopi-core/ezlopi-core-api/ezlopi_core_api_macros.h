/* ===========================================================================
** Copyright (C) 2024 Ezlo Innovation Inc
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
 * @file    ezlopi_core_api_macros.h
 * @brief   These are macros for api-methods
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 *          Lomas Subedi
 *          Riken Maharjan
 *          Nabin Dangi
 * @version 0.1
 * @date    12th DEC 2024
 */

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

        //          ("name",           func,          updater_func)
#if (defined(CONFIG_EZPI_WEBSOCKET_CLIENT) || defined(CONFIG_EZPI_LOCAL_WEBSOCKET_SERVER))

        CLOUD_METHOD("hub.items.list", EZPI_items_list_v3, NULL)
        CLOUD_METHOD("hub.item.value.set", EZPI_items_set_value_v3, NULL)

        CLOUD_METHOD("hub.devices.list", EZPI_devices_list_v3, NULL)
        CLOUD_METHOD("hub.device.name.set", EZPI_device_name_set, EZPI_device_updated)
        CLOUD_METHOD("hub.device.armed.set", EZPI_device_armed_set, EZPI_device_updated)
        CLOUD_METHOD("hub.device.room.set", EZPI_device_room_set, EZPI_device_updated)

        CLOUD_METHOD("hub.device.group.create", EZPI_device_group_create, EZPI_device_group_created)
        CLOUD_METHOD("hub.device.group.delete", EZPI_device_group_delete, EZPI_device_group_deleted)
        CLOUD_METHOD("hub.device.group.update", EZPI_device_group_update, EZPI_device_group_updated)
        CLOUD_METHOD("hub.device.group.get", EZPI_device_group_get, NULL)
        CLOUD_METHOD("hub.device.groups.list", EZPI_device_groups_list, NULL)
        CLOUD_METHOD("hub.device.group.find", EZPI_device_group_find, NULL)
        CLOUD_METHOD("hub.device.item.group.expand", EZPI_device_group_devitem_expand, NULL)

        CLOUD_METHOD("hub.item.group.create", EZPI_item_group_create, EZPI_item_group_created)
        CLOUD_METHOD("hub.item.group.delete", EZPI_item_group_delete, EZPI_item_group_deleted)
        CLOUD_METHOD("hub.item.group.update", EZPI_item_group_update, EZPI_item_group_updated)
        CLOUD_METHOD("hub.item.group.get", EZPI_item_group_get, NULL)
        CLOUD_METHOD("hub.item.groups.list", EZPI_item_groups_list, NULL)

#ifdef CONFIG_EZPI_UTIL_TRACE_EN
        CLOUD_METHOD("hub.log.set", EZPI_hub_cloud_log_set, NULL)
        CLOUD_METHOD("hub.log.local.set", EZPI_hub_serial_log_set, EZPI_hub_serial_log_set_updater)
#endif // CONFIG_EZPI_UTIL_TRACE_EN

#if defined(CONFIG_EZPI_SERV_ENABLE_MODES)
        CLOUD_METHOD("hub.modes.get", EZPI_cloud_modes_get, NULL)
        CLOUD_METHOD("hub.modes.current.get", EZPI_cloud_modes_current_get, NULL)
        CLOUD_METHOD("hub.modes.switch", EZPI_cloud_modes_switch, EZPI_cloud_modes_changed_alarmed)
        CLOUD_METHOD("hub.modes.cancel_switch", EZPI_cloud_modes_cancel_switch, NULL)
        CLOUD_METHOD("hub.modes.entry_delay.cancel", EZPI_cloud_modes_entry_delay_cancel, EZPI_cloud_modes_alarmed)
        CLOUD_METHOD("hub.modes.entry_delay.skip", EZPI_cloud_modes_entry_delay_skip, EZPI_cloud_modes_alarmed)
        CLOUD_METHOD("hub.modes.switch_to_delay.set", EZPI_cloud_modes_switch_to_delay_set, NULL)
        CLOUD_METHOD("hub.modes.alarm_delay.set", EZPI_cloud_modes_alarm_delay_set, NULL)
        CLOUD_METHOD("hub.modes.notifications.set", EZPI_cloud_modes_notifications_set, EZPI_cloud_modes_changed)
        CLOUD_METHOD("hub.modes.disarmed_default.set", EZPI_cloud_modes_disarmed_default_set, EZPI_cloud_modes_changed)
        CLOUD_METHOD("hub.modes.disarmed_devices.add", EZPI_cloud_modes_disarmed_devices_add, EZPI_cloud_modes_disarmed_devices_added)
        CLOUD_METHOD("hub.modes.disarmed_devices.remove", EZPI_cloud_modes_disarmed_devices_remove, EZPI_cloud_modes_disarmed_devices_removed)
        CLOUD_METHOD("hub.modes.alarms_off.add", EZPI_cloud_modes_alarms_off_add, EZPI_cloud_modes_alarms_off_added)
        CLOUD_METHOD("hub.modes.alarms_off.remove", EZPI_cloud_modes_alarms_off_remove, EZPI_cloud_modes_alarms_off_removed)
        CLOUD_METHOD("hub.modes.cameras_off.add", EZPI_cloud_modes_cameras_off_add, EZPI_cloud_modes_cameras_off_added)
        CLOUD_METHOD("hub.modes.cameras_off.remove", EZPI_cloud_modes_cameras_off_remove, EZPI_cloud_modes_cameras_off_removed)
        CLOUD_METHOD("hub.modes.bypass_devices.add", EZPI_cloud_modes_bypass_devices_add, EZPI_cloud_modes_bypass_devices_added)
        CLOUD_METHOD("hub.modes.bypass_devices.remove", EZPI_cloud_modes_bypass_devices_remove, EZPI_cloud_modes_bypass_devices_removed)
        CLOUD_METHOD("hub.modes.protect.set", EZPI_cloud_modes_protect_set, NULL)
        CLOUD_METHOD("hub.modes.protect.buttons.set", EZPI_cloud_modes_protect_buttons_set, EZPI_cloud_modes_protect_button_set_broadcast)
        CLOUD_METHOD("hub.modes.protect.devices.add", EZPI_cloud_modes_protect_devices_add, EZPI_cloud_modes_protect_devices_added)
        CLOUD_METHOD("hub.modes.protect.devices.remove", EZPI_cloud_modes_protect_devices_remove, EZPI_cloud_modes_protect_devices_removed)
        CLOUD_METHOD("hub.modes.entry_delay.set", EZPI_cloud_modes_entry_delay_set, EZPI_cloud_modes_entry_delay_changed)
        CLOUD_METHOD("hub.modes.entry_delay.reset", EZPI_cloud_modes_entry_delay_reset, EZPI_cloud_modes_entry_delay_changed)
#endif // CONFIG_EZPI_SERV_ENABLE_MODES

        CLOUD_METHOD("hub.favorite.list", EZPI_favorite_list_v3, NULL)

        CLOUD_METHOD("hub.gateways.list", EZPI_gateways_list, NULL)

        CLOUD_METHOD("hub.info.get", EZPI_info_get, NULL)

        CLOUD_METHOD("hub.network.get", EZPI_network_get, NULL)
        CLOUD_METHOD("hub.network.wifi.scan.start", EZPI_network_wifi_scan_start, NULL)
        CLOUD_METHOD("hub.network.wifi.scan.stop", EZPI_network_wifi_scan_stop, NULL)
        CLOUD_METHOD("hub.network.wifi.try_connect", EZPI_network_wifi_try_connect, NULL)

#if defined(CONFIG_EZPI_ENABLE_OTA)
        CLOUD_METHOD("cloud.firmware.info.get", EZPI_firmware_info_get, NULL)
        CLOUD_METHOD("hub.firmware.update.start", EZPI_firmware_update_start, NULL)
#endif // CONFIG_EZPI_ENABLE_OTA

        CLOUD_METHOD("hub.device.settings.list", EZPI_device_settings_list_v3, NULL)
        CLOUD_METHOD("hub.device.setting.value.set", EZPI_device_settings_value_set_v3, NULL)
        CLOUD_METHOD("hub.device.setting.reset", EZPI_device_settings_reset_v3, NULL)

        CLOUD_METHOD("hub.time.location.list", EZPI_CLOUD_location_list, NULL)
        CLOUD_METHOD("hub.time.location.set", EZPI_CLOUD_location_set, NULL)
        CLOUD_METHOD("hub.time.location.get", EZPI_CLOUD_location_get, NULL)

#if defined(CONFIG_EZPI_SERV_ENABLE_MESHBOTS)
        CLOUD_METHOD("hub.scenes.list", EZPI_scenes_list, NULL)
        CLOUD_METHOD("hub.scenes.create", EZPI_scenes_create, EZPI_scene_added)

        // #warning "firmware crashes due to this method 'hub.scenes.run', need to fix this"
        CLOUD_METHOD("hub.scenes.run", EZPI_scenes_run, NULL) // firmware crashes due to this method, need to fix this

        CLOUD_METHOD("hub.scenes.get", EZPI_scenes_get, NULL)
        CLOUD_METHOD("hub.scenes.edit", EZPI_scenes_edit, EZPI_scene_changed)
        CLOUD_METHOD("hub.scenes.delete", EZPI_scenes_delete, EZPI_scene_deleted)
        CLOUD_METHOD("hub.scenes.blocks.list", EZPI_scenes_blocks_list, NULL)
        CLOUD_METHOD("hub.scenes.block.data.list", EZPI_scenes_block_data_list, NULL)
        CLOUD_METHOD("hub.scenes.enabled.set", EZPI_scenes_enable_set, EZPI_scene_changed)
        CLOUD_METHOD("hub.scenes.notification.add", EZPI_scenes_notification_add, EZPI_scene_changed)
        CLOUD_METHOD("hub.scenes.notification.remove", EZPI_scenes_notification_remove, EZPI_scene_changed)
        CLOUD_METHOD("hub.scenes.status.get", EZPI_scenes_status_get, NULL) // Incomplete
        CLOUD_METHOD("hub.scenes.room.set", EZPI_scenes_room_set, EZPI_scene_changed)
        CLOUD_METHOD("hub.scenes.time.list", EZPI_scenes_time_list, NULL)
        CLOUD_METHOD("hub.scenes.trigger.devices.list", EZPI_scenes_trigger_device_list, NULL)
        CLOUD_METHOD("hub.scenes.house_modes.set", EZPI_scenes_house_modes_set, NULL)
        CLOUD_METHOD("hub.scenes.action.block.test", EZPI_scenes_action_block_test, NULL)
        CLOUD_METHOD("hub.scenes.block.enabled.set", EZPI_scenes_block_enabled_set, EZPI_scene_added)
        CLOUD_METHOD("hub.scenes.block.status.reset", EZPI_scenes_block_status_reset, NULL)
        CLOUD_METHOD("hub.scenes.meta.set", EZPI_scenes_meta_set, EZPI_scene_changed)
        CLOUD_METHOD("hub.scenes.block.meta.set", EZPI_scenes_blockmeta_set, EZPI_scene_changed)
        CLOUD_METHOD("hub.scenes.stop", EZPI_scenes_stop, NULL)
        CLOUD_METHOD("hub.scenes.clone", EZPI_scenes_clone, EZPI_scene_added)

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

        CLOUD_METHOD("hub.room.list", EZPI_room_list, NULL)
        CLOUD_METHOD("hub.room.create", EZPI_room_create, EZPI_room_created)
        CLOUD_METHOD("hub.room.name.set", EZPI_room_name_set, EZPI_room_edited)
        CLOUD_METHOD("hub.room.order.set", EZPI_room_order_set, EZPI_room_reordered)
        CLOUD_METHOD("hub.room.get", EZPI_room_get, NULL)
        CLOUD_METHOD("hub.room.delete", EZPI_room_delete, EZPI_room_deleted)
        CLOUD_METHOD("hub.room.all.delete", EZPI_room_all_delete, NULL)

#if defined(CONFIG_EZPI_SERV_ENABLE_MESHBOTS)
        CLOUD_METHOD("hub.scenes.scripts.list", EZPI_scenes_scripts_list, NULL)
        CLOUD_METHOD("hub.scenes.scripts.add", EZPI_scenes_scripts_add, NULL)
        CLOUD_METHOD("hub.scenes.scripts.get", EZPI_scenes_scripts_get, NULL)
        CLOUD_METHOD("hub.scenes.scripts.delete", EZPI_scenes_scripts_delete, NULL)
        CLOUD_METHOD("hub.scenes.scripts.set", EZPI_scenes_scripts_set, NULL)
        CLOUD_METHOD("hub.scenes.scripts.run", EZPI_scenes_scripts_run, NULL)
        CLOUD_METHOD("hub.scenes.expressions.set", EZPI_scenes_expressions_set, EZPI_scenes_expressions_added_changed)
        CLOUD_METHOD("hub.scenes.expressions.list", EZPI_scenes_expressions_list, NULL)
        CLOUD_METHOD("hub.scenes.expressions.delete", EZPI_scenes_expressions_delete, EZPI_scenes_expressions_deleted)

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

        CLOUD_METHOD("hub.nma.register.repeat", EZPI_register_repeat, NULL)

        CLOUD_METHOD("hub.reboot", EZPI_core_ezlopi_methods_reboot, NULL)

        CLOUD_METHOD("hub.status.get", EZPI_CLOUD_status_get, NULL)

        CLOUD_METHOD("hub.coordinates.set", EZPI_hub_coordinates_set, NULL)
        CLOUD_METHOD("hub.coordinates.get", EZPI_hub_coordinates_get, NULL)
        CLOUD_METHOD("hub.offline.login.ui", EZPI_CLOUD_offline_login, NULL)

        CLOUD_METHOD("registered", EZPI_registered, NULL)

        CLOUD_METHOD("hub.data.list", EZPI_hub_data_list, NULL)

        CLOUD_METHOD("hub.setting.value.set", EZPI_setting_value_set, EZPI_setting_value_set_broadcast_updater)
        CLOUD_METHOD("hub.settings.list", EZPI_settings_list, NULL)

#endif // (defined(CONFIG_EZPI_WEBSOCKET_CLIENT) || defined(EZPI_LOCAL_WEBSOCKET_SERVER))

        /*******************************************************************************
         *                          Extern Data Declarations
         *******************************************************************************/

        /*******************************************************************************
         *                          Extern Function Prototypes
         *******************************************************************************/

#ifdef __cplusplus
}
#endif

/*******************************************************************************
 *                          End of File
 *******************************************************************************/