#ifndef __EZLOPI_CORE_API_H__
#define __EZLOPI_CORE_API_H__
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "cjext.h"

cJSON *ezlopi_core_api_consume_cjson(const char *who, cJSON *cj_request);
cJSON* ezlopi_core_api_consume(const char * who, const char* payload, uint32_t len);

#endif // __EZLOPI_CORE_API_H__
