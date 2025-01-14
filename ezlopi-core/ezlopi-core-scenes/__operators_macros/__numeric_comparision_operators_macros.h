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
 * @file    __numeric_comparision_operators_macros.h
 * @brief   MACROS to generate enums for numeric comparision operators
 * @author  ezlopi_team_np
 * @version 0.1
 * @date    12th DEC 2024
 */

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
    SCENES_NUM_COMP_OPERATORS(NONE, NULL, NULL, NULL)
    //////////////////////////////////////////////////////////////////////////////
    SCENES_NUM_COMP_OPERATORS(LESS, "<", "less", "compareNumbers")
    SCENES_NUM_COMP_OPERATORS(GREATER, ">", "greater", "compareNumbers")
    SCENES_NUM_COMP_OPERATORS(LESS_EQUAL, "<=", "less equal", "compareNumbers")
    SCENES_NUM_COMP_OPERATORS(GREATER_EQUAL, ">=", "greater equal", "compareNumbers")
    SCENES_NUM_COMP_OPERATORS(EQUAL, "==", "equal", "compareNumbers")
    SCENES_NUM_COMP_OPERATORS(NOT_EQUAL, "!=", "not equal", "compareNumbers")

    SCENES_NUM_COMP_OPERATORS(BETWEEN, "between", "between", "compareNumberRange")
    SCENES_NUM_COMP_OPERATORS(NOT_BETWEEN, "not_between", "not between", "compareNumberRange")
    // SCENES_NUM_COMP_OPERATORS(ANY_OF, "any_of", "any of", "numbersArray")
    // SCENES_NUM_COMP_OPERATORS(NONE_OF, "none_of", "none of", "numbersArray")
    //////////////////////////////////////////////////////////////////////////////
    SCENES_NUM_COMP_OPERATORS(MAX, NULL, NULL, NULL)

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
