#ifndef __EZLOPI_HOME_MODES_H__
#define __EZLOPI_HOME_MODES_H__

#include <cJSON.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

// https://api.ezlo.com/hub/house_modes_manager/index.html

typedef enum e_house_modes_type
{
    EZLOPI_HOUSE_MODES_NONE,
    EZLOPI_HOUSE_MDOES_HOME,
    EZLOPI_HOUSE_MODES_AWAY,
    EZLOPI_HOUSE_MODES_NIGHT,
    EZLOPI_HOUSE_MODES_VACATION,
    EZLOPI_HOUSE_MODES_MAX,
} e_house_modes_type_t;

typedef struct s_house_modes
{
    uint32_t _id;
    uint32_t alarm_delay_ms;
    uint32_t switch_to_delay_ms;

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

} s_house_modes_t;

typedef struct s_entry_delay
{
    uint32_t short_delay_ms;
    uint32_t normal_delay_ms;
    uint32_t extended_delay_ms;
    uint32_t instant_delay_ms;

} s_entry_delay_t;

typedef struct s_abort_window
{
    uint32_t default_delay_ms;
    uint32_t minimum_delay_ms;
    uint32_t maximum_delay_ms;

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
    char type[32]; // default is 'global'
    uint32_t entry_delay_ms;
    uint32_t time_is_left_ms;
    s_sources_t *srouces;

} s_alarmed_t;

typedef struct s_ezlopi_modes
{
    uint32_t current_mode_id;
    uint32_t switch_to_mode_id;

    double alarm_delay_ms;
    double switch_to_delay_ms;
    double time_is_left_to_switch_ms;

    cJSON *cj_alarms;
    cJSON *cj_cameras;
    cJSON *cj_devices;
    s_protect_buttons_t *l_protect_buttons;

    s_entry_delay_t entry_delay;
    s_abort_window_t abort_delay;

    s_alarmed_t alarmed;

    s_house_modes_t *mode_home;
    s_house_modes_t *mode_away;
    s_house_modes_t *mode_night;
    s_house_modes_t *mode_vacation;

} s_ezlopi_modes_t;

void ezlopi_core_default_init(void);
s_ezlopi_modes_t *ezlopi_core_default_mode_get(void);

void ezlopi_core_modes_init(void);

#endif // __EZLOPI_HOME_MODES_H__
