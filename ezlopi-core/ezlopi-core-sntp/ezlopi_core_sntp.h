#ifndef _EZLOPI_SNTP_H_
#define _EZLOPI_SNTP_H_

#include <time.h>
#include <stdint.h>

void ezlopi_core_sntp_init(void);
time_t ezlopi_core_sntp_cget_up_time(void);
uint64_t ezlopi_sntp_core_get_current_time_ms(void);

#endif // _EZLOPI_SNTP_H_
