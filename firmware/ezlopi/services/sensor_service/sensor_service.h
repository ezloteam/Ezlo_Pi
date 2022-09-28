#ifndef __SENSOR_SERVICE_H__
#define __SENSOR_SERVICE_H__
#include "ezlopi_sensors.h"

/**
 * @brief
 *
 */
typedef struct s_ezlo_event
{
    e_ezlopi_actions_t action;
    void *arg;
} s_ezlo_event_t;

/**
 * @brief
 *
 */
void sensor_service_init(void);

/**
 * @brief
 *
 * @param event
 * @param from_isr
 * @return int
 */
int sensor_service_add_event_to_queue(s_ezlo_event_t *event, int from_isr);

#endif // __SENSOR_SERVICE_H__