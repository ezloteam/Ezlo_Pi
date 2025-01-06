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
 * @file    ezlopi_core_event_queue.c
 * @brief   perform function for event queue
 * @author  xx
 * @version 0.1
 * @date    12th DEC 2024
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#if 0

#include "ezlopi_core_event_queue.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "EZLOPI_USER_CONFIG.h"

#include "ezlopi_core_errors.h"

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
static QueueHandle_t generic_queue = NULL;

/*******************************************************************************
*                          Extern Data Definitions
*******************************************************************************/

/*******************************************************************************
*                          Extern Function Definitions
*******************************************************************************/
ezlopi_error_t EZPI_core_event_queue_init(void)
{
    ezlopi_error_t error = EZPI_SUCCESS;
    if (NULL == generic_queue)
    {
        generic_queue = xQueueCreate(20, sizeof(s_ezlo_event_t *));
        error = (NULL != generic_queue) ? error : EZPI_ERR_EVENT_QUEUE_INIT_FAILED;
    }
    return error;
}

ezlopi_error_t EZPI_core_event_queue_send(s_ezlo_event_t *event_data, int from_isr)
{
    ezlopi_error_t error = EZPI_ERR_EVENT_QUEUE_UNINITIALIZED;

    if (NULL != generic_queue)
    {
        if (xQueueIsQueueFullFromISR(generic_queue)) // 'FromISR' or not
        {
            s_ezlo_event_t *tmp_evt_data = NULL;
            if (from_isr)
            {
                BaseType_t xHigherPriorityTaskWoken = pdFALSE;
                xQueueReceiveFromISR(generic_queue, &tmp_evt_data, &xHigherPriorityTaskWoken);
            }
            else
            {
                xQueueReceive(generic_queue, &tmp_evt_data, 0);
            }

            if (tmp_evt_data)
            {
                if (tmp_evt_data->arg)
                {
                    ezlopi_free(__FUNCTION__, tmp_evt_data->arg);
                }
                ezlopi_free(__FUNCTION__, tmp_evt_data);
            }
        }

        BaseType_t send_error = pdTRUE;
        if (from_isr)
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            send_error = xQueueSendFromISR(generic_queue, &event_data, &xHigherPriorityTaskWoken);
        }
        else
        {
            send_error = xQueueSend(generic_queue, &event_data, 0);
        }
        error = pdTRUE == send_error ? EZPI_SUCCESS : EZPI_ERR_EVENT_QUEUE_RECV_SEND_ERROR;
    }

    return error;
}

ezlopi_error_t EZPI_core_event_queue_receive(s_ezlo_event_t **event_data, int time_out_ms)
{
    ezlopi_error_t error = EZPI_ERR_EVENT_QUEUE_UNINITIALIZED;
    if (generic_queue)
    {
        error = (pdTRUE == xQueueReceive(generic_queue, event_data, (time_out_ms / portTICK_RATE_MS))) ? EZPI_SUCCESS : EZPI_ERR_EVENT_QUEUE_RECV_SEND_ERROR;
    }
    return error;
}
/*******************************************************************************
*                         Static Function Definitions
*******************************************************************************/

#endif
/*******************************************************************************
 *                          End of File
 *******************************************************************************/
