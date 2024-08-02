#ifndef _EZLOPI_CORE_NET_H_
#define _EZLOPI_CORE_NET_H_

#include "../../build/config/sdkconfig.h"

#include <string.h>

#include "ezlopi_core_wifi.h"
#include "ezlopi_core_ethernet.h"
#include "ezlopi_core_ping.h"

#if defined(CONFIG_EZPI_CORE_CUSTOM_WSC_LIB)
#include "ezlopi_core_wsc.h"
#elif defined(CONFIG_EZPI_CORE_IDF_WSC_LIB)
#include "ezlopi_core_websocket_client.h"
#endif

typedef struct s_ezlopi_net_status
{
    ezlopi_wifi_status_t* wifi_status;
    e_ping_status_t internet_status;
    bool nma_cloud_connection_status;
} s_ezlopi_net_status_t;

#ifdef CONFIG_EZPI_CORE_ENABLE_ETH
void ezlopi_net_init(void);
#endif  // CONFIG_EZPI_CORE_ENABLE_ETH

s_ezlopi_net_status_t* ezlopi_get_net_status(void);


#endif // _EZLOPI_CORE_NET_H_
