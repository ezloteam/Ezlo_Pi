#ifndef __EZLOPI_CORE_WS_SERVER_CONN_H__
#define __EZLOPI_CORE_WS_SERVER_CONN_H__

#include <lwip/api.h>
#include <lwip/err.h>
#include <lwip/tcpip.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

typedef struct s_ws_server_new_connections
{
    TaskHandle_t handle;
    struct netconn *conn;

    struct s_ws_server_new_connections *next;
} s_ws_server_new_connections_t;

int ezlopi_core_ws_server_conn_add_new_conn(TaskHandle_t handle, struct netconn *conn);

#endif // __EZLOPI_CORE_WS_SERVER_CONN_H__
