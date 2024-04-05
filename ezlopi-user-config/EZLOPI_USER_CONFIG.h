#ifndef _EZLOPI_USER_CONFIG_H_
#define _EZLOPI_USER_CONFIG_H_

/////////////////////////////////////////////////////////////
/////////////// Do not change here //////////////////
////////////////////////////////////////////////////////////

// User Configs
#define EZPI_USR_CONFIG_ENABLE_HEARTBIT_LED
#define EZPI_USR_CONFIG_ENABLE_LOG
#define EZPI_DEV_TYPE_GENERIC 1

// Util
#define EZPI_UTIL_ENABLE_TRACE 1

// Core
#define EZPI_CORE_ENABLE_ETH

// Cloud

// Service
#define EZPI_SERV_ENABLE_MESHBOTS

// EzloPi BLE Service Pass key 
#define EZLOPI_BLE_ENALBE_PASSKEY 0
#define EZLOPI_BLE_ENALBE_PAIRING 1

#define EZLOPI_SERIAL_API_VERSION "1.0.0"
#define EZLOPI_BLE_API_VERSION "1.0.0"

// EzloPi WiFi Configuration 
#define EZLOPI_WIFI_CONNECT_TIMEOUT 5000
#define EZLOPI_WIFI_MIN_PASS_CHAR 8
#define EZLOPI_WIFI_CONN_ATTEMPT_INTERVAL 5000
#define EZLOPI_WIFI_CONN_RETRY_ATTEMPT 2


// EzloPi Default Serial Configuartion 
#define EZPI_SERV_UART_BAUD_DEFAULT      (uint32_t)115200
#define EZPI_SERV_UART_PARITY_DEFAULT      (uint32_t)UART_PARITY_DISABLE
#define EZPI_SERV_UART_START_BIT_DEFAULT      (uint32_t)0
#define EZPI_SERV_UART_STOP_BIT_DEFAULT      (uint32_t)1
#define EZPI_SERV_UART_FRAME_SIZE_DEFAULT      (uint32_t)8
#define EZPI_SERV_UART_FLOW_CTRL_DEFAULT      (uint32_t)UART_HW_FLOWCTRL_DISABLE

#define EZPI_SERV_UART_RX_BUFFER_SIZE (uint32_t)3096

#if defined(CONFIG_IDF_TARGET_ESP32)
#define EZPI_SERV_UART_TXD_PIN (GPIO_NUM_1)
#define EZPI_SERV_UART_RXD_PIN (GPIO_NUM_3)
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
#define EZPI_SERV_UART_TXD_PIN (GPIO_NUM_21)
#define EZPI_SERV_UART_RXD_PIN (GPIO_NUM_20)
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
#define EZPI_SERV_UART_TXD_PIN (GPIO_NUM_43)
#define EZPI_SERV_UART_RXD_PIN (GPIO_NUM_44)
#endif

/////////////////////////////////////////////////////////////
/////////////// Disable user configs here //////////////////
////////////////////////////////////////////////////////////

#undef EZPI_USR_CONFIG_ENABLE_HEARTBIT_LED
#undef EZPI_CORE_ENABLE_ETH
#undef EZPI_DEV_TYPE_GENERIC
// #undef EZPI_SERV_ENABLE_MESHBOTS
#undef EZPI_UTIL_ENABLE_TRACE

#endif // _EZLOPI_USER_CONFIG_H_