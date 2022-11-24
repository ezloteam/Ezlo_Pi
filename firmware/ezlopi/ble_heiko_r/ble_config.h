#include "esp_gatts_api.h"

#define GATTS_TAG "EGTeachersButton" // Prepended to all logging output

#define GATTS_CHAR_VAL_LEN_MAX		22 // maximum length in bytes of a characteristic's value. TODO: find out how to determine this value?

/* Number of services, characteristics and descriptors used in ble_server.c.
 * Needs to be equal to the size of the gatts_service, gatts_char and gatts_descr arrays.
 */
#define GATTS_SERVICE_NUM									2	// Battery Service and Teacher's Button Service
#define GATTS_CHAR_NUM										4	// Combined number of characteristics for all services
#define GATTS_DESCR_NUM										11	// Combined number of descriptors for all characteristics

#define GATTS_BATTERY_SERVICE_UUID							0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x0F, 0x18, 0x00, 0x00 // 16bit shortened UUID for Battery Service 0x180F expanded to 128 bit with Bluetooth Base UUID, in "reverse" LSB<->MSB order
#define GATTS_BATTERY_SERVICE_NUM_HANDLES					1+2*1+2 // 1 for Service Declaration, 2 for Battery Level characteristic, 1 for each descriptor
#define GATTS_TEACHERBUTTON_SERVICE_UUID					0xc5, 0xb9, 0x31, 0x68, 0xfa, 0x31, 0x09, 0xa0, 0x8f, 0x42, 0xa5, 0xd3, 0xe9, 0x17, 0xe2, 0x0b // random 128bit UUID for custom Teacher's Button Service: 0be217e9-d3a5-428f-a009-31fa6831b9c5
#define GATTS_TEACHERBUTTON_SERVICE_NUM_HANDLES				1+2*3+9 // 1 for Service Declaration, 2 for each characteristic, 1 for each descriptor

#define GATTS_TEACHERBUTTON_BUTTON_NUMBER_CHAR_UUID			{0x76, 0xf6, 0x15, 0x1d, 0xd9, 0x2f, 0x0f, 0x8d, 0x4c, 0x46, 0xf0, 0xe5, 0x89, 0x5b, 0x0b, 0x67} // random 128bit UUID: 670b5b89-e5f0-464c-8d0f-2fd91d15f676
#define GATTS_TEACHERBUTTON_REF_VOLTAGE_CHAR_UUID			{0xa0, 0x9e, 0x78, 0x63, 0xf0, 0x0e, 0x75, 0xbf, 0x55, 0x48, 0x22, 0x85, 0xa6, 0xf2, 0x16, 0x32} // random 128but UUID:	3216f2a6-8522-4855-bf75-0ef063789ea0
#define GATTS_TEACHERBUTTON_FIRMWARE_VERSION_CHAR_UUID		{0x64, 0x5d, 0xff, 0x33, 0xd5, 0x2b, 0x42, 0xa1, 0xdc, 0x43, 0xce, 0x9f, 0x73, 0x03, 0x81, 0xc2} // random 128bit UUID: c2810373-9fce-43dc-a142-2bd533ff5d64

/* Profile IDs corresponding to the position in the gatts_service array (see
 * ble_server.c).
 */
#define APP_ID_BATTERY_SERVICE 0
#define APP_ID_TEACHERBUTTON_SERVICE 1

/* Positions of the respective characteristics in the gatts_char array,
 * counting from 0, so that we can more easily access them in the main code.
 */
#define GATTS_BATTERY_LEVEL_CHAR_POS	0
#define GATTS_BUTTON_NUMBER_CHAR_POS	1
#define GATTS_REF_VOLTAGE_CHAR_POS		2

/* GAP advertising configuration, determines how the teacher's button will
 * advertise itself via BLE in configuration mode.
 */
#define BLE_DEVICE_NAME_LEN			23					// Length of the device name below. Count 2 bytes for the button number and add one for the (invisible) \0 at the end of the string
#define BLE_DEVICE_NAME				"EG Teacher's Button %02u"	// Base name used as device name for BLE advertising; %02u will be replaced by the configured button number
#define BLE_MANUFACTURER_DATA_LEN	2					// Length of the manufacturer specific advertising payload. Currently contains only the manufacturer ID (2 bytes)
#define BLE_MANUFACTURER_DATA		{0xFF, 0xFF}		// No manufacturer ID (see https://www.bluetooth.com/specifications/assigned-numbers/company-identifiers)

#define BLE_APPEARANCE				0x4C3	// External appearance of the device. 0x4C3 = 1219 = Button (Control Device), see https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.gap.appearance.xml

#define BLE_SERVICE_DATA_LEN		0
#define BLE_SERVICE_DATA			NULL	// TODO: figure out what "service data" can be used for

#define BLE_ADV_MIN_INTERVAL		800		// Minimum interval in which the advertising data should be sent out. Range: 0x0020 to 0x4000 (X * 0.625ms) -> 500ms
#define BLE_ADV_MAX_INTERVAL		1600	// Maximum interval in which the advertising data should be sent out. Range: 0x0020 to 0x4000 (X * 0.625ms) -> 1000ms

/* The BLE connection parameters will change to these when another device
 * connects to this teacher's button. Advertising is stopped in this case.
 * TODO: figure out what these values mean
 */
#define BLE_CONNECTED_MAX_INTERVAL	400		// Min connection interval (X * 1.25ms, TODO: to be confirmed) -> 500ms
#define BLE_CONNECTED_MIN_INTERVAL	800		// Max connection interval (X * 1.25ms, TODO: to be confirmed) -> 1000ms
#define BLE_CONNECTED_TIMEOUT		400		// Supervision timeout for the LE Link. Range: 0x000A to 0x0C80 (X * 10ms) -> 4000ms

/* These structs define which attributes each service, characteristic or
 * descriptor have.
 * The individual services, characteristics and descriptors are then defined in
 * gatt_config.c
 */
struct gatts_service_inst {
	uint16_t gatts_if;
	uint16_t app_id;
	uint16_t conn_id;
	uint16_t service_handle;
	esp_gatt_srvc_id_t service_id;
	uint16_t num_handles;
};

struct gatts_char_inst {
	uint32_t service_pos;
	esp_bt_uuid_t char_uuid;
	esp_gatt_perm_t char_perm;
	esp_gatt_char_prop_t char_property;
	esp_attr_value_t *char_val;
	esp_attr_control_t *char_control;
	uint16_t char_handle;
	char char_nvs[16];
};

struct gatts_descr_inst {
	uint32_t char_pos;
	esp_bt_uuid_t descr_uuid;
	esp_gatt_perm_t descr_perm;
	esp_attr_value_t *descr_val;
	esp_attr_control_t *descr_control;
	uint16_t descr_handle;
};

extern struct gatts_service_inst gatts_service[];
extern struct gatts_char_inst gatts_char[];
extern struct gatts_descr_inst gatts_descr[];
