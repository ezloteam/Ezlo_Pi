#include <time.h>
#include "cjext.h"
#include "ezlopi_core_log.h"
#include "ezlopi_core_sntp.h"
#include "ezlopi_core_http.h"
#include "ezlopi_core_wifi.h"
#include "ezlopi_util_version.h"

#include "ezlopi_util_trace.h"
#include "bootloader_random.h"

// #ifdef CONFIG_EZPI_OTEL_EN

#define OTEL_HTTP 1
#define CONSOLE_LOG 1

typedef enum e_otel_type
{
    E_OTEL_NONE = 0,
    E_OTEL_LOGS,
    E_OTEL_TRACES,
    E_OTEL_MATRICS,
    E_OTEL_MAX,
} e_otel_type_t;

static QueueHandle_t __telemetry_queue = NULL;
static e_ezpi_trace_severity_t __severity_level = E_TRACE_SEVERITY_WARNING;

static void __otel_task(void *pv);
static void __post_telemetry_logs(char *telemetry_logs);
static void __console_log(e_ezpi_trace_severity_t severity, const char *file, uint32_t line, uint64_t time, char *msg);
static void __otel_log_upcall(e_ezpi_trace_severity_t severity, const char *file, uint32_t line, uint64_t time, char *msg);

int ezlopi_service_otel_add_log_to_telemetry_queue(e_otel_type_t telemetry_type, cJSON *cj_telemetry)
{
    int ret = 0;

    if (cj_telemetry)
    {
        cJSON_AddNumberToObject(__FUNCTION__, cj_telemetry, "otel_type", telemetry_type);

        if (pdTRUE == xQueueIsQueueFullFromISR(__telemetry_queue))
        {
            cJSON *tmp_cjson_obj = NULL;

            xQueueReceive(__telemetry_queue, &tmp_cjson_obj, 0);

            if (tmp_cjson_obj)
            {
                cJSON_Delete(__FUNCTION__, tmp_cjson_obj);
            }
        }

        if (pdFALSE == xQueueSend(__telemetry_queue, &cj_telemetry, 5 / portTICK_PERIOD_MS))
        {
            ret = 1;
        }
    }

    return ret;
}

void ezlopi_service_otel_init(void)
{
    bootloader_random_enable();
    ezlopi_core_log_add_log_upcall(__otel_log_upcall);
    ezlopi_core_log_add_trace_upcall(__otel_trace_upcall);

#if (1 == OTEL_HTTP)
    __telemetry_queue = xQueueCreate(10, sizeof(cJSON *));
    if (__telemetry_queue)
    {
        xTaskCreate(__otel_task, "otel-task", 4096, NULL, 3, NULL);
    }
#endif
}

#if (1 == OTEL_HTTP)
static void __otel_task(void *pv)
{
    while (1)
    {
        cJSON *cj_telemetry = NULL;

        if (pdTRUE == xQueueReceive(__telemetry_queue, &cj_telemetry, portMAX_DELAY))
        {
            e_otel_type_t telemetry_type = E_OTEL_NONE; // need to define
            cJSON *cj_telemetry_type = cJSON_GetObjectItem(__FUNCTION__, cj_telemetry, "otel_type");

            if (cj_telemetry_type && cj_telemetry_type->valueint)
            {
                telemetry_type = (e_otel_type_t)cj_telemetry_type->valueint;
            }

            cJSON_DeleteItemFromObject(__FUNCTION__, cj_telemetry, "otel_type");

            char *telemetry_str = cJSON_PrintBuffered(__FUNCTION__, cj_telemetry, 4096, false);
            cJSON_Delete(__FUNCTION__, cj_telemetry);

            if (telemetry_str)
            {
                switch (telemetry_type)
                {
                case E_OTEL_LOGS:
                {
                    __post_telemetry_logs(telemetry_str);
                    break;
                }
                case E_OTEL_TRACES:
                {
                    // __post_telemetry_traces(telemetry_str);
                    break;
                }
                case E_OTEL_MATRICS:
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

static void __post_telemetry_traces(char *telemetry_traces)
{
    if (EZPI_SUCCESS == ezlopi_wait_for_wifi_to_connect(portMAX_DELAY))
    {
        static const char *default_log_path = "v1/logs";

        // char tmp_url[64];
        // snprintf(tmp_url, sizeof(tmp_url), "%s/%s", CONFIG_EZPI_OTEL_COLLECTOR_ADDRESS, default_log_path);

        cJSON *cj_headers = cJSON_CreateObject(__FUNCTION__);
        if (cj_headers)
        {
            cJSON_AddStringToObject(__FUNCTION__, cj_headers, "Content-Type", "application/json");
        }

        s_ezlopi_http_data_t *ret = ezlopi_http_post_request("https://ot.ezlo.com", 4318, "logs", cj_headers, NULL, NULL, NULL);
        printf("response[%d]: %.*s\r\n", ret->status_code, ret->response_len, ret->response);
    }
}

static void __post_telemetry_logs(char *telemetry_logs)
{
    if (EZPI_SUCCESS == ezlopi_wait_for_wifi_to_connect(portMAX_DELAY))
    {
        static const char *default_log_path = "v1/logs";

        // char tmp_url[64];
        // snprintf(tmp_url, sizeof(tmp_url), "%s/%s", CONFIG_EZPI_OTEL_COLLECTOR_ADDRESS, default_log_path);

        cJSON *cj_headers = cJSON_CreateObject(__FUNCTION__);
        if (cj_headers)
        {
            cJSON_AddStringToObject(__FUNCTION__, cj_headers, "Content-Type", "application/json");
        }

        s_ezlopi_http_data_t *ret = ezlopi_http_post_request("https://ot.ezlo.com", 4318, "logs", cj_headers, NULL, NULL, NULL);
        printf("response[%d]: %.*s\r\n", ret->status_code, ret->response_len, ret->response);
    }
}

#endif // OTEL_HTTP

static void __otel_log_upcall(e_ezpi_trace_severity_t severity, const char *file, uint32_t line, uint64_t log_time, char *msg)
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
            cJSON_AddStringToObject(__FUNCTION__, cj_log, "severity_text", ezlopi_util_trace_get_severity_name_str(severity));

            cJSON *cj_attributes = cJSON_AddObjectToObject(__FUNCTION__, cj_log, "attributes");
            if (cj_attributes)
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_attributes, "otelSpanID", "0");
                cJSON_AddStringToObject(__FUNCTION__, cj_attributes, "otelTraceID", "0");
                cJSON_AddFalseToObject(__FUNCTION__, cj_attributes, "otelTraceSampled");
                cJSON_AddStringToObject(__FUNCTION__, cj_attributes, "otelServiceName", "ezlopi");
                cJSON_AddStringToObject(__FUNCTION__, cj_attributes, "code.filepath", file);
                cJSON_AddNumberToObject(__FUNCTION__, cj_attributes, "code.lineno", line);
            }

            cJSON_AddNumberToObject(__FUNCTION__, cj_log, "dropped_attributes", 0);
            time_t now;
            time(&now);
            uint64_t time_now = now * 1000LL;
            snprintf(tmp_string, sizeof(tmp_string), "%llu", time_now);
            // snprintf(tmp_string, sizeof(tmp_string), "%llu", now * 1000LL);
            cJSON_AddStringToObject(__FUNCTION__, cj_log, "timestamp", tmp_string);

            snprintf(tmp_string, sizeof(tmp_string), "%llu", log_time);
            cJSON_AddStringToObject(__FUNCTION__, cj_log, "observed_timestamp", tmp_string);

            uint32_t rnd_trace_id[4];
            bootloader_fill_random(&rnd_trace_id, sizeof(rnd_trace_id));
            snprintf(tmp_string, sizeof(tmp_string), "%08x%08x%08x%08x", rnd_trace_id[0], rnd_trace_id[1], rnd_trace_id[2], rnd_trace_id[3]);
            cJSON_AddStringToObject(__FUNCTION__, cj_log, "trace_id", tmp_string);

            bootloader_fill_random(&rnd_trace_id, sizeof(rnd_trace_id));
            snprintf(tmp_string, sizeof(tmp_string), "%08x%08x", rnd_trace_id[0], rnd_trace_id[1]);
            cJSON_AddStringToObject(__FUNCTION__, cj_log, "span_id", tmp_string);

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

#if (1 == CONSOLE_LOG) && (1 == OTEL_HTTP)
            char *log_str = cJSON_Print(__FUNCTION__, cj_log);

            if (log_str)
            {
                printf("\r\n%s", log_str);
                ezlopi_free(__FUNCTION__, log_str);
            }

            ezlopi_service_otel_add_log_to_telemetry_queue(E_OTEL_LOGS, cj_log);

#elif (1 == OTEL_HTTP)
            ezlopi_service_otel_add_log_to_telemetry_queue(E_OTEL_LOGS, cj_log);

#elif (1 == CONSOLE_LOG)
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
}

static void ezlopi_service_otel_trace_create(const char *srv_name, const char *ver, const char *host, const char *provider)
{
    cJSON *cj_telemetry = cJSON_CreateObject(__FUNCTION__);
    if (cj_telemetry)
    {
        cJSON *cj_resource = cJSON_AddObjectToObject(__FUNCTION__, cj_telemetry, "resource");
        if (cj_resource)
        {
            cJSON *cj_attributes = cJSON_AddObjectToObject(__FUNCTION__, cj_resource, "attributes");
            if (cj_attributes)
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_attributes, "service.name", "ezlopi");
                cJSON_AddStringToObject(__FUNCTION__, cj_attributes, "service.version", VERSION_STR);
                cJSON_AddStringToObject(__FUNCTION__, cj_attributes, "host.name", "");
                cJSON_AddStringToObject(__FUNCTION__, cj_attributes, "cloud.provider", "");
            }
        }

        cJSON *cj_instrumentation_lib = cJSON_AddObjectToObject(__FUNCTION__, cj_telemetry, "instrumentation_library");
        if (cj_instrumentation_lib)
        {
            cJSON_AddStringToObject(__FUNCTION__, cj_instrumentation_lib, "name", "ezlopi-otel-c");
            // cJSON_AddStringToObject(__FUNCTION__, cj_instrumentation_lib, "version", )
        }
    }
}

static int __otel_trace_upcall(cJSON *cj_traces)
{
    if (cj_traces)
    {
        ezlopi_service_otel_add_log_to_telemetry_queue();
    }
}
