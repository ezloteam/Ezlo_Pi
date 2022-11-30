#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_system.h"
#include "esp_log.h"

#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"

#include "ble_config.h"
#include "version.h"

/* The "layout" of the services, characteristics and descriptors is as follows:
 * - Service: Battery Service
 *   - Characteristic: Battery Level
 *     - Descriptor: Client Characteristic Configuration
 *     - Descriptor: Characteristic Presentation Format (8bit int, percent)
 * - Service: Teacher's Button Service
 *   - Characteristic: Button number
 *     - Descriptor: Characteristic User Description
 *     - Descriptor: Client Characteristic Configuration
 *     - Descriptor: Characteristic Presentation Format (8bit int, unitless)
 *   - Characteristic: Reference voltage
 *     - Descriptor: Characteristic User Description
 *     - Descriptor: Client Characteristic Configuration
 *     - Descriptor: Characteristic Presentation Format (16bit int, unitless)
 *   - Characteristic: Firmware version
 *     - Descriptor: Characteristic User Description
 *     - Descriptor: Client Characteristic Configuration
 *     - Descriptor: Characteristic Presentation Format (UTF-8 string, unitless)
 */

/* First, we define the variables we want to share via GATT.*/

/* Characteristic/descriptor values: Battery level */
// battery level in percent as integer
uint8_t battery_service_char_battery_level_int = 42;
// Client Characteristic Configuration: 0x00: notifications disabled; 0x00: indications disabled
uint8_t battery_service_char_battery_level_descr_config_str[2] = {0x00, 0x00};
// Characteristic Presentation Format: 0x04: unsigned 8 bit integer; 0x00: no exponent; 0x27AD: unit = percentage; 0x01: Bluetooth SIG namespace; 0x0000: No description
uint8_t battery_service_char_battery_level_descr_present_str[7] = {0x04, 0x00, 0xAD, 0x27, 0x01, 0x00, 0x00};

/* Characteristic/descriptor values: Button number */
// pushbutton number to connect to as integer
uint8_t teacherbutton_service_char_button_number_int = 0; // button number will default to 0 (i.e. bind to nearest button upon startup) if it is not yet set in NVS
// Characteristic User Description:
uint8_t teacherbutton_service_char_button_number_descr_user_str[13] = "Button number";
// Client Characteristic Configuration: 0x00: notifications disabled; 0x00: indications disabled
uint8_t teacherbutton_service_char_button_number_descr_config_str[2] = {0x00, 0x00};
// Characteristic Presentation Format: 0x04: unsigned 8 bit integer; 0x00: no exponent; 0x2700: unit = unitless; 0x01: Bluetooth SIG namespace; 0x0000: No description
uint8_t teacherbutton_service_char_button_number_descr_present_str[7] = {0x04, 0x00, 0x00, 0x27, 0x01, 0x00, 0x00};

/* Characteristic/descriptor values: Reference voltage */
// reference voltage for ADC claibration
#define REF_VOLTAGE_DEFAULT 1100
uint8_t teacherbutton_service_char_ref_voltage_int[2] = {REF_VOLTAGE_DEFAULT & 0x00FF, (REF_VOLTAGE_DEFAULT >> 8) & 0x00FF}; // ADC reference voltage in mV as measured externally; default: 110mV
// Characteristic User Description:
uint8_t teacherbutton_service_char_ref_voltage_descr_user_str[18] = "Reference voltage";
// Client Characteristic Configuration: 0x00: notifications disabled; 0x00: indications disabled
uint8_t teacherbutton_service_char_ref_voltage_descr_config_str[2] = {0x00, 0x00};
// Characteristic Presentation Format: 0x06: unsigned 16 bit integer; -3: 10^(-3); 0x2728: unit = volt; 0x01: Bluetooth SIG namespace; 0x0000: No description
uint8_t teacherbutton_service_char_ref_voltage_descr_present_str[7] = {0x06, -3, 0x28, 0x27, 0x01, 0x00, 0x00};

/* Characteristic/descriptor values: Firmware version */
// 3 bytes firmware version
uint8_t teacherbutton_service_char_firmware_version_str[3] = {1, 2, 3}; // {EG_VERSION_MAJOR, EG_VERSION_MINOR, EG_VERSION_REVISION};
// Characteristic User Description:
uint8_t teacherbutton_service_char_firmware_version_descr_user_str[17] = "Firmware version";
// Client Characteristic Configuration: 0x00: notifications disabled; 0x00: indications disabled
uint8_t teacherbutton_service_char_firmware_version_descr_config_str[2] = {0x00, 0x00};
// Characteristic Presentation Format: 0x1B: opaque structure; 0x00: no exponent; 0x2700: unit = unitless; 0x01: Bluetooth SIG namespace; 0x0000: No description
uint8_t teacherbutton_service_char_firmware_version_descr_present_str[7] = {0x1B, 0x00, 0x00, 0x27, 0x01, 0x00, 0x00};

/* The ESP GATTS API doesn't take the above characteristic and descriptor
 * values directly, but needs them wrapped in these structures:
 */
esp_attr_value_t gatts_battery_service_char_battery_level_val = {
	.attr_max_len = GATTS_CHAR_VAL_LEN_MAX,
	.attr_len = sizeof(battery_service_char_battery_level_int),
	.attr_value = &battery_service_char_battery_level_int,
};
esp_attr_value_t gatts_battery_service_char_battery_level_descr_config_val = {
	.attr_max_len = GATTS_CHAR_VAL_LEN_MAX,
	.attr_len = sizeof(battery_service_char_battery_level_descr_config_str),
	.attr_value = battery_service_char_battery_level_descr_config_str,
};
esp_attr_value_t gatts_battery_service_char_battery_level_descr_present_val = {
	.attr_max_len = GATTS_CHAR_VAL_LEN_MAX,
	.attr_len = sizeof(battery_service_char_battery_level_descr_present_str),
	.attr_value = battery_service_char_battery_level_descr_present_str,
};

esp_attr_value_t gatts_teacherbutton_service_char_button_number_val = {
	.attr_max_len = GATTS_CHAR_VAL_LEN_MAX,
	.attr_len = sizeof(teacherbutton_service_char_button_number_int),
	.attr_value = &teacherbutton_service_char_button_number_int,
};
esp_attr_value_t gatts_teacherbutton_service_char_button_number_descr_user_val = {
	.attr_max_len = GATTS_CHAR_VAL_LEN_MAX,
	.attr_len = sizeof(teacherbutton_service_char_button_number_descr_user_str),
	.attr_value = teacherbutton_service_char_button_number_descr_user_str,
};
esp_attr_value_t gatts_teacherbutton_service_char_button_number_descr_config_val = {
	.attr_max_len = GATTS_CHAR_VAL_LEN_MAX,
	.attr_len = sizeof(teacherbutton_service_char_button_number_descr_config_str),
	.attr_value = teacherbutton_service_char_button_number_descr_config_str,
};
esp_attr_value_t gatts_teacherbutton_service_char_button_number_descr_present_val = {
	.attr_max_len = GATTS_CHAR_VAL_LEN_MAX,
	.attr_len = sizeof(teacherbutton_service_char_button_number_descr_present_str),
	.attr_value = teacherbutton_service_char_button_number_descr_present_str,
};

esp_attr_value_t gatts_teacherbutton_service_char_ref_voltage_val = {
	.attr_max_len = GATTS_CHAR_VAL_LEN_MAX,
	.attr_len = sizeof(teacherbutton_service_char_ref_voltage_int),
	.attr_value = teacherbutton_service_char_ref_voltage_int,
};
esp_attr_value_t gatts_teacherbutton_service_char_ref_voltage_descr_user_val = {
	.attr_max_len = GATTS_CHAR_VAL_LEN_MAX,
	.attr_len = sizeof(teacherbutton_service_char_ref_voltage_descr_user_str),
	.attr_value = teacherbutton_service_char_ref_voltage_descr_user_str,
};
esp_attr_value_t gatts_teacherbutton_service_char_ref_voltage_descr_config_val = {
	.attr_max_len = GATTS_CHAR_VAL_LEN_MAX,
	.attr_len = sizeof(teacherbutton_service_char_ref_voltage_descr_config_str),
	.attr_value = teacherbutton_service_char_ref_voltage_descr_config_str,
};
esp_attr_value_t gatts_teacherbutton_service_char_ref_voltage_descr_present_val = {
	.attr_max_len = GATTS_CHAR_VAL_LEN_MAX,
	.attr_len = sizeof(teacherbutton_service_char_ref_voltage_descr_present_str),
	.attr_value = teacherbutton_service_char_ref_voltage_descr_present_str,
};

esp_attr_value_t gatts_teacherbutton_service_char_firmware_version_val = {
	.attr_max_len = GATTS_CHAR_VAL_LEN_MAX,
	.attr_len = sizeof(teacherbutton_service_char_firmware_version_str),
	.attr_value = teacherbutton_service_char_firmware_version_str,
};
esp_attr_value_t gatts_teacherbutton_service_char_firmware_version_descr_user_val = {
	.attr_max_len = GATTS_CHAR_VAL_LEN_MAX,
	.attr_len = sizeof(teacherbutton_service_char_firmware_version_descr_user_str),
	.attr_value = teacherbutton_service_char_firmware_version_descr_user_str,
};
esp_attr_value_t gatts_teacherbutton_service_char_firmware_version_descr_config_val = {
	.attr_max_len = GATTS_CHAR_VAL_LEN_MAX,
	.attr_len = sizeof(teacherbutton_service_char_firmware_version_descr_config_str),
	.attr_value = teacherbutton_service_char_firmware_version_descr_config_str,
};
esp_attr_value_t gatts_teacherbutton_service_char_firmware_version_descr_present_val = {
	.attr_max_len = GATTS_CHAR_VAL_LEN_MAX,
	.attr_len = sizeof(teacherbutton_service_char_firmware_version_descr_present_str),
	.attr_value = teacherbutton_service_char_firmware_version_descr_present_str,
};

/* To define each of our services, we just set the .gatts_if to none and the
 * number of handles this service will use as calculated in ble_config.h
 * The gatts_event_handler() below will take care of setting all the values
 * when the service is initialised.
 *
 * 1st service: Battery Service
 * 2nd service: Teacher's Button Service
 */
s_gatts_service_inst_t gatts_service[GATTS_SERVICE_NUM] = {
	{
		.gatts_if = ESP_GATT_IF_NONE, /* gatts_if not known yet, so initial is ESP_GATT_IF_NONE */
		.num_handles = GATTS_BATTERY_SERVICE_NUM_HANDLES,
	},
	{
		.gatts_if = ESP_GATT_IF_NONE, /* gatts_if not known yet, so initial is ESP_GATT_IF_NONE */
		.num_handles = GATTS_TEACHERBUTTON_SERVICE_NUM_HANDLES,
	},
};

/* Here we define all the characteristics for all the services.
 * To associate a characteristic with its service, set the .service_pos to the
 * corresponding index in the gatts_service array above:
 * 0 = 1st service, 1 = 2nd service, ...
 *
 * Standard bluetooth characteristics such as "Battery level" (see
 * https://www.bluetooth.com/specifications/gatt/characteristics) use a 16 bit
 * UUID. For custom characteristics, we have to use a random 128 bit UUID as
 * generated by https://www.uuidgenerator.net/ .
 * For better readability, our random UUIDs are defined in ble_server.h .
 *
 * The .char_handle is set automatically by the gatts_check_add_char() function
 * below, once the characteristic has been added to the service.
 *
 * .char_nvs is the key under which the characteristic's value is stored in NVS.
 * Its maximum length is 15 bytes (=15 characters). The array is 16 bytes long
 * instead of just 15, because a string we add to it will be null-terminated
 * (\0 is automatically added as last element. Set it to "" when the
 * value is not stored in NVS.
 */
s_gatts_char_inst_t gatts_char[GATTS_CHAR_NUM] = {
	{
		/* Battery Service -> Battery Level */
		.service_pos = 0,				  // Battery service
		.char_uuid.len = ESP_UUID_LEN_16, // Battery Level
		.char_uuid.uuid.uuid16 = ESP_GATT_UUID_BATTERY_LEVEL,
		.char_perm = ESP_GATT_PERM_READ,
		.char_property = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY,
		.char_val = &gatts_battery_service_char_battery_level_val,
		.char_control = NULL,
		.char_handle = 0,
		.char_nvs = "",
	},
	{
		/* Teacher's Button Service -> Button number */
		.service_pos = 1,				   // Teacher's Button service
		.char_uuid.len = ESP_UUID_LEN_128, // Custom characteristic -> 128bit UUID
		.char_uuid.uuid.uuid128 = GATTS_TEACHERBUTTON_BUTTON_NUMBER_CHAR_UUID,
		.char_perm = ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
		.char_property = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE,
		.char_val = &gatts_teacherbutton_service_char_button_number_val,
		.char_control = NULL,
		.char_handle = 0,
		.char_nvs = "button_number",
	},
	{
		/* Teacher's Button Service -> Reference voltage */
		.service_pos = 1,				   // Teacher's Button service
		.char_uuid.len = ESP_UUID_LEN_128, // Custom characteristic -> 128bit UUID
		.char_uuid.uuid.uuid128 = GATTS_TEACHERBUTTON_REF_VOLTAGE_CHAR_UUID,
		.char_perm = ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
		.char_property = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_NOTIFY,
		.char_val = &gatts_teacherbutton_service_char_ref_voltage_val,
		.char_control = NULL,
		.char_handle = 0,
		.char_nvs = "ref_voltage",
	},
	{
		/* Teacher's Button Service -> Firmware version */
		.service_pos = 1,				   // Teacher's Button service
		.char_uuid.len = ESP_UUID_LEN_128, // Custom characteristic -> 128bit UUID
		.char_uuid.uuid.uuid128 = GATTS_TEACHERBUTTON_FIRMWARE_VERSION_CHAR_UUID,
		.char_perm = ESP_GATT_PERM_READ,
		.char_property = ESP_GATT_CHAR_PROP_BIT_READ,
		.char_val = &gatts_teacherbutton_service_char_firmware_version_val,
		.char_control = NULL,
		.char_handle = 0,
		.char_nvs = "",
	},
};

/* Here we define all the descriptors for all the characteristics.
 * To associate a descriptor to a characteristic, set the .char_pos to the
 * corresponding index in the gl_char array above:
 * 0 = 1st characteristic, 1 = 2nd characteristic, ...
 *
 * All we use here are standard bluetooth descriptors (see
 * https://www.bluetooth.com/specifications/gatt/descriptors) with a 16 bit
 * UUID.
 *
 * The .descr_handle is set automatically by the gatts_check_add_descr()
 * function below, once the descriptor has been added to the characteristic.
 */
s_gatts_descr_inst_t gatts_descr[GATTS_DESCR_NUM] = {
	{/* Battery Level -> Client Characteristic Configuration */
	 .char_pos = 0,
	 .descr_uuid.len = ESP_UUID_LEN_16,
	 .descr_uuid.uuid.uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG,
	 .descr_perm = ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
	 .descr_val = &gatts_battery_service_char_battery_level_descr_config_val,
	 .descr_control = NULL,
	 .descr_handle = 0},
	{/* Battery Level -> Characteristic Presentation Format */
	 .char_pos = 0,
	 .descr_uuid.len = ESP_UUID_LEN_16,
	 .descr_uuid.uuid.uuid16 = ESP_GATT_UUID_CHAR_PRESENT_FORMAT,
	 .descr_perm = ESP_GATT_PERM_READ,
	 .descr_val = &gatts_battery_service_char_battery_level_descr_present_val,
	 .descr_control = NULL,
	 .descr_handle = 0},
	{/* Button number -> Characteristic User Description */
	 .char_pos = 1,
	 .descr_uuid.len = ESP_UUID_LEN_16,
	 .descr_uuid.uuid.uuid16 = ESP_GATT_UUID_CHAR_DESCRIPTION,
	 .descr_perm = ESP_GATT_PERM_READ,
	 .descr_val = &gatts_teacherbutton_service_char_button_number_descr_user_val,
	 .descr_control = NULL,
	 .descr_handle = 0},
	{/* Button number -> Client Characteristic Configuration */
	 .char_pos = 1,
	 .descr_uuid.len = ESP_UUID_LEN_16,
	 .descr_uuid.uuid.uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG,
	 .descr_perm = ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
	 .descr_val = &gatts_teacherbutton_service_char_button_number_descr_config_val,
	 .descr_control = NULL,
	 .descr_handle = 0},
	{/* Button number -> Characteristic Presentation Format */
	 .char_pos = 1,
	 .descr_uuid.len = ESP_UUID_LEN_16,
	 .descr_uuid.uuid.uuid16 = ESP_GATT_UUID_CHAR_PRESENT_FORMAT,
	 .descr_perm = ESP_GATT_PERM_READ,
	 .descr_val = &gatts_teacherbutton_service_char_button_number_descr_present_val,
	 .descr_control = NULL,
	 .descr_handle = 0},
	{/* Reference voltage -> Characteristic User Description */
	 .char_pos = 2,
	 .descr_uuid.len = ESP_UUID_LEN_16,
	 .descr_uuid.uuid.uuid16 = ESP_GATT_UUID_CHAR_DESCRIPTION,
	 .descr_perm = ESP_GATT_PERM_READ,
	 .descr_val = &gatts_teacherbutton_service_char_ref_voltage_descr_user_val,
	 .descr_control = NULL,
	 .descr_handle = 0},
	{/* Reference voltage -> Client Characteristic Configuration */
	 .char_pos = 2,
	 .descr_uuid.len = ESP_UUID_LEN_16,
	 .descr_uuid.uuid.uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG,
	 .descr_perm = ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
	 .descr_val = &gatts_teacherbutton_service_char_ref_voltage_descr_config_val,
	 .descr_control = NULL,
	 .descr_handle = 0},
	{/* Reference voltage -> Characteristic Presentation Format */
	 .char_pos = 2,
	 .descr_uuid.len = ESP_UUID_LEN_16,
	 .descr_uuid.uuid.uuid16 = ESP_GATT_UUID_CHAR_PRESENT_FORMAT,
	 .descr_perm = ESP_GATT_PERM_READ,
	 .descr_val = &gatts_teacherbutton_service_char_ref_voltage_descr_present_val,
	 .descr_control = NULL,
	 .descr_handle = 0},
	{/* Firmware version -> Characteristic User Description */
	 .char_pos = 3,
	 .descr_uuid.len = ESP_UUID_LEN_16,
	 .descr_uuid.uuid.uuid16 = ESP_GATT_UUID_CHAR_DESCRIPTION,
	 .descr_perm = ESP_GATT_PERM_READ,
	 .descr_val = &gatts_teacherbutton_service_char_firmware_version_descr_user_val,
	 .descr_control = NULL,
	 .descr_handle = 0},
	{/* Firmware version -> Client Characteristic Configuration */
	 .char_pos = 3,
	 .descr_uuid.len = ESP_UUID_LEN_16,
	 .descr_uuid.uuid.uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG,
	 .descr_perm = ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
	 .descr_val = &gatts_teacherbutton_service_char_firmware_version_descr_config_val,
	 .descr_control = NULL,
	 .descr_handle = 0},
	{/* Firmware version -> Characteristic Presentation Format */
	 .char_pos = 3,
	 .descr_uuid.len = ESP_UUID_LEN_16,
	 .descr_uuid.uuid.uuid16 = ESP_GATT_UUID_CHAR_PRESENT_FORMAT,
	 .descr_perm = ESP_GATT_PERM_READ,
	 .descr_val = &gatts_teacherbutton_service_char_firmware_version_descr_present_val,
	 .descr_control = NULL,
	 .descr_handle = 0}};
