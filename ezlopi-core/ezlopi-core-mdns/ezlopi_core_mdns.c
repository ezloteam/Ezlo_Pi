
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


const char* ezlopi_mdns_instance_name = "EzloPi, an Open Source IoT Platform";
static l_ezlopi_mdns_context_t* ezlopi_mdns_service_cntx = NULL;

static void __mdns_init(void* pv);

int EZPI_core_init_mdns(void)
{
    int ret = 0;

    xTaskCreate(__mdns_init, "mdns service init", 4 * 2048, NULL, 4, NULL);

    return ret;
}


static void ezlopi_mdns_add_service_context(l_ezlopi_mdns_context_t* new_context)
{
    if (new_context)
    {
        if (ezlopi_mdns_service_cntx == NULL)
        {
            ezlopi_mdns_service_cntx = new_context;
        }
        else
        {
            l_ezlopi_mdns_context_t* head_context = ezlopi_mdns_service_cntx;
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
            service_cntx_device_type->value = (NULL == ezlopi_factory_info_v3_get_device_type() ? "null" : ezlopi_factory_info_v3_get_device_type());

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
                char* id_val_str = (char*)malloc(EZPI_MDNS_SERIAL_SIZE);
                if (id_val_str)
                {
                    service_cntx_device_id->key = "Serial";

                    memset(id_val_str, 0, EZPI_MDNS_SERIAL_SIZE);
                    snprintf(id_val_str, EZPI_MDNS_SERIAL_SIZE, "%lld", id_val);
                    service_cntx_device_id->value = (NULL == id_val_str ? "null" : id_val_str);

                    ezlopi_mdns_service_cntx_device_id->mdns_context = service_cntx_device_id;
                    ezlopi_mdns_add_service_context(ezlopi_mdns_service_cntx_device_id);
                }
            }
        }
    }

    // Add manufacturer service
#ifdef EZPI_SERV_MDNS_MANUFACTURER_SERVICE_EN
    l_ezlopi_mdns_context_t* ezlopi_mdns_service_cntx_manufacturer = (l_ezlopi_mdns_context_t*)malloc(sizeof(l_ezlopi_mdns_context_t));
    if (ezlopi_mdns_service_cntx_manufacturer)
    {
        memset(ezlopi_mdns_service_cntx_manufacturer, 0, sizeof(l_ezlopi_mdns_context_t));
        mdns_txt_item_t* service_cntx_manufacturer = (mdns_txt_item_t*)malloc(sizeof(mdns_txt_item_t));
        if (service_cntx_manufacturer)
        {
            memset(service_cntx_manufacturer, 0, sizeof(mdns_txt_item_t));
            service_cntx_manufacturer->key = ezlopi_manufacturer_str;
            service_cntx_manufacturer->value = (NULL == ezlopi_factory_info_v3_get_manufacturer() ? "null" : ezlopi_factory_info_v3_get_manufacturer());

            ezlopi_mdns_service_cntx_manufacturer->mdns_context = service_cntx_manufacturer;

            ezlopi_mdns_add_service_context(ezlopi_mdns_service_cntx_manufacturer);
        }
    }
#endif // EZPI_SERV_MDNS_MANUFACTURER_SERVICE_EN

    // Add brand service
#ifdef EZPI_SERV_MDNS_BRAND_SERVICE_EN
    l_ezlopi_mdns_context_t* ezlopi_mdns_service_cntx_brand = (l_ezlopi_mdns_context_t*)malloc(sizeof(l_ezlopi_mdns_context_t));
    if (ezlopi_mdns_service_cntx_brand)
    {
        memset(ezlopi_mdns_service_cntx_brand, 0, sizeof(l_ezlopi_mdns_context_t));
        mdns_txt_item_t* service_cntx_brand = (mdns_txt_item_t*)malloc(sizeof(mdns_txt_item_t));
        if (service_cntx_brand)
        {
            memset(service_cntx_brand, 0, sizeof(mdns_txt_item_t));
            service_cntx_brand->key = ezlopi_brand_str;
            service_cntx_brand->value = (NULL == ezlopi_factory_info_v3_get_brand() ? "null" : ezlopi_factory_info_v3_get_brand());

            ezlopi_mdns_service_cntx_brand->mdns_context = service_cntx_brand;

            ezlopi_mdns_add_service_context(ezlopi_mdns_service_cntx_brand);
        }
    }
#endif // EZPI_SERV_MDNS_BRAND_SERVICE_EN

    // Add model service
#ifdef EZPI_SERV_MDNS_MODEL_SERVICE_EN
    l_ezlopi_mdns_context_t* ezlopi_mdns_service_cntx_model = (l_ezlopi_mdns_context_t*)malloc(sizeof(l_ezlopi_mdns_context_t));
    if (ezlopi_mdns_service_cntx_model)
    {
        memset(ezlopi_mdns_service_cntx_model, 0, sizeof(l_ezlopi_mdns_context_t));
        mdns_txt_item_t* service_cntx_model = (mdns_txt_item_t*)malloc(sizeof(mdns_txt_item_t));
        if (service_cntx_model)
        {
            memset(service_cntx_model, 0, sizeof(mdns_txt_item_t));
            service_cntx_model->key = ezlopi_model_str;
            service_cntx_model->value = (NULL == ezlopi_factory_info_v3_get_model() ? "null" : ezlopi_factory_info_v3_get_model());

            ezlopi_mdns_service_cntx_model->mdns_context = service_cntx_model;

            ezlopi_mdns_add_service_context(ezlopi_mdns_service_cntx_model);
        }
    }
#endif // EZPI_SERV_MDNS_MODEL_SERVICE_EN

    // Add name service
#ifdef EZPI_SERV_MDNS_NAME_SERVICE_EN
    l_ezlopi_mdns_context_t* ezlopi_mdns_service_cntx_name = (l_ezlopi_mdns_context_t*)malloc(sizeof(l_ezlopi_mdns_context_t));
    if (ezlopi_mdns_service_cntx_name)
    {
        memset(ezlopi_mdns_service_cntx_name, 0, sizeof(l_ezlopi_mdns_context_t));
        mdns_txt_item_t* service_cntx_name = (mdns_txt_item_t*)malloc(sizeof(mdns_txt_item_t));
        if (service_cntx_name)
        {
            memset(service_cntx_name, 0, sizeof(mdns_txt_item_t));
            service_cntx_name->key = ezlopi_name_str;
            service_cntx_name->value = (NULL == ezlopi_factory_info_v3_get_name() ? "null" : ezlopi_factory_info_v3_get_name());

            ezlopi_mdns_service_cntx_name->mdns_context = service_cntx_name;

            ezlopi_mdns_add_service_context(ezlopi_mdns_service_cntx_name);
        }
    }
#endif // EZPI_SERV_MDNS_NAME_SERVICE_EN

}

static mdns_txt_item_t* prepare_mdns_item_service_context(int* service_size)
{
    mdns_txt_item_t* mdns_context = NULL;
    int size = 0;
    l_ezlopi_mdns_context_t* mdns_context_head = ezlopi_mdns_get_service_context();
    if (mdns_context_head)
    {
        while (mdns_context_head)
        {
            size++;
            mdns_context_head = mdns_context_head->next;
        }
        *service_size = size;
        mdns_context = (mdns_txt_item_t*)malloc(size * sizeof(mdns_txt_item_t));
        if (mdns_context)
        {
            int i = 0;
            mdns_context_head = ezlopi_mdns_get_service_context();
            while (mdns_context_head)
            {
                memcpy(&mdns_context[i], mdns_context_head->mdns_context, sizeof(mdns_txt_item_t));
                i++;
                mdns_context_head = mdns_context_head->next;
            }
        }
        else
        {
            TRACE_E("Error allocating memory for mdns_context.");
            mdns_context = NULL;
        }
    }
    else
    {
        TRACE_E("Error receiving service contexts.");
        mdns_context = NULL;
    }
    return mdns_context;
}

static void __mdns_init(void* pv)
{
    int service_size;
    ezlopi_mdns_init_service_context();
    while (1)
    {
        ezlopi_wait_for_wifi_to_connect(portMAX_DELAY);

        esp_err_t err = mdns_init();
        if (err == ESP_OK)
        {

            uint32_t serial_last4 = 0;
            uint64_t id_val = ezlopi_factory_info_v3_get_id();
            if (id_val)
            {
                serial_last4 = id_val % 10000;
            }

            char hostname[EZPI_MDNS_HOSTNAME_SIZE];
            snprintf(hostname, EZPI_MDNS_HOSTNAME_SIZE, "%s_%d", ezlopi_ezlopi_str, serial_last4);

            err = mdns_hostname_set(hostname);
            mdns_instance_name_set("EzloPi mdns string");
            TRACE_I("Successful mDNS Initialization, %s", esp_err_to_name(err));

            mdns_txt_item_t* mdns_context = prepare_mdns_item_service_context(&service_size);
            if (mdns_context)
            {
                TRACE_I("-------- Adding mDNS Service(count: %d) ------------ ", service_size);
                TRACE_I("\tKEY\t\t\t\tValue");
                for (int i = 0; i < service_size; i++)
                {
                    TRACE_I("\t%s\t%s", mdns_context[i].key, mdns_context[i].value);
                }
                mdns_service_add(ezlopi_mdns_instance_name, "_ezlo", "_tcp", 17001, mdns_context, service_size);
                break;
            }
            else
            {
                TRACE_E("No mDNS context found.");
            }
        }
        else
        {
            TRACE_E("Error mDNS Initialization")
        }
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);

}