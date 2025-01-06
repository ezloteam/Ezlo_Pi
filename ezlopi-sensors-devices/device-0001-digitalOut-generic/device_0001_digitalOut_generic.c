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
 * @file    device_0001_digitalOut_generic.c
 * @brief   perform some function on device_0001
 * @author  xx
 * @version 0.1
 * @date    xx
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "../../build/config/sdkconfig.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_hal_gpio.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "ezlopi_service_gpioisr.h"
#include "device_0001_digitalOut_generic.h"
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
static ezlopi_error_t __prepare(void *arg);
static ezlopi_error_t __init(l_ezlopi_item_t *item);
static ezlopi_error_t __set_value(l_ezlopi_item_t *item, void *arg);
static ezlopi_error_t __get_value_cjson(l_ezlopi_item_t *item, void *arg);

static void __interrupt_upcall(void *arg);
static void __toggle_gpio(l_ezlopi_item_t *item);
static void __write_gpio_value(l_ezlopi_item_t *item);
static void __set_gpio_value(l_ezlopi_item_t *item, int value);

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
// #define DEV_TEST_SETTINGS_EN
#ifdef DEV_TEST_SETTINGS_EN
static ezlopi_error_t __settings_callback(e_ezlopi_settings_action_t action, struct l_ezlopi_device_settings_v3 *setting, void *arg, void *user_arg);
static ezlopi_error_t __settings_get(void *arg, l_ezlopi_device_settings_v3_t *setting);
static ezlopi_error_t __settings_set(void *arg, l_ezlopi_device_settings_v3_t *setting);
static ezlopi_error_t __settings_reset(void *arg, l_ezlopi_device_settings_v3_t *setting);
static ezlopi_error_t __settings_update(void *arg, l_ezlopi_device_settings_v3_t *setting);

// static const char *nvs_key_backlight_brightness = "bklt";

uint32_t settings_ids[2];
#endif // DEV_TEST_SETTINGS_EN

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
ezlopi_error_t DEVICE_0001_digitalOut_generic(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    ezlopi_error_t error = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        error = __prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        error = __init(item);
        break;
    }
    case EZLOPI_ACTION_SET_VALUE:
    {
        error = __set_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        error = __get_value_cjson(item, arg);
        break;
    }

    default:
    {
        break;
    }
    }

    return error;
}

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/
static void __setup_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cjson_device)
{
    device->cloud_properties.category = category_switch;
    device->cloud_properties.subcategory = subcategory_in_wall;
    device->cloud_properties.device_type = dev_type_switch_inwall;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}

static void __setup_item_properties(l_ezlopi_item_t *item, cJSON *cjson_device)
{
    int tmp_var = 0;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = true;
    item->cloud_properties.item_name = ezlopi_item_name_switch;
    item->cloud_properties.value_type = value_type_bool;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = NULL;
    item->cloud_properties.item_id = EZPI_core_cloud_generate_item_id();

    CJSON_GET_VALUE_DOUBLE(cjson_device, ezlopi_dev_type_str, item->interface_type);

    CJSON_GET_VALUE_BOOL(cjson_device, ezlopi_is_ip_str, item->interface.gpio.gpio_in.enable);
    if (item->interface.gpio.gpio_in.enable)
    {
        CJSON_GET_VALUE_GPIO(cjson_device, ezlopi_gpio_in_str, item->interface.gpio.gpio_in.gpio_num);
        CJSON_GET_VALUE_DOUBLE(cjson_device, ezlopi_ip_inv_str, item->interface.gpio.gpio_in.invert);
        CJSON_GET_VALUE_DOUBLE(cjson_device, ezlopi_val_ip_str, item->interface.gpio.gpio_in.value);
        CJSON_GET_VALUE_DOUBLE(cjson_device, ezlopi_pullup_ip_str, tmp_var);
        item->interface.gpio.gpio_in.pull = tmp_var ? GPIO_PULLUP_ONLY : GPIO_PULLDOWN_ONLY;
        // item->interface.gpio.gpio_in.interrupt = GPIO_INTR_NEGEDGE;
        item->interface.gpio.gpio_in.interrupt = GPIO_INTR_DISABLE;
    }
    // else
    // {
    //     item->interface.gpio.gpio_in.interrupt = GPIO_INTR_DISABLE;
    // }

    item->interface.gpio.gpio_out.enable = true;
    CJSON_GET_VALUE_GPIO(cjson_device, ezlopi_gpio_out_str, item->interface.gpio.gpio_out.gpio_num);
    CJSON_GET_VALUE_DOUBLE(cjson_device, ezlopi_op_inv_str, item->interface.gpio.gpio_out.invert);
    CJSON_GET_VALUE_DOUBLE(cjson_device, ezlopi_val_op_str, item->interface.gpio.gpio_out.value);
    CJSON_GET_VALUE_DOUBLE(cjson_device, ezlopi_pullup_op_str, tmp_var);
    item->interface.gpio.gpio_out.interrupt = GPIO_INTR_DISABLE;

    item->interface.gpio.gpio_out.pull = tmp_var ? GPIO_PULLUP_ONLY : GPIO_PULLDOWN_ONLY;

    TRACE_D("item->interface.gpio.gpio_in.gpio_num:  %u", item->interface.gpio.gpio_in.gpio_num);
    TRACE_D("item->interface.gpio.gpio_out.gpio_num: %u", item->interface.gpio.gpio_out.gpio_num);
}

static ezlopi_error_t __prepare(void *arg)
{
    ezlopi_error_t error = EZPI_ERR_PREP_DEVICE_PREP_FAILED;

#ifdef DEV_TEST_SETTINGS_EN
    settings_ids[0] = EZPI_core_cloud_generate_settings_id();
    settings_ids[1] = EZPI_core_cloud_generate_settings_id();
#endif

    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (arg)
    {
        cJSON *cjson_device = prep_arg->cjson_device;
        if (cjson_device)
        {
            l_ezlopi_device_t *device = EZPI_core_device_add_device(cjson_device, NULL);
            if (device)
            {
                __setup_device_cloud_properties(device, cjson_device);
                l_ezlopi_item_t *item = EZPI_core_device_add_item_to_device(device, DEVICE_0001_digitalOut_generic);
                if (item)
                {
                    // item->cloud_properties.device_id = device->cloud_properties.device_id;
                    __setup_item_properties(item, cjson_device);
                    error = EZPI_SUCCESS;
                }

#ifdef DEV_TEST_SETTINGS_EN
                l_ezlopi_device_settings_v3_t *setting_user_defined = EZPI_core_device_add_settings_to_device_v3(device, __settings_callback);
                if (setting_user_defined)
                {
                    setting_user_defined->cloud_properties.setting_id = settings_ids[0];
                    error = EZPI_SUCCESS;
                }
                l_ezlopi_device_settings_v3_t *setting_brightness = EZPI_core_device_add_settings_to_device_v3(device, __settings_callback);
                if (setting_brightness)
                {

                    setting_brightness->cloud_properties.setting_id = settings_ids[1];

                    s_digio_settings_t *settings_value = (s_digio_settings_t *)ezlopi_malloc(__FUNCTION__, sizeof(s_digio_settings_t));
                    memset(settings_value, 0, sizeof(s_digio_settings_t));

                    if (EZPI_SUCCESS == EZPI_core_nvs_read_int32(&settings_value->settings_int_data, "bklt"))
                    {
                    }
                    else
                    {
                        settings_value->settings_int_data = 50;
                        if (EZPI_SUCCESS != EZPI_core_nvs_write_int32(settings_value->settings_int_data, "bklt"))
                        {
                            TRACE_E("Error Updating settings values to NVS");
                        }
                    }

                    setting_brightness->user_arg = (void *)settings_value;

                    TRACE_E("settings_data: %p", settings_value);
                    TRACE_E("setting->user_arg: %p", setting_brightness->user_arg);

                    error = EZPI_SUCCESS;
                }
#else
                else
                {
                    EZPI_core_device_free_device(device);
                }
#endif // DEV_TEST_SETTINGS_EN
            }
        }
    }

    return error;
}

static ezlopi_error_t __init(l_ezlopi_item_t *item)
{
    ezlopi_error_t error = EZPI_ERR_INIT_DEVICE_FAILED;
    if (item)
    {
        if (GPIO_IS_VALID_GPIO(item->interface.gpio.gpio_out.gpio_num) &&
            (255 != item->interface.gpio.gpio_out.gpio_num))
        {
            const gpio_config_t io_conf = {
                .pin_bit_mask = (1ULL << item->interface.gpio.gpio_out.gpio_num),
                .mode = GPIO_MODE_OUTPUT,
                .pull_up_en = ((item->interface.gpio.gpio_out.pull == GPIO_PULLUP_ONLY) ||
                               (item->interface.gpio.gpio_out.pull == GPIO_PULLUP_PULLDOWN))
                                  ? GPIO_PULLUP_ENABLE
                                  : GPIO_PULLUP_DISABLE,
                .pull_down_en = ((item->interface.gpio.gpio_out.pull == GPIO_PULLDOWN_ONLY) ||
                                 (item->interface.gpio.gpio_out.pull == GPIO_PULLUP_PULLDOWN))
                                    ? GPIO_PULLDOWN_ENABLE
                                    : GPIO_PULLDOWN_DISABLE,
                .intr_type = GPIO_INTR_DISABLE,
            };

            if (0 == gpio_config(&io_conf))
            {
                // digital_io_write_gpio_value(item);
                __write_gpio_value(item);
                error = EZPI_SUCCESS;
            }
        }
    }

    if (GPIO_IS_VALID_GPIO(item->interface.gpio.gpio_in.gpio_num) &&
        (-1 != item->interface.gpio.gpio_in.gpio_num) &&
        (255 != item->interface.gpio.gpio_in.gpio_num))
    {
        const gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL << item->interface.gpio.gpio_in.gpio_num),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = ((item->interface.gpio.gpio_in.pull == GPIO_PULLUP_ONLY) ||
                           (item->interface.gpio.gpio_in.pull == GPIO_PULLUP_PULLDOWN))
                              ? GPIO_PULLUP_ENABLE
                              : GPIO_PULLUP_DISABLE,
            .pull_down_en = ((item->interface.gpio.gpio_in.pull == GPIO_PULLDOWN_ONLY) ||
                             (item->interface.gpio.gpio_in.pull == GPIO_PULLUP_PULLDOWN))
                                ? GPIO_PULLDOWN_ENABLE
                                : GPIO_PULLDOWN_DISABLE,
            .intr_type = (GPIO_PULLUP_ONLY == item->interface.gpio.gpio_in.pull)
                             ? GPIO_INTR_POSEDGE
                             : GPIO_INTR_NEGEDGE,
        };

        TRACE_D("enabling interrup for pin: %d", item->interface.gpio.gpio_in.gpio_num);

        gpio_config(&io_conf);
        EZPI_service_gpioisr_register_v3(item, __interrupt_upcall, 1000);
        error = EZPI_SUCCESS;
    }

    if (EZPI_SUCCESS == error)
    {
        error = EZPI_ERR_INIT_DEVICE_FAILED;
        if (item->user_arg)
        {
            const gpio_config_t io_conf = {
                .pin_bit_mask = (1ULL << item->interface.gpio.gpio_in.gpio_num),
                .mode = GPIO_MODE_INPUT,
                .pull_up_en = ((item->interface.gpio.gpio_in.pull == GPIO_PULLUP_ONLY) ||
                               (item->interface.gpio.gpio_in.pull == GPIO_PULLUP_PULLDOWN))
                                  ? GPIO_PULLUP_ENABLE
                                  : GPIO_PULLUP_DISABLE,
                .pull_down_en = ((item->interface.gpio.gpio_in.pull == GPIO_PULLDOWN_ONLY) ||
                                 (item->interface.gpio.gpio_in.pull == GPIO_PULLUP_PULLDOWN))
                                    ? GPIO_PULLDOWN_ENABLE
                                    : GPIO_PULLDOWN_DISABLE,
                .intr_type = (GPIO_PULLUP_ONLY == item->interface.gpio.gpio_in.pull)
                                 ? GPIO_INTR_POSEDGE
                                 : GPIO_INTR_NEGEDGE,
            };

            if (0 == gpio_config(&io_conf))
            {
                EZPI_service_gpioisr_register_v3(item, __interrupt_upcall, 1000);
                error = EZPI_SUCCESS;
            }
        }
    }

    return error;
}

static ezlopi_error_t __get_value_cjson(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t error = EZPI_FAILED;
    if (item && arg)
    {
        cJSON *cj_propertise = (cJSON *)arg;
        if (cj_propertise)
        {
            EZPI_core_valueformatter_bool_to_cjson(cj_propertise, item->interface.gpio.gpio_out.value, NULL);
            error = EZPI_SUCCESS;
        }
    }
    return error;
}

static void __set_gpio_value(l_ezlopi_item_t *item, int value)
{
    gpio_set_level(item->interface.gpio.gpio_out.gpio_num, value);
    item->interface.gpio.gpio_out.value = value;
}

static ezlopi_error_t __set_value(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t error = EZPI_FAILED;
    if (item && arg)
    {
        cJSON *cjson_params = (cJSON *)arg;

        if (NULL != cjson_params)
        {
            // CJSON_TRACE("cjson_params", cjson_params);

            int value = 0;
            cJSON *cj_value = cJSON_GetObjectItem(__FUNCTION__, cjson_params, ezlopi_value_str);
            if (cj_value)
            {
                switch (cj_value->type)
                {
                case cJSON_False:
                {
                    value = 0;
                    break;
                }
                case cJSON_True:
                {
                    value = 1;
                    break;
                }
                case cJSON_Number:
                {
                    value = cj_value->valuedouble;
                    break;
                }
                default:
                {
                    break;
                }
                }
            }

            if (255 != item->interface.gpio.gpio_out.gpio_num)
            {
                if (GPIO_IS_VALID_GPIO(item->interface.gpio.gpio_out.gpio_num))
                {
                    __set_gpio_value(item, value);
                    EZPI_core_device_value_updated_from_device_broadcast(item);
                    error = EZPI_SUCCESS;
                }
            }
            else
            {
                l_ezlopi_device_t *curr_device = EZPI_core_device_get_head();
                while (curr_device)
                {
                    l_ezlopi_item_t *curr_item = curr_device->items;
                    while (curr_item)
                    {
                        if ((EZLOPI_DEVICE_INTERFACE_DIGITAL_OUTPUT == curr_item->interface_type) && (255 != curr_item->interface.gpio.gpio_out.gpio_num))
                        {
                            TRACE_D("GPIO-pin: %d", curr_item->interface.gpio.gpio_out.gpio_num);
                            TRACE_D("value: %d", value);
                            __set_gpio_value(curr_item, value);
                            EZPI_core_device_value_updated_from_device_broadcast(curr_item);
                            error = EZPI_SUCCESS;
                        }
                        curr_item = curr_item->next;
                    }
                    curr_device = curr_device->next;
                }

                item->interface.gpio.gpio_out.value = value;
                EZPI_core_device_value_updated_from_device_broadcast(item);
            }
        }
    }
    return error;
}

static void __write_gpio_value(l_ezlopi_item_t *item)
{
    uint32_t write_value = (0 == item->interface.gpio.gpio_out.invert) ? item->interface.gpio.gpio_out.value : (item->interface.gpio.gpio_out.value ? 0 : 1);
    gpio_set_level(item->interface.gpio.gpio_out.gpio_num, write_value);
}

static void __interrupt_upcall(void *arg)
{
    l_ezlopi_item_t *item = (l_ezlopi_item_t *)arg;
    if (item)
    {
        __toggle_gpio(item);
        EZPI_core_device_value_updated_from_device_broadcast(item);
    }
}

static void __toggle_gpio(l_ezlopi_item_t *item)
{
    uint32_t write_value = !(item->interface.gpio.gpio_out.value);
    gpio_set_level(item->interface.gpio.gpio_out.gpio_num, write_value);
    item->interface.gpio.gpio_out.value = write_value;
}

#ifdef DEV_TEST_SETTINGS_EN
static ezlopi_error_t __settings_callback(e_ezlopi_settings_action_t action, struct l_ezlopi_device_settings_v3 *setting, void *arg, void *user_arg)
{
    ezlopi_error_t error = 1;
    switch (action)
    {
    case EZLOPI_SETTINGS_ACTION_GET_SETTING:
    {
        __settings_get(arg, setting);
        break;
    }
    case EZLOPI_SETTINGS_ACTION_SET_SETTING:
    {
        __settings_set(arg, setting);
        EZPI_core_device_value_updated_settings_broadcast(setting);
        break;
    }
    case EZLOPI_SETTINGS_ACTION_RESET_SETTING:
    {
        __settings_reset(arg, setting);
        EZPI_core_device_value_updated_settings_broadcast(setting);
        break;
    }
    case EZLOPI_SETTINGS_ACTION_UPDATE_SETTING:
    {
        __settings_update(arg, setting);
        break;
    }
    default:
        break;
    }
    return error;
}

static ezlopi_error_t __settings_get(void *arg, l_ezlopi_device_settings_v3_t *setting)
{
    ezlopi_error_t errror = EZPI_FAILED;
    cJSON *cjson_propertise = (cJSON *)arg;
    if (cjson_propertise)
    {
        if (setting->cloud_properties.setting_id == settings_ids[0])
        {

            cJSON *label = cJSON_CreateObject(__FUNCTION__);
            cJSON *description = cJSON_CreateObject(__FUNCTION__);
            cJSON *value = cJSON_CreateObject(__FUNCTION__);
            cJSON *value_default = cJSON_CreateObject(__FUNCTION__);

            cJSON_AddStringToObject(__FUNCTION__, label, ezlopi_text_str, "User defined mode");
            cJSON_AddStringToObject(__FUNCTION__, label, ezlopi_lang_tag_str, "ezlopi_presence_user_defined_mode_label");

            cJSON_AddStringToObject(__FUNCTION__, description, ezlopi_text_str, "User defined operation mode, where user can set several distance parameters for setting a custom operation mode");
            cJSON_AddStringToObject(__FUNCTION__, description, ezlopi_lang_tag_str, "ezlopi_presence_user_defined_mode_description");

            cJSON_AddItemToObject(__FUNCTION__, cjson_propertise, ezlopi_label_str, label);
            cJSON_AddItemToObject(__FUNCTION__, cjson_propertise, ezlopi_description_str, description);
            cJSON_AddStringToObject(__FUNCTION__, cjson_propertise, ezlopi_valueType_str, "presence_operation_mode");

            cJSON_AddNumberToObject(__FUNCTION__, value, ezlopi_min_move_distance_str, 0.75);
            cJSON_AddNumberToObject(__FUNCTION__, value, ezlopi_max_move_distance_str, 1.5);
            cJSON_AddNumberToObject(__FUNCTION__, value, ezlopi_min_still_distance_str, 0.75);
            cJSON_AddNumberToObject(__FUNCTION__, value, ezlopi_max_still_distance_str, 1.5);
            cJSON_AddTrueToObject(__FUNCTION__, value, ezlopi_is_active_str);
            cJSON_AddItemToObject(__FUNCTION__, cjson_propertise, ezlopi_value_str, value);

            cJSON_AddNumberToObject(__FUNCTION__, value_default, ezlopi_min_move_distance_str, 0.75);
            cJSON_AddNumberToObject(__FUNCTION__, value_default, ezlopi_max_move_distance_str, 6.0);
            cJSON_AddNumberToObject(__FUNCTION__, value_default, ezlopi_min_still_distance_str, 0.75);
            cJSON_AddNumberToObject(__FUNCTION__, value_default, ezlopi_max_still_distance_str, 6.0);
            cJSON_AddFalseToObject(__FUNCTION__, value_default, ezlopi_is_active_str);
            cJSON_AddItemToObject(__FUNCTION__, cjson_propertise, ezlopi_valueDefault_str, value_default);
        }
        else if (setting->cloud_properties.setting_id == settings_ids[1])
        {
            cJSON *label = cJSON_CreateObject(__FUNCTION__);
            cJSON *description = cJSON_CreateObject(__FUNCTION__);

            cJSON_AddStringToObject(__FUNCTION__, label, ezlopi_text_str, "Backlight Brightness");
            cJSON_AddStringToObject(__FUNCTION__, label, ezlopi_lang_tag_str, "ezlopi_digitalio_pwm_setting_label");

            cJSON_AddStringToObject(__FUNCTION__, description, ezlopi_text_str, "This is PWM setting value for setting the backlight brightness");
            cJSON_AddStringToObject(__FUNCTION__, description, ezlopi_lang_tag_str, "ezlopi_digitalio_pwm_setting_description");

            cJSON_AddItemToObject(__FUNCTION__, cjson_propertise, ezlopi_label_str, label);
            cJSON_AddItemToObject(__FUNCTION__, cjson_propertise, ezlopi_description_str, description);
            cJSON_AddStringToObject(__FUNCTION__, cjson_propertise, ezlopi_valueType_str, "int");

            s_digio_settings_t *settings_data = (s_digio_settings_t *)setting->user_arg;

            cJSON_AddNumberToObject(__FUNCTION__, cjson_propertise, ezlopi_value_str, settings_data->settings_int_data);
            cJSON_AddNumberToObject(__FUNCTION__, cjson_propertise, "valueMin", 0);
            cJSON_AddNumberToObject(__FUNCTION__, cjson_propertise, ezlopi_valueMax_str, 100);
            cJSON_AddNumberToObject(__FUNCTION__, cjson_propertise, ezlopi_valueDefault_str, 50);
        }

        error = EZPI_SUCCESS;
    }

    return error;
}
static ezlopi_error_t __settings_set(void *arg, l_ezlopi_device_settings_v3_t *setting)
{
    ezlopi_error_t error = EZPI_FAILED;
    cJSON *cjson_propertise = (cJSON *)arg;
    if (cjson_propertise)
    {
        if (setting->cloud_properties.setting_id == settings_ids[0])
        {
        }
        else if (setting->cloud_properties.setting_id == settings_ids[1])
        {
            int32_t value = 0;
            CJSON_GET_VALUE_DOUBLE(cjson_propertise, ezlopi_value_str, value);
            TRACE_D("Setting Value : %d", value);

            s_digio_settings_t *settings_data = (s_digio_settings_t *)setting->user_arg;
            settings_data->settings_int_data = value;

            if (EZPI_SUCCESS != EZPI_core_nvs_write_int32(value, "bklt"))
            {
                TRACE_E("Error Updating settings values to NVS");
            }
        }

        error = EZPI_SUCCESS;
    }
    return error
}
static ezlopi_error_t __settings_reset(void *arg, l_ezlopi_device_settings_v3_t *setting)
{
    ezlopi_error_t error = EZPI_SUCCESS;
    if (setting->cloud_properties.setting_id == settings_ids[0])
    {
    }
    else if (setting->cloud_properties.setting_id == settings_ids[1])
    {
        s_digio_settings_t *settings_data = (s_digio_settings_t *)setting->user_arg;
        settings_data->settings_int_data = 50; // 50 being default value

        if (EZPI_SUCCESS != EZPI_core_nvs_write_int32(settings_data->settings_int_data, "bklt"))
        {
            TRACE_E("Error Updating settings values to NVS");
            error = EZPI_FAILED;
        }
    }
    return error;
}

static ezlopi_error_t __settings_update(void *arg, l_ezlopi_device_settings_v3_t *setting)
{
    ezlopi_error_t error = EZPI_SUCCESS;
    cJSON *cjson_propertise = (cJSON *)arg;
    if (cjson_propertise)
    {
        if (setting->cloud_properties.setting_id == settings_ids[0])
        {
        }
        else if (setting->cloud_properties.setting_id == settings_ids[1])
        {
            s_digio_settings_t *settings_data = (s_digio_settings_t *)setting->user_arg;
            cJSON_AddNumberToObject(__FUNCTION__, cjson_propertise, ezlopi_value_str, settings_data->settings_int_data);
        }
    }
    return error;
}

#endif // DEV_TEST_SETTINGS_EN

/*******************************************************************************
 *                          End of File
 *******************************************************************************/