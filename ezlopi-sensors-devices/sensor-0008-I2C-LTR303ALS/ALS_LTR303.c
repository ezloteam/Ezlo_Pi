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
 * @file    ALS_LTR303.c
 * @brief   perform some function on ALS_LTR303
 * @author  ezlopi_team_np
 * @version 0.1
 * @date    xx
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "ALS_LTR303.h"

#warning "NABIN: do not use global and/or static variable in devices! PLEASE!!"

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/
unsigned char gain = LTR3XX_GAIN_1;                    // Gain setting, values = 0-7
unsigned char integration_time = LTR3XX_INTEGTIME_200; // Integration ("shutter") time in milliseconds
unsigned char measurement_rate = LTR3XX_MEASRATE_200;  // Interval between DATA_REGISTERS update
bool valid = 0, intr_status = 0, data_status = 0;
byte error;

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/

bool ltr303_is_data_available(void)
{
    ltr303_get_status(&valid, &gain, &intr_status, &data_status);
    return data_status;
}

esp_err_t ltr303_setup(uint32_t sda, uint32_t scl, bool initialize_i2c)
{
    s_ezlopi_i2c_master_t ltr303_i2c_master_conf = {
        .enable = true,
        .address = LTR303_ADDR,
        .channel = I2C_NUM_0,
        .sda = sda,
        .scl = scl,
        .clock_speed = I2C_MASTER_FREQ_HZ,
    };
    if (!initialize_i2c)
    {
        ltr303_i2c_master_conf.enable = false;
    }
    ltr303_begin(&ltr303_i2c_master_conf);
    unsigned char ID;

    if (ltr303_get_part_id(&ID))
    {
        // printf("Got Sensor Part ID: %02X\n", ID);
    }
    if (ltr303_get_manufac_id(&ID))
    {
        // printf("Got Manuf Part ID: %02X\n", ID);
    }

    // printf("Setting Gain...\n");
    ltr303_set_control(gain, false, false);

    // printf("Set timing...\n");
    ltr303_set_measurement_rate(integration_time, measurement_rate);

    // printf("Powerup...\n");
    ltr303_set_powerup();

    return ESP_OK;
}

esp_err_t ltr303_loop(void)
{

    return ESP_OK;
}

esp_err_t ltr303_get_val(ltr303_data_t *ltr303_data)
{
    if (ltr303_is_data_available())
    {
        unsigned int data0, data1;
        if (ltr303_get_data(&data0, &data1))
        {

// getData() returned true, communication was successful
#if DEBUG
            printf("data0: %d\n", data0);
            printf("data1: %d\n", data1);
#endif
            // To calculate lux, pass all your settings and readings
            // to the getLux() function.

            // The getLux() function will return 1 if the calculation
            // was successful, or 0 if one or both of the sensors was
            // saturated (too much light). If this happens, you can
            // reduce the integration time and/or gain.

            // Perform lux calculation:
            if (ltr303_get_lux(gain, integration_time, data0, data1, &(ltr303_data->lux)))
            {
                return ESP_OK;
            }
        }
    }
    return ESP_FAIL;
}

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          End of File
 *******************************************************************************/