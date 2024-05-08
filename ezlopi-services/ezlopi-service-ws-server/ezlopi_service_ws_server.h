#ifndef __EZLOPI_SERVICE_WS_SERVER_H__
#define __EZLOPI_SERVICE_WS_SERVER_H__

#include "cjext.h"
#include "ezlopi_service_ws_server_clients.h"

#if defined(CONFIG_EZPI_LOCAL_WEBSOCKET_SERVER)

typedef enum e_ws_status
{
    WS_STATUS_RUNNING = 0,
    WS_STATUS_STARTED,
    WS_STATUS_STOPPED,
} e_ws_status_t;

void ezlopi_service_ws_server_stop(void);
void ezlopi_service_ws_server_start(void);

e_ws_status_t ezlopi_service_ws_server_status(void);



#else // CONFIG_EZPI_LOCAL_WEBSOCKET_SERVER

void ezlpi_service_ws_server_dummy(void);

#endif // CONFIG_EZPI_LOCAL_WEBSOCKET_SERVER

#endif // __EZLOPI_SERVICE_WS_SERVER_H__