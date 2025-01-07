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
 * @file    ezlopi_core_api_methods.h
 * @brief   Functions perform operation for API-methods
 * @author  xx
 * @version 0.1
 * @date    12th DEC 2024
 */

#ifndef __EZPI_core_ezlopi_methods_H__
#define __EZPI_core_ezlopi_methods_H__

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <stdio.h>
#include <ctype.h>
#include "cjext.h"
#include <string.h>
#include <stdint.h>

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
    typedef void (*f_method_func_t)(cJSON *cj_request, cJSON *cj_response);
    typedef struct s_method_list_v2
    {
        char *method_name;
        f_method_func_t method;
        f_method_func_t updater;
    } s_method_list_v2_t;
    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/
    /**
     * @brief This function initializes the registration of all core-action methods
     */
    void EZPI_core_ezlopi_methods_registration_init(void);
    /**
     * @brief
     *
     * @param method
     * @return true
     * @return false
     */
    bool EZPI_core_ezlopi_methods_check_method_register(f_method_func_t method);
    /**
     * @brief This funciton returns corresponding INDEX of given action-method
     *
     * @param cj_method CJSON containing target-method
     * @return uint32_t
     */
    uint32_t EZPI_core_ezlopi_methods_search_in_list(cJSON *cj_method);
    /**
     * @brief This function return method-name of corresponding action-method
     *
     * @param method_id Target method_idx
     * @return char*
     */
    char *EZPI_core_ezlopi_methods_get_name_by_id(uint32_t method_id);
    /**
     * @brief This function returns ptr to desired ezlopi-method
     *
     * @param method_id Target method_idx
     * @return f_method_func_t
     */
    f_method_func_t EZPI_core_ezlopi_methods_get_by_id(uint32_t method_id);
    /**
     * @brief This function returns ptr to udpdated function of target-ezlopi-method
     *
     * @param method_id Target method_idx
     * @return f_method_func_t
     */
    f_method_func_t EZPI_core_ezlopi_methods_get_updater_by_id(uint32_t method_id);
    /**
     * @brief This function triggers a soft-reset of the system
     *
     * @param cj_request Pointer to request obj
     * @param cj_response Pointer to response obj
     */
    void EZPI_core_ezlopi_methods_reboot(cJSON *cj_request, cJSON *cj_response);
    /**
     * @brief This function prepares a 'method-not-found' reply for any invalid API-triggers
     *
     * @param cj_request Pointer to request obj
     * @param cj_response Pointer to response obj
     */
    void EZPI_core_ezlopi_methods_rpc_method_notfound(cJSON *cj_request, cJSON *cj_response);

#ifdef __cplusplus
}
#endif

#endif // __EZPI_core_ezlopi_methods_H__

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
