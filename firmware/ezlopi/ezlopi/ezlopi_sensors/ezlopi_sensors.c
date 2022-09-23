#include "ezlopi_actions.h"
#include "ezlopi_sensors.h"

/*Sensors include here*/
#include "sensor_bme280.h"

static f_sensor_list_t sensor_call_list[] = {
#ifdef SENSOR_0010_BME280
    sensor_bme280,
#endif
    NULL,
};

f_sensor_list_t *ezlopi_sensor_get_list(void)
{
    return sensor_call_list;
}

static s_sensors_list_t *sensor_list_head = NULL;

void sensor_service_add_to_list(f_sensor_call_t call_func)
{
    if (sensor_list_head)
    {
        s_sensors_list_t *sensor = sensor_list_head;

        while (sensor->next)
        {
            sensor = sensor->next;
        }

        sensor->next = malloc(sizeof(s_sensors_list_t));
        if (sensor->next)
        {
            sensor->next->call = call_func;
            sensor->next->next = NULL;
        }
    }
    else
    {
        sensor_list_head = malloc(sizeof(s_sensors_list_t));
        if (sensor_list_head)
        {
            sensor_list_head->call = call_func;
            sensor_list_head->next = NULL;
        }
    }
}

s_sensors_list_t *ezlopi_sensor_get_head(void)
{
    return sensor_list_head;
}

#if 0
s_sensors_list_t *ezlopi_sensor_get_next(s_sensors_list_t *current_sensor)
{
    s_sensors_list_t *ret = NULL;
    if (current_sensor)
    {
        ret = current_sensor->next;
    }
    return ret;
}
#endif
