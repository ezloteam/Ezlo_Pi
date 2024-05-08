#include "ezlopi_core_net.h"


static s_ezlopi_net_status_t net_stat;

#ifdef CONFIG_EZPI_CORE_ENABLE_ETH

void ezlopi_net_init(void)
{
    memset(&net_stat, 0, sizeof(s_ezlopi_net_status_t));
    ezlopi_ethernet_init();
}

#endif  // CONFIG_EZPI_CORE_ENABLE_ETH

s_ezlopi_net_status_t* ezlopi_get_net_status(void)
{

    net_stat.wifi_status = ezlopi_wifi_status();
#ifdef CONFIG_EZPI_ENABLE_PING
    net_stat.internet_status = ezlopi_ping_get_internet_status();
#else // CONFIG_EZPI_ENABLE_PING
    net_stat.internet_status = EZLOPI_PING_STATUS_UNKNOWN;
#endif // CONFIG_EZPI_ENABLE_PING

    net_stat.nma_cloud_connection_status = ezlopi_websocket_client_is_connected();

    return &net_stat;
}

