#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "ezlopi_util_uuid.h"
#include "ezlopi_core_ws_server_conn.h"

static s_ws_server_connections_t *ws_conn_head = NULL;

static s_ws_server_connections_t *__create_ws_conn(void);

s_ws_server_connections_t *ezlopi_core_ws_server_conn_get_head(void)
{
    return ws_conn_head;
}

s_ws_server_connections_t *ezlopi_core_ws_server_conn_add_ws_conn(struct netconn *new_net_conn)
{
    s_ws_server_connections_t *ws_conn = NULL;
    if (ws_conn_head)
    {
        s_ws_server_connections_t *curr_conn = ws_conn_head;
        while (curr_conn->next)
        {
            curr_conn = curr_conn->next;
        }

        ws_conn = __create_ws_conn();
        if (ws_conn)
        {
            curr_conn->next = ws_conn;
            ws_conn->net_conn = new_net_conn;
        }
    }
    else
    {
        ws_conn = __create_ws_conn();
        if (ws_conn)
        {
            ws_conn_head = ws_conn;
            ws_conn->net_conn = new_net_conn;
        }
    }

    return ws_conn;
}

int ezlopi_core_ws_server_conn_remove(s_ws_server_connections_t *remove_con)
{
    int ret = 0;

    if (remove_con)
    {
    }

    return ret;
}

s_ws_server_connections_t *ezlopi_core_ws_server_conn_pop(struct netconn *conn)
{
    s_ws_server_connections_t *pop_con = NULL;

    if (conn == ws_conn_head->net_conn)
    {
        pop_con = ws_conn_head;
        ws_conn_head = ws_conn_head->next;
        pop_con->next = NULL;
    }
    else
    {
        s_ws_server_connections_t *curr_conn = ws_conn_head;
        while (curr_conn->next)
        {
            if (curr_conn->next->net_conn == conn)
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

static s_ws_server_connections_t *__create_ws_conn(void)
{
    s_ws_server_connections_t *new_ws_conn = malloc(sizeof(s_ws_server_connections_t));
    if (new_ws_conn)
    {
        new_ws_conn->net_conn = NULL;
        new_ws_conn->task_handle = NULL;

        ezlopi_util_uuid_generate_random(new_ws_conn->uuid);

        new_ws_conn->next = NULL;
    }

    return new_ws_conn;
}