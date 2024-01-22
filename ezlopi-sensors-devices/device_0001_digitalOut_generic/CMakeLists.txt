# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c")
# set(config_src)

idf_component_register(SRCS "device_0001_digitalOut_generic.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES 
                trace 
                ezlopi-core-actions
                ezlopi-core-timer 
                ezlopi-core-devices 
                ezlopi-core-cloud 
                ezlopi-core-valueformatter
                ezlopi-service-gpioisr
                ezlopi-cloud-constants
                )
