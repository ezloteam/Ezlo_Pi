
#include <time.h>
#include "esp_tls.h"
// #ifdef CONFIG_ESP_TLS_USING_MBEDTLS
#include "mbedtls/platform.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/esp_debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"
// #endif

#include "ezlopi_util_trace.h"

#include "ezlopi_core_http.h"
#include "ezlopi_core_event_group.h"
#include "EZLOPI_USER_CONFIG.h"


static void ezlopi_http_free_rx_data(s_rx_data_t* rx_data);
static esp_err_t ezlopi_http_event_handler(esp_http_client_event_t* evt);

#define TAG __FILE__

#define FREE_IF_NOT_NULL(ptr) \
    {                         \
        if (ptr)              \
        {                     \
            ezlopi_free(__FUNCTION__, ptr);        \
            ptr = NULL;       \
        }                     \
    }


#define GET_STRING_SIZE(str) ((NULL != str) ? (strlen(str)) : 0)
#if 0
// typedef struct ll_resp_buf
// {
//     uint32_t len;
//     uint8_t *buffer;
//     struct ll_resp_buf *next;
// } ll_resp_buf_t; // implementation for response
#endif
//--------------- Memory Malloc functions --------------------------
int ezlopi_core_http_calc_empty_bufsize(char* dest_buff, int dest_size, int reqd_size)
{
    int limit = dest_size - GET_STRING_SIZE(dest_buff);
    limit = (limit > 0) ? limit : 0;
    if (limit > reqd_size)
    {
        return limit;
    }
    return 0;
}

int ezlopi_core_http_mem_malloc(char** __dest_ptr, const char* src_ptr)
{
    int ret = GET_STRING_SIZE(src_ptr); // calculate the size of content to be stored
    if (NULL != src_ptr)
    {
        FREE_IF_NOT_NULL(*__dest_ptr);
        // Now, do a fresh allocation
        ret += 5; // (must be: n>2)
        char* tmp_ptr = ezlopi_malloc(__FUNCTION__, ret);
        if (tmp_ptr)
        {
            bzero(tmp_ptr, (ret));
            snprintf(tmp_ptr, ret, "%s", src_ptr);
            // TRACE_D("1. *Malloc_New_buffer : (%p)->(%p) : [%d]", *__dest_ptr, tmp_ptr, ret);
            *__dest_ptr = tmp_ptr; // old gets replaced by new address
            // TRACE_D("2.__dest_ptr(%p) : size=> [%d]", *__dest_ptr, GET_STRING_SIZE(*__dest_ptr));
        }
    }
    else
    {
        TRACE_E(" Error !!, given 'src_ptr' is NULL or Zero .... Allocation Aborted...");
    }
    return ret;
}

int ezlopi_core_http_dyna_relloc(char** Buf, int reqSize)
{
    int ret = 0;
    if ((NULL != *Buf) && (reqSize > 0)) // strictly:  (new-size != 0)
    {
        void* NewBuf = ezlopi_realloc(__FUNCTION__, *Buf, reqSize); // reqSize ≤ 1.6 * n
        if (NULL == NewBuf)
        {
            TRACE_E("Reallocation - failed");
        }
        else
        {
            // TRACE_D("Relocating [%p] to [%p]: NewBuf[%d]", *Buf, NewBuf, reqSize);
            *Buf = NewBuf;
            ret = 1; // return success
        }
    }
    return ret;
}

//---------------------------  Static Mbedtls Functions -----------------------------------------------------
/**
 * @brief Function Trigger http_requests via mbedTLS.
 *
 * @return Address of a memory_block ; (char*)ezlopi_malloc(...)
 */
static void ezlopi_core_http_request_via_mbedTLS(const char* web_server, int web_port_num, const char* url_req, char** resp_buf)
{
    TRACE_I("&result==[%p] --> *resp_buf=>[%p]", resp_buf, *resp_buf);
    int ret, flags, len;
    uint32_t tmp_buf_size = 256;
    char tmp_buf[tmp_buf_size];

    char web_port[10] = { 0 };
    snprintf(web_port, 10, "%d", web_port_num);
    web_port[9] = '\0';

    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl;
    mbedtls_x509_crt cacert;
    mbedtls_ssl_config conf;
    // mbedtls_entropy_context* entropy = ezlopi_malloc(sizeof(mbedtls_entropy_context));
    // mbedtls_ctr_drbg_context* ctr_drbg = ezlopi_malloc(sizeof(mbedtls_ctr_drbg_context));
    // mbedtls_ssl_context* ssl = ezlopi_malloc(sizeof(mbedtls_ssl_context));
    // mbedtls_x509_crt* cacert = ezlopi_malloc(sizeof(mbedtls_x509_crt));
    // mbedtls_ssl_config* conf = ezlopi_malloc(sizeof(mbedtls_ssl_config));
    // if (entropy && ctr_drbg && ssl && cacert && conf)
    // {
    mbedtls_net_context server_fd;
    mbedtls_ssl_init(&ssl);
    mbedtls_x509_crt_init(&cacert);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    // TRACE_I("Seeding the random number generator");
    mbedtls_ssl_config_init(&conf);
    mbedtls_entropy_init(&entropy);
    if (0 != (ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
        NULL, 0)))
    {
        TRACE_E("mbedtls_ctr_drbg_seed returned %d", ret);
        goto exit;
    }
    // TRACE_I("Attaching the certificate bundle...");

    ret = esp_crt_bundle_attach(&conf);
    if (ret < 0)
    {
        TRACE_E("esp_crt_bundle_attach returned -0x%x\n\n", -ret);
        goto exit;
    }
    // TRACE_I("Setting hostname for TLS session...");

    /* Hostname set here should match CN in server certificate */
    if (0 != (ret = mbedtls_ssl_set_hostname(&ssl, web_server)))
    {
        TRACE_E("mbedtls_ssl_set_hostname returned -0x%x", -ret);
        goto exit;
    }

    // TRACE_I("Setting up the SSL/TLS structure...");
    if (0 != (ret = mbedtls_ssl_config_defaults(&conf,
        MBEDTLS_SSL_IS_CLIENT,
        MBEDTLS_SSL_TRANSPORT_STREAM,
        MBEDTLS_SSL_PRESET_DEFAULT)))
    {
        TRACE_E("mbedtls_ssl_config_defaults returned %d", ret);
        goto exit;
    }

    /* MBEDTLS_SSL_VERIFY_OPTIONAL is bad for security, in this example it will print
       a warning if CA verification fails but it will continue to connect.
       You should consider using MBEDTLS_SSL_VERIFY_REQUIRED in your own code.
    */
    // mbedtls_ssl_conf_authmode(conf, MBEDTLS_SSL_VERIFY_REQUIRED);
    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
    mbedtls_ssl_conf_ca_chain(&conf, &cacert, NULL);
    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
#ifdef CONFIG_MBEDTLS_DEBUG
    mbedtls_esp_enable_debug_log(conf, CONFIG_MBEDTLS_DEBUG_LEVEL);
#endif

    if (0 != (ret = mbedtls_ssl_setup(&ssl, &conf)))
    {
        TRACE_E("mbedtls_ssl_setup returned -0x%x\n\n", -ret);
        goto exit;
    }

    mbedtls_net_init(&server_fd);
    // TRACE_I("Connecting to %s:%s...", web_server, web_port);

    if (0 != (ret = mbedtls_net_connect(&server_fd, web_server,
        web_port, MBEDTLS_NET_PROTO_TCP)))
    {
        TRACE_E("mbedtls_net_connect returned -%x", -ret);
        goto exit;
    }
    // TRACE_I("Connected.");
    mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

    // TRACE_I("Performing the SSL/TLS handshake...");
    time_t start_tm = 0, now = 0; // now keeping track of time
    time(&start_tm);
    time(&now);
    while (0 != (ret = mbedtls_ssl_handshake(&ssl)))
    {
        TRACE_W("ret => %x", -ret); // mbedtls_ssl_conf_async_private_cb()
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
        {
            TRACE_E("mbedtls_ssl_handshake returned -0x%x", -ret);
            goto exit;
        }
        time(&now);
        if ((now - start_tm) > (time_t)5) // 5sec
        {
            goto exit;
        }
    }

    // TRACE_I("Verifying peer X.509 certificate...");
    if (0 != (flags = mbedtls_ssl_get_verify_result(&ssl)))
    {
        /* In real life, we probably want to close connection if ret != 0 */
        TRACE_E("Failed to verify peer certificate!");
        bzero(tmp_buf, tmp_buf_size);
        mbedtls_x509_crt_verify_info(tmp_buf, tmp_buf_size, "  ! ", flags);
        TRACE_E("verification Error_info: %s", tmp_buf);
        goto exit;
    }
    else
    {
        // TRACE_I("Certificate verified.");
    }
    // TRACE_I("Cipher suite is %s", mbedtls_ssl_get_ciphersuite(ssl));
    // TRACE_I("Writing HTTP request...");
    size_t written_bytes = 0;
    do
    {
        ret = mbedtls_ssl_write(&ssl, (const unsigned char*)url_req + written_bytes,
            strlen(url_req) - written_bytes);
        if (ret >= 0)
        {
            // TRACE_I("%d bytes written", ret);
            written_bytes += ret;
        }
        else if (ret != MBEDTLS_ERR_SSL_WANT_WRITE && ret != MBEDTLS_ERR_SSL_WANT_READ)
        {
            TRACE_E("mbedtls_ssl_write returned -0x%x", -ret);
            goto exit;
        }
    } while (written_bytes < strlen(url_req));

    // TRACE_I("Reading HTTP response...");
    uint32_t resp_buf_size = tmp_buf_size + 1;
    char* resp_buf_dummy = (char*)ezlopi_malloc(__FUNCTION__, resp_buf_size); // points to a memory-block
    if (resp_buf_dummy)
    {
        bzero(resp_buf_dummy, resp_buf_size); // clear the buffer
        uint8_t reply_count = 0;
        do
        {
            len = tmp_buf_size - 1;
            bzero(tmp_buf, tmp_buf_size);
            ret = mbedtls_ssl_read(&ssl, (unsigned char*)tmp_buf, len);
            if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE)
            {
                TRACE_I("MBEDTLS_ERR_SSL_WANT_READ/WRITE");
                continue;
            }
            if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)
            {
                TRACE_S("MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY");
                ret = 0;
                break;
            }
            if (ret < 0)
            {
                TRACE_E("mbedtls_ssl_read returned -0x%x", -ret);
                break;
            }
            if (ret == 0)
            {
                // TRACE_I("connection closed");
                break;
            }
            len = ret;
            // TRACE_D("ret[%d]: %s", len, tmp_buf);
            if (ret > 0)
            {
                reply_count++;
                if (reply_count > 1)
                {
                    resp_buf_size += (len + 5); // (+2)makes sure ; additional space for '\0'
                    if (ezlopi_core_http_dyna_relloc(&resp_buf_dummy, resp_buf_size))
                    {
                        snprintf(resp_buf_dummy + strlen(resp_buf_dummy), len, "%s", tmp_buf);
                    }
                    else
                    {
                        resp_buf_size -= (len + 5);
                    }
                }
                else
                {
                    snprintf(resp_buf_dummy, len, "%s", tmp_buf); // 513
                }
            }
        } while (1);

        if (strlen(resp_buf_dummy) > 0)
        {
            *resp_buf = resp_buf_dummy;
            TRACE_I("&result==[%p] --> *resp_buf=>[%p]  ", resp_buf, *resp_buf);
        }
        else
        {
            *resp_buf = NULL;
            ezlopi_free(__FUNCTION__, resp_buf_dummy);
        }
    }

    mbedtls_ssl_close_notify(&ssl);
exit:
    mbedtls_ssl_session_reset(&ssl);
    mbedtls_net_free(&server_fd);
    if (0 != ret)
    {
        mbedtls_strerror(ret, tmp_buf, 100);
        TRACE_E("Last error was: -0x%x - %s", -ret, tmp_buf);
    }
    TRACE_D("Minimum free heap size: %d bytes", esp_get_minimum_free_heap_size());

    // Clearing used structures
    mbedtls_ssl_free(&ssl);
    mbedtls_ssl_config_free(&conf);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);

    // ezlopi_free(__FUNCTION__, ssl);
    // ezlopi_free(__FUNCTION__, conf);
    // ezlopi_free(__FUNCTION__, ctr_drbg);
    // ezlopi_free(__FUNCTION__, entropy);
    // ezlopi_free(__FUNCTION__, cacert);
// }
    TRACE_I("Completed a request");
}

/**
 * @brief Function to extract the parameter values from 'config' struct and append it to '*request'
 *
 * @param config  : custom struct consisting [headers + content + server_name + web_port ...]
 * @param request : *ptr that holds the complete request_url
 * @param request_len : total char-capacity the '*request' can contain.
 */
static void ezlopi_core_http_generate_request(s_ezlopi_core_http_mbedtls_t* config, char** request, int request_len)
{
    if ((NULL != config) && (NULL != *request) && (request_len > 0))
    {
        // 1. Identify Http-Method
        switch (config->method)
        {
        case HTTP_METHOD_GET:
        {
            // TRACE_S("HTTP GET-METHOD [%d]", config->method);
            if (((NULL != config->username) && (GET_STRING_SIZE(config->username) > 0)) &&
                ((NULL != config->password) && (GET_STRING_SIZE(config->password) > 0)))
            {
                snprintf(*request, request_len, "GET %s?username=%s&password=%s HTTP/1.0\r\nUser-Agent: esp-idf/1.0 esp32\r\n", config->url, config->username, config->password);
            }
            else
            {
                snprintf(*request, request_len, "GET %s HTTP/1.0\r\nUser-Agent: esp-idf/1.0 esp32\r\n", config->url);
            }
            break;
        }
        case HTTP_METHOD_POST:
        {
            // TRACE_S("HTTP POST-METHOD [%d]", config->method);
            snprintf(*request, request_len, "POST %s HTTP/1.0\r\nUser-Agent: esp-idf/1.0 esp32\r\n", config->url);
            break;
        }
        case HTTP_METHOD_PUT:
        {
            // TRACE_S("HTTP PUT-METHOD [%d]", config->method);
            snprintf(*request, request_len, "PUT %s HTTP/1.0\r\nUser-Agent: esp-idf/1.0 esp32\r\n", config->url);
            break;
        }
        case HTTP_METHOD_DELETE:
        {
            // TRACE_S("HTTP DELETE-METHOD [%d]", config->method);
            snprintf(*request, request_len, "DELETE %s HTTP/1.0\r\nUser-Agent: esp-idf/1.0 esp32\r\n", config->url);
            break;
        }
        default:
        {
            TRACE_E("METHOD NOT FOUND.. {%d}", config->method);
            break;
        }
        }

        // 2. adding 'Headers' to request_buffer
        int max_allowed = 0;
        if ((NULL != config->header) && (GET_STRING_SIZE(config->header) > 0))
        {
            max_allowed = ezlopi_core_http_calc_empty_bufsize(*request, request_len, (strlen(config->header) + 3));
            if (max_allowed > 0)
            {
                snprintf(*request + strlen(*request), max_allowed, "%s\r\n", config->header);
            }
        }
        // 3. adding content body to *request
        if ((NULL != config->content) && (GET_STRING_SIZE(config->content) > 0))
        {
            if ((HTTP_METHOD_GET != config->method) &&
                ((NULL != config->username) && (NULL != config->password)))
            {
                max_allowed = ezlopi_core_http_calc_empty_bufsize(*request, request_len, (strlen(config->username) + strlen(config->password) + strlen(config->content) + 3));
            }
            else
            {
                max_allowed = ezlopi_core_http_calc_empty_bufsize(*request, request_len, (strlen(config->content) + 3));
            }

            if (max_allowed > 0)
            {
                snprintf(*request + strlen(*request), max_allowed, "%s\r\n", config->content);
            }
        }
    }
    else
    {
        TRACE_E("invalid arguments to generate Request_URL");
    }
}

void ezlopi_core_http_mbedtls_req(s_ezlopi_core_http_mbedtls_t* config)
{
    if (config)
    {
        int request_len = (config->url_maxlen) + (config->header_maxlen) + (config->content_maxlen) + 100; // [>25] ; for "skipSecurity: true"
        char* request = ezlopi_malloc(__FUNCTION__, sizeof(char) * request_len);
        if (request)
        {
            bzero(request, request_len);
            request[request_len - 1] = '\0';

            ezlopi_core_http_generate_request(config, &request, request_len);

            // Ready-Up 'request' buffer
            TRACE_I("request[capacity: %d]:\n\n%s[%d]", request_len, request, strlen(request));
            // TRACE_D("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());
            // TRACE_E("&result==[%p]", &(config->response));
            ezlopi_core_http_request_via_mbedTLS(config->web_server, (config->web_port), request, &(config->response));
            if (config->response)
            {
                TRACE_D("*result[%p] =>\n[%d]\n%s", config->response, strlen(config->response), config->response);
                ezlopi_free(__FUNCTION__, config->response); // return to destination buffer
                config->response = NULL;
            }
            ezlopi_free(__FUNCTION__, request);
        }
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------


s_ezlopi_http_data_t* ezlopi_http_get_request(const char* cloud_url, const char* private_key, const char* shared_key, const char* ca_certificate)
{
    char* ret = NULL;
    int status_code = 0;
    s_rx_data_t* my_data = (s_rx_data_t*)ezlopi_malloc(__FUNCTION__, sizeof(s_rx_data_t));
    s_ezlopi_http_data_t* http_get_data = NULL;
    if ((NULL != my_data))
    {
        memset(my_data, 0, sizeof(s_rx_data_t));

        esp_http_client_config_t config = {
            .url = cloud_url,
            .cert_pem = ca_certificate,
            .client_cert_pem = shared_key,
            .client_key_pem = private_key,
            .event_handler = ezlopi_http_event_handler,
            .transport_type = HTTP_TRANSPORT_OVER_SSL,
            .user_data = (void*)(my_data), // my_data will be filled in 'ezlopi_http_event_handler'
        };

        esp_http_client_handle_t client = esp_http_client_init(&config);
        if (NULL != client)
        {
            esp_err_t err = esp_http_client_perform(client);
            status_code = esp_http_client_get_status_code(client);
            if (err == ESP_OK)
            {
                while (!esp_http_client_is_complete_data_received(client))
                {
                    vTaskDelay(50 / portTICK_RATE_MS);
                }

                if (my_data->total_len)
                {
                    ret = (char*)ezlopi_malloc(__FUNCTION__, my_data->total_len + 1);
                    // TRACE_E("Response lent: %d", my_data->total_len);
                    if (ret)
                    {
                        s_rx_data_t* cur_d = my_data;
                        memset(ret, 0, my_data->total_len + 1);

                        while (cur_d)
                        {
                            strcat(ret, cur_d->ptr);
                            TRACE_D("%.*s", cur_d->len, cur_d->ptr);
                            cur_d = cur_d->next;
                        }

                        http_get_data = (s_ezlopi_http_data_t*)ezlopi_malloc(__FUNCTION__, sizeof(s_ezlopi_http_data_t));
                        if (http_get_data)
                        {
                            memset(http_get_data, 0, sizeof(s_ezlopi_http_data_t));
                            http_get_data->response = ret;
                            http_get_data->status_code = status_code;
                        }
                    }
                }
            }
            else
            {
                TRACE_E("Error perform http request %s", esp_err_to_name(err));
            }

            ezlopi_http_free_rx_data(my_data);
            esp_http_client_cleanup(client);
        }
    }
    return http_get_data;
}

s_ezlopi_http_data_t* ezlopi_http_post_request(const char* cloud_url, const char* location, cJSON* headers, const char* private_key, const char* shared_key, const char* ca_certificate)
{
    char* ret = NULL;
    s_rx_data_t* my_data = (s_rx_data_t*)ezlopi_malloc(__FUNCTION__, sizeof(s_rx_data_t));
    s_ezlopi_http_data_t* http_get_data = ezlopi_malloc(__FUNCTION__, sizeof(s_ezlopi_http_data_t));
    memset(http_get_data, 0, sizeof(s_ezlopi_http_data_t));

    if (my_data)
    {
        memset(my_data, 0, sizeof(s_rx_data_t));

        char* uri = ezlopi_malloc(__FUNCTION__, 256);
        if (uri)
        {
            snprintf(uri, 256, "%s/%s", cloud_url, location);
            TRACE_D("URL: %s", uri);
        }
        else
        {
            uri = (char*)cloud_url;
        }

        esp_http_client_config_t config = {
            .url = uri,
            .cert_pem = ca_certificate,
            .client_cert_pem = shared_key,
            .client_key_pem = private_key,
            .event_handler = ezlopi_http_event_handler,
            .transport_type = HTTP_TRANSPORT_OVER_SSL,
            .user_data = (void*)(my_data), // my_data will be filled in 'ezlopi_http_event_handler'
        };

        esp_http_client_handle_t client = esp_http_client_init(&config);

        if (NULL != client)
        {
            esp_http_client_set_method(client, HTTP_METHOD_POST);
            cJSON* header = headers->child;
            while (header)
            {
                // TRACE_I("%s: %s", header->string, header->valuestring);
                esp_http_client_set_header(client, header->string, header->valuestring);
                header = header->next;
            }

            esp_err_t err = esp_http_client_perform(client);

            if (err == ESP_OK)
            {
                http_get_data->status_code = esp_http_client_get_status_code(client);;
                while (!esp_http_client_is_complete_data_received(client))
                {
                    vTaskDelay(10 / portTICK_RATE_MS);
                }

                if (my_data->total_len)
                {
                    ret = (char*)ezlopi_malloc(__FUNCTION__, my_data->total_len + 1);

                    if (ret)
                    {
                        s_rx_data_t* cur_d = my_data;
                        memset(ret, 0, my_data->total_len + 1);

                        while (cur_d)
                        {
                            strcat(ret, cur_d->ptr);
                            TRACE_D("%.*s", cur_d->len, cur_d->ptr);
                            cur_d = cur_d->next;
                        }
                    }

                    http_get_data->response = ret;
                }
            }
            else
            {
                TRACE_E("Error perform http request %s", esp_err_to_name(err));
                ezlopi_free(__FUNCTION__, http_get_data);
                http_get_data = NULL;
            }

            ezlopi_http_free_rx_data(my_data);
            esp_http_client_cleanup(client);
        }
        if (uri)
        {
            ezlopi_free(__FUNCTION__, uri);
        }
    }

    return http_get_data;
}

static esp_err_t ezlopi_http_event_handler(esp_http_client_event_t* evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ERROR:
    {
        TRACE_E("HTTP_EVENT_ERROR");
        break;
    }
    case HTTP_EVENT_ON_CONNECTED:
    {
        TRACE_D("HTTP_EVENT_ON_CONNECTED");
        break;
    }
    case HTTP_EVENT_HEADER_SENT:
    {
        TRACE_D("HTTP_EVENT_HEADER_SENT");
        break;
    }
    case HTTP_EVENT_ON_HEADER:
    {
        TRACE_D("HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;
    }
    case HTTP_EVENT_ON_DATA:
    {
        TRACE_D("HTTP_EVENT_ON_DATA, len=%d, data: %.*s", evt->data_len, evt->data_len, (char*)evt->data);
        // if (!esp_http_client_is_chunked_response(evt->client))
        {
            if (evt->user_data)
            {
                char* tmp_data = (char*)ezlopi_malloc(__FUNCTION__, evt->data_len + 1);
                if (NULL != tmp_data)
                {
                    memcpy(tmp_data, evt->data, evt->data_len);
                    tmp_data[evt->data_len] = '\0';

                    s_rx_data_t* my_data = (s_rx_data_t*)evt->user_data;
                    if (my_data->len || my_data->total_len || my_data->ptr)
                    {
                        s_rx_data_t* cur_dh = my_data;
                        while (cur_dh->next)
                        {
                            cur_dh = cur_dh->next;
                        }

                        cur_dh->next = (s_rx_data_t*)ezlopi_malloc(__FUNCTION__, sizeof(s_rx_data_t));
                        if (cur_dh->next)
                        {
                            cur_dh->next->len = evt->data_len;
                            cur_dh->next->ptr = (char*)tmp_data;
                            cur_dh->next->total_len = 0;
                            my_data->total_len += evt->data_len;
                            cur_dh->next->next = NULL;
                        }
                    }
                    else
                    {
                        my_data->len = evt->data_len;
                        my_data->total_len = evt->data_len;
                        my_data->ptr = (char*)tmp_data;
                        my_data->next = NULL;
                    }
                }
            }
        }

        break;
    }
    case HTTP_EVENT_ON_FINISH:
    {
        TRACE_D("HTTP_EVENT_ON_FINISH");
        break;
    }
    case HTTP_EVENT_DISCONNECTED:
    {
        TRACE_D("HTTP_EVENT_DISCONNECTED");
        int mbedtls_err = 0;
        esp_err_t err = esp_tls_get_and_clear_last_error((esp_tls_error_handle_t)evt->data, &mbedtls_err, NULL);
        if (err != 0)
        {
            TRACE_D("Last esp error code: 0x%x", err);
            TRACE_D("Last mbedtls failure: 0x%x", mbedtls_err);
        }
        break;
    }
    }
    return ESP_OK;
}

static void ezlopi_http_free_rx_data(s_rx_data_t* rx_data)
{
    if (rx_data)
    {
        if (rx_data->next)
        {
            ezlopi_http_free_rx_data(rx_data->next);
        }

        if (rx_data->ptr)
        {
            ezlopi_free(__FUNCTION__, rx_data->ptr);
        }
        ezlopi_free(__FUNCTION__, rx_data);
    }
}
