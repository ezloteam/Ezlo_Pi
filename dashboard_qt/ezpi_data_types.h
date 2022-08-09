#ifndef EZPI_DATA_TYPES_H
#define EZPI_DATA_TYPES_H

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


#define     MAX_DEVICES         10
#define     MAX_GPIOS           28

#define     EZPI_ID             1655702685UL        // Random Ezpi ID
#define     EZPI_DEFAULT_BAUD   115200UL
#define     EZPI_MAX_GPIOS      28

#define     EZPI_MAX_DEVICES    10

#define     EZPI_MAX_DEV_DIO        10
#define     EZPI_MAX_DEV_DIP        10
#define     EZPI_MAX_DEV_ONEWIRE    5
#define     EZPI_MAX_DEV_I2C        5
#define     EZPI_MAX_DEV_SPI        3

#define     MAX_DEV             EZPI_MAX_DEVICES

#define     EZPI_ESP32_GENERIC_PINOUT_COUNT                     40

#define     SIZE_EZPI_OFFSET_CONN_ID_0                          0X0000
#define     SIZE_EZPI_OFFSET_CONN_ID_1                          0X7000
#define     SIZE_EZPI_OFFSET_HUB_ID_0                           0XE000
#define     SIZE_EZPI_OFFSET_HUB_ID_1                           0XF000

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
    EZPI_ITEM_TYPE_LED = 1,
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

enum ezpi_board_type {
    EZPI_BOARD_TYPE_NONE,
    EZPI_BOARD_TYPE_ESP32_GENERIC,
    EZPI_BOARD_TYPE_ESP32_C3,
    EZPI_BOARD_TYPE_ESP32_S3,
    EZPI_BOARD_TYPE_TOTAL
};

typedef struct ezlogic_device_digital_op {
    EZPI_UINT8 dev_type;
    EZPI_STRING dev_name;
    EZPI_UINT16 id_room;
    EZPI_UINT16 id_item;
    EZPI_BOOL val_ip;
    EZPI_BOOL val_op;
    EZPI_UINT8 gpio_in;
    EZPI_UINT8 gpio_out;
    EZPI_BOOL is_ip;
    EZPI_BOOL ip_inv;
    EZPI_BOOL pullup_ip;
    EZPI_BOOL pullup_op;
    EZPI_BOOL op_inv;
} ezlogic_device_digital_op_t;

typedef struct ezlogic_device_digital_ip {
    EZPI_UINT8 dev_type;
    EZPI_STRING dev_name;
    EZPI_UINT16 id_room;
    EZPI_UINT16 id_item;
    EZPI_BOOL val_ip;
    EZPI_UINT8 gpio;
    EZPI_BOOL pull_up;
    EZPI_BOOL logic_inv;
} ezlogic_device_digital_ip_t;

typedef struct ezlogic_device_one_wire {
    EZPI_UINT8 dev_type;
    EZPI_STRING dev_name;
    EZPI_UINT16 id_room;
    EZPI_UINT16 id_item;
    EZPI_BOOL val_ip;
    EZPI_BOOL pull_up;
    EZPI_UINT8 gpio;
} ezlogic_device_one_wire_t;

typedef struct ezlogic_device_I2C {
    EZPI_UINT8 dev_type;
    EZPI_STRING dev_name;
    EZPI_UINT16 id_room;
    EZPI_UINT16 id_item;
    EZPI_UINT8 gpio_sda;
    EZPI_UINT8 gpio_scl;
    EZPI_BOOL pullup_scl;
    EZPI_BOOL pullup_sda;
    EZPI_UINT8 slave_addr;
} ezlogic_device_I2C_t;

typedef struct ezlogic_device_SPI {
    EZPI_UINT8 dev_type;
    EZPI_STRING dev_name;
    EZPI_UINT16 id_room;
    EZPI_UINT16 id_item;
    EZPI_UINT8 gpio_miso;
    EZPI_UINT8 gpio_mosi;
    EZPI_UINT8 gpio_sck;
    EZPI_UINT8 gpio_cs;
} ezlogic_device_SPI_t;

struct device_t {
    char dev_id[SIZE_DEVICE_ID];
    unsigned char dev_type;
    char Name[SIZE_DEVICE_FNAME];
    char roomId[SIZE_ROOM_ID];
    char id_i[SIZE_ID_I];           // Item ID
    bool input_vol;
    bool out_vol;
    unsigned char input_gpio;
    unsigned char out_gpio;
    bool is_input;
    bool checkBox_gpio_in_logic_type;       // Input Inv
    bool checkBox_gpio_out_logic_type;      // Out put Inv
    bool is_meter;
};

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
    std::vector <ezlogic_device_digital_op_t> ezlogic_output_devices;
    std::vector <ezlogic_device_digital_ip_t> ezlogic_input_devices;
    std::vector <ezlogic_device_one_wire_t> ezlogic_onewire_devices;
    std::vector <ezlogic_device_I2C_t> ezlogic_i2c_devices;
    std::vector <ezlogic_device_SPI_t> ezlogic_spi_devices;

public:
    EzPi();
    ~EzPi();
    void EZPI_INIT_BOARD(void);
    void EZPI_SET_BOARD_TYPE(ezpi_board_type board_type);
    ezpi_error_codes_configurator EZPI_ADD_OUTPUT_DEVICE(ezlogic_device_digital_op_t d);
    ezpi_error_codes_configurator EZPI_ADD_INPUT_DEVICE(ezlogic_device_digital_ip_t d);
    ezpi_error_codes_configurator EZPI_ADD_ONEWIRE_DEVICE(ezlogic_device_one_wire_t d);
    ezpi_error_codes_configurator EZPI_ADD_I2C_DEVICE(ezlogic_device_I2C_t d);
    ezpi_error_codes_configurator EZPI_ADD_SPI_DEVICE(ezlogic_device_SPI_t d);

    ezpi_board_type EZPI_GET_BOARD_TYPE(void) {return _ezpi_board_type;}
    std::vector<EZPI_UINT8> EZPI_GET_GPIO_POOL(void) {return ezpi_gpio_pool;}
    void EZPI_SET_GPIO_POOL(EZPI_UINT8 index, ezpi_dev_type d) { ezpi_gpio_pool.at(index) = d;}
    ezpi_dev_type EZPI_GET_GPIO_POOL(EZPI_UINT8 index) {return (ezpi_dev_type)ezpi_gpio_pool[index];}

};

#endif // EZPI_DATA_TYPES_H
