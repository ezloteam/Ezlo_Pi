
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_netif_ip_addr.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "mdns.h"
#include "driver/gpio.h"
#include "netdb.h"

#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_wifi.h"
#include "ezlopi_cloud_keywords.h"
#include "ezlopi_util_trace.h"
#include "ezlopi_core_mdns.h"


static char* generate_hostname(void);
const char* ezlopi_mdns_instance_name = "EzloPi, an Open Source IoT Platform";
static l_ezlopi_mdns_context_t* ezlopi_mdns_service_cntx = NULL;

static void __mdns_init(void* pv);

static void initialise_mdns(void)
{
    char* hostname = generate_hostname();

    // initialize mDNS
    ESP_ERROR_CHECK(mdns_init());
    // set mDNS hostname (required if you want to advertise services)
    ESP_ERROR_CHECK(mdns_hostname_set(hostname));
    TRACE_D("mdns hostname set to: [%s]", hostname);
    // set default mDNS instance name
    ESP_ERROR_CHECK(mdns_instance_name_set(CONFIG_EZPI_MDNS_INSTANCE_NAME));

    uint64_t id_val = ezlopi_factory_info_v3_get_id();
    char* id_val_str = malloc(10 * sizeof(char));
    memset(id_val_str, 0, (10 * sizeof(char)));
    snprintf(id_val_str, 10, "%lld", id_val);

    uint16_t firmware_version_val = ezlopi_factory_info_v3_get_version();
    char* firmware_version_val_str = malloc(10 * sizeof(char));
    memset(firmware_version_val_str, 0, 10 * sizeof(char));
    snprintf(firmware_version_val_str, 10, "%d", firmware_version_val);

    // Define mdns serive context
    mdns_txt_item_t service_context_item[8] =
    {
        {
            .key = "ID",
            .value = id_val_str,
        },
        {
            .key = "name",
            .value = (NULL == ezlopi_factory_info_v3_get_name() ? "null" : ezlopi_factory_info_v3_get_name()),
        },
        {
            .key = "manufacturer",
            .value = (NULL == ezlopi_factory_info_v3_get_manufacturer() ? "null" : ezlopi_factory_info_v3_get_manufacturer()),
        },
        {
            .key = "brand",
            .value = (NULL == ezlopi_factory_info_v3_get_brand() ? "null" : ezlopi_factory_info_v3_get_brand())
        },
        {
            .key = "Model",
            .value = (NULL == ezlopi_factory_info_v3_get_model() ? "null" : ezlopi_factory_info_v3_get_model()),
        },
        {
            .key = "ezlopi_device_type",
            .value = (NULL == ezlopi_factory_info_v3_get_device_type() ? "null" : ezlopi_factory_info_v3_get_device_type()),
        },
        {
            .key = "ezlopi_firmware_version",
            .value = firmware_version_val_str,
        },
        {
            .key = "ezlopi_firmware_build",
            .value = "1.8.2",
        },
    };

    ESP_ERROR_CHECK(mdns_service_add(CONFIG_EZPI_MDNS_INSTANCE_NAME, "_http", "_tcp", 80, service_context_item, 8));
    free(hostname);
}

int EZPI_core_init_mdns(void)
{
    int ret = 0;
#if 0
    // initialize mDNS
    mdns_init();
    mdns_instance_name_set(ezlopi_mdns_instance_name);

    // set mDNS hostname (required if  you want to advertise services)
    char hostname[EZPI_MDNS_HOSTNAME_SIZE];
    char* ezlopi_type = ezlopi_factory_info_v3_get_device_type();
    uint64_t id_val = ezlopi_factory_info_v3_get_id();

    if (0)
    {
        uint32_t id_last4 = id_val % 10000;
        sprintf(hostname, "%s_%d", ezlopi_type, id_last4);
        mdns_hostname_set(hostname);
        // mdns_service_add(ezlopi_mdns_instance_name, hostname, "_tcp", id_last4, NULL, 0);

    }
    else
    {
        // sprintf(hostname, "%s", ezlopi_type);
        // mdns_hostname_set(hostname);
        mdns_hostname_set(ezlopi_type);
        // mdns_service_add(ezlopi_mdns_instance_name, hostname, "_tcp", 80, NULL, 0);
    }

    // mdns_service_add(ezlopi_mdns_instance_name, "_http", "_tcp", 80, NULL, 0);


    // mdns_service_add();


    // esp_err_t mdns_service_port_set(const char* service, const char* proto, uint16_t port)

    //     TRACE_D("mdns hostname set to: [%s]", hostname);
    // // set default mDNS instance name


    // initialise_mdns();
#endif 




    xTaskCreate(__mdns_init, "mdns service init", 4 * 2048, NULL, 4, NULL);

    return ret;
}

/** Generate host name based on sdkconfig, optionally adding a portion of MAC address to it.
 *  @return host name string allocated from the heap
 */
static char* generate_hostname(void)
{
#ifndef CONFIG_MDNS_ADD_MAC_TO_HOSTNAME
    return strdup(CONFIG_EZPI_MDNS_INSTANCE_NAME);
#else
    uint8_t mac[6];
    char* hostname;
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    if (-1 == asprintf(&hostname, "%s-%02X%02X%02X", CONFIG_EZPI_MDNS_INSTANCE_NAME, mac[3], mac[4], mac[5]))
    {
        abort();
    }
    return hostname;
#endif
}

static void ezlopi_mdns_add_service_context(l_ezlopi_mdns_context_t* new_context)
{
    if (new_context)
    {
        l_ezlopi_mdns_context_t* head_context = ezlopi_mdns_service_cntx;
        if (head_context == NULL)
        {
            head_context = new_context;
        }
        else
        {
            while (head_context->next)
            {
                head_context = head_context->next;
            }
            head_context->next = new_context;
        }
    }

}


static l_ezlopi_mdns_context_t* ezlopi_mdns_get_service_context()
{
    return ezlopi_mdns_service_cntx;
}


static void ezlopi_mdns_init_service_context()
{

    // Add one service about EzloPi Device Type
    l_ezlopi_mdns_context_t* ezlopi_mdns_service_cntx_device_type = (l_ezlopi_mdns_context_t*)malloc(sizeof(l_ezlopi_mdns_context_t));
    if (ezlopi_mdns_service_cntx_device_type)
    {
        memset(ezlopi_mdns_service_cntx_device_type, 0, sizeof(l_ezlopi_mdns_context_t));
        mdns_txt_item_t* service_cntx_device_type = (mdns_txt_item_t*)malloc(sizeof(mdns_txt_item_t));
        if (service_cntx_device_type)
        {
            memset(service_cntx_device_type, 0, sizeof(mdns_txt_item_t));
            service_cntx_device_type->key = ezlopi_ezlopi_device_type_str;
            service_cntx_device_type->value = ezlopi_factory_info_v3_get_device_type();;

            ezlopi_mdns_service_cntx_device_type->mdns_context = service_cntx_device_type;

            ezlopi_mdns_add_service_context(ezlopi_mdns_service_cntx_device_type);
        }
    }


    // Add one service about EzloPi Serial
    l_ezlopi_mdns_context_t* ezlopi_mdns_service_cntx_device_id = (l_ezlopi_mdns_context_t*)malloc(sizeof(l_ezlopi_mdns_context_t));

    if (ezlopi_mdns_service_cntx_device_id)
    {
        memset(ezlopi_mdns_service_cntx_device_id, 0, sizeof(l_ezlopi_mdns_context_t));

        mdns_txt_item_t* service_cntx_device_id = (mdns_txt_item_t*)malloc(sizeof(mdns_txt_item_t));
        if (service_cntx_device_id)
        {
            memset(service_cntx_device_id, 0, sizeof(mdns_txt_item_t));
            uint64_t id_val = ezlopi_factory_info_v3_get_id();
            if (id_val)
            {
                char* id_val_str = malloc(EZPI_MDNS_SERIAL_SIZE);
                if (id_val_str)
                {

                    service_cntx_device_id->key = ezlopi_serial_str;

                    memset(id_val_str, 0, EZPI_MDNS_SERIAL_SIZE);
                    snprintf(id_val_str, EZPI_MDNS_SERIAL_SIZE, "%lld", id_val);
                    service_cntx_device_id->value = (NULL == id_val_str ? "null" : id_val_str);

                    ezlopi_mdns_service_cntx_device_id->mdns_context = service_cntx_device_id;
                    ezlopi_mdns_add_service_context(ezlopi_mdns_service_cntx_device_id);
                    free(id_val_str);
                }
            }
        }

    }
}

static void __mdns_init(void* pv)
{

    ezlopi_mdns_init_service_context();
    while (1)
    {
        ezlopi_wait_for_wifi_to_connect(portMAX_DELAY);

        esp_err_t err = mdns_init();
        if (err == ESP_OK)
        {
            err = mdns_hostname_set("ezlopi");
            mdns_instance_name_set("EzloPi mdns string");
            TRACE_I("Successful mDNS Initialization");

            char hostname[EZPI_MDNS_HOSTNAME_SIZE];


            l_ezlopi_mdns_context_t* mdns_contex_head = ezlopi_mdns_get_service_context();
            while (mdns_contex_head) {
                err = mdns_service_add(ezlopi_mdns_instance_name, "_http", "_tcp", 80, mdns_contex_head->mdns_context, 1);
                mdns_contex_head = mdns_contex_head->next;
            }
            break;

            // if (err == ESP_OK)
            // {

            //     TRACE_I("Successful mDNS hostname setting");
            //     break;
            // }
            // else
            // {
            //     TRACE_E("Error mDNS hostname setting");
            // }
        }
        else
        {
            TRACE_E("Error mDNS Initialization")
        }
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);

}