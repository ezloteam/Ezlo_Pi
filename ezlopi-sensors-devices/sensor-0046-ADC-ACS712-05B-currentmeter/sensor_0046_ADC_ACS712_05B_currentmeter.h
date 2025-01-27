/* ===========================================================================
** Copyright (C) 2025 Ezlo Innovation Inc
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
 * @file    sensor_0046_ADC_ACS712_05B_currentmeter.h
 * @brief   perform some function on sensor_0046
 * @author  
 * @version 0.1
 * @date    xx
 */

#ifndef _SENSOR_0046_ADC_ACS712_05B_CURRENTMETER_H_
#define _SENSOR_0046_ADC_ACS712_05B_CURRENTMETER_H_

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "ezlopi_core_actions.h"
#include "ezlopi_core_devices.h"
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
#define ASC712TELC_05B_zero_point_mV 2350 // adc_value at the output ; when no current in input side
#define DEFAULT_AC_FREQUENCY 50

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/
    /**
     * Chip model : ACS712ELCTR-05B-T
     *
     * Optimized Measurement Range (Ip) = +-5A
     *
     * Sensitivity, Sens(Typ) (mV/A) = 185mv/A
     *
     */

    /**
     *  NOTE : ACS712ELCTR-05B-T - module gives (0V - 5V) as analog output .
     *  But ESP32- only allows upto 2.4V max input.
     *
     *  Remedy:  introduce a voltage divider of ratio [1:2 , i.e. 50%]  on the Sensor analog output , so that esp32 adc pin recieves half voltage only.
     *             ( Half of 4.2) -> 2.1V
     *
     *  currentMeter > 5V---+
     *                      |
     *                     {1KOhm}
     *                      |
     *                      +------------+ 2.4V
     *                      |                ^
     *                     {1KOhm}           |  esp32 analog input
     *                      |                v
     *            > 0V------+------------+ 0V
     *
     *
     */

    /**
     * @brief Function to operate on actions
     *
     * @param action Current Action to Operate on
     * @param item Target-Item node
     * @param arg Arg for action
     * @param user_arg User-arg
     * @return ezlopi_error_t
     */
    ezlopi_error_t SENSOR_0046_adc_acs712_05b_currentmeter(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#ifdef __cplusplus
}
#endif

#endif // _SENSOR_0046_ADC_ACS712_05B_CURRENTMETER_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/