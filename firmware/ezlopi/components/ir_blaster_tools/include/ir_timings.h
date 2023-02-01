// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Timings for NEC protocol
 *
 */
#define NEC_LEADING_CODE_HIGH_US (9000)
#define NEC_LEADING_CODE_LOW_US (4500)
#define NEC_PAYLOAD_ONE_HIGH_US (560)
#define NEC_PAYLOAD_ONE_LOW_US (1690)
#define NEC_PAYLOAD_ZERO_HIGH_US (560)
#define NEC_PAYLOAD_ZERO_LOW_US (560)
#define NEC_REPEAT_CODE_HIGH_US (9000)
#define NEC_REPEAT_CODE_LOW_US (2250)
#define NEC_ENDING_CODE_HIGH_US (560)

/**
 * @brief Timings for RC5 protocol
 *
 */
#define RC5_PULSE_DURATION_US (889)

/**
 * @brief Timings for SAMSUNG protocol
 *
 */
#define SAMSUNG_LEADING_CODE_HIGH_US (4500)
#define SAMSUNG_LEADING_CODE_LOW_US (4500)
#define SAMSUNG_PAYLOAD_ONE_HIGH_US (560)
#define SAMSUNG_PAYLOAD_ONE_LOW_US (1690)
#define SAMSUNG_PAYLOAD_ZERO_HIGH_US (560)  //590
#define SAMSUNG_PAYLOAD_ZERO_LOW_US (560)
#define SAMSUNG_REPEAT_CODE_HIGH_US (4500)
#define SAMSUNG_REPEAT_CODE_LOW_US (2250)
#define SAMSUNG_ENDING_CODE_HIGH_US (560)

/**
 * @brief Timings for PANASONIC protocol
 *
 */
#define PANASONIC_LEADING_CODE_HIGH_US (3502)
#define PANASONIC_LEADING_CODE_LOW_US (1750)
#define PANASONIC_PAYLOAD_ONE_HIGH_US (502)
#define PANASONIC_PAYLOAD_ONE_LOW_US (1244)
#define PANASONIC_PAYLOAD_ZERO_HIGH_US (502)
#define PANASONIC_PAYLOAD_ZERO_LOW_US (400)
#define PANASONIC_REPEAT_CODE_HIGH_US (9000)
#define PANASONIC_REPEAT_CODE_LOW_US (2250)
#define PANASONIC_ENDING_CODE_HIGH_US (502)

/**
 * @brief Timings for LG protocol
 *
 */
#define LG_LEADING_CODE_HIGH_US (9000)
#define LG_LEADING_CODE_LOW_US (4500)
#define LG_PAYLOAD_ONE_HIGH_US (560)
#define LG_PAYLOAD_ONE_LOW_US (1690)
#define LG_PAYLOAD_ZERO_HIGH_US (560)
#define LG_PAYLOAD_ZERO_LOW_US (560)
#define LG_REPEAT_CODE_HIGH_US (9000)
#define LG_REPEAT_CODE_LOW_US (2250)
#define LG_ENDING_CODE_HIGH_US (560)

/**
 * @brief Timings for SONY (SIRCS) protocol
 *
 */
#define SONY_LEADING_CODE_HIGH_US (2400)
#define SONY_LEADING_CODE_LOW_US (600)
#define SONY_PAYLOAD_ONE_HIGH_US (1200)
#define SONY_PAYLOAD_ONE_LOW_US (600)
#define SONY_PAYLOAD_ZERO_HIGH_US (600)
#define SONY_PAYLOAD_ZERO_LOW_US (600)
#define SONY_REPEAT_CODE_HIGH_US (2400)
#define SONY_REPEAT_CODE_LOW_US (600)

/**
 * @brief Timings for GREE protocol
 *
 */
#define GREE_LEADING_CODE_HIGH_US (9000)
#define GREE_LEADING_CODE_LOW_US (4500)
#define GREE_PAYLOAD_ONE_HIGH_US (620)
#define GREE_PAYLOAD_ONE_LOW_US (1600)
#define GREE_PAYLOAD_ZERO_HIGH_US (620)
#define GREE_PAYLOAD_ZERO_LOW_US (540)
#define GREE_MESSAGE_SPACE_HIGH_US (620)
#define GREE_MESSAGE_SPACE_LOW_US (19980)
//#define GREE_REPEAT_CODE_HIGH_US (2400)
//#define GREE_REPEAT_CODE_LOW_US (600)
#define GREE_ENDING_CODE_HIGH_US (620)

/**
 * @brief Timings for DISH protocol
 *
 */
#define DISH_LEADING_CODE_HIGH_US (400)
#define DISH_LEADING_CODE_LOW_US (6100)
#define DISH_PAYLOAD_ONE_HIGH_US (400)
#define DISH_PAYLOAD_ONE_LOW_US (1700)
#define DISH_PAYLOAD_ZERO_HIGH_US (400)
#define DISH_PAYLOAD_ZERO_LOW_US (2800)
#define DISH_REPEAT_CODE_HIGH_US (400)
#define DISH_REPEAT_CODE_LOW_US (6200)
#define DISH_ENDING_CODE_HIGH_US (400)

/**
 * @brief Timings for LEGO protocol
 *
 */
#define LEGO_LEADING_CODE_HIGH_US (158)
#define LEGO_LEADING_CODE_LOW_US (1026)
#define LEGO_PAYLOAD_ONE_HIGH_US (158)
#define LEGO_PAYLOAD_ONE_LOW_US (553)
#define LEGO_PAYLOAD_ZERO_HIGH_US (158)
#define LEGO_PAYLOAD_ZERO_LOW_US (263)
#define LEGO_ENDING_CODE_HIGH_US (158)

/**
 * @brief Timings for TOSHIBAAC protocol
 *
 */
#define TOSHIBAAC_LEADING_CODE_HIGH_US (4400)
#define TOSHIBAAC_LEADING_CODE_LOW_US (4400)
#define TOSHIBAAC_PAYLOAD_ONE_HIGH_US (600)
#define TOSHIBAAC_PAYLOAD_ONE_LOW_US (1600)
#define TOSHIBAAC_PAYLOAD_ZERO_HIGH_US (600)
#define TOSHIBAAC_PAYLOAD_ZERO_LOW_US (400)
#define TOSHIBAAC_MESSAGE_SPACE_HIGH_US (600)
#define TOSHIBAAC_MESSAGE_SPACE_LOW_US (8000)
#define TOSHIBAAC_ENDING_CODE_HIGH_US (600)

/**
 * @brief Timings for TOSHIBAAC protocol (50 bit) send 2 times (100-bit)
 *
 */
#define TOSHIBAAC50_LEADING_CODE_HIGH_US (4300)
#define TOSHIBAAC50_LEADING_CODE_LOW_US (4500)
#define TOSHIBAAC50_PAYLOAD_ONE_HIGH_US (400) 
#define TOSHIBAAC50_PAYLOAD_ONE_LOW_US (1700)
#define TOSHIBAAC50_PAYLOAD_ZERO_HIGH_US (400)
#define TOSHIBAAC50_PAYLOAD_ZERO_LOW_US (700)
#define TOSHIBAAC50_MESSAGE_SPACE_HIGH_US (400)
#define TOSHIBAAC50_MESSAGE_SPACE_LOW_US (5650)
#define TOSHIBAAC50_NEW_MESSAGE_SPACE_LOW_US (5300)
#define TOSHIBAAC50_ENDING_CODE_HIGH_US (400)

/**
 * @brief Timings for JVC protocol
 *
 */
#define JVC_LEADING_CODE_HIGH_US (8000)
#define JVC_LEADING_CODE_LOW_US (4000)
#define JVC_PAYLOAD_ONE_HIGH_US (600)
#define JVC_PAYLOAD_ONE_LOW_US (1600)
#define JVC_PAYLOAD_ZERO_HIGH_US (600)
#define JVC_PAYLOAD_ZERO_LOW_US (550)
#define JVC_ENDING_CODE_HIGH_US (600)

/**
 * @brief Timings for AIRTON protocol
 *
 */
#define AIRTON_LEADING_CODE_HIGH_US (6630)
#define AIRTON_LEADING_CODE_LOW_US (3350)
#define AIRTON_PAYLOAD_ONE_HIGH_US (400)
#define AIRTON_PAYLOAD_ONE_LOW_US (1260)
#define AIRTON_PAYLOAD_ZERO_HIGH_US (400)
#define AIRTON_PAYLOAD_ZERO_LOW_US (430)
#define AIRTON_ENDING_CODE_HIGH_US (400)

#ifdef __cplusplus
}
#endif
