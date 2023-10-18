#ifndef _SENSOR_0052_OTHER_MQ135_NH3_DETECTOR_H_
#define _SENSOR_0052_OTHER_MQ135_NH3_DETECTOR_H_

#include "ezlopi_actions.h"
#include "ezlopi_devices.h"
/**
 *  NOTE : MQ135_NH3_gas - module gives (0V - 4.2V) as analog output .
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
 *      (via black straight line of sensitivity graph in the Mq-135 datasheet)
 *
 *  i.e. ---------------------------> [RS_calib / R0] =  3.6f
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
 *    Case. 1: Calculation of slope 'm' Using MQ135's NH3-curve (NH3) in semi-log graph:
 *
 *                                      (x0 , y0) and (  x  , y  )
 *                                          |               |
 *                                          V               V
 *                 #A. chose the points (10,2.6) and (200,0.78) from the 'NH3' graph-line
 *                 ->           m = log(0.78/2.6) / log(200/10)
 *                 ->           m = -0.401
 *
 *                 #B. We chose (100,1)
 *                 ->           b = log(1) - ( -0.401 )*log(100)
 *                 ->           b = 0.802
 *
 *    Case. 2: Calculation of _ppm. First change, {Y to [ratio = RS/R0]} in y-axis  and {X to [_PPM]} in x-axis
 *                  #C. From eq(2),
 *                  ->          x = 10 ^ { [log(y) - b] / m }
 *                  ->       _ppm = 10 ^ [ (log([ratio]) -   0.802 ) / -0.401   ] ----------------------------(3)
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
 *                        ->   Ro =     / Rs_calib  --------------- (4)
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
 * Before writing below  [mq135_eqv_RL], [m_slope_mq135], [b_coeff_mq135] shown values.
 * Please apply voltage-divider schematics as shown above.
 *
 * NOTE .1 : Mannually use multimeter and measure : eqv-resistance [mq135_eqv_RL], between [A0_pin vs GND] of 'MQ-135 sensor'
 *
 * NOTE .2 : For [m_slope_mq135] & [b_coeff_mq135] values:- follow [stage-1] above shown procedures.
 * */
#define mq135_eqv_RL 660.0f     // review the upper diagram clearly to know why this value is assigned
#define RatioMQ135CleanAir 3.6f // -> [RS / R0] = 3.6f
#define MQ135_VOLT_RESOLUTION_Vc 5.0f
#define m_slope_mq135 -0.401f
#define b_coeff_mq135 0.802f
typedef enum
{
    MQ135_GAS_ALARM_NO_GAS,
    MQ135_GAS_ALARM_COMBUSTIBLE_GAS_DETECTED,
    MQ135_GAS_ALARM_TOXIC_GAS_DETECTED,
    MQ135_GAS_ALARM_UNKNOWN,
    MQ135_GAS_ALARM_MAX
} e_mq135_gas_alarm_states_t;

//--------------------------------------------------------------------------------------------------------
//      ACTION FUNCTION
//--------------------------------------------------------------------------------------------------------

int sensor_0052_other_MQ135_NH3_detector(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

//--------------------------------------------------------------------------------------------------------

#endif