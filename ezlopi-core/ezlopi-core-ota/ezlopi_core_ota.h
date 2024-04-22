#ifndef _EZLOPI_CORE_OTA_H_
#define _EZLOPI_CORE_OTA_H_

#include "cjext.h"
#include "nvs_flash.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(CONFIG_EZPI_ENABLE_OTA)
    uint32_t __get_ota_state(void);
    void ezlopi_ota_start(cJSON* cj_url);
#endif

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_CORE_OTA_H_