#ifndef _SENSOR_0062_OTHER_MQ7_CO_DETECTOR_H_
#define _SENSOR_0062_OTHER_MQ7_CO_DETECTOR_H_

#include "ezlopi_actions.h"
#include "ezlopi_devices.h"

/**
 *  NOTE : MQ7_CO_gas - module gives (0V - 4.2V) as analog output .
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
 *     => Reqv ----> RL    {2kOhm}         +------------+ 2.1V
 *     => 1KOhm      |      |              |                ^
 *                   |      |             {1KOhm}           |  esp32 analog input
 *                   V      |              |                v
 *      [0V] ---------------+--> 0V--------+------------+ 0V
 *
 *
 **/

/**
 *  From the graph, we can see that the resistance ratio in fresh air is a constant:
 *      (via black straight line of sensitivity graph in the Mq7_datasheet)
 *
 *  i.e. ---------------------------> [RS_calib / R0] =  27.5f
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
 *    Case. 1: Calculation of slope 'm' Using MQ7's CO-curve (CO) in semi-log graph:
 *
 *                                      (x0 , y0) and (  x  , y  )
 *                                          |               |
 *                                          V               V
 *                 #A. chose the points (50,1.7) and (1000,0.22) from the 'CO' graph-line
 *                 ->           m = log(0.22/1.7) / log(1000/50)
 *                 ->           m = -0.6825
 *
 *                 #B. We chose (100,1)
 *                 ->           b = log(1) - ( -0.6825 )*log(100)
 *                 ->           b = 1.365
 *
 *    Case. 2: Calculation of _ppm. First change, {Y to [ratio = RS/R0]} in y-axis  and {X to [_PPM]} in x-axis
 *                  #C. From eq(2),
 *                  ->          x = 10 ^ { [log(y) - b] / m }
 *                  ->       _ppm = 10 ^ [ (log([ratio]) -   1.365 ) / -0.6825   ] ----------------------------(3)
 *
 *
 *    _________ STAGE 1 : CALIBRATION ____________________________________________________
 *
 *    Case. 3: Here 'ratio' = [Rs/Ro] is still left to be calculated so,
 *                  #D. Using Rs formula :  Rs_calib = [(VC x RL) / VRL] - RL ; where Vc = 5V,
 *                                                                                    RL = 1000 ohm ,
 *                                                                                    VRL = sensor_analog_output
 *                  NOTE : use multimeter and measure the Equivalent resistance (RL)
 *                  ->     Rs_calib = [(5 * 1000) / VRL] - 1000 ;
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
 *                        -> _ppm = 10 ^ [ (log([Rs_gas / Ro]) - 1.365) / -0.6825 ]  ;
 *                                                  -> where ; Ro is taken from eq(4)
 *                                                           ; Rs_gas = [(5 * 1000) / VRL] - 1000 ;
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
 * Before writing below  [mq7_eqv_RL], [m_slope_mq7], [b_coeff_mq7] shown values.
 * Please apply voltage-divider schematics as shown above.
 *
 * NOTE .1 : Mannually use multimeter and measure : eqv-resistance [mq7_eqv_RL], between [A0_pin vs GND] of 'MQ7_sensor'
 *
 * NOTE .2 : For [m_slope_mq7] & [b_coeff_mq7] values:- follow [stage-1] above shown procedures.
 * */
#define mq7_eqv_RL 1000.0f	   // review the upper diagram clearly to know why this value is assigned
#define RatioMQ7CleanAir 27.5f // -> [RS / R0] = 27.5f
#define MQ7_VOLT_RESOLUTION_Vc 5.0f
#define m_slope_mq7 -0.6825f
#define b_coeff_mq7 1.365f

typedef enum
{
	MQ7_GAS_ALARM_NO_GAS,
	MQ7_GAS_ALARM_COMBUSTIBLE_GAS_DETECTED,
	MQ7_GAS_ALARM_TOXIC_GAS_DETECTED,
	MQ7_GAS_ALARM_UNKNOWN,
	MQ7_GAS_ALARM_MAX
} e_mq7_gas_alarm_states_t;

typedef struct mq7_value_t
{
    float _CO_ppm;
    float MQ7_R0_constant;  
}mq7_value_t;
//--------------------------------------------------------------------------------------------------------
//      ACTION FUNCTION
//--------------------------------------------------------------------------------------------------------

int sensor_0062_other_MQ7_CO_detector(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

//--------------------------------------------------------------------------------------------------------

#endif