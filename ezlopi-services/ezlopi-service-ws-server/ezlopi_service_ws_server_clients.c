#include <stdio.h>
#include <string.h>

#include "ezlopi_service_ws_server.h"
#include "ezlopi_service_ws_server_clients.h"

static l_ws_server_client_conn_t *l_client_conn_head = NULL;

static l_ws_server_client_conn_t *__create_client_conn(void);

l_ws_server_client_conn_t *ezlopi_service_ws_server_clients_get_head(void)
{
    return l_client_conn_head;
}

l_ws_server_client_conn_t *ezlopi_service_ws_server_clients_get_by_handle(void *http_handle)
{
    l_ws_server_client_conn_t *curr_client = l_client_conn_head;
    while (curr_client)
    {
        if (curr_client->http_handle == http_handle)
        {
            break;
        }
        curr_client = curr_client->next;
    }

    return curr_client;
}

l_ws_server_client_conn_t *ezlopi_service_ws_server_clients_add(void *http_handle, int http_descriptor)
{
    l_ws_server_client_conn_t *ws_client_conn = NULL;
    if (l_client_conn_head)
    {
        if (l_client_conn_head->http_handle != http_handle)
        {
            uint32_t dup_flag = 0;
            l_ws_server_client_conn_t *curr_conn = l_client_conn_head;
            while (curr_conn->next)
            {
                if (curr_conn->next->http_handle == http_handle)
                {
                    dup_flag = 1;
                    break;
                }
                curr_conn = curr_conn->next;
            }

            if (0 == dup_flag)
            {
                ws_client_conn = __create_client_conn();
                if (ws_client_conn)
                {
                    curr_conn->next = ws_client_conn;
                    ws_client_conn->http_handle = http_handle;
                    ws_client_conn->http_descriptor = http_descriptor;
                }
            }
        }
    }
    else
    {
        ws_client_conn = __create_client_conn();
        if (ws_client_conn)
        {
            l_client_conn_head = ws_client_conn;
            ws_client_conn->http_handle = http_handle;
            ws_client_conn->http_descriptor = http_descriptor;
        }
    }

    return ws_client_conn;
}

int ezlopi_service_ws_server_clients_remove_by_handle(void *http_handle)
{
    int ret = 0;

    if (http_handle)
    {
        l_ws_server_client_conn_t *ws_popped_con = ezlopi_service_ws_server_clients_pop(http_handle);
        if (ws_popped_con)
        {
            free(ws_popped_con);
        }
    }

    return ret;
}

l_ws_server_client_conn_t *ezlopi_service_ws_server_clients_pop(void *http_handle)
{
    l_ws_server_client_conn_t *pop_con = NULL;

    if (http_handle == l_client_conn_head->http_handle)
    {
        pop_con = l_client_conn_head;
        l_client_conn_head = l_client_conn_head->next;
        pop_con->next = NULL;
    }
    else
    {
        l_ws_server_client_conn_t *curr_conn = l_client_conn_head;
        while (curr_conn->next)
        {
            if (curr_conn->next->http_handle == http_handle)
            {
                pop_con = curr_conn->next;
                curr_conn->next = curr_conn->next->next;
                pop_con->next = NULL;
                break;
            }

            curr_conn = curr_conn->next;
        }
    }

    return pop_con;
}

static l_ws_server_client_conn_t *__create_client_conn(void)
{
    l_ws_server_client_conn_t *new_ws_conn = malloc(sizeof(l_ws_server_client_conn_t));
    if (new_ws_conn)
    {
        new_ws_conn->fail_count = 0;
        new_ws_conn->http_handle = NULL;
        new_ws_conn->http_descriptor = 0;
        new_ws_conn->next = NULL;
    }

    return new_ws_conn;
}
