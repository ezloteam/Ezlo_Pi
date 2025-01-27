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
 * @file    bme680_bsec.h
 * @brief   perform some function on bme680
 * @author  
 * @version 0.1
 * @date    xx
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <math.h>
#include "bme68xLibrary.h"
#include "bsec2.h"

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
#pragma once

#define BME680_SDA GPIO_NUM_18
#define BME680_SCL GPIO_NUM_17

#define ACTIVE_I2C I2C_NUM_0
#define BME680_TEST 0
#define SEALEVELPRESSURE_HPA (1013.25)

    typedef struct
    {
        float iaq;
        bool iaq_accuracy;
        float co2_equivalent;
        float voc_equivalent;
        float temperature;
        float pressure;
        float humidity;
        float gas_resistance;
        float altitude;
        bool stabilization_status;
        bool run_in_status;
    } bme680_data_t;

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

    /**
     * @brief : This function checks the BSEC status, prints the respective error code. Halts in case of error
     * @param[in] bsec  : Bsec2 class object
     */
    void check_bsec_status();

    /**
    * @brief : This function initializes the BSEC and BME680 Sensor
    * @param[in] initialize_i2c  :  true : initialize i2c,
                                    false : skips i2c initialization
    */
    void bme680_setup(uint32_t sda, uint32_t scl, bool initialize_i2c);

    /**
     * @brief Function to get bme680 data
     *
     * @param data Pointer to data
     * @return true
     * @return false
     */
    bool bme680_get_data(bme680_data_t *data);

    /**
     * @brief Function to print data
     *
     * @param data Pointer to target data
     * @return true
     * @return false
     */
    bool bme680_print_data(bme680_data_t *data);

    /**
     * @brief Function to read altitude
     *
     * @param pressure Input pressure value
     * @param seaLevel Input seaLevel
     * @return float
     */
    float bme680_read_altitude(float pressure, float seaLevel);

    /**
     * @brief Get the data status object
     *
     * @return true
     * @return false
     */
    bool get_data_status();

#ifdef __cplusplus
}
#endif

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
