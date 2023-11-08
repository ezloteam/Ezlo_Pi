#include "ezlopi_actions.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_devices_list_includes.h"

static s_ezlopi_device_v3_t gc_device_array_v3[] = {
#define EZLOPI_DEVICE_LIST(name_str, id_item, dev_func) {.name = name_str, .id = id_item, .func = dev_func},
#include "ezlopi_devices_list_modules.h"
#undef EZLOPI_DEVICE_LIST
};

s_ezlopi_device_v3_t *ezlopi_devices_list_get_list_v3(void)
{
    return gc_device_array_v3;
}
