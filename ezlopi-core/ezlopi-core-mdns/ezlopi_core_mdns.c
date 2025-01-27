/* ===========================================================================
** Copyright (C) 2025 Ezlo Innovation Inc
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
 * @file    ezlopi_core_mdns.c
 * @brief   Function to operate on mdns
 * @author
 * @version 1.0
 * @date    February 24th, 2024 12:20 AM
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_SERV_MDNS_EN

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

#include "ezlopi_util_trace.h"
#include "ezlopi_cloud_keywords.h"

#include "ezlopi_core_wifi.h"
#include "ezlopi_core_mdns.h"
#include "ezlopi_core_processes.h"
#include "ezlopi_core_factory_info.h"

#include "EZLOPI_USER_CONFIG.h"

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
static void __ezlopi_mdns_add_service_context(l_ezlopi_mdns_context_t *new_context);
static l_ezlopi_mdns_context_t *__ezlopi_mdns_get_service_context();
static void __ezlopi_mdns_init_service_context();
static mdns_txt_item_t *__prepare_mdns_item_service_context(int *service_size);
static void __mdns_init(void *pv);

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/
static l_ezlopi_mdns_context_t *ezlopi_mdns_service_cntx = NULL;

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
ezlopi_error_t EZPI_init_mdns(void)
{
    ezlopi_error_t ret = EZPI_SUCCESS;

    TaskHandle_t ezlopi_core_mdns_service_task_handle = NULL;

#warning "__mdns_init task can be shifted to loop"
    xTaskCreate(__mdns_init, "mdns_svc", EZLOPI_CORE_MDNS_SERVICE_TASK_DEPTH, NULL, 4, &ezlopi_core_mdns_service_task_handle);

#if defined(CONFIG_FREERTOS_USE_TRACE_FACILITY)
    EZPI_core_process_set_process_info(ENUM_EZLOPI_CORE_MDNS_SERVICE_TASK, &ezlopi_core_mdns_service_task_handle, EZLOPI_CORE_MDNS_SERVICE_TASK_DEPTH);
#endif

    return ret;
}

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/
static void __ezlopi_mdns_add_service_context(l_ezlopi_mdns_context_t *new_context)
{
    if (new_context)
    {
        if (ezlopi_mdns_service_cntx == NULL)
        {
            ezlopi_mdns_service_cntx = new_context;
        }
        else
        {
            l_ezlopi_mdns_context_t *head_context = ezlopi_mdns_service_cntx;
            while (head_context->next)
            {
                head_context = head_context->next;
            }
            head_context->next = new_context;
        }
    }
}

static l_ezlopi_mdns_context_t *__ezlopi_mdns_get_service_context()
{
    return ezlopi_mdns_service_cntx;
}

static void __ezlopi_mdns_init_service_context()
{

    // Add one service about EzloPi Device Type
    l_ezlopi_mdns_context_t *ezlopi_mdns_service_cntx_device_type = (l_ezlopi_mdns_context_t *)ezlopi_malloc(__FUNCTION__, sizeof(l_ezlopi_mdns_context_t));
    if (ezlopi_mdns_service_cntx_device_type)
    {
        memset(ezlopi_mdns_service_cntx_device_type, 0, sizeof(l_ezlopi_mdns_context_t));
        mdns_txt_item_t *service_cntx_device_type = (mdns_txt_item_t *)ezlopi_malloc(__FUNCTION__, sizeof(mdns_txt_item_t));
        if (service_cntx_device_type)
        {
            memset(service_cntx_device_type, 0, sizeof(mdns_txt_item_t));
            service_cntx_device_type->key = ezlopi_ezlopi_device_type_str;
            service_cntx_device_type->value = (NULL == EZPI_core_factory_info_v3_get_device_type() ? "null" : EZPI_core_factory_info_v3_get_device_type());

            ezlopi_mdns_service_cntx_device_type->mdns_context = service_cntx_device_type;

            __ezlopi_mdns_add_service_context(ezlopi_mdns_service_cntx_device_type);
        }
    }

    // Add one service about EzloPi Serial
    l_ezlopi_mdns_context_t *ezlopi_mdns_service_cntx_device_id = (l_ezlopi_mdns_context_t *)ezlopi_malloc(__FUNCTION__, sizeof(l_ezlopi_mdns_context_t));

    if (ezlopi_mdns_service_cntx_device_id)
    {
        memset(ezlopi_mdns_service_cntx_device_id, 0, sizeof(l_ezlopi_mdns_context_t));

        mdns_txt_item_t *service_cntx_device_id = (mdns_txt_item_t *)ezlopi_malloc(__FUNCTION__, sizeof(mdns_txt_item_t));
        if (service_cntx_device_id)
        {
            memset(service_cntx_device_id, 0, sizeof(mdns_txt_item_t));
            uint64_t id_val = EZPI_core_factory_info_v3_get_id();
            if (id_val)
            {
                char *id_val_str = (char *)ezlopi_malloc(__FUNCTION__, EZPI_MDNS_SERIAL_SIZE);
                if (id_val_str)
                {
                    service_cntx_device_id->key = "Serial";

                    memset(id_val_str, 0, EZPI_MDNS_SERIAL_SIZE);
                    snprintf(id_val_str, EZPI_MDNS_SERIAL_SIZE, "%lld", id_val);
                    service_cntx_device_id->value = (NULL == id_val_str ? "null" : id_val_str);

                    ezlopi_mdns_service_cntx_device_id->mdns_context = service_cntx_device_id;
                    __ezlopi_mdns_add_service_context(ezlopi_mdns_service_cntx_device_id);
                }
                else
                {
                    ezlopi_free(__FUNCTION__, service_cntx_device_id);
                    ezlopi_free(__FUNCTION__, ezlopi_mdns_service_cntx_device_id);
                    return;
                }
            }
            else
            {
                ezlopi_free(__FUNCTION__, service_cntx_device_id);
                ezlopi_free(__FUNCTION__, ezlopi_mdns_service_cntx_device_id);
                return;
            }
        }
        else
        {
            ezlopi_free(__FUNCTION__, ezlopi_mdns_service_cntx_device_id);
            return;
        }
    }

    // Add manufacturer service
#ifdef CONFIG_EZPI_MDNS_ENABLE_MANUFACTURER_SERVICE
    l_ezlopi_mdns_context_t *ezlopi_mdns_service_cntx_manufacturer = (l_ezlopi_mdns_context_t *)ezlopi_malloc(__FUNCTION__, sizeof(l_ezlopi_mdns_context_t));
    if (ezlopi_mdns_service_cntx_manufacturer)
    {
        memset(ezlopi_mdns_service_cntx_manufacturer, 0, sizeof(l_ezlopi_mdns_context_t));
        mdns_txt_item_t *service_cntx_manufacturer = (mdns_txt_item_t *)ezlopi_malloc(__FUNCTION__, sizeof(mdns_txt_item_t));
        if (service_cntx_manufacturer)
        {
            memset(service_cntx_manufacturer, 0, sizeof(mdns_txt_item_t));
            service_cntx_manufacturer->key = ezlopi_manufacturer_str;
            service_cntx_manufacturer->value = (NULL == EZPI_core_factory_info_v3_get_manufacturer() ? "null" : EZPI_core_factory_info_v3_get_manufacturer());

            ezlopi_mdns_service_cntx_manufacturer->mdns_context = service_cntx_manufacturer;

            __ezlopi_mdns_add_service_context(ezlopi_mdns_service_cntx_manufacturer);
        }
    }
#endif // CONFIG_EZPI_MDNS_ENABLE_MANUFACTURER_SERVICE

    // Add brand service
#ifdef CONFIG_EZPI_MDNS_ENABLE_BRAND_SERVICE
    l_ezlopi_mdns_context_t *ezlopi_mdns_service_cntx_brand = (l_ezlopi_mdns_context_t *)ezlopi_malloc(__FUNCTION__, sizeof(l_ezlopi_mdns_context_t));
    if (ezlopi_mdns_service_cntx_brand)
    {
        memset(ezlopi_mdns_service_cntx_brand, 0, sizeof(l_ezlopi_mdns_context_t));
        mdns_txt_item_t *service_cntx_brand = (mdns_txt_item_t *)ezlopi_malloc(__FUNCTION__, sizeof(mdns_txt_item_t));
        if (service_cntx_brand)
        {
            memset(service_cntx_brand, 0, sizeof(mdns_txt_item_t));
            service_cntx_brand->key = ezlopi_brand_str;
            service_cntx_brand->value = (NULL == EZPI_core_factory_info_v3_get_brand() ? "null" : EZPI_core_factory_info_v3_get_brand());

            ezlopi_mdns_service_cntx_brand->mdns_context = service_cntx_brand;

            __ezlopi_mdns_add_service_context(ezlopi_mdns_service_cntx_brand);
        }
    }
#endif // CONFIG_EZPI_MDNS_ENABLE_BRAND_SERVICE

    // Add model service
#ifdef CONFIG_EZPI_MDNS_ENABLE_MODEL_SERVICE
    l_ezlopi_mdns_context_t *ezlopi_mdns_service_cntx_model = (l_ezlopi_mdns_context_t *)ezlopi_malloc(__FUNCTION__, sizeof(l_ezlopi_mdns_context_t));
    if (ezlopi_mdns_service_cntx_model)
    {
        memset(ezlopi_mdns_service_cntx_model, 0, sizeof(l_ezlopi_mdns_context_t));
        mdns_txt_item_t *service_cntx_model = (mdns_txt_item_t *)ezlopi_malloc(__FUNCTION__, sizeof(mdns_txt_item_t));
        if (service_cntx_model)
        {
            memset(service_cntx_model, 0, sizeof(mdns_txt_item_t));
            service_cntx_model->key = ezlopi_model_str;
            service_cntx_model->value = (NULL == EZPI_core_factory_info_v3_get_model() ? "null" : EZPI_core_factory_info_v3_get_model());

            ezlopi_mdns_service_cntx_model->mdns_context = service_cntx_model;

            __ezlopi_mdns_add_service_context(ezlopi_mdns_service_cntx_model);
        }
    }
#endif // CONFIG_EZPI_MDNS_ENABLE_MODEL_SERVICE

    // Add name service
#ifdef CONFIG_EZPI_MDNS_ENABLE_NAME_SERVICE
    l_ezlopi_mdns_context_t *ezlopi_mdns_service_cntx_name = (l_ezlopi_mdns_context_t *)ezlopi_malloc(__FUNCTION__, sizeof(l_ezlopi_mdns_context_t));
    if (ezlopi_mdns_service_cntx_name)
    {
        memset(ezlopi_mdns_service_cntx_name, 0, sizeof(l_ezlopi_mdns_context_t));
        mdns_txt_item_t *service_cntx_name = (mdns_txt_item_t *)ezlopi_malloc(__FUNCTION__, sizeof(mdns_txt_item_t));
        if (service_cntx_name)
        {
            memset(service_cntx_name, 0, sizeof(mdns_txt_item_t));
            service_cntx_name->key = ezlopi_name_str;
            service_cntx_name->value = (NULL == EZPI_core_factory_info_v3_get_name() ? "null" : EZPI_core_factory_info_v3_get_name());

            ezlopi_mdns_service_cntx_name->mdns_context = service_cntx_name;

            __ezlopi_mdns_add_service_context(ezlopi_mdns_service_cntx_name);
        }
    }
#endif // CONFIG_EZPI_MDNS_ENABLE_NAME_SERVICE
}

static mdns_txt_item_t *__prepare_mdns_item_service_context(int *service_size)
{
    mdns_txt_item_t *mdns_context = NULL;
    int size = 0;
    l_ezlopi_mdns_context_t *mdns_context_head = __ezlopi_mdns_get_service_context();
    if (mdns_context_head)
    {
        while (mdns_context_head)
        {
            size++;
            mdns_context_head = mdns_context_head->next;
        }
        *service_size = size;
        mdns_context = (mdns_txt_item_t *)ezlopi_malloc(__FUNCTION__, size * sizeof(mdns_txt_item_t));
        if (mdns_context)
        {
            int i = 0;
            mdns_context_head = __ezlopi_mdns_get_service_context();
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

static void __mdns_init(void *pv)
{
    int service_size;
    __ezlopi_mdns_init_service_context();

#warning "NABIN: mdns_init() is called in a continuous while loop"
    while (1)
    {
        EZPI_core_wait_for_wifi_to_connect(portMAX_DELAY);

        esp_err_t err = mdns_init();
        if (err == ESP_OK)
        {

            uint32_t serial_last4 = 0;
            uint64_t id_val = EZPI_core_factory_info_v3_get_id();
            if (id_val)
            {
                serial_last4 = id_val % 10000;
            }

            char hostname[EZPI_MDNS_HOSTNAME_SIZE];
            snprintf(hostname, EZPI_MDNS_HOSTNAME_SIZE, "%s_%d", ezlopi_ezlopi_str, serial_last4);

            err = mdns_hostname_set(hostname);
            mdns_instance_name_set("EzloPi mdns string");
            TRACE_I("Successful mDNS Initialization, %s", esp_err_to_name(err));

            mdns_txt_item_t *mdns_context = __prepare_mdns_item_service_context(&service_size);
            if (mdns_context)
            {
                TRACE_I("-------- Adding mDNS Service(count: %d) ------------ ", service_size);
                if (service_size)
                {
                    TRACE_I("\tKEY\t\t\tValue");
                }

                for (int i = 0; i < service_size; i++)
                {
                    TRACE_I("\t%s\t\t%s", mdns_context[i].key, mdns_context[i].value);
                }
                mdns_service_add("EzloPi", "_ezlo", "_tcp", 17001, mdns_context, service_size);
                break;
            }
            else
            {
                TRACE_E("No mDNS context found.");
            }
        }
        else
        {
            TRACE_E("Error mDNS Initialization");
        }
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
#if defined(CONFIG_FREERTOS_USE_TRACE_FACILITY)

    EZPI_core_process_set_is_deleted(ENUM_EZLOPI_CORE_MDNS_SERVICE_TASK);
#endif
    vTaskDelete(NULL);
}

#endif // CONFIG_EZPI_SERV_MDNS_EN

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
