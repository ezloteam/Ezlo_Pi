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
 * @file    main.c
 * @brief   perform some function on data
 * @author  John Doe
 * @version 0.1
 * @date    1st January 2024
 */

#ifndef __EZLOPI_CORE_EZLOPI_METHODS_H__
#define __EZLOPI_CORE_EZLOPI_METHODS_H__

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
    typedef void (*f_method_func_t)(cJSON* cj_request, cJSON* cj_response);
    typedef struct s_method_list_v2
    {
        char* method_name;
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
     * @brief Global function template example
     * Convention : Use capital letter for initial word on extern function
     * maincomponent : Main component as hal, core, service etc.
     * subcomponent : Sub component as i2c from hal, ble from service etc
     * functiontitle : Title of the function
     * eg : EZPI_hal_i2c_init()
     * @param arg
     *
     */
    void ezlopi_core_ezlopi_methods_registration_init(void);
    bool ezlopi_core_elzlopi_methods_check_method_register(f_method_func_t method);

    uint32_t ezlopi_core_ezlopi_methods_search_in_list(cJSON* cj_method);

    char* ezlopi_core_ezlopi_methods_get_name_by_id(uint32_t method_id);
    f_method_func_t ezlopi_core_ezlopi_methods_get_by_id(uint32_t method_id);
    f_method_func_t ezlopi_core_ezlopi_methods_get_updater_by_id(uint32_t method_id);

    void ezlopi_core_ezlopi_methods_reboot(cJSON* cj_request, cJSON* cj_response);
    void ezlopi_core_ezlopi_methods_rpc_method_notfound(cJSON* cj_request, cJSON* cj_response);

#ifdef __cplusplus
}
#endif

#endif // __EZLOPI_CORE_EZLOPI_METHODS_H__

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
