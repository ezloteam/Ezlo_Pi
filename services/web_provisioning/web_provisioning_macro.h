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

CLOUD_METHOD("hub.scenes.list", scenes_list, NULL)
CLOUD_METHOD("hub.scenes.create", scenes_create, NULL)
CLOUD_METHOD("hub.scenes.get", scenes_get, NULL)
CLOUD_METHOD("hub.scenes.edit", scenes_edit, NULL)
CLOUD_METHOD("hub.scenes.delete", scenes_delete, NULL)
CLOUD_METHOD("hub.scenes.blocks.list", scenes_blocks_list, NULL)
CLOUD_METHOD("hub.scenes.block.data.list", scenes_block_data_list, NULL)
CLOUD_METHOD("hub.scenes.scripts.list", scenes_scripts_list, NULL)
CLOUD_METHOD("hub.scenes.scripts.add", scenes_scripts_add, NULL)
CLOUD_METHOD("hub.scenes.scripts.get", scenes_scripts_get, NULL)
CLOUD_METHOD("hub.scenes.scripts.delete", scenes_scripts_delete, NULL)
CLOUD_METHOD("hub.scenes.scripts.set", scenes_scripts_set, NULL)
CLOUD_METHOD("hub.scenes.scripts.run", scenes_scripts_run, NULL)

CLOUD_METHOD("hub.nma.register.repeat", register_repeat, NULL)

CLOUD_METHOD("hub.reboot", __hub_reboot, NULL)
CLOUD_METHOD("registered", registered, NULL)
