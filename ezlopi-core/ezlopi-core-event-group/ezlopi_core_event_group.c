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
 * @file    ezlopi_core_event_group.c
 * @brief   Operation for event groups
 * @author  xx
 * @version 0.1
 * @date    12th DEC 2024
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "ezlopi_util_trace.h"
#include "ezlopi_core_errors.h"

#include "ezlopi_core_event_group.h"

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/
static EventGroupHandle_t ezlopi_event_group_handle = NULL;

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
void EZPI_event_group_create(void)
{
    if (NULL == ezlopi_event_group_handle)
    {
        ezlopi_event_group_handle = xEventGroupCreate();
    }
}

ezlopi_error_t EZPI_core_event_group_set_event(e_ezlopi_event_t event)
{
    // if (ezlopi_event_group_handle && (event < BIT31))
    // {
    //     xEventGroupSetBits(ezlopi_event_group_handle, event);
    // }

    ezlopi_error_t error = EZPI_SUCCESS;
    if (ezlopi_event_group_handle)
    {
        if (event < BIT31)
        {
            xEventGroupSetBits(ezlopi_event_group_handle, event);
        }
        else
        {
            error = EZPI_ERR_EVENT_GROUP_BIT_UNDEFINED;
        }
    }
    else
    {
        error = EZPI_ERR_EVENT_GROUP_UNINITIALIZED;
    }
    return error;
}

ezlopi_error_t EZPI_core_event_group_clear_event(e_ezlopi_event_t event)
{
    // if (ezlopi_event_group_handle && (event < BIT31))
    // {
    //     xEventGroupClearBits(ezlopi_event_group_handle, event);
    // }

    ezlopi_error_t error = EZPI_SUCCESS;
    if (ezlopi_event_group_handle)
    {
        if (event < BIT31)
        {
            xEventGroupClearBits(ezlopi_event_group_handle, event);
        }
        else
        {
            error = EZPI_ERR_EVENT_GROUP_BIT_UNDEFINED;
        }
    }
    else
    {
        error = EZPI_ERR_EVENT_GROUP_UNINITIALIZED;
    }
    return error;
}

ezlopi_error_t EZPI_core_event_group_wait_for_event(e_ezlopi_event_t event, uint32_t wait_time_ms, uint32_t clear_on_exit)
{
    ezlopi_error_t error = EZPI_ERR_EVENT_GROUP_BIT_WAIT_FAILED;
    if (ezlopi_event_group_handle)
    {
        EventBits_t event_bits = xEventGroupWaitBits(ezlopi_event_group_handle, event, clear_on_exit ? pdTRUE : pdFALSE, pdFALSE, ((wait_time_ms == portMAX_DELAY) ? portMAX_DELAY : (wait_time_ms / portTICK_RATE_MS)));
        if (event_bits & event)
        {
            error = EZPI_SUCCESS;
        }
    }
    else
    {
        error = EZPI_ERR_EVENT_GROUP_UNINITIALIZED;
    }

    return error;
}

e_ezlopi_event_t EZPI_core_event_group_get_eventbit_status()
{
    e_ezlopi_event_t ret = 0;
    if (ezlopi_event_group_handle)
    {
        ret = xEventGroupGetBits(ezlopi_event_group_handle);
    }
    return ret;
}

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
