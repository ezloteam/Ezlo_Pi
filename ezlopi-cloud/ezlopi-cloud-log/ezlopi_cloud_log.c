#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_UTIL_TRACE_EN

#include <stdbool.h>
#include <string.h>

#include "ezlopi_core_log.h"
#include "ezlopi_core_sntp.h"

#include "ezlopi_cloud_constants.h"

#include "ezlopi_cloud_log.h"


void ezlopi_hub_cloud_log_set(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON* cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON* cj_log_enable = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_enable_str);
            cJSON* cj_log_severity = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_severity_str);
            if (cj_log_enable && cj_log_severity)
            {
                bool severity_enable = cJSON_IsTrue(cj_log_enable);
                const char* log_severity_type = cj_log_severity->type == cJSON_String ? cj_log_severity->valuestring : NULL;
                ezlopi_core_cloud_log_severity_process(severity_enable, log_severity_type);
            }
        }
    }
}

void ezlopi_hub_serial_log_set(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON* cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON* cj_log_severity = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_severity_str);
            if (cj_log_severity)
            {
                const char* log_severity_type = cj_log_severity->type == cJSON_String ? cj_log_severity->valuestring : NULL;
                ezlopi_core_serial_log_severity_process(log_severity_type);
            }
        }
    }
}

void ezlopi_hub_cloud_log_set_updater(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        uint64_t timestamp = EZPI_CORE_sntp_get_current_time_ms();
        const char* messsage_str = "Log severity set";
        size_t total_len = 15 + strlen(messsage_str) + 2;
        char message[total_len];
        memset(message, 0, total_len);
        snprintf(message, total_len, "%lld: %s", timestamp, messsage_str);

        cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_message_str, message);

        const char* severity_str = ezlopi_core_cloud_log_get_current_severity_enum_str();
        cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_severity_str, severity_str);
    }
}

void ezlopi_hub_serial_log_set_updater(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON* cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON* cj_log_severity = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_severity_str);
            if (cj_log_severity)
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_name_str, "log.level");
                const char* severity_str = ezlopi_core_serial_log_get_current_severity_enum_str();
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_value_str, severity_str);
            }
        }
    }
}

#endif  // CONFIG_EZPI_UTIL_TRACE_EN  