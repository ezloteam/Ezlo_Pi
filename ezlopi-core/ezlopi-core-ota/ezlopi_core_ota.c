
#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_ENABLE_OTA

#include <string.h>
#include <sys/socket.h>

#include "esp_http_client.h"
#include "esp_tls.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_ota_ops.h"
#include "esp_https_ota.h"
#include "esp_crt_bundle.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_core_ota.h"
#include "ezlopi_core_reset.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_processes.h"

#include "ezlopi_service_ota.h"
#include "EZLOPI_USER_CONFIG.h"

#define HASH_LEN 32

#ifdef CONFIG_FIRMWARE_UPGRADE_BIND_IF
/* The interface name value can refer to if_desc in esp_netif_defaults.h */
#if CONFIG_FIRMWARE_UPGRADE_BIND_IF_ETH
static const char *bind_interface_name = "eth";
#elif CONFIG_FIRMWARE_UPGRADE_BIND_IF_STA
static const char *bind_interface_name = "sta";
#endif
#endif

#define OTA_URL_SIZE 512

typedef enum e_ezlopi_ota_state
{
    EZLOPI_OTA_STATE_FINISH = 0,
    EZLOPI_OTA_STATE_STARTED,
    EZLOPI_OTA_STATE_UPDATING,
    EZLOPI_OTA_STATE_FAILED,
    EZLOPI_OTA_STATE_MAX,
} e_ezlopi_ota_state_t;

static int32_t __byte_count = 0;
static volatile uint32_t __ota_in_process = 0;

static void ezlopi_ota_process(void *pv);
static esp_err_t _http_event_handler(esp_http_client_event_t *evt);

uint32_t __get_ota_state(void)
{
    return ((__ota_in_process < EZLOPI_OTA_STATE_MAX) ? __ota_in_process : EZLOPI_OTA_STATE_FINISH);
}

void ezlopi_ota_start(cJSON *url)
{
    if (url && url->valuestring)
    {
        char *ota_url = (char *)ezlopi_malloc(__FUNCTION__, OTA_URL_SIZE);
        if (ota_url)
        {
            memcpy(ota_url, url->valuestring, OTA_URL_SIZE);
            if (0 == __ota_in_process)
            {
                TaskHandle_t ezlopi_core_ota_process_task_handle = NULL;
                xTaskCreate(ezlopi_ota_process, "EzpiOTAProcess", EZLOPI_CORE_OTA_PROCESS_TASK_DEPTH, ota_url, 3, &ezlopi_core_ota_process_task_handle);
                ezlopi_core_process_set_process_info(ENUM_EZLOPI_CORE_OTA_PROCESS_TASK, &ezlopi_core_ota_process_task_handle, EZLOPI_CORE_OTA_PROCESS_TASK_DEPTH);
            }
            else
            {
                TRACE_W("Ota in progress...");
                ezlopi_free(__FUNCTION__, ota_url);
                ota_url = NULL;
            }
        }
    }
}

static void ezlopi_ota_process(void *pv)
{
    __ota_in_process = EZLOPI_OTA_STATE_STARTED;
    char *url = (char *)pv;

    TRACE_S("Starting OTA ");
#ifdef CONFIG_FIRMWARE_UPGRADE_BIND_IF
    esp_netif_t *netif = get_example_netif_from_desc(bind_interface_name);
    if (netif == NULL)
    {
        TRACE_E("Can't find netif from interface description");
        abort();
    }
    struct ifreq ifr;
    esp_netif_get_netif_impl_name(netif, ifr.ifr_name);
    TRACE_S("Bind interface name is %s", ifr.ifr_name);
#endif

    esp_http_client_config_t config = {
        .url = url,
        .event_handler = _http_event_handler,
        .keep_alive_enable = true,
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
        // .cert_pem = ezlopi_factory_info_v2_get_ca_certificate(),
        // .client_cert_pem = ezlopi_factory_info_v2_get_ssl_shared_key(),
        // .client_key_pem = ezlopi_factory_info_v2_get_ssl_private_key(),
        .crt_bundle_attach = esp_crt_bundle_attach,

#ifdef CONFIG_FIRMWARE_UPGRADE_BIND_IF
        .if_name = &ifr,
#endif
    };

#ifdef CONFIG_FIRMWARE_UPGRADE_URL_FROM_STDIN
    char url_buf[OTA_URL_SIZE];
    if (strcmp(config.url, "FROM_STDIN") == 0)
    {
        example_configure_stdin_stdout();
        fgets(url_buf, OTA_URL_SIZE, stdin);
        int len = strlen(url_buf);
        url_buf[len - 1] = '\0';
        config.url = url_buf;
    }
    else
    {
        TRACE_E("Configuration mismatch: wrong firmware upgrade image url");
        abort();
    }
#endif

    __byte_count = 0;

    esp_err_t ret = esp_https_ota(&config);
    if (ret == ESP_OK)
    {
        __ota_in_process = EZLOPI_OTA_STATE_FINISH;
        TRACE_W("Firmware Upgrade Successful, restarting !");
        EZPI_CORE_reset_reboot();
    }
    else
    {
        __ota_in_process = EZLOPI_OTA_STATE_FAILED;
        TRACE_E("Firmware upgrade failed");
    }

    ezlopi_core_process_set_is_deleted(ENUM_EZLOPI_CORE_OTA_PROCESS_TASK);
    vTaskDelete(NULL);

    if (url)
    {
        ezlopi_free(__FUNCTION__, url);
    }
}

static esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ERROR:
    {
        TRACE_D("HTTP_EVENT_ERROR");
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
        __ota_in_process = EZLOPI_OTA_STATE_STARTED;
        TRACE_D("HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;
    }
    case HTTP_EVENT_ON_DATA:
    {
        __ota_in_process = EZLOPI_OTA_STATE_UPDATING;

        TRACE_D("HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        __byte_count += evt->data_len;
        TRACE_D("Received Bytes: %d", __byte_count);
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
        break;
    }
    }
    return ESP_OK;
}
#endif // CONFIG_EZPI_ENABLE_OTA