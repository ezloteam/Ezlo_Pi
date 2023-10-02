

#ifndef __DEVICE_0038_DIGITAL_OUT_RGB_H__
#define __DEVICE_0038_DIGITAL_OUT_RGB_H__


#include "ezlopi_actions.h"
#include "ezlopi_devices.h"
#include "ezlopi_pwm.h"

typedef struct {
    s_ezlopi_pwm_t red_struct;
    s_ezlopi_pwm_t green_struct;
    s_ezlopi_pwm_t blue_struct;
    float brightness;
}device_0038_digitalOut_RGB_struct_t;

int device_0038_digitalOut_RGB(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg);


#endif // __DEVICE_0038_DIGITAL_OUT_RGB_H__

