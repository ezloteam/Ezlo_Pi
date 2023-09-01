

#ifndef __0042_SENSOR_I2C_TSL256_LUMINOSITY_H__
#define __0042_SENSOR_I2C_TSL256_LUMINOSITY_H__

#include "ezlopi_actions.h"
#include "ezlopi_devices.h"
#include "ezlopi_i2c_master.h"

//-----------------------------------------------------------------
// Definations
//-----------------------------------------------------------------
#define TSL2561_ADDRESS (0x39) // floating address

/* CMD_access = "COMMAND CODEs" + "REGISTER ADDRESS" > */

// COMMAND CODEs => Command Register [7:4]
//-----------------
#define SELECT_CMD_REGISTER (1 << 7)
#define CLEAR_INTERRUPT (1 << 6)      // set '1' to clear the current generated interrrupt
#define DO_WORD_TRANSACTION (1 << 5)  // set '1' to perform word operations   // In TSL2561 -> 1 word = 16-bit
#define DO_BLOCK_TRANSACTION (1 << 4) // set '1' to perform block operations  // I2C uses block -> 32-bit

// REGISTER ADDRESS => Command Register [3:0]
// -----------------
#define TSL2561_REGISTER_CONTROL (0x00)
#define TSL2561_REGISTER_TIMING (0x01)
#define TSL2561_REGISTER_THRESHHOLDL_LOW (0x02)  // not required write in this register; since we dont need to react to specific luminosity lvl. [i.e we are only extracting the lux values]
#define TSL2561_REGISTER_THRESHHOLDL_HIGH (0x03) // not required
#define TSL2561_REGISTER_THRESHHOLDH_LOW (0x04)  // not required
#define TSL2561_REGISTER_THRESHHOLDH_HIGH (0x05) // not required
#define TSL2561_REGISTER_INTERRUPT (0x06)
#define TSL2561_REGISTER_CRC (0x08)
#define TSL2561_REGISTER_ID (0x0A)
#define TSL2561_REGISTER_CHAN0_LOW (0x0C)
#define TSL2561_REGISTER_CHAN0_HIGH (0x0D)
#define TSL2561_REGISTER_CHAN1_LOW (0x0E)
#define TSL2561_REGISTER_CHAN1_HIGH (0x0F)
// -----------------

/* Assuming: TSL2561 package -> T_FN_CL package */
// -----------------
// Configuration CMD + bit masks
// -----------------
// 1. [R+W] Control Register (0H)
#define TSL2561_POWER_ON (1 << 1) | (1 << 0)
#define TSL2561_POWER_OFF (0x00)

// 2. [W] Timing Register (1H)
//  -----------------
#define TSL2561_LOW_GAIN_MODE_x1 (0 << 4)
#define TSL2561_HIGH_GAIN_MODE_x16 (1 << 4)
#define TSL2561_STOP_MANNUAL_INTEGRATION (0 << 3)
#define TSL2561_START_MANNUAL_INTEGRATION (1 << 3)
#define TSL2561_INTEGRATION_TIME_13_7_MS (0 << 1) | (0 << 0)
#define TSL2561_INTEGRATION_TIME_101_MS (0 << 1) | (1 << 0)
#define TSL2561_INTEGRATION_TIME_402_MS (1 << 1) | (0 << 0)
#define TSL2561_INTEGRATION_TIME_N_A (1 << 1) | (1 << 0) // for mannual start and stop integrations

// 3. [W] Interrupt Thershold Registers (2H-5H)
//  No configation for these registers

// 4. [W] Interrupt Control Registers (6H)
#define TSL2561_INTERRUPT_DISABLE_MODE (0 << 5) | (0 << 4)
#define TSL2561_INTERRUPT_LEVEL_MODE (0 << 5) | (1 << 4)
#define TSL2561_INTERRUPT_SMBALERT_MODE (1 << 5) | (0 << 4)

// 5. [R] CHIP_ID Register (AH)
#define TSL2561_PART_NUMBER (1 << 6) | (1 << 4) // 0b0101XXXX

/********************* Lux Calculation Parameters ******************/

// Calculation LUX-parameter
#define TSL2561_LUX_LUXSCALE (14)  // Scale by 2^14
#define TSL2561_LUX_RATIOSCALE (9) // Scale ratio by 2^9
#define TSL2561_LUX_CHSCALE (10)   // Scale channel values by 2^10
// Lux-scale for corresponding time values
#define TSL2561_LUX_CHSCALE_13_7_MS (0x7517)                  //  =>  (2^TSL2561_LUX_CHSCALE) * 322/11
#define TSL2561_LUX_CHSCALE_101_MS (0x0FE7)                   //  =>  (2^TSL2561_LUX_CHSCALE) * 322/81
#define TSL2561_LUX_CHSCALE_402_MS (1 << TSL2561_LUX_CHSCALE) //  =>  (2^TSL2561_LUX_CHSCALE) * 1

// T, FN and CL package values
#define TSL2561_LUX_K1T (0x0040) // 0.125 * 2^RATIO_SCALE
#define TSL2561_LUX_B1T (0x01f2) // 0.0304 * 2^LUX_SCALE
#define TSL2561_LUX_M1T (0x01be) // 0.0272 * 2^LUX_SCALE
#define TSL2561_LUX_K2T (0x0080) // 0.250 * 2^RATIO_SCALE
#define TSL2561_LUX_B2T (0x0214) // 0.0325 * 2^LUX_SCALE
#define TSL2561_LUX_M2T (0x02d1) // 0.0440 * 2^LUX_SCALE
#define TSL2561_LUX_K3T (0x00c0) // 0.375 * 2^RATIO_SCALE
#define TSL2561_LUX_B3T (0x023f) // 0.0351 * 2^LUX_SCALE
#define TSL2561_LUX_M3T (0x037b) // 0.0544 * 2^LUX_SCALE
#define TSL2561_LUX_K4T (0x0100) // 0.50 * 2^RATIO_SCALE
#define TSL2561_LUX_B4T (0x0270) // 0.0381 * 2^LUX_SCALE
#define TSL2561_LUX_M4T (0x03fe) // 0.0624 * 2^LUX_SCALE
#define TSL2561_LUX_K5T (0x0138) // 0.61 * 2^RATIO_SCALE
#define TSL2561_LUX_B5T (0x016f) // 0.0224 * 2^LUX_SCALE
#define TSL2561_LUX_M5T (0x01fc) // 0.0310 * 2^LUX_SCALE
#define TSL2561_LUX_K6T (0x019a) // 0.80 * 2^RATIO_SCALE
#define TSL2561_LUX_B6T (0x00d2) // 0.0128 * 2^LUX_SCALE
#define TSL2561_LUX_M6T (0x00fb) // 0.0153 * 2^LUX_SCALE
#define TSL2561_LUX_K7T (0x029a) // 1.3 * 2^RATIO_SCALE
#define TSL2561_LUX_B7T (0x0018) // 0.00146 * 2^LUX_SCALE
#define TSL2561_LUX_M7T (0x0012) // 0.00112 * 2^LUX_SCALE
#define TSL2561_LUX_K8T (0x029a) // 1.3 * 2^RATIO_SCALE
#define TSL2561_LUX_B8T (0x0000) // 0.000 * 2^LUX_SCALE
#define TSL2561_LUX_M8T (0x0000) // 0.000 * 2^LUX_SCALE

typedef enum
{
    TSL2561_INTEGRATIONTIME_13MS = 0,
    TSL2561_INTEGRATIONTIME_101MS,
    TSL2561_INTEGRATIONTIME_402MS,
} integration_t;

typedef enum
{
    TSL2561_GAIN_x1 = 0,
    TSL2561_GAIN_x16,
} gain_t;


int sensor_TSL256_luminosity_v3(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#endif // __0042_SENSOR_I2C_TSL256_LUMINOSITY_H__

