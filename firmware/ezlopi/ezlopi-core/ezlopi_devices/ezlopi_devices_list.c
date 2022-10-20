#include "ezlopi_actions.h"
#include "ezlopi_devices_list.h"
#include "sensor_bme280.h"
#include "digital_io.h"

static const s_ezlopi_sensor_t sensor_list[] = {
#ifdef EZLOPI_SENSOR_0018_BME280
    {EZLOPI_SENSOR_0018_BME280, sensor_bme280},
#endif

#ifdef EZLOPI_SENSOR_0001_LED
    {EZLOPI_SENSOR_0001_LED, digital_io},
#endif

#ifdef EZLOPI_SENSOR_0002_RELAY
    {EZLOPI_SENSOR_0002_RELAY, digital_io},
#endif

#ifdef EZLOPI_SENSOR_0003_PLUG
    {EZLOPI_SENSOR_0003_PLUG, digital_io},
#endif
    {EZLOPI_SENSOR_NONE, NULL}, // Do not remove 'NULL' from list, because the element of the list is compared with NULL for termination of loop
};

const s_ezlopi_sensor_t *ezlopi_sensor_get_list(void)
{
    return sensor_list;
}
