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
 * @file    ezlopi_core_processes.c
 * @brief   Function to perform operation on ezlopi-process/task
 * @author
 * @version 1.0
 * @date    March 7th, 2024 6:27 PM
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "../../build/config/sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ezlopi_util_trace.h"
#include "ezlopi_core_processes.h"
#include "ezlopi_cloud_constants.h"

#if defined(CONFIG_FREERTOS_USE_TRACE_FACILITY)
/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/
typedef struct
{
    TaskHandle_t task_handle;
    size_t stack_depth;
    bool is_deleted;
} s_ezlopi_task_info_t;

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
static size_t __set_default_task_memory_usage(const char *default_task_name);
static bool __check_ezlopi_task(TaskHandle_t task_handle);
static void __set_task_to_arry(cJSON *cj_processes_array);

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/
static s_ezlopi_task_info_t ezlopi_task_info_array[ENUM_TASK_MAX];

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
int ezlopi_core_get_processes_details(cJSON *cj_processes_array)
{
    int ret = 0;
    if (cj_processes_array && cj_processes_array->type == cJSON_Array)
    {
        __set_task_to_arry(cj_processes_array);
        UBaseType_t total_task_numbers = uxTaskGetNumberOfTasks();
        TRACE_E("Total tasks existing are: %d", total_task_numbers);

        TaskStatus_t task_array[total_task_numbers];
        uxTaskGetSystemState(task_array, total_task_numbers, NULL);

        for (int i = 0; i < total_task_numbers; i++)
        {
            if (!__check_ezlopi_task(task_array[i].xHandle))
            {
                cJSON *cj_process = cJSON_CreateObject(__FUNCTION__);
                if (cj_process)
                {
                    cJSON_AddNumberToObject(__FUNCTION__, cj_process, "pid", task_array[i].xTaskNumber);
                    cJSON_AddStringToObject(__FUNCTION__, cj_process, "processName", task_array[i].pcTaskName);
                    size_t default_task_stack_size = __set_default_task_memory_usage(task_array[i].pcTaskName);
                    if (default_task_stack_size != 0)
                    {
                        cJSON_AddNumberToObject(__FUNCTION__, cj_process, "memoryUsage", default_task_stack_size);
                        if (default_task_stack_size != 0)
                        {
                            cJSON_AddNumberToObject(__FUNCTION__, cj_process, "vmRss", (default_task_stack_size - task_array[i].usStackHighWaterMark));
                        }
                        cJSON_AddStringToObject(__FUNCTION__, cj_process, "units", "bytes");
                        cJSON_AddItemToArray(cj_processes_array, cj_process);
                    }
                }
            }
        }
    }
    else
    {
        ret = 1;
    }
    return ret;
}

int EZPI_core_process_set_process_info(e_ezlopi_task_enum_t task_num, TaskHandle_t *task_handle, size_t task_depth)
{
    int ret = 0;
    if (task_handle && *task_handle)
    {
        ezlopi_task_info_array[task_num].stack_depth = task_depth;
        ezlopi_task_info_array[task_num].task_handle = *task_handle;
        ezlopi_task_info_array[task_num].is_deleted = false;
        ret = 1;
    }
    return ret;
}

int EZPI_core_process_set_is_deleted(e_ezlopi_task_enum_t task_num)
{
    int ret = 0;
    if (ezlopi_task_info_array[task_num].task_handle)
    {
        ezlopi_task_info_array[task_num].stack_depth = 0;
        ezlopi_task_info_array[task_num].task_handle = NULL;
        ezlopi_task_info_array[task_num].is_deleted = true;
    }
    return ret;
}

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/

static size_t __set_default_task_memory_usage(const char *default_task_name)
{
    size_t stack_size = 0;
    if (NULL != default_task_name)
    {
        if ((0 == strncmp("IDLE0", default_task_name, 6) || (0 == strncmp("IDLE1", default_task_name, 6))))
        {
            stack_size = CONFIG_FREERTOS_IDLE_TASK_STACKSIZE;
        }
        else if (0 == strncmp("ipc0", default_task_name, 5) || (0 == strncmp("ipc1", default_task_name, 5)))
        {
            stack_size = CONFIG_ESP_IPC_TASK_STACK_SIZE;
        }
        else if (0 == strncmp("esp_timer", default_task_name, 10))
        {
            stack_size = CONFIG_ESP_TIMER_TASK_STACK_SIZE;
        }
        else if (0 == strncmp("Tmr Svc", default_task_name, 8))
        {
            stack_size = CONFIG_FREERTOS_TIMER_TASK_STACK_DEPTH;
        }
#ifdef CONFIG_EZPI_BLE_ENABLE
        else if (0 == strncmp("BTU_TASK", default_task_name, 9))
        {
            // stack_size = CONFIG_BTU_TASK_STACK_SIZE;
            stack_size = 4096;
        }
        else if (0 == strncmp("BTC_TASK", default_task_name, 9))
        {
            // stack_size = CONFIG_BT_BTC_TASK_STACK_SIZE;
            stack_size = 4096;
        }
#endif // CONFIG_EZPI_BLE_ENABLE
        else if (0 == strncmp("sys_evt", default_task_name, 8))
        {
            stack_size = CONFIG_ESP_SYSTEM_EVENT_TASK_STACK_SIZE;
        }
        else if (0 == strncmp("mdns", default_task_name, 5))
        {
            stack_size = CONFIG_MDNS_TASK_STACK_SIZE;
        }
    }
    return stack_size;
}
    
static bool __check_ezlopi_task(TaskHandle_t task_handle)
{
    bool ret = false;
    int ezlopi_task_info_array_size = ENUM_TASK_MAX - 1;
    while (ezlopi_task_info_array_size >= 0)
    {
        if (ezlopi_task_info_array[ezlopi_task_info_array_size].task_handle == task_handle)
        {
            ret = true;
            break;
        }
        ezlopi_task_info_array_size--;
    }
    return ret;
}

static void __set_task_to_arry(cJSON *cj_processes_array)
{
    for (int i = 0; i < ENUM_TASK_MAX; i++)
    {
        if (!ezlopi_task_info_array[i].is_deleted && ezlopi_task_info_array[i].task_handle)
        {
            cJSON *cj_process = cJSON_CreateObject(__FUNCTION__);
            if (cj_process)
            {
                TaskStatus_t task_details;
                vTaskGetInfo(ezlopi_task_info_array[i].task_handle, &task_details, pdTRUE, eInvalid);
                cJSON_AddNumberToObject(__FUNCTION__, cj_process, "pid", task_details.xTaskNumber);
                cJSON_AddStringToObject(__FUNCTION__, cj_process, "processName", (NULL == task_details.pcTaskName ? ezlopi__str : task_details.pcTaskName));
                cJSON_AddNumberToObject(__FUNCTION__, cj_process, "memoryUsage", ezlopi_task_info_array[i].stack_depth);
                cJSON_AddNumberToObject(__FUNCTION__, cj_process, "vmRss", (ezlopi_task_info_array[i].stack_depth - task_details.usStackHighWaterMark));
                cJSON_AddStringToObject(__FUNCTION__, cj_process, "units", "bytes");
                cJSON_AddItemToArray(cj_processes_array, cj_process);
            }
        }
    }
}

#endif // CONFIG_FREERTOS_USE_TRACE_FACILITY

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
