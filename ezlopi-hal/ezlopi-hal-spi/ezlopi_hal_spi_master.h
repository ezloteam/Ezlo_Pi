#ifndef _EZLOPI_HAL_SPI_MASTER_H_
#define _EZLOPI_HAL_SPI_MASTER_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "driver/spi_master.h"

#include "ezlopi_core_errors.h" 

typedef enum e_ezlopi_spi_channel
{
    EZLOPI_SPI_1 = 0,
    EZLOPI_SPI_2,
    EZLOPI_SPI_3,
    EZLOPI_SPI_MAX
} e_ezlopi_spi_channel_t;

typedef enum e_ezlopi_spi_mode
{
    EZLOPI_SPI_CPOL_LOW_CPHA_LOW = 0, // CPOL_LOW(polarity): Logic low, CPHA_LOW(phase) - Data sampled on rising edge and shifted out on the falling edge
    EZLOPI_SPI_CPOL_LOW_CPHA_HIGH,    // CPOL_LOWapolarity): Logic low, CPHA_HIGH(phase) - Data sampled on the falling edge and shifted out on the rising edge
    EZLOPI_SPI_CPOL_HIGH_CPHA_LOW,    // CPOL_HIGH(polarity): Logic high, CPHA_LOW(phase) - Data sampled on the rising edge and shifted out on the falling edge
    EZLOPI_SPI_CPOL_HIGH_CPHA_HIGH    // CPOL_HIGH(polarity): Logic high, CPHA_HIGH(phase) - Data sampled on the falling edge and shifted out on the rising edge
} e_ezlopi_spi_mode_t;

typedef struct s_ezlopi_spi_master
{
    bool enable;
    int cs;
    int miso;
    int mosi;
    int sck;
    uint32_t mode;
    uint32_t clock_speed_mhz;
    uint8_t addr_bits;
    uint8_t command_bits;
    uint32_t transfer_sz;
    uint32_t queue_size;
    uint32_t flags;
    e_ezlopi_spi_channel_t channel;
    spi_device_handle_t handle;

} s_ezlopi_spi_master_t;

ezlopi_error_t ezlopi_spi_master_init(s_ezlopi_spi_master_t *spi_config);

#endif // _EZLOPI_HAL_SPI_MASTER_H_
