#include "ezlopi_util_trace.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_modes.h"
#include "ezlopi_core_modes_cjson.h"
#include "ezlopi_core_cjson_macros.h"

#include "ezlopi_cloud_constants.h"

#include "ezlopi_service_modes.h"
#include "EZLOPI_USER_CONFIG.h"


#if defined(CONFIG_EZPI_SERV_ENABLE_MODES)

static s_ezlopi_modes_t* sg_custom_modes = NULL;
static s_house_modes_t* sg_current_house_mode = NULL;

s_ezlopi_modes_t* ezlopi_core_modes_get_custom_modes(void)
{
    return sg_custom_modes;
}

int ezlopi_core_modes_set_current_house_mode(s_house_modes_t* new_house_mode)
{
    sg_current_house_mode = new_house_mode;
    return 1;
}

s_house_modes_t* ezlopi_core_modes_get_current_house_modes(void)
{
    return sg_current_house_mode;
}

s_house_modes_t* ezlopi_core_modes_get_house_mode_by_id(uint32_t house_mode_id)
{
    s_house_modes_t* _house_mode = NULL;

    if (house_mode_id == sg_custom_modes->mode_home._id)
    {
        _house_mode = &sg_custom_modes->mode_home;
    }
    else if (house_mode_id == sg_custom_modes->mode_away._id)
    {
        _house_mode = &sg_custom_modes->mode_away;
    }
    else if (house_mode_id == sg_custom_modes->mode_night._id)
    {
        _house_mode = &sg_custom_modes->mode_night;
    }
    else if (house_mode_id == sg_custom_modes->mode_vacation._id)
    {
        _house_mode = &sg_custom_modes->mode_vacation;
    }
    else
    {
        TRACE_E("house-mode-id does not match with existing house-modes!");
    }

    return _house_mode;
}

s_house_modes_t* ezlopi_core_modes_get_house_mode_by_name(char* house_mode_name)
{
    s_house_modes_t* _house_mode = NULL;

    if (house_mode_name)
    {
        if (0 == strcmp(house_mode_name, sg_custom_modes->mode_home.name))
        {
            _house_mode = &sg_custom_modes->mode_home;
        }
        else if (0 == strcmp(house_mode_name, sg_custom_modes->mode_away.name))
        {
            _house_mode = &sg_custom_modes->mode_away;
        }
        else if (0 == strcmp(house_mode_name, sg_custom_modes->mode_night.name))
        {
            _house_mode = &sg_custom_modes->mode_night;
        }
        else if (0 == strcmp(house_mode_name, sg_custom_modes->mode_vacation.name))
        {
            _house_mode = &sg_custom_modes->mode_vacation;
        }
        else
        {
            TRACE_E("modes-name does not match with existing modes!");
        }
    }

    return _house_mode;
}

int ezlopi_core_modes_api_get_modes(cJSON* cj_result)
{
    return ezlopi_core_modes_cjson_get_modes(cj_result);
}

int ezlopi_core_modes_api_get_current_mode(cJSON* cj_result)
{
    return ezlopi_core_modes_cjson_get_current_mode(cj_result);
}

int ezlopi_core_modes_api_switch_mode(s_house_modes_t* switch_to_house_mode)
{
    ezlopi_service_modes_stop();
    sg_custom_modes->switch_to_mode_id = switch_to_house_mode->_id;
    sg_custom_modes->time_is_left_to_switch_sec = switch_to_house_mode->switch_to_delay_sec;
    ezlopi_service_modes_start();

    return 1;
}

int ezlopi_core_modes_api_cancel_switch(void)
{
    int ret = 0;
    if (sg_custom_modes)
    {
        ezlopi_service_modes_stop();
        sg_custom_modes->switch_to_mode_id = 0;
        sg_custom_modes->time_is_left_to_switch_sec = 0;
        ezlopi_service_modes_start();
        ret = 1;
    }

    return ret;
}

int ezlopi_core_modes_api_cancel_entry_delay(void)
{
    int ret = 0;
    if (sg_custom_modes)
    {
        ret = 1;
        ezlopi_service_modes_stop();
        sg_custom_modes->entry_delay.short_delay_sec = 0;
        sg_custom_modes->entry_delay.normal_delay_sec = 0;
        sg_custom_modes->entry_delay.extended_delay_sec = 0;
        sg_custom_modes->entry_delay.instant_delay_sec = 0;
        ezlopi_service_modes_start();
    }

    return ret;
}

int ezlopi_core_modes_set_switch_to_delay(uint32_t switch_to_delay)
{
    int ret = 0;
    if (sg_current_house_mode)
    {
        ezlopi_service_modes_stop();
        sg_current_house_mode->switch_to_delay_sec = switch_to_delay;
        ezlopi_core_modes_store_to_nvs();
        ezlopi_service_modes_start();
    }
    return ret;
}

int ezlopi_core_modes_set_alarm_delay(uint32_t alarm_to_delay)
{
    int ret = 0;
    if (sg_current_house_mode)
    {
        ret = 1;
        ezlopi_service_modes_stop();
        sg_current_house_mode->alarm_delay_sec = alarm_to_delay;
        ezlopi_core_modes_store_to_nvs();
        ezlopi_service_modes_start();
    }
    return ret;
}

int ezlopi_core_modes_set_notifications(cJSON* cj_params)
{
    int ret = 0;
    if (cj_params)
    {
        ezlopi_service_modes_stop();

        ezlopi_service_modes_start();
    }
    return ret;
}

int ezlopi_core_modes_store_to_nvs(void)
{
    int ret = 0;
    cJSON* cj_modes = cJSON_CreateObject(__FUNCTION__);
    if (cj_modes)
    {
        ezlopi_core_modes_cjson_get_modes(cj_modes);
        char* modes_str = cJSON_PrintBuffered(__FUNCTION__, cj_modes, 4096, false);
        TRACE_D("length of 'modes_str': %d", strlen(modes_str));

        cJSON_Delete(__FUNCTION__, cj_modes);

        if (modes_str)
        {
            ret = ezlopi_nvs_write_modes(modes_str);
            ezlopi_free(__FUNCTION__, modes_str);
        }
    }

    return ret;
}

void ezlopi_core_modes_init(void)
{
    uint32_t _is_custom_mode_ok = 0;
    char* custom_modes_str = ezlopi_nvs_read_modes();

    if (custom_modes_str)
    {
        cJSON* cj_custom_modes = cJSON_Parse(__FUNCTION__, custom_modes_str);
        ezlopi_free(__FUNCTION__, custom_modes_str);

        CJSON_TRACE("cj_custom-modes", cj_custom_modes);

        if (cj_custom_modes)
        {
            _is_custom_mode_ok = 1;
            sg_custom_modes = ezlopi_core_modes_cjson_parse_modes(cj_custom_modes);
            cJSON_Delete(__FUNCTION__, cj_custom_modes);
        }
    }

    if (0 == _is_custom_mode_ok)
    {
        ezlopi_core_default_init();
        if (NULL == sg_custom_modes)
        {
            sg_custom_modes = ezlopi_core_default_mode_get();
            sg_current_house_mode = &sg_custom_modes->mode_home;
        }
    }
}
#endif // CONFIG_EZPI_SERV_ENABLE_MODES