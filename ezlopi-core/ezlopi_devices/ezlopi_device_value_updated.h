#ifndef __EZLOPI_DEVICE_VALUE_UPDATED_H__
#define __EZLOPI_DEVICE_VALUE_UPDATED_H__
#include "ezlopi_devices_list.h"

#if 0 // v2.x
int ezlopi_device_value_updated_from_device(s_ezlopi_device_properties_t *properties);
#endif
int ezlopi_device_value_updated_from_device_v3(l_ezlopi_item_t *item);
int ezlopi_setting_value_updated_from_device_v3(l_ezlopi_device_settings_t *setting);

#endif // __EZLOPI_DEVICE_VALUE_UPDATED_H__