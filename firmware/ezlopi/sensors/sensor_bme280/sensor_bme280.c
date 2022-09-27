#include "sensor_bme280.h"
#include "ezlopi_actions.h"

static char *sensor_bme280_ezlopi_update_data(void)
{
    char *ret = NULL;

    return ret;
}

static int sensor_bme280_init(void)
{
    int ret = 0;

    return ret;
}

static int sensor_bme280_get_value(void)
{
    int ret = 0;
    sensor_bme280_ezlopi_update_data();
    return ret;
}

static int sensor_bme280_set_value(void *arg)
{
    int ret = 0;

    return ret;
}

static int sensor_bme280_notify_30_seconds(void)
{
    int ret = 0;

    static int seconds_counter;
    seconds_counter = (seconds_counter % 30) ? seconds_counter : 0;

    if (0 == seconds_counter)
    {
        seconds_counter = 0;

        /* Send the value to cloud using web-socket */
        char *data = sensor_bme280_ezlopi_update_data();
        if (data)
        {
            /* Send to ezlo cloud */
        }
    }

    seconds_counter++;
    return ret;
}

int sensor_bme280(e_ezlopi_actions_t action, void *arg) { return 0; }

// int sensor_bme280(e_ezlopi_actions_t action, void *arg)
// {
//     int ret = 0;

//     switch (action)
//     {
//     case EZLOPI_ACTION_INITIALIZE:
//     {
//         ret = sensor_bme280_init();
//         break;
//     }
//     case EZLOPI_ACTION_GET_VALUE:
//     {
//         ret = sensor_bme280_get_value();
//         break;
//     }
//     case EZLOPI_ACTION_SET_VALUE:
//     {
//         ret = sensor_bme280_set_value(arg);
//         break;
//     }
//     case EZLOPI_ACTION_NOTIFY_1000_MS:
//     {
//         ret = sensor_bme280_notify_30_seconds();
//         break;
//     }
//     case EZLOPI_ACTION_HUB_GET_DEVICE:
//     {
//         break;
//     }
//     case EZLOPI_ACTION_HUB_GET_ITEM:
//     {
//         break;
//     }
//     case EZLOPI_ACTION_HUB_UPDATE:
//     {
//         break;
//     }
//     default:
//     {
//         break;
//     }
//     }

//     return ret;
// }