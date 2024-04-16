#ifndef __EZLOPI_CORE_MODES_CJSON_H__
#define __EZLOPI_CORE_MODES_CJSON_H__

#include "cjext.h"
#include <string.h>
#include <stdint.h>

#include "ezlopi_core_modes.h"

#if defined(CONFIG_EZLPI_SERV_ENABLE_MODES)

int ezlopi_core_modes_cjson_get_modes(cJSON* cj_dest);
int ezlopi_core_modes_cjson_get_current_mode(cJSON* cj_dest);
s_ezlopi_modes_t* ezlopi_core_modes_cjson_parse_modes(cJSON* cj_modes);

#endif

#endif // __EZLOPI_CORE_MODES_CJSON_H__