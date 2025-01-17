
/**
 * @file    ezlopi_service_ws_server_clients.h
 * @brief
 * @authors Krishna Kumar Sah (work.krishnasah@gmail.com)
 *          Lomas Subedi
 *          Riken Maharjan
 *          Nabin Dangi
 * @version
 * @date
 */
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

#ifndef __EZLOPI_SERVICE_WS_SERVER_CLIENTS_H__
#define __EZLOPI_SERVICE_WS_SERVER_CLIENTS_H__

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Linked list for websoket clinet connection handler
     *
     */
    typedef struct l_ws_server_client_conn
    {
        void *http_handle;   /**< HTTP connection handle */
        int http_descriptor; /**< HTTP descriptor */
        uint32_t fail_count; /**< Connection failed count */

        struct l_ws_server_client_conn *next; /**< Points to the next clinet handle */
    } l_ws_server_client_conn_t;

    /**
     * @brief Function to remove clinet connected to the websoket server using client connection handle
     *
     * @param[in] http_handle Pointer to the connection handle belonging to the client to remove
     * @return int
     */
    int EZPI_service_ws_server_clients_remove_by_handle(void *http_handle);
    /**
     * @brief Function to get clinet list head
     *
     * @return l_ws_server_client_conn_t*
     * @retval Head of the client handles list
     */
    l_ws_server_client_conn_t *EZPI_service_ws_server_clients_get_head(void);
    /**
     * @brief Function to pop client using client connection handle from the client list
     *
     * @param[in] http_handle Pointer to the connection handle belonging to the client to pop
     * @return l_ws_server_client_conn_t*
     * @retval Pointer to the client poped
     */
    l_ws_server_client_conn_t *EZPI_service_ws_server_clients_pop(void *http_handle);
    /**
     * @brief Function to get websocket client by handle
     *
     * @param[in] http_handle Pointer to the connection handle belonging to the client to get
     * @return l_ws_server_client_conn_t*
     * @retval Pointer to the required client handle
     */
    l_ws_server_client_conn_t *EZPI_service_ws_server_clients_get_by_handle(void *http_handle);
    /**
     * @brief Function to add new client to the websocket client handle list
     *
     * @param[in] http_handle Pointer to the client http handle
     * @param[in] http_desc HTTP descriptro for the clinent
     * @return l_ws_server_client_conn_t*
     * @retval Pointer to the cliend handle added to the list
     */
    l_ws_server_client_conn_t *EZPI_service_ws_server_clients_add(void *http_handle, int http_desc);

#ifdef __cplusplus
}
#endif

#endif // __EZLOPI_SERVICE_WS_SERVER_CLIENTS_H__

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
