#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_UTIL_TRACE_EN

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "cjext.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_heap.h"
#include "ezlopi_core_sntp.h"
#include "ezlopi_core_broadcast.h"
#include "ezlopi_core_event_group.h"
#include "ezlopi_core_errors.h"

#include "ezlopi_cloud_constants.h"

#include "ezlopi_core_log.h"

typedef struct ll_log_upcall
{
    f_trace_upcall_t upcall;
    struct ll_log_upcall *next;
} ll_log_upcall_t;

static ll_log_upcall_t *__log_upcall_head = NULL;
static e_ezpi_trace_severity_t serial_log_severity = E_TRACE_SEVERITY_NONE;
static e_ezpi_trace_severity_t cloud_log_severity = E_TRACE_SEVERITY_NONE;

static int (*__trace_upcall)(cJSON *cj_telemetry) = NULL;

static ezlopi_error_t ezlopi_hub_cloud_log_set_severity(const char *severity_str);
static ezlopi_error_t ezlopi_hub_serial_log_set_severity(const char *severity_str);

static void __cloud_log(e_ezpi_trace_severity_t severity, const char *file, int line, uint32_t time, char *msg);
static void __console_log(e_ezpi_trace_severity_t severity, const char *file, uint32_t line, uint32_t time, char *msg);
static void __log_upcall(e_ezpi_trace_severity_t severity, const char *file, uint32_t line, uint32_t time, char *msg);

void ezlopi_core_log_add_trace_upcall(int (*upcall)(cJSON *cj_telemetry))
{
    if (upcall)
    {
        __trace_upcall = upcall;
    }
}

int ezlopi_core_log_push_trace(cJSON *cj_trace)
{
    int ret = 0;

    if (cj_trace && __trace_upcall)
    {
        ret = __trace_upcall(cj_trace);
    }

    return ret;
}

void ezlopi_core_log_add_log_upcall(f_trace_upcall_t upcall)
{
    if (upcall)
    {
        if (__log_upcall_head)
        {
            ll_log_upcall_t *node = __log_upcall_head;
            while (node->next)
            {
                node = node->next;
            }

            ll_log_upcall_t *new_node = ezlopi_malloc(__FUNCTION__, sizeof(ll_log_upcall_t));
            if (new_node)
            {
                new_node->next = NULL;
                new_node->upcall = upcall;
                node->next = new_node;
            }
        }
        else
        {
            __log_upcall_head = ezlopi_malloc(__FUNCTION__, sizeof(ll_log_upcall_t));

            if (__log_upcall_head)
            {
                __log_upcall_head->next = NULL;
                __log_upcall_head->upcall = upcall;
            }
        }
    }
}

void ezlopi_core_log_init(void)
{
    ezlopi_util_trace_init(__log_upcall);
    ezlopi_core_log_add_log_upcall(__console_log);
    // ezlopi_core_log_add_log_upcall(__cloud_log);
}

//////// Setter
void ezlopi_core_read_set_log_severities()
{
    EZPI_CORE_nvs_read_cloud_log_severity(&cloud_log_severity);
    EZPI_CORE_nvs_read_serial_log_severity(&serial_log_severity);
}

ezlopi_error_t ezlopi_core_cloud_log_severity_process_str(bool severity_enable, const char *severity_str)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (severity_enable)
    {
        ezlopi_hub_cloud_log_set_severity(severity_str);
        ret = EZPI_SUCCESS;
    }
    else
    {
        cloud_log_severity = E_TRACE_SEVERITY_NONE;
        ret = EZPI_SUCCESS;
    }

    EZPI_CORE_nvs_write_cloud_log_severity(cloud_log_severity);
    return ret;
}

ezlopi_error_t ezlopi_core_cloud_log_severity_process_id(const e_ezpi_trace_severity_t severity_level_id)
{
    ezlopi_error_t ret = EZPI_FAILED;

    if ((severity_level_id >= E_TRACE_SEVERITY_NONE) && (severity_level_id <= E_TRACE_SEVERITY_ERROR))
    {
        if (E_TRACE_SEVERITY_WARNING <= severity_level_id)
        {
            cloud_log_severity = severity_level_id;
        }
        else
        {
            cloud_log_severity = E_TRACE_SEVERITY_WARNING;
        }

        EZPI_CORE_nvs_write_cloud_log_severity(cloud_log_severity);
        ret = EZPI_SUCCESS;
    }

    return ret;
}

ezlopi_error_t ezlopi_core_serial_log_severity_process_str(const char *severity_str)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (severity_str)
    {
        ezlopi_hub_serial_log_set_severity(severity_str);
        ret = EZPI_SUCCESS;
    }
    else
    {
        serial_log_severity = E_TRACE_SEVERITY_NONE;
        ret = EZPI_SUCCESS;
    }
    EZPI_CORE_nvs_write_serial_log_severity(serial_log_severity);
    return ret;
}

ezlopi_error_t ezlopi_core_serial_log_severity_process_id(const e_ezpi_trace_severity_t severity_level_id)
{
    ezlopi_error_t ret = EZPI_FAILED;

    if ((severity_level_id <= E_TRACE_SEVERITY_ERROR) && (severity_level_id >= E_TRACE_SEVERITY_NONE))
    {
        serial_log_severity = severity_level_id;
        EZPI_CORE_nvs_write_serial_log_severity(serial_log_severity);
        ret = EZPI_SUCCESS;
    }

    return ret;
}

//////// Getter
const char *ezlopi_core_cloud_log_get_current_severity_enum_str()
{
    return ezlopi_util_trace_get_severity_name_str(cloud_log_severity);
}

const char *ezlopi_core_serial_log_get_current_severity_enum_str()
{
    return ezlopi_util_trace_get_severity_name_str(serial_log_severity);
}

e_ezpi_trace_severity_t ezlopi_core_cloud_log_get_current_severity_enum_val()
{
    return cloud_log_severity;
}

e_ezpi_trace_severity_t ezlopi_core_serial_log_get_current_severity_enum_val()
{
    return serial_log_severity;
}

ezlopi_error_t ezlopi_core_send_cloud_log(int severity, const char *log_str)
{
    ezlopi_error_t ret = EZPI_FAILED;

    if (severity <= ezlopi_core_cloud_log_get_current_severity_enum_val())
    {
        e_ezlopi_event_t event = ezlopi_get_event_bit_status();
        if ((event & EZLOPI_EVENT_NMA_REG) == EZLOPI_EVENT_NMA_REG)
        {
            cJSON *cj_log_broadcast = cJSON_CreateObject(__FUNCTION__);
            if (cj_log_broadcast)
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_log_broadcast, ezlopi_id_str, ezlopi_ui_broadcast_str);
                cJSON_AddStringToObject(__FUNCTION__, cj_log_broadcast, ezlopi_msg_subclass_str, "hub.log");
                cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_log_broadcast, ezlopi_result_str);
                if (cj_result)
                {
                    uint64_t timestamp = EZPI_CORE_sntp_get_current_time_sec();

                    char timestamp_str[64];

                    EZPI_CORE_sntp_epoch_to_iso8601(timestamp_str, sizeof(timestamp_str), (time_t)timestamp);

                    size_t total_len = sizeof(timestamp_str) + strlen(log_str) + 2;
                    char message[total_len];
                    memset(message, 0, total_len);
                    snprintf(message, total_len, "%s: %s", timestamp_str, log_str);

                    cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_message_str, message);

                    char severity_str[10];
                    memset(severity_str, 0, 10);
                    snprintf(severity_str, 10, "%s", ezlopi_util_trace_get_severity_name_str(severity));
                    cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_severity_str, severity_str);
                    if (EZPI_SUCCESS != ezlopi_core_broadcast_add_to_queue(cj_log_broadcast))
                    {
                        cJSON_Delete(__FUNCTION__, cj_log_broadcast);
                    }
                    ret = EZPI_SUCCESS;
                }
            }
        }
    }
    return ret;
}

static ezlopi_error_t ezlopi_core_serial_log_upcall(int severity, const char *log_str)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if ((severity > E_TRACE_SEVERITY_NONE) && (severity <= ezlopi_core_serial_log_get_current_severity_enum_val()))
    {
        printf("%s\n", log_str);
        ret = EZPI_SUCCESS;
    }
    return ret;
}

void ezlopi_core_set_log_upcalls()
{
    // ezlopi_util_set_log_upcalls(ezlopi_core_send_cloud_log, ezlopi_core_serial_log_upcall);
}

/* Static Functions */
static e_ezpi_trace_severity_t ezlopi_core_log_severity_str_to_enum(char *severity_str)
{
    e_ezpi_trace_severity_t e_severity = E_TRACE_SEVERITY_NONE;
    if (0 == strncmp(ezlopi_util_trace_get_severity_name_str(E_TRACE_SEVERITY_TRACE), severity_str, strlen(ezlopi_util_trace_get_severity_name_str(E_TRACE_SEVERITY_TRACE))))
    {
        e_severity = E_TRACE_SEVERITY_TRACE;
    }
    else if (0 == strncmp(ezlopi_util_trace_get_severity_name_str(E_TRACE_SEVERITY_DEBUG), severity_str, strlen(ezlopi_util_trace_get_severity_name_str(E_TRACE_SEVERITY_DEBUG))))
    {
        e_severity = E_TRACE_SEVERITY_DEBUG;
    }
    else if (0 == strncmp(ezlopi_util_trace_get_severity_name_str(E_TRACE_SEVERITY_INFO), severity_str, strlen(ezlopi_util_trace_get_severity_name_str(E_TRACE_SEVERITY_INFO))))
    {
        e_severity = E_TRACE_SEVERITY_INFO;
    }
    else if (0 == strncmp(ezlopi_util_trace_get_severity_name_str(E_TRACE_SEVERITY_WARNING), severity_str, strlen(ezlopi_util_trace_get_severity_name_str(E_TRACE_SEVERITY_WARNING))))
    {
        e_severity = E_TRACE_SEVERITY_WARNING;
    }
    else if (0 == strncmp(ezlopi_util_trace_get_severity_name_str(E_TRACE_SEVERITY_ERROR), severity_str, strlen(ezlopi_util_trace_get_severity_name_str(E_TRACE_SEVERITY_ERROR))))
    {
        e_severity = E_TRACE_SEVERITY_ERROR;
    }

    return e_severity;
}

static ezlopi_error_t ezlopi_hub_cloud_log_set_severity(const char *severity_str)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (severity_str)
    {
        cloud_log_severity = ezlopi_core_log_severity_str_to_enum(severity_str);
    }
    return ret;
}

static ezlopi_error_t ezlopi_hub_serial_log_set_severity(const char *severity_str)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (severity_str)
    {
        serial_log_severity = ezlopi_core_log_severity_str_to_enum(severity_str);
    }
    return ret;
}

static void __console_log(e_ezpi_trace_severity_t severity, const char *file, uint32_t line, uint32_t time, char *msg)
{
    if ((severity > E_TRACE_SEVERITY_NONE) && (serial_log_severity <= severity))
    {
        char trace_header[192];

        switch (severity)
        {
        case E_TRACE_SEVERITY_ERROR:
        {
            snprintf(trace_header, sizeof(trace_header), "\x1B[%sm %s[%d]-%u", COLOR_FONT_RED, file, line, time);
            break;
        }
        case E_TRACE_SEVERITY_WARNING:
        {
            snprintf(trace_header, sizeof(trace_header), "\x1B[%sm %s[%d]-%u", COLOR_FONT_YELLOW, file, line, time);
            break;
        }
        case E_TRACE_SEVERITY_INFO:
        {
            snprintf(trace_header, sizeof(trace_header), "\x1B[%sm %s[%d]-%u", COLOR_FONT_BLUE, file, line, time);
            break;
        }
        case E_TRACE_SEVERITY_TRACE:
        {
            snprintf(trace_header, sizeof(trace_header), "\x1B[%sm %s[%d]-%u", COLOR_FONT_GREEN, file, line, time);
            break;
        }
        case E_TRACE_SEVERITY_DEBUG:
        default:
        {
            snprintf(trace_header, sizeof(trace_header), "\x1B[%sm %s[%d]-%u", COLOR_FONT_BLACK, file, line, time);
            break;
        }
        }

        ets_printf("%s: %s\x1B[0m\r\n", trace_header, msg);
    }
}

static void __cloud_log(e_ezpi_trace_severity_t severity, const char *file, int line, uint32_t time, char *msg)
{
    if (cloud_log_severity <= severity)
    {
        e_ezlopi_event_t event = ezlopi_get_event_bit_status();
        if ((event & EZLOPI_EVENT_NMA_REG) == EZLOPI_EVENT_NMA_REG)
        {
            cJSON *cj_log_broadcast = cJSON_CreateObject(__FUNCTION__);
            if (cj_log_broadcast)
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_log_broadcast, ezlopi_id_str, ezlopi_ui_broadcast_str);
                cJSON_AddStringToObject(__FUNCTION__, cj_log_broadcast, ezlopi_msg_subclass_str, "hub.log");
                cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_log_broadcast, ezlopi_result_str);
                if (cj_result)
                {
                    uint64_t timestamp = EZPI_CORE_sntp_get_current_time_sec();

                    char timestamp_str[64];

                    EZPI_CORE_sntp_epoch_to_iso8601(timestamp_str, sizeof(timestamp_str), (time_t)timestamp);

                    size_t total_len = sizeof(timestamp_str) + strlen(msg) + 2;
                    char message[total_len];
                    memset(message, 0, total_len);
                    snprintf(message, total_len, "%s: %s", timestamp_str, msg);

                    cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_message_str, message);

                    char severity_str[10];
                    memset(severity_str, 0, 10);
                    snprintf(severity_str, 10, "%s", ezlopi_util_trace_get_severity_name_str(severity));
                    cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_severity_str, severity_str);
                    if (EZPI_SUCCESS != ezlopi_core_broadcast_add_to_queue(cj_log_broadcast))
                    {
                        cJSON_Delete(__FUNCTION__, cj_log_broadcast);
                    }
                }
            }
        }
    }
}

static void __log_upcall(e_ezpi_trace_severity_t severity, const char *file, uint32_t line, uint32_t log_time, char *msg)
{
    ll_log_upcall_t *upcall_node = __log_upcall_head;
    while (upcall_node)
    {
        if (upcall_node->upcall)
        {
            upcall_node->upcall(severity, file, line, log_time, msg);
        }

        upcall_node = upcall_node->next;
        vTaskDelay(1);
    }
}

#endif // CONFIG_EZPI_UTIL_TRACE_EN