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
* @file    ezlopi_core_net.c
* @brief   perform some function on core-net
* @author  xx
* @version 0.1
* @date    12th DEC 2024
*/

/*******************************************************************************
*                          Include Files
*******************************************************************************/

#include "ezlopi_core_net.h"
/*******************************************************************************
*                          Extern Data Declarations
*******************************************************************************/

/*******************************************************************************
*                          Extern Function Declarations
*******************************************************************************/

/*******************************************************************************
*                          Type & Macro Definitions
*******************************************************************************/

/*******************************************************************************
*                          Static Function Prototypes
*******************************************************************************/

/*******************************************************************************
*                          Static Data Definitions
*******************************************************************************/

static s_ezlopi_net_status_t net_stat;
/*******************************************************************************
*                          Extern Data Definitions
*******************************************************************************/

/*******************************************************************************
*                          Extern Function Definitions
*******************************************************************************/
#ifdef CONFIG_EZPI_CORE_ENABLE_ETH

void EZPI_net_init(void)
{
    memset(&net_stat, 0, sizeof(s_ezlopi_net_status_t));
    EZPI_ethernet_init();
}

#endif // CONFIG_EZPI_CORE_ENABLE_ETH

s_ezlopi_net_status_t *EZPI_core_net_get_net_status(void)
{

    net_stat.wifi_status = EZPI_core_wifi_status();
#ifdef CONFIG_EZPI_ENABLE_PING
    net_stat.internet_status = EZPI_core_ping_get_internet_status();
#else  // CONFIG_EZPI_ENABLE_PING
    net_stat.internet_status = EZLOPI_PING_STATUS_UNKNOWN;
#endif // CONFIG_EZPI_ENABLE_PING

    // net_stat.nma_cloud_connection_status = EZPI_core_websocket_client_is_connected(client);

    return &net_stat;
}

/*******************************************************************************
*                         Static Function Definitions
*******************************************************************************/


/*******************************************************************************
*                          End of File
*******************************************************************************/



