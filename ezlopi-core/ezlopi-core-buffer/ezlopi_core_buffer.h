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
 * @file    ezlopi_core_buffer.h
 * @brief   perform some function on system-buffer for messages
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 * @version 0.1
 * @date    12th DEC 2024
 */
#ifndef __EZPI_core_buffer_H__
#define __EZPI_core_buffer_H__

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
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
    typedef enum e_buffer_state
    {
        EZ_BUFFER_STATE_NOT_INITIATED = -1,
        EZ_BUFFER_STATE_AVAILABLE = 0,
        EZ_BUFFER_STATE_BUSY = 1,

    } e_buffer_state_t;

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/
    /**
     * @brief Funtion return enum of system-buffer-status
     *
     * @return e_buffer_state_t
     */
    e_buffer_state_t EZPI_core_buffer_status(void);
    /**
     * @brief Funtion to deinit system-buffer
     *
     */
    void EZPI_core_buffer_deinit(void);
    /**
     * @brief Funtion to initialize system-buffer
     *
     * @param len Length of message buffer
     */
    void EZPI_core_buffer_init(uint32_t len);
    /**
     * @brief Function to release the lock of the buffer
     *
     * @param who Pointer to trigger-line and function-name
     */
    void EZPI_core_buffer_release(const char *who);
    /**
     * @brief Funtion to acquire message buffer
     *
     * @param who Pointer to trigger-line and function-name
     * @param len Length of the buffer to utilize
     * @param wait_to_acquired_ms Time to acquire (ms)
     * @return char*
     */
    char *EZPI_core_buffer_acquire(const char *who, uint32_t *len, uint32_t wait_to_acquired_ms);

#ifdef __cplusplus
}
#endif

#endif // __EZPI_core_buffer_H__
/*******************************************************************************
 *                          End of File
 *******************************************************************************/
