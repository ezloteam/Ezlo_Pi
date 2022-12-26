#ifndef __HUB_INFO_H__
#define __HUB_INFO_H__
#include "string.h"
#include "frozen.h"
#include "cJSON.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void info_get(cJSON *cj_request, cJSON *cj_response);

#ifdef __cplusplus
}
#endif

#endif // __HUB_INFO_H__