#ifndef __WEBSOCKET_CLIENT_H__
#define __WEBSOCKET_CLIENT_H__

#include <string>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_log.h"
#include "protocol_examples_common.h"
#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
// #include "memory_map.h"

#include "esp_websocket_client.h"
#include "esp_event.h"
// #include "certificates.h"

class websocket_client
{
private:
public:
    /**
     * @brief Initiates the websocket client and return the object of the client
     * @brief Right now instanciate only one client. If called 'websocket_app_start' function - returns running client instance
     *
     * @return esp_websocket_client_handle_t* object of the initiated client
     */
    esp_websocket_client_handle_t websocket_app_start(std::string &uri, void (*upcall)(const char *, uint32_t));

    /**
     * @brief      Write textual data to the WebSocket connection (data send with WS OPCODE=01, i.e. text)
     *
     * @param[in]  string -> text to send
     *
     * @return
     *     - Number of data was sent
     *     - (-1) if any errors
     */
    int send(std::string &_str);
    int send(char *c_str);

    bool is_connected(void);

    /**
     * @brief Kills the current websocket instance and set to null
     *
     */
    void websocket_client_kill(void);
};

#endif // __WEBSOCKET_CLIENT_H__