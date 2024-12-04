#ifndef _EZLOPI_CORE_WEBSOCKET_CLIENT_H_
#define _EZLOPI_CORE_WEBSOCKET_CLIENT_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_log.h"
#include "../../build/config/sdkconfig.h"
#include "cjext.h"

#include "ezlopi_core_errors.h"

#include "esp_websocket_client.h"

ezlopi_error_t ezlopi_websocket_client_send(esp_websocket_client_handle_t client, char *data, uint32_t len);
// esp_websocket_client_handle_t ezlopi_websocket_client_init(cJSON *uri, void (*msg_upcall)(const char *, uint32_t), void (*connection_upcall)(bool connection));
// esp_websocket_client_handle_t ezlopi_websocket_client_init(cJSON *uri, int (*msg_upcall)(char *, uint32_t, time_t time_ms), void (*connection_upcall)(bool connection));
esp_websocket_client_handle_t ezlopi_websocket_client_init(cJSON *uri, int (*msg_upcall)(char *, uint32_t, time_t time_ms), void (*connection_upcall)(bool connected),
                                                           char *ca_cert, char *ssl_private, char *ssl_shared);
bool ezlopi_websocket_client_is_connected(esp_websocket_client_handle_t client);
void ezlopi_websocket_client_kill(esp_websocket_client_handle_t client);

#if 0
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
    esp_websocket_client_handle_t websocket_app_start(std::string& uri, void (*upcall)(const char*, uint32_t));

    /**
     * @brief      Write textual data to the WebSocket connection (data send with WS OPCODE=01, i.e. text)
     *
     * @param[in]  string -> text to send
     *
     * @return
     *     - Number of data was sent
     *     - (-1) if any errors
     */
    int send(std::string& _str);
    int send(char* c_str);

    bool is_connected(void);

    /**
     * @brief Kills the current websocket instance and set to null
     *
     */
    void websocket_client_kill(void);
};
#endif

#endif // _EZLOPI_CORE_WEBSOCKET_CLIENT_H_