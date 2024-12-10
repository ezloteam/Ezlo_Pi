
/**
 * @file    ezlopi_service_broadcast.c
 * @brief
 * @author
 * @version
 * @date
 */
/* ===========================================================================
** Copyright (C) 2022 Ezlo Innovation Inc
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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "ezlopi_core_processes.h"
#include "ezlopi_core_broadcast.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_errors.h"

#include "ezlopi_cloud_constants.h"

#include "ezlopi_service_loop.h"
#include "ezlopi_service_broadcast.h"

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
/**
 * @brief Broadcast loop that 
 * 
 * @param[in] arg Loop argument 
 */
static void __broadcast_loop(void *arg);
/**
 * @brief Sends data to the broadcast queue which will be sent to the cloud
 * 
 * @param[in] cj_broadcast_data JSON object that contains data to broadcast
 * @return ezlopi_error_t 
 * @retval EZPI_SUCCESS on success, or EZPI_FAILED on error
 */
static ezlopi_error_t ezlopi_service_broadcast_send_to_queue(cJSON *cj_broadcast_data);

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/
static QueueHandle_t __broadcast_queue = NULL;

void ezlopi_service_broadcast_init(void)
{
    __broadcast_queue = xQueueCreate(10, sizeof(cJSON *));
    if (__broadcast_queue)
    {
        ezlopi_core_broadcast_methods_set_queue(ezlopi_service_broadcast_send_to_queue);
        ezlopi_service_loop_add("broadcast-loop", __broadcast_loop, 1, NULL);
    }
}

static void __broadcast_loop(void *arg)
{
    static cJSON *cj_data = NULL;
    static uint32_t broadcast_wait_start = 0;

    if (cj_data)
    {
        if ((xTaskGetTickCount() - broadcast_wait_start) > 1000 / portTICK_RATE_MS)
        {
            ezlopi_core_broadcast_cjson(cj_data);
            cJSON_Delete(__FUNCTION__, cj_data);
            cj_data = NULL;
        }
    }
    else
    {
        if (pdTRUE == xQueueReceive(__broadcast_queue, &cj_data, 10))
        {
            broadcast_wait_start = xTaskGetTickCount();
        }
    }

    vTaskDelay(1);
}

static ezlopi_error_t ezlopi_service_broadcast_send_to_queue(cJSON *cj_broadcast_data)
{
    ezlopi_error_t ret = EZPI_FAILED;

    if (__broadcast_queue && cj_broadcast_data)
    {
        if (xQueueIsQueueFullFromISR(__broadcast_queue))
        {
            cJSON *cj_tmp_data = NULL;
            if (pdTRUE == xQueueReceive(__broadcast_queue, &cj_tmp_data, 5 / portTICK_PERIOD_MS))
            {
                if (cj_tmp_data)
                {
                    cJSON_Delete(__FUNCTION__, cj_tmp_data);
                }
            }
        }
        else
        {
            // TRACE_S(" ----- Adding to broadcast queue -----");
        }

        cJSON *cj_data = cj_broadcast_data;
        if (pdTRUE == xQueueSend(__broadcast_queue, &cj_data, 500 / portTICK_PERIOD_MS))
        {
            ret = EZPI_SUCCESS;
        }
        else
        {
            // TRACE_D(" ----- Failed adding to queue -----");
        }
    }

    return ret;
}

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
