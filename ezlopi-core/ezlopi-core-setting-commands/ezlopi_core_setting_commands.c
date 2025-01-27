/* ===========================================================================
** Copyright (C) 2025 Ezlo Innovation Inc
**
** Under EZLO AVAILABLE SOURCE LICENSE (EASL) AGREEMENT
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/
/**
 * @file     ezlopi_core_setting_commands.c
 * @brief    perform some function on setting cmds
 * @author
 * @version  1.0
 * @date     12th DEC 2024
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

#include "ezlopi_util_trace.h"
#include "cjext.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_log.h"
#include "ezlopi_core_broadcast.h"
#include "ezlopi_core_setting_commands.h"

#include "ezlopi_cloud_constants.h"

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
static int __setting_cmd_process_scale_temperature(const cJSON *cj_params);
static int __setting_cmd_process_date_format(const cJSON *cj_params);
static int __setting_cmd_process_time_format(const cJSON *cj_params);
static int __setting_cmd_process_netork_ping_timeout(const cJSON *cj_params);
static int __setting_cmd_process_log_level(const cJSON *cj_params);
static int __add_temperature_scale_settings(cJSON *cj_settings);
static int __add_date_format_settings(cJSON *cj_settings);
static int __add_time_format_settings(cJSON *cj_settings);
static int __add_network_ping_timeout_settings(cJSON *cj_settings);
static int __add_log_level_settings(cJSON *cj_settings);

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/
static e_enum_temperature_scale_t temperature_scale_to_user = TEMPERATURE_SCALE_CELSIUS;
static e_enum_date_format_t date_format_to_user = DATE_FORMAT_MMDDYY;
static e_enum_time_format_t time_format_to_user = TIME_FORMAT_12;
static int network_ping_timeout_to_user = 10;

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/
const char *ezlopi_core_setting_command_names[SETTING_COMMAND_NAME_MAX] = {
    "scale.temperature",
    "date.format",
    "time.format",
    "network.ping_timeout",
    "logs.level",
};

const char *temperature_scale_enum[TEMPERATURE_SCALE_MAX] = {
    "fahrenheit", // Default tempeature scale
    "celsius",
};

const char *date_format_enum[DATE_FORMAT_MAX] = {
    "mmddyy",
    "ddmmyy",
};

const char *time_format_enum[TIME_FORMAT_MAX] = {
    "12",
    "24",
};

const char *log_indentation_level[4] = {
    "-1",
    "2",
    "4",
    "8",
};

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/

int EZPI_core_setting_updated_broadcast(cJSON *cj_params, cJSON *cj_result)
{
    int ret = -1;
    if (cj_params && cj_result)
    {
        cJSON *cj_name = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_name_str);
        if (cj_name && cJSON_IsString(cj_name))
        {
            cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_name_str, cj_name->valuestring);
            e_ezlopi_core_setting_command_names_t e_name = EZPI_core_setting_commands_get_cmd_enum_from_str(cj_name->valuestring);
            switch (e_name)
            {
            case SETTING_COMMAND_NAME_SCALE_TEMPERATURE:
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_value_str, temperature_scale_enum[temperature_scale_to_user]);
                break;
            }
            case SETTING_COMMAND_NAME_DATE_FORMAT:
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_value_str, date_format_enum[date_format_to_user]);
                break;
            }
            case SETTING_COMMAND_NAME_TIME_FORMAT:
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_value_str, time_format_enum[time_format_to_user]);
                break;
            }
            case SETTING_COMMAND_NAME_NETWORK_PING_TIMEOUT:
            {
                cJSON_AddNumberToObject(__FUNCTION__, cj_result, ezlopi_value_str, network_ping_timeout_to_user);
                break;
            }
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
            case SETTING_COMMAND_NAME_LOG_LEVEL:
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_value_str, EZPI_core_cloud_log_get_current_severity_enum_str());
                break;
            }
#endif
            default:
            {
                break;
            }
            }
            ret = 0;
        }
    }
    return ret;
}

int EZPI_core_setting_commands_process(cJSON *cj_params)
{
    int ret = -1;
    if (cj_params)
    {
        cJSON *cj_name = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_name_str);
        if (cj_name && cJSON_IsString(cj_name))
        {
            e_ezlopi_core_setting_command_names_t e_name = EZPI_core_setting_commands_get_cmd_enum_from_str(cj_name->valuestring);
            switch (e_name)
            {
            case SETTING_COMMAND_NAME_SCALE_TEMPERATURE:
            {
                ret = __setting_cmd_process_scale_temperature(cj_params);
                break;
            }
            case SETTING_COMMAND_NAME_DATE_FORMAT:
            {
                ret = __setting_cmd_process_date_format(cj_params);
                break;
            }
            case SETTING_COMMAND_NAME_TIME_FORMAT:
            {
                ret = __setting_cmd_process_time_format(cj_params);
                break;
            }
            case SETTING_COMMAND_NAME_NETWORK_PING_TIMEOUT:
            {
                ret = __setting_cmd_process_netork_ping_timeout(cj_params);
                break;
            }
            case SETTING_COMMAND_NAME_LOG_LEVEL:
            {
                ret = __setting_cmd_process_log_level(cj_params);
                break;
            }
            default:
            {
                break;
            }
            }
        }
    }

    return ret;
}

int EZPI_core_setting_commands_populate_settings(cJSON *cj_result)
{
    int ret = -1;
    if (cj_result)
    {
        cJSON *cj_settings = cJSON_AddArrayToObject(__FUNCTION__, cj_result, ezlopi_settings_str);
        if (cj_settings)
        {
            __add_date_format_settings(cj_settings);
            __add_time_format_settings(cj_settings);
            __add_network_ping_timeout_settings(cj_settings);
            __add_temperature_scale_settings(cj_settings);
            __add_log_level_settings(cj_settings);
        }
    }
    return ret;
}

int EZPI_core_setting_commands_read_settings()
{
    int ret = 0;

    EZPI_core_nvs_read_temperature_scale((uint32_t *)&temperature_scale_to_user);
    TRACE_I("Temperature scale: %s", temperature_scale_enum[temperature_scale_to_user]);

    EZPI_core_nvs_read_date_format((uint32_t *)&date_format_to_user);
    TRACE_I("Date format: %s", date_format_enum[date_format_to_user]);

    EZPI_core_nvs_read_time_format((uint32_t *)&time_format_to_user);
    TRACE_I("Time format: %s", time_format_enum[time_format_to_user]);

    EZPI_core_nvs_read_network_ping_timeout((uint32_t *)&network_ping_timeout_to_user);
    TRACE_I("Network Ping Timeout: %d", network_ping_timeout_to_user);

#ifdef CONFIG_EZPI_UTIL_TRACE_EN
    EZPI_core_read_set_log_severities();
    // #warning "remove this in release"
    EZPI_core_read_set_log_severities_internal(ENUM_EZLOPI_TRACE_SEVERITY_TRACE);
    // printf("Log severity/level set to: %s\n", EZPI_core_cloud_log_get_current_severity_enum_str());
#endif // CONFIG_EZPI_UTIL_TRACE_EN

    return ret;
}

e_enum_temperature_scale_t EZPI_core_setting_get_temperature_scale()
{
    return temperature_scale_to_user;
}

e_enum_date_format_t EZPI_core_setting_get_date_format()
{
    return date_format_to_user;
}

e_enum_time_format_t EZPI_core_setting_get_time_format()
{
    return time_format_to_user;
}

int EZPI_core_setting_get_network_ping_timeout()
{
    return network_ping_timeout_to_user;
}

e_ezlopi_core_setting_command_names_t EZPI_core_setting_commands_get_cmd_enum_from_str(const char *name)
{
    e_ezlopi_core_setting_command_names_t ret = SETTING_COMMAND_NAME_MAX;
    if (name)
    {
        for (e_ezlopi_core_setting_command_names_t i = 0; i < SETTING_COMMAND_NAME_MAX; i++)
        {
            if (0 == strncmp(ezlopi_core_setting_command_names[i], name, strlen(ezlopi_core_setting_command_names[i])))
            {
                ret = i;
            }
        }
    }
    return ret;
}

const char *EZPI_core_setting_get_temperature_scale_str()
{
    return temperature_scale_enum[temperature_scale_to_user];
}
const char *EZPI_core_setting_get_date_format_str()
{
    return date_format_enum[date_format_to_user];
}
const char *EZPI_core_setting_get_time_format_str()
{
    return time_format_enum[time_format_to_user];
}

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/

static int __setting_cmd_process_scale_temperature(const cJSON *cj_params)
{
    int ret = -1;
    if (cj_params)
    {
        cJSON *cj_value = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_value_str);
        if (cj_value && cJSON_IsString(cj_value))
        {
            for (e_enum_temperature_scale_t i = 0; i < TEMPERATURE_SCALE_MAX; i++)
            {
                if (0 == strncmp(temperature_scale_enum[i], cj_value->valuestring, strlen(temperature_scale_enum[i])))
                {
                    EZPI_core_nvs_write_temperature_scale((uint32_t)i);
                    temperature_scale_to_user = i;
                    ret = 0;
                    break;
                }
            }
        }
    }
    return ret;
}

static int __setting_cmd_process_date_format(const cJSON *cj_params)
{
    int ret = -1;
    if (cj_params)
    {
        cJSON *cj_value = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_value_str);
        if (cj_value && cJSON_IsString(cj_value))
        {
            for (e_enum_date_format_t i = 0; i < DATE_FORMAT_MAX; i++)
            {
                if (0 == strncmp(date_format_enum[i], cj_value->valuestring, strlen(date_format_enum[i])))
                {
                    EZPI_core_nvs_write_date_format((uint32_t)i);
                    date_format_to_user = i;
                    ret = 0;
                    break;
                }
            }
        }
    }
    return ret;
}

static int __setting_cmd_process_time_format(const cJSON *cj_params)
{
    int ret = -1;
    if (cj_params)
    {
        cJSON *cj_value = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_value_str);
        if (cj_value && cJSON_IsString(cj_value))
        {
            for (e_enum_time_format_t i = 0; i < TIME_FORMAT_MAX; i++)
            {
                if (0 == strncmp(time_format_enum[i], cj_value->valuestring, strlen(time_format_enum[i])))
                {
                    EZPI_core_nvs_write_time_format((uint32_t)i);
                    time_format_to_user = i;
                    ret = 0;
                    break;
                }
            }
        }
    }
    return ret;
}

static int __setting_cmd_process_netork_ping_timeout(const cJSON *cj_params)
{
    int ret = 0;
    if (cj_params)
    {
        cJSON *cj_value = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_value_str);
        if (cj_value && cJSON_IsNumber(cj_value))
        {
            EZPI_core_nvs_write_network_ping_timeout((uint32_t)cj_value->valuedouble);
            network_ping_timeout_to_user = (int)cj_value->valuedouble;
            ret = 0;
        }
    }
    return ret;
}

static int __setting_cmd_process_log_level(const cJSON *cj_params)
{
    int ret = -1;
    if (cj_params)
    {
        cJSON *cj_value = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_value_str);
        if (cj_value && cJSON_IsString(cj_value))
        {
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
            EZPI_core_cloud_log_severity_process_str(true, cj_value->valuestring);
#endif
            ret = 0;
        }
    }
    return ret;
}

static int __add_temperature_scale_settings(cJSON *cj_settings)
{
    int ret = -1;

    cJSON *cj_temp = cJSON_CreateObject(__FUNCTION__);
    if (cj_temp)
    {
        cJSON *cj_enum = cJSON_AddArrayToObject(__FUNCTION__, cj_temp, "enum");
        if (cj_enum)
        {
            for (e_enum_temperature_scale_t i = 0; i < TEMPERATURE_SCALE_MAX; i++)
            {
                cJSON_AddItemToArray(cj_enum, cJSON_CreateString(__FUNCTION__, temperature_scale_enum[i]));
            }
        }
        cJSON_AddStringToObject(__FUNCTION__, cj_temp, ezlopi_name_str, ezlopi_core_setting_command_names[SETTING_COMMAND_NAME_SCALE_TEMPERATURE]);
        cJSON_AddStringToObject(__FUNCTION__, cj_temp, ezlopi_value_str, temperature_scale_enum[temperature_scale_to_user]);
        cJSON_AddStringToObject(__FUNCTION__, cj_temp, ezlopi_valueType_str, "token");

        cJSON_AddItemToArray(cj_settings, cj_temp);
    }

    return ret;
}

static int __add_date_format_settings(cJSON *cj_settings)
{
    int ret = -1;

    cJSON *cj_date_fmt = cJSON_CreateObject(__FUNCTION__);
    if (cj_date_fmt)
    {
        cJSON *cj_enum = cJSON_AddArrayToObject(__FUNCTION__, cj_date_fmt, "enum");
        if (cj_enum)
        {
            for (e_enum_date_format_t i = 0; i < DATE_FORMAT_MAX; i++)
            {
                cJSON_AddItemToArray(cj_enum, cJSON_CreateString(__FUNCTION__, date_format_enum[i]));
            }
        }
        cJSON_AddStringToObject(__FUNCTION__, cj_date_fmt, ezlopi_name_str, ezlopi_core_setting_command_names[SETTING_COMMAND_NAME_DATE_FORMAT]);
        cJSON_AddStringToObject(__FUNCTION__, cj_date_fmt, ezlopi_value_str, date_format_enum[date_format_to_user]);
        cJSON_AddStringToObject(__FUNCTION__, cj_date_fmt, ezlopi_valueType_str, "token");

        cJSON_AddItemToArray(cj_settings, cj_date_fmt);
    }

    return ret;
}

static int __add_time_format_settings(cJSON *cj_settings)
{
    int ret = -1;

    cJSON *cj_time_fmt = cJSON_CreateObject(__FUNCTION__);
    if (cj_time_fmt)
    {
        cJSON *cj_enum = cJSON_AddArrayToObject(__FUNCTION__, cj_time_fmt, "enum");
        if (cj_enum)
        {
            for (e_enum_time_format_t i = 0; i < TIME_FORMAT_MAX; i++)
            {
                cJSON_AddItemToArray(cj_enum, cJSON_CreateString(__FUNCTION__, time_format_enum[i]));
            }
        }
        cJSON_AddStringToObject(__FUNCTION__, cj_time_fmt, ezlopi_name_str, ezlopi_core_setting_command_names[SETTING_COMMAND_NAME_TIME_FORMAT]);
        cJSON_AddStringToObject(__FUNCTION__, cj_time_fmt, ezlopi_value_str, time_format_enum[time_format_to_user]);
        cJSON_AddStringToObject(__FUNCTION__, cj_time_fmt, ezlopi_valueType_str, "token");

        cJSON_AddItemToArray(cj_settings, cj_time_fmt);
    }

    return ret;
}

static int __add_network_ping_timeout_settings(cJSON *cj_settings)
{
    int ret = -1;

    cJSON *cj_network_ping_timeout = cJSON_CreateObject(__FUNCTION__);
    if (cj_network_ping_timeout)
    {
        cJSON_AddStringToObject(__FUNCTION__, cj_network_ping_timeout, ezlopi_name_str, ezlopi_core_setting_command_names[SETTING_COMMAND_NAME_NETWORK_PING_TIMEOUT]);
        cJSON_AddNumberToObject(__FUNCTION__, cj_network_ping_timeout, ezlopi_value_str, (const double)network_ping_timeout_to_user);
        cJSON_AddStringToObject(__FUNCTION__, cj_network_ping_timeout, ezlopi_valueType_str, "int");

        cJSON_AddItemToArray(cj_settings, cj_network_ping_timeout);
    }

    return ret;
}

static int __add_log_level_settings(cJSON *cj_settings)
{
    int ret = -1;
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
    const char **log_level_enums = EZPI_core_cloud_log_get_severity_enums();
    if (log_level_enums)
    {
        cJSON *cj_log_level = cJSON_CreateObject(__FUNCTION__);
        if (cj_log_level)
        {
            cJSON *cj_enum = cJSON_AddArrayToObject(__FUNCTION__, cj_log_level, "enum");
            if (cj_enum)
            {
                for (e_trace_severity_t i = 0; i < ENUM_EZLOPI_TRACE_SEVERITY_MAX; i++)
                {
                    cJSON_AddItemToArray(cj_enum, cJSON_CreateString(__FUNCTION__, log_level_enums[i]));
                }
                cJSON_DeleteItemFromArray(__FUNCTION__, cj_enum, 0);
            }
            cJSON_AddStringToObject(__FUNCTION__, cj_log_level, ezlopi_name_str, ezlopi_core_setting_command_names[SETTING_COMMAND_NAME_LOG_LEVEL]);
            const char *current_log_level = EZPI_core_cloud_log_get_current_severity_enum_str();
            if (current_log_level)
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_log_level, ezlopi_value_str, current_log_level);
            }
            cJSON_AddStringToObject(__FUNCTION__, cj_log_level, ezlopi_valueType_str, "token");

            cJSON_AddItemToArray(cj_settings, cj_log_level);
        }
    }
#endif
    return ret;
}

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
