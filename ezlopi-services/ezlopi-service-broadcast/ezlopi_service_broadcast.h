
/**
 * @file    ezlopi_service_broadcast.h
 * @brief
 * @authors Krishna Kumar Sah (work.krishnasah@gmail.com)
 * @version
 * @date
 */
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
 * @file    ezlopi_service_broadcast.h
 * @brief   Contains function declarations for data broadcast
 * @author  
 * @version 1.0
 * @date    February 27, 2024
 */

#ifndef __EZLOPI_SERVICE_BROADCAST_H__
#define __EZLOPI_SERVICE_BROADCAST_H__

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

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

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/
    /**
     * @brief Function to initialize broad cast service
     * @details This function performs following operations
     *  - Sets broadcast queue sender
     *  - Initializes the broadcast loop
     *
     */
    void EZPI_service_broadcast_init(void);

#ifdef __cplusplus
}
#endif

#endif // __EZLOPI_SERVICE_BROADCAST_H__

/*******************************************************************************
 *                          End of File
 *******************************************************************************/