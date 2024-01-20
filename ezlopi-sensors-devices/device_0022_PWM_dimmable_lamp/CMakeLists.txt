
file(GLOB_RECURSE config_src "*.c")
# set(config_src)

idf_component_register(SRCS "device_0022_PWM_dimmable_lamp.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES trace 
             ezlopi_actions 
             ezlopi_hal 
             ezlopi_timer 
             ezlopi_devices 
             ezlopi-cloud-constants 
             gpio_isr_service)