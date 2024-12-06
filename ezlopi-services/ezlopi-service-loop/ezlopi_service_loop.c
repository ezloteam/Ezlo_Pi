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
 * @file    main.c
 * @brief   perform some function on data
 * @author  John Doe
 * @version 0.1
 * @date    1st January 2024
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

#include <string.h>
#include <stdint.h>
#include <time.h>

#include "../../build/config/sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_core_heap.h"
#include "ezlopi_core_actions.h"
#include "ezlopi_core_processes.h"
#include "ezlopi_core_devices_list.h"

#include "ezlopi_service_loop.h"

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/
#define MAX_LEN(str1_len, str2_len) ((str1_len > str2_len) ? str1_len : str2_len)

typedef struct s_loop_node
{
    void *arg;
    f_loop_t loop;
    const char *name;
    uint32_t period_ms;
    uint32_t _timer_ms;

    struct s_loop_node *next;
} s_loop_node_t;

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
static void __loop(void *pv);
static s_loop_node_t *__create_node(const char *name, f_loop_t loop, uint32_t period_ms, void *arg);

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/
static s_loop_node_t *__loop_head = NULL;

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/

/**
 * @brief Global/extern function template example
 * Convention : Use capital letter for initial word on extern function
 * @param arg
 */
void ezlopi_service_loop_add(const char *name, f_loop_t loop, uint32_t period_ms, void *arg)
{
    if (loop && name) // adding to guard [check if 'loop' is already present]
    {
        if (__loop_head)
        {
            s_loop_node_t *__loop_node = __loop_head;
            while (__loop_node->next)
            {
                __loop_node = __loop_node->next;
            }

            __loop_node->next = __create_node(name, loop, (period_ms / portTICK_RATE_MS), arg);
        }
        else
        {
            __loop_head = __create_node(name, loop, (period_ms / portTICK_RATE_MS), arg);
        }
    }
}

void ezlopi_service_loop_remove(f_loop_t loop)
{
    if (loop && __loop_head)
    {
        if (__loop_head->loop == loop)
        {
            s_loop_node_t *__del_node = __loop_head;
            __loop_head = __loop_head->next;
            ezlopi_free(__FUNCTION__, __del_node);
        }
        else
        {
            s_loop_node_t *__loop_node = __loop_head;
            while (__loop_node->next)
            {
                if (__loop_node->next->loop == loop)
                {
                    s_loop_node_t *__del_node = __loop_node->next;
                    __loop_node->next = __loop_node->next->next;
                    ezlopi_free(__FUNCTION__, __del_node);
                    break;
                }

                __loop_node = __loop_node->next;
            }
        }
    }
}

void ezlopi_service_loop_init(void)
{
    TaskHandle_t ezlopi_service_timer_task_handle = NULL;
    // xTaskCreate(event_process, "event_process", EZLOPI_SERVICE_TIMER_TASK_DEPTH, NULL, 4, &ezlopi_service_timer_task_handle);
    xTaskCreate(__loop, "__loop", EZLOPI_SERVICE_LOOP_TASK_DEPTH, NULL, 4, &ezlopi_service_timer_task_handle);
    ezlopi_core_process_set_process_info(ENUM_EZLOPI_SERVICE_LOOP_TASK, &ezlopi_service_timer_task_handle, EZLOPI_SERVICE_LOOP_TASK_DEPTH);
}

/*******************************************************************************
 *                          Static Function Definitions
 *******************************************************************************/
static void __loop(void* pv)
{
    while (1)
    {
        uint32_t __run_time = xTaskGetTickCount();
        s_loop_node_t *__loop_node = __loop_head;

        while (__loop_node)
        {

            uint32_t conditions = ((xTaskGetTickCount() - __loop_node->_timer_ms) >= __loop_node->period_ms);
            if ((NULL != __loop_node->loop) && conditions)
            {

                uint32_t __loop_time = xTaskGetTickCount();
                __loop_node->loop(__loop_node->arg);
                __loop_node->_timer_ms = xTaskGetTickCount();
                __loop_time = (xTaskGetTickCount() - __loop_time);

                // TRACE_D("'%s': \t\t %u(period: %d(cmp: %d))", __loop_node->name ? __loop_node->name : "", __loop_time);

                vTaskDelay(1 / portTICK_RATE_MS);
            }

            __loop_node = __loop_node->next;
        }

        __run_time = xTaskGetTickCount() - __run_time;
        // TRACE_I("loop runtime: %u", __run_time);

        vTaskDelay(1 / portTICK_RATE_MS);
    }
}

static s_loop_node_t *__create_node(const char *name, f_loop_t loop, uint32_t period_ms, void *arg)
{
    s_loop_node_t *__loop_node = ezlopi_malloc(__FUNCTION__, sizeof(s_loop_node_t));

    if (__loop_node)
    {
        __loop_node->arg = arg;
        __loop_node->name = name;
        __loop_node->loop = loop;
        __loop_node->_timer_ms = 0;
        __loop_node->period_ms = period_ms;

        __loop_node->next = NULL;
    }

    return __loop_node;
}

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
