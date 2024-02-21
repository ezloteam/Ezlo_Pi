#ifndef __EZLOPI_CORE_WS_SERVER_CONN_H__
#define __EZLOPI_CORE_WS_SERVER_CONN_H__

#include <lwip/api.h>
#include <lwip/err.h>
#include <lwip/tcpip.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "ezlopi_util_uuid.h"

typedef struct s_ws_server_connections
{
    ezlopi_uuid_t uuid;
    TaskHandle_t handle;
    struct netconn *conn;

    struct s_ws_server_connections *next;
} s_ws_server_connections_t;

s_ws_server_connections_t *ezlopi_core_ws_server_conn_get_head(void);
s_ws_server_connections_t *ezlopi_core_ws_server_conn_add_ws_conn(struct netconn *conn);

#endif // __EZLOPI_CORE_WS_SERVER_CONN_H__
