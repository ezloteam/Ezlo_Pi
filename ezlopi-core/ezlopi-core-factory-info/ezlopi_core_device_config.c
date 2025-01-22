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
 * @file    ezlopi_core_device_config.c
 * @brief   Data defination for factory info and test datas
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 *          Lomas Subedi
 *          Riken Maharjan
 *          Nabin Dangi
 * @version 0.1
 * @date    12th DEC 2024
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "ezlopi_core_factory_info.h"

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
#if (EZLOPI_DEVICE_TYPE_GENERIC == EZLOPI_DEVICE_TYPE)

#elif (EZLOPI_DEVICE_TYPE_IR_BLASTER == EZLOPI_DEVICE_TYPE)
static const char *ir_blaster_constant_config =
    "{\
        \"cmd\": 3,\
        \"dev_detail\":\
        [\
            {\
                \"dev_name\": \"IR_Blaster1\",\
                \"dev_type\": 5,\
                \"gpio\": 3,\
                \"id_item\": 30,\
                \"id_room\": \"\",\
                \"pwm_resln\": 8,\
                \"freq_hz\": 50,\
                \"duty_cycle\": 30\
            }\
        ],\
    \"dev_total\": 1}";

#elif (EZLOPI_DEVICE_TYPE_SWITCH_BOX == EZLOPI_DEVICE_TYPE)
const char *switch_box_constant_config =
    "{\
        \"cmd\": 3,\
        \"dev_detail\":\
        [\
            {\
                \"dev_name\" : \"GXHTC3-Sensor\",\
                \"dev_type\" : 8,\
                \"gpio_scl\" : 45,\
                \"gpio_sda\" : 46,\
                \"id_item\" : 29,\
                \"id_room\" : \"\",\
                \"pullup_scl\" : true,\
                \"pullup_sda\" : true,\
                \"slave_addr\" : 0\
            },\
            {\
                \"dev_name\": \"Switch 1\",\
                \"dev_type\": 1,\
                \"gpio_in\": -1,\
                \"gpio_out\": 39,\
                \"id_item\": 1,\
                \"id_room\": \"\",\
                \"ip_inv\": true,\
                \"is_ip\": false,\
                \"op_inv\": false,\
                \"pullup_ip\": true,\
                \"pullup_op\": true,\
                \"val_ip\": true,\
                \"val_op\": false\
            },\
            {\
                \"dev_name\": \"Switch 2\",\
                \"dev_type\": 1,\
                \"gpio_in\": -1,\
                \"gpio_out\": 38,\
                \"id_item\": 1,\
                \"id_room\": \"\",\
                \"ip_inv\": true,\
                \"is_ip\": false,\
                \"op_inv\": false,\
                \"pullup_ip\": true,\
                \"pullup_op\": true,\
                \"val_ip\": true,\
                \"val_op\": false\
            },\
            {\
                \"dev_name\": \"Switch 3\",\
                \"dev_type\": 1,\
                \"gpio_in\": -1,\
                \"gpio_out\": 15,\
                \"id_item\": 1,\
                \"id_room\": \"\",\
                \"ip_inv\": true,\
                \"is_ip\": false,\
                \"op_inv\": false,\
                \"pullup_ip\": true,\
                \"pullup_op\": true,\
                \"val_ip\": true,\
                \"val_op\": false\
            },\
            {\
                \"dev_name\": \"Switch 4\",\
                \"dev_type\": 1,\
                \"gpio_in\": -1,\
                \"gpio_out\": 4,\
                \"id_item\": 1,\
                \"id_room\": \"\",\
                \"ip_inv\": true,\
                \"is_ip\": false,\
                \"op_inv\": false,\
                \"pullup_ip\": true,\
                \"pullup_op\": true,\
                \"val_ip\": true,\
                \"val_op\": false\
            },\
            {\
                \"dev_name\": \"Switch 5\",\
                \"dev_type\": 1,\
                \"gpio_in\": -1,\
                \"gpio_out\": 5,\
                \"id_item\": 1,\
                \"id_room\": \"\",\
                \"ip_inv\": true,\
                \"is_ip\": false,\
                \"op_inv\": false,\
                \"pullup_ip\": true,\
                \"pullup_op\": true,\
                \"val_ip\": true,\
                \"val_op\": false\
            },\
            {\
                \"dev_name\": \"Switch 6\",\
                \"dev_type\": 1,\
                \"gpio_in\": -1,\
                \"gpio_out\": 6,\
                \"id_item\": 1,\
                \"id_room\": \"\",\
                \"ip_inv\": true,\
                \"is_ip\": false,\
                \"op_inv\": false,\
                \"pullup_ip\": true,\
                \"pullup_op\": true,\
                \"val_ip\": true,\
                \"val_op\": false\
            },\
            {\
                \"dev_name\": \"Switch 7\",\
                \"dev_type\": 1,\
                \"gpio_in\": -1,\
                \"gpio_out\": 7,\
                \"id_item\": 1,\
                \"id_room\": \"\",\
                \"ip_inv\": true,\
                \"is_ip\": false,\
                \"op_inv\": false,\
                \"pullup_ip\": true,\
                \"pullup_op\": true,\
                \"val_ip\": true,\
                \"val_op\": false\
            },\
            {\
                \"dev_name\": \"Switch 8\",\
                \"dev_type\": 1,\
                \"gpio_in\": -1,\
                \"gpio_out\": 12,\
                \"id_item\": 1,\
                \"id_room\": \"\",\
                \"ip_inv\": true,\
                \"is_ip\": false,\
                \"op_inv\": false,\
                \"pullup_ip\": true,\
                \"pullup_op\": true,\
                \"val_ip\": true,\
                \"val_op\": false\
            },\
            {\
                \"dev_name\": \"Switch 9\",\
                \"dev_type\": 1,\
                \"gpio_in\": -1,\
                \"gpio_out\": 17,\
                \"id_item\": 1,\
                \"id_room\": \"\",\
                \"ip_inv\": true,\
                \"is_ip\": false,\
                \"op_inv\": false,\
                \"pullup_ip\": true,\
                \"pullup_op\": true,\
                \"val_ip\": true,\
                \"val_op\": false\
            },\
            {\
                \"dev_name\": \"Switch 10\",\
                \"dev_type\": 1,\
                \"gpio_in\": -1,\
                \"gpio_out\": 18,\
                \"id_item\": 1,\
                \"id_room\": \"\",\
                \"ip_inv\": true,\
                \"is_ip\": false,\
                \"op_inv\": false,\
                \"pullup_ip\": true,\
                \"pullup_op\": true,\
                \"val_ip\": true,\
                \"val_op\": false\
            },\
            {\
                \"dev_name\": \"Master Switch\",\
                \"dev_type\": 1,\
                \"gpio_in\": -1,\
                \"gpio_out\": 255,\
                \"id_item\": 1,\
                \"id_room\": \"\",\
                \"ip_inv\": true,\
                \"is_ip\": false,\
                \"op_inv\": false,\
                \"pullup_ip\": true,\
                \"pullup_op\": true,\
                \"val_ip\": true,\
                \"val_op\": false\
            }\
        ],\
    \"dev_total\": 1}";

#elif (EZLOPI_DEVICE_TYPE_TEST_DEVICE == EZLOPI_DEVICE_TYPE)
#define EZLOPI_DEVICE_TYPE_NAME "generic"
const char *test_device_constant_config =
    "{\
    \"cmd\":3,\
    \"dev_detail\":\
    [\
        {\
            \"dev_type\": 7,\
            \"dev_name\": \"DHT 22\",\
            \"id_room\": \"\",\
            \"id_item\": 16,\
            \"gpio\": 18\
        },\
        {\
            \"dev_type\" : 1,\
            \"dev_name\" : \"Dining Room Main Lamp\",\
            \"id_room\" : \"\",\
            \"id_item\" : 2,\
            \"val_ip\" : true,\
            \"val_op\" : false,\
            \"gpio_in\" : 21,\
            \"gpio_out\" : 2,\
            \"is_ip\" : false,\
            \"ip_inv\" : true,\
            \"pullup_ip\" : true,\
            \"pullup_op\" : true,\
            \"op_inv\" : false\
        },\
        {\
            \"dev_type\":5,\
            \"dev_name\":\"Dimmer\",\
            \"id_room\":\"\",\
            \"id_item\":22,\
            \"gpio\":42,\
            \"pwm_resln\":3,\
            \"freq_hz\":50,\
            \"duty_cycle\":0\
        },\
        {\
            \"dev_name\": \"Backlight Control\",\
            \"dev_type\": 5,\
            \"gpio\": 48,\
            \"id_item\": 9,\
            \"id_room\": \"\",\
            \"pwm_resln\": 3,\
            \"freq_hz\": 50,\
            \"duty_cycle\": 10\
        },\
        {\
            \"dev_type\":10,\
            \"dev_name\":\"RGB LED\",\
            \"id_room\":\"\",\
            \"id_item\":38,\
            \"en_gpio1\":true,\
            \"gpio1\":11,\
            \"en_gpio2\":true,\
            \"gpio2\":10,\
            \"en_gpio3\":true,\
            \"gpio3\":13\
        },\
        {\
            \"dev_type\": 6,\
            \"dev_name\": \"UART_MB1013\",\
            \"id_item\": 21,\
            \"id_room\": \"\",\
            \"gpio_tx\": 3,\
            \"gpio_rx\": 8,\
            \"baud_rate\": 9600\
        }\
    ],\
    \"dev_total\":5\
    }";

#endif
/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
