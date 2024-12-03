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

// #ifndef __HEADER_H__
// #define __HEADER_H__

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

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
    SCENES_STROPS_COMP_OPERATORES(NONE, NULL, NULL, NULL)
    //////////////////////////////////////////////////////////////////////////////
    SCENES_STROPS_COMP_OPERATORES(BEGINS_WITH, "begin", "begins with", "stringOperation")
    SCENES_STROPS_COMP_OPERATORES(ENDS_WITH, "end", "ends with", "stringOperation")
    SCENES_STROPS_COMP_OPERATORES(CONTAINS, "contain", "contains", "stringOperation")
    SCENES_STROPS_COMP_OPERATORES(LENGTH, "length", "length equal to", "stringOperation")
    SCENES_STROPS_COMP_OPERATORES(NOT_BEGIN, "not_begin", "doesn't begin with", "stringOperation")
    SCENES_STROPS_COMP_OPERATORES(NOT_END, "not_end", "doesn't end with", "stringOperation")
    SCENES_STROPS_COMP_OPERATORES(NOT_CONTAIN, "not_contain", "doesn't contain", "stringOperation")
    SCENES_STROPS_COMP_OPERATORES(NOT_LENGTH, "not_length", "length not equal to", "stringOperation")
    //////////////////////////////////////////////////////////////////////////////
    SCENES_STROPS_COMP_OPERATORES(MAX, NULL, NULL, NULL)

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
    void EZPI_maincomponent_subcomponent_functiontitle?( type_t arg );

#ifdef __cplusplus
}
#endif

#endif  // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

// #endif // __HEADER_H__

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
