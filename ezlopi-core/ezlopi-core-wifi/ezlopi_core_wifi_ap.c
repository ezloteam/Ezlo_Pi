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
 * @file    ezlopi_core_wifi_ap.c
 * @brief   perform some function on wifi-AP (captive portal)
 * @author
 * @version 1.0
 * @date    January 6th, 2024 11:03 PM
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "sdkconfig.h"

#ifdef CONFIG_EZPI_ENABLE_CAPTIVE_PORTAL

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "ezlopi_util_trace.h"
#include "dns_hijacking.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_wifi_ap.h"

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/
static esp_netif_t *ezlopi_ap_netif = NULL;

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/

void EZPI_core_configure_wifi_ap()
{
    char ezlopi_wifi_ap_ssid[32];
    char *ezlopi_wifi_ap_pass = "123456789";
    // Check the return type, may be important.
    ezlopi_ap_netif = esp_netif_create_default_wifi_ap();
    wifi_init_config_t wifi_init_configurations = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_configurations));

    wifi_config_t wifi_cred_configurations = {
        .ap = {
            .channel = 1,
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    const char *device_type = EZPI_core_factory_info_v3_get_device_type();
    if ((NULL != device_type) && (isprint(device_type[0])))
    {
        snprintf(ezlopi_wifi_ap_ssid, sizeof(ezlopi_wifi_ap_ssid), "ezlopi_%s_%llu", device_type, EZPI_core_factory_info_v3_get_id());
    }
    else
    {
        uint8_t mac[6];
        memset(mac, 0, sizeof(mac));
        esp_read_mac(mac, ESP_MAC_BT);
        snprintf(ezlopi_wifi_ap_ssid, sizeof(ezlopi_wifi_ap_ssid), "ezlopi_%02x%02x%02x%02x%02x%02x",
                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        ezlopi_wifi_ap_ssid[19] = '\0';
    }

    strncpy((char *)wifi_cred_configurations.ap.ssid, ezlopi_wifi_ap_ssid, sizeof(wifi_cred_configurations.ap.ssid));
    strncpy((char *)wifi_cred_configurations.ap.password, ezlopi_wifi_ap_pass, sizeof(wifi_cred_configurations.ap.password));

    TRACE_I("Setting wifi mode to AP.");
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

    TRACE_I("Setting wifi AP credential configurations.");
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_cred_configurations));
    TRACE_S("Wifi credentials set complete with channel: %d SSID: %s and PASS: %s", wifi_cred_configurations.ap.channel, wifi_cred_configurations.ap.ssid, wifi_cred_configurations.ap.password);

    TRACE_I("Starting wifi AP");
    ESP_ERROR_CHECK(esp_wifi_start());
    TRACE_I("Wifi AP started!!");
}

void EZPI_core_deconfigure_wifi_ap()
{
    esp_netif_destroy_default_wifi(ezlopi_ap_netif);
    ESP_ERROR_CHECK(esp_wifi_stop());
    TRACE_E("Wifi stopped.");
    // ESP_ERROR_CHECK(esp_wifi_deinit());
    // TRACE_E("Wifi resources deinitialized.");
}

void EZPI_start_dns_server()
{
    ip4_addr_t resolve_ip;
    inet_pton(AF_INET, "192.168.4.1", &resolve_ip);

    if (dns_hijack_srv_start(resolve_ip) == ESP_OK)
    {
        TRACE_I("DNS hijack server started");
    }
    else
    {
        TRACE_E("DNS hijack server has not started");
    }
}

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/

#endif // CONFIG_EZPI_ENABLE_CAPTIVE_PORTAL

/*******************************************************************************
 *                          End of File
 *******************************************************************************/