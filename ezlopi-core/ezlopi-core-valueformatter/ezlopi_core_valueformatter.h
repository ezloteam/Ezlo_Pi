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
 * @file    ezlopi_core_valueformatter.h
 * @brief   perform some function on valueformatting operation
 * @author  xx
 * @version 0.1
 * @date    12th DEC 2024
 */

#ifndef _EZLOPI_VALUEFORMATER_H_
#define _EZLOPI_VALUEFORMATER_H_

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <stdbool.h>
#include "cjext.h"

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

    /**
     * @brief Function to convert 'bool' to cjson obj
     *
     * @param cj_root Destinaiton obj to store the converted values
     * @param value New value 'bool'
     * @param scale Scale of the value
     */
    void EZPI_core_valueformatter_bool_to_cjson(cJSON *cj_root, bool value, const char *scale);
    /**
     * @brief Function to convert 'float' to cjson obj
     *
     * @param cj_root Destinaiton obj to store the converted values
     * @param value New value 'float'
     * @param scale Scale of the value
     */
    void EZPI_core_valueformatter_float_to_cjson(cJSON *cj_root, float value, const char *scale);
    /**
     * @brief Function to convert 'double' to cjson obj
     *
     * @param cj_root Destinaiton obj to store the converted values
     * @param value New value 'double'
     * @param scale Scale of the value
     */
    void EZPI_core_valueformatter_double_to_cjson(cJSON *cj_root, double value, const char *scale);
    /**
     * @brief Function to convert 'int32_t' to cjson obj
     *
     * @param cj_root Destinaiton obj to store the converted values
     * @param value New value 'int32_t'
     * @param scale Scale of the value
     */
    void EZPI_core_valueformatter_int32_to_cjson(cJSON *cj_root, int value, const char *scale);
    /**
     * @brief Function to convert 'uint32_t' to cjson obj
     *
     * @param cj_root Destinaiton obj to store the converted values
     * @param value New value 'uint32_t'
     * @param scale Scale of the value
     */
    void EZPI_core_valueformatter_uint32_to_cjson(cJSON *cj_root, uint32_t value, const char *scale);

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_VALUEFORMATER_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
