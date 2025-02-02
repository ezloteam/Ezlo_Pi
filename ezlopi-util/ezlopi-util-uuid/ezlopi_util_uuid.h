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
 * @file    main.c
 * @brief   perform some function on data
 * @author  
 * @version 0.1
 * @date    1st January 2024
 */

#ifndef __EZLOPI_UTIL_UUID_H__
#define __EZLOPI_UTIL_UUID_H__

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

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
#define RAND_LENGTH 4

    typedef unsigned char ezlopi_uuid_t[16];

#define uuid_generate(out) ezlopi_util_uuid_generate_random(out)

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/
    /**
     * @brief Function to generate random uuid
     *
     * @param out
     */
    void ezlopi_util_uuid_generate_random(ezlopi_uuid_t out);
    /**
     * @brief Function to unparse uuid
     *
     * @param uuid Target uuid
     * @param out pointer to destination
     */
    void ezlopi_util_uuid_unparse(const ezlopi_uuid_t uuid, char *out);
    /**
     * @brief Function to copy uuid
     *
     * @param dst
     * @param src
     */
    void ezlopi_util_uuid_copy(ezlopi_uuid_t dst, const ezlopi_uuid_t src);
    /**
     * @brief Function to parse uuid
     *
     * @param in
     * @param uuid
     */
    void ezlopi_util_uuid_parse(const char *in, ezlopi_uuid_t uuid);

#ifdef __cplusplus
}
#endif

#endif // __EZLOPI_UTIL_UUID_H__

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
