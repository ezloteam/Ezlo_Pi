#include <cJSON.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include "ezlopi_util_trace.h"
#include "ezlopi_core_ezlopi_broadcast.h"

static SemaphoreHandle_t __broadcast_lock = NULL;
static l_broadcast_method_t* __method_head = NULL;
static int (*__broadcast_queue_func)(char*) = NULL;

static l_broadcast_method_t* __method_create(f_broadcast_method_t method, uint32_t retries);

void ezlopi_core_ezlopi_broadcast_methods_set_queue(int(*func)(char*))
{
    __broadcast_queue_func = func;
}

int ezlopi_core_ezlopi_broadcast_methods_send_to_queue(char* data)
{
    int ret = 0;
    if (__broadcast_queue_func)
    {
        ret = __broadcast_queue_func(data);
    }
    return ret;
}

int ezlopi_core_ezlopi_broadcast_methods_send_cjson_to_queue(cJSON* cj_data)
{
    int ret = 0;

    if (cj_data && __broadcast_queue_func)
    {
        if (__broadcast_lock)
        {
            if (pdTRUE == xSemaphoreTake(__broadcast_lock, 2000 / portTICK_RATE_MS))
            {
                char* data_to_broadcast = cJSON_PrintBuffered(cj_data, 10 * 1024, cJSON_False);

                if (data_to_broadcast)
                {
                    TRACE_D("length of 'data_to_broadcast': %d", sizeof(data_to_broadcast));

                    ret = __broadcast_queue_func(data_to_broadcast);
                    if (ret <= 0)
                    {
                        free(data_to_broadcast);
                    }
                }

                xSemaphoreGive(__broadcast_lock);
            }
        }
        else
        {
            char* data_to_broadcast = cJSON_PrintBuffered(cj_data, 10 * 1024, cJSON_False);
            if (data_to_broadcast)
            {
                ret = __broadcast_queue_func(data_to_broadcast);
                if (ret <= 0)
                {
                    free(data_to_broadcast);
                }
            }
        }
    }

    return ret;
}

l_broadcast_method_t* ezlopi_core_ezlopi_broadcast_methods_get_head(void)
{
    return __method_head;
}

void ezlopi_core_ezlopi_broadcast_execute(char* data)
{
    if (data)
    {
        l_broadcast_method_t* curr_node = __method_head;

        while (curr_node)
        {
            if (curr_node->func)
            {
                uint32_t retries = curr_node->fail_retry;

                do
                {
                    if (curr_node->func(data) > 0)
                    {
                        break;
                    }

                } while (--retries);
            }

            curr_node = curr_node->next;
        }
    }
}

l_broadcast_method_t* ezlopi_core_ezlopi_broadcast_method_add(f_broadcast_method_t broadcast_method, uint32_t retries)
{
    l_broadcast_method_t* ret = __method_create(broadcast_method, retries);

    if (ret)
    {
        TRACE_D("registering broadcast method ...");

        if (__method_head)
        {
            l_broadcast_method_t* curr_node = __method_head;

            while (curr_node->next)
            {
                curr_node = curr_node->next;
            }

            TRACE_D("registered ...");
            curr_node->next = ret;
        }
        else
        {
            TRACE_D("registered ...");
            __method_head = ret;
        }
    }
    else
    {
        TRACE_E("registering broadcast method failed ...");
    }

    if ((NULL != __method_head) && (NULL == __broadcast_lock))
    {
        __broadcast_lock = xSemaphoreCreateMutex();
        if (__broadcast_lock)
        {
            xSemaphoreGive(__broadcast_lock);
        }
    }

    return ret;
}

void ezlopi_core_ezlopi_broadcast_remove_method(f_broadcast_method_t broadcast_method)
{
    if (__method_head)
    {
        if (broadcast_method == __method_head->func)
        {
            l_broadcast_method_t* remove_node = __method_head;
            __method_head = __method_head->next;
            free(remove_node);
        }
        else
        {
            l_broadcast_method_t* curr_node = __method_head;
            while (curr_node->next)
            {
                if (curr_node->next->func == broadcast_method)
                {
                    l_broadcast_method_t* remove_node = curr_node->next;
                    curr_node->next = curr_node->next->next;
                    free(remove_node);

                    break;
                }

                curr_node = curr_node->next;
            }
        }
    }
}

static l_broadcast_method_t* __method_create(f_broadcast_method_t method, uint32_t retries)
{
    l_broadcast_method_t* method_node = NULL;

    if (method)
    {
        method_node = malloc(sizeof(l_broadcast_method_t));

        if (method_node)
        {
            memset(method_node, 0, sizeof(l_broadcast_method_t));

            method_node->next = NULL;
            method_node->func = method;
            method_node->fail_retry = retries;
            method_node->method_name = "";
        }
        else
        {
            TRACE_E("malloc failed");
        }
    }
    else
    {
        TRACE_E("method is NULL");
    }

    return method_node;
}