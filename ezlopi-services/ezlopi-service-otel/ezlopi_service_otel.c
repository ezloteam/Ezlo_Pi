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

typedef struct s_otel_queue_data
{
    cJSON *cj_data;
    e_otel_type_t type;
} s_otel_queue_data_t;

static QueueHandle_t __telemetry_queue = NULL;
static esp_websocket_client_handle_t __wss_client = NULL;
static const char *__ot_logs_endpoint = "wss://ot.review-staging-op-owkix8.ewr4.opentelemetry.ezlo.com/logs";

static void __otel_task(void *pv);
static void __otel_publish(cJSON *cj_telemetry);
static void __connection_upcall(bool connected);
static int __message_upcall(char *payload, uint32_t len, time_t time_sec);

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

int ezlopi_service_otel_publish_cjson(cJSON *cj_trace)
{
    int ret = 0;

    if (cj_trace)
    {
    }

    return ret;
}

void ezlopi_service_otel_init(void)
{
    bootloader_random_enable();
    __telemetry_queue = xQueueCreate(10, sizeof(s_otel_queue_data_t *));
    xTaskCreate(__otel_task, "otel-service-task", 2 * 2048, NULL, 4, NULL);
    // __otel_task(NULL);
}

static void __otel_trace_decorate(cJSON *cj_traces)
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
                cJSON *cj_resource = cJSON_AddObjectToObject(__FUNCTION__, cj_resourceSpan, ezlopi_resource_str);
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

                cJSON *cj_scopeSpans = cJSON_AddArrayToObject(__FUNCTION__, cj_resourceSpan, "scopeSpans");
                if (cj_scopeSpans)
                {
                    cJSON *cj_scopeSpan = cJSON_CreateObject(__FUNCTION__);
                    if (cj_scopeSpan)
                    {
                        cJSON *cj_scope = cJSON_AddObjectToObject(__FUNCTION__, cj_scopeSpan, "scope");
                        if (cj_scope)
                        {
                            cJSON_AddStringToObject(__FUNCTION__, cj_scope, "name", "my.library");
                            cJSON_AddStringToObject(__FUNCTION__, cj_scope, "version", "1.0.0");
                            cJSON *cj_attributes = cJSON_AddArrayToObject(__FUNCTION__, cj_scope, "attributes");
                            if (cj_attributes)
                            {
                                cJSON *cj_attribute = cJSON_CreateObject(__FUNCTION__);
                                if (cj_attribute)
                                {
                                    cJSON_AddStringToObject(__FUNCTION__, cj_attribute, "key", "my.scope.attribute");
                                    cJSON *cj_value = cJSON_AddObjectToObject(__FUNCTION__, cj_attribute, "value");
                                    if (cj_value)
                                    {
                                        cJSON_AddStringToObject(__FUNCTION__, cj_value, "stringValue", "some scope attribute");
                                    }

                                    if (false == cJSON_AddItemToArray(cj_attributes, cj_attribute))
                                    {
                                        cJSON_Delete(__FUNCTION__, cj_attribute);
                                    }
                                }
                            }
                        }

                        cJSON *cj_spans = cJSON_AddArrayToObject(__FUNCTION__, cj_scopeSpan, "spans");
                        if (cj_spans)
                        {
                            cJSON *cj_span = cJSON_CreateObject(__FUNCTION__);
                            if (cj_span)
                            {
                                cJSON_AddStringToObject(__FUNCTION__, cj_span, "traceId", "");
                                cJSON_AddStringToObject(__FUNCTION__, cj_span, "spanId", "");
                                cJSON_AddStringToObject(__FUNCTION__, cj_span, "paperntSpanId", "");
                                cJSON_AddStringToObject(__FUNCTION__, cj_span, "name", "I'm a server span");
                                cJSON_AddStringToObject(__FUNCTION__, cj_span, "startTimeUnixNano", "");
                                cJSON_AddStringToObject(__FUNCTION__, cj_span, "endTimeUnixNano", "");
                                cJSON_AddNumberToObject(__FUNCTION__, cj_span, "kind", 2);
                                cJSON *cj_attributes = cJSON_AddArrayToObject(__FUNCTION__, cj_span, "attributes");
                                if (cj_attributes)
                                {
                                    cJSON *cj_attribute = cJSON_CreateObject(__FUNCTION__);
                                    if (cj_attribute)
                                    {
                                        cJSON_AddStringToObject(__FUNCTION__, cj_attribute, "key", "my.span.attr");
                                        cJSON *cj_value = cJSON_AddObjectToObject(__FUNCTION__, cj_attribute, "value");
                                        if (cj_value)
                                        {
                                            cJSON_AddStringToObject(__FUNCTION__, cj_value, "stringValue", "some value");
                                        }

                                        if (false == cJSON_AddItemToArray(cj_attributes, cj_attribute))
                                        {
                                            cJSON_Delete(__FUNCTION__, cj_attribute);
                                        }
                                    }
                                }

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
                switch (otel_data->type)
                {
                case E_OTEL_LOGS:
                {
                    __otel_logs_decorate(otel_data->cj_data);
                    break;
                }
                case E_OTEL_TRACES:
                {
                    __otel_trace_decorate(otel_data->cj_data);
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

                __otel_publish(otel_data->cj_data);
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
    vTaskDelete(NULL);
}

static int __message_upcall(char *payload, uint32_t len, time_t time_sec)
{
    int ret = 0;
    if (payload && len)
    {
        TRACE_D("%lu -> wss-payload: %.*s", time_sec, len, payload);
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
