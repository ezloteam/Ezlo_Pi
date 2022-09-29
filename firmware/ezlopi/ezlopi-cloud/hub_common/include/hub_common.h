#ifndef __HUB_COMMON_H__
#define __HUB_COMMON_H__

/**
 * @brief api constants
 *
 */

#ifdef __cplusplus
extern "C"
{
#endif


const char *registered = "registered";
const char *hub_devices_list = "hub.devices.list";
const char *hub_device_name_set = "hub.device.name.set";
const char *hub_devices_settings_list = "hub.devices.settings.list";
const char *hub_device_setting_value_set = "hub.device.setting.value.set";

const char *hub_items_list = "hub.items.list";
const char *hub_item_value_set = "hub.item.value.set";

const char *hub_feature_status_set = "hub.feature.status.set";
const char *hub_features_list = "hub.features.list";

const char *hub_data_list = "hub.data.list";
const char *hub_room_list = "hub.room.list";
const char *hub_scenes_list = "hub.scenes.list";
const char *hub_favorite_list = "hub.favorite.list";
const char *hub_gateways_list = "hub.gateways.list";
const char *hub_info_get = "hub.info.get";
const char *hub_modes_get = "hub.modes.get";
const char *hub_network_get = "hub.network.get";
const char *hub_settings_list = "hub.settings.list";
const char *hub_reboot = "hub.reboot";

#ifdef __cplusplus
}
#endif
#endif