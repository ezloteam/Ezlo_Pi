#include "ezlopi_core_nvs.h"
#include "ezlopi_core_modes.h"
#include "ezlopi_core_modes_cjson.h"

#include "ezlopi_cloud_constants.h"

static s_house_modes_t *sg_current_mode = NULL;
static s_ezlopi_modes_t *sg_custom_modes = NULL;

s_ezlopi_modes_t *ezlopi_core_modes_get_custom_modes(void)
{
    return sg_custom_modes;
}

s_house_modes_t *ezlopi_core_modes_get_current_mode(void)
{
    return sg_current_mode;
}

int ezlopi_core_modes_get_modes(cJSON *cj_result)
{
    return ezlopi_core_modes_cjson_get_modes(cj_result);
}

int ezlopi_core_modes_get_current_mode(cJSON *cj_result)
{
    return ezlopi_core_modes_cjson_get_current_mode(cj_result);
}

void ezlopi_core_modes_init(void)
{
    uint32_t _is_custom_mode_ok = 0;
    char *custom_modes_str = ezlopi_nvs_read_modes();

    if (custom_modes_str)
    {
        cJSON *cj_custom_modes = cJSON_Parse(custom_modes_str);
        if (cj_custom_modes)
        {
            _is_custom_mode_ok = 1;

            cJSON_Delete(cj_custom_modes);
        }

        free(custom_modes_str);
    }

    if (!_is_custom_mode_ok)
    {
        ezlopi_core_default_init();
        if (NULL == sg_custom_modes)
        {
            sg_custom_modes = ezlopi_core_default_mode_get();
        }
    }
}
