#ifndef __NVS_STORAGE_H__
#define __NVS_STORAGE_H__

#include "cJSON.h"
#include "nvs_flash.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void ezlopi_ota_start(cJSON *cj_url);

#ifdef __cplusplus
}
#endif

#endif // __NVS_STORAGE_H__