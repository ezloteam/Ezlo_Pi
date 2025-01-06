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
 * @file    ezlopi_hal_gpio.h
 * @brief   perform some function on GPIO
 * @author  xx
 * @version 0.1
 * @date    xx
 */

#ifndef _EZLOPI_HAL_GPIO_H_
#define _EZLOPI_HAL_GPIO_H_

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

#include <stdio.h>
#include "driver/gpio.h"

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

    typedef gpio_num_t e_ezlopi_gpio_num_t;
    typedef gpio_mode_t e_ezlopi_gpio_mode_t;
    typedef gpio_pull_mode_t e_ezlopi_gpio_pull_mode_t;
    typedef gpio_int_type_t e_ezlopi_gpio_interrupt_t;

    typedef enum e_ezlopi_gpio_value
    {
        EZLOPI_GPIO_LOW = 0,
        EZLOPI_GPIO_HIGH,
        EZLOPI_GPIO_VALUE_MAX
    } e_ezlopi_gpio_value_t;

    typedef enum e_ezlopi_gpio_logic_invert
    {
        EZLOPI_GPIO_LOGIC_NONINVERTED = 0,
        EZLOPI_GPIO_LOGIC_INVERTED,
        EZLOPI_GPIO_LOGIC_MAX
    } e_ezlopi_gpio_logic_invert_t;

    typedef struct s_ezlopi_gpio
    {
        bool enable;
        e_ezlopi_gpio_num_t gpio_num;
        e_ezlopi_gpio_mode_t mode;
        e_ezlopi_gpio_value_t value;
        e_ezlopi_gpio_pull_mode_t pull;
        e_ezlopi_gpio_logic_invert_t invert;
        e_ezlopi_gpio_interrupt_t interrupt;
    } s_ezlopi_gpio_t;

    typedef struct s_ezlopi_gpios
    {
        s_ezlopi_gpio_t gpio_in;
        s_ezlopi_gpio_t gpio_out;
    } s_ezlopi_gpios_t;

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/

    // int ezlopi_gpio_init(s_ezlopi_device_properties_t *properties);

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_HAL_GPIO_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
