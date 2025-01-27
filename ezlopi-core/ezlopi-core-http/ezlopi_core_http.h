/* ===========================================================================
** Copyright (C) 2025 Ezlo Innovation Inc
**
** Under EZLO AVAILABLE SOURCE LICENSE (EASL) AGREEMENT
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/
/**
 * @file    ezlopi_core_http.h
 * @brief   Function to perform operation on http
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 * @version 0.1
 * @date    November 2nd, 2022 5:30 PM
 */

#ifndef _EZLOPI_CORE_HTTP_H_
#define _EZLOPI_CORE_HTTP_H_

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "cjext.h"

#include "ezlopi_core_errors.h"

/*******************************************************************************
 *                          C++ Declaration Wrapper
 *******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

    /*******************************************************************************
     *                          Type & Macro Declarations
     *******************************************************************************/

    typedef struct s_rx_chunk
    {
        int len;
        char *ptr;
        struct s_rx_chunk *next;
    } s_rx_chunk_t;

    typedef struct
    {
        int status;
        int content_length;
        int rx_len;
        s_rx_chunk_t *rx_chunks;
    } s_rx_data_t;

    typedef struct ezlopi_http_data
    {
        char *response;
        int response_len;
        HttpStatus_Code status_code;

    } s_ezlopi_http_data_t;

    typedef struct s_ezlopi_core_http_mbedtls
    {
        esp_http_client_method_t method; // default :- GET_METHOD
        bool skip_cert_common_name_check;
        int web_port;
        char *url;         // ptr => complete_url [.eg. https://www.google.com/json?username=qqqq&password=zzzz ]
        char *web_server;  // ptr => web_sever_name [.eg. www.google.com]
        char *target_page; // ptr => target_page [.eg. ...src/page... ]
        char *header;      // ptr => headers [.eg. {"Accept" : "*/*", ....}]
        char *content;
        char *username;
        char *password;
        char *response;
        uint8_t username_maxlen; // max @ 256 bytes
        uint8_t password_maxlen;
        uint16_t url_maxlen; // max @ 65536 = 60Kb
        uint16_t web_server_maxlen;
        uint16_t targe_page_maxlen;
        uint16_t header_maxlen;
        uint16_t content_maxlen;
        uint16_t response_maxlen;
        TaskHandle_t mbedtls_task_handle;
    } s_ezlopi_core_http_mbedtls_t;

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/

    /**
     * @brief Function to return remaining space in *dest_buffer.
     */
    int EZPI_core_http_calc_empty_bufsize(char *dest_buff, int dest_size, int reqd_size);

    /**
     * @brief This function :- mallocs fresh memory_block to '__dest_ptr' , copies content of 'src_ptr' to '__dest_ptr' and returns malloced_size.
     *
     * @param __dest_ptr [ Address of ptr which will point to (char*) block of memory. ]
     * @param src_ptr    [ Address of the string literal you want to store/append. ]
     *
     * @return (int) [size_of_malloced block]
     */
    int EZPI_core_http_mem_malloc(char **__dest_ptr, const char *src_ptr);

    /**
     * @brief This function creates new_memory_block (size == 'reqSize') & Rellocates Original memory-block (*Buf) to new_memory_block_address
     *
     * @param Buf       [ Address of ptr which will point to (char*) original block of memory. ]
     * @param reqSize   [ new-size to be allocated. ]
     *
     * @return (int) [EZPI_FAILED ==> Fail ; EZPI_SUCCESS ==> Success]
     */
    ezlopi_error_t EZPI_core_http_dyna_relloc(char **Buf, int reqSize);

    /**
     * @brief This Task , generates a http request, by combining information contained in '*config'.
     *
     * @param config : config_struct [ complete_url + webserver-name + web_port + headers + content + username + password + response ]
     */
    void EZPI_core_http_mbedtls_req(s_ezlopi_core_http_mbedtls_t *tmp_http_data);

    /**
     * @brief Funciton to trigger a 'GET' http request
     *
     * @param cloud_url Target URL
     * @param private_key Private-key
     * @param shared_key Shared-key
     * @param ca_certificate Certs
     * @return s_ezlopi_http_data_t*
     */
    s_ezlopi_http_data_t *EZPI_core_http_get_request(const char *cloud_url, const char *private_key, const char *shared_key, const char *ca_certificate);
    /**
     * @brief Function to trigger a 'POST' http request
     *
     * @param cloud_url Target URL
     * @param location Location data in url
     * @param headers Headers for the request
     * @param private_key Private-key
     * @param shared_key Shared-key
     * @param ca_certificate Certs
     * @return s_ezlopi_http_data_t*
     */
    s_ezlopi_http_data_t *EZPI_core_http_post_request(const char *cloud_url, const char *location, cJSON *headers, const char *private_key, const char *shared_key, const char *ca_certificate);

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_CORE_HTTP_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
