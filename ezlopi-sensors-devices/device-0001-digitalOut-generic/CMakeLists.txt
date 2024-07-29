# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c")
# set(config_src)

idf_component_register(SRCS "device_0001_digitalOut_generic.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES 
                ezlopi-util-trace 
                ezlopi-core-nvs
                # # ezlopi-core-timer 
                ezlopi-core-cloud 
                ezlopi-core-actions
                ezlopi-core-devices 
                ezlopi-core-valueformatter
                ezlopi-core-errors

                ezlopi-hal-gpio
                
                ezlopi-cloud-items 
                ezlopi-cloud-constants 

                ezlopi-service-gpioisr
)
