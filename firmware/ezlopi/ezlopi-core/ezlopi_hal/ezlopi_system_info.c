#include "ezlopi_system_info.h"
#include "esp_attr.h"
#include "../../build/config/sdkconfig.h"

static RTC_SLOW_ATTR uint32_t boot_count;

void ezlopi_system_info_increment_boot_count(void)
{
    boot_count += 1;
}

uint32_t ezlopi_system_info_get_boot_count(void)
{
    return boot_count;
}

char *ezlopi_system_info_get_chip_name(void)
{
    return CONFIG_IDF_TARGET;
}