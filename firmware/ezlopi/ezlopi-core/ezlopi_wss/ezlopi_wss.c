//
// Created by samogon on 11.02.20.
//

#include "ezlopi_wss.h"
#include "time.h"
#include "lwip/apps/sntp.h"

#include "frozen.h"
#include "trace.h"
#include "ezlopi_factory_info.h"

static bool is_wss = false;
static char wss_buffer[4096];
static char wss_url[64];
static char wss_port[8];
enum state_wss cur_state_wss = HEADER;

static mbedtls_net_context server_fd;
static mbedtls_ssl_context ssl;
static mbedtls_entropy_context entropy;
static mbedtls_ctr_drbg_context ctr_drbg;
static mbedtls_x509_crt cacert;
static mbedtls_ssl_config conf;
static mbedtls_pk_context p_key;

static char request[512];
static const char *request_format = "GET / HTTP/1.1\n"
                                    "Host: %s\n"
                                    "Connection: Upgrade\n"
                                    "Upgrade: websocket\n"
                                    "Origin: %s\n"
                                    "Sec-WebSocket-Version: 13\n"
                                    "User-Agent: EzloPi-%llu\n"
                                    "Sec-WebSocket-Key: JSUlJXYeLiJ2Hi4idh4uIg==\r\n"
                                    "\r\n";

static wss_upcall_t wss_upcall_function = NULL;

static void wss_recv_func(void);
static void wss_receive_task(void *pvParameters);
static int zap_aut(unsigned char *ttx, unsigned char *text);
static int wss_send_internal(char *buf_s, size_t len);

int wss_client_send(char *buf, size_t len)
{
    if (is_wss && buf && len)
    {
        return wss_send_internal(buf, len);
    }

    return 0;
}

bool wss_client_is_connected(void)
{
    return is_wss;
}

void ezlopi_client_init(char *a_uri, wss_upcall_t wss_rx_upcall_function)
{
    if (a_uri)
    {
        wss_upcall_function = wss_rx_upcall_function;

        snprintf(wss_buffer, sizeof(wss_buffer), "%s", a_uri);
        char *port_start = strrchr(wss_buffer, ':');
        uint32_t tmp_var = 0;

        if (port_start)
        {
            snprintf(wss_port, sizeof(wss_port), "%d", atoi(port_start + 1));
            tmp_var = port_start - wss_buffer;
        }

        snprintf(wss_url, sizeof(wss_url), "%.*s", tmp_var - 6, a_uri + 6);
        snprintf(request, sizeof(request), request_format, wss_url, wss_url, ezlopi_factory_info_get_info()->id);
        TRACE_D("request_format: %s", request);

        xTaskCreate(wss_receive_task, "wss_receive_task", 6 * 1024, NULL, 5, NULL);
    }
}

static void wss_recv_func(void)
{
    if (is_wss)
    {
        int ret;
        struct timeval time_val;
        fd_set read_fds;
        int fd = ((mbedtls_net_context *)&server_fd)->fd;

        if (fd < 0)
        {
            is_wss = false;
            return;
        }

        FD_ZERO(&read_fds);
        FD_SET(fd, &read_fds);

        time_val.tv_sec = 0;
        time_val.tv_usec = 1;

        ret = select(fd + 1, &read_fds, NULL, NULL, &time_val);

        /* Zero fds ready means we timed out */
        if (ret == 0)
        {
            return;
        }

        if (ret < 0)
        {
            is_wss = false;
            return;
        }

        // mbedtls_ssl_conf_read_timeout();
        // ssl.conf->read_timeout
        int retry = 2;
        do
        {
            ret = mbedtls_ssl_read(&ssl, (unsigned char *)&wss_buffer[0], sizeof(wss_buffer));
            // TRACE_I("mbedtls_ssl_read read %d byte", ret);
            // dump(wss_buffer, 0, ret);

            if (ret <= 0 && retry)
            {
                is_wss = false;
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
            if (wss_buffer[0] == 0x88)
            {
                TRACE_I("connection closed");
                is_wss = false;
                return;
            }
            if (wss_buffer[0] == 0x89) // if PING then PONG
            {
                TRACE_I("PING PONG");
                wss_buffer[0] = 0x8a;
                mbedtls_ssl_write(&ssl, (uint8_t *)wss_buffer, ret);
            }
            else
            {
                if (wss_buffer[0] == 0x81 || wss_buffer[0] == 0x01)
                {
                    cur_state_wss = DATA;
                }
            }
        }
        else
        {
            wss_buffer[ret] = 0;
            // TRACE_I("<< WSS-Rx:%s\r\n", wss_buffer);
            if (wss_upcall_function)
            {
                wss_upcall_function(wss_buffer, strlen(wss_buffer));
            }
            cur_state_wss = HEADER;
        }
    }
}

static void wss_receive_task(void *pvParameters)
{
    int ret, flags, len;
    s_ezlopi_factory_info_t *factory = ezlopi_factory_info_get_info();

    if (NULL == factory)
    {
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

    ret = mbedtls_x509_crt_parse(&cacert, (uint8_t *)factory->ssl_shared_key, strlen(factory->ssl_shared_key) + 1);

    if (ret < 0)
    {
        TRACE_E("mbedtls_x509_crt_parse returned -0x%x\n\n", -ret);
        abort();
    }

    ret = mbedtls_pk_parse_key(&p_key, (uint8_t *)factory->ssl_private_key, strlen(factory->ssl_private_key) + 1, NULL, 0);

    TRACE_I("Setting hostname for TLS session...");

    /* Hostname set here should match CN in server certificate */

    if ((ret = mbedtls_ssl_set_hostname(&ssl, wss_url)) != 0)
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

    while (!is_wss)
    {
        mbedtls_net_init(&server_fd);

        TRACE_I("Connecting to  '%s:%s'...", wss_url, wss_port);

        if ((ret = mbedtls_net_connect(&server_fd, wss_url, wss_port, MBEDTLS_NET_PROTO_TCP)) != 0)
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
            bzero(wss_buffer, 512);
            mbedtls_x509_crt_verify_info(wss_buffer, sizeof(wss_buffer), "  ! ", flags);
            TRACE_W("verification info: %s", wss_buffer);
        }
        else
        {
            TRACE_I("Certificate verified.");
        }

        TRACE_I("Cipher suite is %s", mbedtls_ssl_get_ciphersuite(&ssl));
        TRACE_I("Writing HTTP request...");

        wss_send_internal(request, strlen(request));

        TRACE_I("Reading HTTP response...");
        len = sizeof(wss_buffer);

        while (!is_wss)
        {
            bzero(wss_buffer, len);
            ret = mbedtls_ssl_read(&ssl, (unsigned char *)wss_buffer, len);

            if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE)
                continue;

            if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)
            {
                ret = 0;
                is_wss = false;
                break;
            }

            if (ret < 0)
            {
                is_wss = false;
                TRACE_E("mbedtls_ssl_read returned -0x%x", -ret);
                break;
            }

            if (ret == 0)
            {
                is_wss = false;
                TRACE_I("connection closed");
                break;
            }

            len = ret;
            TRACE_I("%d bytes read", len);
            wss_buffer[len] = 0;
            TRACE_I("%s", wss_buffer);

            if (strstr(wss_buffer, "websocket"))
            {
                TRACE_I("WSS OK");
                is_wss = true;
                break;
            }
            else
            {
                break;
            }
        }

        while (is_wss)
        {
            vTaskDelay(100 / portTICK_PERIOD_MS);
            wss_recv_func();
        }

        mbedtls_ssl_close_notify(&ssl);

    exit:
        mbedtls_ssl_session_reset(&ssl);
        mbedtls_net_free(&server_fd);

        if (ret != 0)
        {
            mbedtls_strerror(ret, wss_buffer, 100);
            TRACE_E("Last error was: -0x%x - %s", -ret, wss_buffer);
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

static int wss_send_internal(char *buf_s, size_t len)
{
    size_t written_bytes = 0;
    unsigned char *buf = (unsigned char *)buf_s;
    int ret;
    if (is_wss)
    {
        unsigned char *buf1 = (unsigned char *)malloc(len + 8);
        len = zap_aut(buf, buf1);
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

static int zap_aut(unsigned char *ttx, unsigned char *text)
{
    int i;
    int len1 = 0;
    const unsigned char mask[4] = {0x10, 0x55, 0x21, 0x43};

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
