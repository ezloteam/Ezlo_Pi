#ifndef __EZLOPI_CORE_EZLOPI_METHODS_H__
#define __EZLOPI_CORE_EZLOPI_METHODS_H__

#include <stdio.h>
#include <ctype.h>
#include <cJSON.h>
#include <string.h>
#include <stdint.h>

typedef void (*f_method_func_t)(cJSON *cj_request, cJSON *cj_response);
typedef struct s_method_list_v2
{
    char *method_name;
    f_method_func_t method;
    f_method_func_t updater;
} s_method_list_v2_t;

void ezlopi_core_ezlopi_methods_registration_init(void);
bool ezlopi_core_elzlopi_methods_check_method_register(f_method_func_t method);

uint32_t ezlopi_core_ezlopi_methods_search_in_list(cJSON *cj_method);

char *ezlopi_core_ezlopi_methods_get_name_by_id(uint32_t method_id);
f_method_func_t ezlopi_core_ezlopi_methods_get_by_id(uint32_t method_id);
f_method_func_t ezlopi_core_ezlopi_methods_get_updater_by_id(uint32_t method_id);

void ezlopi_core_ezlopi_methods_reboot(cJSON *cj_request, cJSON *cj_response);
void ezlopi_core_ezlopi_methods_rpc_method_notfound(cJSON *cj_request, cJSON *cj_response);

#endif // __EZLOPI_CORE_EZLOPI_METHODS_H__