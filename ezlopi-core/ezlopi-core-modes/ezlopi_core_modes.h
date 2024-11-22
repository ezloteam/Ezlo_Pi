#ifndef __EZLOPI_HOME_MODES_H__
#define __EZLOPI_HOME_MODES_H__

#include "cjext.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "EZLOPI_USER_CONFIG.h"

#include "ezlopi_core_errors.h"

#if defined(CONFIG_EZPI_SERV_ENABLE_MODES)
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
    uint32_t _id;                 // Numeric representation of the 'House-Mode'
    uint32_t switch_to_delay_sec; // represent a delay before switching to a perticular 'mode'.
    uint32_t alarm_delay_sec;     // Delay before sending alert if armed sensors (door/window or motion sensor) tripped .

    const char *name;
    char *description;

    bool armed;            // flag to indicate if the mode enters the alarmed mode. // Default values: [Home: false], [Away: true], [Night: true], [Vacation: true]
    bool protect;          // Enables or disables Ezlo Protect for a particular house mode. // Default values: [Home: false], [Away: true], [Night: true], [Vacation: true]
    bool disarmed_default; // if true ; utilize the disarmed devices.
    bool notify_all;       // This Flag indicates, notifiations trigger to all user_IDs

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
    uint32_t switch_to_delay_sec;        // Delay (sec) before switch to the all modes // this holds a copy to actual 'SwitchDelay' of active 'houseMode'
    uint32_t alarm_delay;                // Delay (sec) before sending alert if armed sensors (door/window or motion sensor) tripped. // [https://log.ezlo.com/new/hub/house_modes_manager/#hubmodesget-version-20] 	Delay (sec) before sending alert to the all modes   // NOTE : [(alarm_delay_sec > 0) === means 'mode->alarmed' member exists ]

    cJSON *cj_alarms;  // Array of device id which make alarms after trips
    cJSON *cj_cameras; // Array of camera device identifiers with items named make_recording
    cJSON *cj_devices; // Array of device id with security sensors
    s_protect_buttons_t *l_protect_buttons;

    s_entry_delay_t entry_delay; // A dictionary for Entry Delays values
    s_abort_window_t abort_delay;

    s_alarmed_t alarmed; // NOTE :: Present only if the house modes enter the alarmed state. [ie. must have { MODES->house_mode.armed = true}  ]

    s_house_modes_t mode_home; // this structure specifies configuration of 'home-MODE'
    s_house_modes_t mode_away;
    s_house_modes_t mode_night;
    s_house_modes_t mode_vacation;

} s_ezlopi_modes_t;

void ezlopi_core_modes_init(void);
void ezlopi_core_default_init(void);
ezlopi_error_t ezlopi_core_modes_store_to_nvs(void);
s_ezlopi_modes_t *ezlopi_core_default_mode_get(void);
s_house_modes_t *ezlopi_core_modes_get_house_mode_by_id(uint32_t house_mode_id);
s_house_modes_t *ezlopi_core_modes_get_house_mode_by_name(char *house_mode_name);

s_ezlopi_modes_t *ezlopi_core_modes_get_custom_modes(void);
s_house_modes_t *ezlopi_core_modes_get_current_house_modes(void);
ezlopi_error_t ezlopi_core_modes_set_current_house_mode(s_house_modes_t *new_house_mode);

ezlopi_error_t ezlopi_core_modes_api_get_modes(cJSON *cj_result);
ezlopi_error_t ezlopi_core_modes_api_get_current_mode(cJSON *cj_result);
ezlopi_error_t ezlopi_core_modes_api_switch_mode(s_house_modes_t *switch_to_house_mode);
ezlopi_error_t ezlopi_core_modes_api_cancel_switch(void);
ezlopi_error_t ezlopi_core_modes_api_cancel_entry_delay(void);
ezlopi_error_t ezlopi_core_modes_api_skip_entry_delay(void);
ezlopi_error_t ezlopi_core_modes_set_switch_to_delay(uint32_t switch_to_delay);
ezlopi_error_t ezlopi_core_modes_set_alarm_delay(uint32_t switch_to_delay);
ezlopi_error_t ezlopi_core_modes_set_notifications(uint8_t modesId, bool all, cJSON *user_id_aray);
ezlopi_error_t ezlopi_core_modes_set_disarmed_default(uint8_t modesID, bool disarmedDefault);
ezlopi_error_t ezlopi_core_modes_set_unset_device_armed_status(cJSON *cj_device_array, const bool set);
ezlopi_error_t ezlopi_core_modes_add_disarmed_device(uint8_t modeId, const char *device_id_str);
ezlopi_error_t ezlopi_core_modes_remove_disarmed_device(uint8_t modeId, const char *device_id_str);
ezlopi_error_t ezlopi_core_modes_set_protect(uint32_t mode_id, bool protect_state);

ezlopi_error_t ezlopi_core_modes_add_alarm_off(uint8_t mode_id, cJSON *device_id);
ezlopi_error_t ezlopi_core_modes_remove_alarm_off(uint32_t mode_id, cJSON *device_id);

ezlopi_error_t ezlopi_core_modes_bypass_device_add(uint8_t modeId, cJSON *cj_device_id_array);
ezlopi_error_t ezlopi_core_modes_bypass_device_remove(uint8_t modeId, cJSON *cj_device_id_array);

ezlopi_error_t ezlopi_core_modes_cjson_get_current_mode(cJSON *cj_result);
ezlopi_error_t ezlopi_core_modes_set_entry_delay(uint32_t normal_sec, uint32_t extended_sec, uint32_t long_extended_sec, uint32_t instant_sec);
ezlopi_error_t ezlopi_core_modes_reset_entry_delay(void);

///
cJSON *ezlopi_core_modes_cjson_changed(void);
cJSON *ezlopi_core_modes_cjson_alarmed(void);

#endif // CONFIG_EZPI_SERV_ENABLE_MODES

#endif // __EZLOPI_HOME_MODES_H__