
#include "ezlopi_util_trace.h"
#include "cjext.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_setting_commands.h"

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

static e_enum_temperature_scale_t temperature_scale_to_user = TEMPERATURE_SCALE_FAHRENHEIT;
static e_enum_date_format_t date_format_to_user = DATE_FORMAT_MMDDYY;
static e_enum_time_format_t time_format_to_user = TIME_FORMAT_12;
static int network_ping_timeout_to_user = 10;

static e_ezlopi_core_setting_command_names_t ezlopi_core_setting_command_get_name(const char *name)
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

static int ezlopi_core_setting_command_process_scale_temperature(const cJSON *cj_params)
{
    int ret = -1;
    if (cj_params)
    {
        cJSON *cj_value = cJSON_GetObjectItem(__FUNCTION__, cj_params, "value");
        if (cj_value && cJSON_IsString(cj_value))
        {
            for (e_enum_temperature_scale_t i = 0; i < TEMPERATURE_SCALE_MAX; i++)
            {
                if (0 == strncmp(temperature_scale_enum[i], cj_value->valuestring, strlen(temperature_scale_enum[i])))
                {
                    EZPI_CORE_nvs_write_temperature_scale((uint32_t)i);
                    temperature_scale_to_user = i;
                    ret = 0;
                    break;
                }
            }
        }
    }
    return ret;
}

static int ezlopi_core_setting_command_process_date_format(const cJSON *cj_params)
{
    int ret = -1;
    if (cj_params)
    {
        cJSON *cj_value = cJSON_GetObjectItem(__FUNCTION__, cj_params, "value");
        if (cj_value && cJSON_IsString(cj_value))
        {
            for (e_enum_date_format_t i = 0; i < DATE_FORMAT_MAX; i++)
            {
                if (0 == strncmp(date_format_enum[i], cj_value->valuestring, strlen(date_format_enum[i])))
                {
                    EZPI_CORE_nvs_write_date_format((uint32_t)i);
                    date_format_to_user = i;
                    ret = 0;
                    break;
                }
            }
        }
    }
    return ret;
}

static int ezlopi_core_setting_command_process_time_format(const cJSON *cj_params)
{
    int ret = -1;
    if (cj_params)
    {
        cJSON *cj_value = cJSON_GetObjectItem(__FUNCTION__, cj_params, "value");
        if (cj_value && cJSON_IsString(cj_value))
        {
            for (e_enum_time_format_t i = 0; i < TIME_FORMAT_MAX; i++)
            {
                if (0 == strncmp(time_format_enum[i], cj_value->valuestring, strlen(time_format_enum[i])))
                {
                    EZPI_CORE_nvs_write_time_format((uint32_t)i);
                    time_format_to_user = i;
                    ret = 0;
                    break;
                }
            }
        }
    }
    return ret;
}

static int ezlopi_core_setting_command_process_netork_ping_timeout(const cJSON *cj_params)
{
    int ret = 0;
    if (cj_params)
    {
        cJSON *cj_value = cJSON_GetObjectItem(__FUNCTION__, cj_params, "value");
        if (cj_value && cJSON_IsNumber(cj_value))
        {
            EZPI_CORE_nvs_write_network_ping_timeout((uint32_t)cj_value->valuedouble);
            network_ping_timeout_to_user = (int)cj_value->valuedouble;
            ret = 0;
        }
    }
    return ret;
}

static int ezlopi_core_add_temperature_scale_settings(cJSON *cj_settings)
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
        cJSON_AddStringToObject(__FUNCTION__, cj_temp, "name", ezlopi_core_setting_command_names[SETTING_COMMAND_NAME_SCALE_TEMPERATURE]);
        cJSON_AddStringToObject(__FUNCTION__, cj_temp, "value", temperature_scale_enum[temperature_scale_to_user]);
        cJSON_AddStringToObject(__FUNCTION__, cj_temp, "valueType", "token");

        cJSON_AddItemToArray(cj_settings, cj_temp);
    }

    return ret;
}

static int ezlopi_core_add_date_format_settings(cJSON *cj_settings)
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
        cJSON_AddStringToObject(__FUNCTION__, cj_date_fmt, "name", ezlopi_core_setting_command_names[SETTING_COMMAND_NAME_DATE_FORMAT]);
        cJSON_AddStringToObject(__FUNCTION__, cj_date_fmt, "value", date_format_enum[date_format_to_user]);
        cJSON_AddStringToObject(__FUNCTION__, cj_date_fmt, "valueType", "token");

        cJSON_AddItemToArray(cj_settings, cj_date_fmt);
    }

    return ret;
}

static int ezlopi_core_add_time_format_settings(cJSON *cj_settings)
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
        cJSON_AddStringToObject(__FUNCTION__, cj_time_fmt, "name", ezlopi_core_setting_command_names[SETTING_COMMAND_NAME_TIME_FORMAT]);
        cJSON_AddStringToObject(__FUNCTION__, cj_time_fmt, "value", time_format_enum[time_format_to_user]);
        cJSON_AddStringToObject(__FUNCTION__, cj_time_fmt, "valueType", "token");

        cJSON_AddItemToArray(cj_settings, cj_time_fmt);
    }

    return ret;
}

static int ezlopi_core_add_network_ping_timeout_settings(cJSON *cj_settings)
{
    int ret = -1;

    cJSON *cj_network_ping_timeout = cJSON_CreateObject(__FUNCTION__);
    if (cj_network_ping_timeout)
    {
        cJSON_AddStringToObject(__FUNCTION__, cj_network_ping_timeout, "name", ezlopi_core_setting_command_names[SETTING_COMMAND_NAME_NETWORK_PING_TIMEOUT]);
        cJSON_AddNumberToObject(__FUNCTION__, cj_network_ping_timeout, "value", (const double)network_ping_timeout_to_user);
        cJSON_AddStringToObject(__FUNCTION__, cj_network_ping_timeout, "valueType", "int");

        cJSON_AddItemToArray(cj_settings, cj_network_ping_timeout);
    }

    return ret;
}

int ezlopi_core_setting_commands_process(cJSON *cj_params)
{
    int ret = -1;
    if (cj_params)
    {
        cJSON *cj_name = cJSON_GetObjectItem(__FUNCTION__, cj_params, "name");
        if (cj_name && cJSON_IsString(cj_name))
        {
            e_ezlopi_core_setting_command_names_t e_name = ezlopi_core_setting_command_get_name(cj_name->valuestring);
            switch (e_name)
            {
            case SETTING_COMMAND_NAME_SCALE_TEMPERATURE:
            {
                ezlopi_core_setting_command_process_scale_temperature(cj_params);
                break;
            }
            case SETTING_COMMAND_NAME_DATE_FORMAT:
            {
                ezlopi_core_setting_command_process_date_format(cj_params);
                break;
            }
            case SETTING_COMMAND_NAME_TIME_FORMAT:
            {
                ezlopi_core_setting_command_process_time_format(cj_params);
                break;
            }
            case SETTING_COMMAND_NAME_NETWORK_PING_TIMEOUT:
            {
                ezlopi_core_setting_command_process_netork_ping_timeout(cj_params);
                break;
            }
            case SETTING_COMMAND_NAME_LOG_LEVEL:
            {
                printf("Log level set\n");
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

int ezlopi_core_setting_commands_populate_settings(cJSON *cj_result)
{
    int ret = -1;
    if (cj_result)
    {
        cJSON *cj_settings = cJSON_AddArrayToObject(__FUNCTION__, cj_result, "settings");
        if (cj_settings)
        {
            ezlopi_core_add_temperature_scale_settings(cj_settings);
            ezlopi_core_add_date_format_settings(cj_settings);
            ezlopi_core_add_time_format_settings(cj_settings);
            ezlopi_core_add_network_ping_timeout_settings(cj_settings);
        }
    }
    return ret;
}

int ezlopi_core_setting_commands_read_settings()
{
    int ret = 0;

    EZPI_CORE_nvs_read_temperature_scale((uint32_t *)&temperature_scale_to_user);
    printf("Temperature scale: %s\n", temperature_scale_enum[temperature_scale_to_user]);

    EZPI_CORE_nvs_read_date_format((uint32_t *)&date_format_to_user);
    printf("Date format: %s\n", date_format_enum[date_format_to_user]);

    EZPI_CORE_nvs_read_time_format((uint32_t *)&time_format_to_user);
    printf("Time format: %s\n", time_format_enum[time_format_to_user]);

    EZPI_CORE_nvs_read_network_ping_timeout((uint32_t *)&network_ping_timeout_to_user);
    printf("Network Ping Timeout: %d\n", network_ping_timeout_to_user);

    return ret;
}

uint8_t ezlopi_core_setting_get_temperature_scale()
{
    return (uint8_t)temperature_scale_to_user;
}
