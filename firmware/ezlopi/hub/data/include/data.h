#ifndef __HUB_DATA_LIST_H__
#define __HUB_DATA_LIST_H__

#include <string.h>
#include "frozen.h"

#ifdef __cplusplus
extern "C"
{
#endif

    char *data_list(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count);

#ifdef __cplusplus
}
#endif
#endif // __HUB_DATA_LIST_H__