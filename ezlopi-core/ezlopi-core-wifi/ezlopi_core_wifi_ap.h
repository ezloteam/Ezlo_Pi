
#include "sdkconfig.h"

#ifdef CONFIG_EZPI_ENABLE_CAPTIVE_PORTAL

#ifndef _EZLOPI_CORE_WIFI_AP_H_
#define _EZLOPI_CORE_WIFI_AP_H_

void ezlopi_configure_wifi_ap();
void ezlopi_deconfigure_wifi_ap();
void ezlopi_start_dns_server();

#endif // _EZLOPI_CORE_WIFI_AP_H_

#endif // CONFIG_EZPI_ENABLE_CAPTIVE_PORTAL

