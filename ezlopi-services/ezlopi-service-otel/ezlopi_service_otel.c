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
#include "ezlopi_core_sntp.h"
#include "ezlopi_core_wifi.h"
#include "ezlopi_core_buffer.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_websocket_client.h"

#include "ezlopi_service_loop.h"
#include "ezlopi_service_otel.h"

#define OTEL_SPAN_ID_LEN 16
#define OTEL_TRACE_ID_LEN 32

typedef struct s_otel_log
{
    char *message;

    time_t time_stamp;
    uint32_t tick_count;

    const char *file_name;
    uint32_t line_no;
    e_trace_severity_t severity;

    s_otel_attr_t *attributes;

} s_otel_log_t;

typedef struct s_otel_queue_data
{
    e_otel_type_t type;
    union
    {
        s_otel_log_t *log_data;
        s_otel_trace_t *trace_data;
    } otel;
} s_otel_queue_data_t;

static QueueHandle_t __telemetry_queue = NULL;
static esp_websocket_client_handle_t __wss_client = NULL;
static const char *__ot_logs_endpoint = CONFIG_EZPI_OPENTELEMETRY_CLOUD_ENDPOINT;

static void __otel_task(void *pv);
static void __otel_publish(cJSON *cj_telemetry);
static void __connection_upcall(bool connected);
static void __fill_random_hexstring(char *buffer, uint32_t bytelen);

static void __otel_add_scope(cJSON *cj_root);
// static cJSON *__otel_create_attribute(cJSON *cj_attr);
// static cJSON *__otel_create_value(cJSON *cj_value_field);
static void __otel_add_resource(cJSON *cj_resourceLog);
static int __push_to_telemetry_queue(s_otel_queue_data_t *otel_data);

static cJSON *__otel_create_value_struct(s_otel_attr_t *attr);
static cJSON *__otel_create_attribute_struct(s_otel_attr_t *attr);

#ifdef CONFIG_EZPI_OPENTELEMETRY_ENABLE_TRACES
static cJSON *__otel_create_span_struct(s_otel_trace_t *otel_data);
static cJSON *__otel_trace_decorate_struct(s_otel_trace_t *otel_data);
#endif // CONFIG_EZPI_OPENTELEMETRY_ENABLE_TRACES

#ifdef CONFIG_EZPI_OPENTELEMETRY_ENABLE_LOGS
// static cJSON *__otel_logs_decorate(cJSON *cj_logs_info);
static cJSON *__otel_logs_decorate_struct(s_otel_log_t *log_data);

// static void __add_log_info(cJSON *cj_root, cJSON *cj_logs_info);
static void __add_log_info_struct(cJSON *cj_root, s_otel_log_t *log_data);

static void __otel_add_severity_number(cJSON *cj_root, e_trace_severity_t severity);
static int __otel_add_log_to_queue(uint8_t severity, const char *file, uint32_t line, char *log);
#endif // CONFIG_EZPI_OPENTELEMETRY_ENABLE_LOGS

static void __free_attributes(s_otel_attr_t *attr);
static void __free_telemetry_queue_data(s_otel_queue_data_t *otel_data);

#ifdef CONFIG_EZPI_OPENTELEMETRY_ENABLE_TRACES
char *ezlopi_service_otel_fetch_string_value_from_cjson(cJSON *cj_root, const char *key)
{
    char *ret = NULL;

    if (cj_root && key)
    {
        cJSON *cj_item = cJSON_GetObjectItem(__FUNCTION__, cj_root, key);
        if (cj_item && cj_item->valuestring && (cj_item->type == cJSON_String) && cj_item->str_value_len)
        {
            ret = ezlopi_malloc(__FUNCTION__, cj_item->str_value_len + 1);
            if (ret)
            {
                snprintf(ret, cj_item->str_value_len + 1, "%.*s", cj_item->str_value_len, cj_item->valuestring);
            }
        }
    }

    return ret;
}

int ezlopi_service_otel_add_trace_to_telemetry_queue(s_otel_trace_t *trace_obj)
{
    int ret = 0;

    if (trace_obj)
    {
        s_otel_queue_data_t *otel_data = ezlopi_malloc(__FUNCTION__, sizeof(s_otel_queue_data_t));
        if (otel_data)
        {
            otel_data->otel.trace_data = trace_obj;
            otel_data->type = E_OTEL_TRACES;

            ret = __push_to_telemetry_queue(otel_data);
            if (0 == ret)
            {
                ezlopi_free(__FUNCTION__, otel_data);
            }
        }
    }

    return ret;
}
#endif // CONFIG_EZPI_OPENTELEMETRY_ENABLE_TRACES

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
    if (EZPI_SUCCESS == EZPI_core_wait_for_wifi_to_connect(0))
    {
        s_otel_queue_data_t *otel_data = NULL;
        xQueueReceive(__telemetry_queue, &otel_data, 0);

        if (otel_data)
        {
            if (otel_data->otel.log_data || otel_data->otel.trace_data)
            {
                if (true == EZPI_core_websocket_client_is_connected(__wss_client))
                {
                    cJSON *cj_telemetry = NULL;

                    switch (otel_data->type)
                    {
#ifdef CONFIG_EZPI_OPENTELEMETRY_ENABLE_LOGS
                    case E_OTEL_LOGS:
                    {
                        cj_telemetry = __otel_logs_decorate_struct(otel_data->otel.log_data);
                        break;
                    }
#endif // CONFIG_EZPI_OPENTELEMETRY_ENABLE_LOGS

#ifdef CONFIG_EZPI_OPENTELEMETRY_ENABLE_TRACES
                    case E_OTEL_TRACES:
                    {
                        if (otel_data->otel.trace_data)
                        {
                            cj_telemetry = __otel_trace_decorate_struct(otel_data->otel.trace_data);
                        }

                        break;
                    }
#endif // CONFIG_EZPI_OPENTELEMETRY_ENABLE_TRACES

                    case E_OTEL_MATRICS:
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
            }

            __free_telemetry_queue_data(otel_data);
        }
    }
}

static void __otel_publish(cJSON *cj_telemetry)
{
    if (__wss_client && EZPI_core_websocket_client_is_connected(__wss_client) && cj_telemetry)
    {
        uint32_t buffer_len = 0;
        char *buffer = EZPI_core_buffer_acquire(__FUNCTION__, &buffer_len, 2000);
        if (buffer)
        {
            uint32_t rertries = 3;
            cJSON_PrintPreallocated(__FUNCTION__, cj_telemetry, buffer, buffer_len, false);

            while (rertries--)
            {
                if (EZPI_SUCCESS == EZPI_core_websocket_client_send(__wss_client, buffer, strlen(buffer), 1000))
                {
                    break;
                }

                vTaskDelay(10 / portTICK_RATE_MS);
            }

            EZPI_core_buffer_release(__FUNCTION__);
        }
    }
}

static void __otel_task(void *pv)
{
    EZPI_core_wait_for_wifi_to_connect(portTICK_RATE_MS);
    TRACE_D("Starting otel-service");

    cJSON *cjson_uri = cJSON_CreateString(__FUNCTION__, __ot_logs_endpoint);
    if (cjson_uri)
    {
        while (1)
        {
            __wss_client = EZPI_core_websocket_client_init(cjson_uri, NULL, __connection_upcall, NULL, NULL, NULL);
            if (NULL != __wss_client)
            {
                break;
            }

            vTaskDelay(1000 / portTICK_RATE_MS);
        }

        cJSON_Delete(__FUNCTION__, cjson_uri);
    }

    EZPI_service_loop_add("otel-loop", __otel_loop, 50, NULL);
    // EZPI_service_loop_add("otel-test-loop", __otel_test_loop, 5000, NULL);
    vTaskDelete(NULL);
}

static void __connection_upcall(bool connected)
{
    static int pre_status;

    if (connected)
    {
        if (0 == pre_status)
        {
            TRACE_S("WSS-connected!");
            printf("otel: WSS-connected!\r\n");
        }
        else
        {
            TRACE_S("WSS-reconnected!");
            printf("otel: WSS-reconnected!\r\n");
        }

        pre_status = 1;
    }
    else
    {
        TRACE_E("WSS-disconnected!");
        printf("otel: WSS-disconnected!\r\n");
        pre_status = -1;
    }
}

#ifdef CONFIG_EZPI_OPENTELEMETRY_ENABLE_TRACES
static cJSON *__otel_trace_decorate_struct(s_otel_trace_t *otel_data)
{
    cJSON *cj_telemetry = cJSON_CreateObject(__FUNCTION__);
    if (cj_telemetry)
    {
        cJSON_AddStringToObjectWithRef(__FUNCTION__, cj_telemetry, ezlopi_type_str, ezlopi_trace_str);
        cJSON *cj_request = cJSON_AddObjectToObject(__FUNCTION__, cj_telemetry, ezlopi_request_str);
        if (cj_request)
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
                                cJSON *cj_span = __otel_create_span_struct(otel_data);
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
        char *tmp_str = cJSON_Print(__FUNCTION__, cj_telemetry);
        if (tmp_str)
        {
            printf("otel-trace-struct:\r\n%s\r\n", tmp_str);
            ezlopi_free(__FUNCTION__, tmp_str);
        }
#endif
    }

    return cj_telemetry;
}

static cJSON *__otel_create_span_struct(s_otel_trace_t *otel_data)
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
        cJSON_AddNumberToObject(__FUNCTION__, cj_span, ezlopi_kind_str, otel_data->kind);

        if (otel_data->name)
        {
            cJSON_AddStringToObjectWithRef(__FUNCTION__, cj_span, ezlopi_name_str, otel_data->name);
        }

        {
            char tmp_buffer[32];
            snprintf(tmp_buffer, sizeof(tmp_buffer), "%llu", otel_data->start_time * 1000000000LLU);
            cJSON_AddStringToObject(__FUNCTION__, cj_span, ezlopi_startTimeUnixNano_str, tmp_buffer);

            snprintf(tmp_buffer, sizeof(tmp_buffer), "%llu", otel_data->end_time * 1000000000LLU);
            cJSON_AddStringToObject(__FUNCTION__, cj_span, ezlopi_endTimeUnixNano_str, tmp_buffer);
        }

        cJSON *cj_attributes = cJSON_AddArrayToObject(__FUNCTION__, cj_span, ezlopi_attributes_str);
        if (cj_attributes)
        {
            s_otel_attr_t *attr = otel_data->attributes;
            while (attr)
            {
                cJSON *cj_attribute = __otel_create_attribute_struct(attr);
                if (false == cJSON_AddItemToArray(cj_attributes, cj_attribute))
                {
                    cJSON_Delete(__FUNCTION__, cj_attribute);
                }

                attr = attr->next;
            }

            if (otel_data->method)
            {
                s_otel_attr_t tmp_attr;
                tmp_attr.key = ezlopi_method_str;
                tmp_attr.type = E_ATTR_TYPE_STRING;
                tmp_attr.value.string = otel_data->method;

                cJSON *cj_attribute = __otel_create_attribute_struct(&tmp_attr);
                if (false == cJSON_AddItemToArray(cj_attributes, cj_attribute))
                {
                    cJSON_Delete(__FUNCTION__, cj_attribute);
                }
            }

            if (otel_data->msg_subclass)
            {
                s_otel_attr_t tmp_attr;
                tmp_attr.key = ezlopi_msg_subclass_str;
                tmp_attr.type = E_ATTR_TYPE_STRING;
                tmp_attr.value.string = otel_data->msg_subclass;

                cJSON *cj_attribute = __otel_create_attribute_struct(&tmp_attr);
                if (false == cJSON_AddItemToArray(cj_attributes, cj_attribute))
                {
                    cJSON_Delete(__FUNCTION__, cj_attribute);
                }
            }

            if (otel_data->id)
            {
                s_otel_attr_t tmp_attr;
                tmp_attr.key = ezlopi_id_str;
                tmp_attr.type = E_ATTR_TYPE_STRING;
                tmp_attr.value.string = otel_data->id;

                cJSON *cj_attribute = __otel_create_attribute_struct(&tmp_attr);
                if (false == cJSON_AddItemToArray(cj_attributes, cj_attribute))
                {
                    cJSON_Delete(__FUNCTION__, cj_attribute);
                }
            }

            if (otel_data->error)
            {
                s_otel_attr_t tmp_attr;
                tmp_attr.key = ezlopi_error_str;
                tmp_attr.type = E_ATTR_TYPE_STRING;
                tmp_attr.value.string = otel_data->error;

                cJSON *cj_attribute = __otel_create_attribute_struct(&tmp_attr);
                if (false == cJSON_AddItemToArray(cj_attributes, cj_attribute))
                {
                    cJSON_Delete(__FUNCTION__, cj_attribute);
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

    if (true == EZPI_core_websocket_client_is_connected(__wss_client))
    {
        if (log)
        {
            s_otel_queue_data_t *otel_data = ezlopi_malloc(__FUNCTION__, sizeof(s_otel_queue_data_t));
            if (otel_data)
            {
                memset(otel_data, 0, sizeof(s_otel_queue_data_t));

                otel_data->otel.log_data = ezlopi_malloc(__FUNCTION__, sizeof(s_otel_log_t));
                if (otel_data->otel.log_data)
                {
                    memset(otel_data->otel.log_data, 0, sizeof(s_otel_log_t));

                    otel_data->type = E_OTEL_LOGS;
                    otel_data->otel.log_data->message = log;

#if 0
                    uint32_t message_len = strlen(log) + 1;
                    otel_data->otel.log_data->message = ezlopi_malloc(__FUNCTION__, message_len);
                    if (otel_data->otel.log_data->message)
                    {
                        snprintf(otel_data->otel.log_data->message, message_len, "%s", log);
                    }
#endif

                    otel_data->otel.log_data->line_no = line;
                    otel_data->otel.log_data->file_name = file;
                    otel_data->otel.log_data->severity = (e_trace_severity_t)severity;
                    otel_data->otel.log_data->tick_count = xTaskGetTickCount();
                    otel_data->otel.log_data->time_stamp = EZPI_core_sntp_get_current_time_sec();
                }

                ret = __push_to_telemetry_queue(otel_data);
                if (0 == ret)
                {
                    __free_telemetry_queue_data(otel_data);
                }
            }
        }
    }

    return ret;
}

static cJSON *__otel_logs_decorate_struct(s_otel_log_t *log_data)
{
    cJSON *cj_telemetry = cJSON_CreateObject(__FUNCTION__);
    if (cj_telemetry)
    {
        cJSON_AddStringToObjectWithRef(__FUNCTION__, cj_telemetry, ezlopi_type_str, ezlopi_log_str);
        cJSON *cj_request = cJSON_AddObjectToObjectWithRef(__FUNCTION__, cj_telemetry, ezlopi_request_str);
        if (cj_request)
        {
            cJSON *cj_resourceLogs = cJSON_AddArrayToObject(__FUNCTION__, cj_request, ezlopi_resourceLogs_str);
            if (cj_resourceLogs)
            {
                cJSON *cj_resourceLog = cJSON_CreateObject(__FUNCTION__);
                if (cj_resourceLog)
                {
                    // resource
                    __otel_add_resource(cj_resourceLog);

                    // scopeLogs
                    cJSON *cj_scopeLogs = cJSON_AddArrayToObject(__FUNCTION__, cj_resourceLog, ezlopi_scopeLogs_str);
                    if (cj_scopeLogs)
                    {
                        cJSON *cj_scopeLog = cJSON_CreateObject(__FUNCTION__);
                        if (cj_scopeLog)
                        {
                            // scope
                            __otel_add_scope(cj_scopeLog);

                            // logsRecords
                            cJSON *cj_logRecords = cJSON_AddArrayToObject(__FUNCTION__, cj_scopeLog, ezlopi_logRecords_str);
                            if (cj_logRecords)
                            {
                                cJSON *cj_logRecord = cJSON_CreateObject(__FUNCTION__);
                                if (cj_logRecord)
                                {
                                    __add_log_info_struct(cj_logRecord, log_data);
                                    // __add_log_info(cj_logRecord, cj_logs_info);

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
        char *_data_str = cJSON_Print(__FUNCTION__, cj_telemetry);
        if (_data_str)
        {
            printf("otel-logs:\r\n%s\r\n", _data_str);
            ezlopi_free(__FUNCTION__, _data_str);
        }
#endif
    }

    return cj_telemetry;
}

static void __add_log_info_struct(cJSON *cj_root, s_otel_log_t *log_data)
{
    char tmp_buffer[33];

    snprintf(tmp_buffer, sizeof(tmp_buffer), "%llu", EZPI_core_sntp_get_current_time_sec() * 1000000000llu + ((xTaskGetTickCount() - log_data->tick_count) / portTICK_RATE_MS) * 1000000llu);
    cJSON_AddStringToObject(__FUNCTION__, cj_root, ezlopi_timeUnixNano_str, tmp_buffer);

    snprintf(tmp_buffer, sizeof(tmp_buffer), "%llu", log_data->time_stamp * 1000000000llu);
    cJSON_AddStringToObject(__FUNCTION__, cj_root, ezlopi_observedTimeUnixNano_str, tmp_buffer);

    __otel_add_severity_number(cj_root, log_data->severity);

    __fill_random_hexstring(tmp_buffer, OTEL_TRACE_ID_LEN + 1); // +1 for terminating byte
    cJSON_AddStringToObject(__FUNCTION__, cj_root, ezlopi_traceId_str, tmp_buffer);

    __fill_random_hexstring(tmp_buffer, OTEL_SPAN_ID_LEN + 1); // +1 for terminating byte
    cJSON_AddStringToObject(__FUNCTION__, cj_root, ezlopi_spanId_str, tmp_buffer);

    cJSON *cj_body = cJSON_AddObjectToObject(__FUNCTION__, cj_root, ezlopi_body_str);
    if (cj_body)
    {
        cJSON_AddStringToObject(__FUNCTION__, cj_body, ezlopi_stringValue_str, log_data->message ? log_data->message : ezlopi__str);
    }

    if (log_data->attributes)
    {
        cJSON *cj_attributes = cJSON_AddArrayToObject(__FUNCTION__, cj_root, ezlopi_attributes_str);
        if (cj_attributes)
        {
            s_otel_attr_t *attr_node = log_data->attributes;
            while (attr_node)
            {
                cJSON *cj_attribute = __otel_create_attribute_struct(attr_node);
                if (false == cJSON_AddItemToArray(cj_attributes, cj_attribute))
                {
                    cJSON_Delete(__FUNCTION__, cj_attribute);
                }
                attr_node = attr_node->next;
            }
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
        cJSON_AddStringToObjectWithRef(__FUNCTION__, cj_scope, ezlopi_name_str, ezlopi_otel_c_str);
        cJSON_AddStringToObjectWithRef(__FUNCTION__, cj_scope, ezlopi_version_str, ezlopi_otel_version_str);
    }
}

static cJSON *__otel_create_value_struct(s_otel_attr_t *attr)
{
    cJSON *cj_value = cJSON_CreateObject(__FUNCTION__);
    if (cj_value)
    {
        switch (attr->type)
        {
        case E_ATTR_TYPE_BOOL:
        {
            cJSON_AddBoolToObject(__FUNCTION__, cj_value, ezlopi_boolValue_str, attr->value.number ? true : false);
            break;
        }
        case E_ATTR_TYPE_STRING:
        {
            cJSON_AddStringToObject(__FUNCTION__, cj_value, ezlopi_stringValue_str, attr->value.string ? attr->value.string : ezlopi__str);
            break;
        }
        case E_ATTR_TYPE_STRING_CONST:
        {
            cJSON_AddStringToObjectWithRef(__FUNCTION__, cj_value, ezlopi_stringValue_str, attr->value.string ? attr->value.string : ezlopi__str);
            break;
        }
        case E_ATTR_TYPE_NUMBER:
        {
            cJSON_AddNumberToObjectWithRef(__FUNCTION__, cj_value, ezlopi_doubleValue_str, attr->value.number);
            break;
        }
        default:
        {
            cJSON_Delete(__FUNCTION__, cj_value);
            cj_value = NULL;
            break;
        }
        }
    }
    return cj_value;
}

static cJSON *__otel_create_attribute_struct(s_otel_attr_t *attr)
{
    cJSON *cj_attribute = NULL;
    if (attr->key)
    {
        cj_attribute = cJSON_CreateObject(__FUNCTION__);
        if (cj_attribute)
        {
            cJSON *cj_attr_value = __otel_create_value_struct(attr);
            if (false == cJSON_AddItemToObject(__FUNCTION__, cj_attribute, ezlopi_value_str, cj_attr_value))
            {
                cJSON_Delete(__FUNCTION__, cj_attr_value);
            }
            else
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_attribute, ezlopi_key_str, attr->key ? attr->key : ezlopi_null_str);
            }
        }
    }
    return cj_attribute;
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
                uint64_t serial = EZPI_core_factory_info_v3_get_id();
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
            __free_telemetry_queue_data(dump_telemetry);
        }

        if (pdTRUE == xQueueSend(__telemetry_queue, &otel_data, 500 / portTICK_RATE_MS))
        {
            ret = 1;
        }
    }

    return ret;
}

static void __free_attributes(s_otel_attr_t *attr)
{
    s_otel_attr_t *next = NULL;
    while (attr)
    {
        next = attr->next;
        if (attr->type == E_ATTR_TYPE_STRING)
        {
            ezlopi_free(__FUNCTION__, attr->value.string);
        }
        ezlopi_free(__FUNCTION__, attr);
        attr = next;
    }
}

static void __free_telemetry_queue_data(s_otel_queue_data_t *otel_data)
{
    if (otel_data)
    {
        switch (otel_data->type)
        {
        case E_OTEL_LOGS:
        {
            if (otel_data->otel.log_data)
            {
                ezlopi_free(__FUNCTION__, otel_data->otel.log_data->message);
                __free_attributes(otel_data->otel.log_data->attributes);
                ezlopi_free(__FUNCTION__, otel_data->otel.log_data);
            }
            break;
        }
        case E_OTEL_TRACES:
        {
            if (otel_data->otel.trace_data)
            {
                ezlopi_free(__FUNCTION__, otel_data->otel.trace_data->id);
                ezlopi_free(__FUNCTION__, otel_data->otel.trace_data->error);
                ezlopi_free(__FUNCTION__, otel_data->otel.trace_data->method);
                ezlopi_free(__FUNCTION__, otel_data->otel.trace_data->msg_subclass);
                __free_attributes(otel_data->otel.trace_data->attributes);
                ezlopi_free(__FUNCTION__, otel_data->otel.trace_data);
            }
            break;
        }
        case E_OTEL_MATRICS:
        default:
        {
            break;
        }
        }

        ezlopi_free(__FUNCTION__, otel_data);
    }
}

#endif // CONFIG_EZPI_ENABLE_OPENTELEMETRY
