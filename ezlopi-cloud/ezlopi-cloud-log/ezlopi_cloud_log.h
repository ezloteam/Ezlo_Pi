

#ifndef _EZLOPI_CLOUD_LOG_H_
#define _EZLOPI_CLOUD_LOG_H_

#include "cjext.h"


void ezlopi_hub_log_set(cJSON* cj_request, cJSON* cj_response);
void ezlopi_hub_log_set_updater(cJSON* cj_request, cJSON* cj_response);
// void ezlopi_hub_log_broadcast();

#endif // _EZLOPI_CLOUD_LOG_H_

