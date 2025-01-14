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
 * @file    ezlopi_service_loop.h
 * @brief   Contains function declarations related to loop implementation
 * @author  ezlopi_team_np
 * @version 1.0
 * @date    June 20, 2024
 */
#ifndef __EZLOPI_SERVICE_TIMER_H__
#define __EZLOPI_SERVICE_TIMER_H__
/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
/*******************************************************************************
 *                          C++ Declaration Wrapper
 *******************************************************************************/
#ifdef __cpluscplus
extern "C"
{
#endif
    /*******************************************************************************
     *                          Type & Macro Declarations
     *******************************************************************************/
    /**
     * @brief Function pointer that a function has to be of to be registered as a loop
     *
     */
    typedef void (*f_loop_t)(void *arg);
    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/

    /**
     * @brief Function to initialize loop service
     *
     */
    void EZPI_service_loop_init(void);
    /**
     * @brief Function to remove function loop from loop stack
     *
     * @param loop Function to remove from the loop stack
     */
    void EZPI_service_loop_remove(f_loop_t loop);
    /**
     * @brief Function to add function loop to loop stack
     *
     * @param[in] name Name for the loop function
     * @param[in] loop Function to add to the loop stack
     * @param[in] period_ms Time in ms for loop to be called
     * @param[in] arg Argument to pass to the function loop
     */
    void EZPI_service_loop_add(const char *name, f_loop_t loop, uint32_t period_ms, void *arg);

#ifdef __cpluscplus
}
#endif

#endif // __EZLOPI_SERVICE_TIMER_H__

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
