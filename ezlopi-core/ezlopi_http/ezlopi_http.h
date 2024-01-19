#ifndef __HTTP_H__
#define __HTTP_H__

#include "stdio.h"
#include "string.h"
#include "esp_err.h"
#include "esp_log.h"
#include "cJSON.h"
#include "esp_tls.h"
#include "esp_crt_bundle.h"
#include "esp_event.h"
#include "esp_http_client.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct s_rx_data
    {
        char *ptr;
        int len;
        int total_len;
        struct s_rx_data *next;

    } s_rx_data_t;
    typedef struct ezlopi_http_data
    {
        char *response;
        int status_code;

    } s_ezlopi_http_data_t;

#warning "'s_ezlopi_scenes_then_methods_send_http_t' its size look too big"
    typedef struct s_ezlopi_scenes_then_methods_send_http
    {
        bool skip_cert_common_name_check;
        int web_port;
        uint32_t url_maxlen;
        uint32_t web_server_maxlen;
        uint32_t header_maxlen;
        uint32_t content_maxlen;
        uint32_t username_maxlen;
        uint32_t password_maxlen;
        char url[196];
        char web_server[96];
        char header[256];
        char content[384];
        char username[32];
        char password[32];
        esp_http_client_method_t method;
    } s_ezlopi_scenes_then_methods_send_http_t;

    void ezlopi_http_scenes_then_parse_url(s_ezlopi_scenes_then_methods_send_http_t *tmp_http_data, const char *field_value_string);
    void ezlopi_http_scenes_then_parse_content(s_ezlopi_scenes_then_methods_send_http_t *tmp_http_data, const char *field_value_string);
    void ezlopi_http_scenes_then_parse_content_type(s_ezlopi_scenes_then_methods_send_http_t *tmp_http_data, const char *field_value_string);
    void ezlopi_http_scenes_then_parse_headers(s_ezlopi_scenes_then_methods_send_http_t *tmp_http_data, cJSON *value_json);
    void ezlopi_http_scenes_then_parse_skipsecurity(s_ezlopi_scenes_then_methods_send_http_t *tmp_http_data, bool value_bool);
    void ezlopi_http_scenes_then_parse_username_password(s_ezlopi_scenes_then_methods_send_http_t *tmp_http_data, cJSON *value_json);
    void ezlopi_http_scenes_then_sendhttp_request(s_ezlopi_scenes_then_methods_send_http_t *config);

    s_ezlopi_http_data_t *ezlopi_http_get_request(char *cloud_url, cJSON *headers, char *private_key, char *shared_key, char *ca_certificate, esp_http_client_config_t *tmp_config);
    s_ezlopi_http_data_t *ezlopi_http_post_request(char *cloud_url, char *location, char *post_content_data, cJSON *headers, char *private_key, char *shared_key, char *ca_certificate, esp_http_client_config_t *tmp_config);
    s_ezlopi_http_data_t *ezlopi_http_put_request(char *cloud_url, cJSON *headers, char *private_key, char *shared_key, char *ca_certificate, esp_http_client_config_t *tmp_config);
    s_ezlopi_http_data_t *ezlopi_http_delete_request(char *cloud_url, cJSON *headers, char *private_key, char *shared_key, char *ca_certificate, esp_http_client_config_t *tmp_config);
#ifdef __cplusplus
}
#endif

#endif // __HTTP_H__
