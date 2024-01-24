#ifndef _EZLOPI_CORE_GATEWAY_H_
#define _EZLOPI_CORE_GATEWAY_H_

#include <stdlib.h>
#include <stdint.h>

typedef struct s_ezlopi_gateway
{
    bool ready;
    bool status;
    bool manual_device_adding;
    uint32_t _id;
    uint32_t settings;

    char *label;
    char *name;
    char *pluginid;

    char *clear_item_dictionary_command;
    char *set_item_dictionary_value_command;
    char *add_item_dictionary_value_command;
    char *remove_item_dictionary_value_command;
    char *set_setting_dictionary_value_command;

    char *check_device_command;
    char *force_remove_device_command;
    char *update_device_firmware_command;

    char *reset_setting_command;
    char *set_item_value_command;
    char *set_setting_value_command;
    char *set_gateway_setting_vlaue_command;

} s_ezlopi_gateway_t;

s_ezlopi_gateway_t *ezlopi_gateway_get(void);

#endif // _EZLOPI_CORE_GATEWAY_H_
