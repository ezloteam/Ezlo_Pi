#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "ezlopi_core_wsc.h"
#include "ezlopi_core_wifi.h"

#include "ezlopi_service_otel.h"

static s_ssl_websocket_t *__wsc_ssl = NULL;
static const char *__ot_logs_endpoint = "wss://ot.review-staging-op-owkix8.ewr4.opentelemetry.ezlo.com/logs";

static void __otel_task(void *pv);
static void __connection_upcall(bool connected);
static void __message_upcall(const char *payload, uint32_t len);

void ezlopi_service_otel_init(void)
{
    // xTaskCreate(__otel_task, "otel-service-task", 2 * 2048, NULL, 4, NULL);
    __otel_task(NULL);
}

static void __otel_task(void *pv)
{
    ezlopi_wait_for_wifi_to_connect(portTICK_RATE_MS);

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

    // vTaskDelete(NULL);
}

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
