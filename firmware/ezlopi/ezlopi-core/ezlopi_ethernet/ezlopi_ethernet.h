#ifndef __EZLOPI_ETHERNET_H__
#define __EZLOPI_ETHERNET_H__

#include "esp_netif.h"
#include "ezlopi_boards.h"
#include "ezlopi_spi_master.h"

#if (EZLOPI_AMBIENT_TRACKER_PRO_REV_A == EZLOPI_BOARD_TYPE)
#define EZLOPI_ETHERNET_W5500_EN_PIN 13
#define EZLOPI_ETHERNET_W5500_RST_PIN 11
#define EZLOPI_ETHERNET_W5500_INTR_PIN 12
#define EZLOPI_ETHERNET_W5500_SPI_MOSI_PIN 2
#define EZLOPI_ETHERNET_W5500_SPI_MISO_PIN 1
#define EZLOPI_ETHERNET_W5500_SPI_SCLK_PIN 4
#define EZLOPI_ETHERNET_W5500_SPI_CS_PIN 5
#define EZLOPI_ETHERNET_W5500_SPI_HOST EZLOPI_SPI_2
#define EZLOPI_ETHERNET_W5500_SPI_CMD_BITS 16
#define EZLOPI_ETHERNET_W5500_SPI_ADDR_BITS 8
#define EZLOPI_ETHERNET_W5500_SPI_CLOCK_MHZ 12 // 12 MHz
#define EZLOPI_ETHERNET_W5500_PHY_ADDR -1
#endif

typedef enum e_ethernet_status
{
    ETHERNET_STATUS_UNKNOWN = 0,
    ETHERNET_STATUS_STARTED,
    ETHERNET_STATUS_STOPPED,
    ETHERNET_STATUS_LINK_DOWN,
    ETHERNET_STATUS_LINK_UP,
    ETHERNET_STATUS_GOT_IP,
    ETHERNET_STATUS_LOST_IP,

} e_ethernet_status_t;

void ezlopi_ethernet_init(void);
void ezlopi_ethernet_deinit(void);
e_ethernet_status_t ezlopi_ethernet_get_status(void);
esp_netif_ip_info_t *ezlopi_ethernet_get_ip_info(void);

#endif // __EZLOPI_ETHERNET_H__
