#include <stdio.h>
#include <string.h>

#include "ezlopi_core_modes.h"
#include "ezlopi_core_cloud.h"

#include "ezlopi_cloud_constants.h"

static s_ezlopi_modes_t sg_default_mode = {
    .current_mode_id = 0,
    .switch_to_mode_id = 0,

    .alarm_delay_sec = 0,
    .switch_to_delay_sec = 0,
    .time_is_left_to_switch_sec = 0,

    .cj_alarms = NULL,
    .cj_cameras = NULL,
    .cj_devices = NULL,
    .l_protect_buttons = NULL,

    .entry_delay = {
        .short_delay_sec = 30,
        .normal_delay_sec = 30,
        .extended_delay_sec = 30,
        .instant_delay_sec = 0,
    },

    .abort_delay = {
        .default_delay_sec = 30,
        .minimum_delay_sec = 30,
        .maximum_delay_sec = 240,
    },

    .alarmed = {
        .type = "global",
        .entry_delay_sec = 10,
        .time_is_left_sec = 8,
        .sources = NULL,
    },

    .mode_home = NULL,
    .mode_away = NULL,
    .mode_night = NULL,
    .mode_vacation = NULL,
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
    cJSON *cj_empty_array = cJSON_CreateArray();
    sg_default_house_mode.cj_alarms_off_devices = cj_empty_array;
    sg_default_house_mode.cj_bypass_devices = cj_empty_array;
    sg_default_house_mode.cj_cameras_off_devices = cj_empty_array;
    sg_default_house_mode.cj_disarmed_devices = cj_empty_array;
    sg_default_house_mode.cj_notifications = cj_empty_array;

    sg_default_mode.cj_alarms = cj_empty_array;
    sg_default_mode.cj_cameras = cj_empty_array;
    sg_default_mode.cj_devices = cj_empty_array;

    uint32_t initial_id = ezlopi_cloud_get_modes_initial_id();

    sg_default_mode.mode_home = (s_house_modes_t *)malloc(sizeof(s_house_modes_t));
    if (sg_default_mode.mode_home)
    {
        memcpy(sg_default_mode.mode_home, &sg_default_house_mode, sizeof(s_house_modes_t));

        sg_default_mode.mode_home->_id = initial_id + 1;
        sg_default_mode.mode_home->description = ezlopi_mode_home_str;
        snprintf(sg_default_mode.mode_home->name, sizeof(sg_default_mode.mode_vacation->name), "Home");
    }

    sg_default_mode.mode_away = (s_house_modes_t *)malloc(sizeof(s_house_modes_t));
    if (sg_default_mode.mode_away)
    {
        memcpy(sg_default_mode.mode_away, &sg_default_house_mode, sizeof(s_house_modes_t));

        sg_default_mode.mode_away->_id = initial_id + 2;
        sg_default_mode.mode_away->description = ezlopi_mode_away_str;
        snprintf(sg_default_mode.mode_away->name, sizeof(sg_default_mode.mode_vacation->name), "Away");
    }

    sg_default_mode.mode_night = (s_house_modes_t *)malloc(sizeof(s_house_modes_t));
    if (sg_default_mode.mode_night)
    {
        memcpy(sg_default_mode.mode_night, &sg_default_house_mode, sizeof(s_house_modes_t));

        sg_default_mode.mode_night->_id = initial_id + 3;
        sg_default_mode.mode_night->description = ezlopi_mode_night_str;
        snprintf(sg_default_mode.mode_night->name, sizeof(sg_default_mode.mode_vacation->name), "Night");
    }

    sg_default_mode.mode_vacation = (s_house_modes_t *)malloc(sizeof(s_house_modes_t));
    if (sg_default_mode.mode_vacation)
    {
        memcpy(sg_default_mode.mode_vacation, &sg_default_house_mode, sizeof(s_house_modes_t));

        sg_default_mode.mode_vacation->_id = initial_id + 4;
        sg_default_mode.mode_vacation->description = ezlopi_mode_vacation_str;
        snprintf(sg_default_mode.mode_vacation->name, sizeof(sg_default_mode.mode_vacation->name), "Vacation");
    }
}
