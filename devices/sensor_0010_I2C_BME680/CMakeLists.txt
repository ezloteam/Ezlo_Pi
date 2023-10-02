file(GLOB_RECURSE config_src "*.c")
# set(config_src)

idf_component_register(SRCS "bme680_bsec.c" "sensor_0010_I2C_BME680.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES trace        
             gpio_isr_service 
             Bosch_BSEC2 
             ezlopi_actions 
             ezlopi_hal 
             ezlopi_timer 
             ezlopi_devices 
             ezlopi_cloud_constants 
             ezlopi_cloud )
