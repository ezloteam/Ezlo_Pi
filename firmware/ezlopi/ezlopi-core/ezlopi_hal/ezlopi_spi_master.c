#include "stdbool.h"
#include "sdkconfig.h"
#include "driver/spi_common.h"
#include "driver/spi_master.h"
#include "esp_intr_alloc.h"

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
                .max_transfer_sz = spi_config->transfer_sz,
                .flags = spi_config->flags,
#if CONFIG_SPI_MASTER_ISR_IN_IRAM
                .intr_flags = ESP_INTR_FLAG_IRAM
#endif
            };

            spi_device_interface_config_t devcfg = {
                .command_bits = spi_config->command_bits,
                .address_bits = spi_config->addr_bits,
                .mode = spi_config->mode,       // SPI mode 0
                .clock_speed_hz = spi_config->clock_speed_mhz * 1000000,
                .queue_size = spi_config->queue_size,
                .pre_cb = NULL,
                .post_cb = NULL,
                .input_delay_ns = 0,            // the EEPROM output the data half a SPI clock behind.
                .spics_io_num = spi_config->cs, // not used
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