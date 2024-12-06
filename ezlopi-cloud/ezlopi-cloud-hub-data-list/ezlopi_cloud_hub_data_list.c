
#include "cjext.h"
#include "ezlopi_util_trace.h"

#include "ezlopi_core_setting_commands.h"

#include "ezlopi_cloud_constants.h"
#include "ezlopi_cloud_hub_data_list.h"

static void ezlopi_core_hub_data_list_populate_settings_json(cJSON *cj_result_name, const char *field_str, const char *name_str)
{
    e_ezlopi_core_setting_command_names_t settings_name_enum = ezlopi_core_setting_command_get_command_enum_from_str(name_str);
    switch (settings_name_enum)
    {
    case SETTING_COMMAND_NAME_SCALE_TEMPERATURE:
    {
        char *scale_str = (ezlopi_core_setting_get_temperature_scale() == TEMPERATURE_SCALE_FAHRENHEIT) ? "fahrenheit" : "celsius";
        cJSON_AddStringToObject(__FUNCTION__, cj_result_name, field_str, scale_str);
        break;
    }
    case SETTING_COMMAND_NAME_DATE_FORMAT:
    {
        char *date_format = (ezlopi_core_setting_get_date_format() == DATE_FORMAT_MMDDYY) ? "mmddyy" : "ddmmyy";
        cJSON_AddStringToObject(__FUNCTION__, cj_result_name, field_str, date_format);
        break;
    }
    case SETTING_COMMAND_NAME_TIME_FORMAT:
    {
        char *time_format = (ezlopi_core_setting_get_time_format() == TIME_FORMAT_12) ? "12" : "24";
        cJSON_AddStringToObject(__FUNCTION__, cj_result_name, field_str, time_format);
        break;
    }
    default:
    {
        break;
    }
    }
}

static void ezlopi_core_hub_data_list_process_settings_data_list(cJSON *cj_names, cJSON *cj_include, cJSON *cj_settings)
{
    cJSON *cj_include_element = NULL;
    cJSON *cj_names_element = NULL;
    cJSON_ArrayForEach(cj_names_element, cj_names)
    {
        cJSON *cj_result_names = cJSON_AddObjectToObject(__FUNCTION__, cj_settings, cj_names_element->valuestring);
        if (cj_result_names)
        {
            cJSON_ArrayForEach(cj_include_element, cj_include)
            {
                if (0 == strncmp(cj_include_element->valuestring, ezlopi_value_str, 6))
                {
                    ezlopi_core_hub_data_list_populate_settings_json(cj_result_names, cj_include_element->valuestring, cj_names_element->valuestring);
                }
            }
        }
    }
}

void hub_data_list(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON *cj_setting_data_list = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_settings_str);
            if (cj_setting_data_list)
            {
                cJSON *cj_settings = cJSON_AddObjectToObject(__FUNCTION__, cj_result, ezlopi_settings_str);
                cJSON *cj_names_array = cJSON_GetObjectItem(__FUNCTION__, cj_setting_data_list, "names");
                if (cj_names_array && cJSON_IsArray(cj_names_array) && cj_settings)
                {
                    cJSON *cj_fileds = cJSON_GetObjectItem(__FUNCTION__, cj_setting_data_list, "fields");
                    if (cj_fileds)
                    {
                        cJSON *cj_include = cJSON_GetObjectItem(__FUNCTION__, cj_fileds, "include");
                        if (cj_include && cJSON_IsArray(cj_include))
                        {
                            ezlopi_core_hub_data_list_process_settings_data_list(cj_names_array, cj_include, cj_settings);
                        }
                    }
                }
            }
        }
    }
}
