#ifndef _EZLOPI_CORE_HTTP_H_
#define _EZLOPI_CORE_HTTP_H_

#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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
        char* ptr;
        int len;
        int total_len;
        struct s_rx_data* next;

    } s_rx_data_t;
    typedef struct ezlopi_http_data
    {
        char* response;
        HttpStatus_Code status_code;

    } s_ezlopi_http_data_t;

    typedef struct s_ezlopi_core_http_mbedtls
    {
        esp_http_client_method_t method; // default :- GET_METHOD
        bool skip_cert_common_name_check;
        int web_port;
        char* url;        // ptr => complete_url [.eg. https://www.google.com/json?username=qqqq&password=zzzz ]
        char* web_server; // ptr => web_sever_name [.eg. www.google.com]
        char* header;     // ptr => headers [.eg. {"Accept" : "*/*", ....}]
        char* content;
        char* username;
        char* password;
        char* response;
        uint8_t username_maxlen; // max @ 256 bytes
        uint8_t password_maxlen;
        uint16_t url_maxlen; // max @ 65536 = 60Kb
        uint16_t web_server_maxlen;
        uint16_t header_maxlen;
        uint16_t content_maxlen;
        TaskHandle_t mbedtls_task_handle;
    } s_ezlopi_core_http_mbedtls_t;

    /**
     * @brief Function to return remaining space in *dest_buffer.
     */
    int ezlopi_core_http_calc_empty_bufsize(char* dest_buff, int dest_size, int reqd_size);

    /**
     * @brief This function :- mallocs fresh memory_block to '__dest_ptr' , copies content of 'src_ptr' to '__dest_ptr' and returns malloced_size.
     *
     * @param __dest_ptr [ Address of ptr which will point to (char*) block of memory. ]
     * @param src_ptr    [ Address of the string literal you want to store/append. ]
     *
     * @return (int) [size_of_malloced block]
     */
    int ezlopi_core_http_mem_malloc(char** __dest_ptr, const char* src_ptr);

    /**
     * @brief This function creates new_memory_block (size == 'reqSize') & Rellocates Original memory-block (*Buf) to new_memory_block_address
     *
     * @param Buf       [ Address of ptr which will point to (char*) original block of memory. ]
     * @param reqSize   [ new-size to be allocated. ]
     *
     * @return (int) [0 ==> Fail ; 1 ==> Success]
     */
    int ezlopi_core_http_dyna_relloc(char** Buf, int reqSize);

    /**
     * @brief This Task , generates a http request, by combining information contained in '*config'.
     *
     * @param config : config_struct [ complete_url + webserver-name + web_port + headers + content + username + password + response ]
     */
    void ezlopi_core_http_mbedtls_req(s_ezlopi_core_http_mbedtls_t* tmp_http_data);

    s_ezlopi_http_data_t* ezlopi_http_get_request(const char* cloud_url, const char* private_key, const char* shared_key, const char* ca_certificate);
    s_ezlopi_http_data_t* ezlopi_http_post_request(const char* cloud_url, const char* location, cJSON* headers, const char* private_key, const char* shared_key, const char* ca_certificate);

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_CORE_HTTP_H_
