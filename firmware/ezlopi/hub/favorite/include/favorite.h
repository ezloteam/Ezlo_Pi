#ifndef __HUB_FAVORITE_LIST_H__
#define __HUB_FAVORITE_LIST_H__

#include <string.h>
#include "frozen.h"

#ifdef __cplusplus
extern "C"
{
#endif

    char *favorite_list(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);

#ifdef __cplusplus
}
#endif

#endif // __HUB_FAVORITE_LIST_H__