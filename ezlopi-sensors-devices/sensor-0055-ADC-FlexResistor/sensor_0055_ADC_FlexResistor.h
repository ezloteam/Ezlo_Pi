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
 * @file    sensor_0055_ADC_FlexResistor.h
 * @brief   perform some function on sensor_0055
 * @author  
 * @version 0.1
 * @date    xx
 */

#ifndef _SENSOR_0055_ADC_FLEXRESISTOR_H_
#define _SENSOR_0055_ADC_FLEXRESISTOR_H_

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
    /**
     *  NOTE : FlexResistor module gives (0V - 5V) as analog output .
     *  But ESP32- only allows upto 2.4V max input.
     *
     *  Remedy:  introduce 10kOhm [Rout] to achieve voltage divider of ratio [1:2 , i.e. 50%]  on the Sensor analog output ,
     *           so that esp32 adc pin recieves half voltage only.
     *             (Half of 5) -> 2.4V
     *
     *
     *              [flex resistor]
     *              [10kOhm - 110kOhm]
     *   +Vin[5V] ---{Rs?}------+
     *                          |
     *                          |
     *                          |
     * (Multimeter read) _______+--> 2.5V------+
     *                   ^      |              ^
     *                   |      |  [constant]  |
     *                   |      |              |
     *                Rout = {~10kOhm}        esp32 analog input [Vo]
     *                   |      |              |
     *                   |      |              |
     *                   V      |              v
     *      [0V] ---------------+--> 0V--------+------------+ 0V
     *
     *
     *
     *  // Here to find the 'Rs' value we can use voltage divider rule
     *          Vo = (Rout / Rout + Rs) * Vin ....................(1)
     *      or, Vo = (10K / 10K + Rs) * Vin
     *      or, Rs = (10K/Vo)*Vin - 10K
     *      or, Rs = [(Vin/Vo) - 1] * 10K ......................(2)
     *
     *      Now putting Vin = 5V, in eqn(2),
     *          Rs = [(5V / Vo) - 1] * 10K  ....................(3)
     *
     *      So using equation(3) and 'Vo' voltage value we get the required resistance value 'Rs'
     **/

    // measure the resistor value using multimeter
#define flex_Rout 10000.0f // minimum Rout = 10KOhm
#define flex_Vin 5.0f      // Vin = 5V  (default) // if [3.3V] is used instead of [5.0V], Change [flex_Vin=> 5.0f to 3.3f]

    typedef struct flex_t
    {
        int rs_0055;
    } flex_t;

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/

    /**
     * @brief Function to operate on actions
     *
     * @param action Current Action to Operate on
     * @param item Target-Item node
     * @param arg Arg for action
     * @param user_arg User-arg
     * @return ezlopi_error_t
     */
    ezlopi_error_t SENSOR_0055_adc_flexresistor(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#ifdef __cplusplus
}
#endif

#endif //_SENSOR_0055_ADC_FLEXRESISTOR_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
