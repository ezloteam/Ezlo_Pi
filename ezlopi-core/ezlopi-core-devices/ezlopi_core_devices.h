/* ===========================================================================
** Copyright (C) 2024 Ezlo Innovation Inc
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
 * @file    main.c
 * @brief   perform some function on data
 * @author  John Doe
 * @version 0.1
 * @date    1st January 2024
 */

#ifndef EZLOPI_CORE_DEVICES_H
#define EZLOPI_CORE_DEVICES_H

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "cjext.h"

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

/*******************************************************************************
 *                          C++ Declaration Wrapper
 *******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

    /*******************************************************************************
     *                          Type & Macro Declarations
     *******************************************************************************/
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
            s_ezlopi_adc_t adc;
            s_ezlopi_pwm_t pwm;
            s_ezlopi_uart_t uart;
            s_ezlopi_gpios_t gpio;
            s_ezlopi_i2c_master_t i2c_master;
            s_ezlopi_spi_master_t spi_master;
            s_ezlopi_onewire_t onewire_master;
        } interface;

        void *user_arg;
        bool is_user_arg_unique;
        ezlopi_error_t (*func)(e_ezlopi_actions_t action, struct l_ezlopi_item *item, void *arg, void *user_arg);

        struct l_ezlopi_item *next;
    } l_ezlopi_item_t;

    typedef struct l_ezlopi_device_settings_v3
    {
        void *user_arg;
        s_ezlopi_cloud_device_settings_t cloud_properties;
        int (*func)(e_ezlopi_settings_action_t action, struct l_ezlopi_device_settings_v3 *setting, void *arg, void *user_arg);
        struct l_ezlopi_device_settings_v3 *next;

    } l_ezlopi_device_settings_v3_t;

    typedef struct l_ezlopi_device
    {
        l_ezlopi_item_t *items;
        l_ezlopi_device_settings_v3_t *settings;
        s_ezlopi_cloud_device_t cloud_properties;
        struct l_ezlopi_device *next;
    } l_ezlopi_device_t;

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/
    /**
     * @brief Global function template example
     * Convention : Use capital letter for initial word on extern function
     * maincomponent : Main component as hal, core, service etc.
     * subcomponent : Sub component as i2c from hal, ble from service etc
     * functiontitle : Title of the function
     * eg : EZPI_hal_i2c_init()
     * @param arg
     *
     */
    void ezlopi_device_prepare(void);

    l_ezlopi_device_t *ezlopi_device_get_head(void);
    l_ezlopi_device_t *ezlopi_device_add_device(cJSON *cj_device, const char *last_name);

    l_ezlopi_device_t *ezlopi_device_get_by_id(uint32_t device_id);
    l_ezlopi_item_t *ezlopi_device_get_item_by_id(uint32_t item_id);
    l_ezlopi_device_settings_v3_t *ezlopi_device_settings_get_by_id(uint32_t settings_id);

    // l_ezlopi_item_t *ezlopi_device_add_item_to_device(l_ezlopi_device_t *device);
    l_ezlopi_item_t *ezlopi_device_add_item_to_device(l_ezlopi_device_t *device, ezlopi_error_t (*item_func)(e_ezlopi_actions_t action, struct l_ezlopi_item *item, void *arg, void *user_arg));

    l_ezlopi_device_settings_v3_t *ezlopi_device_add_settings_to_device_v3(l_ezlopi_device_t *device, int (*setting_func)(e_ezlopi_settings_action_t action, struct l_ezlopi_device_settings_v3 *setting, void *arg, void *user_arg));

    void ezlopi_device_free_device(l_ezlopi_device_t *device);
    void ezlopi_device_free_device_by_item(l_ezlopi_item_t *item);

    void ezlopi_device_factory_info_reset(void);
    cJSON *ezlopi_device_create_device_table_from_prop(l_ezlopi_device_t *device_prop);
    s_ezlopi_cloud_controller_t *ezlopi_device_get_controller_information(void);
    void ezlopi_device_name_set_by_device_id(uint32_t device_id, const char *new_dev_name);
    void ezlopi_device_set_device_room_id(uint32_t device_id, const char *room_id_str, cJSON *cj_separate_child_devices);
    void ezlopi_device_set_reset_device_armed_status(uint32_t device_id, bool armed);

#ifdef __cplusplus
}
#endif

#endif // EZLOPI_CORE_DEVICES_H

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
