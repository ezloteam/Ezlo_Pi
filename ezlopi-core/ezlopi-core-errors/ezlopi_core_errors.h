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

#ifndef __HEADER_H__
#define __HEADER_H__

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "stdint.h"
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
typedef int32_t ezlopi_error_t;

#define EZPI_SUCCESS                                                    0
#define EZPI_FAILED                                                     -1
    // NVS Related errors
#define EZPI_ERR_NVS_INIT_FAILED                                        0x00000001
// Not implemented
#define EZPI_ERR_NVS_READ_FAILED                                        0x00000002
// Not implemented
#define EZPI_ERR_NVS_WRITE_FAILED                                       0x00000004

// Factory info init related errors
// Not implemented
#define EZPI_ERR_FACTORY_INFO_READ_FAILED                               0x00000008
// Event group related errors
#define EZPI_ERR_EVENT_GROUP_UNINITIALIZED                              0x00000010 // It is returned if event group handle is uninitialized
#define EZPI_ERR_EVENT_GROUP_BIT_WAIT_FAILED                            0x00000020 // It is returned if evvent group waits till timeout without event bit(s) being set
#define EZPI_ERR_EVENT_GROUP_BIT_UNDEFINED                              0x00000040 // If bit set/clear/wait encounters undefined bit or bit value greater than BIT31
// Generic/Test device preperation errors
#define EZPI_ERR_JSON_PARSE_FAILED                                      0x00000080 // It is returned when device configuration parsing fails
// Device intitaliation errors
#define EZPI_ERR_PREP_DEVICE_PREP_FAILED                                0x00000100 // It is returned when device preperation fails when PREPARE event is called on every devices
// Not implemented
#define EZPI_ERR_INIT_DEVICE_FAILED                                     0x00000200 // If any error occurs during initialization, this error is returned to abort futher initialization of the device in case it has to be deleted from the tree
// Room related errors
#define EZPI_ERR_ROOM_INIT_FAILED                                       0x00000400 // If any error occured duing the initialization of the core room module this error is returned
// Core event queue related errors
#define EZPI_ERR_EVENT_QUEUE_INIT_FAILED                                0x00000800 // Failed to initialized event queue
#define EZPI_ERR_EVENT_QUEUE_UNINITIALIZED                              0x00001000 // For uninitialized event queue
#define EZPI_ERR_EVENT_QUEUE_RECV_SEND_ERROR                            0x00002000 // For queue if send or receive fails
// Core SNTP init sync related errors
#define EZPI_ERR_SNTP_INIT_FAILED                                       0x00004000 // For SNTP if failed to sync the status
#define EZPI_ERR_SNTP_LOCATION_SET_FAILED                               0x00008000 // For SNTP if failed to set location

#define EZPI_ERR_HAL_INIT_FAILED                                        0x00010000 // For HAL init failed 

#define EZPI_ERR_BROADCAST_FAILED                                       0x00020000 // Any error occuring during broadcast throws this error

#define EZPI_ERR_MODES_FAILED                                           0x00040000 // Any error occcured during modes related operations


// #warning("Create error code for invalid and empty params")

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
       // void EZPI_core_errors_functiontitle(type_t arg);
    void EZPI_core_error_assert_on_error(ezlopi_error_t error, uint32_t error_to_assert_on);
    const char *EZPI_core_error_code_to_str(ezlopi_error_t error_code);

#ifdef __cplusplus
}
#endif

#endif // __HEADER_H__

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
