#ifndef __DIGITAL_IO_H__
#define __DIGITAL_IO_H__

#include "ezlopi_actions.h"
#include "driver/gpio.h"

int digital_io(e_ezlopi_actions_t action, void *arg);

#endif // __DIGITAL_IO_H__
