#ifndef __HUB_ROOM_H__
#define __HUB_ROOM_H__
#include <string.h>
#include "frozen.h"
#include "cJSON.h"

#ifdef __cplusplus
extern "C"
{
#endif
    cJSON *room_list(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);

#ifdef __cplusplus
}
#endif

#endif // __HUB_ROOM_H__