#ifndef _EZLOPI_USER_CONFIG_H_
#define _EZLOPI_USER_CONFIG_H_

#if 1
#include "../build/config/sdkconfig.h"
#include "ezlopi_util_heap.h"

#if defined(CONFIG_EZPI_DISTRO_FULL_OPTION)
#define CONFIG_EZPI_DISTRO_NAME "EZPI_DISTRO_FULL_OPTION"
#elif defined(CONFIG_EZLOPI_DISTRO_LOCAL)
#define CONFIG_EZPI_DISTRO_NAME "EZLOPI_DISTRO_LOCAL"
#elif defined(CONFIG_EZLOPI_DISTRO_LOCAL_MESHBOT)
#define CONFIG_EZPI_DISTRO_NAME "EZLOPI_DISTRO_LOCAL_MESHBOT"
#elif defined(CONFIG_EZLOPI_DISTRO_CLOUD)
#define CONFIG_EZPI_DISTRO_NAME "EZLOPI_DISTRO_CLOUD"
#elif defined(CONFIG_EZLOPI_DISTRO_CLOUD_MESHBOT)
#define CONFIG_EZPI_DISTRO_NAME "EZLOPI_DISTRO_CLOUD_MESHBOT"
#elif defined(CONFIG_EZLOPI_DISTRO_BLE_CLOUD)
#define CONFIG_EZPI_DISTRO_NAME "EZLOPI_DISTRO_BLE_CLOUD"
#elif defined(CONFIG_EZLOPI_DISTRO_WIFI_HUB)
#define CONFIG_EZPI_DISTRO_NAME "EZLOPI_DISTRO_WIFI_HUB"
#elif defined(CONFIG_EZLOPI_DISTRO_BLE_HUB)
#define CONFIG_EZPI_DISTRO_NAME "EZLOPI_DISTRO_BLE_HUB"
#elif defined(CONFIG_EZLOPI_DISTRO_MINIMAL)
#define CONFIG_EZPI_DISTRO_NAME "EZLOPI_DISTRO_MINIMAL"
#else
#define CONFIG_EZPI_DISTRO_NAME "EZLOPI_DISTRO_CUSTOM"
#endif

#if defined(CONFIG_EZPI_HEAP_ENABLE)
#ifdef malloc
#undef malloc
#endif
#define malloc(x) ezlopi_util_heap_malloc(x, __FILENAME__, __LINE__)

#ifdef calloc
#undef calloc
#endif
#define calloc(x, y) ezlopi_util_heap_calloc(x, y, __FILENAME__, __LINE__)

#ifdef free
#undef free
#endif
#define free(x) ezlopi_util_heap_free(x, __FILENAME__, __LINE__)

#ifdef realloc
#undef realloc
#endif
#define realloc(x, y) ezlopi_util_heap_realloc(x, y, __FILENAME__, __LINE__)

#endif



#define EZLOPI_SERIAL_API_VERSION "1.0.0"

#else
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
#endif

#endif // _EZLOPI_USER_CONFIG_H_