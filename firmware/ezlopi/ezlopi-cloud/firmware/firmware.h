#ifndef __HUB_DATA_LIST_H__
#define __HUB_DATA_LIST_H__

#include <string.h>
#include "frozen.h"
#include "cJSON.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void data_list(cJSON *cj_request, cJSON *cj_response);

#ifdef __cplusplus
}
#endif
#endif // __HUB_DATA_LIST_H__