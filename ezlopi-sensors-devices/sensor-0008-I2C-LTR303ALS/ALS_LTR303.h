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
 * @file    ALS_LTR303.h
 * @brief   perform some function on ALS_LTR303
 * @author  xx
 * @version 0.1
 * @date    xx
 */

#ifndef _ALS_LTR303_H_
#define _ALS_LTR303_H_

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#pragma once

#include "LTR303.h"

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
#define LTR303_SDA GPIO_NUM_18
#define LTR303_SCL GPIO_NUM_17
#define LTR303_INT_ALS_PIN GPIO_NUM_16
#define ACTIVE_I2C I2C_NUM_0

#define DEBUG 0
    typedef struct
    {
        double lux;
    } ltr303_data_t;

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/
    /**
     * @brief Loop
     *
     * @return esp_err_t
     */
    esp_err_t ltr303_loop(void);
    /**
     * @brief Funciton to get ltr303 value
     *
     * @param ltr303_data data
     * @return esp_err_t
     */
    esp_err_t ltr303_get_val(ltr303_data_t *ltr303_data);
    /**
     * @brief Function to check data available
     *
     * @return true
     * @return false
     */
    bool ltr303_is_data_available(void);

    /**
     * @brief : This function initializes the LTR303 Sensor
     * @param[in] initialize_i2c  :  true : initialize i2c,
                                     false : skips i2c initialization
    */
    esp_err_t ltr303_setup(uint32_t sda, uint32_t scl, bool initialize_i2c);

    esp_err_t ltr303_loop(void);
    esp_err_t ltr303_get_val(ltr303_data_t *ltr303_data);
    bool ltr303_is_data_available(void);

#ifdef __cplusplus
}
#endif

#endif // _ALS_LTR303_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/