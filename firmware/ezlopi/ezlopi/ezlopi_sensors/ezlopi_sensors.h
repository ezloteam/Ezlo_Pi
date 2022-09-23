#ifndef __SENSOR_COMMON_H__
#define __SENSOR_COMMON_H__

#include "ezlopi_actions.h"
/**
 * @brief defining the type of sensor call function
 *
 */
typedef int (*f_sensor_call_t)(e_ezlopi_actions_t, void *arg);

/**
 * @brief
 *
 */
typedef int (*f_sensor_list_t)(e_ezlopi_actions_t, void *arg);

/**
 * @brief linked list structure defined to hold the registered sensors
 *
 */
typedef struct s_sensors_list
{
    struct s_sensors_list *next;
    f_sensor_call_t call;
} s_sensors_list_t;

/**
 * @brief 'sensor_service_add_to_list' adds the sensor to the list
 *
 * @param sensor_call main function for the particular function
 */
void sensor_service_add_to_list(f_sensor_call_t call_func);

/**
 * @brief 'ezlopi_sensor_get_head' fetch the head of the registered-sensor list
 *
 * @return s_sensors_list_t* return the head of the registered-sensor list
 */
s_sensors_list_t *ezlopi_sensor_get_head(void);

#if 0
/**
 * @brief 'ezlopi_sensor_get_next'
 *
 * @param current_sensor
 * @return s_sensors_list_t*
 */
s_sensors_list_t *ezlopi_sensor_get_next(s_sensors_list_t *current_sensor);
#endif

#endif // __SENSOR_COMMON_H__