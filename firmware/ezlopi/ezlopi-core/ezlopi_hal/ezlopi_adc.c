    

#include "trace.h"
#include "ezlopi_adc.h"
#include "string.h"
#include "stdlib.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

struct s_ezlopi_analog_object {
    adc_unit_t unit;
    adc_channel_t adc_channel;
    adc_bits_width_t width;
    adc_atten_t attenuation;
    uint32_t vRef;
    esp_adc_cal_characteristics_t* adc_characteristics;
    __ezlopi_adc_upcall upcall;
};

static void ezlopi_adc_check_eFuse_support();
static esp_adc_cal_characteristics_t* ezlopi_adc_get_adc_characteristics(adc_unit_t unit, adc_atten_t attenuation, adc_bits_width_t width, uint32_t vRef);
static void ezlopi_adc_task(void* args);

static bool is_task_created = false;

// object handle array to check if a channel is already configured.
static ezlopi_analog_object_handle_t ezlopi_analog_object_array[ADC_CHANNEL_MAX] = {NULL};


int ezlopi_adc_init(uint8_t gpio_num, uint8_t width, __ezlopi_adc_upcall upcall)
{
    ezlopi_analog_object_handle_t ezlopi_analog_object_handle = (struct s_ezlopi_analog_object*)malloc(sizeof(struct s_ezlopi_analog_object));
    memset(ezlopi_analog_object_handle, 0, sizeof(struct s_ezlopi_analog_object));
    int ret = 0;
    uint8_t channel = 0;
    #if CONFIG_IDF_TARGET_ESP32
    if(((gpio_num - 36) == 0) || ((gpio_num - 36) == 1) || ((gpio_num - 36) == 2) || ((gpio_num - 36) == 3))
    {
        channel = gpio_num - 36;
    }
    else if(((gpio_num - 28) == 4) || ((gpio_num - 28) == 5) || ((gpio_num - 28) == 6) || ((gpio_num - 28) == 7))
    {
        channel = gpio_num - 28;
    }
    else
    {
        TRACE_E("Invalid GPIO number provided.");
    }
    #elif CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32S2
    channel = gpio_num - 1;
    #endif

    if(ADC_WIDTH_MAX <= width)
    {
        TRACE_E("Invalid width(%d) for ADC; must be less than %d", width, ADC_WIDTH_MAX);
        ret = -1;
    }
     else if(ADC1_CHANNEL_MAX < channel)
    {
        TRACE_E("Invalid channel obtained form gpio_num(%d) for ADC channel; must be less than %d.", gpio_num, ADC1_CHANNEL_MAX + 1);
        ret = -1;
    }
    else if(NULL == upcall)
    {
        TRACE_E("upcall was found null.");
        ret = -1;
    }
    else if(NULL != ezlopi_analog_object_array[channel])
    {
        TRACE_E("Invalid gpio_num(%d) for ADC; it is already in use.", gpio_num);
        ret = -1;
    }
    else 
    {
        ezlopi_analog_object_handle->adc_channel = channel;
        ezlopi_analog_object_handle->unit = ADC_UNIT_1;
        ezlopi_analog_object_handle->attenuation = ADC_ATTEN_DB_11;
        #if CONFIG_IDF_TARGET_ESP32
        ezlopi_analog_object_handle->width = width;
        #elif CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32C3
        ezlopi_analog_object_handle->width = ADC_WIDTH_BIT_12;
        #endif
        ezlopi_analog_object_handle->vRef = 1100;
        ezlopi_analog_object_handle->upcall = upcall;
        ezlopi_analog_object_handle->adc_characteristics = ezlopi_adc_get_adc_characteristics(ezlopi_analog_object_handle->unit, ezlopi_analog_object_handle->attenuation, 
                                                                                                    ezlopi_analog_object_handle->width, ezlopi_analog_object_handle->vRef);
        TRACE_D("Checking for eFuse support.");
        ezlopi_adc_check_eFuse_support();

        TRACE_D("Configuring ADC.");
        adc1_config_width(ezlopi_analog_object_handle->width);
        adc1_config_channel_atten((adc1_channel_t)ezlopi_analog_object_handle->adc_channel, ezlopi_analog_object_handle->attenuation);
        TRACE_D("ADC was configured successfully.");

        ezlopi_analog_object_array[ezlopi_analog_object_handle->adc_channel] = ezlopi_analog_object_handle;

        if(false == is_task_created)
        {
            xTaskCreate(ezlopi_adc_task, "ezlopi_adc_task", 2*2048, NULL, 10, NULL);
            is_task_created = true;
        }
        ret = 0;
    }
    return ret;
}

adc_channel_t ezlopi_adc_get_channel_number(ezlopi_analog_object_handle_t ezlopi_analog_object_handle)
{
    return ezlopi_analog_object_handle->adc_channel;
}

static void ezlopi_adc_task(void* args)
{
    s_ezlopi_analog_data_t *data = (s_ezlopi_analog_data_t*)malloc(sizeof(s_ezlopi_analog_data_t));
    while(1)
    {
        for(uint8_t i = 0; i < ADC_CHANNEL_MAX; i++)
        {
            if(NULL != ezlopi_analog_object_array[i])
            {
                memset(data, 0, sizeof(s_ezlopi_analog_data_t));

                data->value = adc1_get_raw((adc1_channel_t)ezlopi_analog_object_array[i]->adc_channel);
                data->voltage = esp_adc_cal_raw_to_voltage(data->value, ezlopi_analog_object_array[i]->adc_characteristics);
                ezlopi_analog_object_array[i]->upcall(data, ezlopi_analog_object_array[i]->adc_channel);
                vTaskDelay(500 / portTICK_PERIOD_MS);
            }
        }
    }
}

static void ezlopi_adc_check_eFuse_support()
{
    #if CONFIG_IDF_TARGET_ESP32
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
        TRACE_I("eFuse Two Point support available.");
    } else {
        TRACE_E("eFuse Two Point support not available.");
    }
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK) {
        printf("eFuse Vref support available");
    } else {
        printf("eFuse Vref support not available");
    }
    #elif CONFIG_IDF_TARGET_ESP32S3
    if(esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP))
    {
        TRACE_I("eFuse Two Point support available.");
    }
    else 
    {
        TRACE_E("eFuse Two Point support not available.");
    }
    #endif
}

static esp_adc_cal_characteristics_t* ezlopi_adc_get_adc_characteristics(adc_unit_t unit, adc_atten_t attenuation, adc_bits_width_t width, uint32_t vRef)
{
    esp_adc_cal_characteristics_t *chars = (esp_adc_cal_characteristics_t*)malloc(sizeof(esp_adc_cal_characteristics_t));
    memset(chars, 0, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t value = esp_adc_cal_characterize(unit, attenuation, width, vRef, chars);
    if (value == ESP_ADC_CAL_VAL_EFUSE_TP) {
        TRACE_I("Characterized using Two Point Value\n");
    } else if (value == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        TRACE_I("Characterized using eFuse Vref\n");
    } else {
        TRACE_I("Characterized using Default Vref\n");
    }
    return chars;
}


