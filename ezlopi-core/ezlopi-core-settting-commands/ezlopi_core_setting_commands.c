
#include "ezlopi_util_trace.h"
#include "cjext.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_setting_commands.h"

const char *ezlopi_core_setting_command_names[] = {
    "scale.temperature",
};

const char *temperature_unit_enum[2] = {
    "fahrenheit", // Default tempeature scale
    "celsius",
};

static e_enum_temperature_scale_t temperature_scale_to_user = TEMPERATURE_SCALE_FAHRENHEIT;

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
    e_enum_temperature_scale_t val = TEMPERATURE_SCALE_MAX;
    if (cj_params)
    {
        cJSON *cj_valtype = cJSON_GetObjectItem(__FUNCTION__, cj_params, "valueType");
        if (cj_valtype && cJSON_IsString(cj_valtype))
        {
            if(0 == strncmp(cj_valtype->valuestring, "token", 6))
            {
                cJSON* cj_value = cJSON_GetObjectItem(__FUNCTION__, cj_params, "value");
                if(cj_value && cJSON_IsString(cj_value))
                {
                    for(e_enum_temperature_scale_t i = 0; i < TEMPERATURE_SCALE_MAX; i++)
                    {
                        if(0 == strncmp(temperature_unit_enum[i], cj_value->valuestring, strlen(temperature_unit_enum[i])))
                        {
                            int err = EZPI_CORE_nvs_write_temperature_scale((uint32_t)i);
                            ret = 0;
                            break;
                        }
                    }
                }
            }
        }
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
            default:
            {
                break;
            }
            }
        }
    }

    return ret;
}
