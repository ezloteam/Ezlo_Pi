# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c")
# set(config_src)

idf_component_register(SRCS "device_0002_digitalOut_relay.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES 
                trace 
                ezlopi-service-gpioisr
                ezlopi-core-actions
                ezlopi-core-timer 
                ezlopi-core-devices 
                ezlopi-core-cloud 
                ezlopi-cloud-constants
                ezlopi-core-valueformatter)
