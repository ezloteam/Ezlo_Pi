#ifndef __WIFI_H__
#define __WIFI_H__

#include "esp_netif_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

    int got_ip_c(void);
    void set_new_wifi_flag_c(void);
    char *get_current_wifi_creds_c(void);
    void wifi_connect_c(const char *ssid, const char *pass);

    // void wifi_init_sta(void);
    void set_new_wifi_flag(void);
    void wifi_initialize(void);
    void wifi_connect(const char *ssid, const char *pass);
    void wifi_connect_from_nvs(void);
    void wait_for_wifi_to_connect(void);
    esp_netif_ip_info_t *wifi_get_ip_infos(void);

#ifdef __cplusplus
}
#endif
#endif // __WIFI_H__