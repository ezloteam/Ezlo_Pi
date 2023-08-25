


#ifndef _030_SENS_DS18B20_SENSOR_H_
#define _030_SENS_DS18B20_SENSOR_H_


#include "ezlopi_actions.h"
#include "ezlopi_devices.h"
#include "stdbool.h"


#define DS18B20_TEMPERATURE_LSB     0
#define DS18B20_TEMPERATURE_HSB     1
#define DS18B20_TH_REGISTER         2
#define DS18B20_TL_REGISTER         3
#define DS18B20_CONFIG_REGISTER     4
#define DS18B20_CRC_REGISTER        8


#define DS18B20_TEMPERATURE_9_BIT_RESOLUTION            0x1F
#define DS18B20_TEMPERATURE_10_BIT_RESOLUTION           0x3F
#define DS18B20_TEMPERATURE_11_BIT_RESOLUTION           0x5F
#define DS18B20_TEMPERATURE_12_BIT_RESOLUTION           0x7F


#define DS18B20_FAMILY_CODE                             0x28


// ROM commands for DS18B20
#define DS18B20_ROM_COMMAND_SEARCH_ROM                  0xF0
#define DS18B20_ROM_COMMAND_READ_ROM                    0x33
#define DS18B20_ROM_COMMAND_MATCH_ROM                   0x55
#define DS18B20_ROM_COMMAND_SKIP_ROM                    0xCC
#define DS18B20_ROM_COMMAND_ALARM_SEARCH                0xEC


#define DS18B20_FUNCTION_COMMAND_CONVERT_TEMP           0x44
#define DS18B20_FUNCTION_COMMAND_WRITE_SCRATCHPAD       0x4E
#define DS18B20_FUNCTION_COMMAND_READ_SCRATCHPAD        0xBE
#define DS18B20_FUNCTION_COMMAND_COPY_SCRATCHPAD        0x48
#define DS18B20_FUNCTION_COMMAND_RECALL_E2              0xB8
#define DS18B20_FUNCTION_COMMAND_READ_POWER_SUPPLY      0xB4


#define DS18B20_TH_HIGHER_THRESHOLD                     42      // 42 degree celcius
#define DS18B20_TL_LOWER_THRESHOLD                      -10     // -10 degree celcius

#define pgm_read_byte(addr)   (*(const unsigned char *)(addr))

static const uint8_t dscrc2x16_table[] = {
	0x00, 0x5E, 0xBC, 0xE2, 0x61, 0x3F, 0xDD, 0x83,
	0xC2, 0x9C, 0x7E, 0x20, 0xA3, 0xFD, 0x1F, 0x41,
	0x00, 0x9D, 0x23, 0xBE, 0x46, 0xDB, 0x65, 0xF8,
	0x8C, 0x11, 0xAF, 0x32, 0xCA, 0x57, 0xE9, 0x74
};


int ds18b20_sensor(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *ezlo_device, void *arg, void *user_arg);
int ds18b20_sensor_v3(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);


#endif // _030_SENS_DS18B20_SENSOR_H_


