#include "trace.h"
#include "string.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "ezlopi_http.h"
#include "ezlopi_devices.h"
#include "ezlopi_scenes_v2.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_factory_info.h"
#include "ezlopi_nvs.h"
#include "ezlopi_scenes_then_methods.h"
#include "esp_crt_bundle.h"
#include "esp_tls.h"

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

int ezlopi_scene_then_set_item_value(l_scenes_list_v2_t *curr_scene, void *arg)
{
    int ret = 0;
    uint32_t item_id = 0;
    // uint32_t value = 0;

    cJSON *cj_params = cJSON_CreateObject();

    if (cj_params)
    {
        l_action_block_v2_t *curr_then = (l_action_block_v2_t *)arg;
        if (curr_then)
        {
            l_fields_v2_t *curr_field = curr_then->fields;
            while (curr_field)
            {
                if (0 == strncmp(curr_field->name, "item", 4))
                {
                    cJSON_AddStringToObject(cj_params, "_id", curr_field->value.value_string);
                    item_id = strtoul(curr_field->value.value_string, NULL, 16);
                    // TRACE_D("item_id: %s", curr_field->value.value_string);
                }
                else if (0 == strncmp(curr_field->name, "value", 5))
                {
                    if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
                    {
                        cJSON_AddNumberToObject(cj_params, "value", curr_field->value.value_int);
                        // TRACE_D("value: %f", curr_field->value.value_double);
                    }
                    else if (EZLOPI_VALUE_TYPE_BOOL == curr_field->value_type)
                    {
                        cJSON_AddBoolToObject(cj_params, "value", curr_field->value.value_bool);
                        // TRACE_D("value: %s", curr_field->value.value_bool ? "true" : "false");
                    }
                    else if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type)
                    {
                        cJSON_AddStringToObject(cj_params, "value", curr_field->value.value_string);
                        // TRACE_D("value: %s", curr_field->value.value_string);
                    }
                }

                curr_field = curr_field->next;
            }

            l_ezlopi_device_t *curr_device = ezlopi_device_get_head();
            uint32_t found_item = 0;
            while (curr_device)
            {
                l_ezlopi_item_t *curr_item = curr_device->items;
                while (curr_item)
                {
                    if (item_id == curr_item->cloud_properties.item_id)
                    {
                        curr_item->func(EZLOPI_ACTION_SET_VALUE, curr_item, cj_params, curr_item->user_arg);
                        found_item = 1;
                        break;
                    }
                    curr_item = curr_item->next;
                }
                if (found_item)
                {
                    TRACE_D("\r\n\r\n FOUND DEVICE \r\n\r\n");
                    break;
                }
                curr_device = curr_device->next;
            }
        }

        cJSON_Delete(cj_params);
    }

    return ret;
}
int ezlopi_scene_then_set_device_armed(l_scenes_list_v2_t *curr_scene, void *arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_send_cloud_abstract_command(l_scenes_list_v2_t *curr_scene, void *arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_switch_house_mode(l_scenes_list_v2_t *curr_scene, void *arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}

//---------------------------------------------------------------------------------------
typedef struct s_ezlopi_scenes_then_methods_send_http
{
    char web_port[5];
    char url[200];
    char web_server[100];
    char header[256];
    char content[256];
    char username[32];
    char password[32];
    bool skip_cert_common_name_check;
    esp_http_client_method_t method;
} s_ezlopi_scenes_then_methods_send_http_t;

static void __https_using_mbedTLS(const char *web_server, const char *web_port, const char *url_req)
{
    char tmp_buf[512];
    int ret, flags, len;

    static mbedtls_entropy_context entropy;
    static mbedtls_ctr_drbg_context ctr_drbg;
    static mbedtls_ssl_context ssl;
    static mbedtls_x509_crt cacert;
    static mbedtls_ssl_config conf;
    static mbedtls_net_context server_fd;

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
        // abort();
        goto exit;
    }
    TRACE_I("Attaching the certificate bundle...");

    ret = esp_crt_bundle_attach(&conf);

    if (ret < 0)
    {
        TRACE_E("esp_crt_bundle_attach returned -0x%x\n\n", -ret);
        // abort();
        goto exit;
    }
    TRACE_I("Setting hostname for TLS session...");

    /* Hostname set here should match CN in server certificate */
    if ((ret = mbedtls_ssl_set_hostname(&ssl, web_server)) != 0)
    {
        TRACE_E("mbedtls_ssl_set_hostname returned -0x%x", -ret);
        // abort();
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
    TRACE_D("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());

    while ((ret = mbedtls_ssl_handshake(&ssl)) != 0)
    {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
        {
            TRACE_E("mbedtls_ssl_handshake returned -0x%x", -ret);
            TRACE_D("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());

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
    TRACE_D("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());

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
    TRACE_D("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());
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
    TRACE_D("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());

exit:
    // mbedtls_ssl_session_reset(&ssl);
    // TRACE_D("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());
    mbedtls_net_free(&server_fd);
    TRACE_D("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());
    if (ret != 0)
    {
        mbedtls_strerror(ret, tmp_buf, 100);
        TRACE_E("Last error was: -0x%x - %s", -ret, tmp_buf);
    }

    static int request_count;
    TRACE_I("Completed %d requests", ++request_count);
    TRACE_D("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());

    mbedtls_ssl_free(&ssl);
    mbedtls_ssl_config_free(&conf);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
    TRACE_D("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());
}

static void __scenes_then_method_http_request_api(s_ezlopi_scenes_then_methods_send_http_t *config, cJSON *tmp_header)
{
    TRACE_W("URI :- '%s' [%d]", config->url, strlen(config->url));
    TRACE_W("WEB_SERVER :- '%s' [%d]", config->web_server, strlen(config->web_server));
    TRACE_W("WEB_PORT :- '%s' ", config->web_port);
    TRACE_W("content : %s", config->content);
    TRACE_W("skip_cert : %s", (config->skip_cert_common_name_check) ? "true" : "false");

    // char *tmp_ca_certificate = ezlopi_factory_info_v2_get_ca_certificate();
    // char *tmp_ssl_shared_key = ezlopi_factory_info_v2_get_ssl_shared_key();
    // char *tmp_ssl_private_key = ezlopi_factory_info_v2_get_ssl_private_key();
    char *tmp_ca_certificate = NULL;
    char *tmp_ssl_shared_key = NULL;
    char *tmp_ssl_private_key = NULL;

    esp_http_client_config_t tmp_http_config = {
        .auth_type = HTTP_AUTH_TYPE_NONE,
        .method = config->method,
        .skip_cert_common_name_check = config->skip_cert_common_name_check,
        .use_global_ca_store = true,
        .crt_bundle_attach = esp_crt_bundle_attach,
        .keep_alive_enable = true,
        // .timeout_ms = 30000,         // 30sec
        // .max_redirection_count = 10, // default 0
        // .keep_alive_idle = 60000,
    };

    s_ezlopi_http_data_t *http_reply = NULL;
    char REQUEST[512] = {'\0'};
    switch (config->method)
    {
    case HTTP_METHOD_GET:
    {
        snprintf(REQUEST, sizeof(REQUEST), "GET %s HTTP/1.0\r\nUser-Agent: esp-idf/1.0 esp32\r\n\r\n", config->url);
        



        TRACE_W(" REQUEST : %s = [%d]\n", REQUEST, strlen(REQUEST));
        __https_using_mbedTLS(config->web_server, config->web_port, REQUEST);
        TRACE_W("HTTP GET-METHOD [%d] : ", tmp_http_config.method);
        http_reply = ezlopi_http_get_request(config->url, tmp_header, tmp_ssl_private_key, tmp_ssl_shared_key, tmp_ca_certificate, &tmp_http_config);
        break;
    }
    case HTTP_METHOD_POST:
    {
        snprintf(REQUEST, sizeof(REQUEST), "POST %s HTTP/1.0\r\nUser-Agent: esp-idf/1.0 esp32\r\n\r\n", config->url);
        TRACE_W(" REQUEST : %s = [%d]\n", REQUEST, strlen(REQUEST));
        __https_using_mbedTLS(config->web_server, config->web_port, REQUEST);

        TRACE_W("HTTP POST-METHOD [%d]", tmp_http_config.method);
        // http_reply = ezlopi_http_post_request(config->url, "", config->content, NULL, tmp_ssl_private_key, tmp_ssl_shared_key, tmp_ca_certificate, &tmp_http_config);
        break;
    }
    // case HTTP_METHOD_PUT:
    // {
    //     TRACE_W("HTTP PUT-METHOD [%d]", tmp_http_config.method);
    //     http_reply = ezlopi_http_put_request(config->url, tmp_header, tmp_ssl_private_key, tmp_ssl_shared_key, tmp_ca_certificate, &tmp_http_config);
    //     break;
    // }
    // case HTTP_METHOD_DELETE:
    // {
    //     TRACE_W("HTTP DELETE-METHOD [%d]", tmp_http_config.method);
    //     http_reply = ezlopi_http_delete_request(config->url, tmp_header, tmp_ssl_private_key, tmp_ssl_shared_key, tmp_ca_certificate, &tmp_http_config);
    //     break;
    // }
    default:
        break;
    }
    if (http_reply)
    {
        TRACE_I("HTTP METHOD[_%d_] Status_resonse = %s, Status_code = %d",
                config->method,
                http_reply->response,
                http_reply->status_code);

        if (http_reply->response)
        {
            free(http_reply->response);
        }
        free(http_reply);
    }
}

int ezlopi_scene_then_send_http_request(l_scenes_list_v2_t *curr_scene, void *arg)
{
    int ret = 0;
    l_action_block_v2_t *curr_then = (l_action_block_v2_t *)arg;
    if (curr_then)
    {
        // configuration for http_request
        s_ezlopi_scenes_then_methods_send_http_t *tmp_http_data = (s_ezlopi_scenes_then_methods_send_http_t *)malloc(sizeof(s_ezlopi_scenes_then_methods_send_http_t));
        if (tmp_http_data)
        {
            memset(tmp_http_data, 0, sizeof(s_ezlopi_scenes_then_methods_send_http_t));

            cJSON *cj_header = cJSON_CreateObject();
            if (cj_header) //  headers
            {
                l_fields_v2_t *curr_field = curr_then->fields;
                while (NULL != curr_field) // fields
                {
                    // create a  requrest line
                    if (0 == strncmp(curr_field->name, "url", 4))
                    {
                        if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && (NULL != curr_field->value.value_string))
                        {
                            snprintf(tmp_http_data->url, sizeof(tmp_http_data->url), "%s", curr_field->value.value_string);
                            snprintf(tmp_http_data->web_port, sizeof(tmp_http_data->web_port), "%s", (NULL != strstr(curr_field->value.value_string, "https")) ? "443" : "80");
                            char *start = strstr(curr_field->value.value_string, "://");
                            if (start != NULL)
                            {
                                start += 3;
                                char *end = strchr(start, '/');
                                if (end != NULL)
                                { // Calculate the length between "://" and the next '/'
                                    int length = end - start;
                                    char domain[100] = {'\0'};
                                    strncpy(domain, start, ((length > 100) ? 100 : length));
                                    domain[100] = '\0';
                                    tmp_http_data->web_server[100] = '\0';
                                    snprintf(tmp_http_data->web_server, sizeof(tmp_http_data->web_server), "%s", domain);
                                }
                            }
                        }
                    }
                    else if (0 == strncmp(curr_field->name, "request", 8))
                    {
                        if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type)
                        {
                            if (0 == strncmp(curr_field->value.value_string, "GET", 4))
                            {
                                tmp_http_data->method = HTTP_METHOD_GET;
                            }
                            if (0 == strncmp(curr_field->value.value_string, "POST", 5))
                            {
                                tmp_http_data->method = HTTP_METHOD_POST;
                            }
                            if (0 == strncmp(curr_field->value.value_string, "PUT", 4))
                            {
                                tmp_http_data->method = HTTP_METHOD_PUT;
                            }
                            if (0 == strncmp(curr_field->value.value_string, "DELETE", 7))
                            {
                                tmp_http_data->method = HTTP_METHOD_DELETE;
                            }
                        }
                    }
                    else if (0 == strncmp(curr_field->name, "credential", 11))
                    {
                        if (EZLOPI_VALUE_TYPE_CREDENTIAL == curr_field->value_type)
                        {
                            if (NULL != curr_field->value.value_json)
                            {
                                char *cj_ptr = cJSON_Print(curr_field->value.value_json);
                                if (cj_ptr)
                                {
                                    TRACE_W("-user/pass sent:-\n%s\n", cj_ptr);
                                    cJSON_free(cj_ptr);

                                    cJSON *userItem = cJSON_GetObjectItem(curr_field->value.value_json, "user");
                                    cJSON *passwordItem = cJSON_GetObjectItem(curr_field->value.value_json, "password");
                                    if ((userItem != NULL) && (passwordItem != NULL))
                                    {
                                        const char *userValue = cJSON_GetStringValue(userItem);
                                        const char *passValue = cJSON_GetStringValue(passwordItem);
                                        snprintf(tmp_http_data->username, sizeof(tmp_http_data->username), "%s", userValue);
                                        snprintf(tmp_http_data->password, sizeof(tmp_http_data->password), "%s", passValue);
                                    }
                                }
                                else
                                {
                                    TRACE_E("Missing 'username' or 'password' field in credential");
                                }
                            }
                        }
                    }
                    else if (0 == strncmp(curr_field->name, "contentType", 12))
                    {
                        if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && (NULL != curr_field->value.value_string))
                        {
                            cJSON_AddStringToObject(cj_header, "Content-Type", curr_field->value.value_string);
                        }
                    }
                    else if (0 == strncmp(curr_field->name, "content", 8))
                    {
                        if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && (NULL != curr_field->value.value_string))
                        {
                            snprintf(tmp_http_data->content, sizeof(tmp_http_data->content), "\r\n%s", curr_field->value.value_string);

                            uint8_t i = 0;
                            for (; i < sizeof(tmp_http_data->content); i++)
                            {
                                if ('\0' == tmp_http_data->content[i])
                                    break;
                            }
                            if (0 < i) // counting content length
                            {
                                char str[10];
                                snprintf(str, sizeof(str), "%d", i);
                                cJSON_AddStringToObject(cj_header, "Content-Length", str);
                                strncat(tmp_http_data->header,"Content-Length:",16);
                                
                            }
                        }
                    }
                    else if (0 == strncmp(curr_field->name, "headers", 8))
                    {
                        if (EZLOPI_VALUE_TYPE_DICTIONARY == curr_field->value_type)
                        {
                            if (NULL != curr_field->value.value_json)
                            {
                                char *cj_ptr = cJSON_Print(curr_field->value.value_json);
                                if (cj_ptr)
                                {
                                    TRACE_W("-HEADERS sent:-\n%s\n", cj_ptr);
                                    cJSON_free(cj_ptr);
                                }
                                cJSON *header = (curr_field->value.value_json->child);
                                while (header)
                                {
                                    cJSON_AddStringToObject(cj_header, header->string, header->valuestring);


                                    header = header->next;
                                }
                            }
                        }
                    }
                    else if (0 == strncmp(curr_field->name, "skipSecurity", 12))
                    {
                        if (EZLOPI_VALUE_TYPE_BOOL == curr_field->value_type)
                        {
                            tmp_http_data->skip_cert_common_name_check = curr_field->value.value_bool;
                        }
                    }

                    curr_field = curr_field->next;
                }

                // function to add the credential field in url or content-body
                if ((0 < strlen(tmp_http_data->username)) && (0 < strlen(tmp_http_data->password)))
                {
                    TRACE_B("writing the credential");
                    char cred[96] = {'\0'};
                    if (HTTP_METHOD_GET == tmp_http_data->method)
                    {
                        snprintf(cred, sizeof(cred), "?username=%s&password=%s", tmp_http_data->username, tmp_http_data->password);
                        strncat(tmp_http_data->url, cred, (int)strlen(cred));
                    }
                    else // for other http-request method
                    {
                        snprintf(cred, sizeof(cred), "\r\nuser:%s\r\npassword:%s", tmp_http_data->username, tmp_http_data->password);
                        strncat(tmp_http_data->content, cred, (int)strlen(cred));
                    }
                }

                // Invoke http-request
                __scenes_then_method_http_request_api(tmp_http_data, cj_header);

                cJSON_Delete(cj_header);
            }
            free(tmp_http_data);
        }
    }

    return ret;
}

//---------------------------------------------------------------------------------------

int ezlopi_scene_then_run_custom_script(l_scenes_list_v2_t *curr_scene, void *arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_run_plugin_script(l_scenes_list_v2_t *curr_scene, void *arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_run_scene(l_scenes_list_v2_t *curr_scene, void *arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_set_scene_state(l_scenes_list_v2_t *curr_scene, void *arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_reset_latch(l_scenes_list_v2_t *curr_scene, void *arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_reset_scene_latches(l_scenes_list_v2_t *curr_scene, void *arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_reboot_hub(l_scenes_list_v2_t *curr_scene, void *arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_reset_hub(l_scenes_list_v2_t *curr_scene, void *arg)
{
    int ret = 0;
    cJSON *cj_params = cJSON_CreateObject();

    if (cj_params)
    {
        l_action_block_v2_t *curr_then = (l_action_block_v2_t *)arg;
        if (curr_then)
        {
            l_fields_v2_t *curr_field = curr_then->fields;
            while (curr_field)
            {
                if (0 == strncmp(curr_field->name, "type", 5))
                {
                    if (EZLOPI_VALUE_TYPE_ENUM == curr_field->value_type)
                    {
                        TRACE_D("value: %s", curr_field->value.value_string);
                        if (0 == strncmp(curr_field->name, "factory", 8))
                        {
                            TRACE_E("Factory Reseting ESP... ");
                            // ezlopi_nvs_set_boot_count(0);
                            // nvs_erase_key(0, "wifi_info");
                            ezlopi_nvs_factory_reset();

                            esp_restart();
                        }
                        if (0 == strncmp(curr_field->name, "soft", 5))
                        {
                            TRACE_E("Rebooting ESP... ");
                            esp_restart();
                        }
                    }
                }

                curr_field = curr_field->next;
            }
        }

        cJSON_Delete(cj_params);
    }
    return ret;
}
int ezlopi_scene_then_cloud_api(l_scenes_list_v2_t *curr_scene, void *arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_set_expression(l_scenes_list_v2_t *curr_scene, void *arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_set_variable(l_scenes_list_v2_t *curr_scene, void *arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}
int ezlopi_scene_then_toggle_value(l_scenes_list_v2_t *curr_scene, void *arg)
{
    TRACE_W("Warning: then-method not implemented!");
    return 0;
}