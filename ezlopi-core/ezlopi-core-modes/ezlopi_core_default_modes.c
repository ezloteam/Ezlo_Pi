#include <stdio.h>
#include <string.h>

#include "ezlopi_core_modes.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_devices.h"

#include "ezlopi_cloud_constants.h"

#if defined(CONFIG_EZPI_SERV_ENABLE_MODES)

static s_ezlopi_modes_t sg_default_mode = {
    .current_mode_id = 0,
    .switch_to_mode_id = 0,

    // .time_is_left_to_alarm_sec = 0,
    .alarm_delay = 0,
    .switch_to_delay_sec = 0,
    .time_is_left_to_switch_sec = 0,
    .switch_to_delay_runs_out = 0,

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
        .entry_delay_sec = 10, // Actual delay to be triggered immediately ; [if house-mode.alarm = true] .
        .time_is_left_sec = 8, // Time remaining to exit delay
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

    .armed = false,   // Default values: Home: false, Away: true, Night: true, Vacation: true
    .protect = false, // Home: false, Away: true, Night: true, Vacation: true.
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

static void __cjson_add_security_device_to_array(cJSON *cj_device_array)
{
    if (cj_device_array && (cj_device_array->type == cJSON_Array))
    {
        l_ezlopi_device_t *curr_device = ezlopi_device_get_head();
        while (curr_device)
        {
            if (EZPI_STRNCMP_IF_EQUAL(category_security_sensor, curr_device->cloud_properties.category, strlen(category_security_sensor), strlen(curr_device->cloud_properties.category)))
            {
                char temp[32];
                memset(temp, 0, 32);
                snprintf(temp, 32, "%08X", curr_device->cloud_properties.device_id);
                cJSON_AddItemToArray(cj_device_array, cJSON_CreateString(__func__, temp));
            }
            curr_device = curr_device->next;
        }
    }
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

    // Adding valid 'security_device' from 'l_ezlopi_device_t' .
    sg_default_mode.cj_devices = cJSON_CreateArray(__func__);
    if (sg_default_mode.cj_devices)
    {
        __cjson_add_security_device_to_array(sg_default_mode.cj_devices);
    }

    sg_default_mode.current_mode_id = EZLOPI_HOUSE_MODE_REF_ID_HOME;

    memcpy(&sg_default_mode.mode_home, &sg_default_house_mode, sizeof(s_house_modes_t));
    sg_default_mode.mode_home.name = ezlopi_Home_str;
    sg_default_mode.mode_home.description = (char *)ezlopi_Home_str;
    sg_default_mode.mode_home.armed = false;
    sg_default_mode.mode_home.protect = true;
    sg_default_mode.mode_home._id = EZLOPI_HOUSE_MODE_REF_ID_HOME;

    memcpy(&sg_default_mode.mode_away, &sg_default_house_mode, sizeof(s_house_modes_t));
    sg_default_mode.mode_away.name = ezlopi_Away_str;
    sg_default_mode.mode_away.description = (char *)ezlopi_Away_str;
    sg_default_mode.mode_away.armed = true;
    sg_default_mode.mode_away.protect = true;
    sg_default_mode.mode_away._id = EZLOPI_HOUSE_MODE_REF_ID_AWAY;

    memcpy(&sg_default_mode.mode_night, &sg_default_house_mode, sizeof(s_house_modes_t));
    sg_default_mode.mode_night.name = ezlopi_Night_str;
    sg_default_mode.mode_night.description = (char *)ezlopi_Night_str;
    sg_default_mode.mode_night.armed = true;
    sg_default_mode.mode_night.protect = true;
    sg_default_mode.mode_night._id = EZLOPI_HOUSE_MODE_REF_ID_NIGHT;

    memcpy(&sg_default_mode.mode_vacation, &sg_default_house_mode, sizeof(s_house_modes_t));
    sg_default_mode.mode_vacation.name = ezlopi_Vacation_str;
    sg_default_mode.mode_vacation.description = (char *)ezlopi_Vacation_str;
    sg_default_mode.mode_vacation.armed = true;
    sg_default_mode.mode_vacation.protect = true;
    sg_default_mode.mode_vacation._id = EZLOPI_HOUSE_MODE_REF_ID_VACATION;
}

#endif // CONFIG_EZPI_SERV_ENABLE_MODES