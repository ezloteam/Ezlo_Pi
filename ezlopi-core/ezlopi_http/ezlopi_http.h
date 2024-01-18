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
        char web_port[5]; // use number instead of char-array
        char url[196];
        char web_server[96];
        char header[256];
        char content[384]; // content size should be dynamic
        char username[32];
        char password[32];
        bool skip_cert_common_name_check;
        esp_http_client_method_t method;
    } s_ezlopi_scenes_then_methods_send_http_t;
    int ezlopi_http_limit_size_check(char *dest_buff, int dest_size, int reqd_size);
    void ezlopi_http_scenes_sendhttp_request_api(s_ezlopi_scenes_then_methods_send_http_t *config, cJSON *tmp_header);

    s_ezlopi_http_data_t *ezlopi_http_get_request(char *cloud_url, cJSON *headers, char *private_key, char *shared_key, char *ca_certificate, esp_http_client_config_t *tmp_config);
    s_ezlopi_http_data_t *ezlopi_http_post_request(char *cloud_url, char *location, char *post_content_data, cJSON *headers, char *private_key, char *shared_key, char *ca_certificate, esp_http_client_config_t *tmp_config);
    s_ezlopi_http_data_t *ezlopi_http_put_request(char *cloud_url, cJSON *headers, char *private_key, char *shared_key, char *ca_certificate, esp_http_client_config_t *tmp_config);
    s_ezlopi_http_data_t *ezlopi_http_delete_request(char *cloud_url, cJSON *headers, char *private_key, char *shared_key, char *ca_certificate, esp_http_client_config_t *tmp_config);
#ifdef __cplusplus
}
#endif

#endif // __HTTP_H__
