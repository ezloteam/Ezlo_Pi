
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

#include "ezlopi_core_factory_info.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_core_mdns.h"


static char* generate_hostname(void);

static void initialise_mdns(void)
{
    char* hostname = generate_hostname();

    // initialize mDNS
    ESP_ERROR_CHECK(mdns_init());
    // set mDNS hostname (required if you want to advertise services)
    ESP_ERROR_CHECK(mdns_hostname_set(hostname));
    TRACE_D("mdns hostname set to: [%s]", hostname);
    // set default mDNS instance name
    ESP_ERROR_CHECK(mdns_instance_name_set(CONFIG_EZPI_MDNS_INSTANCE_NAME));

    uint64_t id_val = ezlopi_factory_info_v3_get_id();
    char* id_val_str = malloc(10 * sizeof(char));
    memset(id_val_str, 0, (10 * sizeof(char)));
    snprintf(id_val_str, 10, "%lld", id_val);

    uint16_t firmware_version_val = ezlopi_factory_info_v3_get_version();
    char* firmware_version_val_str = malloc(10 * sizeof(char));
    memset(firmware_version_val_str, 0, 10 * sizeof(char));
    snprintf(firmware_version_val_str, 10, "%d", firmware_version_val);

    // Define mdns serive context
    mdns_txt_item_t service_context_item[8] =
    {
        {
            .key = "ID",
            .value = id_val_str,
        },
        {
            .key = "name",
            .value = (NULL == ezlopi_factory_info_v3_get_name() ? "null" : ezlopi_factory_info_v3_get_name()),
        },
        {
            .key = "manufacturer",
            .value = (NULL == ezlopi_factory_info_v3_get_manufacturer() ? "null" : ezlopi_factory_info_v3_get_manufacturer()),
        },
        {
            .key = "brand",
            .value = (NULL == ezlopi_factory_info_v3_get_brand() ? "null" : ezlopi_factory_info_v3_get_brand())
        },
        {
            .key = "Model",
            .value = (NULL == ezlopi_factory_info_v3_get_model() ? "null" : ezlopi_factory_info_v3_get_model()),
        },
        {
            .key = "ezlopi_device_type",
            .value = (NULL == ezlopi_factory_info_v3_get_device_type() ? "null" : ezlopi_factory_info_v3_get_device_type()),
        },
        {
            .key = "ezlopi_firmware_version",
            .value = firmware_version_val_str,
        },
        {
            .key = "ezlopi_firmware_build",
            .value = "1.8.2",
        },
    };

    ESP_ERROR_CHECK(mdns_service_add(CONFIG_EZPI_MDNS_INSTANCE_NAME, "_http", "_tcp", 80, service_context_item, 8));
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
static char* generate_hostname(void)
{
#ifndef CONFIG_MDNS_ADD_MAC_TO_HOSTNAME
    return strdup(CONFIG_EZPI_MDNS_INSTANCE_NAME);
#else
    uint8_t mac[6];
    char* hostname;
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    if (-1 == asprintf(&hostname, "%s-%02X%02X%02X", CONFIG_EZPI_MDNS_INSTANCE_NAME, mac[3], mac[4], mac[5]))
    {
        abort();
    }
    return hostname;
#endif
}
