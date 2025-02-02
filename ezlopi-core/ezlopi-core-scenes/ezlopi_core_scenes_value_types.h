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
 * @file    ezlopi_core_scenes_value_types.h
 * @brief   MACROs for scene-value-types
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 * @version 0.1
 * @date    12th DEC 2024
 */

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

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

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/

    EZLOPI_VALUE_TYPE(NONE, "none")
    //
    EZLOPI_VALUE_TYPE(INT, "int")
    EZLOPI_VALUE_TYPE(BOOL, "bool")
    EZLOPI_VALUE_TYPE(FLOAT, "float")
    EZLOPI_VALUE_TYPE(STRING, "string")
    EZLOPI_VALUE_TYPE(OBJECT, "object")
    EZLOPI_VALUE_TYPE(CREDENTIAL, "credential")
    EZLOPI_VALUE_TYPE(DICTIONARY, "dictionary")
    EZLOPI_VALUE_TYPE(ARRAY, "array")
    EZLOPI_VALUE_TYPE(RGB, "rgb")
    EZLOPI_VALUE_TYPE(CAMERA_STREAM, "cameraStream")
    EZLOPI_VALUE_TYPE(USER_CODE, "userCode")
    EZLOPI_VALUE_TYPE(WEEKLY_INTERVAL, "weekly_interval")
    EZLOPI_VALUE_TYPE(DAILY_INTERVAL, "daily_interval")
    EZLOPI_VALUE_TYPE(ENUM, "enum")
    EZLOPI_VALUE_TYPE(TOKEN, "token")
    EZLOPI_VALUE_TYPE(BUTTON_STATE, "button_state")
    EZLOPI_VALUE_TYPE(USER_LOCK_OPERATION, "user_lock_operation")
    EZLOPI_VALUE_TYPE(USER_CODE_ACTION, "user_code_action")
    EZLOPI_VALUE_TYPE(SOUND_INFO, "sound_info")
    EZLOPI_VALUE_TYPE(CAMERA_HOTZONE, "camera_hotzone")
    EZLOPI_VALUE_TYPE(HOTZONE_MATCH, "hotzone_match")
    EZLOPI_VALUE_TYPE(GEOFENCE, "geofence")
    EZLOPI_VALUE_TYPE(ILLUMINANCE, "illuminance")
    EZLOPI_VALUE_TYPE(PRESSURE, "pressure")
    EZLOPI_VALUE_TYPE(SUBSTANCE_AMOUNT, "substance_amount")
    EZLOPI_VALUE_TYPE(POWER, "power")
    EZLOPI_VALUE_TYPE(VELOCITY, "velocity")
    EZLOPI_VALUE_TYPE(ACCELERATION, "acceleration")
    EZLOPI_VALUE_TYPE(DIRECTION, "direction")
    EZLOPI_VALUE_TYPE(GENERAL_PURPOSE, "general_purpose")
    EZLOPI_VALUE_TYPE(ACIDITY, "acidity")
    EZLOPI_VALUE_TYPE(ELECTRIC_POTENTIAL, "electric_potential")
    EZLOPI_VALUE_TYPE(ELECTRIC_CURRENT, "electric_current")
    EZLOPI_VALUE_TYPE(FORCE, "force")
    EZLOPI_VALUE_TYPE(IRRADIANCE, "irradiance")
    EZLOPI_VALUE_TYPE(PRECIPITATION, "precipitation")
    EZLOPI_VALUE_TYPE(LENGTH, "length")
    EZLOPI_VALUE_TYPE(MASS, "mass")
    EZLOPI_VALUE_TYPE(VOLUME_FLOW, "volume_flow")
    EZLOPI_VALUE_TYPE(VOLUME, "volume")
    EZLOPI_VALUE_TYPE(ANGLE, "angle")
    EZLOPI_VALUE_TYPE(FREQUENCY, "frequency")
    EZLOPI_VALUE_TYPE(SEISMIC_INTENSITY, "seismic_intensity")
    EZLOPI_VALUE_TYPE(SEISMIC_MAGNITUDE, "seismic_magnitude")
    EZLOPI_VALUE_TYPE(ULTRAVIOLET, "ultraviolet")
    EZLOPI_VALUE_TYPE(ELECTRICAL_RESISTANCE, "electrical_resistance")
    EZLOPI_VALUE_TYPE(ELECTRICAL_CONDUCTIVITY, "electrical_conductivity")
    EZLOPI_VALUE_TYPE(LOUDNESS, "loudness")
    EZLOPI_VALUE_TYPE(MOISTURE, "moisture")
    EZLOPI_VALUE_TYPE(TIME, "time")
    EZLOPI_VALUE_TYPE(RADON_CONCENTRATION, "radon_concentration")
    EZLOPI_VALUE_TYPE(BLOOD_PRESSURE, "blood_pressure")
    EZLOPI_VALUE_TYPE(ENERGY, "energy")
    EZLOPI_VALUE_TYPE(RF_SIGNAL_STRENGTH, "rf_signal_strength")
    EZLOPI_VALUE_TYPE(TEMPERATURE, "temperature")
    EZLOPI_VALUE_TYPE(HUMIDITY, "humidity")
    EZLOPI_VALUE_TYPE(KILO_VOLT_AMPERE_HOUR, "kilo_volt_ampere_hour")
    EZLOPI_VALUE_TYPE(REACTIVE_POWER_INSTANT, "reactive_power_instant")
    EZLOPI_VALUE_TYPE(AMOUNT_OF_USEFUL_ENERGY, "amount_of_useful_energy")
    EZLOPI_VALUE_TYPE(REACTIVE_POWER_CONSUMPTION, "reactive_power_consumption")
    EZLOPI_VALUE_TYPE(DEVICE, "device")
    EZLOPI_VALUE_TYPE(ITEM, "item")
    EZLOPI_VALUE_TYPE(EXPRESSION, "expression")
    EZLOPI_VALUE_TYPE(24_HOURS_TIME, "24_hours_time")
    EZLOPI_VALUE_TYPE(24_HOURS_TIME_ARRAY, "24_hours_time_array")
    EZLOPI_VALUE_TYPE(INT_ARRAY, "int_array")
    EZLOPI_VALUE_TYPE(INTERVAL, "interval")
    EZLOPI_VALUE_TYPE(HMS_INTERVAL, "hms_interval")
    // Scene specific
    EZLOPI_VALUE_TYPE(BLOCK, "block")
    EZLOPI_VALUE_TYPE(BLOCKS, "blocks")
    EZLOPI_VALUE_TYPE(HOUSE_MODE_ID_ARRAY, "houseModeId_array")
    EZLOPI_VALUE_TYPE(HOUSE_MODE_ID, "houseModeId")
    EZLOPI_VALUE_TYPE(SCENEID, "sceneId")
    EZLOPI_VALUE_TYPE(DEVICE_GROUP, "device_group")
    EZLOPI_VALUE_TYPE(ITEM_GROUP, "item_group")

    //
    EZLOPI_VALUE_TYPE(MAX, NULL)

#ifdef __cplusplus
}
#endif

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

/*******************************************************************************
 *                          End of File
 *******************************************************************************/