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
 * @file    ezlopi_service_webprov.h
 * @brief   Contains function declarations related to web provisioning
 * @author  ezlopi_team_np
 * @version 1.0
 * @date    January 22, 2024
 */

#ifndef __EZLOPI_SERVICE_WEBPROV_H__
#define __EZLOPI_SERVICE_WEBPROV_H__

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "../../build/config/sdkconfig.h"
#include <string.h>
#include "cjext.h"
#include "EZLOPI_USER_CONFIG.h"

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

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/
#if defined(CONFIG_EZPI_WEBSOCKET_CLIENT)
    /**
     * @brief Function to initialize web provisioning
     * @details Following operatons are perfromed
     *  - Start provisioning check task
     *  - Start fetch WSS endpoint task
     *
     */
    void EZPI_service_web_provisioning_init(void);
    /**
     * @brief Function to deinitialize web provisioning
     *
     */
    void EZPI_service_web_provisioning_deinit(void);
    /**
     * @brief Function to get the current message count
     *
     * @return uint32_t
     * @retval currnet message count
     */
    uint32_t EZPI_service_web_provisioning_get_message_count(void);
    /**
     * @brief Function to return webprov status
     *
     * @return true
     * @return false
     */
    bool EZPI_service_webprov_is_connected(void);

#endif // CONFIG_EZPI_WEBSOCKET_CLIENT

#ifdef __cplusplus
}
#endif

#endif // __EZLOPI_SERVICE_WEBPROV_H__

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
