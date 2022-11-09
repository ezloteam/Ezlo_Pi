#ifndef __HUB_FEATURE_H__
#define __HUB_FEATURE_H__

#include <string.h>
#include "frozen.h"

#ifdef __cplusplus
extern "C"
{
#endif

    char *feature_list(const char *payload, uint32_t len, struct json_token *method);

#ifdef __cplusplus
}
#endif

#endif // __HUB_FEATURE_H__