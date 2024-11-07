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
#define CONSOLE_LOG 0

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
static void __post_telemetry(const char *location, char *telemetry_data);

static int __otel_trace_upcall(cJSON *cj_traces);
static void __otel_log_upcall(e_ezpi_trace_severity_t severity, const char *file, uint32_t line, uint32_t time, char *msg);

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

        if (pdTRUE == xQueueSend(__telemetry_queue, &cj_telemetry, 5 / portTICK_PERIOD_MS))
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
    ezlopi_wait_for_wifi_to_connect(portMAX_DELAY);

    while (1)
    {
        cJSON *cj_telemetry = NULL;

        if (pdTRUE == xQueueReceive(__telemetry_queue, &cj_telemetry, portMAX_DELAY))
        {
            bool delete_required_cj_telemetry = true;
            e_otel_type_t telemetry_type = E_OTEL_NONE; // need to define
            cJSON *cj_telemetry_type = cJSON_DetachItemFromObject(__FUNCTION__, cj_telemetry, "otel_type");

            if (cj_telemetry_type)
            {
                if (cj_telemetry_type->valueint)
                {
                    telemetry_type = (e_otel_type_t)cj_telemetry_type->valueint;
                }

                cJSON_Delete(__FUNCTION__, cj_telemetry_type);
            }

            switch (telemetry_type)
            {
            case E_OTEL_LOGS:
            {
                cJSON *cj_logs_obj = cJSON_CreateObject(__FUNCTION__);
                if (cj_logs_obj)
                {
                    cJSON *cj_resourceLogs = cJSON_AddArrayToObject(__FUNCTION__, cj_logs_obj, "resourceLogs");

                    if (cj_resourceLogs)
                    {
                        cJSON *cj_log = cJSON_CreateObject(__FUNCTION__);

                        if (cj_log)
                        {
                            cJSON *cj_resource = cJSON_AddObjectToObject(__FUNCTION__, cj_log, "resource");
                            if (cj_resource)
                            {
                                cJSON *cj_attributes = cJSON_AddArrayToObject(__FUNCTION__, cj_resource, "attributes");
                                if (cj_attributes)
                                {
                                    cJSON *cj_attr = cJSON_CreateObject(__FUNCTION__);
                                    if (cj_attr)
                                    {
                                        cJSON_AddStringToObject(__FUNCTION__, cj_attr, "key", "service.name");
                                        cJSON *cj_value = cJSON_AddObjectToObject(__FUNCTION__, cj_attr, "value");
                                        if (cj_value)
                                        {
                                            cJSON_AddStringToObject(__FUNCTION__, cj_value, "stringValue", "EzloPI");
                                        }

                                        if (false == cJSON_AddItemToArray(cj_attributes, cj_attr))
                                        {
                                            cJSON_Delete(__FUNCTION__, cj_attr);
                                        }
                                    }
                                }
                            }

                            cJSON *cj_scopeLogs = cJSON_AddArrayToObject(__FUNCTION__, cj_log, "scopeLogs");
                            if (cj_scopeLogs)
                            {
                                cJSON *cj_scopeLogs_item = cJSON_CreateObject(__FUNCTION__);
                                if (cj_scopeLogs_item)
                                {
                                    cJSON *cj_scope = cJSON_AddObjectToObject(__FUNCTION__, cj_scopeLogs_item, "scope");
                                    if (cj_scope)
                                    {
                                        cJSON_AddStringToObject(__FUNCTION__, cj_scope, "name", "otel-c");
                                        cJSON_AddStringToObject(__FUNCTION__, cj_scope, "version", "1.0.0");
                                        cJSON_AddArrayToObject(__FUNCTION__, cj_scope, "attributes");
                                    }

                                    if (false == cJSON_AddItemToObject(__FUNCTION__, cj_scopeLogs_item, "logRecords", cj_telemetry))
                                    {
                                        cJSON_Delete(__FUNCTION__, cj_telemetry);
                                    }

                                    if (false == cJSON_AddItemToArray(cj_scopeLogs, cj_scopeLogs_item))
                                    {
                                        cJSON_Delete(__FUNCTION__, cj_scopeLogs_item);
                                    }

                                    delete_required_cj_telemetry = false;
                                }
                            }

                            if (false == cJSON_AddItemToArray(cj_resourceLogs, cj_log))
                            {
                                cJSON_Delete(__FUNCTION__, cj_log);
                            }
                        }
                    }

                    char *telemetry_str = cJSON_PrintBuffered(__FUNCTION__, cj_logs_obj, 4096, false);
                    cJSON_Delete(__FUNCTION__, cj_logs_obj);
                    if (telemetry_str)
                    {
                        __post_telemetry("logs", telemetry_str);
                        ezlopi_free(__FUNCTION__, telemetry_str);
                    }
                }

                // __post_telemetry("hello", telemetry_str);
                break;
            }
            case E_OTEL_TRACES:
            {
                // __post_telemetry("traces", telemetry_str);
                break;
            }
            case E_OTEL_MATRICS:
            {
                // __post_telemetry("matrics", telemetry_str);
                break;
            }

            default:
            {
                break;
            }
            }

            if (delete_required_cj_telemetry)
            {
                cJSON_Delete(__FUNCTION__, cj_telemetry);
            }
        }

        vTaskDelay(5 / portTICK_RATE_MS);
    }

    vTaskDelete(NULL);
}

static void __post_telemetry(const char *location, char *telemetry_data)
{
    if (EZPI_SUCCESS == ezlopi_wait_for_wifi_to_connect(portMAX_DELAY))
    {
        cJSON *cj_headers = cJSON_CreateObject(__FUNCTION__);
        if (cj_headers)
        {
            cJSON_AddStringToObject(__FUNCTION__, cj_headers, "Content-Type", "application/json");
        }

        char uri[128];
        snprintf(uri, sizeof(uri), "https://ot.ezlo.com/%s", location);

        time_t start_time;
        time(&start_time);
        s_ezlopi_http_data_t *ret = ezlopi_http_post_request(uri, cj_headers, telemetry_data, strlen(telemetry_data),
                                                             NULL, NULL, NULL);
        time_t end_time;
        time(&end_time);

        if (cj_headers)
        {
            cJSON_Delete(__FUNCTION__, cj_headers);
        }

        if (ret)
        {
            printf("\r\nHTTP-response-code: %d\r\n", ret->status_code);

            if (ret->response)
            {
                printf("HTTP-response: %.*s\r\n", ret->response_len, ret->response);
                ezlopi_free(__FUNCTION__, ret->response);
            }

            ezlopi_free(__FUNCTION__, ret);
        }

        printf("HTTP-request-duration: %ld Sec.\r\n", end_time - start_time);
    }
}
#endif // OTEL_HTTP

static void __otel_log_upcall(e_ezpi_trace_severity_t severity, const char *file, uint32_t line, uint32_t log_time, char *msg)
{
    // printf("severity: %d | %d\r\n", severity, __severity_level);

    if (__severity_level <= severity)
    {
        cJSON *cj_logRecords = cJSON_CreateArray(__FUNCTION__);
        if (cj_logRecords)
        {
            cJSON *cj_logRecords_item = cJSON_CreateObject(__FUNCTION__);
            if (cj_logRecords_item)
            {
                char tmp_string[128];

                time_t now;
                time(&now);
                snprintf(tmp_string, sizeof(tmp_string), "%llu", now * 1000000000LL);
                cJSON_AddStringToObject(__FUNCTION__, cj_logRecords_item, "timeUnixNano", tmp_string);

                snprintf(tmp_string, sizeof(tmp_string), "%llu", log_time * 1000000000LL);
                cJSON_AddStringToObject(__FUNCTION__, cj_logRecords_item, "observedTimeUnixNano", tmp_string);

                cJSON_AddNumberToObject(__FUNCTION__, cj_logRecords_item, "severityNumber", severity);
                cJSON_AddStringToObject(__FUNCTION__, cj_logRecords_item, "severityText", ezlopi_util_trace_get_severity_name_str(severity));

                // cJSON_AddNumberToObject(__FUNCTION__, cj_logRecords_item, "severityNumber", 2);
                // cJSON_AddStringToObject(__FUNCTION__, cj_logRecords_item, "severityText", "WARNING");

                uint32_t rnd_trace_id[4];
                bootloader_fill_random(&rnd_trace_id, sizeof(rnd_trace_id));
                snprintf(tmp_string, sizeof(tmp_string), "%08X%08X%08X%08X", rnd_trace_id[0], rnd_trace_id[1], rnd_trace_id[2], rnd_trace_id[3]);
                cJSON_AddStringToObject(__FUNCTION__, cj_logRecords_item, "traceId", tmp_string);

                bootloader_fill_random(&rnd_trace_id, sizeof(rnd_trace_id));
                snprintf(tmp_string, sizeof(tmp_string), "%08X%08X", rnd_trace_id[0], rnd_trace_id[1]);
                cJSON_AddStringToObject(__FUNCTION__, cj_logRecords_item, "spanId", tmp_string);

                cJSON *cj_body = cJSON_AddObjectToObject(__FUNCTION__, cj_logRecords_item, "body");
                if (cj_body)
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_body, "stringValue", msg);
                }

                cJSON *cj_attributes = cJSON_AddArrayToObject(__FUNCTION__, cj_logRecords_item, "attributes");
                if (cj_attributes)
                {
                    cJSON *cj_filename = cJSON_CreateObject(__FUNCTION__);
                    if (cj_filename)
                    {
                        cJSON_AddStringToObject(__FUNCTION__, cj_filename, "key", "string.fileName");
                        cJSON *cj_value = cJSON_AddObjectToObject(__FUNCTION__, cj_filename, "value");
                        if (cj_value)
                        {
                            cJSON_AddStringToObject(__FUNCTION__, cj_value, "stringValue", file);
                        }

                        if (false == cJSON_AddItemToArray(cj_attributes, cj_filename))
                        {
                            cJSON_Delete(__FUNCTION__, cj_filename);
                        }
                    }

                    cJSON *cj_lineNo = cJSON_CreateObject(__FUNCTION__);
                    if (cj_lineNo)
                    {
                        cJSON_AddStringToObject(__FUNCTION__, cj_lineNo, "key", "int.lineNo");
                        cJSON *cj_value = cJSON_AddObjectToObject(__FUNCTION__, cj_lineNo, "value");
                        if (cj_value)
                        {
                            cJSON_AddNumberToObject(__FUNCTION__, cj_value, "intValue", line);
                        }

                        if (false == cJSON_AddItemToArray(cj_attributes, cj_lineNo))
                        {
                            cJSON_Delete(__FUNCTION__, cj_lineNo);
                        }
                    }

                    cJSON *cj_freeHeap = cJSON_CreateObject(__FUNCTION__);
                    if (cj_freeHeap)
                    {
                        cJSON_AddStringToObject(__FUNCTION__, cj_freeHeap, "key", "int.freeHeap");
                        cJSON *cj_value = cJSON_AddObjectToObject(__FUNCTION__, cj_freeHeap, "value");
                        if (cj_value)
                        {
                            cJSON_AddNumberToObject(__FUNCTION__, cj_value, "intValue", esp_get_free_heap_size());
                        }

                        if (false == cJSON_AddItemToArray(cj_attributes, cj_freeHeap))
                        {
                            cJSON_Delete(__FUNCTION__, cj_freeHeap);
                        }
                    }

                    cJSON *cj_heapWatermark = cJSON_CreateObject(__FUNCTION__);
                    if (cj_heapWatermark)
                    {
                        cJSON_AddStringToObject(__FUNCTION__, cj_heapWatermark, "key", "int.heapWatermark");
                        cJSON *cj_value = cJSON_AddObjectToObject(__FUNCTION__, cj_heapWatermark, "value");
                        if (cj_value)
                        {
                            cJSON_AddNumberToObject(__FUNCTION__, cj_value, "intValue", esp_get_minimum_free_heap_size());
                        }

                        if (false == cJSON_AddItemToArray(cj_attributes, cj_heapWatermark))
                        {
                            cJSON_Delete(__FUNCTION__, cj_heapWatermark);
                        }
                    }
                }

                if (false == cJSON_AddItemToArray(cj_logRecords, cj_logRecords_item))
                {
                    cJSON_Delete(__FUNCTION__, cj_logRecords_item);
                }
            }

#if (1 == CONSOLE_LOG) && (1 == OTEL_HTTP)
            char *logRecords_str = cJSON_Print(__FUNCTION__, cj_logRecords);
            if (logRecords_str)
            {
                printf("\r\n%s", logRecords_str);
                ezlopi_free(__FUNCTION__, logRecords_str);
            }
            ezlopi_service_otel_add_log_to_telemetry_queue(E_OTEL_LOGS, cj_logRecords);
#elif (1 == OTEL_HTTP)
            ezlopi_service_otel_add_log_to_telemetry_queue(E_OTEL_LOGS, cj_logRecords);
#elif (1 == CONSOLE_LOG)
            char *logRecords_str = cJSON_Print(__FUNCTION__, cj_logRecords);
            cJSON_Delete(__FUNCTION__, cj_logRecords);
            if (logRecords_str)
            {
                printf("\r\nLog:\r\n%s\r\n", logRecords_str);
                ezlopi_free(__FUNCTION__, logRecords_str);
            }
#endif
        }

#if 0
        cJSON *cj_log = cJSON_CreateObject(__FUNCTION__);
        if (cj_log)
        {
            cJSON *cj_resource = cJSON_AddObjectToObject(__FUNCTION__, cj_log, "resource");
            if (cj_resource)
            {
                cJSON *cj_attributes_2 = cJSON_AddObjectToObject(__FUNCTION__, cj_resource, "attributes");
                if (cj_attributes_2)
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_attributes_2, "telemetry.sdk.langauge", "c");
                    cJSON_AddStringToObject(__FUNCTION__, cj_attributes_2, "telemetry.sdk.name", "opentelemetry");
                    cJSON_AddStringToObject(__FUNCTION__, cj_attributes_2, "telemetry.sdk.version", "0.1.0");
                    cJSON_AddStringToObject(__FUNCTION__, cj_attributes_2, "service.name", "ezlopi");
                    cJSON_AddStringToObject(__FUNCTION__, cj_attributes_2, "telemetry.auto.version", "0.18b0");
                }

                cJSON_AddStringToObject(__FUNCTION__, cj_attributes_2, "schema_url", "");
            }

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
                cJSON_AddStringToObject(__FUNCTION__, cj_attributes, "otelServiceName", "EzloPI");
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
#endif
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
    return ezlopi_service_otel_add_log_to_telemetry_queue(E_OTEL_TRACES, cj_traces);
}
