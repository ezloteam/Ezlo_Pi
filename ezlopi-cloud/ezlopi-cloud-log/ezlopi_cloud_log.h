
/**
 * @file    ezlopi_cloud_log.c
 * @brief
 * @author
 * @version
 * @date
 */
/* ===========================================================================
** Copyright (C) 2022 Ezlo Innovation Inc
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

#ifndef _EZLOPI_CLOUD_LOG_H_
#define _EZLOPI_CLOUD_LOG_H_

#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_UTIL_TRACE_EN

#include "cjext.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Function to respond to hub.log.set method
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_hub_cloud_log_set(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function to respond to hub.log.local.set method
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void EZPI_hub_serial_log_set(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief Function that sends serial log set updated response to the cloud
     *
     * @param cj_request incoming JSON request
     * @param cj_response Outgoing JSON response
     */
    void ezlopi_hub_serial_log_set_updater(cJSON *cj_request, cJSON *cj_response);

#endif // CONFIG_EZPI_UTIL_TRACE_EN
#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_CLOUD_LOG_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
