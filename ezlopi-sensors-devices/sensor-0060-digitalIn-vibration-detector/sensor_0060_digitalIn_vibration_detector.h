#ifndef _SENSOR_0060_DIGITALIN_VIBRATION_DETECTOR_H_
#define _SENSOR_0060_DIGITALIN_VIBRATION_DETECTOR_H_

#include "ezlopi_core_actions.h"
#include "ezlopi_core_devices.h"
typedef enum
{
    SW420_VIBRATION_ACTIVITY_NO_ACTIVITY,
    SW420_VIBRATION_ACTIVITY_SHAKE,
    SW420_VIBRATION_ACTIVITY_TILT,
    SW420_VIBRATION_ACTIVITY_DROP,
    SW420_VIBRATION_ACTIVITY_MAX
} e_sw420_vibration_activity_states_t;
//----------------------------------------------------------------------------------------------------------------------
int sensor_0060_digitalIn_vibration_detector(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#endif //_SENSOR_0060_DIGITALIN_VIBRATION_DETECTOR_H_