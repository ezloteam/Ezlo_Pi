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
 * @file    ezlopi_service_ble_ble_auth.h
 * @brief
 * @author
 * @version
 * @date
 */
#ifndef _EZLOPI_SERVICE_BLE_BLE_AUT_H_
#define _EZLOPI_SERVICE_BLE_BLE_AUT_H_

#ifdef CONFIG_EZPI_BLE_ENABLE

/**
 * @brief Enum for authentiucation status
 *
 */
typedef enum e_auth_status
{
    BLE_AUTH_SUCCESS = 1,                   /**< Authentication success status */
    BLE_AUTH_STATUS_UNKNOWN = 0,            /**< Authentication unknown status */
    BLE_AUTH_USER_ID_NOT_FOUND = -1,        /**< User not found status */
    BLE_AUTH_USER_ID_NOT_MATCHED = -2,      /**< User not matched status */
    BLE_AUTH_USER_ID_NVS_WRITE_FAILED = -3, /**< User id write to nvs failed status */
} e_auth_status_t;

/**
 * @brief Function returns last authentication status for the user
 *
 * @return e_auth_status_t
 * @retval Authentication status
 */
e_auth_status_t ezlopi_ble_auth_last_status(void);
/**
 * @brief Function to store user ID
 *
 * @param user_id Pointer to the user ID
 * @return e_auth_status_t
 * @retval Authentication status
 */
e_auth_status_t ezlopi_ble_auth_store_user_id(char *user_id);
/**
 * @brief Function to check user ID
 *
 * @param user_id Pointer to the user ID
 * @return e_auth_status_t
 * @retval Authentication status
 */
e_auth_status_t ezlopi_ble_auth_check_user_id(char *user_id);
/**
 * @brief Function to convert user authentication status to string
 *
 * @param status Authentication status
 * @return char*
 * @retval Authentication status converted to string
 */
char *ezlopi_ble_auth_status_to_string(e_auth_status_t status);

#endif // CONFIG_EZPI_BLE_ENABLE

#endif // _EZLOPI_SERVICE_BLE_BLE_AUT_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
