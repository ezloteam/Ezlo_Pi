#ifndef _DEVICE_0022_PWM_DIMMABLE_BULB_H_
#define _DEVICE_0022_PWM_DIMMABLE_BULB_H_

#include "ezlopi_core_actions.h"
#include "ezlopi_core_devices.h"

typedef struct ezlopi_dimmable_bulb_state_struct
{
    uint32_t previous_brightness_value;
    uint32_t current_brightness_value;
} ezlopi_dimmable_bulb_state_struct_t;

int device_0022_PWM_dimmable_lamp(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#endif // __DIMMABLE_BULB_H__