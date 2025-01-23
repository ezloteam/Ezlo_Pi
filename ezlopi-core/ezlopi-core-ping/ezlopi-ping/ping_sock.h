/* ===========================================================================
** Copyright (C) 2024 Ezlo Innovation Inc
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
 * @file    ping_sock.h
 * @brief   Function to perfrom operations on ping-socket
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 *          Lomas Subedi
 *          Riken Maharjan
 *          Nabin Dangi
 * @version 0.1
 * @date    12th DEC 2024
*/

#ifndef _PING_SOCK_H_
#define _PING_SOCK_H_

/*******************************************************************************
*                          Include Files
*******************************************************************************/
#pragma once

/*******************************************************************************
*                          C++ Declaration Wrapper
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif


#include <stdint.h>
#include "esp_err.h"
#include "lwip/ip_addr.h"
    /*******************************************************************************
    *                          Type & Macro Declarations
    *******************************************************************************/

    /**
    * @brief Type of "ping" session handle
    *
    */
    typedef void *esp_ping_handle_t;

    /**
    * @brief Type of "ping" callback functions
    *
    */
    typedef struct {
        /**
        * @brief arguments for callback functions
        *
        */
        void *cb_args;

        /**
        * @brief Invoked by internal ping thread when received ICMP echo reply packet
        *
        */
        void (*on_ping_success)(esp_ping_handle_t hdl, void *args);

        /**
        * @brief Invoked by internal ping thread when receive ICMP echo reply packet timeout
        *
        */
        void (*on_ping_timeout)(esp_ping_handle_t hdl, void *args);

        /**
        * @brief Invoked by internal ping thread when a ping session is finished
        *
        */
        void (*on_ping_end)(esp_ping_handle_t hdl, void *args);
    } ezlopi_ping_callbacks_t;

    /**
    * @brief Type of "ping" configuration
    *
    */
    typedef struct {
        uint32_t count;           /*!< A "ping" session contains count procedures */
        uint32_t interval_ms;     /*!< Milliseconds between each ping procedure */
        uint32_t timeout_ms;      /*!< Timeout value (in milliseconds) of each ping procedure */
        uint32_t data_size;       /*!< Size of the data next to ICMP packet header */
        int tos;                  /*!< Type of Service, a field specified in the IP header */
        int ttl;                  /*!< Time to Live,a field specified in the IP header */
        ip_addr_t target_addr;    /*!< Target IP address, either IPv4 or IPv6 */
        uint32_t task_stack_size; /*!< Stack size of internal ping task */
        uint32_t task_prio;       /*!< Priority of internal ping task */
        uint32_t interface;       /*!< Netif index, interface=0 means NETIF_NO_INDEX*/
    } ezlopi_ping_config_t;

    /**
     * @brief Default ping configuration
     *
     */
#define ESP_PING_DEFAULT_CONFIG()        \
    {                                    \
        .count = 5,                      \
        .interval_ms = 1000,             \
        .timeout_ms = 1000,              \
        .data_size = 64,                 \
        .tos = 0,                        \
        .ttl = IP_DEFAULT_TTL,           \
        .target_addr = *(IP_ANY_TYPE),   \
        .task_stack_size = 2048 + TASK_EXTRA_STACK_SIZE,         \
        .task_prio = 2,                  \
        .interface = 0,\
    }

#define ESP_PING_COUNT_INFINITE (0) /*!< Set ping count to zero will ping target infinitely */

     /**
     * @brief Profile of ping session
     *
     */
    typedef enum {
        ESP_PING_PROF_SEQNO,   /*!< Sequence number of a ping procedure */
        ESP_PING_PROF_TOS,     /*!< Type of service of a ping procedure */
        ESP_PING_PROF_TTL,     /*!< Time to live of a ping procedure */
        ESP_PING_PROF_REQUEST, /*!< Number of request packets sent out */
        ESP_PING_PROF_REPLY,   /*!< Number of reply packets received */
        ESP_PING_PROF_IPADDR,  /*!< IP address of replied target */
        ESP_PING_PROF_SIZE,    /*!< Size of received packet */
        ESP_PING_PROF_TIMEGAP, /*!< Elapsed time between request and reply packet */
        ESP_PING_PROF_DURATION /*!< Elapsed time of the whole ping session */
    } esp_ping_profile_t;

    /*******************************************************************************
    *                          Extern Data Declarations
    *******************************************************************************/

    /*******************************************************************************
    *                          Extern Function Prototypes
    *******************************************************************************/

    /**
     * @brief Create a ping session
     *
     * @param config ping configuration
     * @param cbs a bunch of callback functions invoked by internal ping task
     * @param hdl_out handle of ping session
     * @return
     *      - ESP_ERR_INVALID_ARG: invalid parameters (e.g. configuration is null, etc)
     *      - ESP_ERR_NO_MEM: out of memory
     *      - ESP_FAIL: other internal error (e.g. socket error)
     *      - ESP_OK: create ping session successfully, user can take the ping handle to do follow-on jobs
     */
    esp_err_t EZPI_ping_new_session(const ezlopi_ping_config_t *config, const ezlopi_ping_callbacks_t *cbs, esp_ping_handle_t *hdl_out);

    /**
     * @brief Start the ping session
     *
     * @param hdl handle of ping session
     * @return
     *      - ESP_ERR_INVALID_ARG: invalid parameters (e.g. ping handle is null, etc)
     *      - ESP_OK: start ping session successfully
     */
    esp_err_t EZPI_ping_start_by_handle(esp_ping_handle_t hdl);

    /**
     * @brief Get runtime profile of ping session
     *
     * @param hdl handle of ping session
     * @param profile type of profile
     * @param data profile data
     * @param size profile data size
     * @return
     *      - ESP_ERR_INVALID_ARG: invalid parameters (e.g. ping handle is null, etc)
     *      - ESP_ERR_INVALID_SIZE: the actual profile data size doesn't match the "size" parameter
     *      - ESP_OK: get profile successfully
     */
    esp_err_t EZPI_ping_get_profile(esp_ping_handle_t hdl, esp_ping_profile_t profile, void *data, uint32_t size);

    /**
    * @brief Delete a ping session
    *
    * @param hdl handle of ping session
    * @return
    *      - ESP_ERR_INVALID_ARG: invalid parameters (e.g. ping handle is null, etc)
    *      - ESP_OK: delete ping session successfully
    */
    esp_err_t EZPI_ping_delete_session(esp_ping_handle_t hdl);

    /**
     * @brief Stop the ping session
     *
     * @param hdl handle of ping session
     * @return
     *      - ESP_ERR_INVALID_ARG: invalid parameters (e.g. ping handle is null, etc)
     *      - ESP_OK: stop ping session successfully
     */
    esp_err_t EZPI_ping_stop_by_handle(esp_ping_handle_t hdl);


#ifdef __cplusplus
}
#endif

#endif // _PING_SOCK_H_

/*******************************************************************************
*                          End of File
*******************************************************************************/
