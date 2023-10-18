

#ifndef DEVICE_0038_OTHER_RGB_H
#define DEVICE_0038_OTHER_RGB_H

#include "ezlopi_actions.h"
#include "ezlopi_devices.h"
#include "ezlopi_pwm.h"

typedef struct
{
    s_ezlopi_pwm_t red_struct;
    s_ezlopi_pwm_t green_struct;
    s_ezlopi_pwm_t blue_struct;
    float brightness;
} device_0038_digitalOut_RGB_struct_t;

int device_0038_other_RGB(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#endif // DEVICE_0038_OTHER_RGB_H
