

#ifndef SENSOR_0024_OTHER_HCSR04_H
#define SENSOR_0024_OTHER_HCSR04_H

#include "ezlopi_actions.h"
#include "ezlopi_devices.h"
#include "ezlopi_uart.h"

#include <driver/gpio.h>
#include <esp_err.h>

int sensor_0024_other_HCSR04(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#endif // SENSOR_0024_OTHER_HCSR04_H
