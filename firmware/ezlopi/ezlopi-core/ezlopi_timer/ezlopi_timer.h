
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

#ifndef EZLOPI_TIMER_H
#define EZLOPI_TIMER_H

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/timer.h"
#include "esp_err.h"
#include "esp_log.h"

/*******************************************************************************
 *                          Type & Macro Declarations
 *******************************************************************************/
#define EZLOPI_TIMER_TAG "EZLOPI TIMER TAG"

#define EZLOPI_TIMER_GRP_0 TIMER_GROUP_0
#define EZLOPI_TIMER_GRP_1 TIMER_GROUP_1
#define EZLOPI_TIMER_IDX_0 TIMER_0
#if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32
#define EZLOPI_TIMER_IDX_1 TIMER_1
#endif

#define EZLOPI_TIMER_DIVIDER (1000) //  Hardware timer clock divider

#ifdef CONFIG_IDF_TARGET_ESP32S3
#define EZLOPI_TIMER_SCALE (XTAL_CLK_FREQ / (EZLOPI_TIMER_DIVIDER)) // convert counter value to seconds
#endif

#ifdef CONFIG_IDF_TARGET_ESP32
#define EZLOPI_TIMER_SCALE (APB_CLK_FREQ / EZLOPI_TIMER_DIVIDER) // convert counter value to seconds
#endif

#ifdef CONFIG_IDF_TARGET_ESP32C3
#define EZLOPI_TIMER_SCALE (APB_CLK_FREQ / EZLOPI_TIMER_DIVIDER) // convert counter value to seconds
#endif

void ezlopi_timer_start_50ms(void);
void ezlopi_timer_start_200ms(void);
// void ezlopi_timer_start_500ms(void);
void ezlopi_timer_start_1000ms(void);

#endif // EZLOPI_TIMER_H