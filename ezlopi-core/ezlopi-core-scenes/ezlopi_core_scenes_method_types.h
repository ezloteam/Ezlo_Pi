EZLOPI_SCENE(METHOD_TYPE_NONE, "none", NULL)
// when
EZLOPI_SCENE(WHEN_METHOD_IS_ITEM_STATE, "isItemState", ezlopi_scene_when_is_item_state)
EZLOPI_SCENE(WHEN_METHOD_IS_ITEM_STATE_CHANGED, "isItemStateChanged", ezlopi_scene_when_is_item_state_changed)
EZLOPI_SCENE(WHEN_METHOD_IS_BUTTON_STATE, "isButtonState", ezlopi_scene_when_is_button_state)
EZLOPI_SCENE(WHEN_METHOD_IS_SUN_STATE, "isSunState", ezlopi_scene_when_is_sun_state)
EZLOPI_SCENE(WHEN_METHOD_IS_DATE, "isDate", ezlopi_scene_when_is_date)
EZLOPI_SCENE(WHEN_METHOD_IS_ONCE, "isOnce", ezlopi_scene_when_is_once)
EZLOPI_SCENE(WHEN_METHOD_IS_INTERVAL, "isInterval", ezlopi_scene_when_is_interval)
EZLOPI_SCENE(WHEN_METHOD_IS_DATE_RANGE, "isDateRange", ezlopi_scene_when_is_date_range)
EZLOPI_SCENE(WHEN_METHOD_IS_USER_LOCK_OPERATION, "isUserLockOperation", ezlopi_scene_when_is_user_lock_operation)

#if defined(CONFIG_EZPI_SERV_ENABLE_MODES)
EZLOPI_SCENE(WHEN_METHOD_IS_HOUSE_MODE_CHANGED_TO, "isHouseModeChangedTo", ezlopi_scene_when_is_house_mode_changed_to)
EZLOPI_SCENE(WHEN_METHOD_IS_HOUSE_MODE_CHANGED_FROM, "isHouseModeChangedFrom", ezlopi_scene_when_is_house_mode_changed_from)
#endif // CONFIG_EZPI_SERV_ENABLE_MODES

EZLOPI_SCENE(WHEN_METHOD_IS_DEVICE_STATE, "isDeviceState", ezlopi_scene_when_is_device_state)
EZLOPI_SCENE(WHEN_METHOD_IS_NETWORK_STATE, "isNetworkState", ezlopi_scene_when_is_network_state)
EZLOPI_SCENE(WHEN_METHOD_IS_SCENE_STATE, "isSceneState", ezlopi_scene_when_is_scene_state)
EZLOPI_SCENE(WHEN_METHOD_IS_GROUP_STATE, "isGroupState", ezlopi_scene_when_is_group_state)
EZLOPI_SCENE(WHEN_METHOD_IS_CLOUD_STATE, "isCloudState", ezlopi_scene_when_is_cloud_state)
EZLOPI_SCENE(WHEN_METHOD_IS_BATTERY_STATE, "isBatteryState", ezlopi_scene_when_is_battery_state)
EZLOPI_SCENE(WHEN_METHOD_IS_BATTERY_LEVEL, "isBatteryLevel", ezlopi_scene_when_is_battery_level)
EZLOPI_SCENE(WHEN_METHOD_COMPARE_NUMBERS, "compareNumbers", ezlopi_scene_when_compare_numbers)
EZLOPI_SCENE(WHEN_METHOD_COMPARE_NUMBER_RANGE, "compareNumberRange", ezlopi_scene_when_compare_number_range)
EZLOPI_SCENE(WHEN_METHOD_COMPARE_STRINGS, "compareStrings", ezlopi_scene_when_compare_strings)
EZLOPI_SCENE(WHEN_METHOD_STRING_OPERATION, "stringOperation", ezlopi_scene_when_string_operation)
EZLOPI_SCENE(WHEN_METHOD_IN_ARRAY, "inArray", ezlopi_scene_when_in_array)
EZLOPI_SCENE(WHEN_METHOD_COMPARE_VALUES, "compareValues", ezlopi_scene_when_compare_values)
EZLOPI_SCENE(WHEN_METHOD_HAS_ATLEAST_ONE_DICTIONARY_VALUE, "hasAtLeastOneDictionaryValue", ezlopi_scene_when_has_atleast_one_dictionary_value)
EZLOPI_SCENE(WHEN_METHOD_IS_FIRMWARE_UPDATE_STATE, "isFirmwareUpdateState", ezlopi_scene_when_is_firmware_update_state)
EZLOPI_SCENE(WHEN_METHOD_IS_DICTIONARY_CHANGED, "isDictionaryChanged", ezlopi_scene_when_is_dictionary_changed)
EZLOPI_SCENE(WHEN_METHOD_IS_DETECTED_IN_HOTZONE, "isDetectedInHotzone", ezlopi_scene_when_is_detected_in_hot_zone)
EZLOPI_SCENE(WHEN_METHOD_AND, "and", ezlopi_scene_when_and)
EZLOPI_SCENE(WHEN_METHOD_NOT, "not", ezlopi_scene_when_not)
EZLOPI_SCENE(WHEN_METHOD_OR, "or", ezlopi_scene_when_or)
EZLOPI_SCENE(WHEN_METHOD_XOR, "xor", ezlopi_scene_when_xor)
EZLOPI_SCENE(WHEN_METHOD_FUNCTION, "function", ezlopi_scene_when_function)
// then
EZLOPI_SCENE(THEN_METHOD_SET_ITEM_VALUE, "setItemValue", ezlopi_scene_then_set_item_value)
EZLOPI_SCENE(THEN_METHOD_SET_DEVICE_ARMED, "setDeviceArmed", ezlopi_scene_then_set_device_armed)
EZLOPI_SCENE(THEN_METHOD_SEND_CLOUD_ABSTRACT_COMMAND, "sendCloudAbstractCommand", ezlopi_scene_then_send_cloud_abstract_command)
EZLOPI_SCENE(THEN_METHOD_SWITCH_HOUSE_MODE, "switchHouseMode", ezlopi_scene_then_switch_house_mode)
EZLOPI_SCENE(THEN_METHOD_SEND_HTTP_REQUEST, "sendHttpRequest", ezlopi_scene_then_send_http_request)
EZLOPI_SCENE(THEN_METHOD_RUN_CUSTOM_SCRIPT, "runCustomScript", ezlopi_scene_then_run_custom_script)
EZLOPI_SCENE(THEN_METHOD_RUN_PLUGIN_SCRIPT, "runPluginScript", ezlopi_scene_then_run_plugin_script)
EZLOPI_SCENE(THEN_METHOD_RUN_SCENE, "runScene", ezlopi_scene_then_run_scene)
EZLOPI_SCENE(THEN_METHOD_SET_SCENE_STATE, "setSceneState", ezlopi_scene_then_set_scene_state)
EZLOPI_SCENE(THEN_RESET_LATCH, "resetLatch", ezlopi_scene_then_reset_latch)
EZLOPI_SCENE(THEN_RESET_SCENE_LATCHES, "resetSceneLatches", ezlopi_scene_then_reset_scene_latches)
EZLOPI_SCENE(THEN_REBOOT_HUB, "rebootHub", ezlopi_scene_then_reboot_hub)
EZLOPI_SCENE(THEN_RESET_HUB, "resetHub", ezlopi_scene_then_reset_hub)
EZLOPI_SCENE(THEN_CLOUD_API, "cloudAPI", ezlopi_scene_then_cloud_api)
EZLOPI_SCENE(THEN_SET_EXPRESSION, "setExpression", ezlopi_scene_then_set_expression)
EZLOPI_SCENE(THEN_SET_VARIABLE, "setVariable", ezlopi_scene_then_set_variable)
EZLOPI_SCENE(THEN_TOGGLE_VALUE, "toggleValue", ezlopi_scene_then_toggle_value)
//
EZLOPI_SCENE(METHOD_TYPE_MAX, "max", NULL)
