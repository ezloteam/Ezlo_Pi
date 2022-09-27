#include "ezlopi_actions.h"
#include "ezlopi_sensors.h"

// #include "sensor_bme280.h"

/*Sensors include here*/
extern int sensor_bme280(e_ezlopi_actions_t action, void *arg);

static const f_sensor_call_t sensor_list[] = {
    // #ifdef SENSOR_0010_BME280
    // sensor_bme280,
    // #endif
    NULL, // Do not remove 'NULL' from list, because the element of the list is compared with NULL for termination of loop
};

const f_sensor_call_t *ezlopi_sensor_get_list(void)
{
    return sensor_list;
}
