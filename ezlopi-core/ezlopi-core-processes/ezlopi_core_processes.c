
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_core_processes.h"

int ezopi_core_get_processes_details(cJSON* cj_processes_array)
{
    int ret = 0;
    if (cj_processes_array && cj_processes_array->type == cJSON_Array)
    {
        UBaseType_t total_task_numbers = uxTaskGetNumberOfTasks();
        TRACE_E("Total tasks existing are: %d", total_task_numbers);
        TaskStatus_t task_array[total_task_numbers];
        uxTaskGetSystemState(task_array, total_task_numbers, NULL);
        printf("\t\tAll tasks: [ ");
        for (int i = 0; i < total_task_numbers; i++)
        {
            printf("(name: %s, pid: %d, base: %p, watermark: 0x%x)", task_array[i].pcTaskName, task_array[i].xTaskNumber, task_array[i].pxStackBase, task_array[i].usStackHighWaterMark);
        }
        printf("]\n\n");
        // cJSON *cj_process = cJSON_CreateObject();
        // if(cj_process)
        // {

        // }
    }
    else
    {
        ret = 1;
    }
    return ret;
}

