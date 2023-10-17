

#ifndef _025_SENS_LDR_DIGITAL_MODULE_H_
#define _025_SENS_LDR_DIGITAL_MODULE_H_

#include "ezlopi_actions.h"
#include "ezlopi_devices.h"

int sensor_ldr_digital_module_v3(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void **user_arg);
#if 0 // v2.x 
int sensor_ldr_digital_module(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *ezlo_device, void *arg, void *user_arg);
#endif

#endif //_025_SENS_LDR_DIGITAL_MODULE_H_
