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
 * @file    sensor_0057_other_KY026_FlameDetector.h
 * @brief   perform some function on sensor_0057
 * @author  
 * @version 0.1
 * @date    xx
 */

#ifndef _SENSOR_0057_OTHER_KY026_FLAMEDETECTOR_H_
#define _SENSOR_0057_OTHER_KY026_FLAMEDETECTOR_H_

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
     *  NOTE : Flame Sensor [KY-026 module] gives (0V - 5V) analog output .
     *  But ESP32- only allows upto 2.4V max input.
     *
     *  Remedy:  introduce a voltage divider of ratio [1:2 , i.e. 50%]  on the Sensor analog output ,
     *           so that esp32 adc pin recieves half voltage only.
     *           (Half of 5V) -> 2.5V
     *
     *
     *
     *  >   +Vc[5V] ------------+
     *                          |
     *                       {1KOhm}
     *                          |
     *                          +--------------+ ~2.5V
     *                          |              ^
     *                       {1kOhm}           | esp32 analog input
     *                          |              v
     *  >     [0V] -------------+--------------+ 0V
     *
     *
     **/

    //------------------------------------------
    /**
     * Please don't forget to uncomment ,
     * -> If you added a voltage divider at sensor's analog output. [Make sure voltage does not exceed 2.5V]
     *      |
     *      |
     *  #define VOLTAGE_DIVIDER_ADDED 1
     */
#define VOLTAGE_DIVIDER_ADDED 1
    //------------------------------------------
    typedef enum
    {
        KY206_HEAT_ALARM_HEAT_OK,
        KY206_HEAT_ALARM_OVERHEAT_DETECTED,
        KY206_HEAT_ALARM_UNDERHEAT_DETECTED,
        KY206_HEAT_ALARM_UNKNOWN,
        KY206_HEAT_ALARM_MAX
    } e_ky206_heat_alarm_states_t;

    typedef struct flame_t
    {
        float absorbed_percent;
    } flame_t;

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
    ezlopi_error_t SENSOR_0057_other_ky026_flamedetector(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#ifdef __cplusplus
}
#endif

#endif //_SENSOR_0057_OTHER_KY026_FLAMEDETECTOR_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/