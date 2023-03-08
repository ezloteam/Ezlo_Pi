#ifndef __HUB_FIRMWARE_H__
#define __HUB_FIRMWARE_H__

#include <string.h>
#include "frozen.h"
#include "cJSON.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void firmware_update(cJSON *cj_request, cJSON *cj_response);

#ifdef __cplusplus
}
#endif
#endif // __HUB_FIRMWARE_H__