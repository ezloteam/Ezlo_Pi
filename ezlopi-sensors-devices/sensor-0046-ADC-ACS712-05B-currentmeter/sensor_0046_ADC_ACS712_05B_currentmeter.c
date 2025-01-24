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
 * @file    sensor_0046_ADC_ACS712_05B_currentmeter.c
 * @brief   perform some function on sensor_0046
 * @author  xx
 * @version 0.1
 * @date    xx
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <math.h>

#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_hal_adc.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "sensor_0046_ADC_ACS712_05B_currentmeter.h"
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
typedef struct s_currentmeter
{
    float amp_value;
} s_currentmeter_t;

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
static ezlopi_error_t __0046_prepare(void *arg);
static ezlopi_error_t __0046_init(l_ezlopi_item_t *item);
static ezlopi_error_t __0046_get_cjson_value(l_ezlopi_item_t *item, void *arg);
static ezlopi_error_t __0046_notify(l_ezlopi_item_t *item);
static void __calculate_current_value(l_ezlopi_item_t *item);

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
ezlopi_error_t SENSOR_0046_adc_acs712_05b_currentmeter(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    ezlopi_error_t ret = EZPI_SUCCESS;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = __0046_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = __0046_init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = __0046_get_cjson_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        ret = __0046_notify(item);
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
 *                         Static Function Definitions
 *******************************************************************************/

static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    device->cloud_properties.category = category_level_sensor;
    device->cloud_properties.subcategory = subcategory_electricity;
    device->cloud_properties.device_type_id = NULL;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type = dev_type_sensor;
}

static void __prepare_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_data)
{
    item->cloud_properties.item_id = EZPI_core_cloud_generate_item_id();
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_current;
    item->cloud_properties.show = true;
    item->cloud_properties.value_type = value_type_electric_current;
    item->cloud_properties.scale = scales_ampere;

    CJSON_GET_VALUE_INT(cj_device, ezlopi_dev_type_str, item->interface_type); // _max = 10
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_str, item->interface.adc.gpio_num);
    item->interface.adc.resln_bit = 3; // ADC 12_bit

    // passing the custom data_structure
    item->is_user_arg_unique = true;
    item->user_arg = user_data;
}
static ezlopi_error_t __0046_prepare(void *arg)
{
    ezlopi_error_t ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (device_prep_arg && (NULL != device_prep_arg->cjson_device))
    {
        s_currentmeter_t *user_data = (s_currentmeter_t *)ezlopi_malloc(__FUNCTION__, sizeof(s_currentmeter_t));
        if (user_data)
        {
            memset(user_data, 0, sizeof(s_currentmeter_t));
            l_ezlopi_device_t *currentmeter_device = EZPI_core_device_add_device(device_prep_arg->cjson_device, NULL);
            if (currentmeter_device)
            {
                __prepare_device_cloud_properties(currentmeter_device, device_prep_arg->cjson_device);
                l_ezlopi_item_t *currentmeter_item = EZPI_core_device_add_item_to_device(currentmeter_device, SENSOR_0046_adc_acs712_05b_currentmeter);
                if (currentmeter_item)
                {
                    __prepare_item_cloud_properties(currentmeter_item, device_prep_arg->cjson_device, user_data);
                    ret = EZPI_SUCCESS;
                }
                else
                {
                    EZPI_core_device_free_device(currentmeter_device);
                    ezlopi_free(__FUNCTION__, user_data);
                }
            }
            else
            {
                ezlopi_free(__FUNCTION__, user_data);
            }
        }
    }
    return ret;
}
static ezlopi_error_t __0046_init(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_ERR_INIT_DEVICE_FAILED;
    if (item)
    {
        s_currentmeter_t *user_data = (s_currentmeter_t *)item->user_arg;
        if (user_data)
        {
            if (GPIO_IS_VALID_GPIO(item->interface.gpio.gpio_in.gpio_num))
            {
                if (EZPI_SUCCESS == EZPI_hal_adc_init(item->interface.adc.gpio_num, item->interface.adc.resln_bit))
                {
                    ret = EZPI_SUCCESS;
                }
            }
        }
    }
    return ret;
}
static ezlopi_error_t __0046_get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        s_currentmeter_t *user_data = (s_currentmeter_t *)item->user_arg;
        if (user_data)
        {
            EZPI_core_valueformatter_float_to_cjson(cj_result, user_data->amp_value, scales_ampere);
            ret = EZPI_SUCCESS;
        }
    }

    return ret;
}
static ezlopi_error_t __0046_notify(l_ezlopi_item_t *item)
{
    // During this calculation the system is polled for 20mS
    ezlopi_error_t ret = EZPI_FAILED;
    if (item)
    {
        s_currentmeter_t *user_data = (s_currentmeter_t *)item->user_arg;
        if (user_data)
        {
            float prev_amp = user_data->amp_value;
            __calculate_current_value(item); // update amp
            if (fabs(user_data->amp_value - prev_amp) > 0.5)
            {
                EZPI_core_device_value_updated_from_device_broadcast(item);
                ret = EZPI_SUCCESS;
            }
        }
    }
    return ret;
}
static void __calculate_current_value(l_ezlopi_item_t *item)
{
    if (NULL != item)
    {
        s_currentmeter_t *user_data = (s_currentmeter_t *)item->user_arg;
        if (user_data)
        {
            s_ezlopi_analog_data_t ezlopi_analog_data = {.value = 0, .voltage = 0};

            uint32_t period_dur = (1000000 / DEFAULT_AC_FREQUENCY); // 20000uS
            int Vnow = 0;
            uint32_t Vsum = 0;
            uint32_t measurements_count = 0;

            // starting 't' instant
            uint32_t t_start = (uint32_t)esp_timer_get_time();
            uint32_t Volt = 0;
            int diff = 0;

            while (((uint32_t)esp_timer_get_time() - t_start) < period_dur) // loops within 1-complete cycle
            {
                EZPI_hal_adc_get_adc_data(item->interface.adc.gpio_num, &ezlopi_analog_data);
                Volt = 2 * (ezlopi_analog_data.voltage); // since the input is half the original value after voltage division
                diff = ((ASC712TELC_05B_zero_point_mV - Volt) > 0 ? (ASC712TELC_05B_zero_point_mV - Volt) : (Volt - ASC712TELC_05B_zero_point_mV));
                // getting the voltage value at this instant
                if (diff > 150 && diff < 1500) // the reading voltage less than 00mV is noise
                {
                    Vnow = Volt - ASC712TELC_05B_zero_point_mV; // ()at zero offset => full-scale/2
                }
                else
                {
                    Vnow = 0;
                }
                Vsum += (Vnow * Vnow); // sumof(I^2 + I^2 + .....)
                measurements_count++;
            }

            // If applied for DC;  'AC_Irms' calculation give same value as 'DC-value'
            if (0 == measurements_count)
            {
                measurements_count = 1; // <-- avoid dividing by zero
            }

            user_data->amp_value = ((float)sqrt(Vsum / measurements_count)) / 185.0f; //  -> I[rms] Ampere
            if ((user_data->amp_value) < 0.4)
            {
                user_data->amp_value = 0;
            }
        }
    }
}

/*******************************************************************************
 *                          End of File
 *******************************************************************************/