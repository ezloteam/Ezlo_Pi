#include "ezlopi_util_trace.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_modes.h"
#include "ezlopi_core_modes_cjson.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_errors.h"

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

ezlopi_error_t ezlopi_core_modes_set_current_house_mode(s_house_modes_t* new_house_mode)
{
    sg_current_house_mode = new_house_mode;
    return EZPI_SUCCESS;
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

ezlopi_error_t ezlopi_core_modes_api_get_modes(cJSON* cj_result)
{
    return ezlopi_core_modes_cjson_get_modes(cj_result);
}

ezlopi_error_t ezlopi_core_modes_api_get_current_mode(cJSON* cj_result)
{
    return ezlopi_core_modes_cjson_get_current_mode(cj_result);
}

ezlopi_error_t ezlopi_core_modes_api_switch_mode(s_house_modes_t* switch_to_house_mode)
{
    ezlopi_service_modes_stop();
    sg_custom_modes->switch_to_mode_id = switch_to_house_mode->_id;
    sg_custom_modes->time_is_left_to_switch_sec = switch_to_house_mode->switch_to_delay_sec;
    ezlopi_service_modes_start();

    return 1;
}

ezlopi_error_t ezlopi_core_modes_api_cancel_switch(void)
{
    ezlopi_error_t ret = EZPI_ERR_MODES_FAILED;
    if (sg_custom_modes)
    {
        ezlopi_service_modes_stop();
        sg_custom_modes->switch_to_mode_id = 0;
        sg_custom_modes->time_is_left_to_switch_sec = 0;
        ezlopi_service_modes_start();
        ret = EZPI_SUCCESS;
    }

    return ret;
}

ezlopi_error_t ezlopi_core_modes_api_cancel_entry_delay(void)
{
    ezlopi_error_t ret = EZPI_ERR_MODES_FAILED;
    if (sg_custom_modes)
    {
        ret = EZPI_SUCCESS;
        ezlopi_service_modes_stop();
        sg_custom_modes->entry_delay.short_delay_sec = 0;
        sg_custom_modes->entry_delay.normal_delay_sec = 0;
        sg_custom_modes->entry_delay.extended_delay_sec = 0;
        sg_custom_modes->entry_delay.instant_delay_sec = 0;
        ezlopi_service_modes_start();
    }

    return ret;
}

ezlopi_error_t ezlopi_core_modes_set_switch_to_delay(uint32_t switch_to_delay)
{
    ezlopi_error_t ret = EZPI_ERR_MODES_FAILED;
    if (sg_current_house_mode)
    {
        ret = EZPI_SUCCESS;
        ezlopi_service_modes_stop();
        sg_current_house_mode->switch_to_delay_sec = switch_to_delay;
        ezlopi_core_modes_store_to_nvs();
        ezlopi_service_modes_start();
    }
    return ret;
}

ezlopi_error_t ezlopi_core_modes_set_alarm_delay(uint32_t alarm_to_delay)
{
    ezlopi_error_t ret = EZPI_ERR_MODES_FAILED;
    if (sg_current_house_mode)
    {
        ret = EZPI_SUCCESS;
        ezlopi_service_modes_stop();
        sg_current_house_mode->alarm_delay_sec = alarm_to_delay;
        ezlopi_core_modes_store_to_nvs();
        ezlopi_service_modes_start();
    }
    return ret;
}

ezlopi_error_t ezlopi_core_modes_set_notifications(cJSON* cj_params)
{
    ezlopi_error_t ret = EZPI_ERR_MODES_FAILED;
    if (cj_params)
    {
        ezlopi_service_modes_stop();

        ezlopi_service_modes_start();
        ret = EZPI_SUCCESS;
    }
    return ret;
}

ezlopi_error_t ezlopi_core_modes_add_alarm_off(uint8_t mode_id, cJSON*  device_id)
{
    ezlopi_error_t ret = EZPI_ERR_MODES_FAILED;
    if ((EZLOPI_HOUSE_MODE_REF_ID_NONE < mode_id) && (EZLOPI_HOUSE_MODE_REF_ID_MAX > mode_id) && device_id)
    {
        ezlopi_service_modes_stop();
        s_house_modes_t* targe_house_mode = ezlopi_core_modes_get_house_mode_by_id(mode_id);
        if (targe_house_mode)
        {
            cJSON* element_to_check = NULL;
            bool add_to_array = true;
            cJSON_ArrayForEach(element_to_check, targe_house_mode->cj_alarms_off_devices)
            {
                if (EZPI_STRNCMP_IF_EQUAL(device_id->valuestring, element_to_check->valuestring, strlen(device_id->valuestring), strlen(element_to_check->valuestring)))
                {
                    add_to_array = false;
                }
            }
            if (add_to_array)
            {
                if (NULL == targe_house_mode->cj_alarms_off_devices)
                {
                    targe_house_mode->cj_alarms_off_devices = cJSON_CreateArray(__func__);
                }
                cJSON_AddItemToArray(targe_house_mode->cj_alarms_off_devices, cJSON_CreateString(__func__, device_id->valuestring));


                if (EZPI_SUCCESS != ezlopi_core_modes_store_to_nvs())
                {
                    TRACE_D("Error!! when adding alarm_off");
                }
                else
                {
                    ret = EZPI_SUCCESS;
                }
            }
        }
        ezlopi_service_modes_start();
    }
    return ret;
}

ezlopi_error_t ezlopi_core_modes_remove_alarm_off(uint32_t mode_id, cJSON* device_id)
{
    ezlopi_error_t ret = EZPI_ERR_MODES_FAILED;
    if ((EZLOPI_HOUSE_MODE_REF_ID_NONE < mode_id) && (EZLOPI_HOUSE_MODE_REF_ID_MAX > mode_id) && device_id)
    {
        ezlopi_service_modes_stop();
        s_house_modes_t* targe_house_mode = ezlopi_core_modes_get_house_mode_by_id(mode_id);
        if (targe_house_mode)
        {
            cJSON* element_to_check = NULL;
            int array_index = 0;
            cJSON_ArrayForEach(element_to_check, targe_house_mode->cj_alarms_off_devices)
            {
                if (EZPI_STRNCMP_IF_EQUAL(device_id->valuestring, element_to_check->valuestring, strlen(device_id->valuestring), strlen(element_to_check->valuestring)))
                {
                    cJSON_DeleteItemFromArray(__func__, targe_house_mode->cj_alarms_off_devices, array_index);
                    break;
                }
                array_index++;
            }

            if (EZPI_SUCCESS != ezlopi_core_modes_store_to_nvs())
            {
                TRACE_D("Error!! when removing alarm_off");
            }
            else
            {
                ret = EZPI_SUCCESS;
            }
        }
        ezlopi_service_modes_start();
    }
    return ret;
}

ezlopi_error_t ezlopi_core_modes_set_protect(uint32_t mode_id, bool protect_state)
{
    ezlopi_error_t ret = EZPI_ERR_MODES_FAILED;
    if (sg_custom_modes)
    {
        ezlopi_service_modes_stop();
        s_house_modes_t* house_mode = NULL; /*0,1,2,3*/
        if (NULL != (house_mode = ezlopi_core_modes_get_house_mode_by_id(mode_id)))
        {
            house_mode->protect = protect_state;

            if (EZPI_SUCCESS != ezlopi_core_modes_store_to_nvs())
            {
                TRACE_E("Error!! , [id = %d] protection failed", mode_id);
            }
            else
            {
                ret = EZPI_SUCCESS;
            }
        }
        ezlopi_service_modes_start();
    }
    return ret;
}


ezlopi_error_t ezlopi_core_modes_set_entry_delay(uint32_t normal_sec, uint32_t short_sec, uint32_t extended_sec, uint32_t instant_sec)
{
    ezlopi_error_t ret = EZPI_ERR_MODES_FAILED;
    if (sg_custom_modes)
    {
        ezlopi_service_modes_stop();
        s_ezlopi_modes_t* curr_mode = ezlopi_core_modes_get_custom_modes();
        if (curr_mode)
        {
            curr_mode->entry_delay.normal_delay_sec = normal_sec;
            curr_mode->entry_delay.short_delay_sec = short_sec;
            curr_mode->entry_delay.extended_delay_sec = extended_sec;
            curr_mode->entry_delay.instant_delay_sec = instant_sec;

            if (EZPI_SUCCESS != ezlopi_core_modes_store_to_nvs())
            {
                TRACE_E("Error!! , failed to set new entry_dalay");
            }
            else
            {
                ret = EZPI_SUCCESS;
            }
        }
        ezlopi_service_modes_start();
    }

    return ret;
}

ezlopi_error_t ezlopi_core_modes_reset_entry_delay(void)
{
    ezlopi_error_t ret = EZPI_ERR_MODES_FAILED;
    if (sg_custom_modes)
    {
        ezlopi_service_modes_stop();
        s_ezlopi_modes_t * curr_mode = ezlopi_core_modes_get_custom_modes();
        if (curr_mode)
        {
            curr_mode->entry_delay.normal_delay_sec = 30;
            curr_mode->entry_delay.short_delay_sec = 30;
            curr_mode->entry_delay.extended_delay_sec = 30;
            curr_mode->entry_delay.instant_delay_sec = 0;

            if (EZPI_SUCCESS != ezlopi_core_modes_store_to_nvs())
            {
                TRACE_E("Error!! , failed to set new entry_dalay");
            }
            else
            {
                ret = EZPI_SUCCESS;
            }
        }
        ezlopi_service_modes_start();
    }

    return ret;
}


ezlopi_error_t ezlopi_core_modes_store_to_nvs(void)
{
    ezlopi_error_t ret = EZPI_ERR_MODES_FAILED;
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