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
 * @file    ezlopi_core_ethernet.h
 * @brief   Function to perform operation on ethernet module
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 *          Lomas Subedi
 *          Riken Maharjan
 *          Nabin Dangi
 * @version 0.1
 * @date    May 9th, 2023 6:10 PM
 */

#ifndef _EZLOPI_CORE_ETHERNET_H_
#define _EZLOPI_CORE_ETHERNET_H_

#ifdef CONFIG_EZPI_CORE_ENABLE_ETH

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

/*******************************************************************************
 *                          C++ Declaration Wrapper
 *******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

    /*******************************************************************************
     *                          Type & Macro Declarations
     *******************************************************************************/
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

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/
    /**
     * @brief Funtion to initialize ethernet
     *
     */
    void EZPI_ethernet_init(void);
    /**
     * @brief Function to deinitialize ethernet service from the system
     *
     */
    void EZPI_ethernet_deinit(void);
    /**
     * @brief Function to return ethernet status
     *
     * @return e_ethernet_status_t
     */
    e_ethernet_status_t EZPI_core_ethernet_get_status(void);
    /**
     * @brief Function to get IP info from ethernet
     *
     * @return esp_netif_ip_info_t*
     */
    esp_netif_ip_info_t *EZPI_core_ethernet_get_ip_info(void);

#ifdef __cplusplus
}
#endif

#endif // CONFIG_EZPI_CORE_ENABLE_ETH

#endif // _EZLOPI_CORE_ETHERNET_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
