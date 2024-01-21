
#include "esp_wifi.h"
#include "esp_event.h"
#include "trace.h"
#include "string.h"
#include "dns_hijacking.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_wifi_ap.h"

static esp_netif_t *ezlopi_ap_netif = NULL;

void ezlopi_configure_wifi_ap()
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

    char *device_type = ezlopi_factory_info_v3_get_device_type();
    if ((NULL != device_type) && (isprint(device_type[0])))
    {
        snprintf(ezlopi_wifi_ap_ssid, sizeof(ezlopi_wifi_ap_ssid), "ezlopi_%s_%llu", device_type, ezlopi_factory_info_v3_get_id());
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

    TRACE_B("Setting wifi mode to AP.");
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

    TRACE_B("Setting wifi AP credential configurations.");
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_cred_configurations));
    TRACE_I("Wifi credentials set complete with channel: %d SSID: %s and PASS: %s", wifi_cred_configurations.ap.channel, wifi_cred_configurations.ap.ssid, wifi_cred_configurations.ap.password);

    TRACE_B("Starting wifi AP");
    ESP_ERROR_CHECK(esp_wifi_start());
    TRACE_B("Wifi AP started!!");
}

void ezlopi_deconfigure_wifi_ap()
{
    esp_netif_destroy_default_wifi(ezlopi_ap_netif);
    ESP_ERROR_CHECK(esp_wifi_stop());
    TRACE_E("Wifi stopped.");
    // ESP_ERROR_CHECK(esp_wifi_deinit());
    // TRACE_E("Wifi resources deinitialized.");
}

void ezlopi_start_dns_server()
{
    ip4_addr_t resolve_ip;
    inet_pton(AF_INET, "192.168.4.1", &resolve_ip);

    if (dns_hijack_srv_start(resolve_ip) == ESP_OK)
    {
        TRACE_B("DNS hijack server started");
    }
    else
    {
        TRACE_E("DNS hijack server has not started");
    }
}
