#include <esp_mac.h>
#include <esp_random.h>
#include <esp_system.h>
#include <bootloader_random.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_websocket_client.h>

#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_ENABLE_OPENTELEMETRY

#include "ezlopi_cloud_constants.h"

#include "ezlopi_core_log.h"
#include "ezlopi_core_wifi.h"
#include "ezlopi_core_buffer.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_websocket_client.h"

#include "ezlopi_service_loop.h"
#include "ezlopi_service_otel.h"

#define OTEL_SPAN_ID_LEN 16
#define OTEL_TRACE_ID_LEN 32

typedef struct s_otel_queue_data
{
    cJSON *cj_data;
    e_otel_type_t type;
} s_otel_queue_data_t;

static QueueHandle_t __telemetry_queue = NULL;
static esp_websocket_client_handle_t __wss_client = NULL;
static const char *__ot_logs_endpoint = CONFIG_EZPI_OPENTELEMETRY_CLOUD_ENDPOINT;

static void __otel_task(void *pv);
static void __otel_publish(cJSON *cj_telemetry);
static void __connection_upcall(bool connected);
static void __fill_random_hexstring(char *buffer, uint32_t bytelen);
static int __message_upcall(char *payload, uint32_t len, time_t time_sec);

static void __otel_add_scope(cJSON *cj_root);
static cJSON *__otel_create_attribute(cJSON *cj_attr);
static cJSON *__otel_create_value(cJSON *cj_value_field);
static void __otel_add_time_stamp_nano(cJSON *cj_root, const char *add_key, const char *for_key, cJSON *cj_trace_info);
static void __cjson_detach_and_add(cJSON *cj_destination, const char *add_key, const char *for_key_str, cJSON *cj_source);
static void __otel_add_resource(cJSON *cj_resourceLog);
static int __push_to_telemetry_queue(s_otel_queue_data_t *otel_data);

#ifdef CONFIG_EZPI_OPENTELEMETRY_ENABLE_TRACES
static cJSON *__otel_create_span(cJSON *cj_trace_info);
static cJSON *__otel_trace_decorate(cJSON *cj_traces_info);
#endif // CONFIG_EZPI_OPENTELEMETRY_ENABLE_TRACES

#ifdef CONFIG_EZPI_OPENTELEMETRY_ENABLE_LOGS
static cJSON *__otel_logs_decorate(cJSON *cj_logs_info);
static void __add_log_info(cJSON *cj_root, cJSON *cj_logs_info);
static void __otel_add_severity_number(cJSON *cj_root, e_trace_severity_t severity);
static int __otel_add_log_to_queue(uint8_t severity, const char *file, uint32_t line, char *log);
#endif // CONFIG_EZPI_OPENTELEMETRY_ENABLE_LOGS

int ezlopi_service_otel_add_trace_to_telemetry_queue(cJSON *cj_trace)
{
    int ret = 0;

#if 1
    if (cj_trace)
    {
        s_otel_queue_data_t *otel_data = ezlopi_malloc(__FUNCTION__, sizeof(s_otel_queue_data_t));
        if (otel_data)
        {
            otel_data->cj_data = cj_trace;
            otel_data->type = E_OTEL_TRACES;
            ret = __push_to_telemetry_queue(otel_data);
            if (0 == ret)
            {
                ezlopi_free(__FUNCTION__, otel_data);
            }
        }
    }
#endif

    return ret;
}

void ezlopi_service_otel_init(void)
{
    bootloader_random_enable();
#ifdef CONFIG_EZPI_OPENTELEMETRY_ENABLE_LOGS
    ezlopi_util_set_otel_log_upcall(__otel_add_log_to_queue, 100);
#endif // CONFIG_EZPI_OPENTELEMETRY_ENABLE_LOGS
    __telemetry_queue = xQueueCreate(10, sizeof(s_otel_queue_data_t *));
    xTaskCreate(__otel_task, "otel-service-task", 2 * 2048, NULL, 4, NULL);
}

static void __otel_loop(void *pv)
{
    if (EZPI_SUCCESS == ezlopi_wait_for_wifi_to_connect(0))
    {
        if (true == ezlopi_websocket_client_is_connected(__wss_client))
        {
            s_otel_queue_data_t *otel_data = NULL;
            xQueueReceive(__telemetry_queue, &otel_data, 0);

            if (otel_data)
            {
                if (otel_data->cj_data)
                {
                    if (true == ezlopi_websocket_client_is_connected(__wss_client))
                    {
                        cJSON *cj_telemetry = NULL;

                        switch (otel_data->type)
                        {
#ifdef CONFIG_EZPI_OPENTELEMETRY_ENABLE_LOGS
                        case E_OTEL_LOGS:
                        {
                            cj_telemetry = __otel_logs_decorate(otel_data->cj_data);
                            break;
                        }
#endif // CONFIG_EZPI_OPENTELEMETRY_ENABLE_LOGS

#ifdef CONFIG_EZPI_OPENTELEMETRY_ENABLE_TRACES
                        case E_OTEL_TRACES:
                        {
                            cj_telemetry = __otel_trace_decorate(otel_data->cj_data);
                            break;
                        }
#endif // CONFIG_EZPI_OPENTELEMETRY_ENABLE_TRACES

                        case E_OTEL_MATRICS:
                        {
                            break;
                        }
                        default:
                        {
                            break;
                        }
                        }

                        if (cj_telemetry)
                        {
                            __otel_publish(cj_telemetry);
                            cJSON_Delete(__FUNCTION__, cj_telemetry);
                        }
                    }

                    cJSON_Delete(__FUNCTION__, otel_data->cj_data);
                }

                ezlopi_free(__FUNCTION__, otel_data);
            }
        }
    }
}

static void __otel_publish(cJSON *cj_telemetry)
{
    if (__wss_client && ezlopi_websocket_client_is_connected(__wss_client) && cj_telemetry)
    {
        uint32_t buffer_len = 0;
        char *buffer = ezlopi_core_buffer_acquire(__FUNCTION__, &buffer_len, 2000);
        if (buffer)
        {
            uint32_t rertries = 3;
            cJSON_PrintPreallocated(__FUNCTION__, cj_telemetry, buffer, buffer_len, false);

            while (rertries--)
            {
                if (EZPI_SUCCESS == ezlopi_websocket_client_send(__wss_client, buffer, strlen(buffer), 1000))
                {
                    break;
                }

                vTaskDelay(10 / portTICK_RATE_MS);
            }

            ezlopi_core_buffer_release(__FUNCTION__);
        }
    }
}

static void __otel_task(void *pv)
{
    ezlopi_wait_for_wifi_to_connect(portTICK_RATE_MS);
    TRACE_D("Starting otel-service");

    cJSON *cjson_uri = cJSON_CreateString(__FUNCTION__, __ot_logs_endpoint);
    if (cjson_uri)
    {
        TRACE_D("uri: %.*s", cjson_uri->str_value_len, cjson_uri->valuestring);

        while (1)
        {
            __wss_client = ezlopi_websocket_client_init(cjson_uri, __message_upcall, __connection_upcall,
                                                        NULL, NULL, NULL);
            if (NULL != __wss_client)
            {
                break;
            }

            vTaskDelay(1000 / portTICK_RATE_MS);
        }

        cJSON_Delete(__FUNCTION__, cjson_uri);
    }

    ezlopi_service_loop_add("otel-loop", __otel_loop, 50, NULL);
    // ezlopi_service_loop_add("otel-test-loop", __otel_test_loop, 5000, NULL);
    vTaskDelete(NULL);
}

static int __message_upcall(char *payload, uint32_t len, time_t time_sec)
{
    int ret = 0;
#if 0
    if (payload && len)
    {
        printf("otel-response:: %lu -> wss-payload: %.*s\r\n", time_sec, len, payload);
        ezlopi_free(__FUNCTION__, payload);
        ret = 1;
    }
#endif
    return ret;
}

static void __connection_upcall(bool connected)
{
    static int pre_status;

    if (connected)
    {
        if (0 == pre_status)
        {
            TRACE_S("WSS-connected!");
        }
        else
        {
            TRACE_S("WSS-reconnected!");
        }

        pre_status = 1;
    }
    else
    {
        TRACE_E("WSS-disconnected!");
        pre_status = -1;
    }
}

#ifdef CONFIG_EZPI_OPENTELEMETRY_ENABLE_TRACES
static cJSON *__otel_trace_decorate(cJSON *cj_traces_info)
{
    cJSON *cj_traceRecord = cJSON_CreateObject(__FUNCTION__);
    if (cj_traceRecord)
    {
        cJSON_AddStringToObject(__FUNCTION__, cj_traceRecord, "type", "trace");
        cJSON *cj_request = cJSON_AddObjectToObject(__FUNCTION__, cj_traceRecord, "request");
        {
            cJSON *cj_resourceSpans = cJSON_AddArrayToObject(__FUNCTION__, cj_request, ezlopi_resourceSpans_str);
            if (cj_resourceSpans)
            {
                cJSON *cj_resourceSpan = cJSON_CreateObject(__FUNCTION__);
                if (cj_resourceSpan)
                {
                    __otel_add_resource(cj_resourceSpan);

                    cJSON *cj_scopeSpans = cJSON_AddArrayToObject(__FUNCTION__, cj_resourceSpan, ezlopi_scopeSpans_str);
                    if (cj_scopeSpans)
                    {
                        cJSON *cj_scopeSpan = cJSON_CreateObject(__FUNCTION__);
                        if (cj_scopeSpan)
                        {
                            __otel_add_scope(cj_scopeSpan);

                            cJSON *cj_spans = cJSON_AddArrayToObject(__FUNCTION__, cj_scopeSpan, ezlopi_spans_str);
                            if (cj_spans)
                            {
                                cJSON *cj_span = __otel_create_span(cj_traces_info);
                                if (cj_span)
                                {
                                    if (false == cJSON_AddItemToArray(cj_spans, cj_span))
                                    {
                                        cJSON_Delete(__FUNCTION__, cj_span);
                                    }
                                }
                            }

                            if (false == cJSON_AddItemToArray(cj_scopeSpans, cj_scopeSpan))
                            {
                                cJSON_Delete(__FUNCTION__, cj_scopeSpan);
                            }
                        }
                    }

                    if (false == cJSON_AddItemToArray(cj_resourceSpans, cj_resourceSpan))
                    {
                        cJSON_Delete(__FUNCTION__, cj_resourceSpan);
                    }
                }
            }
        }
#if 0
        char* _data_str = cJSON_Print(__FUNCTION__, cj_traceRecord);
        // cJSON_Delete(__FUNCTION__, cj_traceRecord);
        // cj_traceRecord = NULL;

        if (_data_str)
        {
            printf("otel-trace:\r\n%s\r\n", _data_str);
            ezlopi_free(__FUNCTION__, _data_str);
        }
#endif
    }

    return cj_traceRecord;
}

static cJSON *__otel_create_span(cJSON *cj_trace_info)
{
    cJSON *cj_span = cJSON_CreateObject(__FUNCTION__);
    if (cj_span)
    {
        char unique_id[33];

        __fill_random_hexstring(unique_id, OTEL_TRACE_ID_LEN + 1); // +
        cJSON_AddStringToObject(__FUNCTION__, cj_span, ezlopi_traceId_str, unique_id);

        __fill_random_hexstring(unique_id, OTEL_SPAN_ID_LEN + 1); //  +1 for zero termination
        cJSON_AddStringToObject(__FUNCTION__, cj_span, ezlopi_spanId_str, unique_id);

        cJSON_AddStringToObject(__FUNCTION__, cj_span, ezlopi_parentSpanId_str, "");

        __cjson_detach_and_add(cj_span, ezlopi_name_str, ezlopi_name_str, cj_trace_info);
        __cjson_detach_and_add(cj_span, ezlopi_kind_str, ezlopi_kind_str, cj_trace_info);

        __otel_add_time_stamp_nano(cj_span, ezlopi_startTimeUnixNano_str, ezlopi_startTime_str, cj_trace_info);
        __otel_add_time_stamp_nano(cj_span, ezlopi_endTimeUnixNano_str, ezlopi_endTime_str, cj_trace_info);

        cJSON *cj_attributes = cJSON_AddArrayToObject(__FUNCTION__, cj_span, ezlopi_attributes_str);
        if (cj_attributes)
        {
            if (cj_trace_info)
            {
                cJSON *cj_attr = cj_trace_info->child;
                while (cj_attr)
                {
                    // printf("%.*s: %.*s\r\n", cj_attr->str_key_len, cj_attr->string, cj_attr->str_value_len, cj_attr->valuestring);
                    cJSON *cj_attribute = __otel_create_attribute(cj_attr);
                    if (false == cJSON_AddItemToArray(cj_attributes, cj_attribute))
                    {
                        cJSON_Delete(__FUNCTION__, cj_attribute);
                    }
                    cj_attr = cj_attr->next;
                }
            }
        }
    }

    return cj_span;
}
#endif // CONFIG_EZPI_OPENTELEMETRY_ENABLE_TRACES

#ifdef CONFIG_EZPI_OPENTELEMETRY_ENABLE_LOGS
static int __otel_add_log_to_queue(uint8_t severity, const char *file, uint32_t line, char *log)
{
    int ret = 0;

    s_otel_queue_data_t *otel_data = ezlopi_malloc(__FUNCTION__, sizeof(s_otel_queue_data_t));
    if (otel_data)
    {
        otel_data->type = E_OTEL_LOGS;

        otel_data->cj_data = cJSON_CreateObject(__FUNCTION__);
        if (otel_data->cj_data)
        {
            cJSON_AddNumberToObject(__FUNCTION__, otel_data->cj_data, ezlopi_logTime_str, EZPI_CORE_sntp_get_current_time_sec());
            __otel_add_severity_number(otel_data->cj_data, (e_trace_severity_t)severity);
            cJSON_AddStringToObject(__FUNCTION__, otel_data->cj_data, ezlopi_message_str, log);
            cJSON_AddStringToObject(__FUNCTION__, otel_data->cj_data, ezlopi_fileName_str, file);
            cJSON_AddNumberToObject(__FUNCTION__, otel_data->cj_data, ezlopi_lineNumber_str, line);
        }

        ret = __push_to_telemetry_queue(otel_data);
        if (0 == ret)
        {
            ezlopi_free(__FUNCTION__, otel_data);
        }
    }

    return ret;
}

static cJSON *__otel_logs_decorate(cJSON *cj_logs_info)
{
    cJSON *cj_logs_telemetry = cJSON_CreateObject(__FUNCTION__);
    if (cj_logs_telemetry)
    {
        cJSON_AddStringToObject(__FUNCTION__, cj_logs_telemetry, "type", "log");
        cJSON *cj_request = cJSON_AddObjectToObject(__FUNCTION__, cj_logs_telemetry, "request");
        if (cj_request)
        {
            cJSON *cj_resourceLogs = cJSON_AddArrayToObject(__FUNCTION__, cj_request, "resourceLogs");
            if (cj_resourceLogs)
            {
                cJSON *cj_resourceLog = cJSON_CreateObject(__FUNCTION__);
                if (cj_resourceLog)
                {
                    // resource
                    __otel_add_resource(cj_resourceLog);

                    // scopeLogs
                    cJSON *cj_scopeLogs = cJSON_AddArrayToObject(__FUNCTION__, cj_resourceLog, "scopeLogs");
                    if (cj_scopeLogs)
                    {
                        cJSON *cj_scopeLog = cJSON_CreateObject(__FUNCTION__);
                        if (cj_scopeLog)
                        {
                            // scope
                            __otel_add_scope(cj_scopeLog);

                            // logsRecords
                            cJSON *cj_logRecords = cJSON_AddArrayToObject(__FUNCTION__, cj_scopeLog, "logRecords");
                            if (cj_logRecords)
                            {
                                cJSON *cj_logRecord = cJSON_CreateObject(__FUNCTION__);
                                if (cj_logRecord)
                                {
                                    __add_log_info(cj_logRecord, cj_logs_info);

                                    if (false == cJSON_AddItemToArray(cj_logRecords, cj_logRecord))
                                    {
                                        cJSON_Delete(__FUNCTION__, cj_logRecord);
                                    }
                                }
                            }

                            if (false == cJSON_AddItemToArray(cj_scopeLogs, cj_scopeLog))
                            {
                                cJSON_Delete(__FUNCTION__, cj_scopeLog);
                            }
                        }
                    }

                    if (false == cJSON_AddItemToArray(cj_resourceLogs, cj_resourceLog))
                    {
                        cJSON_Delete(__FUNCTION__, cj_resourceLog);
                    }
                }
            }
        }

#if 0
        char* _data_str = cJSON_Print(__FUNCTION__, cj_logs_telemetry);
        // cJSON_Delete(__FUNCTION__, cj_logs_telemetry);
        // cj_logs_telemetry = NULL;

        if (_data_str)
        {
            printf("otel-logs:\r\n%s\r\n", _data_str);
            ezlopi_free(__FUNCTION__, _data_str);
        }
#endif
    }

    return cj_logs_telemetry;
}

static void __add_log_info(cJSON *cj_root, cJSON *cj_logs_info)
{
    char tmp_buffer[33];

    snprintf(tmp_buffer, sizeof(tmp_buffer), "%llu", EZPI_CORE_sntp_get_current_time_sec() * 1000000000llu);
    cJSON_AddStringToObject(__FUNCTION__, cj_root, "timeUnixNano", tmp_buffer);

    __otel_add_time_stamp_nano(cj_root, "observedTimeUnixNano", ezlopi_logTime_str, cj_logs_info);
    __cjson_detach_and_add(cj_root, "severityNumber", "severityNumber", cj_logs_info);
    __cjson_detach_and_add(cj_root, "severityText", "severityText", cj_logs_info);

    __fill_random_hexstring(tmp_buffer, OTEL_TRACE_ID_LEN + 1); // +1 for terminating byte
    cJSON_AddStringToObject(__FUNCTION__, cj_root, ezlopi_traceId_str, tmp_buffer);

    __fill_random_hexstring(tmp_buffer, OTEL_SPAN_ID_LEN + 1); // +1 for terminating byte
    cJSON_AddStringToObject(__FUNCTION__, cj_root, ezlopi_spanId_str, tmp_buffer);

    cJSON *cj_body = cJSON_AddObjectToObject(__FUNCTION__, cj_root, "body");
    if (cj_body)
    {
        __cjson_detach_and_add(cj_body, "stringValue", "message", cj_logs_info);
    }

    cJSON *cj_attributes = cJSON_AddArrayToObject(__FUNCTION__, cj_root, ezlopi_attributes_str);
    if (cj_attributes)
    {
        cJSON *cj_attr = cj_logs_info->child;
        while (cj_attr)
        {
            cJSON *cj_attribute = __otel_create_attribute(cj_attr);
            if (false == cJSON_AddItemToArray(cj_attributes, cj_attribute))
            {
                cJSON_Delete(__FUNCTION__, cj_attribute);
            }
            cj_attr = cj_attr->next;
        }
    }
}

static void __otel_add_severity_number(cJSON *cj_root, e_trace_severity_t severity)
{
    double severity_number = 0;
    const char *severity_text = ezlopi__str;

    switch (severity)
    {
    case ENUM_EZLOPI_TRACE_SEVERITY_ERROR:
    {
        severity_number = 17;
        severity_text = ezlopi_ERROR_str;
        break;
    }
    case ENUM_EZLOPI_TRACE_SEVERITY_WARNING:
    {
        severity_number = 13;
        severity_text = ezlopi_WARN_str;
        break;
    }
    case ENUM_EZLOPI_TRACE_SEVERITY_INFO:
    {
        severity_number = 9;
        severity_text = ezlopi_INFO_str;
        break;
    }
    case ENUM_EZLOPI_TRACE_SEVERITY_DEBUG:
    {
        severity_number = 5;
        severity_text = ezlopi_DEBUG_str;
        break;
    }
    case ENUM_EZLOPI_TRACE_SEVERITY_TRACE:
    default:
    {
        severity_number = 1;
        severity_text = ezlopi_TRACE_str;
        break;
    }
    }

    cJSON_AddNumberToObjectWithRef(__FUNCTION__, cj_root, ezlopi_severityNumber_str, severity_number);
    cJSON_AddStringToObjectWithRef(__FUNCTION__, cj_root, ezlopi_severityText_str, severity_text);
}
#endif // CONFIG_EZPI_OPENTELEMETRY_ENABLE_LOGS

static void __otel_add_scope(cJSON *cj_root)
{
    cJSON *cj_scope = cJSON_AddObjectToObject(__FUNCTION__, cj_root, ezlopi_scope_str);
    if (cj_scope)
    {
        cJSON_AddStringToObject(__FUNCTION__, cj_scope, "name", "otel-c");
        cJSON_AddStringToObject(__FUNCTION__, cj_scope, "version", "1.0.0");
    }
}

static cJSON *__otel_create_value(cJSON *cj_value_field)
{
    cJSON *cj_ret_value = cJSON_CreateObject(__FUNCTION__);

    if (cj_value_field)
    {
        switch (cj_value_field->type)
        {
        case cJSON_False:
        {
            cJSON_AddFalseToObject(__FUNCTION__, cj_ret_value, ezlopi_boolValue_str);
            break;
        }
        case cJSON_True:
        {
            cJSON_AddTrueToObject(__FUNCTION__, cj_ret_value, ezlopi_boolValue_str);
            break;
        }
        case cJSON_Number:
        {
            cJSON_AddNumberToObject(__FUNCTION__, cj_ret_value, ezlopi_doubleValue_str, cj_value_field->valuedouble);
            break;
        }
        case cJSON_String:
        {
            cJSON_AddStringToObject(__FUNCTION__, cj_ret_value, ezlopi_stringValue_str, cj_value_field->valuestring ? cj_value_field->valuestring : ezlopi_null_str);
            break;
        }
        case cJSON_Array:
        {
            cJSON *cj_array_value_obj = cJSON_AddObjectToObject(__FUNCTION__, cj_ret_value, ezlopi_arrayValue_str);
            if (cj_array_value_obj)
            {
                cJSON *cj_values = cJSON_AddArrayToObject(__FUNCTION__, cj_array_value_obj, ezlopi_values_str);
                if (cj_values)
                {
                    int array_size = cJSON_GetArraySize(cj_value_field);
                    for (int i = 0; i < array_size; i++)
                    {
                        cJSON *cj_element = cJSON_GetArrayItem(cj_value_field, i);
                        if (cj_element)
                        {
                            cJSON *cj_element_value = __otel_create_value(cj_element);
                            if (cj_element_value)
                            {
                                if (false == cJSON_AddItemToArray(cj_values, cj_element_value))
                                {
                                    cJSON_Delete(__FUNCTION__, cj_element_value);
                                }
                            }
                        }
                    }
                }
            }

            break;
        }
        default:
        {
            break;
        }
        }
    }

    return cj_ret_value;
}

static cJSON *__otel_create_attribute(cJSON *cj_attr)
{
    cJSON *cj_attribute = NULL;
    if (cj_attr)
    {
        cj_attribute = cJSON_CreateObject(__FUNCTION__);
        if (cj_attribute)
        {
            cJSON *cj_attr_value = __otel_create_value(cj_attr);
            if (false == cJSON_AddItemToObject(__FUNCTION__, cj_attribute, ezlopi_value_str, cj_attr_value))
            {
                cJSON_Delete(__FUNCTION__, cj_attr_value);
            }
            else
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_attribute, ezlopi_key_str, cj_attr->string ? cj_attr->string : ezlopi_null_str);
            }
        }
    }
    return cj_attribute;
}

static void __otel_add_time_stamp_nano(cJSON *cj_root, const char *add_key, const char *for_key, cJSON *cj_trace_info)
{
    cJSON *cj_time_stamp = cJSON_DetachItemFromObject(__FUNCTION__, cj_trace_info, for_key);
    if (cj_time_stamp)
    {
        char *method = "null";
        cJSON *cj_method = cJSON_GetObjectItem(__FUNCTION__, cj_trace_info, ezlopi_method_str);
        if (cj_method)
        {
            method = cj_method->valuestring ? cj_method->valuestring : "null";
        }

        printf("-----> %s [%s]: %llu\r\n", method, for_key, (uint64_t)cj_time_stamp->valuedouble);

        char tmp_buffer[32];
        snprintf(tmp_buffer, sizeof(tmp_buffer), "%llu", (uint64_t)(cj_time_stamp->valuedouble * 1000000000llu));
        cJSON_AddStringToObject(__FUNCTION__, cj_root, add_key, tmp_buffer);
        cJSON_Delete(__FUNCTION__, cj_time_stamp);
    }
}

static void __cjson_detach_and_add(cJSON *cj_destination, const char *add_key, const char *for_key_str, cJSON *cj_source)
{
    if (cj_destination && cj_source && for_key_str && add_key)
    {
        cJSON *cj_message = cJSON_DetachItemFromObject(__FUNCTION__, cj_source, for_key_str);
        if (cj_message)
        {
            if (false == cJSON_AddItemToObject(__FUNCTION__, cj_destination, add_key, cj_message))
            {
                cJSON_Delete(__FUNCTION__, cj_message);
            }
        }
    }
}

static void __otel_add_resource_attr(cJSON *cj_root, const char *key, const char *value, bool is_const)
{
    if (cj_root && key && value)
    {
        cJSON *cj_attribute = cJSON_CreateObject(__FUNCTION__);
        if (cj_attribute)
        {
            cJSON_AddStringToObject(__FUNCTION__, cj_attribute, ezlopi_key_str, key);
            cJSON *cj_value = cJSON_AddObjectToObject(__FUNCTION__, cj_attribute, ezlopi_value_str);
            if (cj_value)
            {
                if (is_const)
                {
                    cJSON_AddStringToObjectWithRef(__FUNCTION__, cj_value, ezlopi_stringValue_str, value);
                }
                else
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_value, ezlopi_stringValue_str, value);
                }
            }

            if (false == cJSON_AddItemToArray(cj_root, cj_attribute))
            {
                cJSON_Delete(__FUNCTION__, cj_attribute);
            }
        }
    }
}

static void __otel_add_resource(cJSON *cj_resourceLog)
{
    cJSON *cj_resource = cJSON_AddObjectToObject(__FUNCTION__, cj_resourceLog, ezlopi_resource_str);
    if (cj_resource)
    {
        cJSON *cj_attributes = cJSON_AddArrayToObject(__FUNCTION__, cj_resource, ezlopi_attributes_str);
        if (cj_attributes)
        {
            char tmp_buffer[24];

            __otel_add_resource_attr(cj_attributes, ezlopI_service___name_str, ezlopi_EzloPI_str, true);

            {
                uint64_t serial = ezlopi_factory_info_v3_get_id();
                snprintf(tmp_buffer, sizeof(tmp_buffer), "%llu", serial);
                __otel_add_resource_attr(cj_attributes, ezlopi_serial_str, tmp_buffer, false);
            }

            {
                unsigned char mac_base[6] = {0};
                esp_efuse_mac_get_default(mac_base);
                snprintf(tmp_buffer, sizeof(tmp_buffer), "%2X%2X%2X%2X%2X%2X", mac_base[0], mac_base[1], mac_base[2], mac_base[3], mac_base[4], mac_base[5]);
                __otel_add_resource_attr(cj_attributes, ezlopi_base___mac_str, tmp_buffer, false);
            }
        }
    }
}

static void __fill_random_hexstring(char *buffer, uint32_t bytelen)
{
    if (buffer && bytelen)
    {
        for (int i = 0; i < (bytelen - 1); i += 8)
        {
            uint32_t rnd_num = 0;
            bootloader_fill_random(&rnd_num, sizeof(uint32_t));
            snprintf(buffer + i, (bytelen - i), "%08X", rnd_num);
        }
    }
}

static int __push_to_telemetry_queue(s_otel_queue_data_t *otel_data)
{
    int ret = 0;
    if (__telemetry_queue)
    {
        if (pdTRUE == xQueueIsQueueFullFromISR(__telemetry_queue))
        {
            s_otel_queue_data_t *dump_telemetry = NULL;
            xQueueReceive(__telemetry_queue, &dump_telemetry, 0);
            if (dump_telemetry)
            {
                cJSON_Delete(__FUNCTION__, dump_telemetry->cj_data);
                ezlopi_free(__FUNCTION__, dump_telemetry);
            }
        }

        if (pdTRUE == xQueueSend(__telemetry_queue, &otel_data, 500 / portTICK_RATE_MS))
        {
            ret = 1;
        }
    }
    return ret;
}

#endif // CONFIG_EZPI_ENABLE_OPENTELEMETRY
