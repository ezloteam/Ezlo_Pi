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
 * @file    ezlopi_core_buffer.c
 * @brief   perform some function on system-buffer for messages
 * @author  ezlopi_team_np
 * @version 0.1
 * @date    12th DEC 2024
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include "ezlopi_util_trace.h"
#include "ezlopi_core_buffer.h"
#include "EZLOPI_USER_CONFIG.h"

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
static char *__buffer = NULL;
static uint32_t __buffer_len = 0;
static xSemaphoreHandle __buffer_lock = NULL;
static volatile e_buffer_state_t __buffer_lock_state = EZ_BUFFER_STATE_NOT_INITIATED;

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
e_buffer_state_t EZPI_core_buffer_status(void)
{
    return __buffer_lock_state;
}

void EZPI_core_buffer_deinit(void)
{
    if (__buffer_lock)
    {
        vSemaphoreDelete(__buffer_lock);
        __buffer_lock = NULL;
    }

    ezlopi_free(__FUNCTION__, __buffer);
    __buffer = NULL;

    __buffer_len = 0;
    __buffer_lock_state = EZ_BUFFER_STATE_NOT_INITIATED;
}

void EZPI_core_buffer_init(uint32_t len)
{
    if (__buffer_lock)
    {
        xSemaphoreTake(__buffer_lock, portMAX_DELAY);
    }

    if (__buffer)
    {
        ezlopi_free(__FUNCTION__, __buffer);
        __buffer = NULL;
        __buffer_len = 0;
        __buffer_lock_state = EZ_BUFFER_STATE_NOT_INITIATED;
    }

    __buffer = ezlopi_malloc(__FUNCTION__, len);

    if (__buffer)
    {
        __buffer_len = len;

        if (NULL == __buffer_lock)
        {
            __buffer_lock = xSemaphoreCreateMutex();
            if (__buffer_lock)
            {
                __buffer_lock_state = EZ_BUFFER_STATE_AVAILABLE;
                xSemaphoreGive(__buffer_lock);
            }
            else
            {
                __buffer_len = 0;
                ezlopi_free(__FUNCTION__, __buffer);
                __buffer = NULL;
                __buffer_lock_state = EZ_BUFFER_STATE_NOT_INITIATED;
            }
        }
        else
        {
            __buffer_lock_state = EZ_BUFFER_STATE_AVAILABLE;
            xSemaphoreGive(__buffer_lock);
        }
    }
    else
    {
        if (__buffer_lock)
        {
            vSemaphoreDelete(__buffer_lock);
            __buffer_lock = NULL;
        }

        __buffer_len = 0;
        __buffer_lock_state = EZ_BUFFER_STATE_NOT_INITIATED;
    }
}

char *EZPI_core_buffer_acquire(const char *who, uint32_t *len, uint32_t wait_to_acquired_ms)
{
    char *ret = NULL;
    // uint32_t start_time = xTaskGetTickCount();
    if (__buffer_lock)
    {
        if (pdTRUE == xSemaphoreTake(__buffer_lock, wait_to_acquired_ms / portTICK_RATE_MS))
        {
            ret = __buffer;
            *len = __buffer_len;
            __buffer_lock_state = EZ_BUFFER_STATE_BUSY;
            TRACE_I("(%s): buffer acquired", who);
        }
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
        else
        {
            TRACE_E("(%s): buffer acquire failed!", who);
        }
#endif
    }
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
    else
    {
        TRACE_E("(%s): __buffer_lock = NULL!", who);
    }
#endif

    return ret;
}

void EZPI_core_buffer_release(const char *who)
{
#if 0
    if (__buffer_lock && (EZ_BUFFER_STATE_BUSY == __buffer_lock_state))
    {
        xSemaphoreGive(__buffer_lock);
        __buffer_lock_state = EZ_BUFFER_STATE_AVAILABLE;
        TRACE_I("(%s): buffer release success", who);
    }
    else
    {
        TRACE_E("__buffer_lock: %p", __buffer_lock);
        TRACE_E("__buffer_lock_state: %d", __buffer_lock_state);
        TRACE_E("(%s): buffer release failed!", who);
    }
#endif

    if (__buffer_lock && (pdTRUE == xSemaphoreGive(__buffer_lock)))
    {
        // xSemaphoreGive(__buffer_lock);
        __buffer_lock_state = EZ_BUFFER_STATE_AVAILABLE;
        TRACE_I("(%s): buffer release success", who);
    }
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
    else
    {
        TRACE_E("__buffer_lock: %p", __buffer_lock);
        // TRACE_E("__buffer_lock_state: %d", __buffer_lock_state);
        TRACE_E("(%s): buffer release failed!", who);
    }
#endif
}

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
