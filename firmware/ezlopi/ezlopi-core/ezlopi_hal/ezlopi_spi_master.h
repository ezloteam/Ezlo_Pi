#ifndef __EZLOPI_SPI_MASTER_H__
#define __EZLOPI_SPI_MASTER_H__

#include <stdio.h>

typedef struct s_ezlopi_spi_master
{
    uint32_t miso;
    uint32_t mosi;
    uint32_t sck;
    uint32_t clock_speed;

} s_ezlopi_spi_master_t;

#endif // __EZLOPI_SPI_MASTER_H__
