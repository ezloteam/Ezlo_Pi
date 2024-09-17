file(GLOB_RECURSE config_src "*.c")
# set(config_src)

idf_component_register(SRCS "sensor_0025_digitalIn_LDR.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES    ezlopi-util-trace
                driver
                # ezlopi-core-timer 
                ezlopi-core-cloud 
                ezlopi-core-actions 
                ezlopi-core-devices 
                ezlopi-core-valueformatter 
                ezlopi-core-errors

                ezlopi-cloud-items 
                ezlopi-cloud-constants 
                
                ezlopi-service-gpioisr
                )