#ifndef _EZLOPI_SERVICE_WEBPROV_H_
#define _EZLOPI_SERVICE_WEBPROV_H_

#include <string.h>

#include "cJSON.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void ezlopi_service_web_provisioning_init(void);
    void ezlopi_service_web_provisioning_deinit(void);
    uint32_t ezlopi_service_web_provisioning_get_message_count(void);

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_SERVICE_WEBPROV_H_