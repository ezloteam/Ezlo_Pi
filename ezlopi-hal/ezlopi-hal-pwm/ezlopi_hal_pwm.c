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
* @file    ezlopi_hal_pwm.c
* @brief   perform some function on PWM
* @author  xx
* @version 0.1
* @date    xx
*/

/*******************************************************************************
*                          Include Files
*******************************************************************************/
#include <string.h>
#include <stdlib.h>

#include "ezlopi_hal_pwm.h"
#include "ezlopi_util_trace.h"
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
struct s_ezlopi_pwm_object
{
    ledc_timer_config_t *ledc_timer_configuration;
    ledc_channel_config_t *ledc_channel_configuration;
};

/*******************************************************************************
*                          Static Function Prototypes
*******************************************************************************/
static uint8_t get_available_channel();

/*******************************************************************************
*                          Static Data Definitions
*******************************************************************************/

#if CONFIG_IDF_TARGET_ESP32 
static bool available_channels[LEDC_CHANNEL_MAX] = { false, true, true, true, true, true, true };
#elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
static bool available_channels[LEDC_CHANNEL_MAX] = { true, true, true, true, true, true, true };
#elif CONFIG_IDF_TARGET_ESP32C2 || CONFIG_IDF_TARGET_ESP32C3
static bool available_channels[LEDC_CHANNEL_MAX] = { true, true, true, true, true };
#endif

/*******************************************************************************
*                          Extern Data Definitions
*******************************************************************************/

/*******************************************************************************
*                          Extern Function Definitions
*******************************************************************************/

s_ezlopi_channel_speed_t *EZPI_hal_pwm_init(uint8_t pwm_gpio_num, uint8_t pwm_resln, uint32_t freq_hz, uint32_t duty_cycle)
{

    s_ezlopi_channel_speed_t *ezlopi_channel_speed = (s_ezlopi_channel_speed_t *)ezlopi_malloc(__FUNCTION__, sizeof(s_ezlopi_channel_speed_t));
    memset(ezlopi_channel_speed, 0, sizeof(s_ezlopi_channel_speed_t));

    uint8_t channel = get_available_channel();
    if (LEDC_CHANNEL_MAX == channel)
    {
        TRACE_E("No channels availalbe for PWM.");
    }
    else
    {
        ledc_timer_config_t ezlopi_pwm_timer_cfg = {
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .duty_resolution = pwm_resln,
            .timer_num = LEDC_TIMER_3,
            .freq_hz = freq_hz,
            .clk_cfg = LEDC_AUTO_CLK,
        };
        ledc_channel_config_t ezlopi_pwm_channel_cfg = {
            .gpio_num = pwm_gpio_num,
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .channel = channel,
            .intr_type = LEDC_INTR_DISABLE,
            .timer_sel = LEDC_TIMER_3,
            .duty = duty_cycle,
        };

        esp_err_t error = ledc_timer_config(&ezlopi_pwm_timer_cfg);
        if (error != ESP_OK)
        {
            TRACE_E("Error configuring LEDC timer.(code:%s)", esp_err_to_name(error));
        }
        else
        {
            TRACE_S("LEDC timer configured successfully.");
        }

        error = ledc_channel_config(&ezlopi_pwm_channel_cfg);
        if (error != ESP_OK)
        {
            TRACE_E("Error configuring LEDC channel.(code:%s)", esp_err_to_name(error));
        }
        else
        {
            TRACE_S("LEDC channel configured successfully.");
        }

        ezlopi_channel_speed->channel = channel;
        ezlopi_channel_speed->speed_mode = ezlopi_pwm_channel_cfg.speed_mode;
    }
    return ezlopi_channel_speed;
}

void EZPI_hal_pwm_change_duty(uint32_t channel, uint32_t speed_mode, uint32_t duty)
{
    esp_err_t error = ledc_set_duty(speed_mode, channel, duty);
    error = ledc_update_duty(speed_mode, channel);
    if (error != ESP_OK)
    {
        TRACE_E("Error setting LEDC duty.(code:%s)", esp_err_to_name(error));
    }
    else
    {
        TRACE_S("LEDC duty set successfully.");
    }
}

uint32_t EZPI_hal_pwm_get_duty(uint32_t channel, uint32_t speed_mode)
{
    return ledc_get_duty(speed_mode, channel);
}

/*******************************************************************************
*                         Static Function Definitions
*******************************************************************************/
static uint8_t get_available_channel()
{
    uint8_t channel = 0;
    while (channel != LEDC_CHANNEL_MAX)
    {
        if (available_channels[channel])
        {
            available_channels[channel] = false;
            break;
        }
        channel++;
    }
    return channel;
}

/*******************************************************************************
*                          End of File
*******************************************************************************/