/**
 * @file wss.h
 * @author samogon
 * @brief
 * @version 0.1
 * @date 11.02.20
 *
 * @copyright Copyright (c) 2022
 *
 * @author Krishna Kumar Sah (krriss.shah@gmail.com)
 * @date Modified: 29-7-2022
 *
 */

#ifndef __WSS_H__
#define __WSS_H__

#include <stdio.h>
#include "string.h"
#include "stdbool.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "esp_wifi.h"
#include "sys/queue.h"
#include "esp_event.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_wifi_types.h"

#include "mbedtls/platform.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/esp_debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"

#include "cjext.h"
#include "sdkconfig.h"


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    enum state_wss
    {
        HEADER,
        DATA,
    };

    typedef void (*f_wsc_conn_upcall_t)(bool connected);
    typedef void (*f_wsc_msg_upcall_t)(const char *payload, uint32_t len);

    void ezlopi_core_wsc_kill(void);
    void ezlopi_core_wsc_init(cJSON *uri, f_wsc_msg_upcall_t __message_upcall, f_wsc_conn_upcall_t __connection_upcall);

    int ezlopi_core_wsc_send(char *buf_s, size_t len);
    bool ezlopi_core_wsc_is_connected(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __WSS_H__
