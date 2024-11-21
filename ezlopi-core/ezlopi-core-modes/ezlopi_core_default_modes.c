#include <stdio.h>
#include <string.h>

#include "ezlopi_core_modes.h"
#include "ezlopi_core_cloud.h"

#include "ezlopi_cloud_constants.h"

#if defined(CONFIG_EZPI_SERV_ENABLE_MODES)

static s_ezlopi_modes_t sg_default_mode = {
    .current_mode_id = 0,
    .switch_to_mode_id = 0,

    // .time_is_left_to_alarm_sec = 0,
    .alarm_delay = 0,
    .switch_to_delay_sec = 0,
    .time_is_left_to_switch_sec = 0,

    .cj_alarms = NULL,
    .cj_cameras = NULL,
    .cj_devices = NULL,
    .l_protect_buttons = NULL,

    .entry_delay = {
        .normal_delay_sec = 30,
        .extended_delay_sec = 60,
        .long_extended_delay_sec = 120,
        .instant_delay_sec = 0,
    },

    .abort_delay = {
        .default_delay_sec = 30,
        .minimum_delay_sec = 30,
        .maximum_delay_sec = 240,
    },

    .alarmed = {
        .type = "global",
        .entry_delay_sec = 10, // this delay represent the alarm_delay for 'entry_delay_phase' only
        .time_is_left_sec = 8,
        .sources = NULL,
        .phase = EZLOPI_MODES_ALARM_PHASE_NONE,
        .status = EZLOPI_MODES_ALARM_STATUS_DONE,
        .silent = false,
    },
};

static s_house_modes_t sg_default_house_mode = {
    ._id = 0,
    .alarm_delay_sec = 30,
    .switch_to_delay_sec = 30,

    .name = "",
    .description = "",

    .armed = false,
    .protect = false,
    .disarmed_default = true,

    .cj_notifications = NULL,
    .cj_bypass_devices = NULL,
    .cj_disarmed_devices = NULL,
    .cj_alarms_off_devices = NULL,
    .cj_cameras_off_devices = NULL,
};

s_ezlopi_modes_t *ezlopi_core_default_mode_get(void)
{
    return &sg_default_mode;
}

void ezlopi_core_default_init(void)
{
    sg_default_house_mode.cj_notifications = NULL;
    sg_default_house_mode.cj_bypass_devices = NULL;
    sg_default_house_mode.cj_disarmed_devices = NULL;
    sg_default_house_mode.cj_alarms_off_devices = NULL;
    sg_default_house_mode.cj_cameras_off_devices = NULL;

    sg_default_mode.cj_alarms = NULL;
    sg_default_mode.cj_cameras = NULL;
    sg_default_mode.cj_devices = NULL;

    sg_default_mode.current_mode_id = EZLOPI_HOUSE_MODE_REF_ID_HOME;

    memcpy(&sg_default_mode.mode_home, &sg_default_house_mode, sizeof(s_house_modes_t));
    sg_default_mode.mode_home.name = ezlopi_Home_str;
    sg_default_mode.mode_home.description = (char *)ezlopi_Home_str;
    sg_default_mode.mode_home._id = EZLOPI_HOUSE_MODE_REF_ID_HOME;

    memcpy(&sg_default_mode.mode_away, &sg_default_house_mode, sizeof(s_house_modes_t));
    sg_default_mode.mode_away.name = ezlopi_Away_str;
    sg_default_mode.mode_away.description = (char *)ezlopi_Away_str;
    sg_default_mode.mode_away._id = EZLOPI_HOUSE_MODE_REF_ID_AWAY;

    memcpy(&sg_default_mode.mode_night, &sg_default_house_mode, sizeof(s_house_modes_t));
    sg_default_mode.mode_night.name = ezlopi_Night_str;
    sg_default_mode.mode_night.description = (char *)ezlopi_Night_str;
    sg_default_mode.mode_night._id = EZLOPI_HOUSE_MODE_REF_ID_NIGHT;

    memcpy(&sg_default_mode.mode_vacation, &sg_default_house_mode, sizeof(s_house_modes_t));
    sg_default_mode.mode_vacation.name = ezlopi_Vacation_str;
    sg_default_mode.mode_vacation.description = (char *)ezlopi_Vacation_str;
    sg_default_mode.mode_vacation._id = EZLOPI_HOUSE_MODE_REF_ID_VACATION;
}

#endif // CONFIG_EZPI_SERV_ENABLE_MODES