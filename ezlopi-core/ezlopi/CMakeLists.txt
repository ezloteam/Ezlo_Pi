# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "ezlopi.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES    trace 
                core_sntp
                ezlopi_devices 
                ezlopi_factory_info 
                ezlopi_nvs ezlopi_wifi
                ezlopi_ethernet
                ezlopi_event_queue 
                ezlopi_ping 
                ezlopi_room )
