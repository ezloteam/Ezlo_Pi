file(GLOB_RECURSE config_src "*.c")
# set(config_src)

idf_component_register(SRCS "LTR303.c" "ALS_LTR303.c" "sensor_0008_I2C_LTR303ALS.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES 
                items 
                trace 
                gpio_isr_service 
                ezlopi_actions 
                ezlopi_hal 
                ezlopi_timer 
                ezlopi_devices 
                ezlopi_cloud_constants 
                ezlopi_cloud
                ezlopi_valueformatter)