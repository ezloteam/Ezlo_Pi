
file(GLOB_RECURSE config_src "*.c")
# set(config_src)

idf_component_register(SRCS "device_0022_PWM_dimmable_lamp.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES trace 
             ezlopi-core-actions 
              
             ezlopi-core-timer 
             ezlopi-core-devices 
             ezlopi-cloud-constants 
             ezlopi-service-gpioisr)