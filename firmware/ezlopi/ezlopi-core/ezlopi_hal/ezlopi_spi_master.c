#include "stdbool.h"
#include "sdkconfig.h"
#include "ezlopi_spi_master.h"

int ezlopi_spi_master_init(s_ezlopi_spi_master_t *spi_config)
{
    int ret = 0;

    if (NULL != spi_config)
    {
        if (true == spi_config->enable)
        {
            
        }
    }

    return ret;
}