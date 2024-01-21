#ifndef __DEVICE_0001_DIGITALOUT_GENERIC_H__
#define __DEVICE_0001_DIGITALOUT_GENERIC_H__

#include "ezlopi_core_actions.h"
#include "driver/gpio.h"

typedef struct s_digio_settings
{
    int32_t settings_int_data;
} s_digio_settings_t;

int device_0001_digitalOut_generic(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#endif // __DEVICE_0001_DIGITALOUT_GENERIC_H__
