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
 * @file    ens160.c
 * @brief   perform some function on ens160
 * @author
 * @version 0.1
 * @date    xx
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

/*!
 * @file dfrobot_ens160.cpp
 * @brief  Define the infrastructure dfrobot_ens160 class
 * @n This is a digital metal-oxide multi-gas sensor. It can be controlled by I2C and SPI port.
 * @copyright Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license The MIT License (MIT)
 * @author [qsjhyy](yihuan.huang@dfrobot.com)
 * @version  V1.0
 * @date  2021-10-26
 * @url https://github.com/DFRobot/DFRobot_ENS160
 */
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ens160.h"
#include "EZLOPI_USER_CONFIG.h"

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/

/**
 * @brief  Init function for ens160
 *
 * @param ens160 Pointer to ens160-config struct
 * @return int [0 = NO_ERROR ; -1 = ERR_DATA_BUS ;  -2 = ERR_IC_VERSION]
 */
static int DFROBOT_ens160_begin(ens160_t *ens160);

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/
// static const char *TAG = "ENS160";

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/

/***************** Config function ******************************/

void DFROBOT_ens160_set_pwr_mode(ens160_t *ens160, uint8_t mode)
{
  DFROBOT_ens160_i2c_write_reg(ens160, ENS160_OPMODE_REG, &mode, sizeof(mode));
  vTaskDelay(20 / portTICK_PERIOD_MS); // Give it some time to switch mode
}

void DFROBOT_ens160_set_int_mode(ens160_t *ens160, uint8_t mode)
{
  mode |= (eINTDataDrdyEN | eIntGprDrdyDIS);
  DFROBOT_ens160_i2c_write_reg(ens160, ENS160_CONFIG_REG, &mode, sizeof(mode));
  vTaskDelay(20 / portTICK_PERIOD_MS); // Give it some time to switch mode
}

void DFROBOT_ens160_set_temp_and_hum(ens160_t *ens160, float ambient_temp, float relative_humidity)
{
  uint16_t temp = (ambient_temp + 273.15) * 64;
  uint16_t rh = relative_humidity * 512;
  uint8_t buf[4];

  buf[0] = temp & 0xFF;
  buf[1] = (temp & 0xFF00) >> 8;
  buf[2] = rh & 0xFF;
  buf[3] = (rh & 0xFF00) >> 8;
  DFROBOT_ens160_i2c_write_reg(ens160, ENS160_TEMP_IN_REG, buf, sizeof(buf));
}

/***************** Performance function ******************************/
uint8_t DFROBOT_ens160_get_ens160_status(ens160_t *ens160)
{
  DFROBOT_ens160_i2c_read_reg(ens160, ENS160_DATA_STATUS_REG, &(ens160->status), sizeof(s_sensor_status_t));
  return ens160->status.validity_flag;
}

uint8_t DFROBOT_ens160_get_aqi(ens160_t *ens160)
{
  uint8_t data = 0;
  DFROBOT_ens160_i2c_read_reg(ens160, ENS160_DATA_AQI_REG, &data, sizeof(data));
  return data;
}

uint16_t DFROBOT_ens160_get_tvoc(ens160_t *ens160)
{
  uint8_t buf[2] = {0};
  DFROBOT_ens160_i2c_read_reg(ens160, ENS160_DATA_TVOC_REG, buf, sizeof(buf));
  return ENS160_CONCAT_BYTES(buf[1], buf[0]);
}

uint16_t DFROBOT_ens160_get_eco2(ens160_t *ens160)
{
  uint8_t buf[2] = {0};
  DFROBOT_ens160_i2c_read_reg(ens160, ENS160_DATA_ECO2_REG, buf, sizeof(buf));
  return ENS160_CONCAT_BYTES(buf[1], buf[0]);
}

/************************** crc check calculation function ******************************/
uint8_t DFROBOT_ens160_get_misr(ens160_t *ens160)
{
  uint8_t crc = 0;
  DFROBOT_ens160_i2c_read_reg(ens160, ENS160_DATA_MISR_REG, &crc, sizeof(crc));
  return crc;
}

uint8_t DFROBOT_ens160_calc_misr(ens160_t *ens160, uint8_t data)
{
  uint8_t misr_xor = ((ens160->misr << 1) ^ data) & 0xFF;
  if ((ens160->misr & 0x80) == 0)
    ens160->misr = misr_xor;
  else
    ens160->misr = misr_xor ^ POLY;

  return ens160->misr;
}

/***************** Init and read/write of I2C and SPI interfaces ******************************/

int DFROBOT_ens160_i2c_begin(ens160_t *ens160)
{
  if (EZPI_SUCCESS == EZPI_hal_i2c_master_init(ens160->ezlopi_i2c))
  {
    return DFROBOT_ens160_begin(ens160); // Use the initialization function of the parent class
  }
  return EZPI_FAILED;
}

void DFROBOT_ens160_i2c_write_reg(ens160_t *ens160, uint8_t reg, const void *p_buf, size_t size)
{
  if (p_buf == NULL)
  {
    DBG("p_buf ERROR!! : null pointer");
  }

  uint8_t *_p_buf = (uint8_t *)ezlopi_malloc(__FUNCTION__, size + sizeof(reg));
  memcpy(_p_buf, &reg, sizeof(reg));
  memcpy(_p_buf + 1, p_buf, size);

  EZPI_hal_i2c_master_write_to_device(ens160->ezlopi_i2c, _p_buf, size + sizeof(reg));

  ezlopi_free(__FUNCTION__, _p_buf);
}

size_t DFROBOT_ens160_i2c_read_reg(ens160_t *ens160, uint8_t reg, void *p_buf, size_t size)
{
  if (NULL == p_buf)
  {
    DBG("p_buf ERROR!! : null pointer");
  }
  uint8_t *_p_buf = (uint8_t *)p_buf;

  EZPI_hal_i2c_master_write_to_device(ens160->ezlopi_i2c, &reg, sizeof(reg));
  EZPI_hal_i2c_master_read_from_device(ens160->ezlopi_i2c, _p_buf, size);

  return size;
}

void DFROBOT_ens160_get_data(ens160_t *ens160)
{
  /**
   * Get the sensor operating status
   * Return value: 0-Normal operation,
   *         1-Warm-Up phase, first 3 minutes after power-on.
   *         2-Initial Start-Up phase, first full hour of operation after initial power-on. Only once in the sensor’s lifetime.
   * note: Note that the status will only be stored in the non-volatile memory after an initial 24h of continuous
   *       operation. If unpowered before conclusion of said period, the ENS160 will resume "Initial Start-up" mode
   *       after re-powering.
   */
  uint8_t status = DFROBOT_ens160_get_ens160_status(ens160);
  ESP_LOGI("ENS160", "Sensor operating status : %d", status);

  /**
   * Get the air quality index
   * Return value: 1-Excellent, 2-Good, 3-Moderate, 4-Poor, 5-Unhealthy
   */
  uint8_t aqi = DFROBOT_ens160_get_aqi(ens160);
  ESP_LOGI("ENS160", "Air quality index : %d", aqi);

  /**
   * Get TVOC concentration
   * Return value range: 0–65000, unit: ppb
   */
  uint16_t tvoc = DFROBOT_ens160_get_tvoc(ens160);
  ESP_LOGI("ENS160", "Concentration of total volatile organic compounds : %d ppb", tvoc);

  /**
   * Get CO2 equivalent concentration calculated according to the detected data of VOCs and hydrogen (eCO2 – Equivalent CO2)
   * Return value range: 400–65000, unit: ppm
   * Five levels: Excellent(400 - 600), Good(600 - 800), Moderate(800 - 1000),
   *               Poor(1000 - 1500), Unhealthy(> 1500)
   */
  uint16_t eco2 = DFROBOT_ens160_get_eco2(ens160);
  ESP_LOGI("ENS160", "Carbon dioxide equivalent concentration : %d ppm", eco2);

  ens160->data.status = status;
  ens160->data.aqi = aqi;
  ens160->data.tvoc = tvoc;
  ens160->data.eco2 = eco2;
}

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/
static int DFROBOT_ens160_begin(ens160_t *ens160)
{
  ens160->misr = 0;
  uint8_t id_buf[2] = {0};
  if (0 == DFROBOT_ens160_i2c_read_reg(ens160, ENS160_PART_ID_REG, id_buf, sizeof(id_buf))) // Judge whether the data bus is successful
  {
    DBG("ERR_DATA_BUS");
    return ERR_DATA_BUS;
  }

  DBG("real sensor id= 0x%X", ENS160_CONCAT_BYTES(id_buf[1], id_buf[0]));
  if (ENS160_PART_ID != ENS160_CONCAT_BYTES(id_buf[1], id_buf[0])) // Judge whether the chip version matches
  {
    DBG("ERR_IC_VERSION");
    return ERR_IC_VERSION;
  }
  DFROBOT_ens160_set_pwr_mode(ens160, ENS160_STANDARD_MODE);
  DFROBOT_ens160_set_int_mode(ens160, 0x00);

  DBG("begin ok!");
  return NO_ERR;
}

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
