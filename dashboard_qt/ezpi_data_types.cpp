
#include "ezpi_data_types.h"

EzPi::EzPi():
    _ezpi_board_type(EZPI_BOARD_TYPE_ESP32_GENERIC),
    ezlogic_output_devices_n(0),
    ezlogic_input_devices_n(0),
    ezlogic_onewire_devices_n(0),
    ezlogic_i2c_devices_n(0),
    ezlogic_spi_devices_n(0)
{

    ezlogic_device_types_str = new QStringList;
    ezlogic_item_types_str = new QStringList;

    ezlogic_device_types_str->append("Restricted");
    ezlogic_device_types_str->append("Digital Out");
    ezlogic_device_types_str->append("Digital In");
    ezlogic_device_types_str->append("Analog Out");
    ezlogic_device_types_str->append("Analog In");
    ezlogic_device_types_str->append("PWM");
    ezlogic_device_types_str->append("UART");
    ezlogic_device_types_str->append("Onewire");
    ezlogic_device_types_str->append("I2C");
    ezlogic_device_types_str->append("SPI");


    ezlogic_item_types_str->append("None");
    ezlogic_item_types_str->append("LED");
    ezlogic_item_types_str->append("Relay");
    ezlogic_item_types_str->append("Plug");
    ezlogic_item_types_str->append("Button");
    ezlogic_item_types_str->append("MPU6050");
    ezlogic_item_types_str->append("ADXL345");
    ezlogic_item_types_str->append("GY271");
    ezlogic_item_types_str->append("MCP4725");
    ezlogic_item_types_str->append("GY530");
    ezlogic_item_types_str->append("DS1307");
    ezlogic_item_types_str->append("MAX30100");
    ezlogic_item_types_str->append("BMP280 I2C");
    ezlogic_item_types_str->append("BMP280 SPI");
    ezlogic_item_types_str->append("LNA219");
    ezlogic_item_types_str->append("DHT11");

     ezlogic_firmware_info = new ezlogic_info_t;

}

EzPi::~EzPi() {

}

void EzPi::EZPI_SET_BOARD_TYPE(ezpi_board_type board_type) {

    _ezpi_board_type = board_type;
    switch(_ezpi_board_type) {
    case EZPI_BOARD_TYPE_ESP32_GENERIC:
        ezpi_gpio_pool.resize(EZPI_ESP32_GENERIC_PINOUT_COUNT);
        break;
    case EZPI_BOARD_TYPE_ESP32_C3:
        break;
    case EZPI_BOARD_TYPE_ESP32_S3:
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
        case EZPI_BOARD_TYPE_ESP32_C3:
            break;
        default:
            break;
    }
}


ezpi_error_codes_configurator EzPi::EZPI_ADD_OUTPUT_DEVICE(ezlogic_device_digital_op_t d) {
    if(ezlogic_output_devices.size() >= EZPI_MAX_DEV_DIO) return EZPI_ERROR_REACHED_MAX_DEV;
    ezlogic_output_devices.push_back(d);
    ezlogic_output_devices_n++;
    return EZPI_SUCCESS;
}

ezpi_error_codes_configurator EzPi::EZPI_ADD_INPUT_DEVICE(ezlogic_device_digital_ip_t d) {
    if(ezlogic_input_devices.size() >= EZPI_MAX_DEV_DIP) return EZPI_ERROR_REACHED_MAX_DEV;
    ezlogic_input_devices.push_back(d);
    ezlogic_input_devices_n++;
    return EZPI_SUCCESS;
}

ezpi_error_codes_configurator EzPi::EZPI_ADD_ONEWIRE_DEVICE(ezlogic_device_one_wire_t d) {
    if(ezlogic_onewire_devices.size() >= EZPI_MAX_DEV_ONEWIRE) return EZPI_ERROR_REACHED_MAX_DEV;
    ezlogic_onewire_devices.push_back(d);
    ezlogic_onewire_devices_n++;
    return EZPI_SUCCESS;
}

ezpi_error_codes_configurator EzPi::EZPI_ADD_I2C_DEVICE(ezlogic_device_I2C_t d) {
    if(ezlogic_i2c_devices.size() >= EZPI_MAX_DEV_I2C) return EZPI_ERROR_REACHED_MAX_DEV;
    ezlogic_i2c_devices.push_back(d);
    ezlogic_i2c_devices_n++;
    return EZPI_SUCCESS;
}

ezpi_error_codes_configurator EzPi::EZPI_ADD_SPI_DEVICE(ezlogic_device_SPI_t d) {
    if(ezlogic_spi_devices.size() >= EZPI_MAX_DEV_SPI) return EZPI_ERROR_REACHED_MAX_DEV;
    ezlogic_spi_devices.push_back(d);
    ezlogic_spi_devices_n++;
    return EZPI_SUCCESS;
}
