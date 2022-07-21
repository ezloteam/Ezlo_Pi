#ifndef __HTTP_H__
#define __HTTP_H__

#include "stdio.h"
#include <string>
#include <iostream>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_tls.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "protocol_examples_common.h"

using namespace std;

typedef struct s_rx_data
{
    char *ptr;
    int len;
    int total_len;
    struct s_rx_data *next;

} s_rx_data_t;

class http
{
private:
    static esp_err_t http_event_handler(esp_http_client_event_t *evt);
    void free_rx_data(s_rx_data_t *rx_data);

public:
    string http_get_request(char *cloud_url, char *private_key, char *shared_key, char *ca_certificate);
};

#endif // __HTTP_H__