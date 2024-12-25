
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

#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "ezlopi_core_sntp.h"
#include "ezlopi_core_processes.h"
#include "ezlopi_core_broadcast.h"
#include "ezlopi_core_cjson_macros.h"

#include "ezlopi_cloud_constants.h"

#include "ezlopi_service_loop.h"
#include "ezlopi_service_otel.h"
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
static ezlopi_error_t ezpi_service_broadcast_send_to_queue(cJSON *cj_broadcast_data);

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/
static QueueHandle_t __broadcast_queue = NULL;

void EZPI_service_broadcast_init(void)
{
    __broadcast_queue = xQueueCreate(10, sizeof(cJSON *));
    if (__broadcast_queue)
    {
        EZPI_core_broadcast_methods_set_queue(ezpi_service_broadcast_send_to_queue);
        EZPI_service_loop_add("broadcast-loop", __broadcast_loop, 1, NULL);
    }
}

static void __broadcast_loop(void *arg)
{
    static cJSON *cj_data = NULL;
    static uint32_t broadcast_wait_start = 0;

    if (cj_data)
    {
        if ((xTaskGetTickCount() - broadcast_wait_start) > 5 / portTICK_RATE_MS)
        {
            cJSON *cj_method = cJSON_GetObjectItem(__FUNCTION__, cj_data, ezlopi_method_str);
            cJSON *cj_msg_subclass = cJSON_GetObjectItem(__FUNCTION__, cj_data, ezlopi_msg_subclass_str);
            cJSON *cj_startTime = cJSON_DetachItemFromObject(__FUNCTION__, cj_data, ezlopi_startTime_str);

            EZPI_core_broadcast_cjson(cj_data);
            cJSON_Delete(__FUNCTION__, cj_data);
            cj_data = NULL;

            s_otel_trace_t *trace_obj = ezlopi_malloc(__FUNCTION__, sizeof(s_otel_trace_t));
            if (trace_obj)
            {
                memset(trace_obj, 0, sizeof(s_otel_trace_t));

                trace_obj->kind = E_OTEL_KIND_SERVER;
                trace_obj->start_time = cj_startTime ? cj_startTime->valuedouble : 0;
                trace_obj->end_time = EZPI_core_sntp_get_current_time_sec();
                trace_obj->free_heap = esp_get_free_heap_size();
                trace_obj->heap_watermark = esp_get_minimum_free_heap_size();

                if (cj_msg_subclass && (cj_msg_subclass->type == cJSON_String) && cj_msg_subclass->valuestring && cj_msg_subclass->str_value_len)
                {
                    trace_obj->msg_subclass = ezlopi_malloc(__FUNCTION__, cj_msg_subclass->str_value_len + 1);
                    if (trace_obj->msg_subclass)
                    {
                        snprintf(trace_obj->msg_subclass, cj_msg_subclass->str_value_len + 1, "%.*s", cj_msg_subclass->str_value_len, cj_msg_subclass->valuestring);
                    }
                }

                if (cj_method && (cj_method->type == cJSON_String) && cj_method->valuestring && cj_method->str_value_len)
                {
                    trace_obj->method = ezlopi_malloc(__FUNCTION__, cj_method->str_value_len + 1);
                    if (trace_obj->method)
                    {
                        snprintf(trace_obj->method, cj_method->str_value_len + 1, "%.*s", cj_method->str_value_len, cj_method->valuestring);
                    }
                }

                cJSON_Delete(__FUNCTION__, cj_startTime);

                if (0 == ezlopi_service_otel_add_trace_to_telemetry_queue_struct(trace_obj))
                {
                    ezlopi_free(__FUNCTION__, trace_obj);
                }
            }

#if 0
            if (cj_method_dup || cj_msg_subclass_dup)
            {
                cJSON *cj_trace_telemetry = cJSON_CreateObject(__FUNCTION__);
                if (cj_trace_telemetry)
                {
                    if (false == cJSON_AddItemToObject(__FUNCTION__, cj_trace_telemetry, ezlopi_method_str, cj_method_dup))
                    {
                        cJSON_Delete(__FUNCTION__, cj_method_dup);
                    }

                    if (false == cJSON_AddItemToObject(__FUNCTION__, cj_trace_telemetry, ezlopi_msg_subclass_str, cj_msg_subclass_dup))
                    {
                        cJSON_Delete(__FUNCTION__, cj_method_dup);
                    }

                    cJSON_AddNumberToObject(__FUNCTION__, cj_trace_telemetry, ezlopi_kind_str, 1);
                    cJSON_AddNumberToObject(__FUNCTION__, cj_trace_telemetry, ezlopi_startTime_str, cj_startTime ? cj_startTime->valuedouble : 0);
                    cJSON_AddNumberToObject(__FUNCTION__, cj_trace_telemetry, ezlopi_endTime_str, EZPI_core_sntp_get_current_time_sec());

                    if (0 == ezlopi_service_otel_add_trace_to_telemetry_queue(cj_trace_telemetry))
                    {
                        cJSON_Delete(__FUNCTION__, cj_trace_telemetry);
                    }
                }
                else
                {
                    cJSON_Delete(__FUNCTION__, cj_method_dup);
                    cJSON_Delete(__FUNCTION__, cj_msg_subclass_dup);
                }
            }

            cJSON_Delete(__FUNCTION__, cj_startTime);
#endif
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

static ezlopi_error_t ezpi_service_broadcast_send_to_queue(cJSON *cj_broadcast_data)
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
