#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "cjext.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include "ezlopi_util_trace.h"
#include "ezlopi_core_buffer.h"
#include "ezlopi_core_ezlopi_broadcast.h"
#include "EZLOPI_USER_CONFIG.h"

// static uint32_t __message_count = 0;
static l_broadcast_method_t* __method_head = NULL;
static int (*__broadcast_queue_func)(cJSON* cj_data) = NULL;

static int __call_broadcast_methods(char* data);
static l_broadcast_method_t* __method_create(f_broadcast_method_t method, char* name, uint32_t retries);

void ezlopi_core_ezlopi_broadcast_methods_set_queue(int (*func)(cJSON*))
{
    __broadcast_queue_func = func;
}

int ezlopi_core_ezlopi_broadcast_add_to_queue(cJSON* cj_data)
{
    int ret = 0;
    if (cj_data && __broadcast_queue_func)
    {
        ret = __broadcast_queue_func(cj_data);
    }
    else
    {
        TRACE_E("cj_data: %p, __broadcast_queue_func: %p", cj_data, __broadcast_queue_func);
    }
    return ret;
}

int ezlopi_core_ezlopi_broadcast_cjson(cJSON* cj_data)
{
    int ret = 0;

    if (cj_data)
    {
        uint32_t buffer_len = 0;
        char* data_buffer = ezlopi_core_buffer_acquire(&buffer_len, 5000);

        if (data_buffer && buffer_len)
        {
            TRACE_I("-----------------------------> buffer acquired!");
            memset(data_buffer, 0, buffer_len);

            if (true == cJSON_PrintPreallocated(__FUNCTION__, cj_data, data_buffer, buffer_len, false))
            {
                TRACE_D("----------------- broadcasting:\r\n%s", data_buffer);
                ret = __call_broadcast_methods(data_buffer);
            }

            ezlopi_core_buffer_release();
            TRACE_I("-----------------------------> buffer released!");
        }
        else
        {
            TRACE_E("-----------------------------> buffer acquired failed!");
        }
    }

    return ret;
}

l_broadcast_method_t* ezlopi_core_ezlopi_broadcast_method_add(f_broadcast_method_t broadcast_method, char* method_name, uint32_t retries)
{
    int duplicate_method = 0;
    l_broadcast_method_t* ret = NULL;
    l_broadcast_method_t* curr_node = __method_head;

    while (curr_node)
    {
        if (broadcast_method == curr_node->func)
        {
            duplicate_method = 1;
        }
        curr_node = curr_node->next;
    }

    if (0 == duplicate_method)
    {
        ret = __method_create(broadcast_method, method_name, retries);
        if (ret)
        {
            if (__method_head)
            {
                l_broadcast_method_t* curr_node = __method_head;

                while (curr_node->next)
                {
                    curr_node = curr_node->next;
                }

                curr_node->next = ret;
            }
            else
            {
                __method_head = ret;
            }
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
            free(__FUNCTION__, remove_node);
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
                    free(__FUNCTION__, remove_node);

                    break;
                }

                curr_node = curr_node->next;
            }
        }
    }
}

static int __call_broadcast_methods(char* data)
{
    int ret = 0;
    l_broadcast_method_t* curr_method = __method_head;

    while (curr_method)
    {
        if (curr_method->func)
        {
            ret = 1;
            uint32_t retries = curr_method->fail_retry;

            do
            {
                if (curr_method->func(data))
                {
                    // TRACE_S("broadcasted - method:'%s'\r\ndata: %s", curr_method->method_name ? curr_method->method_name : "", data);
                    break;
                }

                vTaskDelay(10 / portTICK_RATE_MS);

            } while (retries--);
        }

        curr_method = curr_method->next;
    }

    return ret;
}

static l_broadcast_method_t* __method_create(f_broadcast_method_t method, char* method_name, uint32_t retries)
{
    l_broadcast_method_t* method_node = NULL;

    if (method)
    {
        method_node = malloc(__FUNCTION__, sizeof(l_broadcast_method_t));

        if (method_node)
        {
            memset(method_node, 0, sizeof(l_broadcast_method_t));

            method_node->next = NULL;
            method_node->func = method;
            method_node->fail_retry = retries;
            if (method_name)
            {
                snprintf(method_node->method_name, sizeof(method_node->method_name), "%s", method_name);
            }
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