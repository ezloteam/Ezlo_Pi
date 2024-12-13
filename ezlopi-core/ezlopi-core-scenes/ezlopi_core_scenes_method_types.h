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
 * @file    ezlopi_core_scenes_method_types.h
 * @brief   MACROS to generate enums for scene-methods
 * @author  xx
 * @version 0.1
 * @date    12th DEC 2024
*/

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

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
        EZLOPI_SCENE(METHOD_TYPE_NONE, "NULL", NULL, METHOD_CATEGORY_NAN)
                // when
                EZLOPI_SCENE(WHEN_METHOD_IS_ITEM_STATE, "isItemState", EZPI_core_scenes_when_is_item_state, METHOD_CATEGORY_UNDEFINED)
                EZLOPI_SCENE(WHEN_METHOD_IS_ITEM_STATE_CHANGED, "isItemStateChanged", EZPI_core_scenes_when_is_item_state_changed, METHOD_CATEGORY_UNDEFINED)
                EZLOPI_SCENE(WHEN_METHOD_IS_BUTTON_STATE, "isButtonState", EZPI_core_scenes_when_is_button_state, METHOD_CATEGORY_UNDEFINED)
                EZLOPI_SCENE(WHEN_METHOD_IS_SUN_STATE, "isSunState", EZPI_core_scenes_when_is_sun_state, METHOD_CATEGORY_WHEN_TIME)
                EZLOPI_SCENE(WHEN_METHOD_IS_DATE, "isDate", EZPI_core_scenes_when_is_date, METHOD_CATEGORY_WHEN_TIME)
                EZLOPI_SCENE(WHEN_METHOD_IS_ONCE, "isOnce", EZPI_core_scenes_when_is_once, METHOD_CATEGORY_WHEN_TIME)
                EZLOPI_SCENE(WHEN_METHOD_IS_INTERVAL, "isInterval", EZPI_core_scenes_when_is_interval, METHOD_CATEGORY_WHEN_TIME)
                EZLOPI_SCENE(WHEN_METHOD_IS_DATE_RANGE, "isDateRange", EZPI_core_scenes_when_is_date_range, METHOD_CATEGORY_WHEN_TIME)
                EZLOPI_SCENE(WHEN_METHOD_IS_USER_LOCK_OPERATION, "isUserLockOperation", EZPI_core_scenes_when_is_user_lock_operation, METHOD_CATEGORY_UNDEFINED)

#if defined(CONFIG_EZPI_SERV_ENABLE_MODES)
                EZLOPI_SCENE(WHEN_METHOD_IS_HOUSE_MODE_CHANGED_TO, "isHouseModeChangedTo", EZPI_core_scenes_when_is_house_mode_changed_to, METHOD_CATEGORY_WHEN_MODES)
                EZLOPI_SCENE(WHEN_METHOD_IS_HOUSE_MODE_CHANGED_FROM, "isHouseModeChangedFrom", EZPI_core_scenes_when_is_house_mode_changed_from, METHOD_CATEGORY_WHEN_MODES)
                EZLOPI_SCENE(WHEN_METHOD_IS_HOUSE_MODE_ALARM_PHASE_RANGE, "isHouseModeAlarmPhaseRange", EZPI_core_scenes_when_is_house_mode_alarm_phase_range, METHOD_CATEGORY_WHEN_MODES)
                EZLOPI_SCENE(WHEN_METHOD_IS_HOUSE_MODE_SWTICH_TO_RANGE, "isHouseModeSwitchToRange", EZPI_core_scenes_when_is_house_mode_switch_to_range, METHOD_CATEGORY_WHEN_MODES)
#endif // CONFIG_EZPI_SERV_ENABLE_MODES

                EZLOPI_SCENE(WHEN_METHOD_IS_DEVICE_STATE, "isDeviceState", EZPI_core_scenes_when_is_device_state, METHOD_CATEGORY_UNDEFINED)
                EZLOPI_SCENE(WHEN_METHOD_IS_NETWORK_STATE, "isNetworkState", EZPI_core_scenes_when_is_network_state, METHOD_CATEGORY_UNDEFINED)
                EZLOPI_SCENE(WHEN_METHOD_IS_SCENE_STATE, "isSceneState", EZPI_core_scenes_when_is_scene_state, METHOD_CATEGORY_UNDEFINED)
                EZLOPI_SCENE(WHEN_METHOD_IS_GROUP_STATE, "isGroupState", EZPI_core_scenes_when_is_group_state, METHOD_CATEGORY_UNDEFINED)
                EZLOPI_SCENE(WHEN_METHOD_IS_CLOUD_STATE, "isCloudState", EZPI_core_scenes_when_is_cloud_state, METHOD_CATEGORY_UNDEFINED)
                EZLOPI_SCENE(WHEN_METHOD_IS_BATTERY_STATE, "isBatteryState", EZPI_core_scenes_when_is_battery_state, METHOD_CATEGORY_UNDEFINED)
                EZLOPI_SCENE(WHEN_METHOD_IS_BATTERY_LEVEL, "isBatteryLevel", EZPI_core_scenes_when_is_battery_level, METHOD_CATEGORY_UNDEFINED)
                EZLOPI_SCENE(WHEN_METHOD_COMPARE_NUMBERS, "compareNumbers", EZPI_core_scene_when_compare_numbers, METHOD_CATEGORY_UNDEFINED)
                EZLOPI_SCENE(WHEN_METHOD_COMPARE_NUMBER_RANGE, "compareNumberRange", EZPI_core_scene_when_compare_number_range, METHOD_CATEGORY_UNDEFINED)
                EZLOPI_SCENE(WHEN_METHOD_COMPARE_STRINGS, "compareStrings", EZPI_core_scene_when_compare_strings, METHOD_CATEGORY_UNDEFINED)
                EZLOPI_SCENE(WHEN_METHOD_STRING_OPERATION, "stringOperation", EZPI_core_scene_when_string_operation, METHOD_CATEGORY_UNDEFINED)
                EZLOPI_SCENE(WHEN_METHOD_IN_ARRAY, "inArray", EZPI_core_scene_when_in_array, METHOD_CATEGORY_UNDEFINED)
                EZLOPI_SCENE(WHEN_METHOD_COMPARE_VALUES, "compareValues", EZPI_core_scene_when_compare_values, METHOD_CATEGORY_UNDEFINED)
                EZLOPI_SCENE(WHEN_METHOD_HAS_ATLEAST_ONE_DICTIONARY_VALUE, "hasAtLeastOneDictionaryValue", EZPI_core_scene_when_has_atleast_one_dictionary_value, METHOD_CATEGORY_UNDEFINED)
                EZLOPI_SCENE(WHEN_METHOD_IS_FIRMWARE_UPDATE_STATE, "isFirmwareUpdateState", EZPI_core_scenes_when_is_firmware_update_state, METHOD_CATEGORY_UNDEFINED)
                EZLOPI_SCENE(WHEN_METHOD_IS_DICTIONARY_CHANGED, "isDictionaryChanged", EZPI_core_scenes_when_is_dictionary_changed, METHOD_CATEGORY_UNDEFINED)
                EZLOPI_SCENE(WHEN_METHOD_IS_DETECTED_IN_HOTZONE, "isDetectedInHotzone", EZPI_core_scenes_when_is_detected_in_hot_zone, METHOD_CATEGORY_UNDEFINED)
                EZLOPI_SCENE(WHEN_METHOD_AND, "and", EZPI_core_scene_when_and, METHOD_CATEGORY_WHEN_LOGIC)
                EZLOPI_SCENE(WHEN_METHOD_NOT, "not", EZPI_core_scene_when_not, METHOD_CATEGORY_WHEN_LOGIC)
                EZLOPI_SCENE(WHEN_METHOD_OR, "or", EZPI_core_scene_when_or, METHOD_CATEGORY_WHEN_LOGIC)
                EZLOPI_SCENE(WHEN_METHOD_XOR, "xor", EZPI_core_scene_when_xor, METHOD_CATEGORY_WHEN_LOGIC)
                EZLOPI_SCENE(WHEN_METHOD_FUNCTION, "function", EZPI_core_scene_when_function, METHOD_CATEGORY_WHEN_FUNCTION)
                // then
                EZLOPI_SCENE(THEN_METHOD_SET_ITEM_VALUE, "setItemValue", EZPI_core_scene_then_set_item_value, METHOD_CATEGORY_UNDEFINED)
                EZLOPI_SCENE(THEN_METHOD_SET_DEVICE_ARMED, "setDeviceArmed", EZPI_core_scene_then_set_device_armed, METHOD_CATEGORY_UNDEFINED)
                EZLOPI_SCENE(THEN_METHOD_SEND_CLOUD_ABSTRACT_COMMAND, "sendCloudAbstractCommand", EZPI_core_scene_then_send_cloud_abstract_command, METHOD_CATEGORY_UNDEFINED)
                EZLOPI_SCENE(THEN_METHOD_SWITCH_HOUSE_MODE, "switchHouseMode", EZPI_core_scene_then_switch_house_mode, METHOD_CATEGORY_UNDEFINED)
                EZLOPI_SCENE(THEN_METHOD_SEND_HTTP_REQUEST, "sendHttpRequest", EZPI_core_scene_then_send_http_request, METHOD_CATEGORY_UNDEFINED)
                EZLOPI_SCENE(THEN_METHOD_RUN_CUSTOM_SCRIPT, "runCustomScript", EZPI_core_scene_then_run_custom_script, METHOD_CATEGORY_UNDEFINED)
                EZLOPI_SCENE(THEN_METHOD_RUN_PLUGIN_SCRIPT, "runPluginScript", EZPI_core_scene_then_run_plugin_script, METHOD_CATEGORY_UNDEFINED)
                EZLOPI_SCENE(THEN_METHOD_RUN_SCENE, "runScene", EZPI_core_scene_then_run_scene, METHOD_CATEGORY_UNDEFINED)
                EZLOPI_SCENE(THEN_METHOD_SET_SCENE_STATE, "setSceneState", EZPI_core_scene_then_set_scene_state, METHOD_CATEGORY_UNDEFINED)
                EZLOPI_SCENE(THEN_METHOD_RESET_LATCH, "resetLatch", EZPI_core_scene_then_reset_latch, METHOD_CATEGORY_UNDEFINED)
                EZLOPI_SCENE(THEN_METHOD_RESET_SCENE_LATCHES, "resetSceneLatches", EZPI_core_scene_then_reset_scene_latches, METHOD_CATEGORY_UNDEFINED)
                EZLOPI_SCENE(THEN_METHOD_REBOOT_HUB, "rebootHub", EZPI_core_scene_then_reboot_hub, METHOD_CATEGORY_UNDEFINED)
                EZLOPI_SCENE(THEN_METHOD_RESET_HUB, "resetHub", EZPI_core_scene_then_reset_hub, METHOD_CATEGORY_UNDEFINED)
                EZLOPI_SCENE(THEN_METHOD_CLOUD_API, "cloudAPI", EZPI_core_scene_then_cloud_api, METHOD_CATEGORY_UNDEFINED)
                EZLOPI_SCENE(THEN_METHOD_SET_EXPRESSION, "setExpression", EZPI_core_scene_then_set_expression, METHOD_CATEGORY_UNDEFINED)
                EZLOPI_SCENE(THEN_METHOD_SET_VARIABLE, "setVariable", EZPI_core_scene_then_set_variable, METHOD_CATEGORY_UNDEFINED)
                EZLOPI_SCENE(THEN_METHOD_TOGGLE_VALUE, "toggleValue", EZPI_core_scene_then_toggle_value, METHOD_CATEGORY_UNDEFINED)
                //
                EZLOPI_SCENE(METHOD_TYPE_MAX, "max", NULL, METHOD_CATEGORY_MAX)

#ifdef __cplusplus
}
#endif

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

/*******************************************************************************
*                          End of File
*******************************************************************************/





