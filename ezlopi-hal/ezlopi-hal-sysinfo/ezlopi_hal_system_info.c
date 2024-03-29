#include "../../build/config/sdkconfig.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_hal_system_info.h"

static uint32_t boot_count = 0;

uint32_t ezlopi_system_info_get_boot_count(void)
{
    if (0 == boot_count)
    {
        boot_count = ezlopi_nvs_get_boot_count();
    }

    return boot_count;
}

char *ezlopi_system_info_get_chip_name(void)
{
    return CONFIG_IDF_TARGET;
}