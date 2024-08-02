/**
 * @file ezlopi_core_websocket_client.h
 * @author Krishna Kumar Sah
 * @brief
 * @version 1.1
 * @date July 29, 2023
 *
 * @copyright Copyright (c) 2023
 *
 * @author Krishna Kumar Sah (work.krishnasah@gmail.com)
 * @date
 *
 */

#ifndef _EZLOPI_CORE_WEBSOCKET_CLIENT_H_
#define _EZLOPI_CORE_WEBSOCKET_CLIENT_H_

#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_CORE_IDF_WSC_LIB

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_log.h"
#include "cjext.h"

#include "esp_websocket_client.h"

int ezlopi_websocket_client_send(char* data, uint32_t len);
esp_websocket_client_handle_t ezlopi_websocket_client_init(cJSON* uri, void (*msg_upcall)(const char*, uint32_t), void (*connection_upcall)(bool connection));
bool ezlopi_websocket_client_is_connected(void);
void ezlopi_websocket_client_kill(void);

#endif // CONFIG_EZPI_CORE_IDF_WSC_LIB

#endif // _EZLOPI_CORE_WEBSOCKET_CLIENT_H_