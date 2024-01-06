#ifndef __EZLOPI_SCALES_STR_H__
#define __EZLOPI_SCALES_STR_H__

/// @brief https://api.ezlo.com/devices/item_value_types/index.html#scalable-types

/// @brief Temperature
extern const char *scales_celsius;
extern const char *scales_fahrenheit;

/// @brief humidity
extern const char *scales_percent;
extern const char *scales_gram_per_cubic_meter;

/// @brief kilo_volt_ampere_hour
extern const char *scales_kilo_volt_ampere_hour;

/// @brief reactive power
extern const char *scales_kilo_volt_ampere_reactive;

/// @brief amount_of_useful_energy
extern const char *scales_amount_of_useful_energy;

/// @brief length
extern const char *scales_centi_meter;
extern const char *scales_meter;
extern const char *scales_feet;

/// @brief LUX
extern const char *scales_lux;

/// @brief pressure
extern const char *scales_kilo_pascal;

/// @brief electrical_resistance
extern const char *scales_ohm_meter;

/// @brief substance_amount
extern const char *scales_micro_gram_per_cubic_meter;
extern const char *scales_mole_per_cubic_meter;
extern const char *scales_parts_per_million;
extern const char *scales_milli_gram_per_liter;

/// @brief angle
extern const char *scales_north_pole_degress;

/// @brief volume_flow
extern const char *scales_cubic_meter_per_hour;
extern const char *scales_cubic_feet_per_minute;
extern const char *scales_liter_per_hour;

/// @brief force
extern const char *scales_newton;

/// @brief electric_potential
const static char *scales_milli_volt = "milli_volt";
const static char *scales_volt = "volt";

/// @brief acceleration
const static char *scales_meter_per_square_second = "meter_per_square_second";

/// @brief electric_current
const static char *scales_ampere = "ampere";
const static char *scales_milli_ampere = "milli_ampere";

/// @brief mass
const static char *scales_kilo_gram = "kilo_gram";
const static char *scales_pounds = "pounds";

/// @brief irradiance
const static char *scales_watt_per_square_meter = "watt_per_square_meter";

/// @brief frequency
const static char *scales_revolutions_per_minute = "revolutions_per_minute";
const static char *scales_hertz = "hertz";
const static char *scales_kilo_hertz = "kilo_hertz";
const static char *scales_breaths_per_minute = "breaths_per_minute";
const static char *scales_beats_per_minute = "beats_per_minute";

/// @brief magnetic field strength
const static char *scales_guass = "guass";
const static char *scales_tesla = "tesla";
const static char *scales_weber_per_square_meter = "weber_per_square_meter";

#endif // __EZLOPI_SCALES_STR_H__
