
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "cjext.h"
#include "ezlopi_util_heap.h"

#include "ezlopi_core_sntp.h"
#include "ezlopi_core_ezlopi_broadcast.h"
#include "ezlopi_core_nvs.h"
#include "ezlopi_core_event_group.h"

#include "ezlopi_cloud_constants.h"

#include "ezlopi_core_log.h"

const char* log_severity_enum[ENUM_EZLOPI_LOG_SEVERITY_MAX] = {
    "ERROR",
    "WARNING",
    "INFO",
    "DEBUG",
    "TRACE"
};

static e_ezlopi_log_severity_t log_severity = ENUM_EZLOPI_LOG_SEVERITY_INFO;

static int ezlopi_hub_log_set_severity(const char* severity_str)
{
    int ret = 0;
    if (severity_str)
    {
        for (int i = 0; i < ENUM_EZLOPI_LOG_SEVERITY_MAX; i++)
        {
            if (0 == strncmp(log_severity_enum[i], severity_str, strlen(log_severity_enum[i])))
            {
                log_severity = i;
                ret = 1;
                break;
            }
        }
    }
    return ret;
}

void ezlopi_core_read_set_log_severity()
{
    EZPI_CORE_nvs_read_log_severity(&log_severity);
}

int ezlopi_core_log_severity_process(bool severity_enable, const char* severity_str)
{
    int ret = 0;
    if (severity_enable)
    {
        ezlopi_hub_log_set_severity(severity_str);
        ret = 1;
    }
    else
    {
        log_severity = ENUM_EZLOPI_LOG_SEVERITY_MAX;
        ret = 1;
    }
    EZPI_CORE_nvs_write_log_severity(log_severity);
    return ret;
}

const char* ezlopi_core_log_get_current_severity_enum_str()
{
    return log_severity_enum[log_severity];
}

e_ezlopi_log_severity_t ezlopi_core_log_get_current_severity_enum_val()
{
    return log_severity;
}

int ezlopi_core_send_log(int severity, const char* format, ...)
{
    int ret = 0;
    e_ezlopi_event_t event = ezlopi_get_event_bit_status();
    if ((event & EZLOPI_EVENT_NMA_REG) == EZLOPI_EVENT_NMA_REG)
    {
        if (severity <= log_severity)
        {
            char formatted_log[500];
            va_list arglist;
            va_start(arglist, format);
            int bytes_written = vsnprintf(formatted_log, 4096, format, arglist);
            va_end(arglist);

            cJSON* cj_log_broadcast = cJSON_CreateObject();
            if (cj_log_broadcast)
            {
                cJSON_AddStringToObject(cj_log_broadcast, ezlopi_id_str, ezlopi_ui_broadcast_str);
                cJSON_AddStringToObject(cj_log_broadcast, ezlopi_msg_subclass_str, "hub.log");
                cJSON* cj_result = cJSON_AddObjectToObject(cj_log_broadcast, ezlopi_result_str);
                if (cj_result)
                {
                    uint64_t timestamp = EZPI_CORE_sntp_get_current_time_ms();
                    size_t total_len = 15 + bytes_written + 2;
                    char message[total_len];
                    memset(message, 0, total_len);
                    snprintf(message, total_len, "%lld: %s", timestamp, formatted_log);

                    cJSON_AddStringToObject(cj_result, ezlopi_message_str, message);

                    char severity_str[10];
                    memset(severity_str, 0, 10);
                    snprintf(severity_str, 10, "%s", log_severity_enum[severity]);
                    cJSON_AddStringToObject(cj_result, ezlopi_severity_str, severity_str);

                    ezlopi_core_broadcast_log_cjson(cj_log_broadcast);

                    ret = 1;
                }
            }
        }
    }
    return ret;
}

void ezlopi_core_log_set_broadcaster()
{
    ezlopi_util_set_log_broadcaster(ezlopi_core_send_log);
}

