# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c")
# set(config_src)

idf_component_register(SRCS "device_0001_digitalOut_generic.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES 
                trace
                items
                json
                # data
                # devices
                # info
                # modes
                # network
                # registration
                # room 
                # scenes
                # settings
                gpio_isr_service
                ezlopi_hal
                ezlopi_cloud
                ezlopi_settings
                ezlopi_nvs
                ezlopi_actions  
                ezlopi_timer 
                ezlopi_devices 
                ezlopi_cloud_constants 
                ezlopi_valueformatter)
