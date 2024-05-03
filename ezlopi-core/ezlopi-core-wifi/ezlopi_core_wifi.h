#ifndef _EZLOPI_CORE_WIFI_H_
#define _EZLOPI_CORE_WIFI_H_

#include "esp_netif_types.h"
#include "esp_wifi_types.h"
#include "cjext.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef void (*f_ezlopi_wifi_event_upcall)(esp_event_base_t event, int32_t event_id, void* arg);

    typedef struct ll_ezlopi_wifi_event_upcall
    {
        void* arg;
        f_ezlopi_wifi_event_upcall upcall;
        struct ll_ezlopi_wifi_event_upcall* next;
    } ll_ezlopi_wifi_event_upcall_t;

    typedef struct ezlopi_wifi_status
    {
        bool wifi_connection;
        esp_netif_ip_info_t* ip_info;
        wifi_mode_t wifi_mode;
    } ezlopi_wifi_status_t;

    int ezlopi_wifi_got_ip(void);
    void ezlopi_wifi_set_new_wifi_flag(void);
    void ezlopi_wifi_initialize(void);
    esp_err_t ezlopi_wifi_connect(const char* ssid, const char* pass);
    void ezlopi_wifi_connect_from_id_bin(void);
    int ezlopi_wait_for_wifi_to_connect(uint32_t wait_time_ms);
    esp_netif_ip_info_t* ezlopi_wifi_get_ip_infos(void);
    const char* ezlopi_wifi_get_last_disconnect_reason(void);

    ezlopi_wifi_status_t* ezlopi_wifi_status(void);

    void ezlopi_wifi_event_add(f_ezlopi_wifi_event_upcall upcall, void* arg);

    int ezlopi_wifi_get_wifi_mac(uint8_t mac[6]);

    void ezlopi_wifi_scan_start();
    void ezlopi_wifi_scan_stop();

#ifdef __cplusplus
}
#endif
#endif // _EZLOPI_CORE_WIFI_H_