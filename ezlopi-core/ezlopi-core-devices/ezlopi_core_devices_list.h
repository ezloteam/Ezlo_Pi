#ifndef _EZLOPI_CORE_DEVICES_LIST_H_
#define _EZLOPI_CORE_DEVICES_LIST_H_

#include "ezlopi_core_devices.h"
#include "ezlopi_core_errors.h"

typedef ezlopi_error_t (*f_sensor_call_v3_t)(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

typedef struct s_ezlopi_device_v3
{
    char *name;
    uint32_t id;
    f_sensor_call_v3_t func;
} s_ezlopi_device_v3_t;

typedef struct s_ezlopi_prep_arg
{
    cJSON *cjson_device;
    s_ezlopi_device_v3_t *device;
} s_ezlopi_prep_arg_t;

s_ezlopi_device_v3_t *ezlopi_devices_list_get_list_v3(void);

#endif // _EZLOPI_CORE_DEVICES_LIST_H_
