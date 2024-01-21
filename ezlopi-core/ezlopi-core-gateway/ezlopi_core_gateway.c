#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "ezlopi_core_gateway.h"
#include "ezlopi_cloud_constants.h"

static s_ezlopi_gateway_t ezlopi_gateway = {
    .ready = true,
    .status = false,
    .manual_device_adding = false,
    ._id = 0x457a5069,
    .settings = 3,

    .label = "virtual gateway",
    .name = "test_plugin",
    .pluginid = "test_plugin",

    .clear_item_dictionary_command = "",
    .set_item_dictionary_value_command = "HUB:test_plugin/scripts/set_item_dictionary_value",
    .add_item_dictionary_value_command = "HUB:test_plugin/scripts/add_item_dictionary_value",
    .remove_item_dictionary_value_command = "HUB:test_plugin/scripts/remove_item_dictionary_value",
    .set_setting_dictionary_value_command = "",

    .check_device_command = "",
    .force_remove_device_command = "",
    .update_device_firmware_command = "",

    .reset_setting_command = "",
    .set_item_value_command = "HUB:test_plugin/scripts/set_item_value",
    .set_setting_value_command = "HUB:test_plugin/scripts/set_setting_value",
    .set_gateway_setting_vlaue_command = "HUB:test_plugin/scripts/set_gateway_setting_value",
};

s_ezlopi_gateway_t *ezlopi_gateway_get(void)
{
    return &ezlopi_gateway;
}
