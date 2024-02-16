#include <lwip/api.h>
#include <lwip/err.h>
#include <lwip/udp.h>
#include <lwip/tcpip.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "ezlopi_util_trace.h"
#include "ezlopi_core_ws_server.h"
#include "ezlopi_core_ws_server_conn.h"

static TaskHandle_t gs_task_handle = NULL;

static void __server_init(void);
static void __server_process(void *pv);

void ezlopi_core_ws_server_deinit(void)
{
    if (gs_task_handle)
    {
        vTaskDelete(gs_task_handle);
    }
}

void ezlopi_core_ws_server_init(void)
{
    if (NULL == gs_task_handle)
    {
        xTaskCreate(__server_process, "ws-server-process", 2048, NULL, 4, &gs_task_handle);
    }
}

typedef struct s_ws_connections
{
    TaskHandle_t task_handle;
    struct netconn *conn;

    struct s_ws_connections *next;
} s_ws_connections_t;

static s_ws_connections_t *conn_head = NULL;

static void __connection_process(void *pv)
{
    struct netconn *conn = (struct netconn *)pv;

    while (1)
    {
        struct netbuf * net_buffer = NULL;
        err_t err = netconn_recv(conn, &net_buffer);
        
    }
}

static void __server_process(void *pv)
{
    struct netconn *listen_con = netconn_new(NETCONN_TCP);

    if (listen_con)
    {
        netconn_bind(listen_con, NULL, 80);
        err_t err = netconn_listen(listen_con);
        if (err)
        {
            while (1)
            {
                struct netconn *new_con = NULL;
                err = netconn_accept(listen_con, &new_con);
                TRACE_W("new-conn: %s", lwip_strerr(err));

                if ((ERR_OK == err) && new_con)
                {
                    TaskHandle_t conn_handle = NULL;
                    xTaskCreate(__connection_process, "ws-connection", 4 * 1024, new_con, 5, &conn_handle);
                    if (ezlopi_core_ws_server_conn_add_new_conn(conn_handle, new_con))
                    {
                        if (conn_handle)
                        {
                            vTaskDelete(conn_handle);
                        }

                        if (new_con)
                        {
                            netconn_delete(new_con);
                        }
                    }
                }
            }
        }
        else
        {
            netconn_delete(listen_con);
        }
    }

    gs_task_handle = NULL;
    vTaskDelete(NULL);
}