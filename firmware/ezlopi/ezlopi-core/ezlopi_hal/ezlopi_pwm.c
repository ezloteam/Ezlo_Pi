
#include "ezlopi_pwm.h"
#include "trace.h"
#include "string.h"
#include "stdlib.h"


struct s_ezlopi_pwm_object{
    ledc_timer_config_t* ledc_timer_configuration;
    ledc_channel_config_t* ledc_channel_configuration;
};


#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
static bool available_channels[LEDC_CHANNEL_MAX] = {true, true, true, true, true, true, true};
#elif CONFIG_IDF_TARGET_ESP32C2 || CONFIG_IDF_TARGET_ESP32C3
static bool available_channels[LEDC_CHANNEL_MAX] = {true, true, true, true, true};
#endif

static uint8_t get_available_channel();


s_ezlopi_channel_speed_t* ezlopi_pwm_init(uint8_t pwm_gpio_num, uint8_t pwm_resln, uint32_t freq_hz, uint32_t duty_cycle)
{
    int ret = 0;

    s_ezlopi_channel_speed_t* ezlopi_channel_speed = (s_ezlopi_channel_speed_t*)malloc(sizeof(s_ezlopi_channel_speed_t));
    memset(ezlopi_channel_speed, 0, sizeof(s_ezlopi_channel_speed_t));
    
    uint8_t channel = get_available_channel();
    if(LEDC_CHANNEL_MAX == channel)
    {
        TRACE_E("No channels availalbe for PWM.");
        ret = -1;
    }
    else 
    {
        ledc_timer_config_t ezlopi_pwm_timer_cfg = {
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .duty_resolution = pwm_resln,
            .timer_num = LEDC_TIMER_3,
            .freq_hz = freq_hz,
            .clk_cfg = LEDC_AUTO_CLK,
        };
        ledc_channel_config_t ezlopi_pwm_channel_cfg = {
            .gpio_num = pwm_gpio_num,   
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .channel = channel,
            .intr_type = LEDC_INTR_DISABLE, 
            .timer_sel = LEDC_TIMER_3,
            .duty = duty_cycle,
        };

        
        esp_err_t error = ledc_timer_config(&ezlopi_pwm_timer_cfg);
        if(error != ESP_OK)
        {
            TRACE_E("Error configuring LEDC timer.(code:%s)", esp_err_to_name(error));
        }
        else 
        {
            TRACE_I("LEDC timer configured successfully.");
        }

        error = ledc_channel_config(&ezlopi_pwm_channel_cfg);
        if(error != ESP_OK)
        {
            TRACE_E("Error configuring LEDC channel.(code:%s)", esp_err_to_name(error));
        }
        else 
        {
            TRACE_I("LEDC channel configured successfully.");
        }
        ezlopi_channel_speed->channel = channel;
        ezlopi_channel_speed->speed_mode = ezlopi_pwm_channel_cfg.speed_mode;

    }
    return ezlopi_channel_speed;
}


void ezlopi_pwm_change_duty(uint32_t channel, uint32_t speed_mode, uint32_t duty)
{
    esp_err_t error = ledc_set_duty(speed_mode, channel, duty);
    error  = ledc_update_duty(speed_mode, channel);
    if(error != ESP_OK)
    {
        TRACE_E("Error setting LEDC duty.(code:%s)", esp_err_to_name(error));
    }
    else 
    {
        TRACE_I("LEDC duty set successfully.");
    }
}

uint32_t ezlopi_pwm_get_duty(uint32_t channel, uint32_t speed)
{
    return ledc_get_duty(speed, channel);
}


static uint8_t get_available_channel()
{
    uint8_t channel = 0;
    while(channel != LEDC_CHANNEL_MAX)
    {
        if(available_channels[channel])
        {
            available_channels[channel] = false;
            break;
        }
        channel++;
    }
    return channel;
}
