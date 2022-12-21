#ifndef __HUB_ROOM_H__
#define __HUB_ROOM_H__
#include <string.h>
#include "frozen.h"
#include "cJSON.h"

#ifdef __cplusplus
extern "C"
{
#endif
    void room_list(cJSON *cj_request, cJSON *cj_response);

#ifdef __cplusplus
}
#endif

#endif // __HUB_ROOM_H__