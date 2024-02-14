#ifndef _SENSOR_0056_ADC_FORCE_SENSITIVE_RESISTOR_H_
#define _SENSOR_0056_ADC_FORCE_SENSITIVE_RESISTOR_H_

#include "ezlopi_core_actions.h"
#include "ezlopi_core_devices.h"

/**
 *  NOTE : 1) FSR - works with [3.3V]. {Don't use 5V. The pins are only 3.3V tolerant}
 *         2) Force accuracy ranges from approximately ± 5% to ± 25% depending on the consistency of the measurement and actuation system.
 *         3) The default resistance of sensor range [750KOhm to 1MOhm].
 *         4) Immediate Sensor resistance drop below 250KOhm, when force is experienced by sensor.
 *
 *         [froce sensitive resistor]
 *            [10kOhm - 1MOhm]
 * [3.3V] +Vin ---{Rs?}-----+
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
 *      [0V] ---------------+--> 0V -------+------------+ 0V
 *
 *
 *
 *  // STEP 1:-
 *      Here to find the 'Rs' value we can use voltage divider rule
 *      ->  Vo = (Rout / Rout + Rs) * Vin
 *      or, Vo = (10K / 10K + Rs) * Vin
 *      or, Rs = (10K/Vo)*Vin - 10K
 *      or, Rs = [(Vin/Vo) - 1] * 10K
 *
 *      Now putting Vin = 3.3V, in eqn(2),
 *          Rs = [(3.3V / Vo) - 1] * 10K  ....................(A)
 *
 *      So using equation(3) and 'Vo' voltage value we get the required resistance value 'Rs'
 *
 * // STEP 2:-
 *      According to the datasheet the Force(G) calculation formula is given as:
 *
 *      Calculation Process
 *      ~~~~~~~~~~~~~~~~~~~
 *      From, Semi-log graph,
 *               : $. Linear      : y = mx + b
 *               : $. Exponential : log(y) = m*log(x) + b ------------------------ (1)
 *
 *      # On Solving, we get,
 *      ~~~~~~~~~~~~~~~~~~~~~~
 *      => log(y) = m*log(x) + b
 *      => log(x) = [log(y) - b] / m
 *      =>      x = 10 ^ { [log(y) - b] / m } ------------ (2)
 *      where,
 *             -> m = [log(y) - log(y0)] / [log(x) - log(x0)]  = log(y/y0) / log(x/x0) = ? ;
 *             -> b = [log(y) - m*log(x)] = ?;
 *
 *
 *      Case. 1: Calculation of slope 'm' from FSR curve [Force vs kOhm] in semi-log graph:
 *      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *                                      (x0 , y0) and (  x  , y  )
 *                                          |               |
 *                                          V               V
 *                 #A. chose the points (20,30) and (10000,0.25) , we get 'm':
 *                 ->           m = log(0.25/30) / log(10000/20)
 *                 ->           m = -0.7703
 *
 *                 #B. We chose (50,10) , we get 'b':
 *                 ->           b = log(10) - (-0.7703)*log(50)
 *                 ->           b = 2.308
 *
 *      Case. 2: Calculation of Force(G). [ { Y to Rs(Kohm) } in y-axis ]and [ { X to Force(g) } in x-axis]
 *      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *                  #C. From eq(2),
 *                  ->          x = 10 ^ { [log(y) - b] / m }
 *                  ->   Force(G) = 10 ^ [ (log([Rs(Kohm)]) - 2.308) / -0.7703 ] ----------------------------(3)
 *
 *
 *
 *
 **/

// measure the resistor value using multimeter
#define FSR_Rout 10000.0f // Rout = 10000.0 Ohm
#define FSR_Vin 3.3f      // Vin = 3.3V  {Don't use 5V. The pins are only 3.3V tolerant}
#define m_slope_FSR -0.7703f
#define b_coeff_FSR 2.308f

#define FSR_correction_factor 2.5f

typedef struct fsr_t
{
    float fsr_value;
} fsr_t;
//-----------------------------------------------------------------------------------------------------------------------------
int sensor_0056_ADC_Force_Sensitive_Resistor(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#endif //_SENSOR_0056_ADC_FORCE_SENSITIVE_RESISTOR_H_