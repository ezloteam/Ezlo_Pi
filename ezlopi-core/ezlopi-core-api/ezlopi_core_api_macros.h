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
CLOUD_METHOD("hub.item.group.update", EZPI_item_group_update, EZPI_EZPI_item_group_updated)
CLOUD_METHOD("hub.item.group.get", EZPI_item_group_get, NULL)
CLOUD_METHOD("hub.item.groups.list", EZPI_item_groups_list, NULL)

#ifdef CONFIG_EZPI_UTIL_TRACE_EN
CLOUD_METHOD("hub.log.set", EZPI_hub_cloud_log_set, NULL)
CLOUD_METHOD("hub.log.local.set", EZPI_hub_serial_log_set, ezlopi_hub_serial_log_set_updater)
#endif // CONFIG_EZPI_UTIL_TRACE_EN

#if defined(CONFIG_EZPI_SERV_ENABLE_MODES)
CLOUD_METHOD("hub.modes.get", ezlopi_cloud_modes_get, NULL)
CLOUD_METHOD("hub.modes.current.get", ezlopi_cloud_modes_current_get, NULL)
CLOUD_METHOD("hub.modes.switch", ezlopi_cloud_modes_switch, ezlopi_cloud_modes_changed_alarmed)
CLOUD_METHOD("hub.modes.cancel_switch", ezlopi_cloud_modes_cancel_switch, NULL)
CLOUD_METHOD("hub.modes.entry_delay.cancel", ezlopi_cloud_modes_entry_delay_cancel, ezlopi_cloud_modes_alarmed)
CLOUD_METHOD("hub.modes.entry_delay.skip", ezlopi_cloud_modes_entry_delay_skip, ezlopi_cloud_modes_alarmed)
CLOUD_METHOD("hub.modes.switch_to_delay.set", ezlopi_cloud_modes_switch_to_delay_set, NULL)
CLOUD_METHOD("hub.modes.alarm_delay.set", ezlopi_cloud_modes_alarm_delay_set, NULL)
CLOUD_METHOD("hub.modes.notifications.set", ezlopi_cloud_modes_notifications_set, ezlopi_cloud_modes_changed)
CLOUD_METHOD("hub.modes.disarmed_default.set", ezlopi_cloud_modes_disarmed_default_set, ezlopi_cloud_modes_changed)
CLOUD_METHOD("hub.modes.disarmed_devices.add", ezlopi_cloud_modes_disarmed_devices_add, ezlopi_cloud_modes_changed)
CLOUD_METHOD("hub.modes.disarmed_devices.remove", ezlopi_cloud_modes_disarmed_devices_remove, ezlopi_cloud_modes_changed)
CLOUD_METHOD("hub.modes.alarms_off.add", ezlopi_cloud_modes_alarms_off_add, ezlopi_cloud_modes_alarms_off_added)
CLOUD_METHOD("hub.modes.alarms_off.remove", ezlopi_cloud_modes_alarms_off_remove, ezlopi_cloud_modes_alarms_off_removed)
CLOUD_METHOD("hub.modes.cameras_off.add", ezlopi_cloud_modes_cameras_off_add, NULL)
CLOUD_METHOD("hub.modes.cameras_off.remove", ezlopi_cloud_modes_cameras_off_remove, NULL)
CLOUD_METHOD("hub.modes.bypass_devices.add", ezlopi_cloud_modes_bypass_devices_add, ezlopi_cloud_modes_bypass_devices_added)
CLOUD_METHOD("hub.modes.bypass_devices.remove", ezlopi_cloud_modes_bypass_devices_remove, ezlopi_cloud_modes_bypass_devices_removed)
CLOUD_METHOD("hub.modes.protect.set", ezlopi_cloud_modes_protect_set, NULL)
CLOUD_METHOD("hub.modes.protect.buttons.set", ezlopi_cloud_modes_protect_buttons_set, ezlopi_cloud_modes_protect_button_set_broadcast)
CLOUD_METHOD("hub.modes.protect.devices.add", ezlopi_cloud_modes_protect_devices_add, ezlopi_cloud_modes_protect_devices_added)
CLOUD_METHOD("hub.modes.protect.devices.remove", ezlopi_cloud_modes_protect_devices_remove, ezlopi_cloud_modes_protect_devices_removed)
CLOUD_METHOD("hub.modes.entry_delay.set", ezlopi_cloud_modes_entry_delay_set, ezlopi_cloud_modes_entry_delay_changed)
CLOUD_METHOD("hub.modes.entry_delay.reset", ezlopi_cloud_modes_entry_delay_reset, ezlopi_cloud_modes_entry_delay_changed)
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

CLOUD_METHOD("hub.device.settings.list", ezlopi_device_settings_list_v3, NULL)
CLOUD_METHOD("hub.device.setting.value.set", ezlopi_device_settings_value_set_v3, NULL)
CLOUD_METHOD("hub.device.setting.reset", ezlopi_device_settings_reset_v3, NULL)

CLOUD_METHOD("hub.time.location.list", EZPI_CLOUD_location_list, NULL)
CLOUD_METHOD("hub.time.location.set", EZPI_CLOUD_location_set, NULL)
CLOUD_METHOD("hub.time.location.get", EZPI_CLOUD_location_get, NULL)

#if defined(CONFIG_EZPI_SERV_ENABLE_MESHBOTS)
CLOUD_METHOD("hub.scenes.list", scenes_list, NULL)
CLOUD_METHOD("hub.scenes.create", scenes_create, scene_added)

// #warning "firmware crashes due to this method 'hub.scenes.run', need to fix this"
CLOUD_METHOD("hub.scenes.run", scenes_run, NULL) // firmware crashes due to this method, need to fix this

CLOUD_METHOD("hub.scenes.get", scenes_get, NULL)
CLOUD_METHOD("hub.scenes.edit", scenes_edit, scene_changed)
CLOUD_METHOD("hub.scenes.delete", scenes_delete, scene_deleted)
CLOUD_METHOD("hub.scenes.blocks.list", scenes_blocks_list, NULL)
CLOUD_METHOD("hub.scenes.block.data.list", scenes_block_data_list, NULL)
CLOUD_METHOD("hub.scenes.enabled.set", scenes_enable_set, scene_changed)
CLOUD_METHOD("hub.scenes.notification.add", scenes_notification_add, scene_changed)
CLOUD_METHOD("hub.scenes.notification.remove", scenes_notification_remove, scene_changed)
CLOUD_METHOD("hub.scenes.status.get", scenes_status_get, NULL) // Incomplete
CLOUD_METHOD("hub.scenes.room.set", scenes_room_set, scene_changed)
CLOUD_METHOD("hub.scenes.time.list", scenes_time_list, NULL)
CLOUD_METHOD("hub.scenes.trigger.devices.list", scenes_trigger_device_list, NULL)
CLOUD_METHOD("hub.scenes.house_modes.set", scenes_house_modes_set, NULL)
CLOUD_METHOD("hub.scenes.action.block.test", scenes_action_block_test, NULL)
CLOUD_METHOD("hub.scenes.block.enabled.set", scenes_block_enabled_set, scene_added)
CLOUD_METHOD("hub.scenes.block.status.reset", scenes_block_status_reset, NULL)
CLOUD_METHOD("hub.scenes.meta.set", scenes_meta_set, scene_changed)
CLOUD_METHOD("hub.scenes.block.meta.set", scenes_blockmeta_set, scene_changed)
CLOUD_METHOD("hub.scenes.stop", scenes_stop, NULL)
CLOUD_METHOD("hub.scenes.clone", scenes_clone, scene_added)

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

CLOUD_METHOD("hub.room.list", EZPI_room_list, NULL)
CLOUD_METHOD("hub.room.create", EZPI_room_create, EZPI_room_created)
CLOUD_METHOD("hub.room.name.set", EZPI_room_name_set, EZPI_room_edited)
CLOUD_METHOD("hub.room.order.set", EZPI_room_order_set, EZPI_room_reordered)
CLOUD_METHOD("hub.room.get", EZPI_room_get, NULL)
CLOUD_METHOD("hub.room.delete", EZPI_room_delete, EZPI_room_deleted)
CLOUD_METHOD("hub.room.all.delete", EZPI_room_all_delete, NULL)

#if defined(CONFIG_EZPI_SERV_ENABLE_MESHBOTS)
CLOUD_METHOD("hub.scenes.scripts.list", scenes_scripts_list, NULL)
CLOUD_METHOD("hub.scenes.scripts.add", scenes_scripts_add, NULL)
CLOUD_METHOD("hub.scenes.scripts.get", scenes_scripts_get, NULL)
CLOUD_METHOD("hub.scenes.scripts.delete", scenes_scripts_delete, NULL)
CLOUD_METHOD("hub.scenes.scripts.set", scenes_scripts_set, NULL)
CLOUD_METHOD("hub.scenes.scripts.run", scenes_scripts_run, NULL)
CLOUD_METHOD("hub.scenes.expressions.set", scenes_expressions_set, scenes_expressions_added_changed)
CLOUD_METHOD("hub.scenes.expressions.list", scenes_expressions_list, NULL)
CLOUD_METHOD("hub.scenes.expressions.delete", scenes_expressions_delete, scenes_expressions_deleted)

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

CLOUD_METHOD("hub.nma.register.repeat", EZPI_register_repeat, NULL)

CLOUD_METHOD("hub.reboot", ezlopi_core_ezlopi_methods_reboot, NULL)

CLOUD_METHOD("hub.status.get", EZPI_CLOUD_status_get, NULL)

CLOUD_METHOD("hub.coordinates.set", EZPI_hub_coordinates_set, NULL)
CLOUD_METHOD("hub.coordinates.get", EZPI_hub_coordinates_get, NULL)
CLOUD_METHOD("hub.offline.login.ui", EZPI_CLOUD_offline_login, NULL)

CLOUD_METHOD("registered", EZPI_registered, NULL)

CLOUD_METHOD("hub.data.list", EZPI_hub_data_list, NULL)

CLOUD_METHOD("hub.setting.value.set", setting_value_set, setting_value_set_broadcast_updater)
CLOUD_METHOD("hub.settings.list", settings_list, NULL)

#endif // (defined(CONFIG_EZPI_WEBSOCKET_CLIENT) || defined(EZPI_LOCAL_WEBSOCKET_SERVER))
