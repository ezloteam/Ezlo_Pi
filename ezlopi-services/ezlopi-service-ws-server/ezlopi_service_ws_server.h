#ifndef __EZLOPI_SERVICE_WS_SERVER_H__
#define __EZLOPI_SERVICE_WS_SERVER_H__

#include <cJSON.h>
#include "ezlopi_service_ws_server_clients.h"

typedef enum e_ws_status
{
    WS_STATUS_RUNNING = 0,
    WS_STATUS_STOPPED,
} e_ws_status_t;

void ezlopi_service_ws_server_stop(void);
void ezlopi_service_ws_server_start(void);

e_ws_status_t ezlopi_service_ws_server_status(void);

int ezlopi_service_ws_server_send_cjson(cJSON *cj_data);

int ezlopi_service_ws_server_broadcast(char *data);
int ezlopi_service_ws_server_broadcast_cjson(cJSON *cj_data);

#endif // __EZLOPI_SERVICE_WS_SERVER_H__