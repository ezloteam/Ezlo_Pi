#include "ezlopi_util_trace.h"
#include "ezlopi_core_ezlopi_broadcast.h"

static l_broadcast_method_t *method_head = NULL;

static l_broadcast_method_t *__method_create(f_broadcast_method_t method, uint32_t retries);

void ezlopi_core_ezlopi_broadcast_execute(char *data)
{
    if (data)
    {
        l_broadcast_method_t *curr_node = method_head;
        while (curr_node)
        {
            if (curr_node->func)
            {
                uint32_t retries = curr_node->fail_retry;
                TRACE_D("broadcast-method-name: %s", curr_node->method_name ? curr_node->method_name : "");
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

l_broadcast_method_t *ezlopi_core_ezlopi_broadcast_method_add(f_broadcast_method_t broadcast_method, uint32_t retries)
{
    l_broadcast_method_t *ret = __method_create(broadcast_method, retries);

    if (ret)
    {
        if (method_head)
        {
            l_broadcast_method_t *curr_node = method_head;

            while (curr_node->next)
            {
                curr_node = curr_node->next;
            }

            curr_node->next = ret;
        }
        else
        {
            method_head = ret;
        }
    }

    return ret;
}

static l_broadcast_method_t *__method_create(f_broadcast_method_t method, uint32_t retries)
{
    l_broadcast_method_t *method_node = NULL;

    if (method)
    {
        l_broadcast_method_t *method_node = malloc(sizeof(l_broadcast_method_t));

        if (method_node)
        {
            memset(method_node, 0, sizeof(l_broadcast_method_t));

            method_node->next = NULL;
            method_node->func = method;
            method_node->fail_retry = retries;
        }
    }

    return method_node;
}