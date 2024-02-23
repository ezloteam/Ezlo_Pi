
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_netif_ip_addr.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "mdns.h"
#include "driver/gpio.h"
#include "netdb.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_core_mdns.h"

#define EXAMPLE_MDNS_INSTANCE "esp-instance"
#define EXAMPLE_HOST_NAME "ezlopi-device"

static char *generate_hostname(void);

static void initialise_mdns(void)
{
    char *hostname = generate_hostname();

    // initialize mDNS
    ESP_ERROR_CHECK(mdns_init());
    // set mDNS hostname (required if you want to advertise services)
    ESP_ERROR_CHECK(mdns_hostname_set(hostname));
    TRACE_D("mdns hostname set to: [%s]", hostname);
    // set default mDNS instance name
    ESP_ERROR_CHECK(mdns_instance_name_set(EXAMPLE_MDNS_INSTANCE));
    ESP_ERROR_CHECK(mdns_service_add("ESP32-WebServer", "_http", "_tcp", 80, NULL, 0));
    free(hostname);
}

int ezlopi_core_initialize_mdns(void)
{
    int ret = 0;
    initialise_mdns();
    return ret;
}

/** Generate host name based on sdkconfig, optionally adding a portion of MAC address to it.
 *  @return host name string allocated from the heap
 */
static char *generate_hostname(void)
{
#ifndef CONFIG_MDNS_ADD_MAC_TO_HOSTNAME
    return strdup(EXAMPLE_HOST_NAME);
#else
    uint8_t mac[6];
    char *hostname;
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    if (-1 == asprintf(&hostname, "%s-%02X%02X%02X", EXAMPLE_HOST_NAME, mac[3], mac[4], mac[5]))
    {
        abort();
    }
    return hostname;
#endif
}
