#include <stdio.h>
#include <string.h>

#include "ezlopi_core_modes.h"

static s_ezlopi_modes_t default_mode = {
    // Home

    .alarm_delay_ms = 30 * 1000,
    .switch_to_delay_ms = 30 * 1000,
    .time_is_left_to_switch_ms = 30 * 1000,

    .cj_alarms = NULL,
    .cj_cameras = NULL,
    .cj_devices = NULL,
    .l_protect_buttons = NULL,

    .entry_delay = {
        .short_delay_ms = 30 * 1000,
        .normal_delay_ms = 30 * 1000,
        .extended_delay_ms = 30 * 1000,
        .instant_delay_ms = 0,
    },

    .abort_delay = {
        .default_delay_ms = 30 * 1000,
        .minimum_delay_ms = 30 * 1000,
        .maximum_delay_ms = 240 * 1000,
    },

    .alarmed = {
        .type = "",
        .entry_delay_ms = 30 * 1000,
        .time_is_left_ms = 30 * 1000,
        .srouces = NULL,
    },

    .mode_home = NULL,
    .mode_away = NULL,
    .mode_night = NULL,
    .mode_vacation = NULL,
};

static const s_house_modes_t default_house_mode_home = {
    ._id = 0,
    .alarm_delay_ms = 30 * 1000,
    .switch_to_delay_ms = 30 * 1000,

    .name = "",
    .description = "",

    .armed = false,
    .protect = false,
    .disarmed_default = false,

    .cj_notifications = NULL,
    .cj_bypass_devices = NULL,
    .cj_disarmed_devices = NULL,
    .cj_alarms_off_devices = NULL,
    .cj_cameras_off_devices = NULL,
};

s_ezlopi_modes_t *ezlopi_core_default_mode_get(void)
{
    return &default_mode;
}

void ezlopi_core_default_init(void)
{
    default_mode.mode_away = &default_house_mode_home;
    default_mode.mode_home = &default_house_mode_home;
    default_mode.mode_night = &default_house_mode_home;
    default_mode.mode_vacation = &default_house_mode_home;
}