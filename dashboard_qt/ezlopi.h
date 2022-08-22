#ifndef EZLOPI_H
#define EZLOPI_H

#include<QDataStream>

typedef qint8 EZPI_INT8;
typedef quint8 EZPI_UINT8;
typedef qint16 EZPI_INT16;
typedef quint16 EZPI_UINT16;
typedef qint32 EZPI_INT32;
typedef quint32 EZPI_UINT32;
typedef qint64 EZPI_INT64;
typedef quint64 EZPI_UINT64;

typedef bool EZPI_BOOL;

typedef QString EZPI_STRING;

#define     SIZE_DEVICE_ID      8
#define     SIZE_DEVICE_FNAME   16
#define     SIZE_ROOM_ID        8
#define     SIZE_ID_I           8
#define     SIZE_UART_NAME      20

#define     MAX_DEVICES         20

#define     EZPI_ID             1655702685UL        // Random Ezpi ID
#define     EZPI_DEFAULT_BAUD   115200UL
#define     EZPI_MAX_GPIOS      28

#define     EZPI_MAX_DEVICES    10

#define     EZPI_MAX_DEV_DIO        5
#define     EZPI_MAX_DEV_DIP        5
#define     EZPI_MAX_DEV_AIP        5
#define     EZPI_MAX_DEV_ONEWIRE    3
#define     EZPI_MAX_DEV_I2C        3
#define     EZPI_MAX_DEV_SPI        1

#define     MAX_DEV             EZPI_MAX_DEVICES

#define     EZPI_ESP32_GENERIC_PINOUT_COUNT                     40

#define     SIZE_EZPI_OFFSET_CONN_ID_0                          0X0000
#define     SIZE_EZPI_OFFSET_CONN_ID_1                          0X7000
#define     SIZE_EZPI_OFFSET_HUB_ID_0                           0XE000
#define     SIZE_EZPI_OFFSET_HUB_ID_1                           0XF000

#define     EZPI_SERIAL_READ_TIMEOUT                            3000
#define     EZPI_FIRMWARE_CHECK_TIMEOUT                         4000

enum ezpi_high_low {
    EZPI_LOW,
    EZPI_HIGH
};

enum ezpi_error_codes_configurator {
    EZPI_SUCCESS,
    EZPI_ERROR_REACHED_MAX_DEV
};

enum ezpi_dev_type {
    EZPI_DEV_TYPE_RESTRICTED,
    EZPI_DEV_TYPE_DIGITAL_OP,
    EZPI_DEV_TYPE_DIGITAL_IP,
    EZPI_DEV_TYPE_ANALOG_IP,
    EZPI_DEV_TYPE_ANALOG_OP,
    EZPI_DEV_TYPE_PWM,
    EZPI_DEV_TYPE_UART,
    EZPI_DEV_TYPE_ONE_WIRE,
    EZPI_DEV_TYPE_I2C,
    EZPI_DEV_TYPE_SPI,    
    EZPI_DEV_TYPE_TOTAL,
    EZPI_DEV_TYPE_INPUT_ONLY = 253,
    EZPI_DEV_TYPE_OUTPUT_ONLY,
    EZPI_DEV_TYPE_UNCONFIGURED
};

enum ezpi_item_type {
    EZPI_ITEM_TYPE_NONE,
    EZPI_ITEM_TYPE_LED,
    EZPI_ITEM_TYPE_RELAY,
    EZPI_ITEM_TYPE_PLUG,
    EZPI_ITEM_TYPE_BUTTON,
    EZPI_ITEM_TYPE_MPU6050,
    EZPI_ITEM_TYPE_ADXL345,
    EZPI_ITEM_TYPE_GY271,
    EZPI_ITEM_TYPE_MCP4725,
    EZPI_ITEM_TYPE_GY530,
    EZPI_ITEM_TYPE_DS1307,
    EZPI_ITEM_TYPE_MAX30100,
    EZPI_ITEM_TYPE_BMP280_I2C,
    EZPI_ITEM_TYPE_BMP280_SPI,
    EZPI_ITEM_TYPE_LNA219,
    EZPI_ITEM_TYPE_DHT11,
    EZPI_ITEM_TYPE_DHT22,
    EZPI_ITEM_TYPE_POT_GENERIC,
    EZPI_ITEM_TYPE_TOTAL
};

enum commands {
    GET_CONFIG = 0xa0,
    SET_CONFIG,
    SET_VOL,
    SET_WiFi,
    FIRST_DEV,
    SET_DEV,
    GET_DEV,
    END_DEV
};

enum ezpi_cmd {
    CMD_ACTION_NONE,
    CMD_ACTION_GET_INFO,
    CMD_ACTION_SET_WIFI,
    CMD_ACTION_SET_CONFIG,
    CMD_ACTION_GET_CONFIG,
    CMD_ACTION_TOTAL
};

enum ezpi_board_type {
    EZPI_BOARD_TYPE_NONE,
    EZPI_BOARD_TYPE_ESP32_GENERIC,
    EZPI_BOARD_TYPE_ESP32_C3,
    EZPI_BOARD_TYPE_ESP32_S3,
    EZPI_BOARD_TYPE_TOTAL
};

enum ezpi_adc_resln {
    EZPI_ADC_RESLN_NONE,
    EZPI_ADC_RESLN_8_BIT,
    EZPI_ADC_RESLN_10_BIT,
    EZPI_ADC_RESLN_12_BIT,
    EZPI_ADC_RESLN_TOTAL,
};

typedef struct ezpi_info {
    EZPI_UINT32 v_sw;
    EZPI_UINT8 v_type;
    EZPI_UINT16 build;
    EZPI_UINT32 v_idf;
    EZPI_UINT32 uptime;
    EZPI_UINT32 build_date;

    // To be implemented
//    "boot_count": 15,
//    "boot_reason": 2,
//    "mac": 1577079727,
//    "uuid": "65261d76-e584-4d35-aff1-d84bd043",
//    "serial": 100004032,
//    "ssid": "ssid",
//    "dev_type": 1,
//    "dev_flash": 64256,
//    "dev_free_flash": 300,
//    "dev_name": "My Device"

} ezpi_info_t;

typedef struct ezpi_device_digital_op {
    ezpi_dev_type dev_type;
    EZPI_STRING dev_name;
    EZPI_UINT16 id_room;
    ezpi_item_type id_item;
    EZPI_BOOL val_ip;
    EZPI_BOOL val_op;
    EZPI_UINT8 gpio_in;
    EZPI_UINT8 gpio_out;
    EZPI_BOOL is_ip;
    EZPI_BOOL ip_inv;
    EZPI_BOOL pullup_ip;
    EZPI_BOOL pullup_op;
    EZPI_BOOL op_inv;
} ezpi_device_digital_op_t;

typedef struct ezpi_device_digital_ip {
    ezpi_dev_type dev_type;
    EZPI_STRING dev_name;
    EZPI_UINT16 id_room;
    ezpi_item_type id_item;
    EZPI_BOOL val_ip;
    EZPI_UINT8 gpio;
    EZPI_BOOL pull_up;
    EZPI_BOOL logic_inv;
} ezpi_device_digital_ip_t;

typedef struct ezpi_device_analog_ip {
    ezpi_dev_type dev_type;
    EZPI_STRING dev_name;
    EZPI_UINT16 id_room;
    ezpi_item_type id_item;
    EZPI_UINT8 gpio;
    ezpi_adc_resln resln_bit;
} ezpi_device_analog_ip_t;

typedef struct ezpi_device_one_wire {
    ezpi_dev_type dev_type;
    EZPI_STRING dev_name;
    EZPI_UINT16 id_room;
    ezpi_item_type id_item;
    EZPI_BOOL val_ip;
    EZPI_BOOL pull_up;
    EZPI_UINT8 gpio;
} ezpi_device_one_wire_t;

typedef struct ezpi_device_I2C {
    ezpi_dev_type dev_type;
    EZPI_STRING dev_name;
    EZPI_UINT16 id_room;
    ezpi_item_type id_item;
    EZPI_UINT8 gpio_sda;
    EZPI_UINT8 gpio_scl;
    EZPI_BOOL pullup_scl;
    EZPI_BOOL pullup_sda;
    EZPI_UINT8 slave_addr;
} ezpi_device_I2C_t;

typedef struct ezpi_device_SPI {
    ezpi_dev_type dev_type;
    EZPI_STRING dev_name;
    EZPI_UINT16 id_room;
    ezpi_item_type id_item;
    EZPI_UINT8 gpio_miso;
    EZPI_UINT8 gpio_mosi;
    EZPI_UINT8 gpio_sck;
    EZPI_UINT8 gpio_cs;
} ezpi_device_SPI_t;

typedef union conv_u16_array {
    uint8_t data_bytes[sizeof(uint16_t)];
    uint16_t data;
} conv_u16_array_t;

typedef union conv_u64_array {
    uint8_t data_bytes[sizeof(uint64_t)];
    uint64_t data;
} conv_64_array_t;

class EzPi {

private:

    ezpi_board_type _ezpi_board_type;

    std::vector <EZPI_UINT8> ezpi_gpio_pool;
    std::vector <ezpi_device_digital_op_t> ezpi_output_devices;
    std::vector <ezpi_device_digital_ip_t> ezpi_input_devices;
    std::vector <ezpi_device_analog_ip_t> ezpi_analog_input_devices;
    std::vector <ezpi_device_one_wire_t> ezpi_onewire_devices;
    std::vector <ezpi_device_I2C_t> ezpi_i2c_devices;
    std::vector <ezpi_device_SPI_t> ezpi_spi_devices;

    QStringList * ezpi_device_types_str;
    QStringList * ezpi_item_types_str;

    ezpi_info_t * ezpi_firmware_info;

public:
    EzPi();
    ~EzPi();
    void EZPI_INIT_BOARD(void);
    void EZPI_SET_BOARD_TYPE(ezpi_board_type board_type);
    ezpi_error_codes_configurator EZPI_ADD_OUTPUT_DEVICE(ezpi_device_digital_op_t d);
    ezpi_error_codes_configurator EZPI_ADD_INPUT_DEVICE(ezpi_device_digital_ip_t d);
    ezpi_error_codes_configurator EZPI_ADD_AINPUT_DEVICE(ezpi_device_analog_ip_t d);
    ezpi_error_codes_configurator EZPI_ADD_ONEWIRE_DEVICE(ezpi_device_one_wire_t d);
    ezpi_error_codes_configurator EZPI_ADD_I2C_DEVICE(ezpi_device_I2C_t d);
    ezpi_error_codes_configurator EZPI_ADD_SPI_DEVICE(ezpi_device_SPI_t d);

    void EZPI_DELETE_OUTPUT_DEVICE(void) { ezpi_output_devices.pop_back(); }
    void EZPI_DELETE_INPUT_DEVICE(void) { ezpi_input_devices.pop_back(); }
    void EZPI_DELETE_AINPUT_DEVICE(void) { ezpi_analog_input_devices.pop_back(); }
    void EZPI_DELETE_ONEWIRE_DEVICE(void) { ezpi_onewire_devices.pop_back(); }
    void EZPI_DELETE_I2C_DEVICE(void) { ezpi_i2c_devices.pop_back(); }
    void EZPI_DELETE_SPI_DEVICE(void) { ezpi_spi_devices.pop_back(); }

    void EZPI_CLEAR_OUTPUT_DEVICES(void) { ezpi_output_devices.clear(); }
    void EZPI_CLEAR_INPUT_DEVICES(void) { ezpi_input_devices.clear(); }
    void EZPI_CLEAR_AINPUT_DEVICES(void) { ezpi_analog_input_devices.clear(); }
    void EZPI_CLEAR_ONEWIRE_DEVICES(void) { ezpi_onewire_devices.clear(); }
    void EZPI_CLEAR_I2C_DEVICES(void) { ezpi_i2c_devices.clear(); }
    void EZPI_CLEAR_SPI_DEVICES(void) { ezpi_spi_devices.clear(); }

    ezpi_board_type EZPI_GET_BOARD_TYPE(void) {return _ezpi_board_type;}
    std::vector<EZPI_UINT8> EZPI_GET_GPIO_POOL(void) {return ezpi_gpio_pool;}
    void EZPI_SET_GPIO_POOL(EZPI_UINT8 index, ezpi_dev_type d) { ezpi_gpio_pool.at(index) = d;}
    ezpi_dev_type EZPI_GET_GPIO_POOL(EZPI_UINT8 index) {return (ezpi_dev_type)ezpi_gpio_pool[index];}

    std::vector <ezpi_device_digital_op_t> EZPI_GET_OUTPUT_DEVICES() { return ezpi_output_devices; }
    std::vector <ezpi_device_digital_ip_t> EZPI_GET_INPUT_DEVICES() { return ezpi_input_devices; }
    std::vector <ezpi_device_analog_ip_t> EZPI_GET_AINPUT_DEVICES() { return ezpi_analog_input_devices; }
    std::vector <ezpi_device_one_wire_t> EZPI_GET_ONEWIRE_DEVICES() { return ezpi_onewire_devices; }
    std::vector <ezpi_device_I2C_t> EZPI_GET_I2C_DEVICES() { return ezpi_i2c_devices; }
    std::vector <ezpi_device_SPI_t> EZPI_GET_SPI_DEVICES() { return ezpi_spi_devices; }

    EZPI_UINT8 EZPI_GET_DEVICE_COUNT(void) {
        return (EZPI_UINT8)(ezpi_output_devices.size() + \
                ezpi_input_devices.size() + \
                ezpi_analog_input_devices.size() + \
                ezpi_onewire_devices.size() + \
                ezpi_i2c_devices.size() + \
                ezpi_spi_devices.size());
    }


    EZPI_STRING EZPI_GET_DEV_TYPE(ezpi_dev_type d) {return ezpi_device_types_str->at(d);}
    EZPI_STRING EZPI_GET_ITEM_TYPE(ezpi_item_type i) {return ezpi_item_types_str->at(i);}

    void EZPI_SET_FMW_INFO(ezpi_info_t ezlogic_fmw_info) { ezpi_firmware_info = &ezlogic_fmw_info; }

};

#endif // EZLOPI_H
