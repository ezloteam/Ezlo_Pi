#ifndef _EZLOPI_CORE_CLOUD_H_
#define _EZLOPI_CORE_CLOUD_H_

#include <stdbool.h>
#include <stdint.h>

#include "cjext.h"

typedef struct s_ezlopi_cloud_controller
{
    char parent_device_id[40]; // 'parentDeviceId' -> https://api.ezlo.com/hub/devices_api/index.html
    char device_type_id[40];   // 'deviceTypeId' -> https://api.ezlo.com/hub/devices_api/index.html
    char gateway_id[40];       // 'gatewayId' -> https://api.ezlo.com/hub/devices_api/index.html
    bool battery_powered;      // 'batteryPowered' -> https://api.ezlo.com/hub/devices_api/index.html
    bool reachable;            // 'reachable' -> https://api.ezlo.com/hub/devices_api/index.html
    bool armed;                // 'armed' -> https://api.ezlo.com/hub/devices_api/index.html
    // char room_id[40];          // 'roomId' -> https://api.ezlo.com/hub/devices_api/index.html
    bool persistent;           // 'persistent' -> https://api.ezlo.com/hub/devices_api/index.html
    bool service_notification; // 'serviceNotification' -> https://api.ezlo.com/hub/devices_api/index.html
    const char *security;      // 'security' -> https://api.ezlo.com/hub/devices_api/index.html, ezlopi-cloud/ezlopi_cloud_constants/ezlopi_cloud_security_str.h
    bool ready;                // 'ready' -> https://api.ezlo.com/hub/devices_api/index.html
    const char *status;        // 'status' -> https://api.ezlo.com/hub/devices_api/index.html, ezlopi-cloud/ezlopi_cloud_constants/ezlopi_cloud_status_str.h

} s_ezlopi_cloud_controller_t;

typedef struct s_ezlopi_cloud_item
{
    uint32_t item_id;       // '_id' -> https://api.ezlo.com/hub/items_api/index.html,
    uint32_t device_id;     // 'deviceID' -> https://api.ezlo.com/hub/items_api/index.html,
    bool has_getter;        // 'hasGetter' -> https://api.ezlo.com/hub/items_api/index.html,
    bool has_setter;        // 'haSetter' -> https://api.ezlo.com/hub/items_api/index.html,
    const char *item_name;  // 'name' -> https://api.ezlo.com/hub/items_api/index.html, ezlopi-cloud/ezlopi_cloud_constants/ezlopi_cloud_item_name_str.h
    bool show;              // 'show' -> https://api.ezlo.com/hub/items_api/index.html,
    const char *value_type; // 'valueType' -> https://api.ezlo.com/hub/items_api/index.html, // ezlopi-cloud/constants/values_str.h
    const char *scale;      // 'scale' -> https://api.ezlo.com/devices/item_value_types/index.html#scalable-types -> ezlopi-cloud/constants/ezlopi_cloud_scales_str.h

} s_ezlopi_cloud_item_t;

typedef struct s_ezlopi_cloud_device_settings
{
    uint32_t setting_id; // _id -> https://log.ezlo.com/new/hub/settings_api/
    uint32_t device_id;  // deviceId -> https://log.ezlo.com/new/hub/settings_api/
} s_ezlopi_cloud_device_settings_t;

typedef struct s_ezlopi_cloud_device
{
    uint32_t device_id;        // '_id' -> https://api.ezlo.com/hub/devices_api/index.html
    uint32_t parent_device_id; // 'parentDeviceId' -> https://log.ezlo.com/new/hub/devices_api/
    uint32_t room_id;          // 'roomId' -> https://api.ezlo.com/hub/devices_api/index.html
    bool parent_room;          // 'parentRoom' -> 'https://log.ezlo.com/new/hub/devices_api/'   [default => '1']  ; # This flag indicates the 'child_device' is in same 'room_id' as that of 'parent_device'
    char device_name[64];      // 'name' -> https://api.ezlo.com/hub/devices_api/index.html
    char *device_type_id;      // 'deviceTypeId' -> https://log.ezlo.com/new/hub/devices_api/
    const char *category;      // 'category' -> https://api.ezlo.com/hub/devices_api/index.html, ezlopi-cloud/constants/category_str.h
    const char *subcategory;   // 'subcategory' -> https://api.ezlo.com/hub/devices_api/index.html, ezlopi-cloud/constants/subcategory_str.h
    const char *device_type;   // 'type' -> https://api.ezlo.com/hub/devices_api/index.html, ezlopi-cloud/constants/device_types_str.h
    cJSON *info;               // 'info' -> https://log.ezlo.com/new/hub/devices_api/
    bool armed;                // 'armed' -> https://api.ezlo.com/hub/devices_api/index.html
    char protect_config[32];   // 'protectConfig' ->https://log.ezlo.com/new/hub/devices_api/
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

extern const uint32_t DEVICE_ID_START;
extern const uint32_t ITEM_ID_START;
extern const uint32_t ROOM_ID_START;
extern const uint32_t SETTINGS_ID_START;
extern const uint32_t SCENE_ID_START;
extern const uint32_t SCRIPT_ID_START;
extern const uint32_t EXPRESSION_ID_START;
extern const uint32_t MODES_ID_START;
extern const uint32_t SCENE_GROUP_ID_START;
extern const uint32_t SCENE_WHEN_BLOCKID_START;
extern const uint32_t DEVICE_GROUPID_START;

uint32_t ezlopi_cloud_generate_device_id(void);
void ezlopi_cloud_update_device_id(uint32_t device_id);

uint32_t ezlopi_cloud_generate_item_id(void);

uint32_t ezlopi_cloud_generate_room_id(void);

uint32_t ezlopi_cloud_get_gateway_id(void);
uint32_t ezlopi_cloud_generate_gateway_id(void);

uint32_t ezlopi_cloud_generate_settings_id(void);

uint32_t ezlopi_cloud_generate_scene_id(void);
void ezlopi_cloud_update_scene_id(uint32_t a_scene_id);

uint32_t ezlopi_cloud_generate_scene_when_blockId(void);
void ezlopi_cloud_update_when_blockId(uint32_t a_when_blockId);

uint32_t ezlopi_cloud_generate_script_id(void);
void ezlopi_cloud_update_script_id(uint32_t a_script_id);

uint32_t ezlopi_cloud_generate_expression_id(void);
void ezlopi_cloud_update_expression_id(uint32_t a_expression_id);

uint32_t ezlopi_cloud_generate_room_id(void);
void ezlopi_cloud_update_room_id(uint32_t a_room_id);

uint32_t ezlopi_cloud_generate_scene_group_id(void);
void ezlopi_cloud_update_group_id(uint32_t a_group_id);

uint32_t ezlopi_cloud_generate_device_group_id(void);
void ezlopi_cloud_update_device_group_id(uint32_t a_device_group_id);

uint32_t ezlopi_cloud_generate_item_group_id(void);
void ezlopi_cloud_update_item_group_id(uint32_t a_item_group_id);

// uint32_t ezlopi_cloud_get_modes_initial_id(void);

#endif // _EZLOPI_CORE_CLOUD_H_
