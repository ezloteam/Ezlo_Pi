#ifndef _ULTRASONIC_HC_SR04_H_
#define _ULTRASONIC_HC_SR04_H_

#include "ezlopi_actions.h"
#include "ezlopi_devices.h"
#include "ezlopi_uart.h"

int ultrasonic_HC_SR04(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *ezlo_device, void *arg, void *user_arg);

#endif //_ULTRASONIC_HC_SR04_H_