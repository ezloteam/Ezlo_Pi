#ifndef _EZLOPI_SERVICE_WEBPROV_H_
#define _EZLOPI_SERVICE_WEBPROV_H_

#include <string.h>

#include "cjext.h"
#include "EZLOPI_USER_CONFIG.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(CONFIG_EZPI_WEBSOCKET_CLIENT)
    void ezlopi_service_web_provisioning_init(void);
    void ezlopi_service_web_provisioning_deinit(void);
    uint32_t ezlopi_service_web_provisioning_get_message_count(void);
#endif // CONFIG_EZPI_WEBSOCKET_CLIENT

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_SERVICE_WEBPROV_H_