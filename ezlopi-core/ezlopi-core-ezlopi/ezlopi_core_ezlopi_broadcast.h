#ifndef __EZLOPI_CORE_EZLOPI_BROADCAST_H__
#define __EZLOPI_CORE_EZLOPI_BROADCAST_H__

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include "cjext.h"

typedef int (*f_broadcast_method_t)(char* data);

typedef struct l_broadcast_method
{
    char method_name[32];
    uint32_t fail_retry;
    f_broadcast_method_t func;
    struct l_broadcast_method* next;

} l_broadcast_method_t;

int ezlopi_core_ezlopi_broadcast_cjson(cJSON* cj_data);
int ezlopi_core_ezlopi_broadcast_add_to_queue(cJSON* cj_data);
void ezlopi_core_ezlopi_broadcast_remove_method(f_broadcast_method_t broadcast_method);

void ezlopi_core_ezlopi_broadcast_methods_set_queue(int (*func)(cJSON*));
l_broadcast_method_t* ezlopi_core_ezlopi_broadcast_method_add(f_broadcast_method_t broadcast_method, char* method_name, uint32_t retries);

void ezlopi_core_broadcast_log_cjson(cJSON* cj_log_data);

#endif // __EZLOPI_CORE_EZLOPI_BROADCAST_H__
