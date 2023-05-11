#ifndef __EZLOPI_WIFI_H__
#define __EZLOPI_WIFI_H__

#include "esp_netif_types.h"
#include "esp_wifi_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef void (*f_ezlopi_wifi_event_upcall)(esp_event_base_t event, void *arg);

    typedef struct ll_ezlopi_wifi_event_upcall
    {
        void *arg;
        f_ezlopi_wifi_event_upcall upcall;
        struct ll_ezlopi_wifi_event_upcall *next;
    } ll_ezlopi_wifi_event_upcall_t;

    typedef struct ezlopi_wifi_status 
    {
        bool wifi_connection;
        esp_netif_ip_info_t * ip_info;
        wifi_mode_t wifi_mode;
    } ezlopi_wifi_status_t; 

    int ezlopi_wifi_got_ip(void);
    void ezlopi_wifi_set_new_wifi_flag(void);
    void ezlopi_wifi_initialize(void);
    esp_err_t ezlopi_wifi_connect(const char *ssid, const char *pass);
    void ezlopi_wifi_connect_from_nvs(void);
    void ezlopi_wifi_connect_from_id_bin(void);
    void ezlopi_wait_for_wifi_to_connect(void);
    esp_netif_ip_info_t *ezlopi_wifi_get_ip_infos(void);
    const char *ezlopi_wifi_get_last_disconnect_reason(void);

    ezlopi_wifi_status_t * ezlopi_wifi_status(void);

    void ezlopi_wifi_event_add(f_ezlopi_wifi_event_upcall *upcall, void *arg);

#ifdef __cplusplus
}
#endif
#endif // __EZLOPI_WIFI_H__