#ifndef _EZLOPI_CORE_HTTP_H_
#define _EZLOPI_CORE_HTTP_H_

#include <stdio.h>
#include <string.h>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_crt_bundle.h"
#include "cJSON.h"

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
        HttpStatus_Code status_code;

    } s_ezlopi_http_data_t;

    typedef struct s_ezlopi_scenes_then_methods_send_http
    {
        bool skip_cert_common_name_check;
        int web_port;
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

    void ezlopi_core_http_scenes_then_parse_url(s_ezlopi_scenes_then_methods_send_http_t *tmp_http_data, const char *field_value_string);
    void ezlopi_core_http_scenes_then_parse_content(s_ezlopi_scenes_then_methods_send_http_t *tmp_http_data, const char *field_value_string);
    void ezlopi_core_http_scenes_then_parse_content_type(s_ezlopi_scenes_then_methods_send_http_t *tmp_http_data, const char *field_value_string);
    void ezlopi_core_http_scenes_then_parse_headers(s_ezlopi_scenes_then_methods_send_http_t *tmp_http_data, cJSON *cj_value);
    void ezlopi_core_http_scenes_then_parse_skipsecurity(s_ezlopi_scenes_then_methods_send_http_t *tmp_http_data, bool value_bool);
    void ezlopi_core_http_scenes_then_parse_username_password(s_ezlopi_scenes_then_methods_send_http_t *tmp_http_data, cJSON *cj_value);
    void ezlopi_core_http_scenes_then_clear_struct_ptr_mem(s_ezlopi_scenes_then_methods_send_http_t *config);
    void ezlopi_core_http_scenes_then_sendhttp_request(s_ezlopi_scenes_then_methods_send_http_t *config, char **dest_buf_container);

    s_ezlopi_http_data_t *ezlopi_http_get_request(char *cloud_url, char *private_key, char *shared_key, char *ca_certificate);
    s_ezlopi_http_data_t *ezlopi_http_post_request(const char *cloud_url, const char *location, cJSON *headers, const char *private_key, const char *shared_key, const char *ca_certificate);

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_CORE_HTTP_H_
