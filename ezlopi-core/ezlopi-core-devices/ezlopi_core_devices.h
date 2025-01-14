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
 * @file    ezlopi_core_devices.h
 * @brief   perform some function on core-devices
 * @author  ezlopi_team_np
 * @version 0.1
 * @date    12th DEC 2024
 */

#ifndef _EZLOPI_CORE_DEVICES_H_
#define _EZLOPI_CORE_DEVICES_H_

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

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
     * @brief function to prepare the devices
     *
     */
    void EZPI_device_prepare(void);
    /**
     * @brief Funtion to return head-node of device-ll
     *
     * @return l_ezlopi_device_t*
     */
    l_ezlopi_device_t *EZPI_core_device_get_head(void);
    /**
     * @brief Function to add devices to ll
     *
     * @param cj_device new device to add
     * @param last_name Additional name suffix to be added to new_device name
     * @return l_ezlopi_device_t*
     */
    l_ezlopi_device_t *EZPI_core_device_add_device(cJSON *cj_device, const char *last_name);
    /**
     * @brief Function to return device-node by id
     *
     * @param device_id Target device-id
     * @return l_ezlopi_device_t*
     */
    l_ezlopi_device_t *EZPI_core_device_get_by_id(uint32_t device_id);
    /**
     * @brief Function returns item-node by id
     *
     * @param item_id Target item-id
     * @return l_ezlopi_item_t*
     */
    l_ezlopi_item_t *EZPI_core_device_get_item_by_id(uint32_t item_id);
    /**
     * @brief Function to return settings node
     *
     * @param settings_id Target setting id
     * @return l_ezlopi_device_settings_v3_t*
     */
    // l_ezlopi_device_settings_v3_t *EZPI_core_device_settings_get_by_id(uint32_t settings_id);

    /**
     * @brief function to add new-item to device-node
     *
     * @param device target device node
     * @param item_func item-function to trigger
     * @return l_ezlopi_item_t*
     */
    // l_ezlopi_item_t *EZPI_core_device_add_item_to_device(l_ezlopi_device_t *device, ezlopi_error_t(*item_func)(e_ezlopi_actions_t action, struct l_ezlopi_item *item, void *arg, void *user_arg));
    l_ezlopi_item_t *EZPI_core_device_add_item_to_device(l_ezlopi_device_t *device, ezlopi_error_t (*item_func)(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg));
    /**
     * @brief Function to add new-setting to device
     *
     * @param device target device node
     * @param setting_func setting-funtion to trigger
     * @return l_ezlopi_device_settings_v3_t*
     */
    l_ezlopi_device_settings_v3_t *EZPI_core_device_add_settings_to_device_v3(l_ezlopi_device_t *device, int (*setting_func)(e_ezlopi_settings_action_t action, struct l_ezlopi_device_settings_v3 *setting, void *arg, void *user_arg));
    /**
     * @brief Function to free target device node
     *
     * @param device Target device-node
     */
    void EZPI_core_device_free_device(l_ezlopi_device_t *device);
    /**
     * @brief Function to free device node
     *
     * @param item target item-node
     */
    void EZPI_core_device_free_device_by_item(l_ezlopi_item_t *item);
    /**
     * @brief Function to reset to factory info
     *
     */
    void EZPI_core_device_factory_info_reset(void);
    /**
     * @brief Function to prepare device prop from ll
     *
     * @param device_prop Device-node to start from
     * @return cJSON*
     */
    cJSON *EZPI_core_device_create_device_table_from_prop(l_ezlopi_device_t *device_prop);
    /**
     * @brief Function to return controller information
     *
     * @return s_ezlopi_cloud_controller_t*
     */
    s_ezlopi_cloud_controller_t *EZPI_core_device_get_controller_information(void);
    /**
     * @brief Function to set device name to target device-node
     *
     * @param device_id Id of Target device_node
     * @param new_dev_name New name of device-node
     */
    void EZPI_core_device_name_set_by_device_id(uint32_t device_id, const char *new_dev_name);
    /**
     * @brief Function to set device room-id to respective device_nodes
     *
     * @param device_id Target ID of device_node
     * @param room_id_str room_id to be set
     * @param cj_separate_child_devices List of child devices to reset flag for.
     */
    void EZPI_core_device_set_device_room_id(uint32_t device_id, const char *room_id_str, cJSON *cj_separate_child_devices);
    /**
     * @brief Function to set/reset device armed status
     *
     * @param device_id  Target device-id
     * @param armed Flag status
     */
    void EZPI_core_device_set_reset_device_armed_status(uint32_t device_id, bool armed);

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_CORE_DEVICES_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
