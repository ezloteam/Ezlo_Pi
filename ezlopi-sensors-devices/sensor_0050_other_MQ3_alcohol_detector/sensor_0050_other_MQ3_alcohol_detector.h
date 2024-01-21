#ifndef _SENSOR_0050_OTHER_MQ3_ALCOHOL_DETECTOR_H_
#define _SENSOR_0050_OTHER_MQ3_ALCOHOL_DETECTOR_H_

#include "ezlopi_core_actions.h"
#include "ezlopi_core_devices.h"
/**
 *  NOTE : MQ3_alcohol_gas - module gives (0V - 4.2V) as analog output .
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
 *     => ~650 Ohm   |      |              |                ^
 *                   |      |             {1KOhm}           |  esp32 analog input
 *                   V      |              |                v
 *      [0V] ---------------+--> 0V--------+------------+ 0V
 *
 *                     [DIAGRAM - 1]
 **/

/**
 *  From the graph, we can see that the resistance ratio in fresh air is a constant:
 *      (via black straight line of sensitivity graph in the Mq-3 datasheet)
 *
 *  i.e. ---------------------------> [RS_calib / R0] = 60
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
 *    Case. 1: Calculation of slope 'm' Using MQ3's 'alcohol'-curve  in semi-log graph:
 *
 *                                      (x0 , y0) and (  x  , y  )
 *                                          |               |       x-axis : mg/l [ 0.4mg/l = 200ppm]
 *                                          V               V
 *                 #A. chose the points (0.1,2.4) and (10,0.12) from the 'alcohol' graph-line
 *                 ->           m = [log(y/y0) / log(x/x0)]
 *                 ->           m = log(0.12/2.4) / log(10/0.1)
 *                 ->           m =  -0.65
 *
 *                 #B. We chose (1,0.55) , from eq(1)
 *                                  |      x-axis : ppm
 *                                  V
 *                          or  (50,0.55)
 *                 ->           b = [log(y) - m*log(x)]
 *                 ->           b = log(0.55) - (-0.65)*log(1)
 *                 ->           b =  -0.259
 *
 *    Case. 2: Calculation of _mg/L. First change, {Y to [ratio = RS/R0]} in y-axis  and {X to [_mg/L]} in x-axis
 *                  #C. From eq(2),
 *                  ->          x = 10 ^ { [log(y) - b] / m }
 *                  ->       _mg/L = 10 ^ [ (log([ratio]) + 0.259) / -0.65 ] ----------------------------(3)
 *
 *
 *    _________ STAGE 1 : CALIBRATION ____________________________________________________
 *
 *    Case. 3: Here 'ratio' = [Rs/Ro] is still left to be calculated so,
 *                  #D. Using Rs formula :  Rs_calib = [(VC x RL) / VRL] - RL ; where Vc = 5V,
 *                                                                                  RL = 65O ohm ,
 *                                                                                  VRL = sensor_analog_output
 *                  NOTE : use multimeter and measure the Equivalent resistance (RL)
 *                  ->     Rs_calib = [(5 * 980) / VRL] - 980 ;
 *
 *                  #E. Now [Ro constant value] , we get :
 *                        ->   Ro = ratio/Rs_calib
 *                        ->   Ro = 60 / Rs_calib  --------------- (4)
 *
 *
 *    _________ STAGE 2 : mg/L Calculation ____________________________________________________
 *
 *    Case. 4: Calculation of _mg/L using eq(3), we get:
 *                  #F. Formula of _mg/L is:-
 *
 *                      1. Method 1:
 *                        -> _mg/L = 10 ^ [ (log([Rs_gas / Ro]) + 0.259) / -0.65  ]  ;
 *
 *
 *                                                  -> where ; Ro is taken from eq(4)
 *                                                           ; Rs_gas = [(5 * 980) / VRL] - 980 ;
 *
 *
 *
 *
 *
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
 * Before writing below  [mq3_eqv_RL], [m_slope_mq3], [b_coeff_mq3] shown values.
 * Please apply voltage-divider schematics as shown above.
 *
 * NOTE .1 : Mannually use multimeter and measure : eqv-resistance [mq3_eqv_RL], between [A0_pin vs GND] of 'MQ-3 sensor'
 *
 * NOTE .2 : For [m_slope_mq3] & [b_coeff_mq3] values:- follow [stage-1] above shown procedures.
 * */

#define mq3_eqv_RL 660.0f      // review the upper diagram(1) clearly to know why this value is assigned
#define RatioMQ3CleanAir 60.0f // -> [RS / R0] = 60
#define MQ3_VOLT_RESOLUTION_Vc 5.0f
#define m_slope_mq3 -0.65f
#define b_coeff_mq3 -0.259f

typedef enum
{
    MQ3_GAS_ALARM_NO_GAS,
    MQ3_GAS_ALARM_COMBUSTIBLE_GAS_DETECTED,
    MQ3_GAS_ALARM_TOXIC_GAS_DETECTED,
    MQ3_GAS_ALARM_UNKNOWN,
    MQ3_GAS_ALARM_MAX
} e_mq3_gas_alarm_states_t;

typedef struct mq3_value_t
{
    float _alcohol_ppm;
    float MQ3_R0_constant;
} mq3_value_t;
//--------------------------------------------------------------------------------------------------------
//      ACTION FUNCTION
//--------------------------------------------------------------------------------------------------------

int sensor_0050_other_MQ3_alcohol_detector(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

//--------------------------------------------------------------------------------------------------------

#endif