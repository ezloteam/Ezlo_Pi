#ifndef __HTTP_H__
#define __HTTP_H__

#include "stdio.h"
#include "string.h"
#include "esp_err.h"
#include "esp_log.h"
#include "cJSON.h"
// #include "esp_tls.h"
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

    s_ezlopi_http_data_t *ezlopi_http_get_request(char *cloud_url, cJSON *headers, char *private_key, char *shared_key, char *ca_certificate, esp_http_client_config_t *tmp_config);
    s_ezlopi_http_data_t *ezlopi_http_post_request(char *cloud_url, char *location, char *post_content_data, cJSON *headers, char *private_key, char *shared_key, char *ca_certificate, esp_http_client_config_t *tmp_config);
    s_ezlopi_http_data_t *ezlopi_http_put_request(char *cloud_url, cJSON *headers, char *private_key, char *shared_key, char *ca_certificate, esp_http_client_config_t *tmp_config);
    s_ezlopi_http_data_t *ezlopi_http_delete_request(char *cloud_url, cJSON *headers, char *private_key, char *shared_key, char *ca_certificate, esp_http_client_config_t *tmp_config);
#ifdef __cplusplus
}
#endif

#endif // __HTTP_H__
