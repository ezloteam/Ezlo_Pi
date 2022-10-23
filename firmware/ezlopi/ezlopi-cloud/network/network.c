#include <string.h>
#include "esp_wifi.h"
#include "esp_netif_types.h"
#include "network.h"
#include "ezlopi_wifi.h"
#include "trace.h"
#include "frozen.h"

static const char *network_1_start = "{\"error\":null,\"msg_id\":%d,\"id\":\"%.*s\",\"method\":\"hub.network.get\",\"result\":{\"interfaces\":[";
static const char *network_1_net = "{\"_id\":\"wifi\",\"enabled\":\"auto\",\"hwaddr\":\"%s\",\"internetAvailable\":true,"
                                   "\"ipv4\":{\"gateway\":\"" IPSTR "\",\"ip\":\"" IPSTR "\",\"mask\":\"" IPSTR "\",\"mode\":\"dhcp\"},"
                                   "\"network\":\"wan\",\"status\":\"up\",\"type\":\"wifi\"}";
static const char *network_1_end = "]},\"sender\":%.*s}";

char *network_get(const char *data, uint32_t len, struct json_token *method_tok, uint32_t msg_count)
{
    uint32_t buf_len = 2048;
    char *send_buf = (char *)malloc(buf_len);

    if (send_buf)
    {
        char mac_str[20];
        uint8_t mac_addr[6];
        esp_netif_ip_info_t *ip_info = ezlopi_wifi_get_ip_infos();

        esp_read_mac(mac_addr, ESP_MAC_WIFI_STA);
        snprintf(mac_str, sizeof(mac_str), "%02x:%02x:%02x:%02x:%02x:%02x",
                 mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

        struct json_token msg_id = JSON_INVALID_TOKEN;
        json_scanf(data, len, "{id: %T}", &msg_id);

        struct json_token sender = JSON_INVALID_TOKEN;
        int sender_state = json_scanf(data, len, "{sender: %T}", &sender);

        memset(send_buf, 0, buf_len);

        snprintf(send_buf, buf_len, network_1_start, msg_count, msg_id.len, msg_id.ptr);
        snprintf(send_buf + strlen(send_buf), buf_len - strlen(send_buf), network_1_net, mac_str,
                 IP2STR(&ip_info->gw),       // ip4addr_ntoa((const ip4_addr_t *)&ip_info->gw.addr),
                 IP2STR(&ip_info->ip),       // ip4addr_ntoa((const ip4_addr_t *)&ip_info->ip.addr),
                 IP2STR(&ip_info->netmask)); // ip4addr_ntoa((const ip4_addr_t *)&ip_info->netmask.addr));

        snprintf(send_buf + strlen(send_buf), buf_len - strlen(send_buf), network_1_end, sender_state ? sender.len : 2, sender_state ? sender.ptr : "{}");

        TRACE_B(">> WS Tx - '%.*s' [%d]\n\r%s", method_tok->len, method_tok->ptr, strlen(send_buf), send_buf);
    }

    return send_buf;
}