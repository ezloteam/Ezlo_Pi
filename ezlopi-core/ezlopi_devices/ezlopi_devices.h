#ifndef __EZLOPI_DEVICES_H__
#define __EZLOPI_DEVICES_H__

#include <cJSON.h>

#include "ezlopi_gpio.h"
#include "ezlopi_uart.h"
#include "ezlopi_i2c_master.h"
#include "ezlopi_spi_master.h"
#include "ezlopi_onewire.h"
#include "ezlopi_cloud.h"
#include "ezlopi_pwm.h"
#include "ezlopi_adc.h"
#include "ezlopi_uart.h"
#include "ezlopi_actions.h"
#include "ezlopi_settings.h"

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

    void *user_arg;
    int (*func)(e_ezlopi_actions_t action, struct l_ezlopi_item *item, void *arg, void *user_arg);

    struct l_ezlopi_item *next;
} l_ezlopi_item_t;

typedef struct l_ezlopi_device_settings_v3
{
    s_ezlopi_cloud_device_settings_t cloud_properties;
    void *user_arg;
    int (*func)(e_ezlopi_settings_action_t action, struct l_ezlopi_device_settings_v3 *setting, void *arg, void *user_arg);
    struct l_ezlopi_device_settings_v3 *next;

} l_ezlopi_device_settings_v3_t;
typedef struct l_ezlopi_device
{
    uint32_t parent_device_id;
    l_ezlopi_item_t *items;
    l_ezlopi_device_settings_v3_t *settings;
    s_ezlopi_cloud_device_t cloud_properties;
    struct l_ezlopi_device *next;
} l_ezlopi_device_t;

void ezlopi_device_prepare(void);

l_ezlopi_device_t *ezlopi_device_get_head(void);
l_ezlopi_device_t *ezlopi_device_add_device(void);

l_ezlopi_device_t *ezlopi_device_get_by_id(uint32_t device_id);
l_ezlopi_item_t *ezlopi_device_get_item_by_id(uint32_t item_id);

// l_ezlopi_item_t *ezlopi_device_add_item_to_device(l_ezlopi_device_t *device);
l_ezlopi_item_t *ezlopi_device_add_item_to_device(l_ezlopi_device_t *device,
                                                  int (*item_func)(e_ezlopi_actions_t action, struct l_ezlopi_item *item, void *arg, void *user_arg));

l_ezlopi_device_settings_v3_t *ezlopi_device_add_settings_to_device_v3(l_ezlopi_device_t *device,
                                                                       int (*setting_func)(e_ezlopi_settings_action_t action, struct l_ezlopi_device_settings_v3 *setting, void *arg, void *user_arg));

void ezlopi_device_free_device(l_ezlopi_device_t *device);
void ezlopi_device_free_device_by_item(l_ezlopi_item_t *item);
void ezlopi_device_factory_info_reset(void);

cJSON *ezlopi_device_create_device_table_from_prop(l_ezlopi_device_t *device_prop);
s_ezlopi_cloud_controller_t *ezlopi_device_get_controller_information(void);

#endif // __EZLOPI_DEVICE_H__
