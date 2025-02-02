/* ===========================================================================
** Copyright (C) 2025 Ezlo Innovation Inc
**
** Under EZLO AVAILABLE SOURCE LICENSE (EASL) AGREEMENT
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/
/**
 * @file    ezlopi_core_http.c
 * @brief   Function to perform operation on http
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 * @version 1.0
 * @date    November 2nd, 2022 5:30 PM
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <time.h>
#include "esp_tls.h"
#include "mbedtls/platform.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/esp_debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"
#include "esp_crt_bundle.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_cloud_constants.h"

#include "ezlopi_core_sntp.h"
#include "ezlopi_core_http.h"
#include "EZLOPI_USER_CONFIG.h"
#include "ezlopi_core_event_group.h"

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/
#define TAG __FILE__

#define FREE_IF_NOT_NULL(ptr)               \
    {                                       \
        if (ptr)                            \
        {                                   \
            ezlopi_free(__FUNCTION__, ptr); \
            ptr = NULL;                     \
        }                                   \
    }

#define GET_STRING_SIZE(str) ((NULL != str) ? (strlen(str)) : 0)

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
static void __ezlopi_http_req_via_mbedTLS(const char *host_web_server, int web_port_num, const char *url_req, char **resp_buf);
static void __ezlopi_http_generate_request(s_ezlopi_core_http_mbedtls_t *config, char **request, int request_len);
static void __ezlopi_http_free_rx_data(s_rx_chunk_t *rx_chunks);
static esp_err_t __ezlopi_http_event_handler(esp_http_client_event_t *evt);
/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
int EZPI_core_http_calc_empty_bufsize(char *dest_buff, int dest_size, int reqd_size)
{
    int limit = dest_size - GET_STRING_SIZE(dest_buff);
    limit = (limit > 0) ? limit : 0;
    if (limit > reqd_size)
    {
        return limit;
    }
    return 0;
}

int EZPI_core_http_mem_malloc(char **__dest_ptr, const char *src_ptr)
{
    int ret = GET_STRING_SIZE(src_ptr); // calculate the size of content to be stored
    if (NULL != src_ptr)
    {
        FREE_IF_NOT_NULL(*__dest_ptr);
        // Now, do a fresh allocation
        ret += 5; // (must be: n>2)
        char *tmp_ptr = ezlopi_malloc(__FUNCTION__, ret);
        if (tmp_ptr)
        {
            bzero(tmp_ptr, (ret));
            snprintf(tmp_ptr, ret, "%s", src_ptr);
            *__dest_ptr = tmp_ptr; // old gets replaced by new address
        }
    }
    else
    {
        TRACE_E(" Error !!, given 'src_ptr' is NULL or Zero .... Allocation Aborted...");
    }
    return ret;
}

ezlopi_error_t EZPI_core_http_dyna_relloc(char **Buf, int reqSize)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if ((NULL != *Buf) && (reqSize > 0)) // strictly:  (new-size != 0)
    {
        void *NewBuf = ezlopi_realloc(__FUNCTION__, *Buf, reqSize); // reqSize ≤ 1.6 * n
        if (NULL == NewBuf)
        {
            TRACE_E("Reallocation - failed");
        }
        else
        {
            *Buf = NewBuf;
            ret = EZPI_SUCCESS; // return success
        }
    }
    return ret;
}

void EZPI_core_http_mbedtls_req(s_ezlopi_core_http_mbedtls_t *config)
{
    if (config)
    {
        int request_len = (config->url_maxlen) + (config->header_maxlen) + (config->content_maxlen) + 100; // [>25] ; for "skipSecurity: true"
        char *request = ezlopi_malloc(__FUNCTION__, sizeof(char) * request_len);
        if (request)
        {
            bzero(request, request_len);
            request[request_len - 1] = '\0';

            __ezlopi_http_generate_request(config, &request, request_len);
            // Ready-Up 'request' buffer
            TRACE_D("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());
            TRACE_I("request[capacity: %d]:\n\n%s[%d]", request_len, request, strlen(request));
            __ezlopi_http_req_via_mbedTLS(config->web_server, (config->web_port), request, &(config->response)); // (  web_server['host'] , port_num , http_request , *response_ptr] )
            if (config->response)
            {
                TRACE_S("*result[%p] =>\n[%d]\n%s", config->response, strlen(config->response), config->response);
            }

            ezlopi_free(__FUNCTION__, request);
        }
    }
}

s_ezlopi_http_data_t *EZPI_core_http_get_request(const char *cloud_url, const char *private_key, const char *shared_key, const char *ca_certificate)
{
    int status_code = 0;
    s_rx_data_t my_data;
    s_ezlopi_http_data_t *http_response = NULL;

    memset(&my_data, 0, sizeof(s_rx_data_t));

    esp_http_client_config_t config = {
        .url = cloud_url,
        .cert_pem = ca_certificate,
        .client_cert_pem = shared_key,
        .client_key_pem = private_key,
        .event_handler = __ezlopi_http_event_handler,
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
        .user_data = (void *)(&my_data), // my_data will be filled in '__ezlopi_http_event_handler'
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (NULL != client)
    {
        esp_err_t err = esp_http_client_perform(client);
        status_code = esp_http_client_get_status_code(client);

        TRACE_D("get_status_code: %d", status_code);

        http_response = (s_ezlopi_http_data_t *)ezlopi_malloc(__FUNCTION__, sizeof(s_ezlopi_http_data_t));
        if (http_response)
        {
            memset(http_response, 0, sizeof(s_ezlopi_http_data_t));
            http_response->status_code = status_code;

            if (err == ESP_OK)
            {
                while (!esp_http_client_is_complete_data_received(client))
                {
                    if (my_data.status < 0)
                    {
                        break;
                    }

                    vTaskDelay(50 / portTICK_RATE_MS);
                }

                if (my_data.rx_len >= my_data.content_length)
                {
                    char *_data_buffer = (char *)ezlopi_malloc(__FUNCTION__, my_data.content_length + 1);
                    if (_data_buffer)
                    {
                        memset(_data_buffer, 0, my_data.content_length + 1);

                        s_rx_chunk_t *curr_chunk = my_data.rx_chunks;
                        uint32_t _copied_len = 0;

                        while (curr_chunk)
                        {
                            // _copied_len += curr_chunk->len;
                            _copied_len += snprintf(_data_buffer + _copied_len, my_data.content_length + 1 - _copied_len, "%.*s", curr_chunk->len, curr_chunk->ptr);
                            curr_chunk = curr_chunk->next;
                        }

                        http_response->response = _data_buffer;
                        http_response->response_len = strlen(_data_buffer);

                        if (_copied_len == my_data.content_length)
                        {
                            TRACE_S("data received successfully.");
                        }
                    }
                }
                else
                {
                    TRACE_E("Mismatched Content length and received length!");
                }
            }
            else
            {
                TRACE_E("Error perform http request %s", esp_err_to_name(err));
            }
        }
        else
        {
            TRACE_D("get_status_code: %d", status_code);
        }

        __ezlopi_http_free_rx_data(my_data.rx_chunks);
        esp_http_client_cleanup(client);
    }

    return http_response;
}

s_ezlopi_http_data_t *EZPI_core_http_post_request(const char *cloud_url, const char *location, cJSON *headers, const char *private_key, const char *shared_key, const char *ca_certificate)
{
    s_ezlopi_http_data_t *http_get_data = ezlopi_malloc(__FUNCTION__, sizeof(s_ezlopi_http_data_t));

    if (cloud_url && http_get_data)
    {
        s_rx_data_t my_data;
        memset(&my_data, 0, sizeof(s_rx_data_t));

        char uri[256];
        snprintf(uri, sizeof(uri), "%s/%s", cloud_url, location ? location : ezlopi__str);
        TRACE_D("URL: %s", uri);

        esp_http_client_config_t config = {
            .url = uri,
            .cert_pem = ca_certificate,
            .client_cert_pem = shared_key,
            .client_key_pem = private_key,
            .event_handler = __ezlopi_http_event_handler,
            .transport_type = HTTP_TRANSPORT_OVER_SSL,
            .user_data = (void *)(&my_data), // my_data will be filled in '__ezlopi_http_event_handler'
        };

        esp_http_client_handle_t client = esp_http_client_init(&config);

        if (NULL != client)
        {
            esp_http_client_set_method(client, HTTP_METHOD_POST);
            cJSON *header = headers->child;
            while (header)
            {
                // TRACE_I("%s: %s", header->string, header->valuestring);
                esp_http_client_set_header(client, header->string, header->valuestring);
                header = header->next;
            }

            esp_err_t err = esp_http_client_perform(client);

            if (err == ESP_OK)
            {
                http_get_data->status_code = esp_http_client_get_status_code(client);

                while (!esp_http_client_is_complete_data_received(client))
                {
                    if (my_data.status < 0)
                    {
                        break;
                    }

                    vTaskDelay(10 / portTICK_RATE_MS);
                }

                if (my_data.content_length == my_data.rx_len)
                {
                    char *_tmp_buffer = (char *)ezlopi_malloc(__FUNCTION__, my_data.content_length + 1);

                    if (_tmp_buffer)
                    {
                        memset(_tmp_buffer, 0, my_data.content_length + 1);

                        s_rx_chunk_t *curr_chunk = my_data.rx_chunks;

                        while (curr_chunk)
                        {
                            strcat(_tmp_buffer, curr_chunk->ptr);
                            TRACE_D("%.*s", curr_chunk->len, curr_chunk->ptr);
                            curr_chunk = curr_chunk->next;
                        }

                        http_get_data->response = _tmp_buffer;
                    }
                }
                else
                {
                    TRACE_E("Mismatched 'Content length' and 'Received length'!");
                }
            }
            else
            {
                TRACE_E("Error perform http request %s", esp_err_to_name(err));
            }

            __ezlopi_http_free_rx_data(my_data.rx_chunks);
            esp_http_client_cleanup(client);
        }
    }

    return http_get_data;
}

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/
/**
 * @brief Function Trigger http_requests via mbedTLS.
 *
 * @return Address of a memory_block ; (char*)ezlopi_malloc(...)
 */
static void __ezlopi_http_req_via_mbedTLS(const char *host_web_server, int web_port_num, const char *url_req, char **resp_buf)
{
    // TRACE_I("&result==[%p] --> *resp_buf=>[%p]", resp_buf, *resp_buf);
    int ret, flags, len;
    uint32_t tmp_buf_size = 256;
    char tmp_buf[tmp_buf_size];

    char web_port[10] = {0};
    snprintf(web_port, 10, "%d", web_port_num);
    web_port[9] = '\0';

    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl;
    mbedtls_x509_crt cacert;
    mbedtls_ssl_config conf;

    mbedtls_net_context server_fd;
    mbedtls_ssl_init(&ssl);
    mbedtls_x509_crt_init(&cacert);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_ssl_config_init(&conf);
    mbedtls_entropy_init(&entropy);
    if (0 != (ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                          NULL, 0)))
    {
        TRACE_E("mbedtls_ctr_drbg_seed returned %d", ret);
        goto exit;
    }

    ret = esp_crt_bundle_attach(&conf);
    if (ret < 0)
    {
        TRACE_E("esp_crt_bundle_attach returned -0x%x\n\n", -ret);
        goto exit;
    }

    /* Hostname set here should match CN in server certificate */
    if (0 != (ret = mbedtls_ssl_set_hostname(&ssl, host_web_server)))
    {
        TRACE_E("mbedtls_ssl_set_hostname returned -0x%x", -ret);
        goto exit;
    }

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
    mbedtls_esp_enable_debug_log(&conf, CONFIG_MBEDTLS_DEBUG_LEVEL);
#endif

    if (0 != (ret = mbedtls_ssl_setup(&ssl, &conf)))
    {
        TRACE_E("mbedtls_ssl_setup returned -0x%x\n\n", -ret);
        goto exit;
    }

    mbedtls_net_init(&server_fd);

    if (0 != (ret = mbedtls_net_connect(&server_fd, host_web_server,
                                        web_port, MBEDTLS_NET_PROTO_TCP)))
    {
        TRACE_E("mbedtls_net_connect returned -%x", -ret);
        goto exit;
    }

    mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

    time_t start_tm = 0, now = 0; // now keeping track of time
    time(&now);
    start_tm = now;

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

    if (0 != (flags = mbedtls_ssl_get_verify_result(&ssl)))
    {
        /* In real life, we probably want to close connection if ret != 0 */
        TRACE_E("Failed to verify peer certificate!");
        bzero(tmp_buf, tmp_buf_size);
        mbedtls_x509_crt_verify_info(tmp_buf, tmp_buf_size, "  ! ", flags);
        TRACE_E("verification Error_info: %s", tmp_buf);
        goto exit;
    }

    size_t written_bytes = 0;
    do
    {
        ret = mbedtls_ssl_write(&ssl, (const unsigned char *)url_req + written_bytes,
                                strlen(url_req) - written_bytes);
        if (ret >= 0)
        {
            written_bytes += ret;
        }
        else if (ret != MBEDTLS_ERR_SSL_WANT_WRITE && ret != MBEDTLS_ERR_SSL_WANT_READ)
        {
            TRACE_E("mbedtls_ssl_write returned -0x%x", -ret);
            goto exit;
        }
    } while (written_bytes < strlen(url_req));

    uint32_t resp_buf_size = tmp_buf_size + 1;
    char *resp_buf_dummy = (char *)ezlopi_malloc(__FUNCTION__, resp_buf_size); // points to a memory-block
    if (resp_buf_dummy)
    {
        bzero(resp_buf_dummy, resp_buf_size); // clear the buffer
        uint8_t reply_count = 0;
        do
        {
            len = tmp_buf_size - 1;
            bzero(tmp_buf, tmp_buf_size);
            ret = mbedtls_ssl_read(&ssl, (unsigned char *)tmp_buf, len);
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
                break;
            }

            len = ret;

            if (ret > 0)
            {
                reply_count++;
                if (reply_count > 1)
                {
                    resp_buf_size += (len + 5); // (+2)makes sure ; additional space for '\0'
                    if (EZPI_SUCCESS == EZPI_core_http_dyna_relloc(&resp_buf_dummy, resp_buf_size))
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

    TRACE_I("Completed a request");
}

/**
 * @brief Function to extract the parameter values from 'config' struct and append it to '*request'
 *
 * @param config  : custom struct consisting [headers + content + server_name + web_port ...]
 * @param request : *ptr that holds the complete request_url
 * @param request_len : total char-capacity the '*request' can contain.
 */
static void __ezlopi_http_generate_request(s_ezlopi_core_http_mbedtls_t *config, char **request, int request_len)
{
    if ((NULL != config) && (NULL != *request) && (request_len > 0))
    {
        // 1. Identify Http-Method
        switch (config->method)
        {
        case HTTP_METHOD_GET:
        {
            if (((NULL != config->username) && (GET_STRING_SIZE(config->username) > 0)) &&
                ((NULL != config->password) && (GET_STRING_SIZE(config->password) > 0)))
            {
                snprintf(*request, request_len, "GET %s?username=%s&password=%s HTTP/1.1\r\nUser-Agent: esp-idf/1.0 esp32\r\n", config->url, config->username, config->password);
            }
            else
            {
                snprintf(*request, request_len, "GET /%s HTTP/1.1\r\nUser-Agent: esp-idf/1.0 esp32\r\n", config->target_page);
            }
            break;
        }
        case HTTP_METHOD_POST:
        {
            snprintf(*request, request_len, "POST /%s HTTP/1.1\r\nUser-Agent: esp-idf/1.0 esp32\r\n", config->target_page);
            break;
        }
        case HTTP_METHOD_PUT:
        {
            snprintf(*request, request_len, "PUT /%s HTTP/1.1\r\nUser-Agent: esp-idf/1.0 esp32\r\n", config->target_page);
            break;
        }
        case HTTP_METHOD_DELETE:
        {
            snprintf(*request, request_len, "DELETE /%s HTTP/1.1\r\nUser-Agent: esp-idf/1.0 esp32\r\n", config->target_page);
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
            max_allowed = EZPI_core_http_calc_empty_bufsize(*request, request_len, (strlen(config->header) + 3));
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
                max_allowed = EZPI_core_http_calc_empty_bufsize(*request, request_len, (strlen(config->username) + strlen(config->password) + strlen(config->content) + 3));
            }
            else
            {
                max_allowed = EZPI_core_http_calc_empty_bufsize(*request, request_len, (strlen(config->content) + 3));
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

/**
 * @brief Function to handle http-event
 *
 * @param evt Pointer to http event struct
 * @return esp_err_t
 */
static esp_err_t __ezlopi_http_event_handler(esp_http_client_event_t *evt)
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
        break;
    }
    case HTTP_EVENT_ON_HEADER:
    {
        if (0 == strncmp("Content-Length", evt->header_key, ((strlen(evt->header_key) + 1) > 15) ? (strlen(evt->header_key) + 1) : 15))
        {
            s_rx_data_t *my_data = evt->user_data;
            if (my_data)
            {
                my_data->content_length = strtoul(evt->header_value, NULL, 10);
            }
        }
        break;
    }
    case HTTP_EVENT_ON_DATA:
    {
        TRACE_D("HTTP_EVENT_ON_DATA, len=%d, data: %.*s\r\n", evt->data_len, evt->data_len, (char *)evt->data);

        s_rx_data_t *my_data = (s_rx_data_t *)evt->user_data;
        if (my_data)
        {
            char *tmp_data = (char *)ezlopi_malloc(__FUNCTION__, evt->data_len);
            if (NULL != tmp_data)
            {
                memcpy(tmp_data, evt->data, evt->data_len);

                if (NULL == my_data->rx_chunks)
                {
                    my_data->rx_chunks = ezlopi_malloc(__FUNCTION__, sizeof(s_rx_chunk_t));
                    if (my_data->rx_chunks)
                    {
                        my_data->rx_chunks->ptr = tmp_data;
                        my_data->rx_chunks->len = evt->data_len;
                        my_data->rx_chunks->next = NULL;

                        my_data->rx_len += evt->data_len;
                        my_data->status = 0;
                    }
                    else
                    {
                        my_data->status = -1;
                        TRACE_E("malloc-failed!");
                    }
                }
                else
                {
                    int count = 1;
                    s_rx_chunk_t *curr_chunk = my_data->rx_chunks;
                    while (curr_chunk->next)
                    {
                        count += 1;
                        curr_chunk = curr_chunk->next;
                    }

                    curr_chunk->next = ezlopi_malloc(__FUNCTION__, sizeof(s_rx_chunk_t));
                    if (curr_chunk->next)
                    {
                        curr_chunk->next->ptr = tmp_data;
                        curr_chunk->next->len = evt->data_len;
                        curr_chunk->next->next = NULL;

                        my_data->rx_len += evt->data_len;
                        my_data->status = 0;
                        TRACE_W("chunk-count: %d", count + 1);
                    }
                    else
                    {
                        my_data->status = -1;
                        TRACE_E("malloc-failed!");
                    }
                }
            }
            else
            {
                my_data->status = -1;
                TRACE_E("malloc-failed!");
            }
        }
        else
        {
            TRACE_E("my_data: NULL");
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

    vTaskDelay(10 / portTICK_RATE_MS);
    return ESP_OK;
}

/**
 * @brief Function to free rx-data recieved from http requests
 *
 * @param rx_chunks
 */
static void __ezlopi_http_free_rx_data(s_rx_chunk_t *rx_chunks)
{
    if (rx_chunks)
    {
        if (rx_chunks->next)
        {
            __ezlopi_http_free_rx_data(rx_chunks->next);
        }

        if (rx_chunks->ptr)
        {
            ezlopi_free(__FUNCTION__, rx_chunks->ptr);
        }
        ezlopi_free(__FUNCTION__, rx_chunks);
    }
}

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
