/* ===========================================================================
** Copyright (C) 2024 Ezlo Innovation Inc
**
** Under EZLO AVAILABLE SOURCE LICENSE (EASL) AGREEMENT
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/
/**
 * @file    ezlopi_core_ethernet.c
 * @brief   Function to perform operation on ethernet module
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 *          Lomas Subedi
 *          Nabin Dangi
 *          Riken Maharjan
 * @version 0.1
 * @date    May 9th, 2023 6:10 PM
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#ifdef CONFIG_EZPI_CORE_ENABLE_ETH

#include <string.h>

#include "esp_event.h"
#include "driver/gpio.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_core_ethernet.h"

#include "ezlopi_hal_spi_master.h"

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
static void __ethernet_reset(void);
static void __ethernet_gpio_setup(void);
static void __ethernet_setup_basic(void);
static void __eth_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
static void __ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/
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

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
e_ethernet_status_t EZPI_core_ethernet_get_status(void)
{
    return eth_last_status;
}

esp_netif_ip_info_t *EZPI_core_ethernet_get_ip_info(void)
{
    memset(&eth_ip_info, 0, sizeof(esp_netif_ip_info_t));

    if (eth_netif_spi)
    {
        esp_netif_get_ip_info(eth_netif_spi, &eth_ip_info);
    }

    return &eth_ip_info;
}

void EZPI_ethernet_deinit(void)
{
    if (eth_netif_spi)
    {
        esp_event_handler_unregister(IP_EVENT, IP_EVENT_ETH_GOT_IP, &__ip_event_handler);
        esp_event_handler_unregister(IP_EVENT, IP_EVENT_ETH_LOST_IP, &__ip_event_handler);
        esp_event_handler_unregister(ETH_EVENT, ESP_EVENT_ANY_ID, &__eth_event_handler);

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

void EZPI_ethernet_init(void)
{
    __ethernet_gpio_setup();
    __ethernet_reset();
    __ethernet_setup_basic();
}

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/
/**
 * @brief Local/static function template example
 * Convention : Use lowercase letters for all words on static functions
 * @param arg
 */

static void __eth_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    uint8_t mac_addr[6] = {0};
    /* we can get the ethernet driver handle from event data */
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;

    switch (event_id)
    {
    case ETHERNET_EVENT_CONNECTED:
    {
        esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr);
        TRACE_I("Ethernet Link Up");
        TRACE_I("Ethernet HW Addr %02x:%02x:%02x:%02x:%02x:%02x",
                mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
        eth_last_status = ETHERNET_STATUS_LINK_UP;
        break;
    }
    case ETHERNET_EVENT_DISCONNECTED:
    {

        eth_last_status = ETHERNET_STATUS_LINK_DOWN;
        TRACE_I("Ethernet Link Down");
        break;
    }
    case ETHERNET_EVENT_START:
    {
        eth_last_status = ETHERNET_STATUS_STARTED;
        TRACE_I("Ethernet Started");
        break;
    }
    case ETHERNET_EVENT_STOP:
    {
        eth_last_status = ETHERNET_STATUS_STOPPED;
        TRACE_I("Ethernet Stopped");
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

    switch (event_id)
    {
    case IP_EVENT_ETH_GOT_IP:
    {
        memcpy(&eth_ip_info, &event->ip_info, sizeof(esp_netif_ip_info_t));
        TRACE_I("Ethernet Got IP Address");
        TRACE_I("~~~~~~~~~~~");
        TRACE_I("ETHIP:" IPSTR, IP2STR(&eth_ip_info.ip));
        TRACE_I("ETHMASK:" IPSTR, IP2STR(&eth_ip_info.netmask));
        TRACE_I("ETHGW:" IPSTR, IP2STR(&eth_ip_info.gw));
        TRACE_I("~~~~~~~~~~~");
        eth_last_status = ETHERNET_STATUS_GOT_IP;
        break;
    }
    case IP_EVENT_ETH_LOST_IP:
    {
        memset(&eth_ip_info, 0, sizeof(esp_netif_ip_info_t));
        eth_last_status = ETHERNET_STATUS_LOST_IP;
        TRACE_W("Ethernet Lost IP Address");
        break;
    }
    }
}

static void __ethernet_setup_basic(void)
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

    EZPI_hal_spi_master_init(&spi_config);

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

    esp_eth_start(eth_handle_spi);
}

static void __ethernet_gpio_setup(void)
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

static void __ethernet_reset(void)
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

#endif // CONFIG_EZPI_CORE_ENABLE_ETH

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
