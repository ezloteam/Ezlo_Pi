#ifndef __HUB_REGISTERATION_H__
#define __HUB_REGISTERATION_H__

#include <string.h>

#include "cjext.h"

#ifdef __cplusplus
extern "C"
{
#endif
    void registration_init(void);
    void register_repeat(cJSON* cj_request, cJSON* cj_response);
    void registered(cJSON* cj_request, cJSON* cj_response);
    void ezpi_cloud_dummy(void);
#ifdef __cplusplus
}
#endif

#endif // __HUB_REGISTERATION_H__