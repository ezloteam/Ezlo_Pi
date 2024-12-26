#ifndef _SENSOR_0051_OTHER_MQ8_H2_DETECTOR_H_
#define _SENSOR_0051_OTHER_MQ8_H2_DETECTOR_H_

#include "ezlopi_core_actions.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_errors.h"

/**
 *  NOTE : MQ8_hydrogen_gas - module gives (0V - 4.2V) as analog output .
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
 *     => Reqv ----> RL    {1kOhm}         +------------+ 2.1V
 *     => ~660 Ohm   |      |              |                ^
 *                   |      |             {1KOhm}           |  esp32 analog input
 *                   V      |              |                v
 *      [0V] ---------------+--> 0V--------+------------+ 0V
 *
 *
 **/

 /**
  *  From the graph, we can see that the resistance ratio in fresh air is a constant:
  *      (via black straight line of sensitivity graph in the Mq-8 datasheet)
  *
  *  i.e. ---------------------------> [RS_calib / R0] = 70
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
   *    Case. 1: Calculation of slope 'm' Using MQ8's hydrogen-curve (H2) in semi-log graph:
   *
   *                                      (x0 , y0) and (  x  , y  )
   *                                          |               |
   *                                          V               V
   *                 #A. chose the points (200,8.5) and (10000,0.03) from the 'H2' graph-line
   *                 ->           m = log(0.03/8.5) / log(10000/200)
   *                 ->           m = -1.443
   *
   *                 #B. We chose (1000,1)
   *                 ->           b = log(1) - (-1.443)*log(1000)
   *                 ->           b = 4.329
   *
   *    Case. 2: Calculation of _ppm. First change, {Y to [ratio = RS/R0]} in y-axis  and {X to [_PPM]} in x-axis
   *                  #C. From eq(2),
   *                  ->          x = 10 ^ { [log(y) - b] / m }
   *                  ->       _ppm = 10 ^ [ (log([ratio]) - 0.351) / -0.117 ] ----------------------------(3)
   *
   *
   *    _________ STAGE 1 : CALIBRATION ____________________________________________________
   *
   *    Case. 3: Here 'ratio' = [Rs/Ro] is still left to be calculated so,
   *                  #D. Using Rs formula :  Rs_calib = [(VC x RL) / VRL] - RL ; where Vc = 5V,
   *                                                                                  RL = 66O ohm ,
   *                                                                                  VRL = sensor_analog_output
   *                  NOTE : use multimeter and measure the Equivalent resistance (RL)
   *                  ->     Rs_calib = [(5 * 660) / VRL] - 660 ;
   *
   *                  #E. Now [Ro constant value] , we get :
   *                        ->   Ro = ratio/Rs_calib
   *                        ->   Ro = 70 / Rs_calib  --------------- (4)
   *
   *
   *    _________ STAGE 2 : PPM Calculation ____________________________________________________
   *
   *    Case. 4: Calculation of _ppm using eq(3), we get:
   *                  #F. Formula of _ppm is:-
   *                        -> _ppm = 10 ^ [ (log([Rs_gas / Ro]) - 1.019) / -0.3397 ]  ;
   *                                                  -> where ; Ro is taken from eq(4)
   *                                                           ; Rs_gas = [(5 * 660) / VRL] - 660 ;
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
     * Before writing below  [mq8_eqv_RL], [m_slope_mq8], [b_coeff_mq8] shown values.
     * Please apply voltage-divider schematics as shown above.
     *
     * NOTE .1 : Mannually use multimeter and measure : eqv-resistance [mq8_eqv_RL], between [A0_pin vs GND] of 'MQ-8 sensor'
     *
     * NOTE .2 : For [m_slope_mq8] & [b_coeff_mq8] values:- follow [stage-1] above shown procedures.
     * */
#define mq8_eqv_RL 660.0f      // review the upper diagram clearly to know why this value is assigned
#define RatioMQ8CleanAir 70.0f // -> [RS / R0] = 70
#define MQ8_VOLT_RESOLUTION_Vc 5.0f
#define m_slope_mq8 -1.443f
#define b_coeff_mq8 4.329f
#define MQ8_AVG_CAL_COUNT 4
typedef enum
{
    MQ8_GAS_ALARM_NO_GAS,
    MQ8_GAS_ALARM_COMBUSTIBLE_GAS_DETECTED,
    MQ8_GAS_ALARM_TOXIC_GAS_DETECTED,
    MQ8_GAS_ALARM_UNKNOWN,
    MQ8_GAS_ALARM_MAX
} e_mq8_gas_alarm_states_t;

//--------------------------------------------------------------------------------------------------------
//      ACTION FUNCTION
//--------------------------------------------------------------------------------------------------------

ezlopi_error_t sensor_0051_other_MQ8_H2_detector(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

//--------------------------------------------------------------------------------------------------------

#endif //_SENSOR_0051_OTHER_MQ8_H2_DETECTOR_H_