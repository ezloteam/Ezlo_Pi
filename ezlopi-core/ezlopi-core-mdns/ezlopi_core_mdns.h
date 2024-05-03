
#ifndef __EZLOPI_CORE_MDNS_H_
#define __EZLOPI_CORE_MDNS_H_

#ifdef CONFIG_EZPI_SERV_MDNS_EN

#include "mdns.h"

#define EZPI_MDNS_HOSTNAME_SIZE  100
#define EZPI_MDNS_CONTEX_COUNT 2
#define EZPI_MDNS_SERIAL_SIZE 10

typedef struct l_ezlopi_mdns_context
{
    mdns_txt_item_t* mdns_context;
    struct l_ezlopi_mdns_context* next;
} l_ezlopi_mdns_context_t;

int EZPI_core_init_mdns();

#endif // __EZLOPI_CORE_MDNS_H_
#endif // CONFIG_EZPI_SERV_MDNS_EN

