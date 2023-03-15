#ifndef __EZLOPI_CLOUD_H__
#define __EZLOPI_CLOUD_H__

#include <stdbool.h>
#include <stdint.h>

typedef struct s_ezlopi_cloud_info
{
    // cloud information
    char device_name[32];    // device name, 'name' field in 'hub.devices.list'
    uint32_t device_id;      //
    const char *category;    // ezlopi-cloud/constants/category_str.h
    const char *subcategory; // ezlopi-cloud/constants/subcategory_str.h
    const char *device_type; // ezlopi-cloud/constants/device_types_str.h
    const char *value_type;  // ezlopi-cloud/constants/values_str.h
    bool reachable;
    bool battery_powered;
    uint32_t room_id;   //
    char room_name[32]; //

    uint32_t item_id;      //
    const char *item_name; // https://api.ezlo.com/devices/items/index.html

    bool has_getter; //
    bool has_setter;
    bool show; // make device visible on UI

} s_ezlopi_cloud_info_t;

uint32_t ezlopi_cloud_generate_device_id(void);
uint32_t ezlopi_cloud_generate_item_id(void);
uint32_t ezlopi_cloud_generate_room_id(void);
uint32_t ezlopi_cloud_generate_gateway_id(void);

#endif // __EZLOPI_CLOUD_H__
