# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES    trace 
                ezlopi-sntp
                ezlopi-core-devices 
                ezlopi-core-factory-info 
                ezlopi-core-nvs ezlopi-core-wifi
                ezlopi-core-ethernet
                ezlopi-core-event-queue 
                ezlopi-core-ping 
                ezlopi-core-room )
