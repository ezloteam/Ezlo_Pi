#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_websocket_client.h>

#include "ezlopi_core_wsc.h"
#include "ezlopi_core_wifi.h"

#include "ezlopi_service_otel.h"

static s_ssl_websocket_t *__wsc_ssl = NULL;
static const char *__ot_logs_endpoint = "wss://ot.review-staging-op-owkix8.ewr4.opentelemetry.ezlo.com/logs";

static void __otel_task(void *pv);
static void __connection_upcall(bool connected);
static void __message_upcall(const char *payload, uint32_t len);
static void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

void ezlopi_service_otel_init(void)
{
    // xTaskCreate(__otel_task, "otel-service-task", 2 * 2048, NULL, 4, NULL);
    // __otel_task(NULL);
}

static void __otel_task(void *pv)
{
    ezlopi_wait_for_wifi_to_connect(portTICK_RATE_MS);

    TRACE_D("Starting otel-service");

#if 0
    cJSON *cjson_uri = cJSON_CreateString(__FUNCTION__, __ot_logs_endpoint);
    if (cjson_uri)
    {
        while (1)
        {
            __wsc_ssl = ezlopi_core_wsc_init(cjson_uri, __message_upcall, __connection_upcall);
            if (NULL != __wsc_ssl)
            {
                break;
            }
        }

        cJSON_Delete(__FUNCTION__, cjson_uri);
    }
#else // esp-wss
    esp_websocket_client_config_t websocket_cfg = {
        .uri = __ot_logs_endpoint,
        .task_stack = 3 * 1024,
        .buffer_size = EZPI_CORE_WSS_DATA_BUFFER_SIZE,
        .keep_alive_enable = 1,
        .ping_interval_sec = EZPI_CORE_WSS_PING_INTERVAL_SEC,
        .pingpong_timeout_sec = EZPI_CORE_WSS_PING_PONG_TIMEOUT_SEC,

        .buffer_size = 512,

        .cert_pem = NULL,
        .client_key = NULL,
        .client_cert = NULL,
    };

    esp_websocket_client_handle_t *client = esp_websocket_client_init(&websocket_cfg);
    if (client)
    {
        esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY, websocket_event_handler, NULL);
        esp_websocket_client_start(client);
    }

    vTaskDelete(NULL);
#endif
}

#if 1 // esp-websocket
static void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;
    switch (event_id)
    {
    case WEBSOCKET_EVENT_CONNECTED:
    {
        TRACE_W("free-heap: %.02f KB", esp_get_free_heap_size() / 1024.0);
        TRACE_S("WEBSOCKET_EVENT_CONNECTED");
        break;
    }
    case WEBSOCKET_EVENT_DISCONNECTED:
    {
        TRACE_W("free-heap: %.02f KB", esp_get_free_heap_size() / 1024.0);
        TRACE_E("WEBSOCKET_EVENT_DISCONNECTED");

        break;
    }
    case WEBSOCKET_EVENT_DATA:
    {
        // data->op_code;
        TRACE_D("<< RX-data:\r\n%.*s", data->data_len, data->data_ptr);
        break;
    }
    case WEBSOCKET_EVENT_ERROR:
    {
        TRACE_E("WEBSOCKET_EVENT_ERROR");
        break;
    }
    default:
    {
        TRACE_E("Websocket event type not-implemented! value: %u", event_id);
        break;
    }
    }
}
#endif

static void __message_upcall(const char *payload, uint32_t len)
{
    if (payload && len)
    {
        TRACE_D("wss-payload: %.*s", len, payload);
    }
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
