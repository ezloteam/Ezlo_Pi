#ifndef _EZLOPI_CORE_PING_H_
#define _EZLOPI_CORE_PING_H_

typedef enum e_ping_status
{
    EZLOPI_PING_STATUS_UNKNOWN = 0,
    EZLOPI_PING_STATUS_LIVE,
    EZLOPI_PING_STATUS_DISCONNECTED,
} e_ping_status_t;

void ezlopi_ping_init(void);
e_ping_status_t ezlopi_ping_get_internet_status(void);

#endif // _EZLOPI_CORE_PING_H_
