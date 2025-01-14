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
 * @file    gxhtc3.h
 * @brief   perform some function on gxhtc3
 * @author  ezlopi_team_np
 * @version 0.1
 * @date    xx
 */

#ifndef _GXHTC3_H_
#define _GXHTC3_H_

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

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
#define GXHTC3_CRC_POLY 0x31

#define GXHTC3_I2C_SLAVE_ADDRESS 0X60
#define GXHTC3_I2C_TIMEOUT 100

#define GXHTC3_I2C_RAW_DATA_LEN 6
#define GXHTC3_I2C_REG_ID_READ_LEN 3

#define GXHTC3_I2C_CMD_SLEEP 0XB098
#define GXHTC3_I2C_CMD_WAKEUP 0X3517
#define GXHTC3_I2C_CMD_SOFT_RESET 0x805D
#define GXHTC3_I2C_CMD_READ_REG_ID 0xEFC8
#define GXHTC3_I2C_CMD_READ_NM_CSEN_TF 0x7CA2
#define GXHTC3_I2C_CMD_READ_NM_CSEN_HF 0x5C24
#define GXHTC3_I2C_CMD_READ_NM_CSDS_TF 0x7866
#define GXHTC3_I2C_CMD_READ_NM_CSDS_HF 0x58E0
#define GXHTC3_I2C_CMD_READ_LP_CSEN_TF 0x6458
#define GXHTC3_I2C_CMD_READ_LP_CSEN_HF 0x44DE
#define GXHTC3_I2C_CMD_READ_LP_CSDS_TF 0x609C
#define GXHTC3_I2C_CMD_READ_LP_CSDS_HF 0x401A

    typedef struct s_gxhtc3_id
    {
        uint16_t id;
        bool status;

    } s_gxhtc3_id_t;

    typedef struct s_gxhtc3_sensor_handler
    {
        uint8_t i2c_ch_num;
        uint8_t i2c_slave_addr;
        s_gxhtc3_id_t id;
        uint8_t raw_data_reg[GXHTC3_I2C_RAW_DATA_LEN];
        bool read_status;
        float reading_temp_c;
        float reading_rh;

    } s_gxhtc3_sensor_handler_t;

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/
    /**
     * @brief Function to initialize GXHTC3
     *
     * @param i2c_ch_num Target I2C channel
     * @param i2c_slave_addr Slave address
     * @return s_gxhtc3_sensor_handler_t*
     */
    s_gxhtc3_sensor_handler_t *GXHTC3_init(int32_t i2c_ch_num, uint8_t i2c_slave_addr);

    /**
     * @brief Function to read GXHTC3 sensor data
     *
     * @param handler Handler to add sensor data
     * @return true
     * @return false
     */
    bool GXHTC3_read_sensor(s_gxhtc3_sensor_handler_t *handler);

#ifdef __cplusplus
}
#endif

#endif /* _GXHTC3_H_ */

/*******************************************************************************
 *                          End of File
 *******************************************************************************/