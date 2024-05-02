
#include <stdbool.h>
#include <string.h>

#include "ezlopi_core_log.h"


const char* log_severity_enum[ENUM_EZLOPI_LOG_SEVERITY_MAX] = {
    "ERROR",
    "WARNING",
    "INFO",
    "DEBUG",
    "TRACE"
};

static e_ezlopi_log_severity_t log_severity = ENUM_EZLOPI_LOG_SEVERITY_MAX;

static int ezlopi_hub_log_set_severity(const char* severity_str)
{
    int ret = 0;
    if (severity_str)
    {
        for (int i = 0; i < ENUM_EZLOPI_LOG_SEVERITY_MAX; i++)
        {
            if (strncmp(log_severity_enum[i], severity_str, strlen(log_severity_enum[i])))
            {
                log_severity = i;
                ret = 1;
                break;
            }
        }
    }
    return ret;
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
    return ret;
}

const char* ezlopi_core_log_get_current_severity_enum()
{
    return log_severity_enum[log_severity];
}



