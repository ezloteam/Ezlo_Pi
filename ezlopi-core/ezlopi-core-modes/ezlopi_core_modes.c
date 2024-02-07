#include "ezlopi_util_trace.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_modes.h"
#include "ezlopi_core_modes_cjson.h"
#include "ezlopi_core_cjson_macros.h"

#include "ezlopi_cloud_constants.h"

static s_ezlopi_modes_t *sg_custom_modes = NULL;
static s_house_modes_t *sg_current_house_mode = NULL;

s_ezlopi_modes_t *ezlopi_core_modes_get_custom_modes(void)
{
    return sg_custom_modes;
}

s_house_modes_t *ezlopi_core_modes_get_current_house_modes(void)
{
    return sg_current_house_mode;
}

int ezlopi_core_modes_api_get_modes(cJSON *cj_result)
{
    return ezlopi_core_modes_cjson_get_modes(cj_result);
}

int ezlopi_core_modes_api_get_current_mode(cJSON *cj_result)
{
    return ezlopi_core_modes_cjson_get_current_mode(cj_result);
}

int ezlopi_core_modes_api_switch_mode(s_house_modes_t *switch_to_house_mode)
{
    int ret = 0;

    sg_current_house_mode = switch_to_house_mode;
    // if (sg_current_house_mode->cj_bypass_devices)
    // {
    //     cJSON_Delete(sg_current_house_mode->cj_bypass_devices);
    //     sg_current_house_mode->cj_bypass_devices = NULL;
    // }

    sg_custom_modes->switch_to_mode_id = 0;
    sg_custom_modes->current_mode_id = sg_current_house_mode->_id;

    cJSON *cj_modes = cJSON_CreateObject();
    if (cj_modes)
    {
        ezlopi_core_modes_cjson_get_modes(cj_modes);
        char *modes_str = cJSON_Print(cj_modes);
        cJSON_Delete(cj_modes);

        if (modes_str)
        {
            TRACE_D("updated-modes: %s", modes_str);
            cJSON_Minify(modes_str);
            ret = ezlopi_nvs_write_modes(modes_str);
            free(modes_str);
        }
        else
        {
            TRACE_E("here");
        }
    }
    else
    {
        TRACE_E("Here");
    }

    return ret;
}

void ezlopi_core_modes_init(void)
{
    uint32_t _is_custom_mode_ok = 0;
    char *custom_modes_str = ezlopi_nvs_read_modes();

    if (custom_modes_str)
    {
        cJSON *cj_custom_modes = cJSON_Parse(custom_modes_str);
        free(custom_modes_str);

        CJSON_TRACE("cj_custom-modes", cj_custom_modes);

        if (cj_custom_modes)
        {
            _is_custom_mode_ok = 1;
            sg_custom_modes = ezlopi_core_modes_cjson_parse_modes(cj_custom_modes);
            cJSON_Delete(cj_custom_modes);

            cJSON *cj_test_modes = cJSON_CreateObject();
            if (cj_test_modes)
            {
                ezlopi_core_modes_cjson_get_modes(cj_test_modes);
                CJSON_TRACE("cj-test-modes", cj_test_modes);
                cJSON_Delete(cj_test_modes);
            }
        }
    }

    if (0 == _is_custom_mode_ok)
    {
        ezlopi_core_default_init();
        if (NULL == sg_custom_modes)
        {
            sg_custom_modes = ezlopi_core_default_mode_get();
            sg_current_house_mode = &sg_custom_modes->mode_home;

            cJSON *cj_test_modes = cJSON_CreateObject();
            if (cj_test_modes)
            {
                ezlopi_core_modes_cjson_get_modes(cj_test_modes);
                CJSON_TRACE("cj-test-modes", cj_test_modes);
                cJSON_Delete(cj_test_modes);
            }
        }
    }
}
