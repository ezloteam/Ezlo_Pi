// Created by krishna on July 29, 2024.

#include "time.h"
#include "lwip/apps/sntp.h"
#include "mbedtls/net_sockets.h"

#include "ezlopi_core_heap.h"
#include "ezlopi_core_factory_info.h"

#include "ezlopi_util_trace.h"
#include "EZLOPI_USER_CONFIG.h"

#include "ezlopi_core_wsc.h"

static const char *__request_format = "GET / HTTP/1.1\n"
"Host: %s\n"
"Connection: Upgrade\n"
"Upgrade: websocket\n"
"Origin: %s\n"
"Sec-WebSocket-Version: 13\n"
"User-Agent: EzloPi-%llu\n"
"Sec-WebSocket-Key: JSUlJXYeLiJ2Hi4idh4uIg==\r\n\r\n";

static void __rx_task(void *arg);
static int __init_mbedtls(s_ssl_websocket_t *ssl_wsc);
static int __deinit_mbedtls(s_ssl_websocket_t * wsc_ssl);
static int __zap_aut(unsigned char *ttx, unsigned char *text);
static void __setup_wsc_request(cJSON * cj_uri, s_ssl_websocket_t * ssl_ws);
static int __send_internal(s_ssl_websocket_t * wsc_ssl, char *buf_s, size_t len, uint32_t timeout_ms);
static void __timer_callback(TimerHandle_t timer);
static int __upgrade_to_websocket(s_ssl_websocket_t * ssl_wsc);
static int __rx_func(s_ssl_websocket_t * ssl_wsc);


int ezlopi_core_wsc_is_connected(s_ssl_websocket_t * wsc_ssl)
{
    int ret = 0;

    if (wsc_ssl)
    {
        ret = wsc_ssl->is_connected;
    }

    return ret;
}

int ezlopi_core_wsc_kill(s_ssl_websocket_t * wsc_ssl)
{
    int ret = 0;

    // if (wsc_ssl)
    // {
    //     mbedtls_net_close(wsc_ssl->server_fd);
    // }

    return ret;
}

int ezlopi_core_wsc_stop(s_ssl_websocket_t * wsc_ssl)
{
    int ret = 0;

    if (wsc_ssl)
    {
        close(wsc_ssl->server_fd->fd);
    }

    return ret;
}

int ezlopi_core_wsc_send(s_ssl_websocket_t * ssl_wsc, char *buf, size_t len)
{
    int ret = 0;

    if (ssl_wsc && ssl_wsc->is_connected && buf && len)
    {
        ret = __send_internal(ssl_wsc, buf, len, 2000);
    }

    return ret;
}

s_ssl_websocket_t * ezlopi_core_wsc_init(cJSON * uri, f_wsc_msg_upcall_t __message_upcall, f_wsc_conn_upcall_t __connection_upcall)
{
    s_ssl_websocket_t * ssl_ws = NULL;

    if (uri && uri->valuestring)
    {
        ssl_ws = ezlopi_malloc(__FUNCTION__, sizeof(s_ssl_websocket_t));

        if (ssl_ws)
        {
            memset(ssl_ws, 0, sizeof(s_ssl_websocket_t));

            ssl_ws->message_upcall_func = __message_upcall;
            ssl_ws->connection_upcall_func = __connection_upcall;

            __setup_wsc_request(uri, ssl_ws);
#if 0
            snprintf(__wsc_buffer, sizeof(__wsc_buffer), "%.*s", uri->str_value_len, uri->valuestring);
            char *port_start = strrchr(__wsc_buffer, ':');

            if (port_start)
            {
                snprintf(__wsc_port, sizeof(__wsc_port), "%d", atoi(port_start + 1));
                url_len = port_start - __wsc_buffer;
            }

            snprintf(__wsc_url, sizeof(__wsc_url), "%.*s", (uri->str_value_len - 6), (uri->valuestring + 6));
            snprintf(__request, sizeof(__request), __request_format, __wsc_url, __wsc_url, ezlopi_factory_info_v3_get_id());

            TRACE_D("__request_format: %s", __request);
#endif

            ssl_ws->timer = xTimerCreate("wsc-timer", (1000 / portTICK_RATE_MS), pdTRUE, ssl_ws, __timer_callback);
            xTaskCreate(__rx_task, "wsc-rx_task", 6 * 1024, ssl_ws, 5, &ssl_ws->task_handle);
        }
    }

    return ssl_ws;
}

static int __init_mbedtls(s_ssl_websocket_t * wsc_ssl)
{
    int ret = 0;

    do {
        wsc_ssl->conf = ezlopi_malloc(__FUNCTION__, sizeof(mbedtls_ssl_config));
        wsc_ssl->ssl_ctx = ezlopi_malloc(__FUNCTION__, sizeof(mbedtls_ssl_context));
        wsc_ssl->server_fd = ezlopi_malloc(__FUNCTION__, sizeof(mbedtls_net_context));
        wsc_ssl->entropy = ezlopi_malloc(__FUNCTION__, sizeof(mbedtls_entropy_context));
        wsc_ssl->ctr_drbg = ezlopi_malloc(__FUNCTION__, sizeof(mbedtls_ctr_drbg_context));

        wsc_ssl->cacert = ezlopi_malloc(__FUNCTION__, sizeof(mbedtls_x509_crt));
        wsc_ssl->shared_cert = ezlopi_malloc(__FUNCTION__, sizeof(mbedtls_x509_crt));
        wsc_ssl->private_key = ezlopi_malloc(__FUNCTION__, sizeof(mbedtls_pk_context));

        wsc_ssl->str_cacert = ezlopi_factory_info_v3_get_ca_certificate();
        wsc_ssl->str_shared_cert = ezlopi_factory_info_v3_get_ssl_shared_key();
        wsc_ssl->str_private_key = ezlopi_factory_info_v3_get_ssl_private_key();

        wsc_ssl->buffer_len = 4096;
        wsc_ssl->buffer = ezlopi_malloc(__FUNCTION__, wsc_ssl->buffer_len);

        if (wsc_ssl->conf && wsc_ssl->ssl_ctx && wsc_ssl->server_fd && wsc_ssl->entropy && wsc_ssl->ctr_drbg &&
            wsc_ssl->cacert && wsc_ssl->shared_cert && wsc_ssl->private_key &&
            wsc_ssl->str_cacert && wsc_ssl->str_shared_cert && wsc_ssl->str_private_key &&
            wsc_ssl->buffer)
        {
            mbedtls_ssl_init(wsc_ssl->ssl_ctx);
            mbedtls_ssl_config_init(wsc_ssl->conf);
            mbedtls_entropy_init(wsc_ssl->entropy);
            mbedtls_ctr_drbg_init(wsc_ssl->ctr_drbg);

            mbedtls_pk_init(wsc_ssl->private_key);
            mbedtls_x509_crt_init(wsc_ssl->cacert);
            mbedtls_x509_crt_init(wsc_ssl->shared_cert);


            if ((ret = mbedtls_ctr_drbg_seed(wsc_ssl->ctr_drbg, mbedtls_entropy_func, wsc_ssl->entropy, NULL, 0)) != 0)//
            {
                ret = -1;
                TRACE_E("mbedtls_ctr_drbg_seed returned %d", ret);
                break;
            }

            ret = mbedtls_x509_crt_parse(wsc_ssl->cacert, (uint8_t *)wsc_ssl->str_cacert, strlen(wsc_ssl->str_cacert) + 1);
            TRACE_W("RET: -0x%04x [%d]", -ret, ret);

            ret |= mbedtls_x509_crt_parse(wsc_ssl->shared_cert, (uint8_t *)wsc_ssl->str_shared_cert, strlen(wsc_ssl->str_shared_cert) + 1);
            TRACE_W("RET: -0x%04x [%d]", -ret, ret);

            ret |= mbedtls_pk_parse_key(wsc_ssl->private_key, (uint8_t *)wsc_ssl->str_private_key, strlen(wsc_ssl->str_private_key) + 1, NULL, 0);
            TRACE_W("RET: -0x%04x [%d]", -ret, ret);

            if (ret >= 0)
            {
                if ((ret = mbedtls_ssl_set_hostname(wsc_ssl->ssl_ctx, wsc_ssl->url)) != 0)
                {
                    ret = -1;
                    TRACE_E("mbedtls_ssl_set_hostname returned -0x%x", -ret);
                    break;
                }

                if ((ret = mbedtls_ssl_config_defaults(wsc_ssl->conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
                {
                    ret = -1;
                    TRACE_E("mbedtls_ssl_config_defaults returned %d", ret);
                    break;
                }

                mbedtls_ssl_conf_authmode(wsc_ssl->conf, MBEDTLS_SSL_VERIFY_NONE);
                mbedtls_ssl_conf_ca_chain(wsc_ssl->conf, wsc_ssl->cacert, NULL);
                mbedtls_ssl_conf_own_cert(wsc_ssl->conf, wsc_ssl->shared_cert, wsc_ssl->private_key);

                mbedtls_ssl_conf_rng(wsc_ssl->conf, mbedtls_ctr_drbg_random, wsc_ssl->ctr_drbg);
                // mbedtls_ssl_conf_read_timeout(wsc_ssl->conf, 1000);

                if ((ret = mbedtls_ssl_setup(wsc_ssl->ssl_ctx, wsc_ssl->conf)) != 0)
                {
                    ret = -1;
                    TRACE_E("mbedtls_ssl_setup returned -0x%x\n\n", -ret);
                    break;
                }

                ret = 1;
            }
        }
        else
        {
            ret = -1;
            TRACE_E("'malloc' failed!");
            break;
        }
    } while (0);

    if (ret <= 0)
    {
        TRACE_E("Some error occured");
    }

    return ret;
}

static int __deinit_mbedtls(s_ssl_websocket_t * wsc_ssl)
{
    int ret = 0;

    if (wsc_ssl)
    {
        mbedtls_ssl_config_free(wsc_ssl->conf);
        ezlopi_free(__FUNCTION__, wsc_ssl->conf);
        wsc_ssl->conf = NULL;

        mbedtls_ssl_free(wsc_ssl->ssl_ctx);
        ezlopi_free(__FUNCTION__, wsc_ssl->ssl_ctx);
        wsc_ssl->ssl_ctx = NULL;

        mbedtls_net_free(wsc_ssl->server_fd);
        ezlopi_free(__FUNCTION__, wsc_ssl->server_fd);
        wsc_ssl->server_fd = NULL;

        mbedtls_entropy_free(wsc_ssl->entropy);
        ezlopi_free(__FUNCTION__, wsc_ssl->entropy);
        wsc_ssl->entropy = NULL;

        mbedtls_ctr_drbg_free(wsc_ssl->ctr_drbg);
        ezlopi_free(__FUNCTION__, wsc_ssl->ctr_drbg);
        wsc_ssl->ctr_drbg = NULL;


        mbedtls_x509_crt_free(wsc_ssl->cacert);
        ezlopi_free(__FUNCTION__, wsc_ssl->cacert);
        wsc_ssl->cacert = NULL;

        mbedtls_x509_crt_free(wsc_ssl->shared_cert);
        ezlopi_free(__FUNCTION__, wsc_ssl->shared_cert);
        wsc_ssl->shared_cert = NULL;

        mbedtls_pk_free(wsc_ssl->private_key);
        ezlopi_free(__FUNCTION__, wsc_ssl->private_key);
        wsc_ssl->private_key = NULL;

        ezlopi_free(__FUNCTION__, wsc_ssl->buffer);
        wsc_ssl->buffer = NULL;
        wsc_ssl->buffer_len = 0;

        ret = 1;
    }

    return ret;
}

static void __setup_wsc_request(cJSON * cj_uri, s_ssl_websocket_t * ssl_wsc)
{
    if (cj_uri && cj_uri->valuestring && ssl_wsc && ssl_wsc->url)
    {
        uint32_t url_len = 0;
        char tmp_buf[cj_uri->str_value_len + 1];

        snprintf(tmp_buf, sizeof(tmp_buf), "%.*s", cj_uri->str_value_len, cj_uri->valuestring);
        char *port_start = strrchr(tmp_buf, ':');

        if (port_start)
        {
            snprintf(ssl_wsc->port, sizeof(ssl_wsc->port), "%d", atoi(port_start + 1));
            url_len = port_start - tmp_buf;
        }

        snprintf(ssl_wsc->url, sizeof(ssl_wsc->url), "%.*s", url_len - 6, cj_uri->valuestring + 6);

        // snprintf(__request, sizeof(__request), __request_format, __wsc_url, __wsc_url, ezlopi_factory_info_v3_get_id());
        // TRACE_D("__request_format: %s", __request);
    }
}

static int __send_internal(s_ssl_websocket_t * wsc_ssl, char *buf_s, size_t len, uint32_t timeout)
{
    int ret = 0;

    if (wsc_ssl)
    {
        size_t written_bytes = 0;
        unsigned char *buf = (unsigned char *)buf_s;

        if (wsc_ssl->is_connected)
        {
            unsigned char *buf1 = (unsigned char *)malloc(len + 8);
            if (buf1)
            {
                len = __zap_aut(buf, buf1);
                buf = buf1;
            }
            else
            {
                written_bytes = MBEDTLS_ERR_SSL_ALLOC_FAILED;
            }
        }

        do
        {
            struct timeval time_val;
            fd_set write_fds;
            int fd = wsc_ssl->server_fd->fd;

            if (fd < 0)
            {
                return (-1);
            }

            FD_ZERO(&write_fds);
            FD_SET(fd, &write_fds);

            time_val.tv_sec = timeout / 1000;
            time_val.tv_usec = timeout ? ((timeout % 1000) * 1000) : 1;

            do {
                TRACE_W("waiting for write-ready.......");
                vTaskDelay(1 / portTICK_RATE_MS);
                ret = select(fd + 1, NULL, &write_fds, NULL, &time_val);
            } while (ret == 4);

            // mbedtls_net_poll(wsc_ssl->server_fd, MBEDTLS_NET_POLL_WRITE, timeout);

            /* Zero fds ready means we timed out */
            if (ret <= 0)
            {
                TRACE_E("POLL FAILED!");
                return (-1);
            }

            TRACE_W("POLL SUCCESS");

            ret = mbedtls_ssl_write(wsc_ssl->ssl_ctx, (const unsigned char *)buf + written_bytes, len - written_bytes);
            if (ret >= 0)
            {
                written_bytes += ret;
            }
            else if (ret != MBEDTLS_ERR_SSL_WANT_WRITE && ret != MBEDTLS_ERR_SSL_WANT_READ)
            {
                TRACE_E("mbedtls_ssl_write returned -0x%x", -ret);
                return ret;
            }
        } while (written_bytes < len);

        TRACE_W("write-success");

        if ((char *)buf != buf_s)
        {
            free(buf);
        }
    }

    return ret;
}

static int __zap_aut(unsigned char *ttx, unsigned char *text)
{
    int i;
    int len1 = 0;
    const unsigned char mask[4] = { 0x10, 0x55, 0x21, 0x43 };

    // TRACE_I("Sending dan to wss:: len: %d\r\n%s", strlen((char *)ttx), ttx);

    text[0] = 0x81;
    if (strlen((char *)ttx) < 126)
    {
        text[1] = (strlen((char *)ttx) | 0x80) & 0xff;
    }
    else
    {
        len1 = 2;
        text[1] = 126 | 0x80;
        text[2] = (strlen((char *)ttx) >> 8) & 0xff;
        text[3] = strlen((char *)ttx) & 0xff;
    }

    text[2 + len1] = mask[0];
    text[3 + len1] = mask[1];
    text[4 + len1] = mask[2];
    text[5 + len1] = mask[3];

    for (i = 0; i < strlen((char *)ttx); i++)
    {
        text[6 + i + len1] = ttx[i] ^ mask[i % 4];
    }
    len1 = strlen((char *)ttx) + 6 + len1;

    return len1;
}

static void __rx_task(void *arg)
{
    s_ssl_websocket_t * __ssl_wsc = (s_ssl_websocket_t *)arg;
    if (__ssl_wsc)
    {
        while (1)
        {
            do {
                int ret = 0;

                if (__init_mbedtls(__ssl_wsc) > 0)
                {
                    TRACE_D("mbedtls-init success.");

                    mbedtls_net_init(__ssl_wsc->server_fd);
                    mbedtls_net_set_nonblock(__ssl_wsc->server_fd);

                    TRACE_I("Connecting to  '%s:%s'...", __ssl_wsc->url, __ssl_wsc->port);

                    if ((ret = mbedtls_net_connect(__ssl_wsc->server_fd, __ssl_wsc->url, __ssl_wsc->port, MBEDTLS_NET_PROTO_TCP)) != 0)
                    {
                        TRACE_E("mbedtls_net_connect returned -%x", -ret);
                        break;
                    }

                    TRACE_I("Connected.");

                    mbedtls_ssl_set_bio(__ssl_wsc->ssl_ctx, __ssl_wsc->server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

                    TRACE_I("Performing the SSL/TLS handshake...");

                    while ((ret = mbedtls_ssl_handshake(__ssl_wsc->ssl_ctx)) != 0)
                    {
                        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
                        {
                            TRACE_E("mbedtls_ssl_handshake returned -0x%x", -ret);
                            break;
                        }
                    }

                    TRACE_S("Handshake complete");
                    TRACE_I("Verifying peer X.509 certificate...");

                    int flags = 0;

                    if ((flags = mbedtls_ssl_get_verify_result(__ssl_wsc->ssl_ctx)) != 0)
                    {
                        /* In real life, we probably want to close connection if ret != 0 */
                        TRACE_W("Failed to verify peer certificate!");
                        memset(__ssl_wsc->buffer, 0, __ssl_wsc->buffer_len);
                        mbedtls_x509_crt_verify_info(__ssl_wsc->buffer, sizeof(__ssl_wsc->buffer), "  ! ", flags);
                        TRACE_W("verification info: %s", __ssl_wsc->buffer);
                    }
                    else
                    {
                        TRACE_I("Certificate verified.");
                    }

                    TRACE_I("Cipher suite is %s", mbedtls_ssl_get_ciphersuite(__ssl_wsc->ssl_ctx));

                    __ssl_wsc->e_state = STATE_HEADER;

                    if (__upgrade_to_websocket(__ssl_wsc))
                    {
                        int retry = 30;

                        while (__ssl_wsc->is_connected)
                        {
                            vTaskDelay(20 / portTICK_RATE_MS);

                            if (__rx_func(__ssl_wsc) < 0)
                            {
                                TRACE_E("'rx-func' failed!, retry: %d", retry);

                                if (0 == retry--)
                                {
                                    __ssl_wsc->is_connected = false;
                                    if (__ssl_wsc->connection_upcall_func) {
                                        __ssl_wsc->connection_upcall_func(false);
                                    }

                                    break;
                                }

                            }
                            else
                            {
                                retry = 3;
                            }
                        }
                    }

                    TRACE_E("Web-socket disconnected!");
                    TRACE_W("Releasing 'mbedtls' resources!");

                    mbedtls_ssl_close_notify(__ssl_wsc->ssl_ctx);
                    mbedtls_ssl_session_reset(__ssl_wsc->ssl_ctx);

                    // mbedtls_net_free(__ssl_wsc->server_fd);
                    // __ssl_wsc->server_fd = NULL;
                }
            } while (0);

            TRACE_I("--deinit-mbedtls");
            __deinit_mbedtls(__ssl_wsc);
            vTaskDelay(1000 / portTICK_RATE_MS);
        }
    }

    vTaskDelete(NULL);
}

static int __upgrade_to_websocket(s_ssl_websocket_t * ssl_wsc)
{
    int ret = 0;

    if (ssl_wsc)
    {
        do {
            ssl_wsc->is_connected = false;
            TRACE_I("Writing HTTP __request...");

            snprintf(ssl_wsc->buffer, ssl_wsc->buffer_len, __request_format, ssl_wsc->url, ssl_wsc->url, ezlopi_factory_info_v3_get_id());
            TRACE_D("__request_format: \r\n%s", ssl_wsc->buffer ? ssl_wsc->buffer : "null");

            ret = __send_internal(ssl_wsc, ssl_wsc->buffer, strlen(ssl_wsc->buffer), 5000);

            if (ret > 0)
            {
                TRACE_S("WSC-upgrade request sent. ret: %d", ret);

                int read_len = 0;
                memset(ssl_wsc->buffer, 0, ssl_wsc->buffer_len);
                ret = mbedtls_ssl_read(ssl_wsc->ssl_ctx, (uint8_t *)ssl_wsc->buffer, ssl_wsc->buffer_len);

                if ((ret == MBEDTLS_ERR_SSL_WANT_READ) || (ret == MBEDTLS_ERR_SSL_WANT_WRITE))
                {
                    read_len += ret;
                    continue;
                }

                if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)
                {
                    TRACE_E("peer closed connection!");

                    ret = 0;
                    ssl_wsc->is_connected = false;
                    if (ssl_wsc->connection_upcall_func) {
                        ssl_wsc->connection_upcall_func(false);
                    }

                    break;
                }

                if (ret < 0)
                {
                    TRACE_E("connection closed");

                    ssl_wsc->is_connected = false;
                    if (ssl_wsc->connection_upcall_func) {
                        ssl_wsc->connection_upcall_func(false);
                    }

                    TRACE_E("mbedtls_ssl_read returned -0x%x", -ret);
                    break;
                }

                if (ret == 0)
                {
                    TRACE_E("connection closed");

                    ssl_wsc->is_connected = false;
                    if (ssl_wsc->connection_upcall_func) {
                        ssl_wsc->connection_upcall_func(false);
                    }

                    TRACE_E("mbedtls_ssl_read returned -0x%x", -ret);
                    break;
                }

                read_len = ret;
                TRACE_I("%d bytes read", read_len);
                ssl_wsc->buffer[read_len] = 0;

                TRACE_I("%s", ssl_wsc->buffer);

                if (strstr(ssl_wsc->buffer, "websocket"))
                {
                    ret = 1;
                    TRACE_I("WSC Connected.");
                    ssl_wsc->is_connected = true;
                    if (ssl_wsc->connection_upcall_func) {
                        ssl_wsc->connection_upcall_func(true);
                    }
                    break;
                }
            }
            else
            {
                TRACE_E("sending-failed!");
            }

            vTaskDelay(1000 / portTICK_RATE_MS);

        } while (ssl_wsc->is_connected == false);
    }

    return ret;
}

static void __timer_callback(TimerHandle_t timer)
{
    TRACE_I("timer-callback");
    s_ssl_websocket_t* wss_ssl = (s_ssl_websocket_t *)pvTimerGetTimerID(timer);
    ezlopi_core_wsc_stop(wss_ssl);
}

static int __rx_func(s_ssl_websocket_t * ssl_wsc)
{
    int ret = 0;

    if (ssl_wsc && ssl_wsc->is_connected)
    {
        while (1)
        {

            int retry = 5;

            do
            {
                struct timeval time_val;
                fd_set read_fds;
                int fd = ssl_wsc->server_fd->fd;

                if (fd < 0)
                {
                    return (-1);
                }

                FD_ZERO(&read_fds);
                FD_SET(fd, &read_fds);

                time_val.tv_sec = 0;
                time_val.tv_usec = 1;

                ret = select(fd + 1, &read_fds, NULL, NULL, &time_val);

                /* Zero fds ready means we timed out */
                if (ret == 0)
                {
                    return 0;
                }
                else if (ret < 0)
                {
                    return (-1);
                }


                ret = mbedtls_ssl_read(ssl_wsc->ssl_ctx, (uint8_t *)ssl_wsc->buffer, ssl_wsc->buffer_len);

                if (ret < 0)
                {
                    TRACE_I("mbedtls_ssl_read returned: -0x%04x [%d]", -ret, ret);

                    if (retry == 0) {
                        return (-1);
                    }
                }
                else if (ret >= 0)
                {
                    break;
                }

                vTaskDelay(5);

            } while (retry--);

            if (ssl_wsc->e_state == STATE_HEADER)
            {
                if (ssl_wsc->buffer[0] == 0x88)
                {
                    return -1;
                }

                if (ssl_wsc->buffer[0] == 0x89) // if PING then PONG
                {
                    TRACE_I("PING PONG");
                    ssl_wsc->buffer[0] = 0x8A;
                    mbedtls_ssl_write(ssl_wsc->ssl_ctx, (uint8_t *)ssl_wsc->buffer, ret);
                }
                else
                {
                    if (ssl_wsc->buffer[0] == 0x81 || ssl_wsc->buffer[0] == 0x01)
                    {
                        ssl_wsc->e_state = STATE_DATA;
                    }
                }
            }
            else
            {
                ssl_wsc->buffer[ret] = 0;
                // TRACE_I("<< WSS-Rx:%s\r\n", __wsc_buffer);
                if (ssl_wsc->message_upcall_func)
                {
                    ssl_wsc->message_upcall_func(ssl_wsc->buffer, strlen(ssl_wsc->buffer));
                }

                ssl_wsc->e_state = STATE_HEADER;
            }

        }
    }

    return ret;
}
