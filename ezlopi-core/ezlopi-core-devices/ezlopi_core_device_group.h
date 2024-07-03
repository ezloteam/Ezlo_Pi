#ifndef EZLOPI_CORE_DEVICES_GROUP_H
#define EZLOPI_CORE_DEVICES_GROUP_H

#include "ezlopi_core_devices.h"


typedef enum e_ezlopi_device_grp_entrydelay_type
{
    EZLOPI_DEVICE_GRP_ENTRYDELAY_UNDEFINED = 0,
    EZLOPI_DEVICE_GRP_ENTRYDELAY_NONE,
    EZLOPI_DEVICE_GRP_ENTRYDELAY_NORMAL,
    EZLOPI_DEVICE_GRP_ENTRYDELAY_LONG_EXTENDED,
    EZLOPI_DEVICE_GRP_ENTRYDELAY_EXTENDED,
    EZLOPI_DEVICE_GRP_ENTRYDELAY_INSTANT,
    EZLOPI_DEVICE_GRP_ENTRYDELAY_MAX
}e_ezlopi_device_grp_entrydelay_type_t;

typedef enum e_ezlopi_device_grp_role_type
{
    EZLOPI_DEVICE_GRP_ROLE_NONE = 0,
    EZLOPI_DEVICE_GRP_ROLE_USER,
    EZLOPI_DEVICE_GRP_ROLE_HOUSE_MODES,
    EZLOPI_DEVICE_GRP_ROLE_MAX
}e_ezlopi_device_grp_role_type_t;

typedef struct l_ezlopi_device_grp
{
    uint32_t grp_id;
    char grp_name[128];
    cJSON * categories;         // array containing required categories/sub-categories
    cJSON * devices;            // array containing DeviceIds to be added
    cJSON * exceptions;         // array of DeviceIds to exclude from this group
    bool persistent;
    e_ezlopi_device_grp_entrydelay_type_t entry_delay;
    bool follow_entry;
    e_ezlopi_device_grp_role_type_t role;
    char package_id;
    struct l_ezlopi_device_grp* next;
}l_ezlopi_device_grp_t;


// ------------ device-group --------------------
l_ezlopi_device_grp_t* ezlopi_device_grp_get_head(void);

#endif//EZLOPI_CORE_DEVICES_GROUP_H