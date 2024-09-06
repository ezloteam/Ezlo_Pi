

#ifndef __EZLOPI_CLOUD_SETTING_COMMANDS_H__
#define __EZLOPI_CLOUD_SETTING_COMMANDS_H__

#include "cjext.h"

void setting_value_set(cJSON* cj_request, cJSON* cj_response);
void settings_list(cJSON* cj_request, cJSON* cj_response);

#endif // __EZLOPI_CLOUD_SETTING_COMMANDS_H__
