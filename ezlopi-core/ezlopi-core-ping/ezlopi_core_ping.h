#ifndef _EZLOPI_CORE_PING_H_
#define _EZLOPI_CORE_PING_H_



typedef enum e_ping_status
{
    EZLOPI_PING_STATUS_UNKNOWN = 0,
    EZLOPI_PING_STATUS_LIVE,
    EZLOPI_PING_STATUS_DISCONNECTED,
} e_ping_status_t;

#include "../../build/config/sdkconfig.h"
#ifdef CONFIG_EZPI_ENABLE_PING

void ezlopi_ping_init(void);
e_ping_status_t ezlopi_ping_get_internet_status(void);

#endif // CONFIG_EZPI_ENABLE_PING

#endif // _EZLOPI_CORE_PING_H_
