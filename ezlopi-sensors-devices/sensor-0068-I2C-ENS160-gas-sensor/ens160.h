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
 * @file    ens160.h
 * @brief   perform some function on ens160
 * @author  ezlopi_team_np
 * @version 0.1
 * @date    xx
 */

#ifndef __DFRobot_ENS160_H__
#define __DFRobot_ENS160_H__

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

/*!
 * @file  DFRobot_ENS160.h
 * @brief  Define infrastructure of DFRobot_ENS160 class
 * @details  This is a Digital Metal-Oxide Multi-Gas Sensor. It can be controlled by I2C and SPI port.
 * @n        Detection of a variety of gases, such as volatile organic compounds (VOCs), as
 * @n        well as hydrogen and nitrogen dioxide, has superior selectivity and accuracy.
 * @copyright  Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license  The MIT License (MIT)
 * @author  [qsjhyy](yihuan.huang@dfrobot.com)
 * @version  V1.0
 * @date  2021-10-25
 * @url  https://github.com/DFRobot/DFRobot_ENS160
 */

#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "ezlopi_hal_i2c_master.h"

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

#define ENABLE_DBG //!< Open this macro and you can see the details of the program
#ifdef ENABLE_DBG
  // #define DBG(...) {Serial.print("[");Serial.print(__FUNCTION__); Serial.print("(): "); Serial.print(__LINE__); Serial.print(" ] "); Serial.println(__VA_ARGS__);}
#define DBG(fmt, ...) ESP_LOGI("ENS160", "[%s():%d] " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define DBG(...)
#endif

#define delay(x) vTaskDelay((x) / portTICK_RATE_MS)

#define POLY (uint8_t)(0x1D)             ///< 0b00011101 = x^8+x^4+x^3+x^2+x^0 (x^8 is implicit)
#define ENS160_PART_ID (uint16_t)(0x160) ///< ENS160 chip version

  /* ENS160 register address */
#define ENS160_PART_ID_REG (uint8_t)(0x00) ///< This 2-byte register contains the part number in little endian of the ENS160.

#define ENS160_OPMODE_REG (uint8_t)(0x10)  ///< This 1-byte register sets the Operating Mode of the ENS160.
#define ENS160_CONFIG_REG (uint8_t)(0x11)  ///< This 1-byte register configures the action of the INTn pin.
#define ENS160_COMMAND_REG (uint8_t)(0x12) ///< This 1-byte register allows some additional commands to be executed on the ENS160.

#define ENS160_TEMP_IN_REG (uint8_t)(0x13) ///< This 2-byte register allows the host system to write ambient temperature data to ENS160 for compensation.
#define ENS160_RH_IN_REG (uint8_t)(0x15)   ///< This 2-byte register allows the host system to write relative humidity data to ENS160 for compensation.

#define ENS160_DATA_STATUS_REG (uint8_t)(0x20) ///< This 1-byte register indicates the current STATUS of the ENS160.

#define ENS160_DATA_AQI_REG (uint8_t)(0x21)  ///< This 1-byte register reports the calculated Air Quality Index according to the UBA.
#define ENS160_DATA_TVOC_REG (uint8_t)(0x22) ///< This 2-byte register reports the calculated TVOC concentration in ppb.
#define ENS160_DATA_ECO2_REG (uint8_t)(0x24) ///< This 2-byte register reports the calculated equivalent CO2-concentration in ppm, based on the detected VOCs and hydrogen.
#define ENS160_DATA_ETOH_REG (uint8_t)(0x22) ///< This 2-byte register reports the calculated ethanol concentration in ppb.

#define ENS160_DATA_T_REG (uint8_t)(0x30)  ///< This 2-byte register reports the temperature used in its calculations (taken from TEMP_IN, if supplied).
#define ENS160_DATA_RH_REG (uint8_t)(0x32) ///< This 2-byte register reports the relative humidity used in its calculations (taken from RH_IN if supplied).

#define ENS160_DATA_MISR_REG (uint8_t)(0x38) ///< This 1-byte register reports the calculated checksum of the previous DATA_ read transaction (of n-bytes).

#define ENS160_GPR_WRITE_REG (uint8_t)(0x40) ///< This 8-byte register is used by several functions for the Host System to pass data to the ENS160.
#define ENS160_GPR_READ_REG (uint8_t)(0x48)  ///< This 8-byte register is used by several functions for the ENS160 to pass data to the Host System.

/* CMD(0x12) register command */
#define ENS160_COMMAND_NOP (uint8_t)(0x00)        ///< reserved. No command.
#define ENS160_COMMAND_GET_APPVER (uint8_t)(0x0E) ///< Get FW Version Command.
#define ENS160_COMMAND_CLRGPR (uint8_t)(0xCC)     ///< Clears GPR Read Registers Command.

/* OPMODE(Address 0x10) register mode */
#define ENS160_SLEEP_MODE (uint8_t)(0x00)    ///< DEEP SLEEP mode (low power standby).
#define ENS160_IDLE_MODE (uint8_t)(0x01)     ///< IDLE mode (low-power).
#define ENS160_STANDARD_MODE (uint8_t)(0x02) ///< STANDARD Gas Sensing Modes.

/* Convenience Macro */
#define ENS160_CONCAT_BYTES(msb, lsb) (((uint16_t)msb << 8) | (uint16_t)lsb) ///< Macro combines two 8-bit data into one 16-bit data

#define ENS160_I2C_SDA GPIO_NUM_18
#define ENS160_I2C_SCL GPIO_NUM_17
#define ENS160_I2C_ADDRESS 0x52
#define ENS160_I2C_SPEED 400000

#define NO_ERR 0            // No error
#define ERR_DATA_BUS (-1)   // Data bus error
#define ERR_IC_VERSION (-2) // Chip version error

  /************************* Interrupt Pin Configuration *******************************/
  /**
   * @enum  eINTPinActive_t
   * @brief  Interrupt pin active signal level
   */
  typedef enum
  {
    eINTPinActiveLow = 0 << 6,  /**< Active low */
    eINTPinActiveHigh = 1 << 6, /**< Active high */
  } eINTPinActive_t;

  /**
   * @enum  eINTPinMode_t
   * @brief  Interrupt pin output driving mode
   */
  typedef enum
  {
    eINTPinOD = 0 << 5, /**<  Open drain */
    eINTPinPP = 1 << 5, /**< Push / Pull */
  } eINTPinMode_t;

  /**
   * @enum  eIntGprDrdy_t
   * @brief  The status of interrupt pin when new data appear in General Purpose Read Registers
   */
  typedef enum
  {
    eIntGprDrdyDIS = 0 << 3, /**< Disable */
    eIntGprDrdyEN = 1 << 3,  /**< Enable */
  } eIntGprDrdy_t;

  /**
   * @enum  eINTDataDrdy_t
   * @brief  The status of interrupt pin when new data appear in DATA_XXX
   */
  typedef enum
  {
    eINTDataDrdyDIS = 0 << 1, /**< Disable */
    eINTDataDrdyEN = 1 << 1,  /**< Enable */
  } eINTDataDrdy_t;

  /**
   * @enum  eINTMode_t
   * @brief  Interrupt pin main switch mode
   */
  typedef enum
  {
    eINTModeDIS = 0, /**< Disable */
    eINTModeEN = 1,  /**< Enable */
  } eINTMode_t;

  /**
   * @enum  eSensorStatus_t
   * @brief  The sensor status
   */
  typedef enum
  {
    eNormalOperation = 0,     /**< Normal operation */
    eWarmUpPhase = 1,         /**< Warm-Up phase */
    eInitialStartUpPhase = 2, /**< Initial Start-Up phase */
    eInvalidOutput = 3,       /**< Invalid output */
  } eSensorStatus_t;

  /************************* Sensor Status *******************************/
  /**
   * @struct sSensorStatus_t
   * @brief Sensor status flag is buffered into "DATA_STATUS (Address 0x20)" register
   * @note Register structure:
   * @n -----------------------------------------------------------------------------------
   * @n |    b7    |   b6   |    b5   |    b4   |    b3   |    b2   |    b1    |    b0    |
   * @n -----------------------------------------------------------------------------------
   * @n |  STATAS  | STATER |     reserved      |   VALIDITY FLAG   |  NEWDAT  |  NEWGPR  |
   * @n -----------------------------------------------------------------------------------
   */
  typedef struct
  {
    uint8_t gpr_drdy : 1;      /**< General purpose register data ready */
    uint8_t data_drdy : 1;     /**< Measured data ready */
    uint8_t validity_flag : 2; /**< 0: Normal operation, 1: Warm-Up phase, 2: Initial Start-Up phase, 3: Invalid output */
    uint8_t reserved : 2;      /**< Reserved bit */
    uint8_t stater : 1;        /**< High indicates that an error is detected. E.g. Invalid Operating Mode has been selected. */
    uint8_t status : 1;        /**< High indicates that an OPMODE is running */
  } __attribute__((packed)) s_sensor_status_t;

  typedef struct ens160_data
  {
    uint8_t status;
    uint8_t aqi;
    uint16_t tvoc;
    uint16_t eco2;
  } ens160_data_t;

  typedef struct ens160
  {
    ens160_data_t data;
    s_ezlopi_i2c_master_t *ezlopi_i2c;
    // Private variables
    s_sensor_status_t status;
    uint8_t misr; // Mirror of DATA_MISR (0 is hardware default)
  } ens160_t;

  /*******************************************************************************
   *                          Extern Data Declarations
   *******************************************************************************/

  /*******************************************************************************
   *                          Extern Function Prototypes
   *******************************************************************************/

  /**
   * @brief Function to extract ens160 sensor data
   *
   * @param ens160 Pointer to store extracted data to.
   */
  void DFROBOT_ens160_get_data(ens160_t *ens160);

  /************************** Config function ******************************/
  /**
   * @fn setPWRMode
   * @brief Set power mode
   * @param mode Configurable power mode:
   * @n       ENS160_SLEEP_MODE: DEEP SLEEP mode (low power standby)
   * @n       ENS160_IDLE_MODE: IDLE mode (low-power)
   * @n       ENS160_STANDARD_MODE: STANDARD Gas Sensing Modes
   * @return None
   */
  void DFROBOT_ens160_set_pwr_mode(ens160_t *ens160, uint8_t mode);

  /**
   * @fn setINTMode
   * @brief Interrupt config (INT)
   * @param mode Interrupt mode to be set, perform OR operation on the following to get mode:
   * @n       Interrupt setting (the interrupt occur when a new data is uploaded): eINTModeDIS-Disable interrupt, eINTModeEN-Enable interrupt
   * @n       Interrupt pin output driving mode: eINTPinOD-Open drain output, eINTPinPP-Push pull output
   * @n       Interrupt pin active level: eINTPinActiveLow-Active low, eINTPinActiveHigh-Active high
   * @return None
   */
  void DFROBOT_ens160_set_int_mode(ens160_t *ens160, uint8_t mode);

  /**
   * @fn setTempAndHum
   * @brief Users write ambient temperature and relative humidity into ENS160 for calibration compensation of the measured gas data.
   * @param ambientTemp Compensate the current ambient temperature, float type, unit: C
   * @param relativeHumidity Compensate the current ambient humidity, float type, unit: %rH
   * @return None
   */
  void DFROBOT_ens160_set_temp_and_hum(ens160_t *ens160, float ambient_temp, float relative_humidity);

  /************************** Performance function ******************************/
  /**
   * @fn getENS160Status
   * @brief This API is used to get the sensor operating status
   * @return Operating status:
   * @n        eNormalOperation: Normal operation;
   * @n        eWarmUpPhase: Warm-Up phase;
   * @n        eInitialStartUpPhase: Initial Start-Up phase;
   * @n        eInvalidOutput: Invalid output
   */
  uint8_t DFROBOT_ens160_get_ens160_status(ens160_t *ens160);

  /**
   * @fn getAQI
   * @brief Get the air quality index calculated on the basis of UBA
   * @return Return value range: 1-5 (Corresponding to five levels of Excellent, Good, Moderate, Poor and Unhealthy respectively)
   */
  uint8_t DFROBOT_ens160_get_aqi(ens160_t *ens160);

  /**
   * @fn getTVOC
   * @brief Get TVOC concentration
   * @return Return value range: 0–65000, unit: ppb
   */
  uint16_t DFROBOT_ens160_get_tvoc(ens160_t *ens160);

  /**
   * @fn getECO2
   * @brief Get CO2 equivalent concentration calculated according to the detected data of VOCs and hydrogen (eCO2 – Equivalent CO2)
   * @return Return value range: 400–65000, unit: ppm
   * @note Five levels: Excellent(400 - 600), Good(600 - 800), Moderate(800 - 1000),
   * @n                  Poor(1000 - 1500), Unhealthy(> 1500)
   */
  uint16_t DFROBOT_ens160_get_eco2(ens160_t *ens160);

  /************************** crc check calculation function and command sending function ******************************/
  /**
   * @fn getMISR
   * @brief Get the current crc check code of the sensor
   * @return The current crc check code of the sensor
   */
  uint8_t DFROBOT_ens160_get_misr(ens160_t *ens160);

  /**
   * @fn calcMISR
   * @brief Calculate the current crc check code to contrast it with the MISR read from the sensor
   * @param data The measured data just obtained from the sensor
   * @return The current calculated crc check code
   */
  uint8_t DFROBOT_ens160_calc_misr(ens160_t *ens160, uint8_t data);

  /**
   * @fn begin
   * @brief Subclass init function
   * @return int type, indicates returning init status
   * @retval 0 NO_ERROR
   * @retval -1 ERR_DATA_BUS
   * @retval -2 ERR_IC_VERSION
   */
  int DFROBOT_ens160_i2c_begin(ens160_t *ens160);

  /**
   * @fn writeReg
   * @brief Write register value through I2C bus
   * @param reg  Register address 8bits
   * @param pBuf Storage and buffer for data to be written
   * @param size Length of data to be written
   * @return None
   */
  void DFROBOT_ens160_i2c_write_reg(ens160_t *ens160, uint8_t reg, const void *p_buf, size_t size);

  /**
   * @fn readReg
   * @brief Read register value through I2C bus
   * @param reg  Register address 8bits
   * @param pBuf Storage and buffer for data to be read
   * @param size Length of data to be read
   * @return Return the read length, returning 0 means reading failed
   */
  size_t DFROBOT_ens160_i2c_read_reg(ens160_t *ens160, uint8_t reg, void *pBuf, size_t size);

#ifdef __cplusplus
}
#endif

#endif // __DFRobot_ENS160_H__

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
