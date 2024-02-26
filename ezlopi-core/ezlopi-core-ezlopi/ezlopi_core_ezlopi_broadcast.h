#ifndef __EZLOPI_CORE_EZLOPI_BROADCAST_H__
#define __EZLOPI_CORE_EZLOPI_BROADCAST_H__

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

typedef int (*f_broadcast_method_t)(char *data);

typedef struct l_broadcast_method
{
    char *method_name;
    uint32_t fail_retry;
    f_broadcast_method_t func;
    struct l_broadcast_method *next;

} l_broadcast_method_t;

void ezlopi_core_ezlopi_broadcast_execute(char *data);
l_broadcast_method_t *ezlopi_core_ezlopi_broadcast_method_add(f_broadcast_method_t broadcast_method, uint32_t retries);

#endif // __EZLOPI_CORE_EZLOPI_BROADCAST_H__
