#include "jsn_sr04t.h"
#include "trace.h"
#include "ezlopi_cloud_value_type_str.h"
#include "ezlopi_device_value_updated.h"

jsn_sr04t_config_t jsn_sr04t_config;
jsn_sr04t_data_t jsn_sr04t_data;
char *message;
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
void log_raw_data(jsn_sr04t_raw_data_t jsn_sr04t_raw_data)
{
    ESP_LOGD(TAG1, "data_received = %u", jsn_sr04t_raw_data.data_received);
    ESP_LOGD(TAG1, "IS AN EROOR = %u", jsn_sr04t_raw_data.is_an_error);
    ESP_LOGD(TAG1, "RAW = %d", jsn_sr04t_raw_data.raw);
    ESP_LOGD(TAG1, "distance in cm = %f", jsn_sr04t_raw_data.distance_cm);
}

void jsn_sr04t_print_data(jsn_sr04t_data_t jsn_sr04t_data)
{
    ESP_LOGD(TAG1, "data_received = %u", jsn_sr04t_data.data_received);
    ESP_LOGD(TAG1, "IS AN EROOR = %u", jsn_sr04t_data.is_an_error);
    ESP_LOGI(TAG1, "distance in cm = %f", jsn_sr04t_data.distance_cm);
}

int JSN_SR04T(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg)
{
    int ret = 0;

    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = ezlopi_JSN_SR04T_prepare_and_add(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = ezlopi_JSN_SR04T_init(properties);
        break;
    }
    // case EZLOPI_ACTION_NOTIFY_200_MS:
    // case EZLOPI_ACTION_SET_VALUE:
    // {
    //     // TRACE_B("HEre");
    //     ret = ezlopi_JSN_SR04T_update_value(properties, arg);
    //     break;
    // }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = ezlopi_JSN_SR04T_get_value_cjson(properties, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        ret = ezlopi_JSN_SR04T_update_value(properties, arg);
        break;
    }
    default:
    {
        break;
    }
    }

    return ret;
}

static int ezlopi_JSN_SR04T_prepare_and_add(void *args)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)args;

    if ((NULL != device_prep_arg) && (NULL != device_prep_arg->cjson_device))
    {
        s_ezlopi_device_properties_t *JSN_SR04T_sensor_properties = JSN_SR04T_sensor_prepare(device_prep_arg->cjson_device);
        if (JSN_SR04T_sensor_properties)
        {
            if (0 == ezlopi_devices_list_add(device_prep_arg->device, JSN_SR04T_sensor_properties, NULL))
            {
                free(JSN_SR04T_sensor_properties);
            }
            else
            {
                ret = 1;
            }
        }
    }

    return ret;
}

static s_ezlopi_device_properties_t *JSN_SR04T_sensor_prepare(cJSON *cjson_device)
{
    s_ezlopi_device_properties_t *JSN_SR04T_sensor_properties = malloc(sizeof(s_ezlopi_device_properties_t));

    if (JSN_SR04T_sensor_properties)
    {
        memset(JSN_SR04T_sensor_properties, 0, sizeof(s_ezlopi_device_properties_t));
        JSN_SR04T_sensor_properties->interface_type = EZLOPI_DEVICE_INTERFACE_DIGITAL_OUTPUT;

        char *device_name = NULL;
        CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
        ASSIGN_DEVICE_NAME(JSN_SR04T_sensor_properties, device_name);
        JSN_SR04T_sensor_properties->ezlopi_cloud.category = category_level_sensor;
        JSN_SR04T_sensor_properties->ezlopi_cloud.subcategory = subcategory_water;
        JSN_SR04T_sensor_properties->ezlopi_cloud.item_name = ezlopi_item_name_water_level_alarm;
        JSN_SR04T_sensor_properties->ezlopi_cloud.device_type = dev_type_sensor;
        JSN_SR04T_sensor_properties->ezlopi_cloud.value_type = value_type_token;
        JSN_SR04T_sensor_properties->ezlopi_cloud.has_getter = true;
        JSN_SR04T_sensor_properties->ezlopi_cloud.has_setter = false;
        JSN_SR04T_sensor_properties->ezlopi_cloud.reachable = true;
        JSN_SR04T_sensor_properties->ezlopi_cloud.battery_powered = false;
        JSN_SR04T_sensor_properties->ezlopi_cloud.show = true;
        JSN_SR04T_sensor_properties->ezlopi_cloud.room_name[0] = '\0';
        JSN_SR04T_sensor_properties->ezlopi_cloud.device_id = ezlopi_cloud_generate_device_id();
        JSN_SR04T_sensor_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
        JSN_SR04T_sensor_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();

        CJSON_GET_VALUE_INT(cjson_device, "gpio_in", JSN_SR04T_sensor_properties->interface.gpio.gpio_in.gpio_num);
        CJSON_GET_VALUE_INT(cjson_device, "gpio_out", JSN_SR04T_sensor_properties->interface.gpio.gpio_out.gpio_num);

        JSN_SR04T_sensor_properties->interface.gpio.gpio_out.enable = true;
        JSN_SR04T_sensor_properties->interface.gpio.gpio_out.interrupt = GPIO_INTR_DISABLE;
        JSN_SR04T_sensor_properties->interface.gpio.gpio_out.invert = EZLOPI_GPIO_LOGIC_NONINVERTED;
        JSN_SR04T_sensor_properties->interface.gpio.gpio_out.mode = GPIO_MODE_OUTPUT;
        JSN_SR04T_sensor_properties->interface.gpio.gpio_out.pull = GPIO_PULLDOWN_ONLY;
        JSN_SR04T_sensor_properties->interface.gpio.gpio_out.value = 0;

        JSN_SR04T_sensor_properties->interface.gpio.gpio_in.enable = true;
        JSN_SR04T_sensor_properties->interface.gpio.gpio_in.interrupt = GPIO_INTR_DISABLE;
        JSN_SR04T_sensor_properties->interface.gpio.gpio_in.invert = EZLOPI_GPIO_LOGIC_NONINVERTED;
        JSN_SR04T_sensor_properties->interface.gpio.gpio_in.mode = GPIO_MODE_INPUT;
        JSN_SR04T_sensor_properties->interface.gpio.gpio_in.pull = GPIO_PULLDOWN_DISABLE;
        JSN_SR04T_sensor_properties->interface.gpio.gpio_in.value = 0;
    }

    return JSN_SR04T_sensor_properties;
}

static int ezlopi_JSN_SR04T_init(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;
    jsn_sr04t_config = (jsn_sr04t_config_t)JSN_SR04T_CONFIG_DEFAULT();
    jsn_sr04t_config.trigger_gpio_num = properties->interface.gpio.gpio_out.gpio_num;
    jsn_sr04t_config.echo_gpio_num = properties->interface.gpio.gpio_in.gpio_num;
    jsn_sr04t_config.rmt_channel = 4;
    if (ESP_OK == init_JSN_SR04T(&jsn_sr04t_config))
    {
        TRACE_I("JSN_SR04T initialized");
        ret = 1;
    }
    else
    {
        TRACE_E("JSN_SR04T not initializeed");
    }
    return ret;
}

static int ezlopi_JSN_SR04T_update_value(s_ezlopi_device_properties_t *properties, void *arg)
{
    int ret = 0;
    jsn_sr04t_data = (jsn_sr04t_data_t)JSN_SR04T_DATA_DEFAULT();
    TRACE_I("Measuring the distance");
    ret = measurement(&jsn_sr04t_config, &jsn_sr04t_data);
    if (ESP_OK == ret)
    {
        jsn_sr04t_print_data(jsn_sr04t_data);

        // arg = &jsn_sr04t_data;
    }
    else
    {
        ESP_LOGE(TAG1, "ERROR in getting measurement");
        message = "unknown";
        // ezlopi_device_value_updated_from_device(properties);
    }
    ezlopi_device_value_updated_from_device(properties);
    vTaskDelay(pdMS_TO_TICKS(1000));
    return ret;
}

static int ezlopi_JSN_SR04T_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args)
{
    int ret = 0;
    cJSON *cjson_propertise = (cJSON *)args;
    if (cjson_propertise)
    {
        // cJSON_AddNumberToObject(cjson_propertise, "value", jsn_sr04t_data.distance_cm);
        if (jsn_sr04t_data.distance_cm >= 50 && jsn_sr04t_data.distance_cm < 100)
        {
            cJSON_AddStringToObject(cjson_propertise, "value", "water_level_ok");
        }
        else if (jsn_sr04t_data.distance_cm >= 100)
        {
            cJSON_AddStringToObject(cjson_propertise, "value", "water_level_below_low_threshold");
        }
        else if (jsn_sr04t_data.distance_cm < 50)
        {
            cJSON_AddStringToObject(cjson_propertise, "value", "water_level_above_high_threshold");
        }
        else
        {
            // cJSON_AddStringToObject(cjson_propertise, "value", "unknown");
            cJSON_AddStringToObject(cjson_propertise, "value", *message);
        }
        ret = 1;
    }
    TRACE_B("%s", cJSON_Print(cjson_propertise));
    return ret;
}

esp_err_t init_JSN_SR04T(jsn_sr04t_config_t *jsn_sr04t_config)
{
    esp_err_t ret = ESP_OK;
    ESP_LOGD(TAG1, "%s()", __FUNCTION__);

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
        ESP_LOGE(TAG1, "%s(). ABORT. error configuring the trigger gpio pin", __FUNCTION__);
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
        ESP_LOGE(TAG1, "%s(). ABORT. error configuring the echo gpio pin", __FUNCTION__);
        goto err;
    }

    if (jsn_sr04t_config->no_of_samples == 0)
    {
        ret = ESP_ERR_INVALID_ARG;
        ESP_LOGE(TAG1, "%s(). ABORT. jsn_sr04t_config->nbr_of_samples cannot be 0", __FUNCTION__);
    }

    if (jsn_sr04t_config->is_init == true)
    {
        ret = ESP_ERR_INVALID_ARG;
        ESP_LOGE(TAG1, "%s(). ABORT. already init'd", __FUNCTION__);
    }

    /*
     *       RMT
     */
    rmt_config_t rx_config = RMT_CONFIG_JSN_SR04T_ECHO(jsn_sr04t_config->echo_gpio_num, jsn_sr04t_config->rmt_channel);
    ret = rmt_config(&rx_config);
    if (ESP_OK != ret)
    {
        ret = ESP_ERR_INVALID_RESPONSE;
        ESP_LOGE(TAG1, "%s(). ABORT. error configuring the RMT configuration", __FUNCTION__);
        goto err;
    }

    ret = rmt_driver_install(rx_config.channel, 2048, 0);
    if (ESP_OK != ret)
    {
        ret = ESP_ERR_INVALID_RESPONSE;
        ESP_LOGE(TAG1, "%s(). ABORT. error installing the RMT Driver", __FUNCTION__);
        goto err;
    }

    jsn_sr04t_config->is_init = true;

err:
    return ret;
}

esp_err_t raw_measeurement(jsn_sr04t_config_t *jsn_sr04t_config, jsn_sr04t_raw_data_t *jsn_sr04t_raw_data)
{
    esp_err_t ret = ESP_OK;
    ESP_LOGD(TAG1, "%s()", __FUNCTION__);

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
        ESP_LOGD("Length", "Received RMT words = %d", length);
        rmt_item32_t *temp_ptr = items; // Use a temporary pointer (=pointing to the beginning of the item array)
        for (uint8_t i = 0; i < length; i++)
        {
            ESP_LOGD(TAG1, "  %2i :: [level 0]: %1d - %5d microsec, [level 1]: %3d - %5d microsec",
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
            ESP_LOGE(TAG1, "%s(). ABORT. Out Of Range: distance_cm < %d (%f) ", __FUNCTION__,
                     minimum_detection_value_in_cm, jsn_sr04t_raw_data->distance_cm);

            jsn_sr04t_raw_data->is_an_error = true;
            goto err;
        }

        if (jsn_sr04t_raw_data->distance_cm > maximum_detection_value_in_cm)
        {
            ret = ESP_ERR_INVALID_RESPONSE;
            ESP_LOGE(TAG1, "%s(). ABORT. Out Of Range: distance_cm < %d (%f) ", __FUNCTION__,
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
                ESP_LOGE(TAG1,
                         "%s(). ABORT. Invalid value: adjusted distance <= 0 (subtracted sensor_artifact_cm) (%f) | err %i (%s)",
                         __FUNCTION__,
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

esp_err_t measurement(jsn_sr04t_config_t *jsn_sr04t_config, jsn_sr04t_data_t *jsn_sr04t_data)
{
    esp_err_t ret = ESP_OK;
    uint32_t count_errors = 0;
    double distance = 0;
    ESP_LOGD(TAG1, "%s()", __FUNCTION__);

    jsn_sr04t_data->data_received = false;
    jsn_sr04t_data->is_an_error = false;
    jsn_sr04t_data->distance_cm = 0.0;

    jsn_sr04t_raw_data_t sample[jsn_sr04t_config->no_of_samples];

    for (int i = 0; i < jsn_sr04t_config->no_of_samples; i++)
    {
        ret = raw_measeurement(jsn_sr04t_config, &sample[i]);
        if (ESP_OK != ret)
        {
            ESP_LOGE(TAG1, "ERROR in reading");
            goto err;
        }
        log_raw_data(sample[i]);
        if (sample[i].is_an_error == true)
        {
            ESP_LOGE(TAG1, "ERROR");
            count_errors += 1;
        }
        distance += sample[i].distance_cm;
    }

    if (count_errors > 0)
    {
        jsn_sr04t_data->is_an_error = true;
        ret = ESP_ERR_INVALID_RESPONSE;
        ESP_LOGE(TAG1, "%s(). Abort At least one measurement is incorrect", __FUNCTION__);
        goto err;
    }

    jsn_sr04t_data->data_received = true;
    jsn_sr04t_data->distance_cm = distance / jsn_sr04t_config->no_of_samples;

err:
    return ret;
}