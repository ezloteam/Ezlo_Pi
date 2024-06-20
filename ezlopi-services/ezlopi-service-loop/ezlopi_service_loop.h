#ifndef _EZLOPI_SERVICE_TIMER_H_
#define _EZLOPI_SERVICE_TIMER_H_

#include "string.h"
#include "stdint.h"

typedef void (*f_loop_t)(void);

void ezlopi_service_loop_init(void);
void ezlopi_service_loop_add(const char * name, f_loop_t loop, uint32_t period_ms);

#endif // _EZLOPI_SERVICE_TIMER_H_