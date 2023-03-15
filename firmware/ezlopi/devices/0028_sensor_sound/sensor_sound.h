

#ifndef __SENSOR_SOUND_H__
#define __SENSOR_SOUND_H__

#include "ezlopi_actions.h"
#include "ezlopi_devices.h"
#include "ezlopi_uart.h"

int sound_sensor(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *ezlo_device, void *arg, void *user_arg);

#endif //__SENSOR_SOUND_H__
