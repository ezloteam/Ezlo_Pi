#include "ezlopi_core_net.h"

static s_ezlopi_net_status_t net_stat;

void ezlopi_net_init(void)
{
    memset(&net_stat, 0, sizeof(s_ezlopi_net_status_t));
    ezlopi_ethernet_init();
}

s_ezlopi_net_status_t *ezlopi_get_net_status(void)
{

    net_stat.wifi_status = ezlopi_wifi_status();
    net_stat.internet_status = ezlopi_ping_get_internet_status();
    net_stat.nma_cloud_connection_status = ezlopi_websocket_client_is_connected();

    return &net_stat;
}