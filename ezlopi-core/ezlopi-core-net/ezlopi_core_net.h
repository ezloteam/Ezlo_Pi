#ifndef _EZLOPI_CORE_NET_H_
#define _EZLOPI_CORE_NET_H_

#include <string.h>

#include "ezlopi_core_wifi.h"
#include "ezlopi_core_ethernet.h"
#include "ezlopi_core_ping.h"
#include "ezlopi_core_websocket_client.h"

typedef struct s_ezlopi_net_status
{
    ezlopi_wifi_status_t *wifi_status;
    e_ping_status_t internet_status;
    bool nma_cloud_connection_status;
} s_ezlopi_net_status_t;

s_ezlopi_net_status_t *ezlopi_get_net_status(void);
#endif // _EZLOPI_CORE_NET_H_
