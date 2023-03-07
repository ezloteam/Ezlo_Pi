#include "string.h"

#include "cJSON.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "trace.h"
#include "ezlopi_nvs.h"
#include "ezlopi_http.h"
#include "ezlopi_factory_info.h"

static QueueHandle_t data_queue = NULL;

static void ezlopi_ota_task(void *pv);
static esp_err_t ezlopi_http_event_handler(esp_http_client_event_t *evt);

void ezlopi_ota_start(cJSON *cj_url)
{
    data_queue = xQueueCreate(20, sizeof(char *));
    xTaskCreate(ezlopi_ota_task, "ezlopi ota task", 2048, NULL, 3, NULL);
}

static void ezlopi_ota_task(void *pv)
{
    cJSON *cj_url = (cJSON *)pv;
    char *rx_data = NULL;

    if (cj_url && cj_url->valuestring)
    {

        esp_http_client_config_t config = {
            .url = cj_url->valuestring,
            .event_handler = ezlopi_http_event_handler,
            .user_data = (void *)(&rx_data),
        };

        if (strstr(cj_url->valuestring, "https://"))
        {
            config.transport_type = HTTP_TRANSPORT_OVER_SSL;
            config.cert_pem = ezlopi_factory_info_v2_get_ca_certificate();
            config.client_cert_pem = ezlopi_factory_info_v2_get_ssl_shared_key();
            config.client_key_pem = ezlopi_factory_info_v2_get_ssl_private_key();
        }
        else
        {
            config.transport_type = HTTP_TRANSPORT_OVER_TCP;
            config.cert_pem = NULL;
            config.client_cert_pem = NULL;
            config.client_key_pem = NULL;
        }

        esp_http_client_handle_t client = esp_http_client_init(&config);
        if (NULL != client)
        {
            esp_err_t err = esp_http_client_perform(client);

            if (err == ESP_OK)
            {
                while (!esp_http_client_is_complete_data_received(client))
                {
                    vTaskDelay(50 / portTICK_RATE_MS);
                }
            }
            else
            {
                TRACE_E("Error perform http request %s", esp_err_to_name(err));
            }

            ezlopi_http_free_rx_data(my_data);
            esp_http_client_cleanup(client);
        }
    }
}

static esp_err_t ezlopi_http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ERROR:
    {
        TRACE_E("HTTP_EVENT_ERROR");
        break;
    }
    case HTTP_EVENT_ON_CONNECTED:
    {
        TRACE_D("HTTP_EVENT_ON_CONNECTED");
        break;
    }
    case HTTP_EVENT_HEADER_SENT:
    {
        TRACE_D("HTTP_EVENT_HEADER_SENT");
        break;
    }
    case HTTP_EVENT_ON_HEADER:
    {
        TRACE_D("HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;
    }
    case HTTP_EVENT_ON_DATA:
    {
        TRACE_D("HTTP_EVENT_ON_DATA, len=%d, data: %.*s", evt->data_len, evt->data_len, (char *)evt->data);
        // if (!esp_http_client_is_chunked_response(evt->client))
        {
            if (evt->user_data)
            {
                char *tmp_data = (char *)malloc(evt->data_len + 1);
                if (NULL != tmp_data)
                {
                    xQueueSend(data_queue, &tmp_data, 0);
                }
            }
        }

        break;
    }
    case HTTP_EVENT_ON_FINISH:
    {
        TRACE_D("HTTP_EVENT_ON_FINISH");
        break;
    }
    case HTTP_EVENT_DISCONNECTED:
    {
        TRACE_D("HTTP_EVENT_DISCONNECTED");
        int mbedtls_err = 0;
        esp_err_t err = esp_tls_get_and_clear_last_error((esp_tls_error_handle_t)evt->data, &mbedtls_err, NULL);
        if (err != 0)
        {
            TRACE_D("Last esp error code: 0x%x", err);
            TRACE_D("Last mbedtls failure: 0x%x", mbedtls_err);
        }
        break;
    }
    }
    return ESP_OK;
}