
#ifndef __EZLOPI_CORE_SETTING_COMMANDS_H__
#define __EZLOPI_CORE_SETTING_COMMANDS_H__

#include "cjext.h"

typedef enum
{
    SETTING_COMMAND_NAME_SCALE_TEMPERATURE = 0,
    SETTING_COMMAND_NAME_DATE_FORMAT,
    SETTING_COMMAND_NAME_TIME_FORMAT,
    SETTING_COMMAND_NAME_NETWORK_PING_TIMEOUT,
    SETTING_COMMAND_NAME_LOG_LEVEL,
    SETTING_COMMAND_NAME_MAX,
} e_ezlopi_core_setting_command_names_t;

typedef enum
{
    TEMPERATURE_SCALE_FAHRENHEIT = 0,
    TEMPERATURE_SCALE_CELSIUS,
    TEMPERATURE_SCALE_MAX,
} e_enum_temperature_scale_t;

typedef enum
{
    DATE_FORMAT_MMDDYY = 0,
    DATE_FORMAT_DDMMYY,
    DATE_FORMAT_MAX,
} e_enum_date_format_t;

typedef enum
{
    TIME_FORMAT_12 = 0,
    TIME_FORMAT_24,
    TIME_FORMAT_MAX,
} e_enum_time_format_t;

int ezlopi_core_setting_commands_process(cJSON *cj_params);
int ezlopi_core_setting_commands_populate_settings(cJSON *cj_result);
int ezlopi_core_setting_commands_read_settings();
e_enum_temperature_scale_t ezlopi_core_setting_get_temperature_scale();
e_enum_date_format_t ezlopi_core_setting_get_date_format();
e_enum_time_format_t ezlopi_core_setting_get_time_format();
int ezlopi_core_setting_get_network_ping_timeout();


#endif /// __EZLOPI_CORE_SETTING_COMMANDS_H__
