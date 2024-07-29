#ifndef _DEVICE_0001_DIGITALOUT_GENERIC_H_
#define _DEVICE_0001_DIGITALOUT_GENERIC_H_

#include "ezlopi_core_device_macros.h"

#if defined(DEVICE_0001_DIGITALOUT_GENERIC)

#include "ezlopi_core_actions.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_errors.h"

typedef struct s_digio_settings
{
    int32_t settings_int_data;
} s_digio_settings_t;

ezlopi_error_t device_0001_digitalOut_generic(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#endif

#endif // _DEVICE_0001_DIGITALOUT_GENERIC_H_
