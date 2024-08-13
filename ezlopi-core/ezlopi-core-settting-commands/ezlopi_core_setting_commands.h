
#ifndef __EZLOPI_CORE_SETTING_COMMANDS_H__
#define __EZLOPI_CORE_SETTING_COMMANDS_H__

#include "cjext.h"

typedef enum
{
    SETTING_COMMAND_VALUE_TYPE_TOKEN = 0,
    SETTING_COMMAND_VALUE_TYPE_INT,
    SETTING_COMMAND_VALUE_TYPE_MAX,
} e_ezlopi_core_setting_commands_valuetype_enum_t;

typedef enum
{
    SETTING_COMMAND_NAME_SCALE_TEMPERATURE = 0,
    SETTING_COMMAND_NAME_MAX,
} e_ezlopi_core_setting_command_names_t;

typedef enum
{
    TEMPERATURE_SCALE_FAHRENHEIT = 0,
    TEMPERATURE_SCALE_CELSIUS,
    TEMPERATURE_SCALE_MAX,
} e_enum_temperature_scale_t;

int ezlopi_core_setting_commands_process(cJSON *cj_params);

#endif /// __EZLOPI_CORE_SETTING_COMMANDS_H__
