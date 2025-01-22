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
 * @file    ezlopi_hal_gpio.c
 * @brief   perform some function on GPIO
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 *          Lomas Subedi
 *          Riken Maharjan
 *          Nabin Dangi
 * @version 0.1
 * @date    xx
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
// #include "ezlopi_hal_gpio.h"
// #include "ezlopi_core_devices.h"
/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/

// int ezlopi_gpio_init(s_ezlopi_device_properties_t *properties)
// {
//     int ret = 0;

//     if (GPIO_IS_VALID_OUTPUT_GPIO(properties->interface.gpio.gpio_out.gpio_num))
//     {
//         const gpio_config_t io_conf = {
//             .pin_bit_mask = (1ULL << properties->interface.gpio.gpio_out.gpio_num),
//             .mode = GPIO_MODE_OUTPUT,
//             .pull_up_en = ((properties->interface.gpio.gpio_out.pull == GPIO_PULLUP_ONLY) ||
//                            (properties->interface.gpio.gpio_out.pull == GPIO_PULLUP_PULLDOWN))
//                               ? GPIO_PULLUP_ENABLE
//                               : GPIO_PULLUP_DISABLE,
//             .pull_down_en = ((properties->interface.gpio.gpio_out.pull == GPIO_PULLDOWN_ONLY) ||
//                              (properties->interface.gpio.gpio_out.pull == GPIO_PULLUP_PULLDOWN))
//                                 ? GPIO_PULLDOWN_ENABLE
//                                 : GPIO_PULLDOWN_DISABLE,
//             .intr_type = GPIO_INTR_DISABLE,
//         };

//         gpio_config(&io_conf);
//         // digital_io_write_gpio_value(properties);
//     }

//     if (GPIO_IS_VALID_GPIO(properties->interface.gpio.gpio_in.gpio_num))
//     {
//         const gpio_config_t io_conf = {
//             .pin_bit_mask = (1ULL << properties->interface.gpio.gpio_in.gpio_num),
//             .mode = GPIO_MODE_OUTPUT,
//             .pull_up_en = ((properties->interface.gpio.gpio_in.pull == GPIO_PULLUP_ONLY) ||
//                            (properties->interface.gpio.gpio_in.pull == GPIO_PULLUP_PULLDOWN))
//                               ? GPIO_PULLUP_ENABLE
//                               : GPIO_PULLUP_DISABLE,
//             .pull_down_en = ((properties->interface.gpio.gpio_in.pull == GPIO_PULLDOWN_ONLY) ||
//                              (properties->interface.gpio.gpio_in.pull == GPIO_PULLUP_PULLDOWN))
//                                 ? GPIO_PULLDOWN_ENABLE
//                                 : GPIO_PULLDOWN_DISABLE,
//             .intr_type = (GPIO_PULLUP_ONLY == properties->interface.gpio.gpio_in.pull)
//                              ? GPIO_INTR_POSEDGE
//                              : GPIO_INTR_NEGEDGE,
//         };

//         gpio_config(&io_conf);
//         // digital_io_isr_service_init(properties);
//     }

//     return ret;
// }

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
