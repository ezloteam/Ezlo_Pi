#include "sdkconfig.h"
#if defined(CONFIG_EZPI_ENABLE_SYSTEM_TEMPERATURE)

#ifndef _EZLOPI_SERVICE_SYSTEM_TEMPERATURE_SENSOR_H_
#define _EZLOPI_SERVICE_SYSTEM_TEMPERATURE_SENSOR_H_

int ezlopi_system_temperature_device(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#endif // _EZLOPI_SERVICE_SYSTEM_TEMPERATURE_SENSOR_H_


#endif // CONFIG_EZPI_ENABLE_SYSTEM_TEMPERATURE

