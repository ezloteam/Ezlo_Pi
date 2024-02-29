#ifndef __EZLOPI_SERVICE_WS_SERVER_CLIENTS_H__
#define __EZLOPI_SERVICE_WS_SERVER_CLIENTS_H__

typedef struct l_ws_server_client_conn
{
    void *http_handle;
    int http_descriptor;
    uint32_t fail_count;

    struct l_ws_server_client_conn *next;
} l_ws_server_client_conn_t;

int ezlopi_service_ws_server_clients_remove_by_handle(void *http_handle);
l_ws_server_client_conn_t *ezlopi_service_ws_server_clients_get_head(void);
l_ws_server_client_conn_t *ezlopi_service_ws_server_clients_pop(void *http_handle);
l_ws_server_client_conn_t *ezlopi_service_ws_server_clients_get_by_handle(void *http_handle);
l_ws_server_client_conn_t *ezlopi_service_ws_server_clients_add(void *http_handle, int http_desc);

#endif // __EZLOPI_SERVICE_WS_SERVER_CLIENTS_H__