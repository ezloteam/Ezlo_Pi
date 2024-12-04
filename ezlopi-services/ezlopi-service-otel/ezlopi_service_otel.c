#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_websocket_client.h>

#include "ezlopi_core_wsc.h"
#include "ezlopi_core_wifi.h"
#include "ezlopi_core_websocket_client.h"

#include "ezlopi_service_otel.h"

static esp_websocket_client_handle_t __wss_client = NULL;
static const char *__ot_logs_endpoint = "wss://ot.review-staging-op-owkix8.ewr4.opentelemetry.ezlo.com/logs";

static void __otel_task(void *pv);
static void __connection_upcall(bool connected);
static int __message_upcall(char *payload, uint32_t len, time_t time_sec);

void ezlopi_service_otel_init(void)
{
    xTaskCreate(__otel_task, "otel-service-task", 2 * 2048, NULL, 4, NULL);
    // __otel_task(NULL);
}

static void __otel_task(void *pv)
{
    ezlopi_wait_for_wifi_to_connect(portTICK_RATE_MS);
    TRACE_D("Starting otel-service");

    cJSON *cjson_uri = cJSON_CreateString(__FUNCTION__, __ot_logs_endpoint);
    if (cjson_uri)
    {
        TRACE_D("uri: %.*s", cjson_uri->str_value_len, cjson_uri->valuestring);

        // char *ca_cert = ezlopi_factory_info_v3_get_ca_certificate();
        // char *ssl_shared = ezlopi_factory_info_v3_get_ssl_shared_key();
        // char *ssl_private = ezlopi_factory_info_v3_get_ssl_private_key();

        while (1)
        {
            // __wss_client = ezlopi_websocket_client_init(cjson_uri, __message_upcall, __connection_upcall,
            //                                             ca_cert, ssl_private, ssl_shared);
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
