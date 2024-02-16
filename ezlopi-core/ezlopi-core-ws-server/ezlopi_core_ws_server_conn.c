#include <string.h>
#include "ezlopi_core_ws_server_conn.h"

static s_ws_server_new_connections_t *conn_head = NULL;

static s_ws_server_new_connections_t *__create_new_conn(TaskHandle_t handle, struct netconn *conn);

int ezlopi_core_ws_server_conn_add_new_conn(TaskHandle_t handle, struct netconn *conn)
{
    int ret = 0;
    if (conn_head)
    {
        s_ws_server_new_connections_t *curr_conn = conn_head;
        while (curr_conn->next)
        {
            curr_conn = curr_conn->next;
        }

        curr_conn->next = __create_new_conn(handle, conn);
    }
    else
    {
        conn_head = __create_new_conn(handle, conn);
    }

    return ret;
}

int ezlopi_core_ws_server_conn_remove_conn(struct netconn *conn)
{
    int ret = 0;

    if (conn == conn_head->conn)
    {
        netconn_delete(conn);
        
    }
    else
    {
        s_ws_server_new_connections_t *curr_conn = conn_head;
        while (curr_conn->next)
        {
            if (curr_conn->next == conn)
            {
                break;
            }

            curr_conn = curr_conn->next;
        }
    }

    return ret;
}

static s_ws_server_new_connections_t *__create_new_conn(TaskHandle_t handle, struct netconn *conn)
{
    s_ws_server_new_connections_t *new_ws_conn = malloc(sizeof(s_ws_server_new_connections_t));
    if (new_ws_conn)
    {
        memset(new_ws_conn, 0, sizeof(s_ws_server_new_connections_t));

        new_ws_conn->conn = conn;
        new_ws_conn->handle = handle;
        new_ws_conn->next = NULL;
    }

    return new_ws_conn;
}