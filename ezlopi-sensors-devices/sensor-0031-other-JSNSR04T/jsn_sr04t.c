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
 * @file    jsn_sr04t.c
 * @brief   perform some function on jsn_sr04t
 * @author
 * @version 0.1
 * @date    xx
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

#include "jsn_sr04t.h"
#include "ezlopi_util_trace.h"
#include "ezlopi_cloud_value_type_str.h"
#include "ezlopi_core_device_value_updated.h"

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/
/**
 * @brief Default configuration for RX channel
 *
 */
#define RMT_CONFIG_JSN_SR04T_ECHO(gpio, channel_id) \
    {                                               \
        .rmt_mode = RMT_MODE_RX,                    \
        .channel = channel_id,                      \
        .gpio_num = gpio,                           \
        .clk_div = 80,                              \
        .mem_block_num = 1,                         \
        .flags = 0,                                 \
        .rx_config = {                              \
            .idle_threshold = 20000,                \
            .filter_ticks_thresh = 100,             \
            .filter_en = true,                      \
        }                                           \
    }

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/
// static const char *TAG1 = "JSN_SR04T_V3";

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
void log_raw_data(jsn_sr04t_raw_data_t jsn_sr04t_raw_data)
{
    TRACE_D("data_received = %u", jsn_sr04t_raw_data.data_received);
    TRACE_D("IS AN EROOR = %u", jsn_sr04t_raw_data.is_an_error);
    TRACE_D("RAW = %d", jsn_sr04t_raw_data.raw);
    TRACE_D("distance in cm = %f", jsn_sr04t_raw_data.distance_cm);
}

esp_err_t JSN_sr04t_init(jsn_sr04t_config_t *jsn_sr04t_config)
{
    esp_err_t ret = ESP_OK;

    // GPIO's configurations
    gpio_config_t pin_config;

    pin_config.pin_bit_mask = (1ULL << jsn_sr04t_config->trigger_gpio_num);
    pin_config.mode = GPIO_MODE_OUTPUT;
    pin_config.pull_down_en = GPIO_PULLDOWN_ENABLE; // @important
    pin_config.pull_up_en = GPIO_PULLUP_DISABLE;
    pin_config.intr_type = GPIO_PIN_INTR_DISABLE;
    ret = gpio_config(&pin_config);
    if (ESP_OK != ret)
    {
        ret = ESP_ERR_INVALID_RESPONSE;
        TRACE_E("ABORT. error configuring the trigger gpio pin");
        goto err;
    }

    pin_config.pin_bit_mask = (1ULL << jsn_sr04t_config->echo_gpio_num);
    pin_config.mode = GPIO_MODE_INPUT;
    pin_config.pull_down_en = GPIO_PULLDOWN_DISABLE; // @important
    pin_config.pull_up_en = GPIO_PULLUP_DISABLE;
    pin_config.intr_type = GPIO_PIN_INTR_DISABLE;
    ret = gpio_config(&pin_config);
    if (ESP_OK != ret)
    {
        ret = ESP_ERR_INVALID_RESPONSE;
        TRACE_E("ABORT. error configuring the echo gpio pin");
        goto err;
    }

    if (jsn_sr04t_config->no_of_samples == 0)
    {
        ret = ESP_ERR_INVALID_ARG;
        TRACE_E("ABORT. jsn_sr04t_config->nbr_of_samples cannot be 0");
    }

    if (jsn_sr04t_config->is_init == true)
    {
        ret = ESP_ERR_INVALID_ARG;
        TRACE_E("ABORT. already init'd");
    }

    /*
     *       RMT
     */
    rmt_config_t rx_config = RMT_CONFIG_JSN_SR04T_ECHO(jsn_sr04t_config->echo_gpio_num, jsn_sr04t_config->rmt_channel);
    ret = rmt_config(&rx_config);
    if (ESP_OK != ret)
    {
        ret = ESP_ERR_INVALID_RESPONSE;
        TRACE_E("ABORT. error configuring the RMT configuration");
        goto err;
    }

    ret = rmt_driver_install(rx_config.channel, 2048, 0);
    if (ESP_OK != ret)
    {
        ret = ESP_ERR_INVALID_RESPONSE;
        TRACE_E("ABORT. error installing the RMT Driver");
        goto err;
    }

    jsn_sr04t_config->is_init = true;

err:
    return ret;
}

esp_err_t JSN_sr04t_raw_calc(jsn_sr04t_config_t *jsn_sr04t_config, jsn_sr04t_raw_data_t *jsn_sr04t_raw_data)
{
    esp_err_t ret = ESP_OK;

    // Reset receive values
    jsn_sr04t_raw_data->data_received = false;
    jsn_sr04t_raw_data->is_an_error = false;
    jsn_sr04t_raw_data->raw = 0;
    jsn_sr04t_raw_data->distance_cm = 0.0;

    RingbufHandle_t rb = NULL;
    rmt_item32_t *items = NULL;
    size_t length = 0;

    // get RMT RX ringbuffer
    rmt_get_ringbuf_handle(jsn_sr04t_config->rmt_channel, &rb);
    assert(rb != NULL);
    // Start receive
    rmt_rx_start(jsn_sr04t_config->rmt_channel, true);

    // initiate the measurement in the sensor
    gpio_set_level(jsn_sr04t_config->trigger_gpio_num, 0);
    ets_delay_us(60000);
    gpio_set_level(jsn_sr04t_config->trigger_gpio_num, 1);
    ets_delay_us(25);
    gpio_set_level(jsn_sr04t_config->trigger_gpio_num, 0);

    // begin to receive the timing
    items = (rmt_item32_t *)xRingbufferReceive(rb, &length, 100 / portTICK_PERIOD_MS);
    if (items)
    {
        length /= 4; // one RMT = 4 Bytes
        TRACE_I("Received RMT words = %d", length);
        rmt_item32_t *temp_ptr = items; // Use a temporary pointer (=pointing to the beginning of the item array)
        for (uint8_t i = 0; i < length; i++)
        {
            TRACE_D("  %2i :: [level 0]: %1d - %5d microsec, [level 1]: %3d - %5d microsec",
                    i,
                    temp_ptr->level0, temp_ptr->duration0,
                    temp_ptr->level1, temp_ptr->duration1);
            temp_ptr++;
        }

        jsn_sr04t_raw_data->data_received = true;
        jsn_sr04t_raw_data->raw = items->duration0;
        jsn_sr04t_raw_data->distance_cm = (jsn_sr04t_raw_data->raw / 2) * 0.0343; // sound velocity used here

        if (jsn_sr04t_raw_data->distance_cm < minimum_detection_value_in_cm)
        {
            ret = ESP_ERR_INVALID_RESPONSE;
            TRACE_E("ABORT. Out Of Range: distance_cm < %d (%f) ",
                    minimum_detection_value_in_cm, jsn_sr04t_raw_data->distance_cm);

            jsn_sr04t_raw_data->is_an_error = true;
            goto err;
        }

        if (jsn_sr04t_raw_data->distance_cm > maximum_detection_value_in_cm)
        {
            ret = ESP_ERR_INVALID_RESPONSE;
            TRACE_E("ABORT. Out Of Range: distance_cm < %d (%f) ",
                    maximum_detection_value_in_cm, jsn_sr04t_raw_data->distance_cm);
            jsn_sr04t_raw_data->is_an_error = true;
            goto err;
        }

        // ADJUST with distance_sensor_to_artifact_cm (default 0cm).
        if (jsn_sr04t_config->offset_cm != 0.0)
        {
            jsn_sr04t_raw_data->distance_cm -= jsn_sr04t_config->offset_cm;
            if (jsn_sr04t_raw_data->distance_cm <= 0.0)
            {
                ret = ESP_ERR_INVALID_RESPONSE;
                TRACE_E("ABORT. Invalid value: adjusted distance <= 0 (subtracted sensor_artifact_cm) (%f) | err %i (%s)",
                        jsn_sr04t_raw_data->distance_cm, ret, esp_err_to_name(ret));
                jsn_sr04t_raw_data->is_an_error = true;
                goto err;
            }
        }

        // after parsing the data, return spaces to ringbuffer.
        vRingbufferReturnItem(rb, (void *)items);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
    else
    {
        ret = ESP_ERR_INVALID_RESPONSE;
    }

err:
    rmt_rx_stop(jsn_sr04t_config->rmt_channel);

    return ret;
}

esp_err_t JSN_sr04t_measurement(jsn_sr04t_config_t *jsn_sr04t_config, jsn_sr04t_data_t *jsn_sr04t_data)
{
    esp_err_t ret = ESP_OK;
    uint32_t count_errors = 0;
    double distance = 0;

    jsn_sr04t_data->data_received = false;
    jsn_sr04t_data->is_an_error = false;
    jsn_sr04t_data->distance_cm = 0.0;

    jsn_sr04t_raw_data_t sample[jsn_sr04t_config->no_of_samples];

    for (int i = 0; i < jsn_sr04t_config->no_of_samples; i++)
    {
        ret = JSN_sr04t_raw_calc(jsn_sr04t_config, &sample[i]);
        if (ESP_OK != ret)
        {
            TRACE_E("ERROR in reading");
        }
        log_raw_data(sample[i]);
        if (sample[i].is_an_error == true)
        {
            TRACE_E("ERROR");
            count_errors += 1;
        }
        // distance += sample[i].distance_cm;
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }

    for (int i = 0; i < jsn_sr04t_config->no_of_samples; i++)
    {
        if (!sample[i].is_an_error)
        {
            distance += sample[i].distance_cm;
        }
    }

    jsn_sr04t_data->data_received = true;
    jsn_sr04t_data->distance_cm = distance / (jsn_sr04t_config->no_of_samples - count_errors);

    // err:
    return jsn_sr04t_data->data_received;
}

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          End of File
 *******************************************************************************/