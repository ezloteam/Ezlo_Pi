

/**
 * @file    ezlopi_cloud_item_name_str.c
 * @brief
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 * @version
 * @date
 */
/* ===========================================================================
** Copyright (C) 2022 Ezlo Innovation Inc
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

/// @brief https://api.ezlo.com/devices/items/index.html
const char *ezlopi_item_name_undefined = "";
const char *ezlopi_item_name_acceleration_x_axis = "acceleration_x_axis";
const char *ezlopi_item_name_acceleration_y_axis = "acceleration_y_axis";
const char *ezlopi_item_name_acceleration_z_axis = "acceleration_z_axis";
// const  char *ezlopi_item_name_appliance_status = "appliance_status";
// const  char *ezlopi_item_name_lock_operation = "lock_operation";
// const  char *ezlopi_item_name_user_code_operation = "user_code_operation";
// const  char *ezlopi_item_name_dw_handle_state = "dw_handle_state";
const char *ezlopi_item_name_dw_state = "dw_state";
// const  char *ezlopi_item_name_latch_state = "latch_state";
// const  char *ezlopi_item_name_keypad_state = "keypad_state";
// const  char *ezlopi_item_name_emergency_state = "emergency_state";
// const  char *ezlopi_item_name_barrier_initialization = "barrier_initialization";
// const  char *ezlopi_item_name_barrier_unattended_operation = "barrier_unattended_operation";
// const  char *ezlopi_item_name_barrier_vacation_mode = "barrier_vacation_mode";
// const  char *ezlopi_item_name_barrier_safety_beam_obastacle = "barrier_safety_beam_obstacle";
// const  char *ezlopi_item_name_barrier_problem_sensors = "barrier_problem_sensors";
// const  char *ezlopi_item_name_barrier_short_circuit = "barrier_short_circuit";
// const  char *ezlopi_item_name_barrier_fail_events = "barrier_fail_events";
const char *ezlopi_item_name_button_state = "button_state";
// const  char *ezlopi_item_name_test_state = "test_state";
// const  char *ezlopi_item_name_co_alarm = "co_alarm";
// const  char *ezlopi_item_name_maintenance_state = "maintenance_state";
const char *ezlopi_item_name_sounding_mode = "sounding_mode";
// const  char *ezlopi_item_name_sound_list = "sound_list";
const char *ezlopi_item_name_sound_volume = "sound_volume";
// const  char *ezlopi_item_name_sound_select = "sound_select";
// const  char *ezlopi_item_name_sound_playback = "sound_playback";
// const  char *ezlopi_item_name_periodic_inspection_state = "periodic_inspection_state";
// const  char *ezlopi_item_name_co2_alarm = "co2_alarm";
const char *ezlopi_item_name_gas_alarm = "gas_alarm";
const char *ezlopi_item_name_heat_alarm = "heat_alarm";
// const  char *ezlopi_item_name_siren_alarm = "siren_alarm";
const char *ezlopi_item_name_light_alarm = "light_alarm";
// const  char *ezlopi_item_name_light_color_transition = "light_color_transition";
const char *ezlopi_item_name_temperature_changes = "temperature_changes";
// const  char *ezlopi_item_name_intrusion_alarm = "intrusion_alarm";
// const  char *ezlopi_item_name_tampering_cover_alarm = "tampering_cover_alarm";
// const  char *ezlopi_item_name_glass_breakage_alarm = "glass_breakage_alarm";
// const  char *ezlopi_item_name_tampering_move_alarm = "tampering_move_alarm";
// const  char *ezlopi_item_name_tampering_impact_alarm = "tampering_impact_alarm";
// const  char *ezlopi_item_name_tampering_invalid_code_alarm = "tampering_invalid_code_alarm";
// const  char *ezlopi_item_name_smoke_alarm = "smoke_alarm";
// const  char *ezlopi_item_name_dust_in_device = "dust_in_device";
const char *ezlopi_item_name_water_leak_alarm = "water_leak_alarm";
const char *ezlopi_item_name_water_filter_replacement_alarm = "water_filter_replacement_alarm";
const char *ezlopi_item_name_water_flow_alarm = "water_flow_alarm";
// const  char *ezlopi_item_name_water_pressure_alarm = "water_pressure_alarm";
// const  char *ezlopi_item_name_water_temperature_alarm = "water_temperature_alarm";
const char *ezlopi_item_name_water_level_alarm = "water_level_alarm";
// const  char *ezlopi_item_name_water_pump_state = "water_pump_state";
// const  char *ezlopi_item_name_water_valve_state = "water_valve_state";
// const  char *ezlopi_item_name_master_water_valve_state = "master_water_valve_state";
// const  char *ezlopi_item_name_water_valve_short_circuit = "water_valve_short_circuit";
// const  char *ezlopi_item_name_master_water_valve_short_circuit = "master_water_valve_short_circuit";
// const  char *ezlopi_item_name_water_valve_current_alarm = "water_valve_current_alarm";
// const  char *ezlopi_item_name_master_water_valve_current_alarm = "master_water_valve_current_alarm";
// const  char *ezlopi_item_name_rain_alarm = "rain_alarm";
// const  char *ezlopi_item_name_moisture_alarm = "moisture_alarm";
// const  char *ezlopi_item_name_freeze_alarm = "freeze_alarm";
// const  char *ezlopi_item_name_power_state = "power_state";
// const  char *ezlopi_item_name_ac_state = "ac_state";
// const  char *ezlopi_item_name_power_surge_state = "power_surge_state";
// const  char *ezlopi_item_name_voltage_drop_drift_state = "voltage_drop_drift_state";
// const  char *ezlopi_item_name_over_current_state = "over_current_state";
// const  char *ezlopi_item_name_over_voltage_state = "over_voltage_state";
// const  char *ezlopi_item_name_over_load_state = "over_load_state";
// const  char *ezlopi_item_name_load_error_state = "load_error_state";
// const  char *ezlopi_item_name_battery_maintenance_state = "battery_maintenance_state";
// const  char *ezlopi_item_name_battery_charging_state = "battery_charging_state";
// const  char *ezlopi_item_name_battery_backup = "battery_backup";
const char *ezlopi_item_name_angle_position = "angle_position";
const char *ezlopi_item_name_atmospheric_pressure = "atmospheric_pressure";
// const  char *ezlopi_item_name_barometric_pressure = "barometric_pressure";
// const  char *ezlopi_item_name_barrier = "barrier";
// const  char *ezlopi_item_name_barrier_state = "barrier_state";
// const  char *ezlopi_item_name_hw_state = "hw_state";
// const  char *ezlopi_item_name_sw_state = "sw_state";
// const  char *ezlopi_item_name_emergency_shutoff = "emergency_shutoff";
// const  char *ezlopi_item_name_digital_input_state = "digital_input_state";
// const  char *ezlopi_item_name_clock_state = "clock_state";
// const  char *ezlopi_item_name_remaining_time = "remaining_time";
const char *ezlopi_item_name_basic = "basic";
// const  char *ezlopi_item_name_battery = "battery";
const char *ezlopi_item_name_co2_level = "co2_level";
// const  char *ezlopi_item_name_co_level = "co_level";
// const  char *ezlopi_item_name_dew_point = "dew_point";
const char *ezlopi_item_name_dimmer = "dimmer";
const char *ezlopi_item_name_dimmer_down = "dimmer_down";
const char *ezlopi_item_name_dimmer_stop = "dimmer_stop";
const char *ezlopi_item_name_dimmer_up = "dimmer_up";
// const  char *ezlopi_item_name_direction = "direction";
const char *ezlopi_item_name_distance = "distance";
// const  char *ezlopi_item_name_door_lock = "door_lock";
// const  char *ezlopi_item_name_electric_meter_amper = "electric_meter_amper";
// const  char *ezlopi_item_name_electric_meter_kvah = "electric_meter_kvah";
// const  char *ezlopi_item_name_electric_meter_kvar = "electric_meter_kvar";
// const  char *ezlopi_item_name_electric_meter_kvarh = "electric_meter_kvarh";
// const  char *ezlopi_item_name_electric_meter_kwh = "electric_meter_kwh";
// const  char *ezlopi_item_name_electric_meter_power_factor = "electric_meter_power_factor";
// const  char *ezlopi_item_name_electric_meter_pulse = "electric_meter_pulse";
// const  char *ezlopi_item_name_electric_meter_volt = "electric_meter_volt";
// const  char *ezlopi_item_name_electric_meter_watt = "electric_meter_watt";
// const  char *ezlopi_item_name_electric_resist = "electric_resist";
// const  char *ezlopi_item_name_water_meter_volume = "water_meter_volume";
// const  char *ezlopi_item_name_water_meter_pulse = "water_meter_pulse";
// const  char *ezlopi_item_name_gas_meter_volume = "gas_meter_volume";
// const  char *ezlopi_item_name_gas_meter_pulse = "gas_meter_pulse";
// const  char *ezlopi_item_name_heating_meter_energy = "heating_meter_energy";
// const  char *ezlopi_item_name_cooling_meter_energy = "cooling_meter_energy";
const char *ezlopi_item_name_humidity = "humidity";
// const  char *ezlopi_item_name_loudness = "loudness";
const char *ezlopi_item_name_lux = "lux";
// const  char *ezlopi_item_name_meter_reset = "meter_reset";
const char *ezlopi_item_name_moisture = "moisture";
const char *ezlopi_item_name_motion = "motion";
// const  char *ezlopi_item_name_power = "power";
// const  char *ezlopi_item_name_pressure = "pressure";
const char *ezlopi_item_name_rgbcolor = "rgbcolor";
// const  char *ezlopi_item_name_rgbcolor_down = "rgbcolor_down";
// const  char *ezlopi_item_name_rgbcolor_stop = "rgbcolor_stop";
// const  char *ezlopi_item_name_rgbcolor_up = "rgbcolor_up";
// const  char *ezlopi_item_name_rotation = "rotation";
// const  char *ezlopi_item_name_security_threat = "security_threat";
// const  char *ezlopi_item_name_seismic_intensity = "seismic_intensity";
// const  char *ezlopi_item_name_seismic_magnitude = "seismic_magnitude";
// const  char *ezlopi_item_name_rain_rate = "rain_rate";
// const  char *ezlopi_item_name_shutter_command = "shutter_command";
// const  char *ezlopi_item_name_shutter_state = "shutter_state";
// const  char *ezlopi_item_name_soil_temperature = "soil_temperature";
const char *ezlopi_item_name_solar_radiation = "solar_radiation";
const char *ezlopi_item_name_switch = "switch";
const char *ezlopi_item_name_temp = "temp";
// const  char *ezlopi_item_name_thermostat_fan_mode = "thermostat_fan_mode";
// const  char *ezlopi_item_name_thermostat_fan_state = "thermostat_fan_state";
// const  char *ezlopi_item_name_thermostat_mode = "thermostat_mode";
// const  char *ezlopi_item_name_thermostat_energy_saving_mode = "thermostat_energy_saving_mode";
// const  char *ezlopi_item_name_thermostat_operating_state = "thermostat_operating_state";
// const  char *ezlopi_item_name_thermostat_setpoint = "thermostat_setpoint";
// const  char *ezlopi_item_name_thermostat_setpoint_heating = "thermostat_setpoint_heating";
// const  char *ezlopi_item_name_thermostat_setpoint_cooling = "thermostat_setpoint_cooling";
// const  char *ezlopi_item_name_tide_level = "tide_level";
// const  char *ezlopi_item_name_ultraviolet = "ultraviolet";
// const  char *ezlopi_item_name_user_codes_keypad_mode = "user_codes_keypad_mode";
// const  char *ezlopi_item_name_master_code = "master_code";
// const  char *ezlopi_item_name_master_code_state = "master_code_state";
// const  char *ezlopi_item_name_user_codes = "user_codes";
// const  char *ezlopi_item_name_user_lock_operation = "user_lock_operation";
// const  char *ezlopi_item_name_user_code_action = "user_code_action";
// const  char *ezlopi_item_name_user_codes_scan_progress = "user_codes_scan_progress";
// const  char *ezlopi_item_name_velocity = "velocity";
const char *ezlopi_item_name_voltage = "voltage";
const char *ezlopi_item_name_current = "current";
const char *ezlopi_item_name_weight = "weight";
// const  char *ezlopi_item_name_air_flow = "air_flow";
// const  char *ezlopi_item_name_tank_capacity = "tank_capacity";
// const  char *ezlopi_item_name_water_temperature = "water_temperature";
const char *ezlopi_item_name_electrical_resistivity = "electrical_resistivity";
// const  char *ezlopi_item_name_electrical_conductivity = "electrical_conductivity";
// const  char *ezlopi_item_name_frequency = "frequency";
// const  char *ezlopi_item_name_time_period = "time_period";
// const  char *ezlopi_item_name_target_temperature = "target_temperature";
// const  char *ezlopi_item_name_blood_pressure = "blood_pressure";
const char *ezlopi_item_name_water_flow = "water_flow";
// const  char *ezlopi_item_name_water_pressure = "water_pressure";
// const  char *ezlopi_item_name_boiler_water_temperature = "boiler_water_temperature";
// const  char *ezlopi_item_name_domestic_hot_water_temperature = "domestic_hot_water_temperature";
// // const  char *ezlopi_item_name_outside_temperature = "outside_temperature";
// // const  char *ezlopi_item_name_exhaust_temperature = "exhaust_temperature";
// const  char *ezlopi_item_name_heart_rate_lf_hf_ratio = "heart_rate_lf_hf_ratio";
// const  char *ezlopi_item_name_water_oxidation_reduction_potential = "water_oxidation_reduction_potential";
// const  char *ezlopi_item_name_water_chlorine_level = "water_chlorine_level";
// const  char *ezlopi_item_name_exhaust_temperature = "exhaust_temperature";
// const  char *ezlopi_item_name_outside_temperature = "outside_temperature";
// // const  char *ezlopi_item_name_water_chlorine_level = "water_chlorine_level";
// const  char *ezlopi_item_name_water_acidity = "water_acidity";
const char *ezlopi_item_name_particulate_matter_10 = "particulate_matter_10";
const char *ezlopi_item_name_particulate_matter_2_dot_5 = "particulate_matter_2_dot_5";
// const  char *ezlopi_item_name_program_status = "program_status";
// const  char *ezlopi_item_name_program_failures = "program_failures";
// const  char *ezlopi_item_name_position = "position";
// const  char *ezlopi_item_name_sleep_apnea = "sleep_apnea";
// const  char *ezlopi_item_name_sleep_stage = "sleep_stage";
// const  char *ezlopi_item_name_voc_level_status = "voc_level_status";
// const  char *ezlopi_item_name_rf_signal_strength = "rf_signal_strength";
// const  char *ezlopi_item_name_basal_metabolic_rate = "basal_metabolic_rate";
// const  char *ezlopi_item_name_body_mass_index = "body_mass_index";
// const  char *ezlopi_item_name_body_mass = "body_mass";
// const  char *ezlopi_item_name_total_body_water = "total_body_water";
// const  char *ezlopi_item_name_fat_mass = "fat_mass";
// const  char *ezlopi_item_name_muscle_mass = "muscle_mass";
// const  char *ezlopi_item_name_relative_modulation_level = "relative_modulation_level";
// const  char *ezlopi_item_name_respiratory_rate = "respiratory_rate";
const char *ezlopi_item_name_smoke_density = "smoke_density";
// const  char *ezlopi_item_name_heart_rate = "heart_rate";
// const  char *ezlopi_item_name_soil_salinity = "soil_salinity";
// const  char *ezlopi_item_name_soil_reactivity = "soil_reactivity";
// const  char *ezlopi_item_name_soil_humidity = "soil_humidity";
const char *ezlopi_item_name_volatile_organic_compound_level = "volatile_organic_compound_level";
// const  char *ezlopi_item_name_methane_density = "methane_density";
// const  char *ezlopi_item_name_radon_concentration = "radon_concentration";
// const  char *ezlopi_item_name_formaldehyde_level = "formaldehyde_level";
// const  char *ezlopi_item_name_weekly_user_code_intervals = "weekly_user_code_intervals";
// const  char *ezlopi_item_name_daily_user_code_intervals = "daily_user_code_intervals";
// const  char *ezlopi_item_name_goto_favorite = "goto_favorite";
// const  char *ezlopi_item_name_zones_status = "zones_status";
const char *ezlopi_item_name_activity = "activity";
// const  char *ezlopi_item_name_bed_activity = "bed_activity";
// const  char *ezlopi_item_name_ammonia = "ammonia";
const char *ezlopi_item_name_applied_force_on_sensor = "applied_force_on_sensor";
// const  char *ezlopi_item_name_bone_mass = "bone_mass";
// const  char *ezlopi_item_name_condenser_coil_temperature = "condenser_coil_temperature";
// const  char *ezlopi_item_name_defrost_temperature = "defrost_temperature";
// const  char *ezlopi_item_name_discharge_line_temperature = "discharge_line_temperature";
// const  char *ezlopi_item_name_discharge_pressure = "discharge_pressure";
// const  char *ezlopi_item_name_evaporator_coil_temperature = "evaporator_coil_temperature";
// const  char *ezlopi_item_name_lead = "lead";
// const  char *ezlopi_item_name_liquid_line_temperature = "liquid_line_temperature";
const char *ezlopi_item_name_motion_direction = "motion_direction";
// const  char *ezlopi_item_name_nitrogen_dioxide = "nitrogen_dioxide";
// const  char *ezlopi_item_name_ozone = "ozone";
const char *ezlopi_item_name_particulate_matter_1 = "particulate_matter_1";
// const  char *ezlopi_item_name_return_air_temperature = "return_air_temperature";
// const  char *ezlopi_item_name_suction_pressure = "suction_pressure";
// const  char *ezlopi_item_name_sulfur_dioxide = "sulfur_dioxide";
// const  char *ezlopi_item_name_supply_air_temperature = "supply_air_temperature";
const char *ezlopi_item_name_send_ir_code = "send_ir_code";
const char *ezlopi_item_name_learn_ir_code = "learn_ir_code";
const char *ezlopi_item_name_soil_humidity = "soil_humidity";

const char *ezlopi_item_name_magnetic_strength_x_axis = "magnetic_strength_x_axis";
const char *ezlopi_item_name_magnetic_strength_y_axis = "magnetic_strength_y_axis";
const char *ezlopi_item_name_magnetic_strength_z_axis = "magnetic_strength_z_axis";

const char *ezlopi_item_name_gyroscope_x_axis = "gyroscope_x_axis";
const char *ezlopi_item_name_gyroscope_y_axis = "gyroscope_y_axis";
const char *ezlopi_item_name_gyroscope_z_axis = "gyroscope_z_axis";

const char *ezlopi_item_name_learn_fingerprint = "learn_fingerprint";
const char *ezlopi_item_name_fingerprint_action = "fingerprint_action";
const char *ezlopi_item_name_fingerprint_ids = "fingerprint_ids";

const char *ezlopi_item_name_particles_0_dot_3_um = "particles_0_dot_3_um";
const char *ezlopi_item_name_particles_0_dot_5_um = "particles_0_dot_5_um";
const char *ezlopi_item_name_particles_1_um = "particles_1_um";
const char *ezlopi_item_name_particles_2_dot_5_um = "particles_2_dot_5_um";
const char *ezlopi_item_name_particles_5_um = "particles_5_um";
const char *ezlopi_item_name_particles_10_um = "particles_10_um";

/*******************************************************************************
 *                          End of File
 *******************************************************************************/