/* ===========================================================================
** Copyright (C) 2025 Ezlo Innovation Inc
**
** Under EZLO AVAILABLE SOURCE LICENSE (EASL) AGREEMENT
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/
/**
 * @file    ezlopi_core_ota.c
 * @brief   Function to perform ota operation
 * @author
 * @version 1.0
 * @date    March 7th, 2023 8:52 PM
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
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

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/
#define HASH_LEN 32
#define OTA_URL_SIZE 512

typedef enum e_ezlopi_ota_state
{
    EZLOPI_OTA_STATE_FINISH = 0,
    EZLOPI_OTA_STATE_STARTED,
    EZLOPI_OTA_STATE_UPDATING,
    EZLOPI_OTA_STATE_FAILED,
    EZLOPI_OTA_STATE_MAX,
} e_ezlopi_ota_state_t;
/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/
static int32_t __byte_count = 0;
static volatile uint32_t __ota_in_process = 0;

static void ezlopi_ota_process(void *pv);
static esp_err_t _http_event_handler(esp_http_client_event_t *evt);

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/

uint32_t EPZI_core_ota_get_state(void)
{
    return ((__ota_in_process < EZLOPI_OTA_STATE_MAX) ? __ota_in_process : EZLOPI_OTA_STATE_FINISH);
}

void EZPI_core_ota_start(cJSON *url)
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
#if defined(CONFIG_FREERTOS_USE_TRACE_FACILITY)
                EZPI_core_process_set_process_info(ENUM_EZLOPI_CORE_OTA_PROCESS_TASK, &ezlopi_core_ota_process_task_handle, EZLOPI_CORE_OTA_PROCESS_TASK_DEPTH);
#endif
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

/*******************************************************************************
 *                          Static Function Definitions
 *******************************************************************************/
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

#include "ca_mios.h"
    esp_http_client_config_t config = {
        .url = url,
        .event_handler = _http_event_handler,
        .cert_pem = ca_mios, // ezlopi_factory_info_v3_get_ssl_shared_key(),
                             // .buffer_size = 1024,
                             // .buffer_size_tx = 1024,
    // .client_cert_pem = ezlopi_factory_info_v3_get_ssl_shared_key(),
    // .client_key_pem = ezlopi_factory_info_v3_get_ssl_private_key(),

#ifdef CONFIG_FIRMWARE_UPGRADE_BIND_IF
        .if_name = &ifr,
#endif
    };

#ifdef CONFIG_FIRMWARE_UPGRADE_URL_FROM_STDIN
    char url_buf[OTA_URL_SIZE];
    if (strncmp(config.url, "FROM_STDIN", ((strlen(config.url) + 1) > 11 ? (strlen(config.url) + 1) : 11)) == 0)
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
        EZPI_core_reset_reboot();
    }
    else
    {
        __ota_in_process = EZLOPI_OTA_STATE_FAILED;
        TRACE_E("Firmware upgrade failed");
    }

#if defined(CONFIG_FREERTOS_USE_TRACE_FACILITY)
    EZPI_core_process_set_is_deleted(ENUM_EZLOPI_CORE_OTA_PROCESS_TASK);
#endif
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
        TRACE_E("HTTP_EVENT_ERROR");
        TRACE_E("HTTP_EVENT_ERROR, error code: %d", esp_http_client_get_errno(evt));
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
/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/

#endif // CONFIG_EZPI_ENABLE_OTA

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
