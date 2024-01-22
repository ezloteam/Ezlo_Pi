#ifndef _EZLOPI_HAL_SYSTEM_INFO_H_
#define _EZLOPI_HAL_SYSTEM_INFO_H_
#include "string.h"
#include "stdint.h"
#include "stdio.h"

char *ezlopi_system_info_get_chip_name(void);
uint32_t ezlopi_system_info_get_boot_count(void);

#endif // _EZLOPI_HAL_SYSTEM_INFO_H_
