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
 * @file    ezlopi_core_event_group.h
 * @brief   Operation for event groups
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 *          Lomas Subedi
 *          Riken Maharjan
 *          Nabin Dangi
 * @version 0.1
 * @date    12th DEC 2024
 */

#ifndef _EZLOPI_CORE_EVENT_GROUP_H_
#define _EZLOPI_CORE_EVENT_GROUP_H_

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

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
#define EZLOPI_EVENT_BASE 0x10

    typedef enum e_ezlopi_event
    {
        EZLOPI_EVENT_OTA = BIT0,
        EZLOPI_EVENT_WIFI_CONNECTED = BIT1,
        EZLOPI_EVENT_WIFI_FAIL = BIT2,
        EZLOPI_EVENT_NMA_REG = BIT3,
        EZLOPI_EVENT_PING = BIT4,
        // can not be more than BIT31
    } e_ezlopi_event_t;

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/
    /**
     * @brief Function to create event-group
     *
     */
    void EZPI_event_group_create(void);
    /**
     * @brief Function to set event
     *
     * @param event enum of target event
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_event_group_set_event(e_ezlopi_event_t event);
    /**
     * @brief Funtion to clear event
     *
     * @param event enum of target event
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_event_group_clear_event(e_ezlopi_event_t event);
    /**
     * @brief Funtion to trigger event
     *
     * @param event Target event
     * @param wait_time_ms Wait time for event to occur
     * @param clear_on_exit Clears event bits after occurance
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_event_group_wait_for_event(e_ezlopi_event_t event, uint32_t wait_time_ms, uint32_t clear_on_exit);
    /**
     * @brief Funtion to get event-bit status
     *
     * @return e_ezlopi_event_t
     */
    e_ezlopi_event_t EZPI_core_event_group_get_eventbit_status();

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_CORE_EVENT_GROUP_H_
/*******************************************************************************
 *                          End of File
 *******************************************************************************/
