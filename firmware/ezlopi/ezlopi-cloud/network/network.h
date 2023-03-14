#ifndef __NETWORK_H__
#define __NETWORK_H__

#include <string.h>
#include "frozen.h"
#include "cJSON.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void network_get(cJSON *cj_request, cJSON *cj_response);

#ifdef __cplusplus
}
#endif
#endif //__NETWORK_H__
