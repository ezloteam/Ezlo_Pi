#ifndef __EZLOPI_HOME_MODES_H__
#define __EZLOPI_HOME_MODES_H__

#include "cjext.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "EZLOPI_USER_CONFIG.h"

#if defined (CONFIG_EZPI_SERV_ENABLE_MODES)
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
    uint32_t alarm_delay_sec;
    uint32_t switch_to_delay_sec;

    const char* name;
    char* description;

    bool armed;
    bool protect;
    bool disarmed_default;

    cJSON* cj_notifications;
    cJSON* cj_bypass_devices;
    cJSON* cj_disarmed_devices;
    cJSON* cj_alarms_off_devices;
    cJSON* cj_cameras_off_devices;

} s_house_modes_t;

typedef struct s_entry_delay
{
    uint32_t short_delay_sec;
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
    struct s_protect_buttons* next;

} s_protect_buttons_t;

typedef struct s_sources
{
    uint32_t delay;
    uint32_t device_id;
    long long time_stamp;
    struct s_sources* next;

} s_sources_t;

typedef struct s_alarmed
{
    char type[32]; // default is 'global'
    uint32_t entry_delay_sec;
    uint32_t time_is_left_sec;
    s_sources_t* sources;

} s_alarmed_t;

typedef struct s_ezlopi_modes
{
    uint32_t current_mode_id;
    uint32_t switch_to_mode_id;

    uint32_t alarm_delay_sec;
    uint32_t switch_to_delay_sec;
    uint32_t time_is_left_to_switch_sec;

    cJSON* cj_alarms;
    cJSON* cj_cameras;
    cJSON* cj_devices;
    s_protect_buttons_t* l_protect_buttons;

    s_entry_delay_t entry_delay;
    s_abort_window_t abort_delay;

    s_alarmed_t alarmed;

    s_house_modes_t mode_home;
    s_house_modes_t mode_away;
    s_house_modes_t mode_night;
    s_house_modes_t mode_vacation;

} s_ezlopi_modes_t;

void ezlopi_core_modes_init(void);
void ezlopi_core_default_init(void);
int ezlopi_core_modes_store_to_nvs(void);
s_ezlopi_modes_t* ezlopi_core_default_mode_get(void);
s_house_modes_t* ezlopi_core_modes_get_house_mode_by_id(uint32_t house_mode_id);
s_house_modes_t* ezlopi_core_modes_get_house_mode_by_name(char* house_mode_name);

s_ezlopi_modes_t* ezlopi_core_modes_get_custom_modes(void);
s_house_modes_t* ezlopi_core_modes_get_current_house_modes(void);
int ezlopi_core_modes_set_current_house_mode(s_house_modes_t* new_house_mode);

int ezlopi_core_modes_api_get_modes(cJSON* cj_result);
int ezlopi_core_modes_api_get_current_mode(cJSON* cj_result);
int ezlopi_core_modes_api_switch_mode(s_house_modes_t* switch_to_house_mode);
int ezlopi_core_modes_api_cancel_switch(void);
int ezlopi_core_modes_api_cancel_entry_delay(void);
int ezlopi_core_modes_set_switch_to_delay(uint32_t switch_to_delay);
int ezlopi_core_modes_set_alarm_delay(uint32_t switch_to_delay);
int ezlopi_core_modes_set_protect(uint32_t mode_id, bool protect_state);

int ezlopi_core_modes_add_alarm_off(uint8_t mode_id, cJSON*  device_id);
int ezlopi_core_modes_remove_alarm_off(uint32_t mode_id, cJSON* device_id);

int ezlopi_core_modes_cjson_get_current_mode(cJSON* cj_result);
int ezlopi_core_modes_set_entry_delay(uint32_t normal_sec, uint32_t short_sec, uint32_t extended_sec, uint32_t instant_sec);
int ezlopi_core_modes_reset_entry_delay(void);

///
cJSON* ezlopi_core_modes_cjson_changed(void);

#endif // CONFIG_EZPI_SERV_ENABLE_MODES


#endif // __EZLOPI_HOME_MODES_H__