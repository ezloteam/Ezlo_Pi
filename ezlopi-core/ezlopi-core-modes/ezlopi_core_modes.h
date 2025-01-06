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
 * @file    ezlopi_core_modes.h
 * @brief   These function perform opertation on HouseModes
 * @author  xx
 * @version 0.1
 * @date    12th DEC 2024
 */

#ifndef __EZLOPI_HOME_MODES_H__
#define __EZLOPI_HOME_MODES_H__

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "cjext.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "EZLOPI_USER_CONFIG.h"

#include "ezlopi_core_errors.h"

#if defined(CONFIG_EZPI_SERV_ENABLE_MODES)

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

    typedef enum e_modes_ref_id_idx
    {
        EZLOPI_HOUSE_MODE_REF_ID_NONE = 0,
        EZLOPI_HOUSE_MODE_REF_ID_HOME,
        EZLOPI_HOUSE_MODE_REF_ID_AWAY,
        EZLOPI_HOUSE_MODE_REF_ID_NIGHT,
        EZLOPI_HOUSE_MODE_REF_ID_VACATION,
        EZLOPI_HOUSE_MODE_REF_ID_MAX
    } e_modes_ref_id_idx_t;

    // https://api.ezlo.com/hub/house_modes_manager/index.html

    typedef enum e_house_modes_type
    {
        EZLOPI_HOUSE_MODES_NONE = 0,
        EZLOPI_HOUSE_MODES_HOME,
        EZLOPI_HOUSE_MODES_AWAY,
        EZLOPI_HOUSE_MODES_NIGHT,
        EZLOPI_HOUSE_MODES_VACATION,
        EZLOPI_HOUSE_MODES_MAX,
    } e_house_modes_type_t;

    typedef enum e_modes_alarm_phase
    {
        EZLOPI_MODES_ALARM_PHASE_NONE = 0,
        EZLOPI_MODES_ALARM_PHASE_IDLE,
        EZLOPI_MODES_ALARM_PHASE_BYPASS,
        EZLOPI_MODES_ALARM_PHASE_ENTRYDELAY,
        EZLOPI_MODES_ALARM_PHASE_MAIN,
        EZLOPI_MODES_ALARM_PHASE_MAX,
    } e_modes_alarm_phase_t;

    typedef enum e_modes_alarm_status
    {
        EZLOPI_MODES_ALARM_STATUS_DONE = 0,
        EZLOPI_MODES_ALARM_STATUS_BEGIN,
        EZLOPI_MODES_ALARM_STATUS_CANCELED
    } e_modes_alarm_status_t;

    typedef struct s_house_modes // this resembles a category of 'mode'.
    {
        const char *name;
        char *description;
        uint32_t _id;                 // Numeric representation of the 'House-Mode'
        uint32_t switch_to_delay_sec; // represent a delay before switching to a perticular 'mode'.
        uint32_t alarm_delay_sec;     // Delay before sending alert if armed sensors (door/window or motion sensor) tripped .
        bool armed;                   // flag to indicate if the mode enters the alarmed mode. // Default values: [Home: false], [Away: true], [Night: true], [Vacation: true]
        bool protect;                 // Enables or disables Ezlo Protect for a particular house mode. // Default values: [Home: false], [Away: true], [Night: true], [Vacation: true]
        bool disarmed_default;        // if true ; utilize the disarmed devices.
        bool notify_all;              // This Flag indicates, notifiations trigger to all user_IDs

        cJSON *cj_notifications;       // Specific list of user_IDs to notify
        cJSON *cj_bypass_devices;      // Devices bypass list --> for no alert/security consequences.
        cJSON *cj_disarmed_devices;    // NOTE: BY default contains -> 'alarm and camera devices' ; So besides these, other devices have to be added mannually
        cJSON *cj_alarms_off_devices;  // (auto add alarm-type devices) // these devices are auto added to 'cj_disarmed_devices'
        cJSON *cj_cameras_off_devices; // (auto add camera-type devices) // these devices are auto added to 'cj_disarmed_devices'

    } s_house_modes_t;

    typedef struct s_entry_delay
    {
        uint32_t long_extended_delay_sec;
        // uint32_t short_delay_sec; // not short should be'long_extentded' in (hub.modes v3.0)
        uint32_t normal_delay_sec;
        uint32_t extended_delay_sec;
        uint32_t instant_delay_sec;

    } s_entry_delay_t;

    typedef struct s_abort_window
    {
        uint32_t default_delay_sec;
        uint32_t minimum_delay_sec;
        uint32_t maximum_delay_sec;

    } s_abort_window_t;

    typedef struct s_protect_buttons
    {
        uint32_t device_id;
        void (*func)(void);
        char service_name[32];
        struct s_protect_buttons *next;

    } s_protect_buttons_t;

    typedef struct s_sources
    {
        uint32_t delay;
        uint32_t device_id;
        long long time_stamp;
        struct s_sources *next;

    } s_sources_t;

    typedef struct s_alarmed
    {
        char type[32];                      // default is 'global' ( Indicates that the alarmDelay value was taken from the house modes settings)
        uint32_t entry_delay_sec;           // If house modes alarmed, and HouseModes.alarmDelay > 0, entry delay period is started . [The default value is given from 'MODE->s_entry_delay_t' choice]
        volatile uint32_t time_is_left_sec; // Number of seconds left to the end of the Entry delay.
        bool silent;                        // Default: false ... When : true ; websocket clients should treat the alarm as the silent alarm: no indication of alarm is allowed.
        e_modes_alarm_phase_t phase;        // --> [Not in  documentation ; Added for broadcast purpose] === alarm_phases_type : [idle / bypassed / entryDelay / main]
        e_modes_alarm_status_t status;      // --> [Not in  documentation ; Added for broadcast purpose] === House_mode status for 'alaram_phase'
        s_sources_t *sources;               // Contains an array of devices that waiting for the entry delay to finish. They are security devices which emitted security events
    } s_alarmed_t;

    typedef struct s_ezlopi_modes
    {
        uint32_t current_mode_id;
        uint32_t switch_to_mode_id;

        uint32_t time_is_left_to_switch_sec; //  (switch_to_delay_sec - N_sec) //Time left (sec) after start to switch to the mode
        uint32_t switch_to_delay_sec;        //  Delay (sec) before switch to the all modes // this holds a copy to actual 'SwitchDelay' of active 'houseMode'
        uint32_t alarm_delay;                //  Delay (sec) before sending alert if armed sensors (door/window or motion sensor) tripped. // [https://log.ezlo.com/new/hub/house_modes_manager/#hubmodesget-version-20] 	Delay (sec) before sending alert to the all modes   // NOTE : [(alarm_delay_sec > 0) === means 'mode->alarmed' member exists ]
        uint32_t switch_to_delay_runs_out;   //  A period of 'exit-time-delay' ; switching to/from the armed mode.

        cJSON *cj_alarms;  // Array of device id which make alarms after trips (Main alert list)
        cJSON *cj_cameras; // Array of camera device identifiers with items named make_recording
        cJSON *cj_devices; // Array of device id with security sensors
        s_protect_buttons_t *l_protect_buttons;

        s_entry_delay_t entry_delay;  // A dictionary for Entry Delays values
        s_abort_window_t abort_delay; // 'default / min / max' - delay values

        s_alarmed_t alarmed; // NOTE :: Present only if the house modes enter the alarmed state. [ie. must have { MODES->house_mode.armed = true}  ]

        s_house_modes_t mode_home; // this structure specifies configuration of 'home-MODE'
        s_house_modes_t mode_away;
        s_house_modes_t mode_night;
        s_house_modes_t mode_vacation;

    } s_ezlopi_modes_t;

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/

    /**
     * @brief This function initializes House-mode task
     */
    void EZPI_core_modes_init(void);
    /**
     * @brief This function initializes default house-mode information
     */
    void EZPI_core_default_init(void);
    /**
     * @brief This function store new house-mode to nvs
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_modes_store_to_nvs(void);

    /**
     * @brief This function initializes default house-mode information
     */
    s_ezlopi_modes_t *EZPI_core_default_mode_get(void);
    /**
     * @brief This function returns house-mode information by id
     *
     * @param house_mode_id Target house-mode-id
     * @return s_house_modes_t *
     */
    s_house_modes_t *EZPI_core_modes_get_house_mode_by_id(uint32_t house_mode_id);
    /**
     * @brief This function returns house-mode information by name
     *
     * @param house_mode_name Name of target house-mode
     * @return s_house_modes_t *
     */
    s_house_modes_t *EZPI_core_modes_get_house_mode_by_name(char *house_mode_name);

    /**
     * @brief This function return active house-mode information
     *
     * @return s_ezlopi_modes_t *
     */
    s_ezlopi_modes_t *EZPI_core_modes_get_custom_modes(void);
    /**
     * @brief This function return active house-mode struct information
     *
     * @return s_house_modes_t *
     */
    s_house_modes_t *EZPI_core_modes_get_current_house_modes(void);
    /**
     * @brief This function sets 'new-house-mode' as active
     *
     * @param new_house_mode Name of target house-mode
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_modes_set_current_house_mode(s_house_modes_t *new_house_mode);
    /**
     * @brief This function sets/resets device armed flag
     *
     * @param cj_device_array Pointer to array of target-devices
     * @param set Value to set
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_modes_set_unset_device_armed_status(cJSON *cj_device_array, const bool set);

    /**
     * @brief This function extracts house-mode information from 'cj_result'
     *
     * @param cj_result Destinaiton which stores modes
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_modes_api_get_modes(cJSON *cj_result);
    /**
     * @brief This function extract house-mode information
     *
     * @param cj_result Destinaiton which stores modes info
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_modes_api_get_current_mode(cJSON *cj_result);
    /**
     * @brief This function switch-mode to new mode
     *
     * @param switch_to_house_mode Pointer to new node
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_modes_api_switch_mode(s_house_modes_t *switch_to_house_mode);
    /**
     * @brief This function cancels switching of new mode
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_modes_api_cancel_switch(void);
    /**
     * @brief This function cancel entry-delay-phase of 'house-mode' loop
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_modes_api_cancel_entry_delay(void);
    /**
     * @brief This function skips entry-delay-phase of 'house-mode' loop
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_modes_api_skip_entry_delay(void);
    /**
     * @brief This function sets the delay time before switching for active modes
     *
     * @param switch_to_delay switch_to_delay time in sec
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_modes_api_set_switch_to_delay(uint32_t switch_to_delay);
    /**
     * @brief This function sets alarmDelay for active modes
     *
     * @param switch_to_delay AlarmDelay time in sec
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_modes_api_set_alarm_delay(uint32_t switch_to_delay);
    /**
     * @brief This function sets notification_ids for active modes
     *
     * @param modeId Target ModeID to modify
     * @param all Flag for notification-type
     * @param user_id_aray Target Ids to add
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_modes_api_set_notifications(uint8_t modeId, bool all, cJSON *user_id_aray);
    /**
     * @brief This function sets disarmed_default for active modes
     *
     * @param modeId target modeId to set the flag for
     * @param disarmedDefault value of flag to set
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_modes_api_set_disarmed_default(uint8_t modeId, bool disarmedDefault);
    /**
     * @brief This function add disarmed_default for active modes
     *
     * @param modeId Flag for notification-type
     * @param device_id_str  Id of the device to add
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_modes_api_add_disarmed_device(uint8_t modeId, const char *device_id_str);
    /**
     * @brief This function removes devices from disarmed_device list
     *
     * @param modeId Target mode_id
     * @param device_id_str Id of the device to remove
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_modes_api_remove_disarmed_device(uint8_t modeId, const char *device_id_str);
    /**
     * @brief This function sets protect flag for a house-mode
     *
     * @param modeId Target mode_id
     * @param protect_state Flag to protect a mode
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_modes_api_set_protect(uint32_t modeId, bool protect_state);
    /**
     * @brief This function sets 'protect_button' information
     *
     * @param service_str Type of protect service for the button
     * @param deviceId Id of the device to remove
     * @param status Status of the button changes
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_modes_api_set_protect_button(char *service_str, uint32_t deviceId, uint8_t *status);
    /**
     * @brief This function adds devices to 'protect'
     *
     * @param user_id_aray Array of user_ids to add
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_modes_api_add_protect_devices(cJSON *user_id_aray);
    /**
     * @brief This function adds devices to 'protect'
     *
     * @param user_id_aray Array of user_ids to remove
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_modes_api_remove_protect_devices(cJSON *user_id_aray);

    /**
     * @brief This function to add alarm_off devices from perticular mode
     *
     * @param modeId The target-mode-id
     * @param device_id_str alarm_off device_id to add
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_modes_api_add_alarm_off(uint8_t modeId, const char *device_id_str);
    /**
     * @brief This function to remove alarm_off devices from perticular mode
     *
     * @param modeId The target-mode-id
     * @param device_id_str alarm_off device_id to remove
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_modes_api_remove_alarm_off(uint32_t modeId, const char *device_id_str);
    /**
     * @brief This function to add cameras_off devices from perticular mode
     *
     * @param modeId The target-mode-id
     * @param device_id_str cameras_off device_id to add
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_modes_api_add_cameras_off(uint8_t modeId, const char *device_id_str);
    /**
     * @brief This function to remove cameras_off devices from perticular mode
     *
     * @param modeId The target-mode-id
     * @param device_id_str cameras_off device_id to remove
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_modes_api_remove_cameras_off(uint8_t modeId, const char *device_id_str);

    /**
     * @brief This function to add bypass devices from perticular mode
     *
     * @param modeId The target-mode-id
     * @param cj_device_id_array List of bypass devices to add
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_modes_api_bypass_device_add(uint8_t modeId, cJSON *cj_device_id_array);
    /**
     * @brief This function to remove bypass devices from perticular mode
     *
     * @param modeId The target-mode-id
     * @param cj_device_id_array List of bypass devices to remove
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_modes_api_bypass_device_remove(uint8_t modeId, cJSON *cj_device_id_array);

    /**
     * @brief This function to get current house-mode-id
     *
     * @param cj_result Cjson that stores active house-mode-id
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_modes_cjson_get_current_mode(cJSON *cj_result);
    /**
     * @brief This function sets entry_delay
     *
     * @param normal_sec Normal entry-delay sec
     * @param extended_sec Extended entry-delay sec
     * @param long_extended_sec Long extended entry-delay sec
     * @param instant_sec Instant entry-delay sec
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_modes_api_set_entry_delay(uint32_t normal_sec, uint32_t extended_sec, uint32_t long_extended_sec, uint32_t instant_sec);
    /**
     * @brief This function resets entry_delay
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_modes_api_reset_entry_delay(void);

#ifdef __cplusplus
}
#endif

#endif // CONFIG_EZPI_SERV_ENABLE_MODES

#endif // __EZLOPI_HOME_MODES_H__

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
