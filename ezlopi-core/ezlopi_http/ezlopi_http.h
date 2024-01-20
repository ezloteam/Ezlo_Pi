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
        uint16_t web_port;
        uint16_t url_maxlen; // cap to 65536
        uint16_t web_server_maxlen;
        uint16_t header_maxlen;
        uint16_t content_maxlen;
        uint8_t username_maxlen; // cap to 256
        uint8_t password_maxlen;
        char *url;
        char *web_server;
        char *header;
        char *content;
        char *username;
        char *password;
        esp_http_client_method_t method;
    } s_ezlopi_scenes_then_methods_send_http_t;

    void ezlopi_http_scenes_then_parse_url(s_ezlopi_scenes_then_methods_send_http_t *tmp_http_data, const char *field_value_string);
    void ezlopi_http_scenes_then_parse_content(s_ezlopi_scenes_then_methods_send_http_t *tmp_http_data, const char *field_value_string);
    void ezlopi_http_scenes_then_parse_content_type(s_ezlopi_scenes_then_methods_send_http_t *tmp_http_data, const char *field_value_string);
    void ezlopi_http_scenes_then_parse_headers(s_ezlopi_scenes_then_methods_send_http_t *tmp_http_data, cJSON *value_json);
    void ezlopi_http_scenes_then_parse_skipsecurity(s_ezlopi_scenes_then_methods_send_http_t *tmp_http_data, bool value_bool);
    void ezlopi_http_scenes_then_parse_username_password(s_ezlopi_scenes_then_methods_send_http_t *tmp_http_data, cJSON *value_json);
    void ezlopi_http_scenes_then_clear_struct_ptr_mem(s_ezlopi_scenes_then_methods_send_http_t *config);
    void ezlopi_http_scenes_then_sendhttp_request(s_ezlopi_scenes_then_methods_send_http_t *config);

    s_ezlopi_http_data_t *ezlopi_http_get_request(char *cloud_url, cJSON *headers, char *private_key, char *shared_key, char *ca_certificate, esp_http_client_config_t *tmp_config);
    s_ezlopi_http_data_t *ezlopi_http_post_request(char *cloud_url, char *location, char *post_content_data, cJSON *headers, char *private_key, char *shared_key, char *ca_certificate, esp_http_client_config_t *tmp_config);
    s_ezlopi_http_data_t *ezlopi_http_put_request(char *cloud_url, cJSON *headers, char *private_key, char *shared_key, char *ca_certificate, esp_http_client_config_t *tmp_config);
    s_ezlopi_http_data_t *ezlopi_http_delete_request(char *cloud_url, cJSON *headers, char *private_key, char *shared_key, char *ca_certificate, esp_http_client_config_t *tmp_config);
#ifdef __cplusplus
}
#endif

#endif // __HTTP_H__
