//
// Created by samogon on 11.02.20.
//

#include "time.h"
#include "lwip/apps/sntp.h"

#include "ezlopi_core_wsc.h"
#include "ezlopi_core_factory_info.h"


#include "ezlopi_util_trace.h"
#include "EZLOPI_USER_CONFIG.h"

static bool __is_wsc = false;

static char __wsc_url[64];
static char __wsc_port[8];
static char __wsc_buffer[4096];

enum state_wss cur_state_wss = HEADER;

static mbedtls_net_context server_fd;
static mbedtls_ssl_context ssl;
static mbedtls_entropy_context entropy;
static mbedtls_ctr_drbg_context ctr_drbg;
static mbedtls_x509_crt cacert;
static mbedtls_ssl_config conf;
static mbedtls_pk_context p_key;

static char __request[512];
static const char *__request_format = "GET / HTTP/1.1\n"
"Host: %s\n"
"Connection: Upgrade\n"
"Upgrade: websocket\n"
"Origin: %s\n"
"Sec-WebSocket-Version: 13\n"
"User-Agent: EzloPi-%llu\n"
"Sec-WebSocket-Key: JSUlJXYeLiJ2Hi4idh4uIg==\r\n"
"\r\n";

static f_wsc_msg_upcall_t __message_upcall_func = NULL;
static f_wsc_conn_upcall_t __connection_upcall_func = NULL;

static void __rx_func(void);
static void __rx_task(void *pvParameters);
static int __zap_aut(unsigned char *ttx, unsigned char *text);
static int __send_internal(char *buf_s, size_t len);

int ezlopi_core_wsc_send(char *buf, size_t len)
{
    if (__is_wsc && buf && len)
    {
        return __send_internal(buf, len);
    }

    return 0;
}

bool ezlopi_core_wsc_is_connected(void)
{
    return __is_wsc;
}

void ezlopi_core_wsc_kill(void)
{

}

void ezlopi_core_wsc_init(cJSON * uri, f_wsc_msg_upcall_t __message_upcall, f_wsc_conn_upcall_t __connection_upcall)
{
    if (uri && uri->valuestring)
    {
        uint32_t url_len = 0;

        __message_upcall_func = __message_upcall;
        __connection_upcall_func = __connection_upcall;

        snprintf(__wsc_buffer, sizeof(__wsc_buffer), "%.*s", uri->str_value_len, uri->valuestring);
        char *port_start = strrchr(__wsc_buffer, ':');

        if (port_start)
        {
            snprintf(__wsc_port, sizeof(__wsc_port), "%d", atoi(port_start + 1));
            url_len = port_start - __wsc_buffer;
        }

        snprintf(__wsc_url, sizeof(__wsc_url), "%.*s", url_len - 6, uri->valuestring + 6);
        snprintf(__request, sizeof(__request), __request_format, __wsc_url, __wsc_url, ezlopi_factory_info_v3_get_id());

        TRACE_D("__request_format: %s", __request);

        xTaskCreate(__rx_task, "__rx_task", 6 * 1024, NULL, 5, NULL);
    }
}

static void __rx_func(void)
{
    if (__is_wsc)
    {
        int ret;
        fd_set _read_fds;
        struct timeval _time_val;
        int fd = ((mbedtls_net_context *)&server_fd)->fd;

        if (fd < 0)
        {
            __is_wsc = false;
            if (__connection_upcall_func)
                __connection_upcall_func(false);
            return;
        }

        FD_ZERO(&_read_fds);
        FD_SET(fd, &_read_fds);

        _time_val.tv_sec = 0;
        _time_val.tv_usec = 1;

        ret = select(fd + 1, &_read_fds, NULL, NULL, &_time_val);

        /* Zero fds ready means we timed out */
        if (ret == 0)
        {
            return;
        }

        if (ret < 0)
        {
            __is_wsc = false;
            if (__connection_upcall_func)
                __connection_upcall_func(false);
            return;
        }

        // mbedtls_ssl_conf_read_timeout();
        // ssl.conf->read_timeout

        int retry = 2;

        do
        {
            ret = mbedtls_ssl_read(&ssl, (unsigned char *)&__wsc_buffer[0], sizeof(__wsc_buffer));
            // TRACE_I("mbedtls_ssl_read read %d byte", ret);
            // dump(__wsc_buffer, 0, ret);

            if (ret <= 0 && retry)
            {
                __is_wsc = false;
                if (__connection_upcall_func)
                    __connection_upcall_func(false);

                TRACE_I("connection closed, ret: -0x%x\n\n", -ret);
                return;
            }
            else
            {
                break;
            }
        } while (--retry);

        if (cur_state_wss == HEADER)
        {
            if (__wsc_buffer[0] == 0x88)
            {
                TRACE_I("connection closed");

                __is_wsc = false;
                if (__connection_upcall_func)
                    __connection_upcall_func(false);

                return;
            }

            if (__wsc_buffer[0] == 0x89) // if PING then PONG
            {
                TRACE_I("PING PONG");
                __wsc_buffer[0] = 0x8A;
                mbedtls_ssl_write(&ssl, (uint8_t *)__wsc_buffer, ret);
            }
            else
            {
                if (__wsc_buffer[0] == 0x81 || __wsc_buffer[0] == 0x01)
                {
                    cur_state_wss = DATA;
                }
            }
        }
        else
        {
            __wsc_buffer[ret] = 0;
            // TRACE_I("<< WSS-Rx:%s\r\n", __wsc_buffer);
            if (__message_upcall_func)
            {
                __message_upcall_func(__wsc_buffer, strlen(__wsc_buffer));
            }
            cur_state_wss = HEADER;
        }
    }
}

static void __rx_task(void *pvParameters)
{
    int ret, flags, len;
    const char * ssl_ca_cert = ezlopi_factory_info_v3_get_ca_certificate();
    const char * ssl_shared_key = ezlopi_factory_info_v3_get_ssl_shared_key();
    const char * ssl_private_key = ezlopi_factory_info_v3_get_ssl_private_key();

    if ((NULL == ssl_ca_cert) || (NULL == ssl_private_key) || (NULL == ssl_shared_key))
    {
        ezlopi_free(__FUNCTION__, ssl_ca_cert);
        ezlopi_free(__FUNCTION__, ssl_private_key);
        ezlopi_free(__FUNCTION__, ssl_shared_key);
        vTaskDelete(NULL);
    }

    mbedtls_ssl_init(&ssl);
    mbedtls_pk_init(&p_key);
    mbedtls_x509_crt_init(&cacert);
    mbedtls_ctr_drbg_init(&ctr_drbg);

    TRACE_D("esp_get_free_heap_size - %d", esp_get_free_heap_size());
    TRACE_I("Seeding the random number generator");

    mbedtls_ssl_config_init(&conf);
    mbedtls_entropy_init(&entropy);

    if ((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, NULL, 0)) != 0)
    {
        TRACE_E("mbedtls_ctr_drbg_seed returned %d", ret);
        abort();
    }

    TRACE_I("Loading the CA root certificate...");

    ret = mbedtls_x509_crt_parse(&cacert, (uint8_t *)ssl_shared_key, strlen(ssl_shared_key) + 1);

    if (ret < 0)
    {
        TRACE_E("mbedtls_x509_crt_parse returned -0x%x\n\n", -ret);
        abort();
    }

    ret = mbedtls_pk_parse_key(&p_key, (uint8_t *)ssl_private_key, strlen(ssl_private_key) + 1, NULL, 0);

    TRACE_I("Setting hostname for TLS session...");

    /* Hostname set here should match CN in server certificate */

    if ((ret = mbedtls_ssl_set_hostname(&ssl, __wsc_url)) != 0)
    {
        TRACE_E("mbedtls_ssl_set_hostname returned -0x%x", -ret);
        abort();
    }

    TRACE_I("Setting up the SSL/TLS structure...");

    if ((ret = mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
    {
        TRACE_E("mbedtls_ssl_config_defaults returned %d", ret);
        goto exit;
    }

    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_NONE);
    mbedtls_ssl_conf_ca_chain(&conf, &cacert, NULL);
    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
    mbedtls_ssl_conf_own_cert(&conf, &cacert, &p_key);

    if ((ret = mbedtls_ssl_setup(&ssl, &conf)) != 0)
    {
        TRACE_E("mbedtls_ssl_setup returned -0x%x\n\n", -ret);
        goto exit;
    }

    while (!__is_wsc)
    {
        mbedtls_net_init(&server_fd);

        TRACE_I("Connecting to  '%s:%s'...", __wsc_url, __wsc_port);

        if ((ret = mbedtls_net_connect(&server_fd, __wsc_url, __wsc_port, MBEDTLS_NET_PROTO_TCP)) != 0)
        {
            TRACE_E("mbedtls_net_connect returned -%x", -ret);
            goto exit;
        }

        TRACE_I("Connected.");

        mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

        TRACE_I("Performing the SSL/TLS handshake...");

        while ((ret = mbedtls_ssl_handshake(&ssl)) != 0)
        {
            if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
            {
                TRACE_E("mbedtls_ssl_handshake returned -0x%x", -ret);
                goto exit;
            }
        }

        TRACE_I("Verifying peer X.509 certificate...");

        if ((flags = mbedtls_ssl_get_verify_result(&ssl)) != 0)
        {
            /* In real life, we probably want to close connection if ret != 0 */
            TRACE_W("Failed to verify peer certificate!");
            bzero(__wsc_buffer, 512);
            mbedtls_x509_crt_verify_info(__wsc_buffer, sizeof(__wsc_buffer), "  ! ", flags);
            TRACE_W("verification info: %s", __wsc_buffer);
        }
        else
        {
            TRACE_I("Certificate verified.");
        }

        TRACE_I("Cipher suite is %s", mbedtls_ssl_get_ciphersuite(&ssl));
        TRACE_I("Writing HTTP __request...");

        __send_internal(__request, strlen(__request));

        TRACE_I("Reading HTTP response...");
        len = sizeof(__wsc_buffer);

        while (!__is_wsc)
        {
            bzero(__wsc_buffer, len);
            ret = mbedtls_ssl_read(&ssl, (unsigned char *)__wsc_buffer, len);

            if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE)
                continue;

            if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)
            {
                ret = 0;
                __is_wsc = false;
                if (__connection_upcall_func)
                    __connection_upcall_func(false);

                break;
            }

            if (ret < 0)
            {
                __is_wsc = false;
                if (__connection_upcall_func)
                    __connection_upcall_func(false);

                TRACE_E("mbedtls_ssl_read returned -0x%x", -ret);
                break;
            }

            if (ret == 0)
            {
                __is_wsc = false;
                if (__connection_upcall_func)
                    __connection_upcall_func(false);

                TRACE_I("connection closed");
                break;
            }

            len = ret;
            TRACE_I("%d bytes read", len);
            __wsc_buffer[len] = 0;
            TRACE_I("%s", __wsc_buffer);

            if (strstr(__wsc_buffer, "websocket"))
            {
                TRACE_I("WSS OK");
                __is_wsc = true;
                if (__connection_upcall_func)
                    __connection_upcall_func(true);
                break;
            }
            else
            {
                break;
            }
        }

        while (__is_wsc)
        {
            vTaskDelay(100 / portTICK_PERIOD_MS);
            __rx_func();
        }

        mbedtls_ssl_close_notify(&ssl);

    exit:
        mbedtls_ssl_session_reset(&ssl);
        mbedtls_net_free(&server_fd);

        if (ret != 0)
        {
            mbedtls_strerror(ret, __wsc_buffer, 100);
            TRACE_E("Last error was: -0x%x - %s", -ret, __wsc_buffer);
        }

        static int request_count;
        TRACE_I("Completed %d requests", ++request_count);

        for (int countdown = 10; countdown >= 0; countdown--)
        {
            TRACE_I("%d...", countdown);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        TRACE_I("Starting again!");
    }
}

static int __send_internal(char *buf_s, size_t len)
{
    size_t written_bytes = 0;
    unsigned char *buf = (unsigned char *)buf_s;
    int ret;
    if (__is_wsc)
    {
        unsigned char *buf1 = (unsigned char *)malloc(len + 8);
        len = __zap_aut(buf, buf1);
        buf = buf1;
    }

    do
    {
        ret = mbedtls_ssl_write(&ssl, (const unsigned char *)buf + written_bytes, len - written_bytes);
        if (ret >= 0)
        {
            written_bytes += ret;
        }
        else if (ret != MBEDTLS_ERR_SSL_WANT_WRITE && ret != MBEDTLS_ERR_SSL_WANT_READ)
        {
            TRACE_E("mbedtls_ssl_write returned -0x%x", -ret);
            return written_bytes;
        }
    } while (written_bytes < len);

    if ((char *)buf != buf_s)
    {
        free(buf);
    }

    return written_bytes;
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
