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
 * @file    ezlopi_core_mdns.h
 * @brief   Function to operate on mdns service
 * @author  ezlopi_team_np
 * @version 0.1
 * @date    12th DEC 2024
 */

#ifndef __EZLOPI_CORE_MDNS_H_
#define __EZLOPI_CORE_MDNS_H_

#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_SERV_MDNS_EN

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "mdns.h"

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
#define EZPI_MDNS_HOSTNAME_SIZE 100
#define EZPI_MDNS_CONTEX_COUNT 2
#define EZPI_MDNS_SERIAL_SIZE 10

    typedef struct l_ezlopi_mdns_context
    {
        mdns_txt_item_t *mdns_context;
        struct l_ezlopi_mdns_context *next;
    } l_ezlopi_mdns_context_t;

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/
    /**
     * @brief Function to initialize mdns service
     *
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_init_mdns();

#ifdef __cplusplus
}
#endif

#endif // CONFIG_EZPI_SERV_MDNS_EN

#endif // __EZLOPI_CORE_MDNS_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
