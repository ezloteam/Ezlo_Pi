#ifndef __DIGITAL_IO_H__
#define __DIGITAL_IO_H__

#include "ezlopi_actions.h"
#include "driver/gpio.h"

// int digital_io(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg);
// int digital_io(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg);
int digital_io_v3(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#endif // __DIGITAL_IO_H__
