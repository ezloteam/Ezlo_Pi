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
 * @file    ezlopi_core_heap.h
 * @brief   Function to operate on heap
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 *          Lomas Subedi
 *          Riken Maharjan
 *          Nabin Dangi
 * @version 0.1
 * @date    12th DEC 2024
 */

#ifndef __EZLOPI_UTIL_HEAP_TRACE_H__
#define __EZLOPI_UTIL_HEAP_TRACE_H__

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "EZLOPI_USER_CONFIG.h"

#ifdef CONFIG_EZPI_HEAP_ENABLE

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
     * @brief Function to flush heap
     *
     */
    void EZPI_core_util_heap_flush(void);
    /**
     * @brief Function to trace the heap
     *
     * @param print_freed Flag to trigger free
     */
    void EZPI_core_util_heap_trace(bool print_freed);
    /**
     * @brief  Function to free heap
     *
     * @param who pointer to function triggering
     * @param ptr Pointer to heap memory
     * @param __file_name file name
     * @param line_number Line number
     */
    void EZPI_core_util_heap_free(const char *who, void *ptr, const char *__file_name, uint32_t line_number);

    /**
     * @brief Function to allocate heap mem
     *
     * @param who pointer to func_name and line num
     * @param size size to malloc
     * @param file_name File name
     * @param line_no Line number
     * @return void*
     */
    void *EZPI_core_util_heap_malloc(const char *who, size_t size, const char *file_name, uint32_t line_no);
    /**
     * @brief Function to allocate heap mem
     *
     * @param who  pointer to func_name and line num
     * @param count  Counter to mem slots
     * @param size  size to malloc
     * @param file_name  filename
     * @param line_no  line number
     * @return void*
     */
    void *EZPI_core_util_heap_calloc(const char *who, size_t count, size_t size, const char *file_name, uint32_t line_no);
    /**
     * @brief Function to reallocate heap mem
     *
     * @param who pointer to func_name and line num
     * @param ptr Pointer to start address
     * @param new_size new size to
     * @param file_name Filename
     * @param line_no Line number
     * @return void*
     */
    void *EZPI_core_util_heap_realloc(const char *who, void *ptr, size_t new_size, const char *file_name, uint32_t line_no);

#ifdef __cplusplus
}
#endif

#endif // CONFIG_EZPI_HEAP_ENABLE
#endif // __EZLOPI_UTIL_HEAP_TRACE_H__

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
