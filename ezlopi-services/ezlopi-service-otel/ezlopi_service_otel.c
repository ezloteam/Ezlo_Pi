#include <esp_random.h>
#include <bootloader_random.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_websocket_client.h>

#include "ezlopi_cloud_constants.h"

#include "ezlopi_core_wsc.h"
#include "ezlopi_core_wifi.h"
#include "ezlopi_core_buffer.h"
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
static const char *__ot_logs_endpoint = "wss://ot.review-staging-op-owkix8.ewr4.opentelemetry.ezlo.com/traces";

static void __otel_task(void *pv);
static void __otel_publish(cJSON *cj_telemetry);
static void __connection_upcall(bool connected);
static void __fill_random_hexstring(char *buffer, uint32_t bytelen);
static int __message_upcall(char *payload, uint32_t len, time_t time_sec);

static cJSON *__otel_create_scope(void);
static cJSON *__otel_create_value(cJSON *cj_value_field);
static cJSON *__otel_create_attribute(cJSON *cj_attr);
static void __otel_add_time_stamp_nano(cJSON *cj_span, const char *add_key, const char *for_key, cJSON *cj_trace_info);
static void __cjson_copy_and_delete(cJSON *cj_haystack, const char *key_str, cJSON *cj_needle);
static cJSON *__otel_create_span(cJSON *cj_trace_info);
static cJSON *__otel_create_resource(void);
static cJSON *__otel_trace_decorate(cJSON *cj_traces_info);
static void __otel_logs_decorate(cJSON *cj_logs);
static int __push_to_telemetry_queue(s_otel_queue_data_t *otel_data);

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

int ezlopi_service_otel_add_trace_to_telemetry_queue(cJSON *cj_trace)
{
    int ret = 0;
    if (cj_trace)
    {
        s_otel_queue_data_t *otel_data = ezlopi_malloc(__FUNCTION__, sizeof(s_otel_queue_data_t));
        if (otel_data)
        {
            otel_data->cj_data = cj_trace;
            otel_data->type = E_OTEL_TRACES;
            ret = __push_to_telemetry_queue(otel_data);

#if 0
            if (pdTRUE == xQueueIsQueueFullFromISR(__telemetry_queue))
            {
                s_otel_queue_data_t *dump_telemetry = NULL;
                xQueueReceive(__telemetry_queue, &dump_telemetry, 0);
                if (dump_telemetry)
                {
                    ezlopi_free(__FUNCTION__, dump_telemetry);
                }
            }

            if (pdTRUE == xQueueSend(__telemetry_queue, &otel_data, 500 / portTICK_RATE_MS))
            {
                ret = 1;
            }
            else
            {
                ezlopi_free(__FUNCTION__, otel_data);
            }
#endif
        }
    }

    return ret;
}

void ezlopi_service_otel_init(void)
{
    bootloader_random_enable();
    __telemetry_queue = xQueueCreate(10, sizeof(s_otel_queue_data_t *));
    xTaskCreate(__otel_task, "otel-service-task", 2 * 2048, NULL, 4, NULL);
}

#if 0
static void __otel_test_loop(void *pv)
{
    printf("otel-test-loop\r\n");
    s_otel_queue_data_t *otel_data = ezlopi_malloc(__FUNCTION__, sizeof(s_otel_queue_data_t));
    if (otel_data)
    {
        otel_data->type = E_OTEL_TRACES;
        otel_data->cj_data = cJSON_CreateObject(__FUNCTION__);
        if (otel_data->cj_data)
        {
            cJSON_AddStringToObject(__FUNCTION__, otel_data->cj_data, ezlopi_method_str, "hub.items.list");
            cJSON_AddStringToObject(__FUNCTION__, otel_data->cj_data, ezlopi_name_str, "websocket-client");
            cJSON_AddNumberToObject(__FUNCTION__, otel_data->cj_data, ezlopi_kind_str, 1);
            cJSON_AddNumberToObject(__FUNCTION__, otel_data->cj_data, ezlopi_startTime_str, 1733828105);
            cJSON_AddNumberToObject(__FUNCTION__, otel_data->cj_data, ezlopi_endTime_str, 1733828106);
        }

        if (pdFALSE == xQueueSend(__telemetry_queue, &otel_data, 0))
        {
            cJSON_Delete(__FUNCTION__, otel_data->cj_data);
            ezlopi_free(__FUNCTION__, otel_data);
        }
    }
}
#endif

static void __otel_loop(void *pv)
{
    if (EZPI_SUCCESS == ezlopi_wait_for_wifi_to_connect(0))
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
                    case E_OTEL_LOGS:
                    {
                        // __otel_logs_decorate(otel_data->cj_data);
                        break;
                    }
                    case E_OTEL_TRACES:
                    {
                        cj_telemetry = __otel_trace_decorate(otel_data->cj_data);
                        break;
                    }
                    case E_OTEL_MATRICS:
                    {
                        break;
                    }
                    default:
                    {
                        break;
                    }
                    }

                    __otel_publish(cj_telemetry);
                }

                cJSON_Delete(__FUNCTION__, otel_data->cj_data);
            }

            ezlopi_free(__FUNCTION__, otel_data);
        }
    }
}

static void __otel_publish(cJSON *cj_telemetry)
{
    if (__wss_client && ezlopi_websocket_client_is_connected(__wss_client))
    {
        uint32_t buffer_len = 0;
        char *buffer = ezlopi_core_buffer_acquire(__FUNCTION__, &buffer_len, 2000);
        if (buffer)
        {
            cJSON_PrintPreallocated(__FUNCTION__, cj_telemetry, buffer, buffer_len, false);
            ezlopi_websocket_client_send(__wss_client, buffer, strlen(buffer), 1000);
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
    if (payload && len)
    {
        printf("otel-response:: %lu -> wss-payload: %.*s\r\n", time_sec, len, payload);
    }
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

static cJSON *__otel_trace_decorate(cJSON *cj_traces_info)
{
    cJSON *cj_traceRecord = cJSON_CreateObject(__FUNCTION__);
    if (cj_traceRecord)
    {
        cJSON *cj_resourceSpans = cJSON_AddArrayToObject(__FUNCTION__, cj_traceRecord, ezlopi_resourceSpans_str);
        if (cj_resourceSpans)
        {
            cJSON *cj_resourceSpan = cJSON_CreateObject(__FUNCTION__);
            if (cj_resourceSpan)
            {
                cJSON *cj_resource = __otel_create_resource();
                if (cj_resource)
                {
                    if (false == cJSON_AddItemToObject(__FUNCTION__, cj_resourceSpan, ezlopi_resource_str, cj_resource))
                    {
                        cJSON_Delete(__FUNCTION__, cj_resource);
                    }
                }

                cJSON *cj_scopeSpans = cJSON_AddArrayToObject(__FUNCTION__, cj_resourceSpan, ezlopi_scopeSpans_str);
                if (cj_scopeSpans)
                {
                    cJSON *cj_scopeSpan = cJSON_CreateObject(__FUNCTION__);
                    if (cj_scopeSpan)
                    {
                        cJSON *cj_scope = __otel_create_scope();
                        if (cj_scope)
                        {
                            if (false == cJSON_AddItemToObject(__FUNCTION__, cj_scopeSpan, ezlopi_scope_str, cj_scope))
                            {
                                cJSON_Delete(__FUNCTION__, cj_scope);
                            }
                        }

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

#if 0
        char *_data_str = cJSON_Print(__FUNCTION__, cj_traceRecord);
        cJSON_Delete(__FUNCTION__, cj_traceRecord);
        cj_traceRecord = NULL;

        if (_data_str)
        {
            printf("otel-trace:\r\n%s\r\n", _data_str);
            ezlopi_free(__FUNCTION__, _data_str);
        }
#endif
    }

    return cj_traceRecord;
}

static void __otel_logs_decorate(cJSON *cj_logs)
{
    if (cj_logs)
    {
        cJSON *cj_resourceLogs = cJSON_AddArrayToObject(__FUNCTION__, cj_logs, "resourceLogs");

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

                        if (false == cJSON_AddItemToObject(__FUNCTION__, cj_scopeLogs_item, "logRecords", cj_logs))
                        {
                            cJSON_Delete(__FUNCTION__, cj_logs);
                        }

                        if (false == cJSON_AddItemToArray(cj_scopeLogs, cj_scopeLogs_item))
                        {
                            cJSON_Delete(__FUNCTION__, cj_scopeLogs_item);
                        }
                    }
                }

                if (false == cJSON_AddItemToArray(cj_resourceLogs, cj_log))
                {
                    cJSON_Delete(__FUNCTION__, cj_log);
                }
            }
        }
    }
}

static cJSON *__otel_create_scope(void)
{
    cJSON *cj_scope = cJSON_CreateObject(__FUNCTION__);
    if (cj_scope)
    {
        cJSON_AddStringToObject(__FUNCTION__, cj_scope, "name", "otel-c");
        cJSON_AddStringToObject(__FUNCTION__, cj_scope, "version", "1.0.0");
    }

    return cj_scope;
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

static void __otel_add_time_stamp_nano(cJSON *cj_span, const char *add_key, const char *for_key, cJSON *cj_trace_info)
{
    cJSON *cj_time_stamp = cJSON_DetachItemFromObject(__FUNCTION__, cj_trace_info, for_key);
    if (cj_time_stamp)
    {
        char tmp_buffer[32];
        snprintf(tmp_buffer, sizeof(tmp_buffer), "%llu", (uint64_t)(cj_time_stamp->valuedouble * 1000000000llu));
        cJSON_AddStringToObject(__FUNCTION__, cj_span, add_key, tmp_buffer);
        cJSON_Delete(__FUNCTION__, cj_time_stamp);
    }
}

static void __cjson_copy_and_delete(cJSON *cj_haystack, const char *key_str, cJSON *cj_needle)
{
    if (cj_haystack && cj_needle && key_str)
    {
        cJSON *cj_dup = cJSON_Duplicate(__FUNCTION__, cj_needle, true);
        if (cj_dup)
        {
            if (false == cJSON_AddItemToObject(__FUNCTION__, cj_haystack, key_str, cj_dup))
            {
                cJSON_Delete(__FUNCTION__, cj_dup);
            }
        }

        cJSON_Delete(__FUNCTION__, cj_needle);
    }
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

        __cjson_copy_and_delete(cj_span, ezlopi_name_str, cJSON_DetachItemFromObject(__FUNCTION__, cj_trace_info, ezlopi_name_str));
        __cjson_copy_and_delete(cj_span, ezlopi_kind_str, cJSON_DetachItemFromObject(__FUNCTION__, cj_trace_info, ezlopi_kind_str));
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
                    printf("%.*s: %.*s\r\n", cj_attr->str_key_len, cj_attr->string, cj_attr->str_value_len, cj_attr->valuestring);
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

static cJSON *__otel_create_resource(void)
{
    cJSON *cj_resource = cJSON_CreateObject(__FUNCTION__);
    if (cj_resource)
    {
        cJSON *cj_attributes = cJSON_AddArrayToObject(__FUNCTION__, cj_resource, ezlopi_attributes_str);
        if (cj_attributes)
        {
            cJSON *cj_attribute = cJSON_CreateObject(__FUNCTION__);
            if (cj_attribute)
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_attribute, ezlopi_key_str, ezlopI_service___name);
                cJSON *cj_value = cJSON_AddObjectToObject(__FUNCTION__, cj_attribute, ezlopi_value_str);
                if (cj_value)
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_value, ezlopi_stringValue_str, ezlopi_EzloPI_str);
                }

                if (false == cJSON_AddItemToArray(cj_attributes, cj_attribute))
                {
                    cJSON_Delete(__FUNCTION__, cj_attribute);
                }
            }
        }
    }
    return cj_resource;
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
    if (pdTRUE == xQueueIsQueueFullFromISR(__telemetry_queue))
    {
        s_otel_queue_data_t *dump_telemetry = NULL;
        xQueueReceive(__telemetry_queue, &dump_telemetry, 0);
        if (dump_telemetry)
        {
            ezlopi_free(__FUNCTION__, dump_telemetry);
        }
    }

    if (pdTRUE == xQueueSend(__telemetry_queue, &otel_data, 500 / portTICK_RATE_MS))
    {
        ret = 1;
    }
    else
    {
        ezlopi_free(__FUNCTION__, otel_data);
    }
    return ret;
}
