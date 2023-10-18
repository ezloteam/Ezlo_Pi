#ifndef __DIMMABLE_BULB_H__
#define __DIMMABLE_BULB_H__

#include "ezlopi_actions.h"
#include "ezlopi_devices.h"

typedef struct ezlopi_dimmable_bulb_state_struct{
    uint32_t previous_brightness_value;
    uint32_t current_brightness_value;
}ezlopi_dimmable_bulb_state_struct_t;

// int ezlopi_dimmable_bulb(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *ezlo_device, void *arg, void *user_arg);
int ezlopi_dimmable_bulb_v3(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg);


#endif // __DIMMABLE_BULB_H__