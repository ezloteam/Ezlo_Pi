#ifndef __EZLOPI_HOME_MODES_H__
#define __EZLOPI_HOME_MODES_H__

#include <cJSON.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

// https://api.ezlo.com/hub/house_modes_manager/index.html

typedef struct s_house_modes
{
    uint32_t _id;
    uint32_t alarm_delay;
    uint32_t switch_to_delay;

    char name[32];
    char description[128];

    bool armed;
    bool protect;
    bool disarmed_default;

    cJSON *cj_notifications;
    cJSON *cj_bypass_devices;
    cJSON *cj_disarmed_devices;
    cJSON *cj_alarms_off_devices;
    cJSON *cj_cameras_off_devices;

    struct s_house_modes *next;

} s_house_modes_t;

typedef struct s_entry_delay
{
    uint32_t short_delay;
    uint32_t normal_delay;
    uint32_t extended_delay;
    uint32_t instant_delay;

} s_entry_delay_t;

typedef struct s_abort_window
{
    uint32_t default_delay;
    uint32_t minimum_delay;
    uint32_t maximum_delay;

} s_abort_window_t;

typedef struct s_protect_buttons
{
    uint32_t device_id;
    void (*func)(void);
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
    char type[32];
    uint32_t entry_delay;
    uint32_t time_is_left;
    s_sources_t *srouces;

} s_alarmed_t;

typedef struct s_ezlopi_modes
{
    uint32_t sg_alarm_delay;
    uint32_t sg_switch_to_delay;
    uint32_t sg_time_is_left_to_switch;

    cJSON *cj_alarms;
    cJSON *cj_cameras;
    cJSON *cj_devices;
    s_protect_buttons_t *l_protect_buttons;

    s_entry_delay_t entry_delay;
    s_abort_window_t abort_delay;

    s_house_modes_t *l_modes;
    s_alarmed_t alarmed;

} s_ezlopi_modes_t;

#endif // __EZLOPI_HOME_MODES_H__
