#ifndef __EZLOPI_SYSTEM_INFO_H__
#define __EZLOPI_SYSTEM_INFO_H__
#include "string.h"
#include "stdint.h"
#include "stdio.h"

char *ezlopi_system_info_get_chip_name(void);
uint32_t ezlopi_system_info_get_boot_count(void);

#endif // __EZLOPI_SYSTEM_INFO_H__
