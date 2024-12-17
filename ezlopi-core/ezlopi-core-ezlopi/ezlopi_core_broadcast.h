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
 * @file    ezlopi_core_broadcast.h
 * @brief   Function to perform broadcast operations
 * @author  xx
 * @version 0.1
 * @date    12th DEC 2024
*/

#ifndef __EZLOPI_CORE_EZLOPI_BROADCAST_H__
#define __EZLOPI_CORE_EZLOPI_BROADCAST_H__

/*******************************************************************************
*                          Include Files
*******************************************************************************/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
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
    typedef ezlopi_error_t(*f_broadcast_method_t)(char *data);

    typedef struct l_broadcast_method
    {
        char method_name[32];
        uint32_t fail_retry;
        f_broadcast_method_t func;
        struct l_broadcast_method *next;

    } l_broadcast_method_t;

    /*******************************************************************************
    *                          Extern Data Declarations
    *******************************************************************************/

    /*******************************************************************************
    *                          Extern Function Prototypes
    *******************************************************************************/
    // int EZPI_core_broadcast_log_cjson(cJSON *cj_log_data);
    /**
    * @brief Funtion to broadcast message from cjson
    *
    * @param cj_data Pointer to data to be added into queue
    * @return ezlopi_error_t
    */
    ezlopi_error_t EZPI_core_broadcast_cjson(cJSON *cj_data);
    /**
     * @brief Function to add broadcast message to queue
     *
     * @param cj_data Pointer to data to be added into queue
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_broadcast_add_to_queue(cJSON *cj_data);
    /**
     * @brief Funtion to remove broadcast method from ll
     *
     * @param broadcast_method Target broadcast-method
     */
    void EZPI_core_broadcast_remove_method(f_broadcast_method_t broadcast_method);
    /**
     * @brief Function to set broadcast method into queue
     *
     * @param func Target method to add into queue
     */
    void EZPI_core_broadcast_methods_set_queue(ezlopi_error_t(*func)(cJSON *));
    /**
     * @brief Function to add broadcast method
     *
     * @param broadcast_method broadcast method to add
     * @param method_name New Method name
     * @param retries No of max retires to add into queue
     * @return l_broadcast_method_t*
     */
    l_broadcast_method_t *EZPI_core_broadcast_method_add(f_broadcast_method_t broadcast_method, char *method_name, uint32_t retries);


#ifdef __cplusplus
}
#endif

#endif // __EZLOPI_CORE_EZLOPI_BROADCAST_H__

/*******************************************************************************
*                          End of File
*******************************************************************************/