#ifndef __NETWORK_H__
#define __NETWORK_H__

#include <string.h>
#include "frozen.h"
#include "cJSON.h"

#ifdef __cplusplus
extern "C"
{
#endif

    cJSON *network_get(const char *data, uint32_t len, struct json_token *method_tok, uint32_t msg_count);

#ifdef __cplusplus
}
#endif
#endif //__NETWORK_H__
