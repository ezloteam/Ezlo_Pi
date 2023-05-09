#include <stdio.h>
#include <string.h>

#include "esp_netif.h"
#include "esp_eth.h"
#include "esp_event.h"
#include "driver/gpio.h"
#include "esp_eth_mac.h"

#include "ezlopi_ethernet.h"
#include "ezlopi_spi_master.h"

static void ezlopi_ethernet_reset(void);
static void ezlopi_ethernet_gpio_setup(void);
static s_ezlopi_spi_master_t *ezlopi_ethernet_spi_setup(void);

void ezlopi_ethernet_init(void)
{
    ezlopi_ethernet_gpio_setup();
    ezlopi_ethernet_reset();
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_netif_init());
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_event_loop_create_default());
}

static void ezlopi_ethernet_setup_basic(void)
{
    const char *eth_key_desc_str = "ezlopi";
    esp_netif_inherent_config_t esp_netif_config = ESP_NETIF_INHERENT_DEFAULT_ETH();

    esp_netif_config.route_prio = 30;
    esp_netif_config.if_key = eth_key_desc_str;
    esp_netif_config.if_desc = eth_key_desc_str;

    esp_netif_config_t cfg_spi = {
        .base = &esp_netif_config,
        .stack = ESP_NETIF_NETSTACK_DEFAULT_ETH,
    };

    esp_netif_t *eth_netif_spi = esp_netif_new(&cfg_spi);
    eth_mac_config_t mac_config_spi = ETH_MAC_DEFAULT_CONFIG();
    eth_phy_config_t phy_config_spi = ETH_PHY_DEFAULT_CONFIG();

    s_ezlopi_spi_master_t *spi_conf = ezlopi_ethernet_spi_setup();

    esp_eth_handle_t eth_handle_spi = NULL;

    eth_w5500_config_t w5500_config = ETH_W5500_DEFAULT_CONFIG(spi_conf->handle);

    w5500_config.int_gpio_num = EZLOPI_ETHERNET_W5500_INTR_PIN;
    phy_config_spi.phy_addr = EZLOPI_ETHERNET_W5500_PHY_ADDR;
    phy_config_spi.reset_gpio_num = EZLOPI_ETHERNET_W5500_RST_PIN;

    esp_eth_mac_t *mac_spi = esp_eth_mac_new_w5500(&w5500_config, &mac_config_spi);
    esp_eth_phy_t *phy_spi = esp_eth_phy_new_w5500(&phy_config_spi);

    esp_eth_config_t eth_config_spi = ETH_DEFAULT_CONFIG(mac_spi, phy_spi);
    ESP_ERROR_CHECK(esp_eth_driver_install(&eth_config_spi, &eth_handle_spi));

    uint8_t base_mac[6];
    esp_read_mac(base_mac, ESP_MAC_ETH);
    base_mac[5] += i;
    ESP_ERROR_CHECK(esp_eth_ioctl(spi_conf->handle, ETH_CMD_S_MAC_ADDR, base_mac));
    ESP_ERROR_CHECK(esp_netif_attach(eth_netif_spi[i], esp_eth_new_netif_glue(eth_handle_spi)));

    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, NULL));

    esp_eth_start(eth_handle_spi);
}

static 

static s_ezlopi_spi_master_t *ezlopi_ethernet_spi_setup(void)
{
    esp_err_t ret = ESP_OK;
    s_ezlopi_spi_master_t spi = {
        .enable = true,
        .cs = EZLOPI_ETHERNET_W5500_SPI_CS_PIN,
        .miso = EZLOPI_ETHERNET_W5500_SPI_MISO_PIN,
        .mosi = EZLOPI_ETHERNET_W5500_SPI_MOSI_PIN,
        .sck = EZLOPI_ETHERNET_W5500_SPI_SCLK_PIN,
        .mode = 0,
        .clock_speed_mhz = EZLOPI_ETHERNET_W5500_SPI_CLOCK_MHZ,
        .addr_length = EZLOPI_ETHERNET_W5500_SPI_ADDR_BITS,
        .command_length = EZLOPI_ETHERNET_W5500_SPI_CMD_BITS,
        .transfer_sz = 4096,
        .flags = SPICOMMON_BUSFLAG_MASTER,
        .channel = EZLOPI_ETHERNET_W5500_SPI_HOST,
    };

    ezlopi_spi_master_init(&spi);
}

static void ezlopi_ethernet_gpio_setup(void)
{
    gpio_config_t gpio_conf = {
        .pin_bit_mask = (1UL << EZLOPI_ETHERNET_W5500_EN_PIN) | (1UL << EZLOPI_ETHERNET_W5500_RST_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    status = gpio_config(&gpio_conf);
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