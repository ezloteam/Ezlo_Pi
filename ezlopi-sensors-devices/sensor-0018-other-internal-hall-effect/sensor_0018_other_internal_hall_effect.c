/* ===========================================================================
** Copyright (C) 2024 Ezlo Innovation Inc
**
** Under EZLO AVAILABLE SOURCE LICENSE (EASL) AGREEMENT
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/

/**
 * @file    main.c
 * @brief   perform some function on data
 * @author  John Doe
 * @version 0.1
 * @date    1st January 2024
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_IDF_TARGET_ESP32

#include <math.h>
#include "ezlopi_util_trace.h"
// #include "esp_err.h"

// // #include "ezlopi_core_timer.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"
#include "ezlopi_core_processes.h"
#include "ezlopi_core_errors.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "sensor_0018_other_internal_hall_effect.h"
#include "EZLOPI_USER_CONFIG.h"

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/
const char *hall_door_window_states[] = {
    "dw_is_opened",
    "dw_is_closed",
    "unknown",
};

typedef struct s_hall_data
{
    bool calibration_complete;
    char *hall_state;
    int Custom_stable_val;
} s_hall_data_t;

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
static ezlopi_error_t __prepare(void *arg);
static ezlopi_error_t __init(l_ezlopi_item_t *item);
static ezlopi_error_t __get_item_cjson(l_ezlopi_item_t *item, void *arg);
static ezlopi_error_t __get_value_cjson(l_ezlopi_item_t *item, void *arg);
static ezlopi_error_t __notify(l_ezlopi_item_t *item);
static void __setup_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __setup_item_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_data);
static void __hall_calibration_task(void *params);

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/

/**
 * @brief Global/extern function template example
 * Convention : Use capital letter for initial word on extern function
 * @param arg
 */
ezlopi_error_t sensor_0018_other_internal_hall_effect(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    ezlopi_error_t ret = EZPI_SUCCESS;

    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = __prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = __init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    {
        ret = __get_item_cjson(item, arg);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = __get_value_cjson(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        ret = __notify(item);
        break;
    }

    default:
    {
        break;
    }
    }

    return ret;
}

/*******************************************************************************
 *                          Static Function Definitions
 *******************************************************************************/
static void __setup_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    device->cloud_properties.category = category_security_sensor;
    device->cloud_properties.subcategory = subcategory_door;
    device->cloud_properties.device_type = dev_type_doorlock;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __setup_item_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_data)
{
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.item_name = ezlopi_item_name_dw_state;
    item->cloud_properties.value_type = value_type_token;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = NULL;

    item->interface.gpio.gpio_in.gpio_num = GPIO_NUM_36;
    item->interface_type = EZLOPI_DEVICE_INTERFACE_DIGITAL_INPUT;

    item->is_user_arg_unique = true;
    item->user_arg = user_data;
}

static ezlopi_error_t __prepare(void *arg)
{
    ezlopi_error_t ret = EZPI_SUCCESS;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (arg && prep_arg->cjson_device)
    {
        cJSON *cj_device = prep_arg->cjson_device;
        s_hall_data_t *user_data = (s_hall_data_t *)ezlopi_malloc(__FUNCTION__, sizeof(s_hall_data_t));
        if (user_data)
        {
            memset(user_data, 0, sizeof(s_hall_data_t));
            l_ezlopi_device_t *hall_device = ezlopi_device_add_device(cj_device, NULL);
            if (hall_device)
            {
                __setup_device_cloud_properties(hall_device, cj_device);
                l_ezlopi_item_t *hall_item = ezlopi_device_add_item_to_device(hall_device, sensor_0018_other_internal_hall_effect);
                if (hall_item)
                {
                    __setup_item_properties(hall_item, cj_device, user_data);
                }
                else
                {
                    ezlopi_device_free_device(hall_device);
                    ezlopi_free(__FUNCTION__, user_data);
                    ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
                }
            }
            else
            {
                ezlopi_free(__FUNCTION__, user_data);
                ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
            }
        }
        else
        {
            ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
        }
    }
    else
    {
        ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
    }
    return ret;
}

static ezlopi_error_t __init(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_SUCCESS;
    if (item)
    {
#ifdef CONFIG_IDF_TARGET_ESP32
        esp_err_t error = adc1_config_width(ADC_WIDTH_BIT_12);
#else
        esp_err_t error = ESP_ERR_NOT_FOUND;
#endif
        s_hall_data_t *user_data = (s_hall_data_t *)item->user_arg;
        if (user_data)
        {
            if (ESP_OK == error)
            {
                TRACE_I("Width configuration was successfully done!");
                TRACE_W("Calibrating.....");
                user_data->hall_state = "dw_is_closed";
                TaskHandle_t ezlopi_sensor_hall_callibration_task_handle = NULL;
                xTaskCreate(__hall_calibration_task, "Hall_Calibration_Task", EZLOPI_SENSOR_HALL_CALLIBRATION_TASK_DEPTH, item, 1, &ezlopi_sensor_hall_callibration_task_handle);
                ezlopi_core_process_set_process_info(ENUM_EZLOPI_SENSOR_HALL_CALLIBRATION_TASK, &ezlopi_sensor_hall_callibration_task_handle, EZLOPI_SENSOR_HALL_CALLIBRATION_TASK_DEPTH);
            }
            else
            {
                TRACE_E("Error 'sensor_door_init'. error: %s)", esp_err_to_name(error));
                ret = EZPI_ERR_INIT_DEVICE_FAILED;
            }
        }
        else
        {
            ret = EZPI_ERR_INIT_DEVICE_FAILED;
        }
    }
    else
    {
        ret = EZPI_ERR_INIT_DEVICE_FAILED;
    }
    return ret;
}

static ezlopi_error_t __get_item_cjson(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item && arg)
    {
        s_hall_data_t *user_data = (s_hall_data_t *)item->user_arg;
        if (user_data)
        {
            cJSON *cj_result = (cJSON *)arg;
            if (cj_result)
            {
                //-------------------  POSSIBLE JSON ENUM LPGNTENTS ----------------------------------
                cJSON *json_array_enum = cJSON_CreateArray(__FUNCTION__);
                if (NULL != json_array_enum)
                {
                    for (uint8_t i = 0; i < HALL_DOOR_WINDOW_MAX; i++)
                    {
                        cJSON *json_value = cJSON_CreateString(__FUNCTION__, hall_door_window_states[i]);
                        if (NULL != json_value)
                        {
                            cJSON_AddItemToArray(json_array_enum, json_value);
                        }
                    }
                    cJSON_AddItemToObject(__FUNCTION__, cj_result, ezlopi_enum_str, json_array_enum);
                }
                //--------------------------------------------------------------------------------------
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_value_str, user_data->hall_state);
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_valueFormatted_str, user_data->hall_state);
                ret = EZPI_SUCCESS;
            }
        }
    }
    return ret;
}

static ezlopi_error_t __get_value_cjson(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item && arg)
    {
        s_hall_data_t *user_data = (s_hall_data_t *)item->user_arg;
        if (user_data)
        {
            cJSON *cj_result = (cJSON *)arg;
            if (cj_result)
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_valueFormatted_str, user_data->hall_state);
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_value_str, user_data->hall_state);
                ret = EZPI_SUCCESS;
            }
        }
    }
    return ret;
}

static ezlopi_error_t __notify(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item)
    {
        s_hall_data_t *user_data = (s_hall_data_t *)item->user_arg;
        if (user_data)
        {
            if (user_data->calibration_complete)
            {
                char *curret_value = NULL;
#ifdef CONFIG_IDF_TARGET_ESP32
                int sensor_data = hall_sensor_read();
#else
                int sensor_data = 0;
#endif
                TRACE_D(" Hall door value ; %d", sensor_data);

                curret_value = ((fabs(user_data->Custom_stable_val - sensor_data) > 35) ? "dw_is_closed" : "dw_is_opened");

                if (curret_value != user_data->hall_state) // calls update only if there is change in state
                {
                    user_data->hall_state = curret_value;
                    ezlopi_device_value_updated_from_device_broadcast(item);
                }
                ret = EZPI_SUCCESS;
            }
        }
    }
    return ret;
}

static void __hall_calibration_task(void *params) // calibrate task
{
    l_ezlopi_item_t *item = (l_ezlopi_item_t *)params;
    if (item)
    {
        s_hall_data_t *user_data = (s_hall_data_t *)item->user_arg;
        if (user_data)
        {

            float sensor_data = (float)hall_sensor_read();
            for (uint8_t i = 0; i < 10; i++)
            {
                sensor_data = (sensor_data * .8f) + (.2f * (float)hall_sensor_read());
                vTaskDelay(500);
            }
            user_data->Custom_stable_val = (int)sensor_data;
            TRACE_W("Calibration Complete...... Stable_hall is : [%d]", user_data->Custom_stable_val);
            user_data->calibration_complete = true;
        }
    }
    ezlopi_core_process_set_is_deleted(ENUM_EZLOPI_SENSOR_HALL_CALLIBRATION_TASK);
    vTaskDelete(NULL);
}

#endif // CONFIG_IDF_TARGET_ESP32

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
