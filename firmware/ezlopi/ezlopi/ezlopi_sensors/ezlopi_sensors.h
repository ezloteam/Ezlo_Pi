#ifndef __SENSOR_COMMON_H__
#define __SENSOR_COMMON_H__

#include "ezlopi_actions.h"

typedef int (*f_sensor_call_t)(e_ezlopi_actions_t, void *arg);

void sensor_service_add_to_schedule(f_sensor_call_t sensor_call);
s_sensors_list_t *ezlopi_sensor_get_next_sensor(s_sensors_list_t *current_sensor);

#endif // __SENSOR_COMMON_H__