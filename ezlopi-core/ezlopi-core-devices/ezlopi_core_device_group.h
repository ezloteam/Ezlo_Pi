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
    uint32_t _id;
    char name[128];
    cJSON * categories;         // array containing required categories/sub-categories
    cJSON * devices;            // array containing DeviceIds to be added
    cJSON * exceptions;         // array of DeviceIds to exclude from this group
    bool persistent;
    e_ezlopi_device_grp_entrydelay_type_t entry_delay;
    bool follow_entry;
    e_ezlopi_device_grp_role_type_t role;
    char package_id[32];
    struct l_ezlopi_device_grp* next;
}l_ezlopi_device_grp_t;


// ------------ device-group --------------------
void ezlopi_device_group_init(void);

l_ezlopi_device_grp_t* ezlopi_core_device_group_get_head(void);
l_ezlopi_device_grp_t* ezlopi_core_device_group_get_by_id(uint32_t _id);
uint32_t ezlopi_core_device_group_store_nvs_devgrp(cJSON* cj_new_device_grp);
uint32_t ezlopi_core_device_group_get_list(cJSON* cj_devgrp_array);
l_ezlopi_device_grp_t * ezlopi_core_device_group_new_devgrp_populate(cJSON *cj_params, uint32_t new_device_grp_id);

int ezlopi_core_device_group_edit_by_id(uint32_t scene_id, cJSON* cj_scene);

void ezlopi_core_device_group_depopulate_by_id_v2(uint32_t _id);
void ezlopi_core_device_group_remove_id_from_list(uint32_t _id);

#endif//EZLOPI_CORE_DEVICES_GROUP_H