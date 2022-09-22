#include "ezlopi_actions.h"
#include "ezlopi_sensors.h"

typedef struct s_sensors_list
{
    struct s_sensors_list *next;
    f_sensor_call_t sensor_call;
} s_sensors_list_t;

static s_sensors_list_t *sensor_schedule_head = NULL;

void sensor_service_add_to_schedule(f_sensor_call_t sensor_call)
{
    if (sensor_schedule_head)
    {
        s_sensors_list_t *sensor = sensor_schedule_head;

        while (sensor->next)
        {
            sensor = sensor->next;
        }

        sensor->next = malloc(sizeof(s_sensors_list_t));
        if (sensor->next)
        {
            sensor->next->sensor_call = sensor_call;
            sensor->next->next = NULL;
        }
    }
    else
    {
        sensor_schedule_head = malloc(sizeof(s_sensors_list_t));
        if (sensor_schedule_head)
        {
            sensor_schedule_head->sensor_call = sensor_call;
            sensor_schedule_head->next = NULL;
        }
    }
}

s_sensors_list_t *ezlopi_sensor_get_next_sensor(s_sensors_list_t *current_sensor)
{
    s_sensors_list_t *ret = NULL;
    if (current_sensor)
    {
        ret = current_sensor->next;
    }
    return ret;
}