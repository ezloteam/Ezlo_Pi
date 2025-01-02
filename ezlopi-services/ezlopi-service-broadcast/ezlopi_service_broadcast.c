
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
// static ezlopi_error_t ezpi_service_broadcast_send_to_queue(cJSON *cj_broadcast_data);
static ezlopi_error_t ezpi_service_broadcast_send_to_queue(s_broadcast_struct_t *broadcast_data);

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
    static uint32_t broadcast_wait_start = 0;
    static s_broadcast_struct_t *__broadcast_data = NULL;

    if (__broadcast_data)
    {
        if ((xTaskGetTickCount() - broadcast_wait_start) > 5 / portTICK_RATE_MS)
        {
            if (__broadcast_data->cj_broadcast_data)
            {
#ifdef CONFIG_EZPI_OPENTELEMETRY_ENABLE_TRACES

                char *id_str = ezlopi_service_otel_fetch_string_value_from_cjson(__broadcast_data->cj_broadcast_data, ezlopi_id_str);
                char *error_str = ezlopi_service_otel_fetch_string_value_from_cjson(__broadcast_data->cj_broadcast_data, ezlopi_error_str);
                char *method_str = ezlopi_service_otel_fetch_string_value_from_cjson(__broadcast_data->cj_broadcast_data, ezlopi_method_str);
                char *msg_subclass_str = ezlopi_service_otel_fetch_string_value_from_cjson(__broadcast_data->cj_broadcast_data, ezlopi_msg_subclass_str);

#if 0
                cJSON *cj_id = cJSON_GetObjectItem(__FUNCTION__, __broadcast_data->cj_broadcast_data, ezlopi_id_str);
                if (cj_id && cj_id->valuestring && (cj_id->type == cJSON_String) && cj_id->str_value_len)
                {
                    id_str = ezlopi_malloc(__FUNCTION__, cj_id->str_value_len + 1);
                    if (id_str)
                    {
                        snprintf(id_str, cj_id->str_value_len + 1, "%.*s", cj_id->str_value_len, cj_id->valuestring);
                    }
                }

                cJSON *cj_error = cJSON_GetObjectItem(__FUNCTION__, __broadcast_data->cj_broadcast_data, ezlopi_error_str);
                if (cj_error && cj_error->valuestring && (cj_error->type == cJSON_String) && cj_id->str_value_len)
                {
                    error_str = ezlopi_malloc(__FUNCTION__, cj_error->str_value_len + 1);
                    if (error_str)
                    {
                        snprintf(error_str, cj_error->str_value_len + 1, "%.*s", cj_error->str_value_len, cj_error->valuestring);
                    }
                }

                cJSON *cj_msg_subclass = cJSON_GetObjectItem(__FUNCTION__, __broadcast_data->cj_broadcast_data, ezlopi_msg_subclass_str);
                if (cj_msg_subclass && cj_msg_subclass->valuestring && (cj_msg_subclass->type == cJSON_String) && cj_msg_subclass->str_value_len)
                {
                    msg_subclass_str = ezlopi_malloc(__FUNCTION__, cj_msg_subclass->str_value_len + 1);
                    if (msg_subclass_str)
                    {
                        snprintf(msg_subclass_str, cj_msg_subclass->str_value_len + 1, "%.*s", cj_msg_subclass->str_value_len, cj_msg_subclass->valuestring);
                    }
                }

                cJSON *cj_method = cJSON_GetObjectItem(__FUNCTION__, __broadcast_data->cj_broadcast_data, ezlopi_method_str);
                if (cj_method)
                {
                    method_str = ezlopi_malloc(__FUNCTION__, cj_method->str_value_len + 1);
                    if (method_str)
                    {
                        snprintf(method_str, cj_method->str_value_len + 1, "%.*s", cj_method->str_value_len, cj_method->valuestring);
                    }
                }
#endif

#endif
                EZPI_core_broadcast_cjson(__broadcast_data->cj_broadcast_data);
                cJSON_Delete(__FUNCTION__, __broadcast_data->cj_broadcast_data);
                __broadcast_data->cj_broadcast_data = NULL;

#ifdef CONFIG_EZPI_OPENTELEMETRY_ENABLE_TRACES
                s_otel_trace_t *trace_obj = ezlopi_malloc(__FUNCTION__, sizeof(s_otel_trace_t));
                if (trace_obj)
                {
                    memset(trace_obj, 0, sizeof(s_otel_trace_t));

                    trace_obj->kind = E_OTEL_KIND_CLIENT;
                    trace_obj->start_time = __broadcast_data->time_stamp;
                    trace_obj->end_time = EZPI_core_sntp_get_current_time_sec();
                    trace_obj->free_heap = esp_get_free_heap_size();
                    trace_obj->heap_watermark = esp_get_minimum_free_heap_size();
                    trace_obj->name = EZPI_core_brodcast_source_to_name(__broadcast_data->source);
                    trace_obj->tick_count = xTaskGetTickCount();

                    trace_obj->id = id_str;
                    trace_obj->error = error_str;
                    trace_obj->method = method_str;
                    trace_obj->msg_subclass = msg_subclass_str;

                    id_str = NULL;
                    error_str = NULL;
                    method_str = NULL;
                    msg_subclass_str = NULL;

                    if (0 == ezlopi_service_otel_add_trace_to_telemetry_queue(trace_obj))
                    {
                        id_str = trace_obj->id;                     // re-assigning to free in case adding to queue fails
                        error_str = trace_obj->error;               // re-assigning to free in case adding to queue fails
                        method_str = trace_obj->method;             // re-assigning to free in case adding to queue fails
                        msg_subclass_str = trace_obj->msg_subclass; // re-assigning to free in case adding to queue fails

                        ezlopi_free(__FUNCTION__, trace_obj);
                    }
                }

                ezlopi_free(__FUNCTION__, id_str);
                ezlopi_free(__FUNCTION__, error_str);
                ezlopi_free(__FUNCTION__, method_str);
                ezlopi_free(__FUNCTION__, msg_subclass_str);
#endif
            }

            ezlopi_free(__FUNCTION__, __broadcast_data);
            __broadcast_data = NULL;
        }
    }
    else
    {
        if (pdTRUE == xQueueReceive(__broadcast_queue, &__broadcast_data, 10))
        {
            broadcast_wait_start = xTaskGetTickCount();
        }
    }

    vTaskDelay(1);
}

static ezlopi_error_t ezpi_service_broadcast_send_to_queue(s_broadcast_struct_t *broadcast_data)
{
    ezlopi_error_t ret = EZPI_FAILED;

    if (__broadcast_queue && broadcast_data)
    {
        if (xQueueIsQueueFullFromISR(__broadcast_queue))
        {
            s_broadcast_struct_t *tmp_data = NULL;
            if (pdTRUE == xQueueReceive(__broadcast_queue, &tmp_data, 5 / portTICK_PERIOD_MS))
            {
                if (tmp_data)
                {
                    cJSON_Delete(__FUNCTION__, tmp_data->cj_broadcast_data);
                    ezlopi_free(__FUNCTION__, tmp_data);
                }
            }
        }

        if (pdTRUE == xQueueSend(__broadcast_queue, &broadcast_data, 500 / portTICK_PERIOD_MS))
        {
            ret = EZPI_SUCCESS;
        }
    }

    return ret;
}

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
