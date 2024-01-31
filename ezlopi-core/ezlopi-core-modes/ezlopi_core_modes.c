#include "ezlopi_cloud_constants.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_modes.h"

static s_house_modes_t *sg_current_mode = NULL;
static s_ezlopi_modes_t *sg_custom_modes = NULL;

static void __cjson_add_number_as_hex_string(cJSON *cj_dest, char *obj_name, uint32_t number)
{
    if (cj_dest && obj_name && number)
    {
        char tmp_str[32];
        snprintf(tmp_str, sizeof(tmp_str), "08x", number);
        cJSON_AddStringToObject(cj_dest, obj_name, tmp_str);
    }
}

int ezlopi_core_modes_add_to_result(cJSON *cj_result)
{
    if (sg_custom_modes)
    {
        __cjson_add_number_as_hex_string(cj_result, ezlopi_current_str, sg_custom_modes->current_mode_id);
        __cjson_add_number_as_hex_string(cj_result, ezlopi_current_str, sg_custom_modes->switch_to_mode_id);

        cJSON_AddNumberToObject(cj_result, ezlopi_timeIsLeftToSwitch_str, sg_custom_modes->time_is_left_to_switch_ms);
        cJSON_AddNumberToObject(cj_result, ezlopi_switchToDelay_str, )
    }
}

void ezlopi_core_modes_init(void)
{
    ezlopi_core_default_init();
}
