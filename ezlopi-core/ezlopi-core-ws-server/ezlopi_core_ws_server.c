#include <string.h>

#include <lwip/api.h>
#include <lwip/err.h>
#include <lwip/udp.h>
#include <lwip/tcpip.h>
#include <lwip/netbuf.h>
#include <sha/sha_dma.h>
#include <mbedtls/base64.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "ezlopi_util_uuid.h"
#include "ezlopi_util_trace.h"

#include "ezlopi_core_ws_server.h"
#include "ezlopi_core_event_group.h"
#include "ezlopi_core_ws_server_conn.h"
#include "ezlopi_core_ws_server_config.h"

/**
 * @brief
 *
 *             0              1               2              3
 *     _______________ _______________ _______________ _______________
 *     0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7
 *    +-+-+-+-+-------+-+-------------+-------------------------------+
 *    |F|R|R|R| opcode|M| Payload len |    Extended payload length    |
 *    |I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
 *    |N|V|V|V|       |S|             |   (if payload len==126/127)   |
 *    | |1|2|3|       |K|             |                               |
 *    +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
 *    |     Extended payload length continued, if payload len == 127  |
 *    + - - - - - - - - - - - - - - - +-------------------------------+
 *    |                               |Masking-key, if MASK set to 1  |
 *    +-------------------------------+-------------------------------+
 *    | Masking-key (continued)       |          Payload Data         |
 *    +-------------------------------- - - - - - - - - - - - - - - - +
 *    :                     Payload Data continued ...                :
 *    + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
 *    |                     Payload Data continued ...                |
 *    +---------------------------------------------------------------+
 *    for reference: https://datatracker.ietf.org/doc/html/rfc6455 -> page no. 28 (i.e. Base Framing Protocol)
 */

typedef enum e_ws_opcode
{
    WS_OPCODE_CONTINUOUS_FRAME = 0x00,
    WS_OPCODE_TEXT_FRAME,
    WS_OPCODE_BINARY_FRAME,

    WS_OPCODE_CONNECTION_CLOSE = 0x08,
    WS_OPCODE_PING_FRAME = 0x09,
    WS_OPCODE_PONG_FRAME = 0x0A,
} e_ws_opcode_t;

typedef struct s_ws_header_frame
{
    // Defines the interpretation of the "Payload data".
    uint8_t opcode : 4;

    // MUST be 0 unless an extension is negotiated that defines meanings for non-zero values.
    uint8_t rsv1 : 1;
    uint8_t rsv2 : 1;
    uint8_t rsv3 : 1;

    // Indicates that this is the final fragment in a message.  The first fragment MAY also be the final fragment.
    uint8_t fin : 1;

    // Payload length:  7 bits, 7+16 bits, or 7+64 bits
    // range 0 to 125, is payload length in 7 bits
    // if payload_len == 126, following 2 bytes interpreted as a 16-bit payload length
    // if payload_len == 127, following 8 bytes interpreted as a 64-bit payload length
    uint8_t payload_len : 7;

    // Defines whether the "Payload data" is masked.
    uint8_t mask : 1;
} s_ws_header_frame_t;

static const char *gsc_sec_key_name = "Sec-WebSocket-Key:";
static const char *gsc_wss_guid = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
static TaskHandle_t gs_task_handle = NULL;

static void __server_init(void);
static void __server_process(void *pv);
static int __ws_data_handle(s_ws_server_connections_t *ws_conn);
static int __new_connection_handsake(s_ws_server_connections_t *ws_conn);

static int __process_payload(s_ws_header_frame_t *header_frame, uint8_t *rx_data);
static int __process_extended_payload_2(s_ws_header_frame_t *header_frame, uint8_t *rx_data);
static int __process_extended_payload_8(s_ws_header_frame_t *header_frame, uint8_t *rx_data);

void ezlopi_core_ws_server_deinit(void)
{
    if (gs_task_handle)
    {
        vTaskDelete(gs_task_handle);
    }

    s_ws_server_connections_t *curr_con = ezlopi_core_ws_server_conn_get_head();
}

void ezlopi_core_ws_server_init(void)
{
    if (NULL == gs_task_handle)
    {
        xTaskCreate(__server_process, "ws-server-process", 2048, NULL, 4, &gs_task_handle);
    }
}

static void __connection_process(void *pv)
{
    s_ws_server_connections_t *ws_conn = (s_ws_server_connections_t *)pv;

    if (ws_conn)
    {
        if (ws_conn->net_conn)
        {
            int ret = __new_connection_handsake(ws_conn);

            if (ret)
            {
                do
                {
                    __ws_data_handle(ws_conn);

                    vTaskDelay(1);
                } while (1);
            }

            netconn_delete(ws_conn->net_conn);
            ws_conn->net_conn = NULL;
            ws_conn->task_handle = NULL;
        }
    }
    else
    {
        TRACE_E("ws-conn is NULL");
    }

    vTaskDelete(NULL);
}

static void __server_process(void *pv)
{
    ezlopi_event_group_wait_for_event(EZLOPI_EVENT_WIFI_CONNECTED, portMAX_DELAY, false);

    struct netconn *listen_con = netconn_new(NETCONN_TCP);

    if (listen_con)
    {
        netconn_bind(listen_con, IP4_ADDR_ANY, 80);
        err_t err = netconn_listen(listen_con);
        if (ERR_OK == err)
        {
            while (1)
            {
                struct netconn *new_con = NULL;

                TRACE_D("waiting for new connection...");
                err = netconn_accept(listen_con, &new_con);
                if ((ERR_OK == err) && new_con)
                {
                    ip_addr_t conn_ip;
                    uint16_t conn_port = 0;
                    s_ws_server_connections_t *ws_conn = ezlopi_core_ws_server_conn_add_ws_conn(new_con);

                    netconn_getaddr(new_con, &conn_ip, &conn_port, 0);
                    TRACE_D("connection-request from: %s:%u", ipaddr_ntoa(&conn_ip), conn_port);

                    if (ws_conn)
                    {
                        xTaskCreate(__connection_process, "ws-connection", 4 * 1024, ws_conn, 5, &ws_conn->task_handle);
                    }
                    else
                    {
                        netconn_delete(new_con);
                    }
                }
            }
        }
        else
        {
            TRACE_E("error[%d]: %s", err, lwip_strerr(err));
            netconn_delete(listen_con);
            TRACE_E("failed");
        }
    }
    else
    {
        TRACE_E("FAILED");
    }

    gs_task_handle = NULL;
    vTaskDelete(NULL);
}

static int __new_connection_handsake(s_ws_server_connections_t *ws_conn)
{
    int ret = 0;

    if (ws_conn && ws_conn->net_conn)
    {
        struct netbuf *net_buffer = NULL;
        err_t err = netconn_recv(ws_conn->net_conn, &net_buffer);

        if (NULL != net_buffer)
        {
            char *buf_data = NULL;
            uint16_t data_size = 0;

            netbuf_data(net_buffer, &buf_data, &data_size);

            if (buf_data)
            {
                char *client_sec_key_start = strstr(buf_data, gsc_sec_key_name);
                if (client_sec_key_start)
                {
                    char concated_strings[80];
                    client_sec_key_start += strlen(gsc_sec_key_name) + 1;
                    uint32_t client_sec_key_len = strstr(client_sec_key_start, "\r\n") - client_sec_key_start;

                    ezlopi_util_uuid_generate_random(ws_conn->uuid);
                    uint32_t tmp_len = snprintf(concated_strings, sizeof(concated_strings), "%.*s%s", client_sec_key_len, client_sec_key_start, gsc_wss_guid);

                    uint8_t hash_of_cat[20];
                    memset(hash_of_cat, 0, sizeof(hash_of_cat));
                    esp_sha(SHA1, (uint8_t *)concated_strings, strlen(concated_strings), (uint8_t *)hash_of_cat);

                    uint32_t o_len = 0;
                    char base64_data_of_cat[32];
                    memset(base64_data_of_cat, 0, sizeof(base64_data_of_cat));
                    mbedtls_base64_encode((uint8_t *)base64_data_of_cat, sizeof(base64_data_of_cat), &o_len, hash_of_cat, sizeof(hash_of_cat));

                    static const char *sc_handsake_response_frmt =
                        "HTTP/1.1 101 Switching Protocols \r\n"
                        "Upgrade: websocket\r\n"
                        "Connection: Upgrade\r\n"
                        "Sec-WebSocket-Accept: %.*s\r\n\r\n";

                    char *handshake_response = malloc(256);
                    if (handshake_response)
                    {
                        snprintf(handshake_response, 256, sc_handsake_response_frmt, sizeof(base64_data_of_cat), base64_data_of_cat);
                        netconn_write(ws_conn->net_conn, handshake_response, strlen(handshake_response), NETCONN_COPY);

                        TRACE_D("handshake_response sent:\r\n%s", handshake_response);
                        free(handshake_response);
                        ret = 1;
                    }
                }
            }

            netbuf_delete(net_buffer);
        }
        else
        {
            TRACE_E("Here");
        }
    }
    else
    {
        TRACE_E("Here");
    }

    return ret;
}

static int __ws_data_handle(s_ws_server_connections_t *ws_conn)
{
    int ret = 0;
    if (ws_conn)
    {
        TRACE_D("Here");
        struct netbuf *net_buffer = NULL;
        err_t err = netconn_recv(ws_conn->net_conn, &net_buffer);
        TRACE_W("err: %s", lwip_strerr(err));

        if (net_buffer)
        {
            uint8_t *rx_data = NULL;
            uint32_t data_len = 0;

            netbuf_first(net_buffer);
            netbuf_data(net_buffer, &rx_data, &data_len);
            net_buffer = NULL;

            if (rx_data)
            {
                dump("rx_data", rx_data, 0, data_len);

                uint8_t *payload_start = NULL;
                s_ws_header_frame_t *header_frame = (s_ws_header_frame_t *)rx_data;

                if (header_frame->payload_len <= 125)
                {
                    __process_payload(header_frame, rx_data);
                }
                else if (header_frame == 126)
                {
                    __process_extended_payload_2(header_frame, rx_data);
                }
                else if (header_frame == 127)
                {
                    __process_extended_payload_8(header_frame, rx_data);
                }

                switch (header_frame->opcode)
                {
                case WS_OPCODE_TEXT_FRAME:
                {
                    TRACE_I("text frame");
                    break;
                }
                case WS_OPCODE_CONNECTION_CLOSE:
                {
                    TRACE_I("connection close");
                    break;
                }
                case WS_OPCODE_CONTINUOUS_FRAME:
                {
                    TRACE_I("continuous frame");
                    break;
                }
                case WS_OPCODE_BINARY_FRAME:
                {
                    TRACE_I("binary frame");
                    break;
                }
                case WS_OPCODE_PING_FRAME:
                {
                    TRACE_I("ping");
                }
                case WS_OPCODE_PONG_FRAME:
                {
                    TRACE_I("pong");
                    break;
                }
                default:
                {
                    TRACE_E("unknown upcode");
                    break;
                }
                }
            }

            netbuf_delete(net_buffer);
        }
        else
        {
            TRACE_E("netbuf is null");
        }
    }

    return ret;
}

static int __process_payload(s_ws_header_frame_t *header_frame, uint8_t *rx_data)
{
    int ret = 0;

    if (header_frame && rx_data)
    {
        uint32_t data_len = (uint32_t)header_frame->payload_len;
        uint8_t *payload_start = rx_data + 2;
        TRACE_D("payload-len: %u", data_len);
    }

    return ret;
}

static int __process_extended_payload_2(s_ws_header_frame_t *header_frame, uint8_t *rx_data)
{
    int ret = 0;
    if (header_frame && rx_data)
    {
        uint16_t data_len = (uint16_t)(rx_data + 2);
        uint8_t *payload_start = rx_data + 4;
        TRACE_D("payload-len: %u", data_len);
    }
    return ret;
}

static int __process_extended_payload_8(s_ws_header_frame_t *header_frame, uint8_t *rx_data)
{
    int ret = 0;
    if (header_frame && rx_data)
    {
        uint64_t data_len = ((uint64_t)(rx_data + 2));
        uint8_t *payload_start = rx_data + 10;
        TRACE_D("payload-len: %llu", data_len);
    }
    return ret;
}
