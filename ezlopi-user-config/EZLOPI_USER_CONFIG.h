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

/////////////////////////////////////////////////////////////
/////////////// Disable user configs here //////////////////
////////////////////////////////////////////////////////////

#undef EZPI_USR_CONFIG_ENABLE_HEARTBIT_LED
#undef EZPI_CORE_ENABLE_ETH
#undef EZPI_DEV_TYPE_GENERIC
// #undef EZPI_SERV_ENABLE_MESHBOTS
#undef EZPI_UTIL_ENABLE_TRACE

#endif // _EZLOPI_USER_CONFIG_H_