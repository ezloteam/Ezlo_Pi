
#include "ezlopi.h"

EzPi::EzPi() {

    ezpi_device_types_str = new QStringList;
    ezpi_item_types_str = new QStringList;

    ezpi_device_types_str->append("Restricted");
    ezpi_device_types_str->append("Digital Out");
    ezpi_device_types_str->append("Digital In");
    ezpi_device_types_str->append("Analog In");
    ezpi_device_types_str->append("Analog Out");
    ezpi_device_types_str->append("PWM");
    ezpi_device_types_str->append("UART");
    ezpi_device_types_str->append("Onewire");
    ezpi_device_types_str->append("I2C");
    ezpi_device_types_str->append("SPI");
    ezpi_device_types_str->append("Other");


    ezpi_item_types_str->append("None");
    ezpi_item_types_str->append("LED");
    ezpi_item_types_str->append("Relay");
    ezpi_item_types_str->append("Plug");
    ezpi_item_types_str->append("Button");
    ezpi_item_types_str->append("MPU6050");
    ezpi_item_types_str->append("ADXL345");
    ezpi_item_types_str->append("GY271");
    ezpi_item_types_str->append("MCP4725");
    ezpi_item_types_str->append("GY530");
    ezpi_item_types_str->append("DS1307");
    ezpi_item_types_str->append("MAX30100");
    ezpi_item_types_str->append("BMP280 I2C");
    ezpi_item_types_str->append("BMP280 SPI");
    ezpi_item_types_str->append("LNA219");
    ezpi_item_types_str->append("DHT11");
    ezpi_item_types_str->append("DHT22");
    ezpi_item_types_str->append("POT");
    ezpi_item_types_str->append("Door Sensor");
    ezpi_item_types_str->append("PIR Sensor");

     ezpi_firmware_info = new ezpi_info_t;

}

EzPi::~EzPi() {

}

void EzPi::EZPI_SET_BOARD_TYPE(ezpi_board_type board_type) {

    _ezpi_board_type = board_type;
    switch(_ezpi_board_type) {
    case EZPI_BOARD_TYPE_ESP32_GENERIC:
        ezpi_gpio_pool.resize(EZPI_ESP32_GENERIC_PINOUT_COUNT);
        break;
    case EZPI_BOARD_TYPE_ESP32_S3:
        ezpi_gpio_pool.resize(EZPI_ESP32_S3_PINOUT_COUNT);
        break;
    default:
        break;
    }
}

void EzPi::EZPI_INIT_BOARD(void) {

    switch (_ezpi_board_type) {
        case EZPI_BOARD_TYPE_ESP32_GENERIC:
            // Reference [unofficial]
            // https://randomnerdtutorials.com/esp32-pinout-reference-gpios/
            ezpi_gpio_pool.at(0) = (EZPI_UINT8)EZPI_DEV_TYPE_RESTRICTED;
            ezpi_gpio_pool.at(1) = EZPI_DEV_TYPE_RESTRICTED;
            ezpi_gpio_pool.at(2) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(3) = EZPI_DEV_TYPE_RESTRICTED;
            ezpi_gpio_pool.at(4) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(5) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(6) = EZPI_DEV_TYPE_RESTRICTED;
            ezpi_gpio_pool.at(7) = EZPI_DEV_TYPE_RESTRICTED;
            ezpi_gpio_pool.at(8) = EZPI_DEV_TYPE_RESTRICTED;
            ezpi_gpio_pool.at(9) = EZPI_DEV_TYPE_RESTRICTED;
            ezpi_gpio_pool.at(10) = EZPI_DEV_TYPE_RESTRICTED;
            ezpi_gpio_pool.at(11) = EZPI_DEV_TYPE_RESTRICTED;
            ezpi_gpio_pool.at(12) = EZPI_DEV_TYPE_RESTRICTED;
            ezpi_gpio_pool.at(13) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(14) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(15) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(16) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(17) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(18) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(19) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(20) = EZPI_DEV_TYPE_RESTRICTED;
            ezpi_gpio_pool.at(21) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(22) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(23) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(24) = EZPI_DEV_TYPE_RESTRICTED;
            ezpi_gpio_pool.at(25) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(26) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(27) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(28) = EZPI_DEV_TYPE_RESTRICTED;
            ezpi_gpio_pool.at(29) = EZPI_DEV_TYPE_RESTRICTED;
            ezpi_gpio_pool.at(30) = EZPI_DEV_TYPE_RESTRICTED;
            ezpi_gpio_pool.at(31) = EZPI_DEV_TYPE_RESTRICTED;
            ezpi_gpio_pool.at(32) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(33) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(34) = EZPI_DEV_TYPE_INPUT_ONLY;
            ezpi_gpio_pool.at(35) = EZPI_DEV_TYPE_INPUT_ONLY;
            ezpi_gpio_pool.at(36) = EZPI_DEV_TYPE_INPUT_ONLY;
            ezpi_gpio_pool.at(37) = EZPI_DEV_TYPE_RESTRICTED;
            ezpi_gpio_pool.at(38) = EZPI_DEV_TYPE_RESTRICTED;
            ezpi_gpio_pool.at(39) = EZPI_DEV_TYPE_INPUT_ONLY;
            break;
        case EZPI_BOARD_TYPE_ESP32_S3:
            // Reference
            // https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-reference/peripherals/gpio.html
            ezpi_gpio_pool.at(0) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(1) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(2) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(3) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(4) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(5) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(6) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(7) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(8) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(9) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(10) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(11) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(12) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(13) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(14) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(15) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(16) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(17) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(18) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(19) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(20) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(21) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(22) = EZPI_DEV_TYPE_RESTRICTED;
            ezpi_gpio_pool.at(23) = EZPI_DEV_TYPE_RESTRICTED;
            ezpi_gpio_pool.at(24) = EZPI_DEV_TYPE_RESTRICTED;
            ezpi_gpio_pool.at(25) = EZPI_DEV_TYPE_RESTRICTED;
            ezpi_gpio_pool.at(26) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(27) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(28) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(29) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(30) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(31) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(32) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(33) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(34) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(35) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(36) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(37) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(38) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(39) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(40) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(41) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(42) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(43) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(44) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(45) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(46) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(47) = EZPI_DEV_TYPE_UNCONFIGURED;
            ezpi_gpio_pool.at(48) = EZPI_DEV_TYPE_UNCONFIGURED;
            break;
        default:
            break;
    }
}


ezpi_error_codes_configurator EzPi::EZPI_ADD_OUTPUT_DEVICE(ezpi_device_digital_op_t d) {
    if(ezpi_output_devices.size() >= EZPI_MAX_DEV_DIO) return EZPI_ERROR_REACHED_MAX_DEV;

    if(d.is_ip) EZPI_SET_GPIO_POOL(d.gpio_in, EZPI_DEV_TYPE_DIGITAL_IP);

    EZPI_SET_GPIO_POOL(d.gpio_out, EZPI_DEV_TYPE_DIGITAL_IP);

    ezpi_output_devices.push_back(d);
    return EZPI_SUCCESS;
}

ezpi_error_codes_configurator EzPi::EZPI_ADD_INPUT_DEVICE(ezpi_device_digital_ip_t d) {
    if(ezpi_input_devices.size() >= EZPI_MAX_DEV_DIP) return EZPI_ERROR_REACHED_MAX_DEV;
    EZPI_SET_GPIO_POOL(d.gpio, EZPI_DEV_TYPE_DIGITAL_IP);
    ezpi_input_devices.push_back(d);
    return EZPI_SUCCESS;
}

ezpi_error_codes_configurator EzPi::EZPI_ADD_AINPUT_DEVICE(ezpi_device_analog_ip_t d) {
    if(ezpi_analog_input_devices.size() >= EZPI_MAX_DEV_AIP) return EZPI_ERROR_REACHED_MAX_DEV;
    EZPI_SET_GPIO_POOL(d.gpio, EZPI_DEV_TYPE_ANALOG_IP);
    ezpi_analog_input_devices.push_back(d);
    return EZPI_SUCCESS;
}

ezpi_error_codes_configurator EzPi::EZPI_ADD_ONEWIRE_DEVICE(ezpi_device_one_wire_t d) {
    if(ezpi_onewire_devices.size() >= EZPI_MAX_DEV_ONEWIRE) return EZPI_ERROR_REACHED_MAX_DEV;
    EZPI_SET_GPIO_POOL(d.gpio, EZPI_DEV_TYPE_ONE_WIRE);
    ezpi_onewire_devices.push_back(d);
    return EZPI_SUCCESS;
}

ezpi_error_codes_configurator EzPi::EZPI_ADD_I2C_DEVICE(ezpi_device_I2C_t d) {
    if(ezpi_i2c_devices.size() >= EZPI_MAX_DEV_I2C) return EZPI_ERROR_REACHED_MAX_DEV;
    EZPI_SET_GPIO_POOL(d.gpio_sda, EZPI_DEV_TYPE_I2C);
    EZPI_SET_GPIO_POOL(d.gpio_scl, EZPI_DEV_TYPE_I2C);
    ezpi_i2c_devices.push_back(d);
    return EZPI_SUCCESS;
}

ezpi_error_codes_configurator EzPi::EZPI_ADD_SPI_DEVICE(ezpi_device_SPI_t d) {
    if(ezpi_spi_devices.size() >= EZPI_MAX_DEV_SPI) return EZPI_ERROR_REACHED_MAX_DEV;
    EZPI_SET_GPIO_POOL(d.gpio_miso, EZPI_DEV_TYPE_SPI);
    EZPI_SET_GPIO_POOL(d.gpio_mosi, EZPI_DEV_TYPE_SPI);
    EZPI_SET_GPIO_POOL(d.gpio_sck, EZPI_DEV_TYPE_SPI);
    EZPI_SET_GPIO_POOL(d.gpio_cs, EZPI_DEV_TYPE_DIGITAL_OP);
    ezpi_spi_devices.push_back(d);
    return EZPI_SUCCESS;
}

ezpi_error_codes_configurator EzPi::EZPI_ADD_OTHER_DEVICE(ezpi_device_other_t d) {
    if(ezpi_other_devices.size() >= EZPI_MAX_DEV_OTHER) return EZPI_ERROR_REACHED_MAX_DEV;
    if(d.en_gpio1) EZPI_SET_GPIO_POOL(d.gpio1, EZPI_DEV_TYPE_DIGITAL_OP);
    if(d.en_gpio2) EZPI_SET_GPIO_POOL(d.gpio2, EZPI_DEV_TYPE_DIGITAL_OP);
    if(d.en_gpio3) EZPI_SET_GPIO_POOL(d.gpio3, EZPI_DEV_TYPE_DIGITAL_OP);
    ezpi_other_devices.push_back(d);
    return EZPI_SUCCESS;
}


void EzPi::EZPI_DELETE_OUTPUT_DEVICE(void) {
    ezpi_device_digital_op_t d = ezpi_output_devices.back();
    if(d.is_ip) ezpi_gpio_pool[d.gpio_in] = EZPI_DEV_TYPE_UNCONFIGURED;
    ezpi_gpio_pool[d.gpio_out] = EZPI_DEV_TYPE_UNCONFIGURED;
    ezpi_output_devices.pop_back();
}

void EzPi::EZPI_DELETE_INPUT_DEVICE(void) {

    ezpi_device_digital_ip_t d = ezpi_input_devices.back();
    ezpi_gpio_pool[d.gpio] = EZPI_DEV_TYPE_UNCONFIGURED;
    ezpi_input_devices.pop_back();
}

void EzPi::EZPI_DELETE_AINPUT_DEVICE(void) {
    ezpi_device_analog_ip_t d = ezpi_analog_input_devices.back();
    ezpi_gpio_pool[d.gpio] = EZPI_DEV_TYPE_UNCONFIGURED;
    ezpi_analog_input_devices.pop_back();
}

void EzPi::EZPI_DELETE_ONEWIRE_DEVICE(void) {
    ezpi_device_one_wire_t d = ezpi_onewire_devices.back();
    ezpi_gpio_pool[d.gpio] = EZPI_DEV_TYPE_UNCONFIGURED;
    ezpi_onewire_devices.pop_back();
}

void EzPi::EZPI_DELETE_I2C_DEVICE(void) {
    ezpi_device_I2C_t d = ezpi_i2c_devices.back();
    ezpi_gpio_pool[d.gpio_scl] = EZPI_DEV_TYPE_UNCONFIGURED;
    ezpi_gpio_pool[d.gpio_sda] = EZPI_DEV_TYPE_UNCONFIGURED;
    ezpi_i2c_devices.pop_back();
}

void EzPi::EZPI_DELETE_SPI_DEVICE(void) {
    ezpi_device_SPI_t d = ezpi_spi_devices.back();
    ezpi_gpio_pool[d.gpio_miso] = EZPI_DEV_TYPE_UNCONFIGURED;
    ezpi_gpio_pool[d.gpio_mosi] = EZPI_DEV_TYPE_UNCONFIGURED;
    ezpi_gpio_pool[d.gpio_sck] = EZPI_DEV_TYPE_UNCONFIGURED;
    ezpi_gpio_pool[d.gpio_cs] = EZPI_DEV_TYPE_UNCONFIGURED;
    ezpi_spi_devices.pop_back();
}

void EzPi::EZPI_DELETE_OTHER_DEVICE(void) {
    ezpi_device_other_t d = ezpi_other_devices.back();
    if(d.en_gpio1) ezpi_gpio_pool[d.gpio1] = EZPI_DEV_TYPE_UNCONFIGURED;
    if(d.en_gpio2) ezpi_gpio_pool[d.gpio2] = EZPI_DEV_TYPE_UNCONFIGURED;
    if(d.en_gpio3) ezpi_gpio_pool[d.gpio3] = EZPI_DEV_TYPE_UNCONFIGURED;
    ezpi_other_devices.pop_back();
}
