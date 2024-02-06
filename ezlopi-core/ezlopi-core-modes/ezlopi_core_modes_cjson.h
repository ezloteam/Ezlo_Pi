#ifndef __EZLOPI_CORE_MODES_CJSON_H__
#define __EZLOPI_CORE_MODES_CJSON_H__

#include <cJSON.h>
#include <string.h>
#include <stdint.h>

int ezlopi_core_modes_cjson_get_modes(cJSON *cj_dest);
int ezlopi_core_modes_cjson_get_current_mode(cJSON *cj_dest);

#endif // __EZLOPI_CORE_MODES_CJSON_H__