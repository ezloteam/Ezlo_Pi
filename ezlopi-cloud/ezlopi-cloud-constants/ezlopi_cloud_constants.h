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

#ifndef __EZLOPI_CLOUD_CONSTANTS_H__
#define __EZLOPI_CLOUD_CONSTANTS_H__
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
//------------------- MACRO FOR COMPARING EQUAL STR ------------------------------------------------------------

/**
 * @brief Macro to returnn the larger string length
 *
 *
 */
#define BIGGER_LEN(str1_len, str2_len) ((str1_len > str2_len) ? str1_len : str2_len)
/**
 * @brief Macro to compre two strings
 *
 *
 */
#define COMPARE_IF_EQUAL(STR1, STR2, STR1_LEN, STR2_LEN) (strncmp(STR1, STR2, BIGGER_LEN(STR1_LEN, STR2_LEN)) == 0)
/**
 * @brief Macro to compre two strings if they are equal
 *
 *
 */
#define EZPI_STRNCMP_IF_EQUAL(STR1, STR2, STR1_LEN, STR2_LEN) \
    ((NULL == STR1)                         ? false           \
     : (NULL == STR2)                       ? false           \
     : ((0 == STR1_LEN) && (0 == STR2_LEN)) ? false           \
                                            : COMPARE_IF_EQUAL(STR1, STR2, STR1_LEN, STR2_LEN))
//-------------------------------------------------------------------------------------------------------------

#include "ezlopi_cloud_keywords.h"
#include "ezlopi_cloud_scales_str.h"
#include "ezlopi_cloud_methods_str.h"
#include "ezlopi_cloud_devices_str.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_item_name_str.h"
#include "ezlopi_cloud_room_types_str.h"
#include "ezlopi_cloud_thermostat_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_cloud_device_types_str.h"


    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // __EZLOPI_CLOUD_CONSTANTS_H__

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
