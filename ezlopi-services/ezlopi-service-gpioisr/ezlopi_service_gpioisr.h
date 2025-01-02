
/**
 * @file    ezlopi_service_gpioisr.h
 * @brief
 * @author
 * @version
 * @date
 */
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

#ifndef _EZLOPI_SERVICE_GPIOISR_H_
#define _EZLOPI_SERVICE_GPIOISR_H_

#include "ezlopi_core_devices_list.h"

/*******************************************************************************
 *                          C++ Declaration Wrapper
 *******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Function handles the interrupt registered for corresponding GPIO
     *
     */
    typedef void (*f_interrupt_upcall_t)(void *);

    /**
     * @brief initializes the GPIO ISR for ezlopi
     * @details This function performs following operations
     *  - Initializes GPIO event queue
     *  - Starts task for gpio process
     *
     */
    void EZPI_service_gpioisr_init(void);
    /**
     * @brief Function to register GPIO ISR
     * @details This function performs following operations
     *  - Enables GPIO interrupt for the gpio the provided item is connected to
     *  - Adds GPIO ISR handler
     *
     * @param[in] item  Item that contains pin information
     * @param[in] __upcall  Interuupt upcall to be called on interrupt for the registered pin
     * @param[in] debounce_ms Pin debounce to wait for
     */
    void EZPI_service_gpioisr_register_v3(l_ezlopi_item_t *item, f_interrupt_upcall_t __upcall, TickType_t debounce_ms);

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_SERVICE_GPIOISR_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
