// #ifndef __SENSOR_COMMON_H__
// #define __SENSOR_COMMON_H__

// #include "ezlopi_actions.h"

// #define EZLOPI_SENSOR_NONE 0
// #define EZLOPI_SENSOR_0018_BME280 18

// /**
//  * @brief defining the type of sensor call function
//  *
//  */
// // int sensor_bme280(e_ezlopi_actions_t action, void *arg);
// typedef int (*f_sensor_call_t)(e_ezlopi_actions_t action, void *arg);
// typedef struct s_sensor_id
// {
//     uint16_t id;
//     f_sensor_call_t func;
// } s_ezlopi_device_t;

// /**
//  * @brief Provides the list of available sensors
//  *
//  * @return f_sensor_call_t*
//  */
// const s_ezlopi_device_t *ezlopi_sensor_get_list(void);

// #if 0
// /**
//  * @brief 'ezlopi_sensor_get_next'
//  *
//  * @param current_sensor
//  * @return s_sensors_list_t*
//  */
// s_sensors_list_t *ezlopi_sensor_get_next(s_sensors_list_t *current_sensor);
// #endif

// #endif // __SENSOR_COMMON_H__