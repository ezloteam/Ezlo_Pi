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
 * @file    ezlopi_core_ble_buffer.c
 * @brief   perform some function on ble-operations
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 *          Lomas Subedi
 *          Riken Maharjan
 *          Nabin Dangi
 * @version 0.1
 * @date    12th DEC 2024
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_BLE_ENABLE

#include <string.h>

#include "ezlopi_core_ble_buffer.h"
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

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/

s_linked_buffer_t *EZPI_core_ble_buffer_create(esp_ble_gatts_cb_param_t *param)
{
    s_linked_buffer_t *linked_buffer = ezlopi_malloc(__FUNCTION__, sizeof(s_linked_buffer_t));
    if (linked_buffer)
    {
        memset(linked_buffer, 0, sizeof(s_linked_buffer_t));
        if ((NULL != param->write.value) && (param->write.len > 0))
        {
            linked_buffer->buffer = ezlopi_malloc(__FUNCTION__, param->write.len);
            if (linked_buffer->buffer)
            {
                linked_buffer->len = param->write.len;
                memcpy(linked_buffer->buffer, param->write.value, param->write.len);
            }
            else
            {
                ezlopi_free(__FUNCTION__, linked_buffer);
                linked_buffer = NULL;
            }
        }
        else
        {
            ezlopi_free(__FUNCTION__, linked_buffer);
            linked_buffer = NULL;
        }
    }

    return linked_buffer;
}

void EZPI_core_ble_buffer_add_to_buffer(s_linked_buffer_t *buffer, esp_ble_gatts_cb_param_t *param)
{
    while (buffer->next)
    {
        buffer = buffer->next;
    }

    buffer->next = EZPI_core_ble_buffer_create(param);
}

void EZPI_core_ble_buffer_free_buffer(s_linked_buffer_t *l_buffer)
{
    if (l_buffer)
    {
        if (l_buffer->buffer)
        {
            ezlopi_free(__FUNCTION__, l_buffer->buffer);
            l_buffer->buffer = NULL;
        }
        EZPI_core_ble_buffer_free_buffer(l_buffer->next);
        l_buffer->next = NULL;
        ezlopi_free(__FUNCTION__, l_buffer);
    }
}

void EZPI_core_ble_buffer_accumulate_to_start(s_linked_buffer_t *l_buffer)
{
    if (l_buffer)
    {
        uint32_t tot_len = 0;
        s_linked_buffer_t *tmp_buffer = l_buffer;
        while (tmp_buffer)
        {
            tot_len += tmp_buffer->len;
            tmp_buffer = tmp_buffer->next;
        }

        uint8_t *tot_buffer = ezlopi_malloc(__FUNCTION__, tot_len + 1);
        if (tot_buffer)
        {
            memset(tot_buffer, 0, tot_len + 1);
            uint32_t pos = 0;
            tmp_buffer = l_buffer;
            while (tmp_buffer)
            {
                memcpy(&tot_buffer[pos], tmp_buffer->buffer, tmp_buffer->len);
                pos += tmp_buffer->len;
                tmp_buffer = tmp_buffer->next;
            }

            if (l_buffer->buffer)
            {
                ezlopi_free(__FUNCTION__, l_buffer->buffer);
                l_buffer->buffer = NULL;
            }

            l_buffer->buffer = tot_buffer;
            l_buffer->len = tot_len;
        }

        EZPI_core_ble_buffer_free_buffer(l_buffer->next);
        l_buffer->next = NULL;
    }
}
/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          End of File
 *******************************************************************************/

#endif // CONFIG_EZPI_BLE_ENABLE