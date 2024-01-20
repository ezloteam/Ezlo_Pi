#ifndef __HUB_INFO_H__
#define __HUB_INFO_H__
#include "string.h"

#include "cJSON.h"

#ifdef __cplusplus
extern "C"
{
#endif

    char *ezlopi_tick_to_time(uint32_t ms);
    void info_get(cJSON *cj_request, cJSON *cj_response);

#ifdef __cplusplus
}
#endif

#endif // __HUB_INFO_H__