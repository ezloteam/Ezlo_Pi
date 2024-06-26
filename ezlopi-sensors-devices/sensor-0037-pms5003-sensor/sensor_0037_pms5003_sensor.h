

#ifndef _SENSOR_PMS5003_UART_H_
#define _SENSOR_PMS5003_UART_H_

#include "ezlopi_core_actions.h"
#include "ezlopi_core_devices.h"

#include "pms5003.h"

int sensor_0037_pms5003_v3(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);
int pms5003_sensor_preapre_devices_and_items(cJSON *cj_properties, uint32_t *parent_id);

#endif // _SENSOR_PMS5003_UART_H_
