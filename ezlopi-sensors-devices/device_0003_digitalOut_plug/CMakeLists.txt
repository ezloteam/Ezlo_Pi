# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c")
# set(config_src)

idf_component_register(SRCS "device_0003_digitalOut_plug.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES 
                trace 
                gpio_isr_service
                ezlopi_actions
                ezlopi_timer 
                ezlopi_devices 
                ezlopi_cloud 
                ezlopi-cloud-constants
                ezlopi_valueformatter)
