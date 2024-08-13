#ifndef __EZLOPI_CLOUD_CONSTANTS_H__
#define __EZLOPI_CLOUD_CONSTANTS_H__

#include <string.h>
#include <stdarg.h>

#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_devices_str.h"
#include "ezlopi_cloud_keywords.h"
#include "ezlopi_cloud_methods_str.h"
#include "ezlopi_cloud_room_types_str.h"
#include "ezlopi_cloud_scales_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_cloud_thermostat_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "ezlopi_cloud_item_name_str.h"

#include "EZLOPI_USER_CONFIG.h"

int ezlopi_create_cloud_strs(char *cloud_char, size_t cloud_char_size, const char* format, ...)
{
    int ret = -1;
    if(cloud_char)
    {
        memset(cloud_char, 0, cloud_char_size);
        va_list args;
        va_start(args, format);
        vsnprintf(cloud_char, cloud_char_size, format, args);
        va_end(args);
        ret = 0;
    }
    return ret;
}


#endif // __EZLOPI_CLOUD_CONSTANTS_H__
