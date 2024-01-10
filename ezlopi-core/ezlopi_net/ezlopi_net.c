#include "ezlopi_net.h"

void ezlopi_net_init(void)
{
    ezlopi_ethernet_init();
}

s_ezlopi_net_status_t *ezlopi_get_net_status(void)
{
    s_ezlopi_net_status_t *net_stat = (s_ezlopi_net_status_t *)malloc(sizeof(s_ezlopi_net_status_t));
    if (net_stat)
    {
        net_stat->wifi_status = ezlopi_wifi_status();
        net_stat->internet_status = ezlopi_ping_get_internet_status();
        net_stat->nma_cloud_connection_status = ezlopi_websocket_client_is_connected();
    }
    else
    {
        net_stat = NULL;
    }

    return net_stat;
}