#include <string.h>

#include <lwip/api.h>
#include <lwip/err.h>
#include <lwip/udp.h>
#include <lwip/tcpip.h>
#include <mbedtls/sha1.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "ezlopi_util_uuid.h"
#include "ezlopi_util_trace.h"

#include "ezlopi_core_ws_server.h"
#include "ezlopi_core_ws_server_conn.h"
#include "ezlopi_core_ws_server_config.h"

static const char *gsc_sec_key_name = "Sec-WebSocket-Key:";
static TaskHandle_t gs_task_handle = NULL;

static void __server_init(void);
static void __server_process(void *pv);

void ezlopi_core_ws_server_deinit(void)
{
    if (gs_task_handle)
    {
        vTaskDelete(gs_task_handle);
    }

    s_ws_server_connections_t *curr_con = ezlopi_core_ws_server_conn_get_head();
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
    ezlopi_uuid_t server_uuid;
    ezlopi_uuid_t client_uuid;
    TaskHandle_t task_handle;
    struct netconn *conn;

    struct s_ws_connections *next;
} s_ws_connections_t;

static s_ws_connections_t *conn_head = NULL;

static void __connection_process(void *pv)
{
    s_ws_connections_t *ws_conn = (s_ws_connections_t *)pv;

    if (ws_conn)
    {
        if (ws_conn->conn)
        {
            while (1)
            {
                TRACE_D("here");
                struct netbuf *net_buffer = NULL;
                err_t err = netconn_recv(ws_conn->conn, &net_buffer);
                TRACE_D("here");

                if ((ERR_OK == err) && net_buffer)
                {
                    char *data_str = NULL;
                    uint32_t data_len = 0;

                    netbuf_data(net_buffer, &data_str, &data_len);

                    TRACE_D("ws-rx-data: %s", data_str);
                }

                vTaskDelay(100 / portTICK_PERIOD_MS);
            }

            netconn_delete(ws_conn->conn);
            ws_conn->conn = NULL;
            ws_conn->task_handle = NULL;
        }
    }
    else
    {
        TRACE_E("ws-conn is NULL");
    }

    vTaskDelete(NULL);
}

static void __server_process(void *pv)
{
    struct netconn *listen_con = netconn_new(NETCONN_TCP);

    if (listen_con)
    {
        netconn_bind(listen_con, IP4_ADDR_ANY, 80);
        err_t err = netconn_listen(listen_con);
        if (ERR_OK == err)
        {
            while (1)
            {
                struct netconn *new_con = NULL;

                TRACE_D("waiting for new connection...");
                err = netconn_accept(listen_con, &new_con);
                if ((ERR_OK == err) && new_con)
                {
                    s_ws_server_connections_t *ws_conn = ezlopi_core_ws_server_conn_add_ws_conn(new_con);
                    if (ws_conn)
                    {
                        struct netbuf *tmp_net_buf = NULL;
                        err_t err = netconn_recv(new_con, &tmp_net_buf);

                        TRACE_W("error: %s", lwip_strerr(err));

                        if ( tmp_net_buf)
                        {
                            char *data = NULL;
                            uint32_t data_len = 0;

                            netbuf_data(tmp_net_buf, &data, &data_len);

                            if (data)
                            {
                                TRACE_W("data: %s", data);
                            }
                        }

                        xTaskCreate(__connection_process, "ws-connection", 4 * 1024, ws_conn, 5, &ws_conn->handle);
                    }
                    else
                    {
                        netconn_delete(new_con);
                    }
                }
            }
        }
        else
        {
            TRACE_E("error[%d]: %s", err, lwip_strerr(err));
            netconn_delete(listen_con);
            TRACE_E("failed");
        }
    }
    else
    {
        TRACE_E("FAILED");
    }

    gs_task_handle = NULL;
    vTaskDelete(NULL);
}

static int __new_connection_handsake(s_ws_connections_t *ws_conn)
{
    int ret = 0;

    if (ws_conn && ws_conn->conn)
    {
        struct netbuf *net_buffer = NULL;
        err_t err = netconn_recv(ws_conn->conn, &net_buffer);

        if (err && net_buffer)
        {
            char *net_buf_data = NULL;
            uint32_t data_size = 0;

            netbuf_data(net_buffer, net_buf_data, data_size);

            if (net_buf_data)
            {
                TRACE_D("handsake-data: %s", net_buf_data);

                char *client_sec_key = strstr(net_buf_data, gsc_sec_key_name);
                if (client_sec_key)
                {
                    char concated_strings[80];
                    client_sec_key += strlen(gsc_sec_key_name) + 1;
                    uint32_t client_sec_key_len = strstr(client_sec_key, "\r\n") - (uint32_t)client_sec_key;

                    ezlopi_util_uuid_generate_random(ws_conn->server_uuid);
                    uint32_t tmp_len = snprintf(concated_strings, sizeof(concated_strings), "%.*s", client_sec_key_len, client_sec_key);
                    ezlopi_util_uuid_unparse(ws_conn->server_uuid, concated_strings + tmp_len);

                    // esp_sha(SHA1, )
                }
            }
        }
    }
    return ret;
}