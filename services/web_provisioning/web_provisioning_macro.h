//          ("name",           func,          updater_func)
CLOUD_METHOD("hub.items.list", items_list_v3, NULL)
CLOUD_METHOD("hub.item.value.set", items_set_value_v3, items_update_v3)

CLOUD_METHOD("hub.devices.list", devices_list_v3, NULL)

CLOUD_METHOD("hub.room.list", room_list, NULL)

CLOUD_METHOD("hub.modes.get", modes_get, NULL)

CLOUD_METHOD("hub.favorite.list", favorite_list_v3, NULL)

CLOUD_METHOD("hub.gateways.list", gateways_list, NULL)

CLOUD_METHOD("hub.info.get", info_get, NULL)

CLOUD_METHOD("hub.network.get", network_get, NULL)

CLOUD_METHOD("cloud.firmware.info.get", firmware_info_get, NULL)
CLOUD_METHOD("hub.firmware.update.start", firmware_update_start, NULL)

CLOUD_METHOD("hub.device.settings.list", ezlopi_device_settings_list_v3, NULL)
CLOUD_METHOD("hub.device.setting.value.set", ezlopi_device_settings_value_set_v3, NULL)
CLOUD_METHOD("hub.device.setting.reset", ezlopi_device_settings_reset_v3, NULL)

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

CLOUD_METHOD("hub.room.list", room_list, NULL)
CLOUD_METHOD("hub.room.create", room_create, room_created)
CLOUD_METHOD("hub.room.name.set", room_name_set, room_edited)
CLOUD_METHOD("hub.room.order.set", NULL, NULL)
CLOUD_METHOD("hub.room.get", room_get, NULL)
CLOUD_METHOD("hub.room.delete", room_delete, room_deleted)
CLOUD_METHOD("hub.room.all.delete", room_all_delete, NULL)

CLOUD_METHOD("hub.scenes.scripts.list", scenes_scripts_list, NULL)
CLOUD_METHOD("hub.scenes.scripts.add", scenes_scripts_add, NULL)
CLOUD_METHOD("hub.scenes.scripts.get", scenes_scripts_get, NULL)
CLOUD_METHOD("hub.scenes.scripts.delete", scenes_scripts_delete, NULL)
CLOUD_METHOD("hub.scenes.scripts.set", scenes_scripts_set, NULL)
CLOUD_METHOD("hub.scenes.scripts.run", scenes_scripts_run, NULL)

CLOUD_METHOD("hub.scenes.expressions.set", scenes_expressions_set, NULL)
CLOUD_METHOD("hub.scenes.expressions.list", scenes_expressions_list, NULL)
CLOUD_METHOD("hub.scenes.expressions.delete", scenes_expressions_delete, NULL)

CLOUD_METHOD("hub.nma.register.repeat", register_repeat, NULL)

CLOUD_METHOD("hub.reboot", __hub_reboot, NULL)
CLOUD_METHOD("registered", registered, NULL)
