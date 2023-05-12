#include <stdio.h>
#include <string.h>

#include "esp_netif.h"
#include "esp_eth.h"
#include "esp_event.h"
#include "driver/gpio.h"
#include "esp_eth_mac.h"

#include "trace.h"

#include "ezlopi_ethernet.h"
#include "ezlopi_spi_master.h"

static const char *eth_key_desc_str = "ezlopi_eth";
static esp_netif_t *eth_netif_spi = NULL;
static esp_eth_mac_t *eth_mac_spi = NULL;
static esp_eth_phy_t *eth_phy_spi = NULL;
static esp_eth_handle_t eth_handle_spi = NULL;
static esp_eth_netif_glue_handle_t eth_glue = NULL;
static e_ethernet_status_t eth_last_status = ETHERNET_STATUS_UNKNOWN;
static esp_netif_ip_info_t eth_ip_info;

static s_ezlopi_spi_master_t spi_config = {
    .enable = true,
    .cs = EZLOPI_ETHERNET_W5500_SPI_CS_PIN,
    .miso = EZLOPI_ETHERNET_W5500_SPI_MISO_PIN,
    .mosi = EZLOPI_ETHERNET_W5500_SPI_MOSI_PIN,
    .sck = EZLOPI_ETHERNET_W5500_SPI_SCLK_PIN,
    .mode = EZLOPI_SPI_CPOL_LOW_CPHA_LOW,
    .clock_speed_mhz = EZLOPI_ETHERNET_W5500_SPI_CLOCK_MHZ,
    .addr_bits = EZLOPI_ETHERNET_W5500_SPI_ADDR_BITS,
    .command_bits = EZLOPI_ETHERNET_W5500_SPI_CMD_BITS,
    .transfer_sz = 4096,
    .queue_size = 20,
    .flags = SPICOMMON_BUSFLAG_MASTER,
    .channel = EZLOPI_ETHERNET_W5500_SPI_HOST,
};

static void ezlopi_ethernet_reset(void);
static void ezlopi_ethernet_gpio_setup(void);
static void ezlopi_ethernet_setup_basic(void);
static s_ezlopi_spi_master_t *ezlopi_ethernet_spi_setup(void);
static void eth_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
static void __ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

e_ethernet_status_t ezlopi_ethernet_get_status(void)
{
    return eth_last_status;
}

esp_netif_ip_info_t *ezlopi_ethernet_get_ip_info(void)
{
    memset(&eth_ip_info, 0, sizeof(esp_netif_ip_info_t));

    if (eth_netif_spi)
    {
        esp_netif_get_ip_info(eth_netif_spi, &eth_ip_info);
    }

    return &eth_ip_info;
}

void ezlopi_ethernet_deinit(void)
{
    if (eth_netif_spi)
    {
        esp_event_handler_unregister(IP_EVENT, IP_EVENT_ETH_GOT_IP, &__ip_event_handler);
        esp_event_handler_unregister(IP_EVENT, IP_EVENT_ETH_LOST_IP, &__ip_event_handler);
        esp_event_handler_unregister(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler);

        esp_eth_stop(eth_handle_spi);
        esp_eth_del_netif_glue(eth_glue);
        esp_eth_clear_default_handlers(eth_netif_spi);

        esp_eth_driver_uninstall(eth_handle_spi);
        eth_phy_spi->del(eth_phy_spi);
        eth_mac_spi->del(eth_mac_spi);
        esp_netif_destroy(eth_netif_spi);

        eth_phy_spi = NULL;
        eth_mac_spi = NULL;
        eth_glue = NULL;
        eth_netif_spi = NULL;
        eth_handle_spi = NULL;
    }
}

void ezlopi_ethernet_init(void)
{
    ezlopi_ethernet_gpio_setup();
    ezlopi_ethernet_reset();
    ezlopi_ethernet_setup_basic();
}

static void eth_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    uint8_t mac_addr[6] = {0};
    /* we can get the ethernet driver handle from event data */
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;
    eth_speed_t speed;

    switch (event_id)
    {
    case ETHERNET_EVENT_CONNECTED:
    {
        esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr);
        TRACE_B("Ethernet Link Up");
        TRACE_B("Ethernet HW Addr %02x:%02x:%02x:%02x:%02x:%02x",
                mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
        eth_last_status = ETHERNET_STATUS_LINK_UP;
        break;
    }
    case ETHERNET_EVENT_DISCONNECTED:
    {

        eth_last_status = ETHERNET_STATUS_LINK_DOWN;
        TRACE_B("Ethernet Link Down");
        break;
    }
    case ETHERNET_EVENT_START:
    {
        eth_last_status = ETHERNET_STATUS_STARTED;
        TRACE_B("Ethernet Started");
        break;
    }
    case ETHERNET_EVENT_STOP:
    {
        eth_last_status = ETHERNET_STATUS_STOPPED;
        TRACE_B("Ethernet Stopped");
        break;
    }
    default:
    {
        break;
    }
    }
}

static void __ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    const esp_netif_ip_info_t *eth_ip_info = &event->ip_info;

    switch (event_id)
    {
    case IP_EVENT_ETH_GOT_IP:
    {
        TRACE_B("Ethernet Got IP Address");
        TRACE_B("~~~~~~~~~~~");
        TRACE_B("ETHIP:" IPSTR, IP2STR(&eth_ip_info->ip));
        TRACE_B("ETHMASK:" IPSTR, IP2STR(&eth_ip_info->netmask));
        TRACE_B("ETHGW:" IPSTR, IP2STR(&eth_ip_info->gw));
        TRACE_B("~~~~~~~~~~~");
        eth_last_status = ETHERNET_STATUS_GOT_IP;
        break;
    }
    case IP_EVENT_ETH_LOST_IP:
    {
        eth_last_status = ETHERNET_STATUS_LOST_IP;
        TRACE_W("Ethernet Got IP Address");
        break;
    }
    }
}

static void ezlopi_ethernet_setup_basic(void)
{
    esp_netif_inherent_config_t esp_netif_config = ESP_NETIF_INHERENT_DEFAULT_ETH();

    esp_netif_config.route_prio = 30;
    esp_netif_config.if_key = eth_key_desc_str;
    esp_netif_config.if_desc = eth_key_desc_str;

    esp_netif_config_t cfg_spi = {
        .base = &esp_netif_config,
        .stack = ESP_NETIF_NETSTACK_DEFAULT_ETH,
    };

    eth_netif_spi = esp_netif_new(&cfg_spi);
    eth_mac_config_t mac_config_spi = ETH_MAC_DEFAULT_CONFIG();
    eth_phy_config_t phy_config_spi = ETH_PHY_DEFAULT_CONFIG();

    ezlopi_spi_master_init(&spi_config);

    eth_w5500_config_t w5500_config = ETH_W5500_DEFAULT_CONFIG(spi_config.handle);

    w5500_config.int_gpio_num = EZLOPI_ETHERNET_W5500_INTR_PIN;
    phy_config_spi.phy_addr = EZLOPI_ETHERNET_W5500_PHY_ADDR;
    phy_config_spi.reset_gpio_num = EZLOPI_ETHERNET_W5500_RST_PIN;

    eth_mac_spi = esp_eth_mac_new_w5500(&w5500_config, &mac_config_spi);
    eth_phy_spi = esp_eth_phy_new_w5500(&phy_config_spi);

    esp_eth_config_t eth_config_spi = ETH_DEFAULT_CONFIG(eth_mac_spi, eth_phy_spi);
    ESP_ERROR_CHECK(esp_eth_driver_install(&eth_config_spi, &eth_handle_spi));

    uint8_t base_mac[6];
    esp_read_mac(base_mac, ESP_MAC_ETH);
    dump("ethernet mac", base_mac, 0, sizeof(base_mac));
    ESP_ERROR_CHECK(esp_eth_ioctl(eth_handle_spi, ETH_CMD_S_MAC_ADDR, base_mac));
    eth_glue = esp_eth_new_netif_glue(eth_handle_spi);
    ESP_ERROR_CHECK(esp_netif_attach(eth_netif_spi, eth_glue));

    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &__ip_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_LOST_IP, &__ip_event_handler, NULL));

    esp_eth_start(eth_handle_spi);
}

static void ezlopi_ethernet_gpio_setup(void)
{
    gpio_config_t gpio_conf = {
        .pin_bit_mask = (1UL << EZLOPI_ETHERNET_W5500_EN_PIN) | (1UL << EZLOPI_ETHERNET_W5500_RST_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    gpio_config(&gpio_conf);
}

static void ezlopi_ethernet_reset(void)
{
    /* 1.Enable PWR to W5500 via mosfet */
    gpio_set_level(EZLOPI_ETHERNET_W5500_EN_PIN, 1);
    /* 2. Wait 500mS */
    vTaskDelay(500 / portTICK_RATE_MS);
    /* 3. Pull reset low, wait 5mS */
    gpio_set_level(EZLOPI_ETHERNET_W5500_RST_PIN, 0);
    vTaskDelay(5 / portTICK_RATE_MS);
    /* 4. Pull reset high, wait 10mS */
    gpio_set_level(EZLOPI_ETHERNET_W5500_RST_PIN, 1);
    vTaskDelay(10 / portTICK_RATE_MS);
}