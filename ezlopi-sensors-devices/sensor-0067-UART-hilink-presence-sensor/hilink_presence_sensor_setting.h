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
 * @file    hilink_presence_sensor_setting.h
 * @brief   perform some function on hilink_presence_sensor_setting
 * @author  ezlopi_team_np
 * @version 0.1
 * @date    xx
 */

#ifndef _HILINK_PRESENCE_SENSOR_SETTINGS_H_
#define _HILINK_PRESENCE_SENSOR_SETTINGS_H_

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "ezlopi_util_trace.h"
// #include "cjext.h"
#include "ezlopi_core_devices.h"

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
#define HILINK_PRESENCE_SENSOR_SETTING_PREDEFINED_LABEL_TEXT "Pre-defined operation mode"
#define HILINK_PRESENCE_SENSOR_SETTING_PREDEFINED_DESCRIPTION_TEXT "Pre-Defined Operation mode, there will be different operation mode for different use cases"
#define HILINK_PRESENCE_SENSOR_SETTING_USER_DEFINED_LABEL_TEXT "User defined mode"
#define HILINK_PRESENCE_SENSOR_SETTING_USER_DEFINED_DESCRIPTION_TEXT "User defined operation mode, where user can set several distance parameters for setting a custom operation mode"
#define HILINK_PRESENCE_SENSOR_SETTING_RADAR_DISTANCE_SENSITIVITY_LABEL_TEXT "Radar Distance Sensitivity"
#define HILINK_PRESENCE_SENSOR_SETTING_RADAR_DISTANCE_SENSITIVITY_DESCRIPTION_TEXT "Presence Sensor Pro device will send update to its cloud when the distance change by an object is more than the sensitivity value set."

#define SLEEP_MODE_CLOSE_TEXT "Sleep mode close range"
#define SLEEP_MODE_MID_TEXT "Sleep mode mid range"
#define SLEEP_MODE_LONG_TEXT "Sleep mode long range"
#define MOVEMENT_MODE_CLOSE_TEXT "Movement mode close range"
#define MOVEMENT_MODE_MID_TEXT "Movement mode mid range"
#define MOVEMENT_MODE_LONG_TEXT "Movement mode long range"
#define BASIC_ROUTINE_MODE_CLOSE_TEXT "Basic mode close range"
#define BASIC_ROUTINE_MODE_MID_TEXT "Basic mode mid range"
#define BASIC_ROUTINE_MODE_LONG_TEXT "Basic mode long range"
#define USER_DEFINED_MODE_TEXT "User defined mode, when selected means this setting is not used, instead \"User defined mode\" setting is in use"

#define HILINK_PRESENCE_SENSOR_SETTING_PREDEFINED_LABEL_LANG_TAG "ezlopi_presence_predefined_operation_mode_label"
#define HILINK_PRESENCE_SENSOR_SETTING_PREDEFINED_DESCRIPTION_LANG_TAG "ezlopi_presence_predefined_operation_mode_description"
#define HILINK_PRESENCE_SENSOR_SETTING_USER_DEFINED_LABEL_LANG_TAG "ezlopi_presence_user_defined_mode_label"
#define HILINK_PRESENCE_SENSOR_SETTING_USER_DEFINED_DESCRIPTION_LANG_TAG "ezlopi_presence_user_defined_mode_description"
#define HILINK_PRESENCE_SENSOR_SETTING_RADAR_DISTANCE_SENSITIVITY_LABEL_LANG_TAG "ezlopi_presence_radar_distance_sensitivity_setting_label"
#define HILINK_PRESENCE_SENSOR_SETTING_RADAR_DISTANCE_SENSITIVITY_DESCRIPTION_LANG_TAG "ezlopi_presence_radar_distance_sensitivity_setting_description"

#define SLEEP_MODE_CLOSE_LANG_TAG "ezlopi_presence_sleep_close"
#define SLEEP_MODE_MID_LANG_TAG "ezlopi_presence_sleep_mid"
#define SLEEP_MODE_LONG_LANG_TAG "ezlopi_presence_sleep_long"
#define MOVEMENT_MODE_CLOSE_LANG_TAG "ezlopi_presence_movement_close"
#define MOVEMENT_MODE_MID_LANG_TAG "ezlopi_presence_movement_mid"
#define MOVEMENT_MODE_LONG_LANG_TAG "ezlopi_presence_movement_long"
#define BASIC_ROUTINE_MODE_CLOSE_LANG_TAG "ezlopi_presence_basic_close"
#define BASIC_ROUTINE_MODE_MID_LANG_TAG "ezlopi_presence_basic_mid"
#define BASIC_ROUTINE_MODE_LONG_LANG_TAG "ezlopi_presence_basic_long"
#define USER_DEFINED_MODE_LANG_TAG "ezlopi_presence_user_defined_mode"

    typedef struct s_hilink_predefined_setting_value
    {
        char setting_value[50];
    } s_hilink_predefined_setting_value_t;

    typedef struct s_hilink_userdefined_setting_value
    {
        float min_move_distance;
        float max_move_distance;
        float min_still_distance;
        float max_still_distance;
        uint8_t timeout;
        bool is_active;
    } s_hilink_userdefined_setting_value_t;

    typedef struct s_hilink_radar_distance_sensitivity_value
    {
        int distance_sensitivity_value;
    } s_hilink_radar_distance_sensitivity_value_t;

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/
    /**
     * @brief Function to operate on actions
     *
     * @param action Current Action to Operate on
     * @param item Target-Item node
     * @param arg Arg for action
     * @param user_arg User-arg
     * @return ezlopi_error_t
     */
    int HILINK_presence_sensor_initialize_settings(l_ezlopi_device_t *device);
    /**
     * @brief Function to apply HILINK_presence_sensor setting
     *
     * @return int
     */
    int HILINK_presence_sensor_apply_settings(void);
    /**
     * @brief Functio to set moving target range
     *
     * @param moving_target_distance target distance
     * @return true
     * @return false
     */
    bool HILINK_presence_sensor_target_in_detectable_range(const uint16_t moving_target_distance);

#ifdef __cplusplus
}
#endif

#endif // _HILINK_PRESENCE_SENSOR_SETTINGS_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/