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
 * @file    main.c
 * @brief   perform some function on data
 * @author  John Doe
 * @version 0.1
 * @date    1st January 2024
 */

#ifndef __EZLOPI_ITEM_NAME_STR_H__
#define __EZLOPI_ITEM_NAME_STR_H__

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
    /// @brief https://api.ezlo.com/devices/items/index.html
    extern const char *ezlopi_item_name_undefined;
    extern const char *ezlopi_item_name_acceleration_x_axis;
    extern const char *ezlopi_item_name_acceleration_y_axis;
    extern const char *ezlopi_item_name_acceleration_z_axis;
    // extern const  char *ezlopi_item_name_appliance_status;
    // extern const  char *ezlopi_item_name_lock_operation;
    // extern const  char *ezlopi_item_name_user_code_operation;
    // extern const  char *ezlopi_item_name_dw_handle_state;
    extern const char *ezlopi_item_name_dw_state;
    // extern const  char *ezlopi_item_name_latch_state;
    // extern const  char *ezlopi_item_name_keypad_state;
    // extern const  char *ezlopi_item_name_emergency_state;
    // extern const  char *ezlopi_item_name_barrier_initialization;
    // extern const  char *ezlopi_item_name_barrier_unattended_operation;
    // extern const  char *ezlopi_item_name_barrier_vacation_mode;
    // extern const  char *ezlopi_item_name_barrier_safety_beam_obastacle;
    // extern const  char *ezlopi_item_name_barrier_problem_sensors;
    // extern const  char *ezlopi_item_name_barrier_short_circuit;
    // extern const  char *ezlopi_item_name_barrier_fail_events;
    // extern const  char *ezlopi_item_name_button_state;
    // extern const  char *ezlopi_item_name_test_state;
    // extern const  char *ezlopi_item_name_co_alarm;
    // extern const  char *ezlopi_item_name_maintenance_state;
    extern const char *ezlopi_item_name_sounding_mode;
    // extern const  char *ezlopi_item_name_sound_list;
    extern const  char *ezlopi_item_name_sound_volume;
    // extern const  char *ezlopi_item_name_sound_select;
    // extern const  char *ezlopi_item_name_sound_playback;
    // extern const  char *ezlopi_item_name_periodic_inspection_state;
    // extern const  char *ezlopi_item_name_co2_alarm;
    extern const char *ezlopi_item_name_gas_alarm;
    extern const char *ezlopi_item_name_heat_alarm;
    // extern const  char *ezlopi_item_name_siren_alarm;
    extern const char *ezlopi_item_name_light_alarm;
    // extern const  char *ezlopi_item_name_light_color_transition;
    extern const char *ezlopi_item_name_temperature_changes;
    // extern const  char *ezlopi_item_name_intrusion_alarm;
    // extern const  char *ezlopi_item_name_tampering_cover_alarm;
    // extern const  char *ezlopi_item_name_glass_breakage_alarm;
    // extern const  char *ezlopi_item_name_tampering_move_alarm;
    // extern const  char *ezlopi_item_name_tampering_impact_alarm;
    // extern const  char *ezlopi_item_name_tampering_invalid_code_alarm;
    // extern const  char *ezlopi_item_name_smoke_alarm;
    // extern const  char *ezlopi_item_name_dust_in_device;
    extern const char *ezlopi_item_name_water_leak_alarm;
    extern const char *ezlopi_item_name_water_filter_replacement_alarm;
    extern const char *ezlopi_item_name_water_flow_alarm;
    // extern const  char *ezlopi_item_name_water_pressure_alarm;
    // extern const  char *ezlopi_item_name_water_temperature_alarm;
    extern const char *ezlopi_item_name_water_level_alarm;
    // extern const  char *ezlopi_item_name_water_pump_state;
    // extern const  char *ezlopi_item_name_water_valve_state;
    // extern const  char *ezlopi_item_name_master_water_valve_state;
    // extern const  char *ezlopi_item_name_water_valve_short_circuit;
    // extern const  char *ezlopi_item_name_master_water_valve_short_circuit;
    // extern const  char *ezlopi_item_name_water_valve_current_alarm;
    // extern const  char *ezlopi_item_name_master_water_valve_current_alarm;
    // extern const  char *ezlopi_item_name_rain_alarm;
    // extern const  char *ezlopi_item_name_moisture_alarm;
    // extern const  char *ezlopi_item_name_freeze_alarm;
    // extern const  char *ezlopi_item_name_power_state;
    // extern const  char *ezlopi_item_name_ac_state;
    // extern const  char *ezlopi_item_name_power_surge_state;
    // extern const  char *ezlopi_item_name_voltage_drop_drift_state;
    // extern const  char *ezlopi_item_name_over_current_state;
    // extern const  char *ezlopi_item_name_over_voltage_state;
    // extern const  char *ezlopi_item_name_over_load_state;
    // extern const  char *ezlopi_item_name_load_error_state;
    // extern const  char *ezlopi_item_name_battery_maintenance_state;
    // extern const  char *ezlopi_item_name_battery_charging_state;
    // extern const  char *ezlopi_item_name_battery_backup;
    extern const char *ezlopi_item_name_angle_position;
    extern const char *ezlopi_item_name_atmospheric_pressure;
    // extern const  char *ezlopi_item_name_barometric_pressure;
    // extern const  char *ezlopi_item_name_barrier;
    // extern const  char *ezlopi_item_name_barrier_state;
    // extern const  char *ezlopi_item_name_hw_state;
    // extern const  char *ezlopi_item_name_sw_state;
    // extern const  char *ezlopi_item_name_emergency_shutoff;
    // extern const  char *ezlopi_item_name_digital_input_state;
    // extern const  char *ezlopi_item_name_clock_state;
    // extern const  char *ezlopi_item_name_remaining_time;
    extern const char *ezlopi_item_name_basic;
    // extern const  char *ezlopi_item_name_battery;
    extern const char *ezlopi_item_name_co2_level;
    // extern const  char *ezlopi_item_name_co_level;
    // extern const  char *ezlopi_item_name_dew_point;
    extern const char *ezlopi_item_name_dimmer;
    extern const char *ezlopi_item_name_dimmer_down;
    extern const char *ezlopi_item_name_dimmer_stop;
    extern const char *ezlopi_item_name_dimmer_up;
    // extern const  char *ezlopi_item_name_direction;
    extern const char *ezlopi_item_name_distance;
    // extern const  char *ezlopi_item_name_door_lock;
    // extern const  char *ezlopi_item_name_electric_meter_amper;
    // extern const  char *ezlopi_item_name_electric_meter_kvah;
    // extern const  char *ezlopi_item_name_electric_meter_kvar;
    // extern const  char *ezlopi_item_name_electric_meter_kvarh;
    // extern const  char *ezlopi_item_name_electric_meter_kwh;
    // extern const  char *ezlopi_item_name_electric_meter_power_factor;
    // extern const  char *ezlopi_item_name_electric_meter_pulse;
    // extern const  char *ezlopi_item_name_electric_meter_volt;
    // extern const  char *ezlopi_item_name_electric_meter_watt;
    // extern const  char *ezlopi_item_name_electric_resist;
    // extern const  char *ezlopi_item_name_water_meter_volume;
    // extern const  char *ezlopi_item_name_water_meter_pulse;
    // extern const  char *ezlopi_item_name_gas_meter_volume;
    // extern const  char *ezlopi_item_name_gas_meter_pulse;
    // extern const  char *ezlopi_item_name_heating_meter_energy;
    // extern const  char *ezlopi_item_name_cooling_meter_energy;
    extern const char *ezlopi_item_name_humidity;
    // extern const  char *ezlopi_item_name_loudness;
    extern const char *ezlopi_item_name_lux;
    // extern const  char *ezlopi_item_name_meter_reset;
    extern const char *ezlopi_item_name_moisture;
    extern const char *ezlopi_item_name_motion;
    // extern const  char *ezlopi_item_name_power;
    // extern const  char *ezlopi_item_name_pressure;
    extern const char *ezlopi_item_name_rgbcolor;
    // extern const  char *ezlopi_item_name_rgbcolor_down;
    // extern const  char *ezlopi_item_name_rgbcolor_stop;
    // extern const  char *ezlopi_item_name_rgbcolor_up;
    // extern const  char *ezlopi_item_name_rotation;
    // extern const  char *ezlopi_item_name_security_threat;
    // extern const  char *ezlopi_item_name_seismic_intensity;
    // extern const  char *ezlopi_item_name_seismic_magnitude;
    // extern const  char *ezlopi_item_name_rain_rate;
    // extern const  char *ezlopi_item_name_shutter_command;
    // extern const  char *ezlopi_item_name_shutter_state;
    // extern const  char *ezlopi_item_name_soil_temperature;
    extern const char *ezlopi_item_name_solar_radiation;
    extern const char *ezlopi_item_name_switch;
    extern const char *ezlopi_item_name_temp;
    // extern const  char *ezlopi_item_name_thermostat_fan_mode;
    // extern const  char *ezlopi_item_name_thermostat_fan_state;
    // extern const  char *ezlopi_item_name_thermostat_mode;
    // extern const  char *ezlopi_item_name_thermostat_energy_saving_mode;
    // extern const  char *ezlopi_item_name_thermostat_operating_state;
    // extern const  char *ezlopi_item_name_thermostat_setpoint;
    // extern const  char *ezlopi_item_name_thermostat_setpoint_heating;
    // extern const  char *ezlopi_item_name_thermostat_setpoint_cooling;
    // extern const  char *ezlopi_item_name_tide_level;
    // extern const  char *ezlopi_item_name_ultraviolet;
    // extern const  char *ezlopi_item_name_user_codes_keypad_mode;
    // extern const  char *ezlopi_item_name_master_code;
    // extern const  char *ezlopi_item_name_master_code_state;
    // extern const  char *ezlopi_item_name_user_codes;
    // extern const  char *ezlopi_item_name_user_lock_operation;
    // extern const  char *ezlopi_item_name_user_code_action;
    // extern const  char *ezlopi_item_name_user_codes_scan_progress;
    // extern const  char *ezlopi_item_name_velocity;
    extern const char *ezlopi_item_name_voltage;
    extern const char *ezlopi_item_name_current;
    extern const char *ezlopi_item_name_weight;
    // extern const  char *ezlopi_item_name_air_flow;
    // extern const  char *ezlopi_item_name_tank_capacity;
    // extern const  char *ezlopi_item_name_water_temperature;
    extern const char *ezlopi_item_name_electrical_resistivity;
    // extern const  char *ezlopi_item_name_electrical_conductivity;
    // extern const  char *ezlopi_item_name_frequency;
    // extern const  char *ezlopi_item_name_time_period;
    // extern const  char *ezlopi_item_name_target_temperature;
    // extern const  char *ezlopi_item_name_blood_pressure;
    extern const char *ezlopi_item_name_water_flow;
    // extern const  char *ezlopi_item_name_water_pressure;
    // extern const  char *ezlopi_item_name_boiler_water_temperature;
    // extern const  char *ezlopi_item_name_domestic_hot_water_temperature;
    // // extern const  char *ezlopi_item_name_outside_temperature;
    // // extern const  char *ezlopi_item_name_exhaust_temperature;
    // extern const  char *ezlopi_item_name_heart_rate_lf_hf_ratio;
    // extern const  char *ezlopi_item_name_water_oxidation_reduction_potential;
    // extern const  char *ezlopi_item_name_water_chlorine_level;
    // extern const  char *ezlopi_item_name_exhaust_temperature;
    // extern const  char *ezlopi_item_name_outside_temperature;
    // // extern const  char *ezlopi_item_name_water_chlorine_level;
    // extern const  char *ezlopi_item_name_water_acidity;
    extern const  char *ezlopi_item_name_particulate_matter_10;
    extern const  char *ezlopi_item_name_particulate_matter_2_dot_5;
    // extern const  char *ezlopi_item_name_program_status;
    // extern const  char *ezlopi_item_name_program_failures;
    // extern const  char *ezlopi_item_name_position;
    // extern const  char *ezlopi_item_name_sleep_apnea;
    // extern const  char *ezlopi_item_name_sleep_stage;
    // extern const  char *ezlopi_item_name_voc_level_status;
    // extern const  char *ezlopi_item_name_rf_signal_strength;
    // extern const  char *ezlopi_item_name_basal_metabolic_rate;
    // extern const  char *ezlopi_item_name_body_mass_index;
    // extern const  char *ezlopi_item_name_body_mass;
    // extern const  char *ezlopi_item_name_total_body_water;
    // extern const  char *ezlopi_item_name_fat_mass;
    // extern const  char *ezlopi_item_name_muscle_mass;
    // extern const  char *ezlopi_item_name_relative_modulation_level;
    // extern const  char *ezlopi_item_name_respiratory_rate;
    extern const char *ezlopi_item_name_smoke_density;
    // extern const  char *ezlopi_item_name_heart_rate;
    // extern const  char *ezlopi_item_name_soil_salinity;
    // extern const  char *ezlopi_item_name_soil_reactivity;
    // extern const  char *ezlopi_item_name_soil_humidity;
    extern const char *ezlopi_item_name_volatile_organic_compound_level;
    // extern const  char *ezlopi_item_name_methane_density;
    // extern const  char *ezlopi_item_name_radon_concentration;
    // extern const  char *ezlopi_item_name_formaldehyde_level;
    // extern const  char *ezlopi_item_name_weekly_user_code_intervals;
    // extern const  char *ezlopi_item_name_daily_user_code_intervals;
    // extern const  char *ezlopi_item_name_goto_favorite;
    // extern const  char *ezlopi_item_name_zones_status;
    extern const char *ezlopi_item_name_activity;
    // extern const  char *ezlopi_item_name_bed_activity;
    // extern const  char *ezlopi_item_name_ammonia;
    extern const char *ezlopi_item_name_applied_force_on_sensor;
    // extern const  char *ezlopi_item_name_bone_mass;
    // extern const  char *ezlopi_item_name_condenser_coil_temperature;
    // extern const  char *ezlopi_item_name_defrost_temperature;
    // extern const  char *ezlopi_item_name_discharge_line_temperature;
    // extern const  char *ezlopi_item_name_discharge_pressure;
    // extern const  char *ezlopi_item_name_evaporator_coil_temperature;
    // extern const  char *ezlopi_item_name_lead;
    // extern const  char *ezlopi_item_name_liquid_line_temperature;
    extern const  char *ezlopi_item_name_motion_direction;
    // extern const  char *ezlopi_item_name_nitrogen_dioxide;
    // extern const  char *ezlopi_item_name_ozone;
    extern const  char *ezlopi_item_name_particulate_matter_1;
    // extern const  char *ezlopi_item_name_return_air_temperature;
    // extern const  char *ezlopi_item_name_suction_pressure;
    // extern const  char *ezlopi_item_name_sulfur_dioxide;
    // extern const  char *ezlopi_item_name_supply_air_temperature;
    extern const char *ezlopi_item_name_send_ir_code;
    extern const char *ezlopi_item_name_learn_ir_code;
    extern const char *ezlopi_item_name_soil_humidity;

    extern const char *ezlopi_item_name_magnetic_strength_x_axis;
    extern const char *ezlopi_item_name_magnetic_strength_y_axis;
    extern const char *ezlopi_item_name_magnetic_strength_z_axis;

    extern const char *ezlopi_item_name_gyroscope_x_axis;
    extern const char *ezlopi_item_name_gyroscope_y_axis;
    extern const char *ezlopi_item_name_gyroscope_z_axis;

    extern const char *ezlopi_item_name_learn_fingerprint;
    extern const char *ezlopi_item_name_fingerprint_action;
    extern const char *ezlopi_item_name_fingerprint_ids;

    extern const char *ezlopi_item_name_particles_0_dot_3_um;
    extern const char *ezlopi_item_name_particles_0_dot_5_um;
    extern const char *ezlopi_item_name_particles_1_um;
    extern const char *ezlopi_item_name_particles_2_dot_5_um;
    extern const char *ezlopi_item_name_particles_5_um;
    extern const char *ezlopi_item_name_particles_10_um;

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // __EZLOPI_ITEM_NAME_STR_H__

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
