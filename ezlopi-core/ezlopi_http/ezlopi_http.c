#include "string.h"

#include "trace.h"
#include "cJSON.h"
#include "esp_http_client.h"
#include "ezlopi_http.h"

#ifdef CONFIG_ESP_TLS_USING_MBEDTLS
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "mbedtls/platform.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/esp_debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"
#endif

static void ezlopi_http_free_rx_data(s_rx_data_t *rx_data);
static esp_err_t ezlopi_http_event_handler(esp_http_client_event_t *evt);

#define TAG __FILE__

#define FREE_IF_NOT_NULL(ptr) \
    {                         \
        if (ptr)              \
        {                     \
            free(ptr);        \
            ptr = NULL;       \
        }                     \
    }

//--------------- Scenes:- Sendhttp_request_method --------------------------
static void __ezlopi_http_request_via_mbedTLS(const char *web_server, int web_port_num, const char *url_req)
{
    int ret, flags, len;
    char tmp_buf[512];
    char web_port[10] = {0};
    snprintf(web_port, 10, "%d", web_port_num);
    web_port[10] = '\0';

    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl;
    mbedtls_x509_crt cacert;
    mbedtls_ssl_config conf;
    mbedtls_net_context server_fd;

    mbedtls_ssl_init(&ssl);
    mbedtls_x509_crt_init(&cacert);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    TRACE_I("Seeding the random number generator");
    mbedtls_ssl_config_init(&conf);
    mbedtls_entropy_init(&entropy);

    if ((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                     NULL, 0)) != 0)
    {
        TRACE_E("mbedtls_ctr_drbg_seed returned %d", ret);
        goto exit;
    }
    TRACE_I("Attaching the certificate bundle...");

    ret = esp_crt_bundle_attach(&conf);

    if (ret < 0)
    {
        TRACE_E("esp_crt_bundle_attach returned -0x%x\n\n", -ret);
        goto exit;
    }
    TRACE_I("Setting hostname for TLS session...");

    /* Hostname set here should match CN in server certificate */
    if ((ret = mbedtls_ssl_set_hostname(&ssl, web_server)) != 0)
    {
        TRACE_E("mbedtls_ssl_set_hostname returned -0x%x", -ret);
        goto exit;
    }

    TRACE_I("Setting up the SSL/TLS structure...");

    if ((ret = mbedtls_ssl_config_defaults(&conf,
                                           MBEDTLS_SSL_IS_CLIENT,
                                           MBEDTLS_SSL_TRANSPORT_STREAM,
                                           MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
    {
        TRACE_E("mbedtls_ssl_config_defaults returned %d", ret);
        goto exit;
    }

    /* MBEDTLS_SSL_VERIFY_OPTIONAL is bad for security, in this example it will print
       a warning if CA verification fails but it will continue to connect.
       You should consider using MBEDTLS_SSL_VERIFY_REQUIRED in your own code.
    */
    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
    mbedtls_ssl_conf_ca_chain(&conf, &cacert, NULL);
    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
#ifdef CONFIG_MBEDTLS_DEBUG
    mbedtls_esp_enable_debug_log(&conf, CONFIG_MBEDTLS_DEBUG_LEVEL);
#endif

    if ((ret = mbedtls_ssl_setup(&ssl, &conf)) != 0)
    {
        TRACE_E("mbedtls_ssl_setup returned -0x%x\n\n", -ret);

        goto exit;
    }
    mbedtls_net_init(&server_fd);

    TRACE_I("Connecting to %s:%s...", web_server, web_port);

    if ((ret = mbedtls_net_connect(&server_fd, web_server,
                                   web_port, MBEDTLS_NET_PROTO_TCP)) != 0)
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
        bzero(tmp_buf, sizeof(tmp_buf));
        mbedtls_x509_crt_verify_info(tmp_buf, sizeof(tmp_buf), "  ! ", flags);
        TRACE_W("verification info: %s", tmp_buf);
    }
    else
    {
        TRACE_I("Certificate verified.");
    }
    TRACE_I("Cipher suite is %s", mbedtls_ssl_get_ciphersuite(&ssl));
    TRACE_I("Writing HTTP request...");

    size_t written_bytes = 0;
    do
    {
        ret = mbedtls_ssl_write(&ssl, (const unsigned char *)url_req + written_bytes,
                                strlen(url_req) - written_bytes);
        if (ret >= 0)
        {
            TRACE_I("%d bytes written", ret);
            written_bytes += ret;
        }
        else if (ret != MBEDTLS_ERR_SSL_WANT_WRITE && ret != MBEDTLS_ERR_SSL_WANT_READ)
        {
            TRACE_E("mbedtls_ssl_write returned -0x%x", -ret);
            goto exit;
        }
    } while (written_bytes < strlen(url_req));

    TRACE_I("Reading HTTP response...");
    do
    {
        len = sizeof(tmp_buf) - 1;
        bzero(tmp_buf, sizeof(tmp_buf));
        ret = mbedtls_ssl_read(&ssl, (unsigned char *)tmp_buf, len);

        if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE)
        {
            continue;
        }
        if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)
        {
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
            TRACE_I("connection closed");
            break;
        }
        len = ret;
        TRACE_D(" [%d] bytes read :\n %s", len, tmp_buf);

    } while (1);

    mbedtls_ssl_close_notify(&ssl);
exit:
    // mbedtls_ssl_session_reset(&ssl); // dont need this line
    mbedtls_net_free(&server_fd);
    TRACE_D("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());
    if (ret != 0)
    {
        mbedtls_strerror(ret, tmp_buf, 100);
        TRACE_E("Last error was: -0x%x - %s", -ret, tmp_buf);
    }

    // Clearing used structures
    mbedtls_ssl_free(&ssl);
    mbedtls_ssl_config_free(&conf);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
    TRACE_I("Completed a request");
}
static int ezlopi_http_limit_size_check(char *dest_buff, int dest_size, int reqd_size)
{
    int limit = dest_size - (strlen(dest_buff) + 1);
    limit = (limit < 0) ? 0 : limit;
    if (limit > reqd_size)
    {
        return limit;
    }
    return 0;
}
static int fresh_dynamic_alloc(char **__dest_ptr, const char *src_ptr)
{
    int ret = 0;
    if (NULL != *__dest_ptr) // clear if occupied
    {
        free(*__dest_ptr);
        *__dest_ptr = NULL;
    }
    else if (NULL == *__dest_ptr) // do a fresh allocation
    {

        size_t dest_length = ((NULL != src_ptr) ? strlen(src_ptr) : 0) + 1; // non-zero
        char *tmp_ptr = malloc(sizeof(char) * dest_length);                 // automagically converted to the correct type.
        if (tmp_ptr)
        {
            bzero(tmp_ptr, (sizeof(char) * dest_length));
            TRACE_D("Dynamic allocation complete");
            if (NULL != src_ptr)
            {
                TRACE_B("Copying to newly allocated buffer");
                snprintf(tmp_ptr, dest_length, "%s", src_ptr);
            }
            tmp_ptr[dest_length] = '\0';
            *__dest_ptr = tmp_ptr;
            ret = 1;
        }
    }
    return ret;
}

static int dynamic_relloc_and_append(char **Buf, const char *src_ptr, size_t NewSize)
{
    if ((NULL != *Buf) && (NULL != src_ptr))
    {
        printf("*Buf :%p \n", *Buf);
        void *NewBuf = realloc(*Buf, sizeof(char) * NewSize);
        if ((NewBuf != NULL)) // newSize  = old + strlen(src_ptr)
        {
            printf("NewBuf :%s\n", NewBuf);
            snprintf(NewBuf + strlen(NewBuf), NewSize, "%s", src_ptr);
            printf("NewBuf :%s\n", NewBuf);
            *Buf = NewBuf;
            return 1; // return success
        }
    }
    return 0;
}

void ezlopi_http_scenes_then_parse_url(s_ezlopi_scenes_then_methods_send_http_t *tmp_http_data, const char *field_value_string)
{
    snprintf(tmp_http_data->url, sizeof(tmp_http_data->url), "%s", field_value_string);
#warning "web_port -> use number instead of string"
    tmp_http_data->web_port = (NULL != strstr(field_value_string, "https")) ? 443 : 80;
    // snprintf(tmp_http_data->web_port, sizeof(tmp_http_data->web_port), "%s", (NULL != strstr(field_value_string, "https")) ? "443" : "80");

    // parse_web_host_name
    char *start = strstr(field_value_string, "://");
    if (start != NULL)
    {
        start += 3;
        int buf_size = sizeof(tmp_http_data->web_server);
        int length = 0;
        char *end = strchr(start, '/');
        if (end != NULL)
        {
            length = end - start;
            if ((length + 1) < buf_size)
            {
                snprintf(tmp_http_data->web_server, (length + 1), "%s", start);
            }
        }
        else
        {
            const char *ptr = field_value_string;
            length = strlen(field_value_string) - (int)(start - ptr);
            if ((length + 1) < buf_size)
            {
                snprintf(tmp_http_data->web_server, (length + 1), "%s", (ptr + ((int)(start - ptr))));
            }
        }
        tmp_http_data->web_server[buf_size] = '\0';
    }

    // 1. adding 'host' to header-buffer
    int max_allowed = ezlopi_http_limit_size_check(tmp_http_data->header, sizeof(tmp_http_data->header), (6 + (strlen(tmp_http_data->web_server) + 1)) + 3);
    if (max_allowed > 0)
    {
        snprintf((tmp_http_data->header) + (strlen(tmp_http_data->header)), max_allowed, "Host: %s\r\n", tmp_http_data->web_server);
    }
}
void ezlopi_http_scenes_then_parse_content(s_ezlopi_scenes_then_methods_send_http_t *tmp_http_data, const char *field_value_string)
{
    snprintf(tmp_http_data->content, sizeof(tmp_http_data->content), "%s\r\n", field_value_string);
    uint32_t i = 0; // variable to store 'content-length'
    for (; i < strlen(field_value_string); i++)
    {
        if ('\0' == tmp_http_data->content[i])
            break;
    }
    if (i > 0)
    {
        char str[i];
        snprintf(str, sizeof(str), "%d", i);
        // 3. adding 'Content-Length' to header-buffer
        int max_allowed = ezlopi_http_limit_size_check(tmp_http_data->header, sizeof(tmp_http_data->header), (16 + strlen(str)) + 3);
        if (max_allowed > 0)
        {
            snprintf((tmp_http_data->header) + strlen(tmp_http_data->header), max_allowed, "Content-Length: %s\r\n", str);
        }
    }
}
void ezlopi_http_scenes_then_parse_content_type(s_ezlopi_scenes_then_methods_send_http_t *tmp_http_data, const char *field_value_string)
{
    // 2. adding 'Content-Type' to header-buffer
    int max_allowed = ezlopi_http_limit_size_check(tmp_http_data->header, sizeof(tmp_http_data->header), (14 + strlen(field_value_string)) + 3);
    if (max_allowed > 0)
    {
        snprintf((tmp_http_data->header) + (strlen(tmp_http_data->header)), max_allowed, "Content-Type: %s\r\n", field_value_string);
    }
}
void ezlopi_http_scenes_then_parse_headers(s_ezlopi_scenes_then_methods_send_http_t *tmp_http_data, cJSON *value_json)
{
    int max_allowed = 0;
    cJSON *header = (value_json->child);
    while (header)
    {
        // 4. adding 'remaining' to header-buffer
        max_allowed = ezlopi_http_limit_size_check(tmp_http_data->header, sizeof(tmp_http_data->header), ((strlen(header->string) + 1) + 2 + (strlen(header->valuestring) + 1)) + 3);
        if (max_allowed > 0)
        {
            snprintf((tmp_http_data->header) + (strlen(tmp_http_data->header)), max_allowed, "%s: %s\r\n", header->string, header->valuestring);
        }
        header = header->next;
    }
}
void ezlopi_http_scenes_then_parse_skipsecurity(s_ezlopi_scenes_then_methods_send_http_t *tmp_http_data, bool value_bool)
{
    // 4. adding 'remaining' to header-buffer
    tmp_http_data->skip_cert_common_name_check = value_bool;
    int max_allowed = ezlopi_http_limit_size_check(tmp_http_data->header, sizeof(tmp_http_data->header), (14 + (strlen((1 == value_bool) ? "true" : "false"))) + 3);
    if (max_allowed > 0)
    {
        snprintf((tmp_http_data->header) + strlen(tmp_http_data->header),
                 max_allowed,
                 "skipSecurity: %s\r\n",
                 ((value_bool) ? "true" : "false"));
    }
}
void ezlopi_http_scenes_then_parse_username_password(s_ezlopi_scenes_then_methods_send_http_t *tmp_http_data, cJSON *value_json)
{
    cJSON *userItem = cJSON_GetObjectItem(value_json, "user");
    cJSON *passwordItem = cJSON_GetObjectItem(value_json, "password");
    if ((NULL != userItem) && (NULL != passwordItem))
    {
        const char *userValue = cJSON_GetStringValue(userItem);
        const char *passValue = cJSON_GetStringValue(passwordItem);

        snprintf(tmp_http_data->username, sizeof(tmp_http_data->username), "%s", userValue);
        snprintf(tmp_http_data->password, sizeof(tmp_http_data->password), "%s", passValue);
    }
}
void ezlopi_http_scenes_then_sendhttp_request(s_ezlopi_scenes_then_methods_send_http_t *config)
{
    TRACE_W("skip_cert : %s", (config->skip_cert_common_name_check) ? "true" : "false");
    TRACE_W("[%d]WEB_PORT :- '%d' ", (sizeof(config->web_port)), config->web_port);
    TRACE_W("[%d]URI :- '%s' [%d]", (sizeof(config->url)), config->url, strlen(config->url));
    TRACE_W("[%d]WEB_SERVER :- '%s' [%d]", (sizeof(config->web_server)), config->web_server, strlen(config->web_server));
    TRACE_W("[%d]Content : occupied [%d] ", (sizeof(config->content)), strlen(config->content));
    TRACE_W("[%d]Header : occupied [%d] ", (sizeof(config->header)), strlen(config->header));

    char REQUEST[1024] = {'\0'}; // need to make it dynamic
    switch (config->method)
    {
    case HTTP_METHOD_GET:
    {
        TRACE_I("HTTP GET-METHOD [%d] : ", config->method);
        snprintf(REQUEST, sizeof(REQUEST), "GET %s HTTP/1.0\r\nUser-Agent: esp-idf/1.0 esp32\r\n", config->url);
        break;
    }
    case HTTP_METHOD_POST:
    {
        TRACE_I("HTTP POST-METHOD [%d]", config->method);
        snprintf(REQUEST, sizeof(REQUEST), "POST %s HTTP/1.0\r\nUser-Agent: esp-idf/1.0 esp32\r\n", config->url);
        break;
    }
    case HTTP_METHOD_PUT:
    {
        TRACE_I("HTTP PUT-METHOD [%d]", config->method);
        snprintf(REQUEST, sizeof(REQUEST), "PUT %s HTTP/1.0\r\nUser-Agent: esp-idf/1.0 esp32\r\n", config->url);
        break;
    }
    case HTTP_METHOD_DELETE:
    {
        TRACE_I("HTTP DELETE-METHOD [%d]", config->method);
        snprintf(REQUEST, sizeof(REQUEST), "DELETE %s HTTP/1.0\r\nUser-Agent: esp-idf/1.0 esp32\r\n", config->url);
        break;
    }
    default:
        break;
    }
    // adding 'Headers' to request_buffer
    int max_allowed = 0;
    max_allowed = ezlopi_http_limit_size_check(REQUEST, sizeof(REQUEST), (strlen(config->header) + 1) + 3);
    if (max_allowed > 0)
    {
        snprintf(REQUEST + (strlen(REQUEST)), max_allowed, "%s\r\n", config->header);
    }
    // adding content body to request
    max_allowed = ezlopi_http_limit_size_check(REQUEST, sizeof(REQUEST), (strlen(config->content) + 1) + 3);
    if (max_allowed > 0)
    {
        snprintf(REQUEST + (strlen(REQUEST)), max_allowed, "%s\r\n", config->content);
    }

    REQUEST[sizeof(REQUEST)] = '\0'; // null terminating array
    TRACE_D("REQUEST : \n%s = [%d]\n", REQUEST, strlen(REQUEST));

    // executing the request
    __ezlopi_http_request_via_mbedTLS(config->web_server, config->web_port, REQUEST);
}
//---------------------------------------------------------------------------

s_ezlopi_http_data_t *ezlopi_http_get_request(char *cloud_url, cJSON *headers, char *private_key, char *shared_key, char *ca_certificate, esp_http_client_config_t *tmp_config)
{
    char *ret = NULL;
    int status_code = 0;
    s_rx_data_t *my_data = (s_rx_data_t *)malloc(sizeof(s_rx_data_t));
    s_ezlopi_http_data_t *http_get_data = NULL;
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
            .user_data = (void *)(my_data), // my_data will be filled in 'ezlopi_http_event_handler'
        };
        if (NULL != tmp_config)
        {
            config.crt_bundle_attach = tmp_config->crt_bundle_attach,
            config.auth_type = tmp_config->auth_type,
            config.method = tmp_config->method;
            config.timeout_ms = tmp_config->timeout_ms; // Time for remote server to answer
            config.keep_alive_enable = tmp_config->keep_alive_enable;
            config.keep_alive_idle = tmp_config->keep_alive_idle; // Time for transfer response // default 5s
            config.use_global_ca_store = tmp_config->use_global_ca_store,
            config.max_redirection_count = tmp_config->max_redirection_count; // default 0
            config.skip_cert_common_name_check = tmp_config->skip_cert_common_name_check;
        }

        // TRACE_E("cloud_url: %s", cloud_url);
        // TRACE_E("ca_certificate: %s", ca_certificate);
        // TRACE_E("shared_key: %s", shared_key);
        // TRACE_E("private_key: %s", private_key);
        esp_http_client_handle_t client = esp_http_client_init(&config);
        if (NULL != client)
        {
            if (NULL != headers)
            {
                TRACE_B(" list of Headers :-");
                cJSON *header = headers->child;
                while (header)
                {
                    TRACE_B("%s: %s", header->string, header->valuestring);
                    esp_http_client_set_header(client, header->string, header->valuestring);
                    header = header->next;
                }
            }
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
                    ret = (char *)malloc(my_data->total_len + 1);
                    // TRACE_E("Response lent: %d", my_data->total_len);
                    if (ret)
                    {
                        s_rx_data_t *cur_d = my_data;
                        memset(ret, 0, my_data->total_len + 1);

                        while (cur_d)
                        {
                            strcat(ret, cur_d->ptr);
                            // TRACE_D("%.*s", cur_d->len, cur_d->ptr);
                            cur_d = cur_d->next;
                        }

                        http_get_data = (s_ezlopi_http_data_t *)malloc(sizeof(s_ezlopi_http_data_t));
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

s_ezlopi_http_data_t *ezlopi_http_post_request(char *cloud_url, char *location, char *post_content_data, cJSON *headers, char *private_key, char *shared_key, char *ca_certificate, esp_http_client_config_t *tmp_config)
{
    char *ret = NULL;
    int status_code = 0;
    s_rx_data_t *my_data = (s_rx_data_t *)malloc(sizeof(s_rx_data_t));
    s_ezlopi_http_data_t *http_get_data = malloc(sizeof(s_ezlopi_http_data_t));

    if (my_data)
    {
        memset(my_data, 0, sizeof(s_rx_data_t));

        char *uri = malloc(256);
        if (uri)
        {
            snprintf(uri, 256, "%s/%s", cloud_url, location);
            TRACE_D("URL: %s", uri);
        }
        else
        {
            uri = cloud_url;
        }

        esp_http_client_config_t config = {
            .url = uri,
            .cert_pem = ca_certificate,
            .client_cert_pem = shared_key,
            .client_key_pem = private_key,
            .event_handler = ezlopi_http_event_handler,
            .transport_type = HTTP_TRANSPORT_OVER_SSL,
            .user_data = (void *)(my_data), // my_data will be filled in 'ezlopi_http_event_handler'
        };

        if (NULL != tmp_config)
        {
            config.auth_type = tmp_config->auth_type,
            config.timeout_ms = tmp_config->timeout_ms; // Time for remote server to answer
            config.keep_alive_enable = tmp_config->keep_alive_enable;
            config.keep_alive_idle = tmp_config->keep_alive_idle; // Time for transfer response // default 5s
            config.disable_auto_redirect = tmp_config->disable_auto_redirect;
            config.max_redirection_count = tmp_config->max_redirection_count; // default 0
            config.max_authorization_retries = tmp_config->max_authorization_retries;
            config.skip_cert_common_name_check = tmp_config->skip_cert_common_name_check;
        }

        esp_http_client_handle_t client = esp_http_client_init(&config);
        if (NULL != client)
        {
            esp_http_client_set_method(client, HTTP_METHOD_POST);
            if (NULL != headers)
            {
                TRACE_B(" list of Headers :-");
                cJSON *header = headers->child;
                while (header)
                {
                    TRACE_B("%s: %s", header->string, header->valuestring);
                    esp_http_client_set_header(client, header->string, header->valuestring);
                    header = header->next;
                }
            }
            if (NULL != post_content_data)
            {
                char tmp_str[100] = {0};
                snprintf(tmp_str, sizeof(tmp_str), "%s", post_content_data);
                esp_http_client_set_post_field(client, tmp_str, sizeof(tmp_str));
            }
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
                    ret = (char *)malloc(my_data->total_len + 1);

                    if (ret)
                    {
                        s_rx_data_t *cur_d = my_data;
                        memset(ret, 0, my_data->total_len + 1);

                        while (cur_d)
                        {
                            strcat(ret, cur_d->ptr);
                            // TRACE_D("%.*s", cur_d->len, cur_d->ptr);
                            cur_d = cur_d->next;
                        }
                    }
                    http_get_data->response = ret;
                    http_get_data->status_code = status_code;
                }
                http_get_data->response = ret;
                http_get_data->status_code = status_code;
            }
            else
            {
                TRACE_E("Error perform http request %s", esp_err_to_name(err));
                free(http_get_data);
                http_get_data = NULL;
            }

            ezlopi_http_free_rx_data(my_data);
            esp_http_client_cleanup(client);
        }
    }
    return http_get_data;
}

s_ezlopi_http_data_t *ezlopi_http_put_request(char *cloud_url, cJSON *headers, char *private_key, char *shared_key, char *ca_certificate, esp_http_client_config_t *tmp_http_config)
{
    s_ezlopi_http_data_t *http_get_data = NULL;
    http_get_data = ezlopi_http_get_request(cloud_url, headers, private_key, shared_key, ca_certificate, tmp_http_config);
    return http_get_data;
}

s_ezlopi_http_data_t *ezlopi_http_delete_request(char *cloud_url, cJSON *headers, char *private_key, char *shared_key, char *ca_certificate, esp_http_client_config_t *tmp_http_config)
{
    s_ezlopi_http_data_t *http_get_data = NULL;
    http_get_data = ezlopi_http_get_request(cloud_url, headers, private_key, shared_key, ca_certificate, tmp_http_config);
    return http_get_data;
}

static esp_err_t ezlopi_http_event_handler(esp_http_client_event_t *evt)
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
        TRACE_D("HTTP_EVENT_ON_DATA, len=%d, data: %.*s", evt->data_len, evt->data_len, (char *)evt->data);
        // if (!esp_http_client_is_chunked_response(evt->client))
        {
            if (evt->user_data)
            {
                char *tmp_data = (char *)malloc(evt->data_len + 1);
                if (NULL != tmp_data)
                {
                    memcpy(tmp_data, evt->data, evt->data_len);
                    tmp_data[evt->data_len] = '\0';

                    s_rx_data_t *my_data = (s_rx_data_t *)evt->user_data;
                    if (my_data->len || my_data->total_len || my_data->ptr)
                    {
                        s_rx_data_t *cur_dh = my_data;
                        while (cur_dh->next)
                        {
                            cur_dh = cur_dh->next;
                        }

                        cur_dh->next = (s_rx_data_t *)malloc(sizeof(s_rx_data_t));
                        if (cur_dh->next)
                        {
                            cur_dh->next->len = evt->data_len;
                            cur_dh->next->ptr = (char *)tmp_data;
                            cur_dh->next->total_len = 0;
                            my_data->total_len += evt->data_len;
                            cur_dh->next->next = NULL;
                        }
                    }
                    else
                    {
                        my_data->len = evt->data_len;
                        my_data->total_len = evt->data_len;
                        my_data->ptr = (char *)tmp_data;
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

static void ezlopi_http_free_rx_data(s_rx_data_t *rx_data)
{
    if (rx_data)
    {
        if (rx_data->next)
        {
            ezlopi_http_free_rx_data(rx_data->next);
        }

        if (rx_data->ptr)
        {
            free(rx_data->ptr);
        }
        free(rx_data);
    }
}
