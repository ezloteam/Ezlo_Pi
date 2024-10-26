#include "cjext.h"
#include "ezlopi_core_log.h"
#include "ezlopi_core_sntp.h"
#include "ezlopi_core_http.h"
#include "ezlopi_util_trace.h"
// #include "bootloader_random.h"

static e_ezpi_trace_severity_t __severity_level = E_TRACE_SEVERITY_WARNING;
static QueueHandle_t __telemetry_queue = NULL;

static void __otel_task(void *pv);
static void __post_telemetry_logs(char *telemetry_logs);
static ezlopi_error_t __otel_logs_capture(e_ezpi_trace_severity_t severity, const char *log_str);
static void __console_log(e_ezpi_trace_severity_t severity, const char *file, int line, uint64_t time, char *msg);
static void __otel_console_log(e_ezpi_trace_severity_t severity, const char *file, int line, uint64_t time, char *msg);

int ezlopi_service_otel_add_to_telemetry_queue(int telemetry_type, cJSON *cj_telemetry)
{
    int ret = 0;

    if (cj_telemetry)
    {
        cJSON_AddNumberToObject(__FUNCTION__, cj_telemetry, "otel_type", telemetry_type);
    }

    return ret;
}

void ezlopi_service_otel_init(void)
{
    // ezlopi_util_set_log_upcalls(NULL, __otel_logs_capture);
    // ezlopi_util_trace_add_upcall(__console_log);
    ezlopi_util_trace_add_upcall(__otel_console_log);

    __telemetry_queue = xQueueCreate(10, sizeof(cJSON *));
    if (__telemetry_queue)
    {
        xTaskCreate(__otel_task, "otel-task", 4096, NULL, 3, NULL);
    }
}

static void __otel_task(void *pv)
{
    while (1)
    {
        cJSON *cj_telemetry = NULL;

        if (pdTRUE == xQueueReceive(__telemetry_queue, &cj_telemetry, portMAX_DELAY))
        {
            char *telemetry_str = cJSON_PrintBuffered(__FUNCTION__, cj_telemetry, 4096, false);
            cJSON_Delete(__FUNCTION__, cj_telemetry);

            if (telemetry_str)
            {
                int telemetry_type = 0; // need to define

                switch (telemetry_type)
                {
                case 0:
                {
                    __post_telemetry_logs(telemetry_str);
                    break;
                }
                case 1:
                {
                    // __post_telemetry_traces(telemetry_str);
                    break;
                }
                case 2:
                {
                    // __post_telemetry_matrics(telemetry_str);
                    break;
                }

                default:
                {
                    break;
                }
                }

                ezlopi_free(__FUNCTION__, telemetry_str);
            }
        }

        vTaskDelay(10 / portTICK_RATE_MS);
    }

    vTaskDelete(NULL);
}

static void __post_telemetry_logs(char *telemetry_logs)
{
    static const char *default_log_path = "/v1/logs";

    // char tmp_url[64];
    // snprintf(tmp_url, sizeof(tmp_url), "%s/%s", CONFIG_EZPI_OTEL_COLLECTOR_ADDRESS, default_log_path);

    cJSON *cj_headers = cJSON_CreateObject(__FUNCTION__);
    if (cj_headers)
    {
        cJSON_AddStringToObject(__FUNCTION__, cj_headers, "Content-Type", "application/json");
    }

    ezlopi_http_post_request(CONFIG_EZPI_OTEL_COLLECTOR_ADDRESS, default_log_path, cj_headers, NULL, NULL, NULL);
}

static void __otel_console_log(e_ezpi_trace_severity_t severity, const char *file, int line, uint64_t time, char *msg)
{
    if (severity <= __severity_level)
    {
        cJSON *cj_log = cJSON_CreateObject(__FUNCTION__);

        if (cj_log)
        {
            char tmp_string[128];

            cJSON_AddStringToObject(__FUNCTION__, cj_log, "body", msg);

            snprintf(tmp_string, sizeof(tmp_string), "<SeverityNumber.%s>: %d", ezlopi_util_trace_get_severity_name_str(severity), severity);
            cJSON_AddStringToObject(__FUNCTION__, cj_log, "severity_number", tmp_string);
            cJSON_AddStringToObject(__FUNCTION__, cj_log, "severity_text", ezlopi_core_log_severity_to_str(severity));

            cJSON *cj_attributes = cJSON_AddObjectToObject(__FUNCTION__, cj_log, "attributes");
            if (cj_attributes)
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_attributes, "otelSpanID", "0");
                cJSON_AddStringToObject(__FUNCTION__, cj_attributes, "otelTraceID", "0");
                cJSON_AddFalseToObject(__FUNCTION__, cj_attributes, "otelTraceSampled");
                cJSON_AddStringToObject(__FUNCTION__, cj_attributes, "otelServiceName", "ezlopi");
                cJSON_AddStringToObject(__FUNCTION__, cj_attributes, "code.filepath", "");
                cJSON_AddNumberToObject(__FUNCTION__, cj_attributes, "code.lineno", 0);
            }

            cJSON_AddNumberToObject(__FUNCTION__, cj_log, "dropped_attributes", 0);
            snprintf(tmp_string, "%llu", EZPI_CORE_sntp_get_current_time_ms());
            cJSON_AddStringToObject(__FUNCTION__, cj_log, "timestamp", tmp_string);

            snprintf(tmp_string, "%llu", time);
            cJSON_AddStringToObject(__FUNCTION__, cj_log, "observed_timestamp", "0");

            cJSON_AddStringToObject(__FUNCTION__, cj_log, "trace_id", "0x00000000000000000000000000000000");
            cJSON_AddStringToObject(__FUNCTION__, cj_log, "span_id", "0x0000000000000000");
            cJSON_AddFalseToObject(__FUNCTION__, cj_log, "trace_flags");

            cJSON *cj_resource = cJSON_AddObjectToObject(__FUNCTION__, cj_log, "resource");
            if (cj_resource)
            {
                cJSON *cj_attributes_2 = cJSON_AddObjectToObject(__FUNCTION__, cj_resource, "attributes");
                if (cj_attributes_2)
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_attributes_2, "telemetry.sdk.langauge", "c");
                    cJSON_AddStringToObject(__FUNCTION__, cj_attributes_2, "telemetry.sdk.name", "opentelemetry");
                    cJSON_AddStringToObject(__FUNCTION__, cj_attributes_2, "telemetry.sdk.version", "0.1.0");
                    cJSON_AddStringToObject(__FUNCTION__, cj_attributes_2, "telemetry.sdk.langauge", "c");
                    cJSON_AddStringToObject(__FUNCTION__, cj_attributes_2, "service.name", "ezlopi");
                    cJSON_AddStringToObject(__FUNCTION__, cj_attributes_2, "telemetry.auto.version", "0.18b0");
                }

                cJSON_AddStringToObject(__FUNCTION__, cj_attributes_2, "schema_url", "");
            }
        }

#if 1 // for testing purpose only
        char *log_str = cJSON_Print(__FUNCTION__, cj_log);
        cJSON_Delete(__FUNCTION__, cj_log);
        if (log_str)
        {
            printf("\r\nLog:\r\n%s\r\n", log_str);
            ezlopi_free(__FUNCTION__, log_str);
        }
#endif
    }
}

static void __console_log(e_ezpi_trace_severity_t severity, const char *file, int line, uint64_t time, char *msg)
{
    switch (severity)
    {
    case E_TRACE_SEVERITY_ERROR:
    {
        printf("\r\n\x1B[%sm %s[%d]-%llu: ", COLOR_FONT_RED, file, line, time);
        break;
    }
    case E_TRACE_SEVERITY_WARNING:
    {
        printf("\r\n\x1B[%sm %s[%d]-%llu: ", COLOR_FONT_YELLOW, file, line, time);
        break;
    }
    case E_TRACE_SEVERITY_INFO:
    {
        printf("\r\n\x1B[%sm %s[%d]-%llu: ", COLOR_FONT_BLUE, file, line, time);
        break;
    }
    case E_TRACE_SEVERITY_TRACE:
    {
        printf("\r\n\x1B[%sm %s[%d]-%llu: ", COLOR_FONT_GREEN, file, line, time);
        break;
    }
    case E_TRACE_SEVERITY_DEBUG:
    default:
    {
        printf("\r\n\x1B[%sm %s[%d]-%llu: ", COLOR_FONT_BLACK, file, line, time);
        break;
    }
    }

    printf(msg);
    printf("\x1B[0m\r\n");
}

static ezlopi_error_t __otel_logs_capture(e_ezpi_trace_severity_t severity, const char *log_str)
{
    ezlopi_error_t ret = EZPI_FAILED;

    if ((severity > E_TRACE_SEVERITY_NONE) && (severity <= ezlopi_core_serial_log_get_current_severity_enum_val()))
    {
        cJSON *cj_log = cJSON_CreateObject(__FUNCTION__);

        if (cj_log)
        {
            cJSON_AddStringToObject(__FUNCTION__, cj_log, "body", log_str);

            char tmp_string[128];
            snprintf(tmp_string, sizeof(tmp_string), "<SeverityNumber.%s>: %d", ezlopi_core_log_severity_to_str(severity), severity);
            cJSON_AddStringToObject(__FUNCTION__, cj_log, "severity_number", tmp_string);
            cJSON_AddStringToObject(__FUNCTION__, cj_log, "severity_text", ezlopi_core_log_severity_to_str(severity));

            cJSON *cj_attributes = cJSON_AddObjectToObject(__FUNCTION__, cj_log, "attributes");
            if (cj_attributes)
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_attributes, "otelSpanID", "0");
                cJSON_AddStringToObject(__FUNCTION__, cj_attributes, "otelTraceID", "0");
                cJSON_AddFalseToObject(__FUNCTION__, cj_attributes, "otelTraceSampled");
                cJSON_AddStringToObject(__FUNCTION__, cj_attributes, "otelServiceName", "ezlopi");
                cJSON_AddStringToObject(__FUNCTION__, cj_attributes, "code.filepath", "");
                cJSON_AddNumberToObject(__FUNCTION__, cj_attributes, "code.lineno", 0);
            }

            cJSON_AddNumberToObject(__FUNCTION__, cj_log, "dropped_attributes", 0);
            cJSON_AddStringToObject(__FUNCTION__, cj_log, "timestamp", "0");
            cJSON_AddStringToObject(__FUNCTION__, cj_log, "observed_timestamp", "0");
            cJSON_AddStringToObject(__FUNCTION__, cj_log, "trace_id", "0x00000000000000000000000000000000");
            cJSON_AddStringToObject(__FUNCTION__, cj_log, "span_id", "0x0000000000000000");
            cJSON_AddFalseToObject(__FUNCTION__, cj_log, "trace_flags");

            cJSON *cj_resource = cJSON_AddObjectToObject(__FUNCTION__, cj_log, "resource");
            if (cj_resource)
            {
                cJSON *cj_attributes_2 = cJSON_AddObjectToObject(__FUNCTION__, cj_resource, "attributes");
                if (cj_attributes_2)
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_attributes_2, "telemetry.sdk.langauge", "c");
                    cJSON_AddStringToObject(__FUNCTION__, cj_attributes_2, "telemetry.sdk.name", "opentelemetry");
                    cJSON_AddStringToObject(__FUNCTION__, cj_attributes_2, "telemetry.sdk.version", "0.1.0");
                    cJSON_AddStringToObject(__FUNCTION__, cj_attributes_2, "telemetry.sdk.langauge", "c");
                    cJSON_AddStringToObject(__FUNCTION__, cj_attributes_2, "service.name", "ezlopi");
                    cJSON_AddStringToObject(__FUNCTION__, cj_attributes_2, "telemetry.auto.version", "0.18b0");
                }

                cJSON_AddStringToObject(__FUNCTION__, cj_attributes_2, "schema_url", "");
            }
        }

        char *log_str = cJSON_Print(__FUNCTION__, cj_log);
        if (log_str)
        {
            printf("\r\nLog:\r\n%s\r\n", log_str);
        }

        ret = EZPI_SUCCESS;
    }

    return ret;
}
