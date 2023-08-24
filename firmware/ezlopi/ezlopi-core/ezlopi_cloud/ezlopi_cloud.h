#ifndef __EZLOPI_CLOUD_H__
#define __EZLOPI_CLOUD_H__

#include <stdbool.h>
#include <stdint.h>

typedef struct s_ezlopi_cloud_controller
{
    char parent_device_id[40]; // 'parentDeviceId' -> https://api.ezlo.com/hub/devices_api/index.html
    char device_type_id[40];   // 'deviceTypeId' -> https://api.ezlo.com/hub/devices_api/index.html
    char gateway_id[40];       // 'gatewayId' -> https://api.ezlo.com/hub/devices_api/index.html
    bool battery_powered;      // 'batteryPowered' -> https://api.ezlo.com/hub/devices_api/index.html
    bool reachable;            // 'reachable' -> https://api.ezlo.com/hub/devices_api/index.html
    bool armed;                // 'armed' -> https://api.ezlo.com/hub/devices_api/index.html
    char room_id[40];          // 'roomId' -> https://api.ezlo.com/hub/devices_api/index.html
    bool persistent;           // 'persistent' -> https://api.ezlo.com/hub/devices_api/index.html
    bool service_notification; // 'serviceNotification' -> https://api.ezlo.com/hub/devices_api/index.html
    char *security;            // 'security' -> https://api.ezlo.com/hub/devices_api/index.html, ezlopi-cloud/ezlopi_cloud_constants/ezlopi_cloud_security_str.h
    bool ready;                // 'ready' -> https://api.ezlo.com/hub/devices_api/index.html
    char *status;              // 'status' -> https://api.ezlo.com/hub/devices_api/index.html, ezlopi-cloud/ezlopi_cloud_constants/ezlopi_cloud_status_str.h

} s_ezlopi_cloud_controller_t;

typedef struct s_ezlopi_cloud_item
{
    uint32_t item_id;       // '_id' -> https://api.ezlo.com/hub/items_api/index.html,
    bool has_getter;        // 'hasGetter' -> https://api.ezlo.com/hub/items_api/index.html,
    bool has_setter;        // 'haSetter' -> https://api.ezlo.com/hub/items_api/index.html,
    const char *item_name;  // 'name' -> https://api.ezlo.com/hub/items_api/index.html, ezlopi-cloud/ezlopi_cloud_constants/ezlopi_item_name_str.h
    bool show;              // 'show' -> https://api.ezlo.com/hub/items_api/index.html,
    const char *value_type; // 'valueType' -> https://api.ezlo.com/hub/items_api/index.html, // ezlopi-cloud/constants/values_str.h
    const char *scale;      // 'scale' -> https://api.ezlo.com/devices/item_value_types/index.html#scalable-types -> ezlopi-cloud/constants/ezlopi_cloud_scales_str.h

} s_ezlopi_cloud_item_t;

typedef struct s_ezlopi_cloud_device
{
    uint32_t device_id;      // '_id' -> https://api.ezlo.com/hub/devices_api/index.html
    char device_name[32];    // 'name' -> https://api.ezlo.com/hub/devices_api/index.html
    const char *category;    // 'category' -> https://api.ezlo.com/hub/devices_api/index.html, ezlopi-cloud/constants/category_str.h
    const char *subcategory; // 'subcategory' -> https://api.ezlo.com/hub/devices_api/index.html, ezlopi-cloud/constants/subcategory_str.h
    const char *device_type; // 'type' -> https://api.ezlo.com/hub/devices_api/index.html, ezlopi-cloud/constants/device_types_str.h

} s_ezlopi_cloud_device_t;

typedef struct s_ezlopi_cloud_info
{
    // cloud information
    char device_name[32];    // device name, 'name' field in 'hub.devices.list'
    uint32_t device_id;      //
    const char *category;    // ezlopi-cloud/constants/category_str.h
    const char *subcategory; // ezlopi-cloud/constants/subcategory_str.h
    const char *device_type; // ezlopi-cloud/constants/device_types_str.h [type in device API]
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
uint32_t ezlopi_cloud_get_gateway_id(void);
uint32_t ezlopi_cloud_generate_scene_id(void);

#endif // __EZLOPI_CLOUD_H__
