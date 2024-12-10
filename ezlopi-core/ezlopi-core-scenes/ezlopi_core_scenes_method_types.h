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
 * @file    main.c
 * @brief   perform some function on data
 * @author  John Doe
 * @version 0.1
 * @date    1st January 2024
 */

// #ifndef __HEADER_H__
// #define __HEADER_H__

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
    EZLOPI_SCENE(METHOD_TYPE_NONE, "NULL", NULL, "NULL")
    // when
    EZLOPI_SCENE(WHEN_METHOD_IS_ITEM_STATE, "isItemState", ezlopi_scene_when_is_item_state, "null")
    EZLOPI_SCENE(WHEN_METHOD_IS_ITEM_STATE_CHANGED, "isItemStateChanged", ezlopi_scene_when_is_item_state_changed, "null")
    EZLOPI_SCENE(WHEN_METHOD_IS_BUTTON_STATE, "isButtonState", ezlopi_scene_when_is_button_state, "null")
    EZLOPI_SCENE(WHEN_METHOD_IS_SUN_STATE, "isSunState", ezlopi_scene_when_is_sun_state, "when_category_time")
    EZLOPI_SCENE(WHEN_METHOD_IS_DATE, "isDate", ezlopi_scene_when_is_date, "when_category_time")
    EZLOPI_SCENE(WHEN_METHOD_IS_ONCE, "isOnce", ezlopi_scene_when_is_once, "when_category_time")
    EZLOPI_SCENE(WHEN_METHOD_IS_INTERVAL, "isInterval", ezlopi_scene_when_is_interval, "when_category_time")
    EZLOPI_SCENE(WHEN_METHOD_IS_DATE_RANGE, "isDateRange", ezlopi_scene_when_is_date_range, "when_category_time")
    EZLOPI_SCENE(WHEN_METHOD_IS_USER_LOCK_OPERATION, "isUserLockOperation", ezlopi_scene_when_is_user_lock_operation, "null")

    #if defined(CONFIG_EZPI_SERV_ENABLE_MODES)
    EZLOPI_SCENE(WHEN_METHOD_IS_HOUSE_MODE_CHANGED_TO, "isHouseModeChangedTo", ezlopi_scene_when_is_house_mode_changed_to, "when_category_modes")
    EZLOPI_SCENE(WHEN_METHOD_IS_HOUSE_MODE_CHANGED_FROM, "isHouseModeChangedFrom", ezlopi_scene_when_is_house_mode_changed_from, "when_category_modes")
    EZLOPI_SCENE(WHEN_METHOD_IS_HOUSE_MODE_ALARM_PHASE_RANGE, "isHouseModeAlarmPhaseRange", ezlopi_scene_when_is_house_mode_alarm_phase_range, "when_category_modes")
    EZLOPI_SCENE(WHEN_METHOD_IS_HOUSE_MODE_SWTICH_TO_RANGE, "isHouseModeSwitchToRange", ezlopi_scene_when_is_house_mode_switch_to_range, "when_category_modes")
    #endif // CONFIG_EZPI_SERV_ENABLE_MODES

    EZLOPI_SCENE(WHEN_METHOD_IS_DEVICE_STATE, "isDeviceState", ezlopi_scene_when_is_device_state, "null")
    EZLOPI_SCENE(WHEN_METHOD_IS_NETWORK_STATE, "isNetworkState", ezlopi_scene_when_is_network_state, "null")
    EZLOPI_SCENE(WHEN_METHOD_IS_SCENE_STATE, "isSceneState", ezlopi_scene_when_is_scene_state, "null")
    EZLOPI_SCENE(WHEN_METHOD_IS_GROUP_STATE, "isGroupState", ezlopi_scene_when_is_group_state, "null")
    EZLOPI_SCENE(WHEN_METHOD_IS_CLOUD_STATE, "isCloudState", ezlopi_scene_when_is_cloud_state, "null")
    EZLOPI_SCENE(WHEN_METHOD_IS_BATTERY_STATE, "isBatteryState", ezlopi_scene_when_is_battery_state, "null")
    EZLOPI_SCENE(WHEN_METHOD_IS_BATTERY_LEVEL, "isBatteryLevel", ezlopi_scene_when_is_battery_level, "null")
    EZLOPI_SCENE(WHEN_METHOD_COMPARE_NUMBERS, "compareNumbers", ezlopi_scene_when_compare_numbers, "null")
    EZLOPI_SCENE(WHEN_METHOD_COMPARE_NUMBER_RANGE, "compareNumberRange", ezlopi_scene_when_compare_number_range, "null")
    EZLOPI_SCENE(WHEN_METHOD_COMPARE_STRINGS, "compareStrings", ezlopi_scene_when_compare_strings, "null")
    EZLOPI_SCENE(WHEN_METHOD_STRING_OPERATION, "stringOperation", ezlopi_scene_when_string_operation, "null")
    EZLOPI_SCENE(WHEN_METHOD_IN_ARRAY, "inArray", ezlopi_scene_when_in_array, "null")
    EZLOPI_SCENE(WHEN_METHOD_COMPARE_VALUES, "compareValues", ezlopi_scene_when_compare_values, "null")
    EZLOPI_SCENE(WHEN_METHOD_HAS_ATLEAST_ONE_DICTIONARY_VALUE, "hasAtLeastOneDictionaryValue", ezlopi_scene_when_has_atleast_one_dictionary_value, "null")
    EZLOPI_SCENE(WHEN_METHOD_IS_FIRMWARE_UPDATE_STATE, "isFirmwareUpdateState", ezlopi_scene_when_is_firmware_update_state, "null")
    EZLOPI_SCENE(WHEN_METHOD_IS_DICTIONARY_CHANGED, "isDictionaryChanged", ezlopi_scene_when_is_dictionary_changed, "null")
    EZLOPI_SCENE(WHEN_METHOD_IS_DETECTED_IN_HOTZONE, "isDetectedInHotzone", ezlopi_scene_when_is_detected_in_hot_zone, "null")
    EZLOPI_SCENE(WHEN_METHOD_AND, "and", ezlopi_scene_when_and, "when_category_logic")
    EZLOPI_SCENE(WHEN_METHOD_NOT, "not", ezlopi_scene_when_not, "when_category_logic")
    EZLOPI_SCENE(WHEN_METHOD_OR, "or", ezlopi_scene_when_or, "when_category_logic")
    EZLOPI_SCENE(WHEN_METHOD_XOR, "xor", ezlopi_scene_when_xor, "when_category_logic")
    EZLOPI_SCENE(WHEN_METHOD_FUNCTION, "function", ezlopi_scene_when_function, "when_category_function")
    // then
    EZLOPI_SCENE(THEN_METHOD_SET_ITEM_VALUE, "setItemValue", ezlopi_scene_then_set_item_value, "null")
    EZLOPI_SCENE(THEN_METHOD_SET_DEVICE_ARMED, "setDeviceArmed", ezlopi_scene_then_set_device_armed, "null")
    EZLOPI_SCENE(THEN_METHOD_SEND_CLOUD_ABSTRACT_COMMAND, "sendCloudAbstractCommand", ezlopi_scene_then_send_cloud_abstract_command, "null")
    EZLOPI_SCENE(THEN_METHOD_SWITCH_HOUSE_MODE, "switchHouseMode", ezlopi_scene_then_switch_house_mode, "null")
    EZLOPI_SCENE(THEN_METHOD_SEND_HTTP_REQUEST, "sendHttpRequest", ezlopi_scene_then_send_http_request, "null")
    EZLOPI_SCENE(THEN_METHOD_RUN_CUSTOM_SCRIPT, "runCustomScript", ezlopi_scene_then_run_custom_script, "null")
    EZLOPI_SCENE(THEN_METHOD_RUN_PLUGIN_SCRIPT, "runPluginScript", ezlopi_scene_then_run_plugin_script, "null")
    EZLOPI_SCENE(THEN_METHOD_RUN_SCENE, "runScene", ezlopi_scene_then_run_scene, "null")
    EZLOPI_SCENE(THEN_METHOD_SET_SCENE_STATE, "setSceneState", ezlopi_scene_then_set_scene_state, "null")
    EZLOPI_SCENE(THEN_METHOD_RESET_LATCH, "resetLatch", ezlopi_scene_then_reset_latch, "null")
    EZLOPI_SCENE(THEN_METHOD_RESET_SCENE_LATCHES, "resetSceneLatches", ezlopi_scene_then_reset_scene_latches, "null")
    EZLOPI_SCENE(THEN_METHOD_REBOOT_HUB, "rebootHub", ezlopi_scene_then_reboot_hub, "null")
    EZLOPI_SCENE(THEN_METHOD_RESET_HUB, "resetHub", ezlopi_scene_then_reset_hub, "null")
    EZLOPI_SCENE(THEN_METHOD_CLOUD_API, "cloudAPI", ezlopi_scene_then_cloud_api, "null")
    EZLOPI_SCENE(THEN_METHOD_SET_EXPRESSION, "setExpression", ezlopi_scene_then_set_expression, "null")
    EZLOPI_SCENE(THEN_METHOD_SET_VARIABLE, "setVariable", ezlopi_scene_then_set_variable, "null")
    EZLOPI_SCENE(THEN_METHOD_TOGGLE_VALUE, "toggleValue", ezlopi_scene_then_toggle_value, "null")
    //
    EZLOPI_SCENE(METHOD_TYPE_MAX, "max", NULL, "max")

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

// #endif // __HEADER_H__

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
