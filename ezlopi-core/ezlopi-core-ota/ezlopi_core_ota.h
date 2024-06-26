#ifndef _EZLOPI_CORE_OTA_H_
#define _EZLOPI_CORE_OTA_H_

#ifdef CONFIG_EZPI_ENABLE_OTA

#include "cjext.h"
#include "nvs_flash.h"

#ifdef __cplusplus
extern "C"
{
#endif


    uint32_t __get_ota_state(void);
    void ezlopi_ota_start(cJSON* cj_url);


#ifdef __cplusplus
}
#endif

#endif // CONFIG_EZPI_ENABLE_OTA

#endif // _EZLOPI_CORE_OTA_H_