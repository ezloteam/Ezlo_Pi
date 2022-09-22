#ifndef __SENSOR_SERVICE_H__
#define __SENSOR_SERVICE_H__

void sensor_service_add_to_list(void (*sensor_call)(e_ezlopi_actions_t, void *arg));

#endif // __SENSOR_SERVICE_H__