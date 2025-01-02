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
 * @file    pms5003.h
 * @brief   perform some function on pms5003
 * @author  xx
 * @version 0.1
 * @date    xx
*/

#ifndef _PMS5003_H_
#define _PMS5003_H_

/*******************************************************************************
*                          Include Files
*******************************************************************************/

#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "driver/gpio.h"

#include "ezlopi_hal_uart.h"

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

    // Ambient Pro - Constants
#define PMS_SET_PIN GPIO_NUM_39
#define PMS_RESET_PIN GPIO_NUM_42
#define PMS_RX_PIN GPIO_NUM_41
#define PMS_TX_PIN GPIO_NUM_40
#define PMS_UART_PORT UART_NUM_1
#define PMS_UART_BUF_SIZE 128
#define PMS_UART_BUAD_RATE 9600

#define PMS_ACTIVE_TIME 30
#define PMS_SLEEP_TIME 60
#define PMS_STABILITY_TIME 15 // Here
#define PMS_TOTAL_TIME (PMS_ACTIVE_TIME + PMS_SLEEP_TIME)

#define PMS_GET_TOTAL_TIME(x, y) (x + y)

#define PMS_CHECK_ACTIVE_STATUS(status) (status & 0x01)
#define PMS_CHECK_SLEEP_STATUS(status) (status & 0x02)

#define PMS_SET_ACTIVE_STATUS(status) (status |= 0x01)
#define PMS_SET_SLEEP_STATUS(status) (status |= 0x02)
#define PMS_SET_READ_STATUS(status) (status |= 0x04)

/**! Structure holding Plantower's standard packet **/
    typedef struct PMSAQIdata
    {
        uint16_t framelen;       ///< How long this data chunk is
        uint16_t pm10_standard,  ///< Standard PM1.0
            pm25_standard,       ///< Standard PM2.5
            pm100_standard;      ///< Standard PM10.0
        uint16_t pm10_env,       ///< Environmental PM1.0
            pm25_env,            ///< Environmental PM2.5
            pm100_env;           ///< Environmental PM10.0
        uint16_t particles_03um, ///< 0.3um Particle Count
            particles_05um,      ///< 0.5um Particle Count
            particles_10um,      ///< 1.0um Particle Count
            particles_25um,      ///< 2.5um Particle Count
            particles_50um,      ///< 5.0um Particle Count
            particles_100um;     ///< 10.0um Particle Count
        uint16_t unused;         ///< Unused
        uint16_t checksum;       ///< Packet checksum
        uint8_t available;
    } PM25_AQI_Data;

    typedef struct
    {
        uint32_t pms_set_pin;
        uint32_t pms_reset_pin;
        uint32_t pms_tx_pin;
        uint32_t pms_rx_pin;
        uint32_t pms_baud_rate;
        uint32_t pms_active_time;
        uint32_t pms_sleep_time;
        uint32_t pms_stability_time;
        uint8_t volatile pmsStatusReg;
        /*
         * pmsStatusReg =  | X | X | X | X | X | EN_READ | SLEEP_MODE | ACTIVE_MODE |
         *                 | 7 | 6 | 5 | 4 | 3 |   2     |      1     |      0     |
         */
        PM25_AQI_Data pms_data;
        int counter;
    } s_pms5003_sensor_object;


    /*******************************************************************************
    *                          Extern Data Declarations
    *******************************************************************************/

    /*******************************************************************************
    *                          Extern Function Prototypes
    *******************************************************************************/

    /**
     * @brief Function to initialize PMS sensor
     *
     * @param pms_object
     */
    void pms_init(s_pms5003_sensor_object *pms_object);
    /**
     * @brief Function to print pms sensor data
     *
     * @param data
     */
    void pms_print_data(PM25_AQI_Data *data);
    /**
     * @brief Function to check if data is available
     *
     * @param data Pointer to data-structure
     * @return true
     * @return false
     */
    bool pms_is_data_available(PM25_AQI_Data *data);
    /**
     * @brief Function to set the data.available flag to false
     *
     * @param data Pointer to target structure
     */
    void pms_set_data_available_to_false(PM25_AQI_Data *data);

#ifdef __cplusplus
}
#endif

#endif// _PMS5003_H_

/*******************************************************************************
*                          End of File
*******************************************************************************/