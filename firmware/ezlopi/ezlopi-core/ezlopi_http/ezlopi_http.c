#include "string.h"

#include "esp_tls.h"
#include "esp_http_client.h"
#include "trace.h"
#include "ezlopi_http.h"

static void ezlopi_http_free_rx_data(s_rx_data_t *rx_data);
static esp_err_t ezlopi_http_event_handler(esp_http_client_event_t *evt);

#define TAG __FILE__

#define FREE_IF_NOT_NULL(ptr) \
    {                         \
        if (ptr)              \
        {                     \
            free(ptr);        \
            ptr = NULL;       \
        }                     \
    }

char *ezlopi_http_get_request(char *cloud_url, char *private_key, char *shared_key, char *ca_certificate)
{
    char *ret = NULL;
    s_rx_data_t *my_data = (s_rx_data_t *)malloc(sizeof(s_rx_data_t));

    if (my_data)
    {
        memset(my_data, 0, sizeof(s_rx_data_t));

        esp_http_client_config_t config = {
            .url = cloud_url,
            .cert_pem = ca_certificate,
            .client_cert_pem = shared_key,
            .client_key_pem = private_key,
            .event_handler = ezlopi_http_event_handler,
            .transport_type = HTTP_TRANSPORT_OVER_SSL,
            .user_data = (void *)(my_data), // my_data will be filled in 'ezlopi_http_event_handler'
        };

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

                if (my_data->total_len)
                {
                    ret = (char *)malloc(my_data->total_len + 1);

                    if (ret)
                    {
                        s_rx_data_t *cur_d = my_data;
                        memset(ret, 0, my_data->total_len + 1);

                        while (cur_d)
                        {
                            strcat(ret, cur_d->ptr);
                            TRACE_D("%.*s", cur_d->len, cur_d->ptr);
                            cur_d = cur_d->next;
                        }
                    }
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

    return ret;
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
                    memcpy(tmp_data, evt->data, evt->data_len);
                    tmp_data[evt->data_len] = '\0';

                    s_rx_data_t *my_data = (s_rx_data_t *)evt->user_data;
                    if (my_data->len || my_data->total_len || my_data->ptr)
                    {
                        s_rx_data_t *cur_dh = my_data;
                        while (cur_dh->next)
                        {
                            cur_dh = cur_dh->next;
                        }

                        cur_dh->next = (s_rx_data_t *)malloc(sizeof(s_rx_data_t));
                        if (cur_dh->next)
                        {
                            cur_dh->next->len = evt->data_len;
                            cur_dh->next->ptr = (char *)tmp_data;
                            cur_dh->next->total_len = 0;
                            my_data->total_len += evt->data_len;
                            cur_dh->next->next = NULL;
                        }
                    }
                    else
                    {
                        my_data->len = evt->data_len;
                        my_data->total_len = evt->data_len;
                        my_data->ptr = (char *)tmp_data;
                        my_data->next = NULL;
                    }
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

static void ezlopi_http_free_rx_data(s_rx_data_t *rx_data)
{
    if (rx_data)
    {
        if (rx_data->next)
        {
            ezlopi_http_free_rx_data(rx_data->next);
        }

        if (rx_data->ptr)
        {
            free(rx_data->ptr);
        }
        free(rx_data);
    }
}