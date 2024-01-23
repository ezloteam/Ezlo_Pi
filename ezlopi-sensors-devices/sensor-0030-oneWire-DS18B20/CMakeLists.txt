file(GLOB_RECURSE config_src "*.c")
# set(config_src)

idf_component_register(SRCS "sensor_0030_oneWire_DS18B20.c" "ds18b20_onewire.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES    ezlopi-util-trace  
                driver
                ezlopi-core-timer 
                ezlopi-core-cloud 
                ezlopi-core-actions 
                ezlopi-core-devices 
                ezlopi-core-valueformatter 

                ezlopi-cloud-items
                ezlopi-cloud-constants 
                )