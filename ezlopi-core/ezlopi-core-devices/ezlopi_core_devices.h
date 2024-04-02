#ifndef EZLOPI_CORE_DEVICES_H
#define EZLOPI_CORE_DEVICES_H

// #include "cJSON.h"

#include "ezlopi_core_cloud.h"
#include "ezlopi_core_actions.h"
#include "ezlopi_core_settings.h"
#include "ezlopi_core_cjson_macros.h"

#include "ezlopi_hal_adc.h"
#include "ezlopi_hal_pwm.h"
#include "ezlopi_hal_uart.h"
#include "ezlopi_hal_gpio.h"
#include "ezlopi_hal_onewire.h"
#include "ezlopi_hal_i2c_master.h"
#include "ezlopi_hal_spi_master.h"

// #include "ezlopi_cloud_settings.h"
#define CJSON_GET_VALUE_GPIO(root, item_name, item_val)       \
    {                                                         \
        cJSON *o_item = cJSON_GetObjectItem(root, item_name); \
        if (o_item && o_item->type == cJSON_Number)           \
        {                                                     \
            item_val = o_item->valuedouble;                      \
        }                                                     \
        else                                                  \
        {                                                     \
            item_val = -1;                                    \
            TRACE_E("%s not found!", item_name);              \
        }                                                     \
    }

// TRACE_I("%s: %d", item_name, item_val);

#define CJSON_GET_VALUE_STRING(root, item_name, item_val)     \
    {                                                         \
        cJSON *o_item = cJSON_GetObjectItem(root, item_name); \
        if (o_item && o_item->valuestring)                    \
        {                                                     \
            item_val = o_item->valuestring;                   \
        }                                                     \
        else                                                  \
        {                                                     \
            item_val = NULL;                                  \
            TRACE_E("%s: NULL", item_name);                   \
        }                                                     \
    }
// TRACE_I("%s: %s", item_name, item_val ? item_val : "");

#define CJSON_GET_VALUE_STRING_BY_COPY(root, item_name, item_val)     \
    {                                                                 \
        char *tmp_item_val = NULL;                                    \
        CJSON_GET_VALUE_STRING(root, item_name, tmp_item_val);        \
        if (tmp_item_val)                                             \
        {                                                             \
            snprintf(item_val, sizeof(item_val), "%s", tmp_item_val); \
        }                                                             \
    }

#define ASSIGN_DEVICE_NAME(digital_io_device_properties, dev_name)                                \
    {                                                                                             \
        if ((NULL != dev_name) && ('\0' != dev_name[0]))                                          \
        {                                                                                         \
            snprintf(digital_io_device_properties->ezlopi_cloud.device_name,                      \
                     sizeof(digital_io_device_properties->ezlopi_cloud.device_name),              \
                     "%s", dev_name);                                                             \
        }                                                                                         \
        else                                                                                      \
        {                                                                                         \
            snprintf(digital_io_device_properties->ezlopi_cloud.device_name,                      \
                     sizeof(digital_io_device_properties->ezlopi_cloud.device_name),              \
                     "dev-%d:digital_out", digital_io_device_properties->ezlopi_cloud.device_id); \
        }                                                                                         \
    }

// typedef int (*f_item_func_t)(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg);

typedef enum e_ezlopi_device_interface_type
{
    EZLOPI_DEVICE_INTERFACE_NONE = 0,
    EZLOPI_DEVICE_INTERFACE_DIGITAL_OUTPUT = 1,
    EZLOPI_DEVICE_INTERFACE_DIGITAL_INPUT = 2,
    EZLOPI_DEVICE_INTERFACE_ANALOG_INPUT = 3,
    EZLOPI_DEVICE_INTERFACE_ANALOG_OUTPUT = 4,
    EZLOPI_DEVICE_INTERFACE_PWM = 5,
    EZLOPI_DEVICE_INTERFACE_UART = 6,
    EZLOPI_DEVICE_INTERFACE_ONEWIRE_MASTER = 7,
    EZLOPI_DEVICE_INTERFACE_I2C_MASTER = 8,
    EZLOPI_DEVICE_INTERFACE_SPI_MASTER = 9,
    EZLOPI_DEVICE_INTERFACE_MAX
} e_ezlopi_device_interface_type_t;

typedef struct s_ezlopi_saved_item_info
{
    uint32_t item_id;
    bool favorite;
} s_ezlopi_saved_item_info_t;

typedef struct l_ezlopi_item
{
    s_ezlopi_saved_item_info_t saved_info;
    s_ezlopi_cloud_item_t cloud_properties;
    e_ezlopi_device_interface_type_t interface_type;
    union
    {
        s_ezlopi_uart_t uart;
        s_ezlopi_i2c_master_t i2c_master;
        s_ezlopi_spi_master_t spi_master;
        s_ezlopi_onewire_t onewire_master;
        s_ezlopi_gpios_t gpio;
        s_ezlopi_pwm_t pwm;
        s_ezlopi_adc_t adc;
    } interface;

    void* user_arg;
    bool is_user_arg_unique;
    int (*func)(e_ezlopi_actions_t action, struct l_ezlopi_item* item, void* arg, void* user_arg);

    struct l_ezlopi_item* next;
} l_ezlopi_item_t;

typedef struct l_ezlopi_device_settings_v3
{
    void* user_arg;
    s_ezlopi_cloud_device_settings_t cloud_properties;
    int (*func)(e_ezlopi_settings_action_t action, struct l_ezlopi_device_settings_v3* setting, void* arg, void* user_arg);
    struct l_ezlopi_device_settings_v3* next;

} l_ezlopi_device_settings_v3_t;

typedef struct l_ezlopi_device
{
    l_ezlopi_item_t* items;
    l_ezlopi_device_settings_v3_t* settings;
    s_ezlopi_cloud_device_t cloud_properties;
    struct l_ezlopi_device* next;
} l_ezlopi_device_t;

void ezlopi_device_prepare(void);

l_ezlopi_device_t* ezlopi_device_get_head(void);
l_ezlopi_device_t* ezlopi_device_add_device(cJSON* cj_device , const char * last_name);

l_ezlopi_device_t* ezlopi_device_get_by_id(uint32_t device_id);
l_ezlopi_item_t* ezlopi_device_get_item_by_id(uint32_t item_id);
l_ezlopi_device_settings_v3_t* ezlopi_device_settings_get_by_id(uint32_t settings_id);

// l_ezlopi_item_t *ezlopi_device_add_item_to_device(l_ezlopi_device_t *device);
l_ezlopi_item_t* ezlopi_device_add_item_to_device(l_ezlopi_device_t* device,
    int (*item_func)(e_ezlopi_actions_t action, struct l_ezlopi_item* item, void* arg, void* user_arg));

l_ezlopi_device_settings_v3_t* ezlopi_device_add_settings_to_device_v3(l_ezlopi_device_t* device,
    int (*setting_func)(e_ezlopi_settings_action_t action, struct l_ezlopi_device_settings_v3* setting, void* arg, void* user_arg));

void ezlopi_device_free_device(l_ezlopi_device_t* device);
void ezlopi_device_free_device_by_item(l_ezlopi_item_t* item);
void ezlopi_device_factory_info_reset(void);
cJSON* ezlopi_device_create_device_table_from_prop(l_ezlopi_device_t* device_prop);
s_ezlopi_cloud_controller_t* ezlopi_device_get_controller_information(void);
void ezlopi_device_name_set_by_device_id(uint32_t device_id, cJSON* cj_new_name);

#endif // EZLOPI_CORE_DEVICES_H
