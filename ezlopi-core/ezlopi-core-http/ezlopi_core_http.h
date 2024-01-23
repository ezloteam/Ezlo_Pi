#ifndef _EZLOPI_CORE_HTTP_H_
#define _EZLOPI_CORE_HTTP_H_

#include <stdio.h>
#include <string.h>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_http_client.h"
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
        int status_code;

    } s_ezlopi_http_data_t;

    s_ezlopi_http_data_t *ezlopi_http_get_request(char *cloud_url, char *private_key, char *shared_key, char *ca_certificate);
    s_ezlopi_http_data_t *ezlopi_http_post_request(char *cloud_url, char *location, cJSON *headers, char *private_key, char *shared_key, char *ca_certificate);

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_CORE_HTTP_H_
