

#ifndef _EZLOPI_SERVICE_URI_H_
#define _EZLOPI_SERVICE_URI_H_

#include "esp_err.h"
#include "esp_http_server.h"

void ezlopi_begin_ap_server_service();
void ezlopi_end_ap_server_service();
int ezlopi_get_wifi_cred(char *wifi_cred);

#endif // _EZLOPI_SERVICE_URI_H_
