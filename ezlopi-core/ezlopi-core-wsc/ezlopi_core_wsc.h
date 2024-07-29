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

    typedef void (*f_wsc_conn_upcall_t)(bool connected);
    typedef void (*f_wsc_msg_upcall_t)(const char *payload, uint32_t len);

    typedef enum e_state_wss
    {
        STATE_DATA,
        STATE_HEADER,
    } e_state_wss_t;

    typedef struct s_ssl_websocket {
        char url[64];
        char port[8];
        char * buffer;
        uint32_t buffer_len;

        bool is_connected;
        e_state_wss_t e_state;

        mbedtls_ssl_config * conf;
        mbedtls_ssl_context * ssl_ctx;
        mbedtls_net_context * server_fd;
        mbedtls_entropy_context * entropy;
        mbedtls_ctr_drbg_context * ctr_drbg;

        mbedtls_x509_crt * cacert;
        mbedtls_x509_crt * shared_cert;
        mbedtls_pk_context * private_key;

        char * str_cacert;
        char * str_private_key;
        char * str_shared_cert;

        TimerHandle_t timer;
        TaskHandle_t task_handle;
        f_wsc_msg_upcall_t message_upcall_func;
        f_wsc_conn_upcall_t connection_upcall_func;

    } s_ssl_websocket_t;

    int ezlopi_core_wsc_kill(s_ssl_websocket_t * wsc_ssl);
    s_ssl_websocket_t * ezlopi_core_wsc_init(cJSON *uri, f_wsc_msg_upcall_t __message_upcall, f_wsc_conn_upcall_t __connection_upcall);

    int ezlopi_core_wsc_send(s_ssl_websocket_t * wsc_ssl, char *buf_s, size_t len);
    int ezlopi_core_wsc_is_connected(s_ssl_websocket_t * wsc_ssl);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __WSS_H__
