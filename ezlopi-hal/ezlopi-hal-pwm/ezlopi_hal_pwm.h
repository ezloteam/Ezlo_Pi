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
 * @file    ezlopi_hal_pwm.h
 * @brief   perform some function on pwm
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 *          Lomas Subedi
 *          Riken Maharjan
 *          Nabin Dangi
 * @version 0.1
 * @date    xx
 */

#ifndef _EZLOPI_PWM_H_
#define _EZLOPI_PWM_H_

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

#include "driver/ledc.h"
#include "ezlopi_core_errors.h"
/*******************************************************************************
 *                          C++ Declaration Wrapper
 *******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

    /*******************************************************************************
     *                          Type & Macro Declarations
     *******************************************************************************/
    typedef struct s_ezlopi_channel_speed
    {
        uint32_t channel;
        uint32_t speed_mode;
    } s_ezlopi_channel_speed_t;

    typedef struct ezlopi_pwm
    {
        int gpio_num;
        uint32_t channel;
        uint32_t speed_mode;
        uint8_t pwm_resln; // pwm resolution
        uint32_t freq_hz;
        uint32_t duty_cycle;
        int value;
    } s_ezlopi_pwm_t;

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/

    /**
     * @brief Function to initialize pwm
     *
     * @param pwm_gpio_num Target GPIO to set as pwm
     * @param pwm_resln PWM resolution
     * @param freq_hz Frequecy
     * @param duty_cycle duty cycle
     * @return s_ezlopi_channel_speed_t*
     */
    s_ezlopi_channel_speed_t *EZPI_hal_pwm_init(uint8_t pwm_gpio_num, uint8_t pwm_resln, uint32_t freq_hz, uint32_t duty_cycle);
    /**
     * @brief Function to set the pwm duty cycle
     *
     * @param channel Target- gpio Channel
     * @param speed channel group with specified speed mode
     * @param duty duty cycle
     */
    void EZPI_hal_pwm_change_duty(uint32_t channel, uint32_t speed_mode, uint32_t duty);
    /**
     * @brief Function to get the pwm duty cycle
     *
     * @param channel Target- gpio Channel
     * @param speed_mode channel group with specified speed mode
     * @return uint32_t
     */
    uint32_t EZPI_hal_pwm_get_duty(uint32_t channel, uint32_t speed_mode);

#ifdef __cplusplus
}
#endif

#endif //_EZLOPI_PWM_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/