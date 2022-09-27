#include "ezlopi_actions.h"
#include "ezlopi_sensors.h"

#include "sensor_bme280.h"

/*Sensors include here*/

static int (*sensor_list[])(e_ezlopi_actions_t action, void *arg) = {
    // #ifdef SENSOR_0010_BME280
    // &sensor_bme280,
    sensor_bme280,
    // #endif
    NULL, // Do not remove 'NULL' from list, because the element of the list is compared with NULL for termination of loop
};

const f_sensor_call_t *ezlopi_sensor_get_list(void)
{
    sensor_bme280(EZLOPI_ACTION_GET_VALUE, NULL);
    return sensor_list;
}
