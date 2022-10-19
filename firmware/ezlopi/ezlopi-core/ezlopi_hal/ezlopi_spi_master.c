#include "stdbool.h"
#include "sdkconfig.h"
#include "driver/spi_common.h"
#include "driver/spi_master.h"

#include "ezlopi_spi_master.h"
#include "trace.h"

typedef enum e_spi_state
{
    SPI_UNINITIALIZED = 0,
    SPI_INITIALIZED
} e_spi_state_t;

#define SPI_ERROR_CHECK(ret, err_msg) \
    {                                 \
        if (ESP_OK != ret)            \
        {                             \
            TRACE_E(err_msg);         \
            break;                    \
        }                             \
    }

static e_spi_state_t spi_port_status[3] = {SPI_UNINITIALIZED, SPI_UNINITIALIZED, SPI_UNINITIALIZED};

int ezlopi_spi_master_init(s_ezlopi_spi_master_t *spi_config)
{
    int ret = 0;

    if (NULL != spi_config)
    {
        if (true == spi_config->enable)
        {
            spi_bus_config_t buscfg = {
                .miso_io_num = spi_config->miso,
                .mosi_io_num = spi_config->mosi,
                .sclk_io_num = spi_config->sck,
                .quadhd_io_num = -1,
                .quadwp_io_num = -1,
                .max_transfer_sz = 16,
            };

            spi_device_interface_config_t devcfg = {
                .command_bits = 10,
                .clock_speed_hz = spi_config->clock_speed,
                .mode = EZLOPI_SPI_CPOL_LOW_CPHA_LOW, // SPI mode 0
                .spics_io_num = -1,                   // not used
                .queue_size = 1,
                .flags = SPI_DEVICE_HALFDUPLEX | SPI_DEVICE_POSITIVE_CS,
                .pre_cb = NULL,
                .post_cb = NULL,
                .input_delay_ns = 0, // the EEPROM output the data half a SPI clock behind.
            };

            do
            {
                ret = spi_bus_initialize(spi_config->channel, &buscfg, SPI_DMA_CH_AUTO);
                SPI_ERROR_CHECK(ret, "SPI-master 'spi_bus_initialize' failed!");

                ret = spi_bus_add_device(spi_config->channel, &devcfg, &spi_config->handle);
                SPI_ERROR_CHECK(ret, "SPI-master 'spi_bus_add_device' failed!");

                spi_port_status[spi_config->channel] = SPI_INITIALIZED;
            } while (0);
        }
    }

    return ret;
}

int ezlopi_spi_master_deinit(s_ezlopi_spi_master_t *spi_config)
{
    int ret = 0;

    if (NULL != spi_config)
    {
        if (SPI_INITIALIZED == spi_port_status[spi_config->channel])
        {
            spi_bus_remove_device(spi_config->handle);
            spi_config->handle = NULL;
            spi_port_status[spi_config->channel] = SPI_UNINITIALIZED;
        }
    }

    return ret;
}