#include "string.h"
#include "stdint.h"
#include "time.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


typedef struct s_sensors_schedule {
    struct s_sensors_schedule * next_sensor;
    void (*sample_func)(void);
    time_t interval_ms;
    time_t next_sample_time;
    void (*initiator_func)(void);

} s_sensors_schedule_t;

static s_sensors_schedule_t * sensor_schedule_head = NULL;

static void sensor_schedular(void *pv);

void add_sensor_to_schedule(void(*sample_func)(void))
{
    if (sensor_schedule_head) {

    } else {
        
    }
}

void sensor_service(void){
    xTaskCreate(sensor_schedular, "sensor schedualr", 4*1024, NULL, 4, NULL);
}

static void sensor_schedular(void *pv)
{
    while (1)
    {
        /* code */
    }
}