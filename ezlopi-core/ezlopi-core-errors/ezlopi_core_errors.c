/* ===========================================================================
** Copyright (C) 2022 Ezlo Innovation Inc
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

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
/**
 * Include Sequence
 * 1. C standard libraries eg. stdint, string etc.
 * 2. Third party component eg. cJSON, freeRTOS etc etc
 * 3. ESP-IDF specific eg. driver/gpio, esp_log etc.
 * 4. EzloPi HAL specific eg ezlopi_hal_i2c
 * 5. EzloPi core specific eg. ezlopi_core_ble
 * 6. EzloPi cloud specific eg. ezlopi_cloud_items
 * 7. EzloPi Service specific eg ezlopi_service_ble
 * 8. EzloPi Sensors and Device Specific eg. ezlopi_device_0001_digitalout_generic
 */
#include <assert.h>

#include "ezlopi_core_errors.h"
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

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/

/**
 * @brief Global/extern function template example
 * Convention : Use capital letter for initial word on extern function
 * @param arg
 */

void EZPI_core_error_assert_on_error(ezlopi_error_t error, uint32_t error_to_assert_on)
{
    assert(error != error_to_assert_on);
}

const char *EZPI_core_error_code_to_str(ezlopi_error_t error_code)
{
    const char *code_str = NULL;
    switch (error_code)
    {
    case EZPI_SUCCESS:
    {
        code_str = "EZPI_SUCCESS";
        break;
    }
    case EZPI_FAILED:
    {
        code_str = "EZPI_FAILED";
        break;
    }
    case EZPI_ERR_NVS_INIT_FAILED:
    {
        code_str = "EZPI_ERR_NVS_INIT_FAILED";
        break;
    }
    case EZPI_ERR_NVS_READ_FAILED:
    {
        code_str = "EZPI_ERR_NVS_READ_FAILED";
        break;
    }
    case EZPI_ERR_NVS_WRITE_FAILED:
    {
        code_str = "EZPI_ERR_NVS_WRITE_FAILED";
        break;
    }
    case EZPI_ERR_FACTORY_INFO_READ_FAILED:
    {
        code_str = "EZPI_ERR_FACTORY_INFO_READ_FAILED";
        break;
    }
    case EZPI_ERR_EVENT_GROUP_UNINITIALIZED:
    {
        code_str = "EZPI_ERR_EVENT_GROUP_UNINITIALIZED";
        break;
    }
    case EZPI_ERR_EVENT_GROUP_BIT_WAIT_FAILED:
    {
        code_str = "EZPI_ERR_EVENT_GROUP_BIT_WAIT_FAILED";
        break;
    }
    case EZPI_ERR_EVENT_GROUP_BIT_UNDEFINED:
    {
        code_str = "EZPI_ERR_EVENT_GROUP_BIT_UNDEFINED";
        break;
    }
    case EZPI_ERR_JSON_PARSE_FAILED:
    {
        code_str = "EZPI_ERR_JSON_PARSE_FAILED";
        break;
    }
    case EZPI_ERR_PREP_DEVICE_PREP_FAILED:
    {
        code_str = "EZPI_ERR_PREP_DEVICE_PREP_FAILED";
        break;
    }
    case EZPI_ERR_INIT_DEVICE_FAILED:
    {
        code_str = "EZPI_ERR_INIT_DEVICE_FAILED";
        break;
    }
    case EZPI_ERR_ROOM_INIT_FAILED:
    {
        code_str = "EZPI_ERR_ROOM_INIT_FAILED";
        break;
    }
    case EZPI_ERR_EVENT_QUEUE_INIT_FAILED:
    {
        code_str = "EZPI_ERR_EVENT_QUEUE_INIT_FAILED";
        break;
    }
    case EZPI_ERR_EVENT_QUEUE_UNINITIALIZED:
    {
        code_str = "EZPI_ERR_EVENT_QUEUE_UNINITIALIZED";
        break;
    }
    case EZPI_ERR_EVENT_QUEUE_RECV_SEND_ERROR:
    {
        code_str = "EZPI_ERR_EVENT_QUEUE_RECV_SEND_ERROR";
        break;
    }
    case EZPI_ERR_SNTP_INIT_FAILED:
    {
        code_str = "EZPI_ERR_SNTP_INIT_FAILED";
        break;
    }
    case EZPI_ERR_SNTP_LOCATION_SET_FAILED:
    {
        code_str = "EZPI_ERR_SNTP_LOCATION_SET_FAILED";
        break;
    }
    case EZPI_ERR_HAL_INIT_FAILED:
    {
        code_str = "EZPI_ERR_HAL_INIT_FAILED";
        break;
    }
    case EZPI_ERR_BROADCAST_FAILED:
    {
        code_str = "EZPI_ERR_BROADCAST_FAILED";
        break;
    }
    case EZPI_ERR_MODES_FAILED:
    {
        code_str = "EZPI_ERR_MODES_FAILED";
        break;
    }
    default:
        break;
    }
    return code_str;
}

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/

// static void ezpi_subcomponent_functiontitle(type_t arg)
// {
// }

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
