//          ("name",           func,          updater_func)
CLOUD_METHOD("hub.items.list", items_list_v3, NULL)
CLOUD_METHOD("hub.item.value.set", items_set_value_v3, NULL)

CLOUD_METHOD("hub.devices.list", devices_list_v3, NULL)
CLOUD_METHOD("hub.device.name.set", device_name_set, device_updated)

#if defined(CONFIG_EZLPI_SERV_ENABLE_MODES)
CLOUD_METHOD("hub.modes.get", ezlopi_cloud_modes_get, NULL)
CLOUD_METHOD("hub.modes.current.get", ezlopi_cloud_modes_current_get, NULL)
CLOUD_METHOD("hub.modes.switch", ezlopi_cloud_modes_switch, NULL)
CLOUD_METHOD("hub.modes.cancel_switch", ezlopi_cloud_modes_cancel_switch, NULL)
CLOUD_METHOD("hub.modes.entry_delay.cancel", ezlopi_cloud_modes_entry_delay_cancel, NULL)
CLOUD_METHOD("hub.modes.entry_delay.skip", ezlopi_cloud_modes_entry_delay_skip, NULL)
CLOUD_METHOD("hub.modes.switch_to_delay.set", ezlopi_cloud_modes_switch_to_delay_set, NULL)
CLOUD_METHOD("hub.modes.alarm_delay.set", ezlopi_cloud_modes_alarm_delay_set, NULL)
CLOUD_METHOD("hub.modes.notifications.set", ezlopi_cloud_modes_notifications_set, NULL)
CLOUD_METHOD("hub.modes.disarmed_default.set", ezlopi_cloud_modes_disarmed_default_set, NULL)
CLOUD_METHOD("hub.modes.disarmed_devices.add", ezlopi_cloud_modes_disarmed_devices_add, NULL)
CLOUD_METHOD("hub.modes.disarmed_devices.remove", ezlopi_cloud_modes_disarmed_devices_remove, NULL)
CLOUD_METHOD("hub.modes.alarms_off.add", ezlopi_cloud_modes_alarms_off_add, NULL)
CLOUD_METHOD("hub.modes.alarms_off.remove", ezlopi_cloud_modes_alarms_off_remove, NULL)
CLOUD_METHOD("hub.modes.cameras_off.add", ezlopi_cloud_modes_cameras_off_add, NULL)
CLOUD_METHOD("hub.modes.cameras_off.remove", ezlopi_cloud_modes_cameras_off_remove, NULL)
CLOUD_METHOD("hub.modes.bypass_devices.add", ezlopi_cloud_modes_bypass_devices_add, NULL)
CLOUD_METHOD("hub.modes.bypass_devices.remove", ezlopi_cloud_modes_bypass_devices_remove, NULL)
CLOUD_METHOD("hub.modes.protect.set", ezlopi_cloud_modes_protect_set, NULL)
CLOUD_METHOD("hub.modes.protect.buttons.set", ezlopi_cloud_modes_protect_buttons_set, NULL)
CLOUD_METHOD("hub.modes.protect.devices.add", ezlopi_cloud_modes_protect_devices_add, NULL)
CLOUD_METHOD("hub.modes.protect.devices.remove", ezlopi_cloud_modes_protect_devices_remove, NULL)
CLOUD_METHOD("hub.modes.entry_delay.set", ezlopi_cloud_modes_entry_delay_set, NULL)
CLOUD_METHOD("hub.modes.entry_delay.reset", ezlopi_cloud_modes_entry_delay_reset, NULL)
#endif // CONFIG_EZLPI_SERV_ENABLE_MODES

CLOUD_METHOD("hub.favorite.list", favorite_list_v3, NULL)

CLOUD_METHOD("hub.gateways.list", gateways_list, NULL)

CLOUD_METHOD("hub.info.get", info_get, NULL)

CLOUD_METHOD("hub.network.get", network_get, NULL)
CLOUD_METHOD("hub.network.wifi.scan.start", network_wifi_scan_start, NULL)
CLOUD_METHOD("hub.network.wifi.scan.stop", network_wifi_scan_stop, NULL)

CLOUD_METHOD("cloud.firmware.info.get", firmware_info_get, NULL)
CLOUD_METHOD("hub.firmware.update.start", firmware_update_start, NULL)

CLOUD_METHOD("hub.device.settings.list", ezlopi_device_settings_list_v3, NULL)
CLOUD_METHOD("hub.device.setting.value.set", ezlopi_device_settings_value_set_v3, NULL)
CLOUD_METHOD("hub.device.setting.reset", ezlopi_device_settings_reset_v3, NULL)

CLOUD_METHOD("hub.time.location.list", EZPI_CLOUD_location_list, NULL)
CLOUD_METHOD("hub.time.location.set", EZPI_CLOUD_location_set, NULL)
CLOUD_METHOD("hub.time.location.get", EZPI_CLOUD_location_get, NULL)

#if CONFIG_EZPI_SERV_ENABLE_MESHBOTS
CLOUD_METHOD("hub.scenes.list", scenes_list, NULL)
CLOUD_METHOD("hub.scenes.create", scenes_create, scene_added)
CLOUD_METHOD("hub.scenes.run", scenes_run, NULL)
CLOUD_METHOD("hub.scenes.get", scenes_get, NULL)
CLOUD_METHOD("hub.scenes.edit", scenes_edit, scene_changed)
CLOUD_METHOD("hub.scenes.delete", scenes_delete, scene_deleted)
CLOUD_METHOD("hub.scenes.blocks.list", scenes_blocks_list, NULL)
CLOUD_METHOD("hub.scenes.block.data.list", scenes_block_data_list, NULL)
CLOUD_METHOD("hub.scenes.enabled.set", scenes_enable_set, scene_changed)
CLOUD_METHOD("hub.scenes.notification.add", scenes_notification_add, scene_changed)
CLOUD_METHOD("hub.scenes.notification.remove", scenes_notification_remove, scene_changed)
// CLOUD_METHOD("hub.scenes.status.get", scenes_status_get, NULL) // Incomplete
#endif

CLOUD_METHOD("hub.room.list", room_list, NULL)
CLOUD_METHOD("hub.room.create", room_create, room_created)
CLOUD_METHOD("hub.room.name.set", room_name_set, room_edited)
CLOUD_METHOD("hub.room.order.set", room_order_set, room_reordered)
CLOUD_METHOD("hub.room.get", room_get, NULL)
CLOUD_METHOD("hub.room.delete", room_delete, room_deleted)
CLOUD_METHOD("hub.room.all.delete", room_all_delete, NULL)

#if CONFIG_EZPI_SERV_ENABLE_MESHBOTS
CLOUD_METHOD("hub.scenes.scripts.list", scenes_scripts_list, NULL)
CLOUD_METHOD("hub.scenes.scripts.add", scenes_scripts_add, NULL)
CLOUD_METHOD("hub.scenes.scripts.get", scenes_scripts_get, NULL)
CLOUD_METHOD("hub.scenes.scripts.delete", scenes_scripts_delete, NULL)
CLOUD_METHOD("hub.scenes.scripts.set", scenes_scripts_set, NULL)
CLOUD_METHOD("hub.scenes.scripts.run", scenes_scripts_run, NULL)

CLOUD_METHOD("hub.scenes.expressions.set", scenes_expressions_set, NULL)
CLOUD_METHOD("hub.scenes.expressions.list", scenes_expressions_list, NULL)
CLOUD_METHOD("hub.scenes.expressions.delete", scenes_expressions_delete, NULL)
#endif

CLOUD_METHOD("hub.nma.register.repeat", register_repeat, NULL)

CLOUD_METHOD("hub.reboot", ezlopi_core_ezlopi_methods_reboot, NULL)
CLOUD_METHOD("registered", registered, NULL)

CLOUD_METHOD("hub.coordinates.set", hub_coordinates_set, NULL)
CLOUD_METHOD("hub.coordinates.get", hub_coordinates_get, NULL)
CLOUD_METHOD("hub.offline.login.ui", EZPI_CLOUD_offline_login, NULL)
