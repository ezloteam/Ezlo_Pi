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
 * @file    sensor_0059_other_MQ6_LPG_detector.h
 * @brief   perform some function on sensor_0059
 * @author  xx
 * @version 0.1
 * @date    xx
*/

#ifndef _SENSOR_0059_OTHER_MQ6_LPG_DETECTOR_H_
#define _SENSOR_0059_OTHER_MQ6_LPG_DETECTOR_H_

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
     *  NOTE : MQ6_LPG_gas - module gives (0V - 4.2V) as analog output .
     *  But ESP32- only allows upto 2.4V max input.
     *
     *  Remedy:  introduce a voltage divider of ratio [1:2 , i.e. 50%]  on the Sensor analog output , so that esp32 adc pin recieves half voltage only.
     *             (Half of 4.2) -> 2.1V
     *
     *
     *              [variable]
     *  > +Vc [5V] ---{Rs}------+
     *                          |
     *                          |
     *                          |
     * (Multimeter read) _______+--> 4.2V------+
     *    (Ao pin vs)    ^      |              |
     *     (gnd_pin)     |      |             {1KOhm}
     *                   |      | [constant]   |
     *     => Reqv ----> RL  {1000 Ohm}         +------------+ 2.1V
     *     => 500 Ohm    |      |              |                ^
     *                   |      |             {1KOhm}           |  esp32 analog input
     *                   V      |              |                v
     *      [0V] ---------------+--> 0V--------+------------+ 0V
     *
     *
     **/

     /**
      *  From the graph, we can see that the resistance ratio in fresh air is a constant:
      *      (via black straight line of sensitivity graph in the Mq-4 datasheet)
      *
      *  i.e. ---------------------------> [RS_calib / R0] =  10
      *
      * To calculate R0 we will need to find the value of the RS in fresh air.
      *  This will be done by taking the analog average readings from the sensor and converting it to voltage.
      *  Then we will use the 'RS' formula to find 'R0'.
      *
      *  i.e. ----------------------------> RS_calib = [(VC x RL) / VRL] - RL;
      */

      /**
       *   Calculation Process
       *   ~~~~~~~~~~~~~~~~~~~
       *                          : $. Linear      : y = mx + b
       *                          : $. Exponential : log(y) = m*log(x) + b
       *
       *   # On Solving, we get,
       *         => log(y) = m*log(x) + b ------------------------ (1)
       *         => log(x) = [log(y) - b] / m
       *         =>      x = 10 ^ { [log(y) - b] / m }------------ (2)
       *         where,
       *                -> m = [log(y) - log(y0)] / [log(x) - log(x0)]  => [ m = log(y/y0) / log(x/x0) ] ;
       *                -> b = [log(y) - m*log(x)] = ?
       *
       *    Case. 1: Calculation of slope 'm' Using MQ6's LPG-curve (CH4) in semi-log graph:
       *
       *                                      (x0 , y0) and (  x  , y  )
       *                                          |               |
       *                                          V               V
       *                 #A. chose the points (200,2) and (10000,0.38) from the 'CH4' graph-line
       *                 ->           m = log(0.38/2) / log(10000/200)
       *                 ->           m = -0.424
       *
       *                 #B. We chose (1000,1)
       *                 ->           b = log(1) - ( -0.424 )*log(1000)
       *                 ->           b = 1.272
       *
       *    Case. 2: Calculation of _ppm. First change, {Y to [ratio = RS/R0]} in y-axis  and {X to [_PPM]} in x-axis
       *                  #C. From eq(2),
       *                  ->          x = 10 ^ { [log(y) - b] / m }
       *                  ->       _ppm = 10 ^ [ (log([ratio]) - 1.272 ) / -0.424   ] ----------------------------(3)
       *
       *
       *    _________ STAGE 1 : CALIBRATION ____________________________________________________
       *
       *    Case. 3: Here 'ratio' = [Rs/Ro] is still left to be calculated so,
       *                  #D. Using Rs formula :  Rs_calib = [(VC x RL) / VRL] - RL ; where Vc = 5V,
       *                                                                                  RL = 500 ohm ,
       *                                                                                  VRL = sensor_analog_output
       *                  NOTE : use multimeter and measure the Equivalent resistance (RL)
       *                  ->     Rs_calib = [(5 * 500) / VRL] - 500 ;
       *
       *                  #E. Now [Ro constant value] , we get :
       *                        ->   Ro = ratio/Rs_calib
       *                        ->   Ro =     / Rs_calib  --------------- (4)
       *
       *
       *    _________ STAGE 2 : PPM Calculation ____________________________________________________
       *
       *    Case. 4: Calculation of _ppm using eq(3), we get:
       *                  #F. Formula of _ppm is:-
       *                        -> _ppm = 10 ^ [ (log([Rs_gas / Ro]) - 1.272 ) / -0.424 ]  ;
       *                                                  -> where ; Ro is taken from eq(4)
       *                                                           ; Rs_gas = [(5 * 500) / VRL] - 500 ;
       *    ________________________________________________________________________________________
       *
       */

       //------------------------------------------
       /**
        * Please don't forget to uncomment ,
        * -> If you added a voltage divider at sensor's analog output. [Make sure voltage does not exceed 2.5V]
        *      |
        *      |
        *      V
        */
#define VOLTAGE_DIVIDER_ADDED 1
        //------------------------------------------

        /**
         * Before writing below  [mq6_eqv_RL], [m_slope_mq6], [b_coeff_mq6] shown values.
         * Please apply voltage-divider schematics as shown above.
         *
         * NOTE .1 : Mannually use multimeter and measure : eqv-resistance [mq6_eqv_RL], between [A0_pin vs GND] of 'MQ-4 sensor'
         *
         * NOTE .2 : For [m_slope_mq6] & [b_coeff_mq6] values:- follow [stage-1] above shown procedures.
         * */
#define mq6_eqv_RL 500.0f      // review the upper diagram clearly to know why this value is assigned
#define RatioMQ6CleanAir 10.0f // -> [RS / R0] = 10.0f
#define MQ6_VOLT_RESOLUTION_Vc 5.0f
#define m_slope_mq6 -0.424f
#define b_coeff_mq6 1.272f
#define MQ6_AVG_CAL_COUNT 4

    typedef enum
    {
        MQ6_GAS_ALARM_NO_GAS,
        MQ6_GAS_ALARM_COMBUSTIBLE_GAS_DETECTED,
        MQ6_GAS_ALARM_TOXIC_GAS_DETECTED,
        MQ6_GAS_ALARM_UNKNOWN,
        MQ6_GAS_ALARM_MAX
    } e_mq6_gas_alarm_states_t;


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
    ezlopi_error_t SENSOR_0059_other_mq6_lpg_detector(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#ifdef __cplusplus
}
#endif

#endif //_SENSOR_0059_OTHER_MQ6_LPG_DETECTOR_H_

/*******************************************************************************
*                          End of File
*******************************************************************************/