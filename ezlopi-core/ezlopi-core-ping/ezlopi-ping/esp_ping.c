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
* @file    esp_ping.c
* @brief   Function to perform pings
* @author  xx
* @version 0.1
* @date    12th DEC 2024
*/

/*******************************************************************************
*                          Include Files
*******************************************************************************/


#include <string.h>
#include "esp_ping.h"

#include "lwip/ip_addr.h"

/*******************************************************************************
*                          Extern Data Declarations
*******************************************************************************/

/*******************************************************************************
*                          Extern Function Declarations
*******************************************************************************/

/*******************************************************************************
*                          Type & Macro Definitions
*******************************************************************************/

typedef struct _ping_option {
    ip_addr_t ping_target;
    uint32_t ping_count;
    uint32_t ping_rcv_timeout;
    uint32_t ping_delay;
    uint32_t interface;
    size_t ping_data_len;
    uint16_t ping_id;
    u8_t ping_tos;
    u8_t ping_ttl;
    esp_ping_found_fn ping_res_fn;
    esp_ping_found    ping_res;
    void *ping_reserve;
} s_ping_option_t;

/*******************************************************************************
*                          Static Function Prototypes
*******************************************************************************/

/*******************************************************************************
*                          Static Data Definitions
*******************************************************************************/
static s_ping_option_t _ping_option_info[1];

/*******************************************************************************
*                          Extern Data Definitions
*******************************************************************************/

/*******************************************************************************
*                          Extern Function Definitions
*******************************************************************************/

esp_err_t esp_ping_set_target(ping_target_id_t opt_id, void *opt_val, uint32_t opt_len)
{
    esp_err_t ret = ESP_OK;

    if (opt_val == NULL)
    {
        return ESP_ERR_PING_INVALID_PARAMS;
    }

    switch (opt_id)
    {
    case PING_TARGET_IP_ADDRESS:
        ipaddr_aton(opt_val, &(_ping_option_info->ping_target));
        break;
    case PING_TARGET_IP_ADDRESS_COUNT:
        ESP_PING_CHECK_OPTLEN(opt_len, uint32_t);
        _ping_option_info->ping_count = *(uint32_t *)opt_val;
        break;
    case PING_TARGET_IF_INDEX:
        ESP_PING_CHECK_OPTLEN(opt_len, uint32_t);
        _ping_option_info->interface = *(uint32_t *)opt_val;
        break;
    case PING_TARGET_RCV_TIMEO:
        ESP_PING_CHECK_OPTLEN(opt_len, uint32_t);
        _ping_option_info->ping_rcv_timeout = (*(uint32_t *)opt_val);
        break;
    case PING_TARGET_DELAY_TIME:
        ESP_PING_CHECK_OPTLEN(opt_len, uint32_t);
        _ping_option_info->ping_delay = (*(uint32_t *)opt_val);
        break;
    case PING_TARGET_DATA_LEN:
        ESP_PING_CHECK_OPTLEN(opt_len, size_t);
        _ping_option_info->ping_data_len = (*(size_t *)opt_val);
        break;
    case PING_TARGET_ID:
        ESP_PING_CHECK_OPTLEN(opt_len, uint16_t);
        _ping_option_info->ping_id = *(uint16_t *)opt_val;
        break;
    case PING_TARGET_IP_TOS:
        ESP_PING_CHECK_OPTLEN(opt_len, u8_t);
        _ping_option_info->ping_tos = *(u8_t *)opt_val;
        break;
    case PING_TARGET_RES_FN:
        _ping_option_info->ping_res_fn = opt_val;
        break;
    case PING_TARGET_RES_RESET:
        memset(&_ping_option_info->ping_res, 0, sizeof(_ping_option_info->ping_res));
        break;
    default:
        ret = ESP_ERR_PING_INVALID_PARAMS;
        break;
    }

    return ret;
}

esp_err_t esp_ping_get_target(ping_target_id_t opt_id, void *opt_val, uint32_t opt_len)
{
    esp_err_t ret = ESP_OK;

    if (opt_val == NULL)
    {
        return ESP_ERR_PING_INVALID_PARAMS;
    }

    switch (opt_id)
    {
    case PING_TARGET_IP_ADDRESS:
        ip_addr_copy(*(ip_addr_t *)opt_val, _ping_option_info->ping_target);
        break;
    case PING_TARGET_IP_ADDRESS_COUNT:
        ESP_PING_CHECK_OPTLEN(opt_len, uint32_t);
        *(uint32_t *)opt_val = _ping_option_info->ping_count;
        break;
    case PING_TARGET_IF_INDEX:
        ESP_PING_CHECK_OPTLEN(opt_len, uint32_t);
        *(uint32_t *)opt_val = _ping_option_info->interface;
        break;
    case PING_TARGET_RCV_TIMEO:
        ESP_PING_CHECK_OPTLEN(opt_len, uint32_t);
        *(uint32_t *)opt_val = _ping_option_info->ping_rcv_timeout;
        break;
    case PING_TARGET_DELAY_TIME:
        ESP_PING_CHECK_OPTLEN(opt_len, uint32_t);
        *(uint32_t *)opt_val = _ping_option_info->ping_delay;
        break;
    case PING_TARGET_DATA_LEN:
        ESP_PING_CHECK_OPTLEN(opt_len, size_t);
        *(size_t *)opt_val = _ping_option_info->ping_data_len;
        break;
    case PING_TARGET_ID:
        ESP_PING_CHECK_OPTLEN(opt_len, uint16_t);
        *(uint16_t *)opt_val = _ping_option_info->ping_id;
        break;
    case PING_TARGET_IP_TOS:
        ESP_PING_CHECK_OPTLEN(opt_len, uint16_t);
        *(uint16_t *)opt_val = _ping_option_info->ping_tos;
        break;
    default:
        ret = ESP_ERR_PING_INVALID_PARAMS;
        break;
    }

    return ret;
}

esp_err_t esp_ping_result(uint8_t res_val, uint16_t ping_len, uint32_t ping_time)
{
    esp_err_t ret = ESP_OK;

    _ping_option_info->ping_res.ping_err = res_val;

    if (res_val != PING_RES_FINISH)
    {
        _ping_option_info->ping_res.bytes = ping_len;
        _ping_option_info->ping_res.resp_time = ping_time;
        _ping_option_info->ping_res.total_bytes += ping_len;
        _ping_option_info->ping_res.send_count++;

        if (res_val == PING_RES_TIMEOUT)
        {
            _ping_option_info->ping_res.timeout_count++;
        }
        else
        {
            if (!_ping_option_info->ping_res.min_time || (ping_time < _ping_option_info->ping_res.min_time))
            {
                _ping_option_info->ping_res.min_time = ping_time;
            }

            if (ping_time > _ping_option_info->ping_res.max_time)
            {
                _ping_option_info->ping_res.max_time = ping_time;
            }


            _ping_option_info->ping_res.total_time += ping_time;
            _ping_option_info->ping_res.recv_count++;
        }
    }

    if (_ping_option_info->ping_res_fn)
    {
        _ping_option_info->ping_res_fn(PING_TARGET_RES_FN, &_ping_option_info->ping_res);
        if (res_val == PING_RES_FINISH)
        {
            memset(&_ping_option_info->ping_res, 0, sizeof(esp_ping_found));
        }
    }

    return ret;
}


/*******************************************************************************
*                         Static Function Definitions
*******************************************************************************/

/*******************************************************************************
*                          End of File
*******************************************************************************/