#include "../../build/config/sdkconfig.h"
#ifdef CONFIG_EZPI_ENABLE_PING

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"

#include "ezlopi_util_trace.h"

#include "./ezlopi-ping/ping_sock.h"

#include "ezlopi_core_ping.h"

static uint32_t __ping_fail_count = 0;
static esp_ping_handle_t __ping_handle = NULL;
static e_ping_status_t __ping_status = EZLOPI_PING_STATUS_UNKNOWN;

static void __on_ping_end(esp_ping_handle_t hdl, void* args);
static void __on_ping_success(esp_ping_handle_t hdl, void* args);
static void __on_ping_timeout(esp_ping_handle_t hdl, void* args);

e_ping_status_t ezlopi_ping_get_internet_status(void)
{
    return __ping_status;
}

void ezlopi_ping_init(void)
{
    ezlopi_ping_config_t config = ESP_PING_DEFAULT_CONFIG();

    config.count = 0; // 0 : run forever
    config.timeout_ms = 2000;
    config.interval_ms = 10000;

    // parse IP address
    ip_addr_t target_addr;
    struct addrinfo hint;
    struct addrinfo* res = NULL;
    memset(&hint, 0, sizeof(hint));
    memset(&target_addr, 0, sizeof(target_addr));

    /* convert domain name to IP address */
    if (getaddrinfo("1.1.1.1", NULL, &hint, &res) != 0)
    {
        TRACE_D("ping_handle: unknown host '1.1.1.1'.");
        return;
    }

    if (res->ai_family == AF_INET)
    {
        struct in_addr addr4 = ((struct sockaddr_in*)(res->ai_addr))->sin_addr;
        inet_addr_to_ip4addr(ip_2_ip4(&target_addr), &addr4);
    }
    else
    {
        struct in6_addr addr6 = ((struct sockaddr_in6*)(res->ai_addr))->sin6_addr;
        inet6_addr_to_ip6addr(ip_2_ip6(&target_addr), &addr6);
    }

    freeaddrinfo(res);

    config.target_addr = target_addr;
    config.task_stack_size = 4096;

    /* set callback functions */
    ezlopi_ping_callbacks_t cbs = {
        .on_ping_success = __on_ping_success,
        .on_ping_timeout = __on_ping_timeout,
        .on_ping_end = __on_ping_end,
        .cb_args = NULL };

    ezlopi_ping_new_session(&config, &cbs, &__ping_handle);
    ezlopi_ping_start_by_handle(__ping_handle);

    return;
}

void ezlopi_ping_stop(void)
{
    if (__ping_handle)
    {
        ezlopi_ping_stop_by_handle(__ping_handle);
        __ping_status = EZLOPI_PING_STATUS_UNKNOWN;
    }
}

static void __on_ping_success(esp_ping_handle_t hdl, void* args)
{
    uint8_t ttl;
    uint16_t seqno;
    uint32_t elapsed_time, recv_len;
    ip_addr_t target_addr;
    ezlopi_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
    ezlopi_ping_get_profile(hdl, ESP_PING_PROF_TTL, &ttl, sizeof(ttl));
    ezlopi_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
    ezlopi_ping_get_profile(hdl, ESP_PING_PROF_SIZE, &recv_len, sizeof(recv_len));
    ezlopi_ping_get_profile(hdl, ESP_PING_PROF_TIMEGAP, &elapsed_time, sizeof(elapsed_time));
    TRACE_I("%d bytes from %s icmp_seq=%d ttl=%d time=%d ms\n",
        recv_len, inet_ntoa(target_addr.u_addr.ip4), seqno, ttl, elapsed_time);
    __ping_fail_count = 0;
    __ping_status = EZLOPI_PING_STATUS_LIVE;
}

static void __on_ping_timeout(esp_ping_handle_t hdl, void* args)
{
    uint16_t seqno;
    ip_addr_t target_addr;
    ezlopi_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
    ezlopi_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
    TRACE_W("From %s icmp_seq=%d timeout\n", inet_ntoa(target_addr.u_addr.ip4), seqno);

    __ping_fail_count++;
    if (__ping_fail_count > 2)
    {
        __ping_status = EZLOPI_PING_STATUS_DISCONNECTED;
    }
}

static void __on_ping_end(esp_ping_handle_t hdl, void* args)
{
    ip_addr_t target_addr;
    uint32_t transmitted;
    uint32_t received;
    uint32_t total_time_ms;

    ezlopi_ping_get_profile(hdl, ESP_PING_PROF_REQUEST, &transmitted, sizeof(transmitted));
    ezlopi_ping_get_profile(hdl, ESP_PING_PROF_REPLY, &received, sizeof(received));
    ezlopi_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
    ezlopi_ping_get_profile(hdl, ESP_PING_PROF_DURATION, &total_time_ms, sizeof(total_time_ms));

#if  (1 == ENABLE_TRACE)
    uint32_t loss = (uint32_t)((1 - ((float)received) / transmitted) * 100);

    if (IP_IS_V4(&target_addr))
    {
        TRACE_D("--- %s ping_handle statistics ---\n", inet_ntoa(*ip_2_ip4(&target_addr)));
    }
    else
    {
        TRACE_D("--- %s ping_handle statistics ---\n", inet6_ntoa(*ip_2_ip6(&target_addr)));
    }

    TRACE_D("%d packets transmitted, %d received, %d%% packet loss, time %dms\n",
        transmitted, received, loss, total_time_ms);
#endif
    // delete the ping_handle sessions, so that we clean up all resources and can create a new ping_handle session
    // we don't have to call delete function in the callback, instead we can call delete function from other tasks
    ezlopi_ping_start_by_handle(hdl);
    TRACE_D("Re-starting ping_handle...");

    // ezlopi_ping_delete_session(hdl);
}

#endif // CONFIG_EZPI_ENABLE_PING
