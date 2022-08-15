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

#include "esp_system.h"
#include "sys/queue.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "driver/gpio.h"
#include "driver/uart.h"

#include "mbedtls/platform.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/esp_debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"

#include "sdkconfig.h"
#include "frozen.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    enum state_wss
    {
        HEADER,
        DATA,
    };

    typedef void (*wss_upcall_t)(const char *payload, uint32_t len);

    void wss_client_init(struct json_token *uri, wss_upcall_t wss_upcall);
    int wss_client_send(char *buf_s, size_t len);
    bool wss_client_is_connected(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __WSS_H__
