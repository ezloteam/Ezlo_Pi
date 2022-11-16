#ifndef __EZLOPI_WIFI_H__
#define __EZLOPI_WIFI_H__

#include "esp_netif_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

    int ezlopi_wifi_got_ip(void);
    void ezlopi_wifi_set_new_wifi_flag(void);
    void ezlopi_wifi_initialize(void);
    esp_err_t ezlopi_wifi_connect(const char *ssid, const char *pass);
    void ezlopi_wifi_connect_from_nvs(void);
    void ezlopi_wait_for_wifi_to_connect(void);
    esp_netif_ip_info_t *ezlopi_wifi_get_ip_infos(void);

#ifdef __cplusplus
}
#endif
#endif // __EZLOPI_WIFI_H__