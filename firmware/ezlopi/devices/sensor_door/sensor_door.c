

#include "trace.h"
#include "sensor_door.h"
#include "esp_err.h"
#include "ezlopi_actions.h"
#include "ezlopi_timer.h"
#include "items.h"
#include "cJSON.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "ezlopi_device_value_updated.h"

static int sensor_door_init(void *args);
static int get_door_sensor_value(void *args);
static int sensor_door_prepare(void *args);
static int sensor_door_add(void *args);


int setup_door_sensor(e_ezlopi_actions_t action, void *args)
{
    int ret = 0;
    switch(action)
    {
        case EZLOPI_ACTION_PREPARE:
        {
            TRACE_I("%s", ezlopi_actions_to_string(action));
            break;
        }
        case EZLOPI_ACTION_INITIALIZE:
        {
            TRACE_I("%s", ezlopi_actions_to_string(action));
            break;
        }
        case EZLOPI_ACTION_NOTIFY_500_MS:
    {
        TRACE_I("%s", ezlopi_actions_to_string(action));
        break;
    }
        default:
        {
            TRACE_E("Default action encountered.(action: %s)", ezlopi_actions_to_string(action));
            break;
        }
    }

    return ret;

}

static int sensor_door_prepare(void *args)
{
    int ret = 0;

    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)args;

    

    return ret;
}

static int get_door_sensor_value(void *args)
{
    TRACE_E("Reading value from the door sensor.");
    return hall_sensor_read();
}

static int sensor_door_init(void *args)
{
    esp_err_t error = adc1_config_width(ADC_WIDTH_BIT_12);
    if(error)
    {
        TRACE_E("Error configuring ADC_1's bit width.(error: %s)", esp_err_to_name(error));
    }
    else 
    {
        TRACE_I("Width configuration was successfully done!");
    }
    return error;
}

