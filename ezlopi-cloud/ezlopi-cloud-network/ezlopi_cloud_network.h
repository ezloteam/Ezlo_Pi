#ifndef __NETWORK_H__
#define __NETWORK_H__

#include <string.h>

#include "cjext.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void network_get(cJSON* cj_request, cJSON* cj_response);
    void network_wifi_scan_start(cJSON* cj_request, cJSON* cj_response);
    void network_wifi_scan_stop(cJSON* cj_request, cJSON* cj_response);
    void network_wifi_try_connect(cJSON* cj_request, cJSON* cj_response);

#ifdef __cplusplus
}
#endif
#endif //__NETWORK_H__
